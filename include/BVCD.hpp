#pragma once

#include "pch.hpp"
#include "helper.hpp"
#include <typeinfo>
#include "enum_bitmask.hpp"

#include "VSIF.hpp"
#include <LzmaLib.h>

VSIF::ValveScenesImageFile* Helper::vsif;


namespace BVCD {
    using namespace Helper;
    static  constexpr float One255th = 0.003922f;
    static  constexpr float One4096th = 0.000244f;
    enum Event_Type{
		Event_Unspecified,
		Event_Section, Event_Expression, Event_LookAt, Event_MoveTo,
		Event_Speak, Event_Gesture, Event_Sequence, Event_Face,
		Event_FireTrigger, Event_FlexAnimation, Event_SubScene, Event_Loop,
        Event_Interrupt, Event_StopPoint, Event_PermitResponses, Event_Generic,
        Event_Invalid =-1
    } ;
    static  const char Event_TypeNames[17][16] = {
		"unspecified",
		"section", "expression", "lookat", "moveto",
		"speak", "gesture", "sequence", "face",
		"firetrigger", "flexanimation", "subscene", "loop",
		"interrupt", "stoppoint", "permitresponses", "generic"
	};
    static  char Interpolators[16][23] = {
        "default", "catmullrom_normalize_x", "easein", "easeout",
        "easeinout", "bspline", "linear_interp", "kochanek",
        "kochanek_early", "kochanek_late", "simple_cubic", "catmullrom",
        "catmullrom_normalize", "catmullrom_tangent", "exponential_decay", "hold"
    };
    static inline std::string eventTypeToString(Event_Type event){
        return Event_TypeNames[event];
    };

    static inline Event_Type eventTypeFromString(std::string event) {
        for (int i=0;i<17;i++) {
            if (event == Event_TypeNames[i]){
                return (Event_Type)i;
            }
        }
        return Event_Invalid;
    }

    //

    enum class VCD_Flags : unsigned char
    {
        resumeCondition=1<<0,
        lockBodyFacing=1<<1,
        fixedLength=1<<2,
        isActive=1<<3,
        forceShortMovement=1<<4,
        playOverScript=1<<5
    };
    enum class TrackFlags : unsigned char
    {
        isEnabled=1<<0,
        isCombo=1<<1,
    };
    enum class VCD_CC_Flags: unsigned char
    {
        usingCombinedFile=1<<0,
        combinedUsingGenderToken=1<<1,
        suppressingCaptionAttenuation=1<<2,
    };
    enum class VCD_AbsTagType : unsigned char
    {
        playback=0,
        shifted=1
    };
    enum class VCD_CC_Type: unsigned char
    {
        master=0,
        slave=1,
        disabled=2,
    };

}
ENABLE_BITMASK_OPERATORS(BVCD::VCD_Flags);
ENABLE_BITMASK_OPERATORS(BVCD::VCD_CC_Flags);
ENABLE_BITMASK_OPERATORS(BVCD::TrackFlags);

namespace BVCD {

struct VCD_Sample {
    float time;
    float value;
};
template <typename S>
void serialize(S& s, VCD_Sample& sample) {
    uint16_t shortTmp;
    s.value4b(sample.time);

    s.value2b(shortTmp);
    sample.value=shortTmp*One255th;

}
struct VCD_Ramp { // CCurveData
    //ramp name is event in VCD_Event and scene in VCD
    std::vector<VCD_Sample> samples;

};
template <typename S>
void serialize(S& s, VCD_Ramp& ramp) {
    uint8_t byteTmp;
    s.value1b(byteTmp);
    ramp.samples.resize(byteTmp);
    for (int i=0;i<byteTmp;i++)
        s.object(ramp.samples[i]);

}
struct VCD_CC {
    VCD_CC_Type type;
    std::string cc_token;
    VCD_CC_Flags flags;
};
template <typename S>
void serialize(S& s, VCD_CC& subtitles) {
    uint8_t byteTmp;

    s.value1b(byteTmp);
    subtitles.type=(VCD_CC_Type)byteTmp;
    uint16_t shortTmp;
    s.value2b(shortTmp);
    subtitles.cc_token=Helper::vsif->stringPool.getStringByID(shortTmp);
    s.value1b(byteTmp);
    subtitles.flags=(VCD_CC_Flags)byteTmp;
}
struct Flex_Samples {
    float time;
    float value;
    std::string fromCurve; //left
    std::string toCurve; //right
};
template <typename S>
void serialize(S& s, Flex_Samples& sample) {
    uint16_t shortTmp;
    s.value4b(sample.time);

    s.value2b(shortTmp);
    sample.value=shortTmp*One255th;
    uint8_t charTmp;
    s.value1b(charTmp);
    sample.toCurve = Interpolators[charTmp];
    s.value1b(charTmp);
    sample.fromCurve = Interpolators[charTmp];
}
struct Flex_Tracks {
    std::string name;
    TrackFlags flags;
    float minRange,maxRange;//(0.0,1.0)
    std::vector<Flex_Samples> samples;
    std::vector<Flex_Samples> comboSamples;
};
template <typename S>
void serialize(S& s, Flex_Tracks& track) {
    uint16_t shortTmp;
    s.value2b(shortTmp);
    track.name=Helper::vsif->stringPool.getStringByID(shortTmp);
    uint8_t byteTmp;
    s.value1b(byteTmp);
    track.flags=(TrackFlags)byteTmp;
    s.value4b(track.minRange);
    s.value4b(track.minRange);
    assert(track.minRange>=0&&track.maxRange<=1);
    s.value2b(shortTmp);
    track.samples.resize(shortTmp);
    for (int i=0;i<shortTmp;i++)
        s.object(track.samples[i]);
    track.comboSamples.resize(shortTmp);
    for (int i=0;i<shortTmp;i++)
        s.object(track.comboSamples[i]);


}
struct VCD_EventFlex {
    std::vector<Flex_Tracks> tracks;
};
template <typename S>
void serialize(S& s, VCD_EventFlex& flex) {
    uint8_t byteTmp;
    s.value1b(byteTmp);
    flex.tracks.resize(byteTmp);
    for (int i=0;i<byteTmp;i++)
        s.object(flex.tracks[i]);
}
struct VCD_RelTags {
    std::string name;
    float duration;
};
template <typename S>
void serialize(S& s, VCD_RelTags& tags) {

    uint16_t shortTmp;
    s.value2b(shortTmp);
    tags.name=Helper::vsif->stringPool.getStringByID(shortTmp);

    s.value2b(shortTmp);
    tags.duration=shortTmp*One255th;
}
struct VCD_RelTag {
    std::string name;
    std::string wavName;
};
template <typename S>
void serialize(S& s, VCD_RelTag& tag) {
    uint16_t shortTmp;
    s.value2b(shortTmp);
    tag.name=Helper::vsif->stringPool.getStringByID(shortTmp);
    s.value2b(shortTmp);
    tag.wavName=Helper::vsif->stringPool.getStringByID(shortTmp);

}
struct VCD_FlexTimingTags {

    std::string name;
    float duration;
};
template <typename S>
void serialize(S& s, VCD_FlexTimingTags& tags) {
    uint16_t shortTmp;
    s.value2b(shortTmp);
    tags.name=Helper::vsif->stringPool.getStringByID(shortTmp);
    s.value2b(shortTmp);
    tags.duration=shortTmp*One255th;
}
struct VCD_AbsTags {
    VCD_AbsTagType type;
    std::string name;
    float duration;

};
template <typename S>
void serialize(S& s, VCD_AbsTags& tags) {

    uint8_t byteTmp;
    s.value1b(byteTmp);
    tags.type=(VCD_AbsTagType)byteTmp;
    uint16_t shortTmp;
    s.value2b(shortTmp);
    tags.name=Helper::vsif->stringPool.getStringByID(shortTmp);
    s.value2b(shortTmp);
    tags.duration=shortTmp*One4096th;
}

    struct VCD_Event {
        Event_Type eventType;
        std::string name;
        float eventStart,eventEnd;
        std::string param1,param2,param3;
        VCD_Ramp ramp;
        VCD_Flags flags;
        float distanceToTarget;
        std::vector<VCD_RelTags> relativeTags;
        std::vector<VCD_FlexTimingTags> flexTimingTags;
        std::vector<VCD_AbsTags> absoluteTags;
        float sequenceDuration =-1.0f; //not present outside of gesture event
        bool usingRelativetag;
        VCD_RelTag relativeTag;
        VCD_EventFlex flex;
        uint8_t loopCount;
        VCD_CC closeCaptions;




    };

    template <typename S>
    void serialize(S& s, VCD_Event& e) {
    uint8_t flagTmp;

    uint16_t stringIndexTmp;
    s.value1b(flagTmp);
    e.eventType=(Event_Type)flagTmp;
    s.value2b(stringIndexTmp);
    e.name=Helper::vsif->stringPool.getStringByID(stringIndexTmp);
    s.value4b(e.eventStart);
    s.value4b(e.eventEnd);
    s.value2b(stringIndexTmp);
    e.param1=Helper::vsif->stringPool.getStringByID(stringIndexTmp);
    s.value2b(stringIndexTmp);
    e.param2=Helper::vsif->stringPool.getStringByID(stringIndexTmp);
    s.value2b(stringIndexTmp);
    e.param3=Helper::vsif->stringPool.getStringByID(stringIndexTmp);
    s.object(e.ramp);
    s.value1b(flagTmp);
    e.flags = (VCD_Flags)flagTmp;
    s.value4b(e.distanceToTarget);
    uint8_t tagCount;
    s.value1b(tagCount);
    e.relativeTags.resize(tagCount);
    for (int i=0;i<tagCount;i++)
        s.object(e.relativeTags[i]);
    s.value1b(tagCount);
    e.flexTimingTags.resize(tagCount);
    for (int i=0;i<tagCount;i++)
        s.object(e.flexTimingTags[i]);
    s.value1b(tagCount);
    e.absoluteTags.resize(tagCount);
    for (int i=0;i<tagCount;i++)
        s.object(e.absoluteTags[i]);
    uint8_t tag = tagCount;
    s.value1b(tagCount);
    e.absoluteTags.reserve(tagCount);
    for (int i=0;i<tagCount;i++)
        s.object(e.absoluteTags[i+tag]);
    if(Event_Type::Event_Gesture==e.eventType) {
        s.value4b(e.sequenceDuration);
    }
    s.value1b(flagTmp);
    e.usingRelativetag = flagTmp?true:false;
    if (e.usingRelativetag)
        s.object(e.relativeTag);
    s.value1b(e.loopCount);
    if(Event_Type::Event_Speak==e.eventType) {
    s.object(e.closeCaptions);

    }


}


    struct VCD_Channel {
        std::string name;
        std::vector<VCD_Event> events;
        bool isActive;
    };
    template <typename S>
    void serialize(S& s, VCD_Channel& c) {

        uint16_t stringIndex;
        s.value2b(stringIndex);
        c.name = Helper::vsif->stringPool.getStringByID(stringIndex);
        uint8_t eventCount;
        s.value1b(eventCount);
        c.events.resize(eventCount);
        for (int i=0;i<eventCount;i++)
            s.object(c.events[i]);
        uint8_t isActive;

        s.value1b(isActive);
        c.isActive = (isActive)?true:false;
};
    struct VCD_Actor {
        std::string name;
        std::vector<VCD_Channel> channels;
        bool isActive;

    };
    template <typename S>
    void serialize(S& s, VCD_Actor& a) {
        uint16_t stringIndex;
        s.value2b(stringIndex);
        a.name = Helper::vsif->stringPool.getStringByID(stringIndex);
        uint8_t channelCount;
        s.value1b(channelCount);
        a.channels.resize(channelCount);
        for (int i=0;i<channelCount;i++)
            s.object(a.channels[i]);
        uint8_t isActive;

        s.value1b(isActive);
        a.isActive = (isActive)?true:false;

}


    class VCD {
    public:
        uint32_t magic; // bvcd
        uint8_t version;
        uint32_t CRC;
        std::vector<VCD_Event> events;
        std::vector<VCD_Actor> actors;
        VCD_Ramp ramp;
        //bool ignorePhonemes;

    };

    template <typename S>
    void serialize(S& s, VCD& vcd) {
        //std::string test = Helper::vsif->stringPool.getStringByID(5);
        s.value4b(vcd.magic);
        s.value1b(vcd.version);
        s.value4b(vcd.CRC);
        uint8_t eventsCount;
        s.value1b(eventsCount);
        vcd.events.resize(eventsCount);
        for (int i=0;i<eventsCount;i++)
            s.object(vcd.events[i]);
        uint8_t actorsCount;
        s.value1b(actorsCount);

        vcd.actors.resize(actorsCount);
        for (int i=0;i<actorsCount;i++)
            s.object(vcd.actors[i]);
        s.object(vcd.ramp);


        /*
        uint8_t tmpIgnorePhonemes;
        s.value1b(tmpIgnorePhonemes);
        vcd.ignorePhonemes = (tmpIgnorePhonemes)?true:false;
        */
}


    class CompressedVCD {
    public:
        uint32_t magic; //LZMA
        uint32_t realSize;
        uint32_t compressedSize;
        char properties[5];
        std::vector<char> compressedBuffer;
    };

    //template <typename S>
    template <typename S>
    void serialize(S& s, CompressedVCD& cVCD) {
        s.value4b(cVCD.magic);
        s.value4b(cVCD.realSize);
        s.value4b(cVCD.compressedSize);
        s.container(cVCD.properties);

        cVCD.compressedBuffer.resize(cVCD.compressedSize);
        char* tmp = new char[cVCD.compressedSize];
        //HACK: There MUST be faster way.
        s.adapter().template readBuffer<1,char>(tmp,(size_t)cVCD.compressedSize);
        //assert();
        cVCD.compressedBuffer = std::vector<char>(tmp, tmp + cVCD.compressedSize + 1);
        delete[] tmp;

    };


struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }

    template <typename Iterator=std::vector<char>::iterator>
    membuf(Iterator begin, Iterator end) {
        this->setg(begin.base(),begin.base(),end.base());
    }
};


    inline VCD getSceneFromBuffer(std::vector<char> buffer) {
        uint32_t readMagic = FourCC(new char[4]{buffer[0],buffer[1],buffer[2],buffer[3]});

        using Buffer = std::vector<char>;
        using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

        if (readMagic == FourCC("LZMA")) {
            CompressedVCD vcdToDecompress;
            bitsery::quickDeserialization<InputAdapter, CompressedVCD>(InputAdapter{ buffer.begin(),buffer.end() }, vcdToDecompress);
            //since we use streams we need to copy to one
            /*
            boost::iostreams::filtering_istreambuf in;
            in.push(boost::iostreams::lzma_decompressor());
            in.push(boost::make_iterator_range(vcdToDecompress.compressedBuffer.begin(),vcdToDecompress.compressedBuffer.end()));
            */
            std::vector<char> decompressedBuffer;
            char* decompressed = new char[vcdToDecompress.realSize];

            LzmaUncompress((unsigned char*)decompressed,
                           (size_t*)vcdToDecompress.realSize,
                           (unsigned char*)vcdToDecompress.compressedBuffer.data(),
                           (size_t*)vcdToDecompress.compressedSize,
                           (unsigned char*)&vcdToDecompress.properties,5);
            decompressedBuffer.assign(decompressed,decompressed+vcdToDecompress.realSize+1);


    //        boost::iostreams::stream_buffer<boost::iostreams::back_insert_device<std::vector<uint8_t>>> outStream(decompressedBuffer);

            return getSceneFromBuffer(decompressedBuffer);

            }
        assert(FourCC("bvcd"));
        VCD vcd;
        bitsery::quickDeserialization<InputAdapter, VCD>(InputAdapter{ buffer.begin(),buffer.end() }, vcd);


        return vcd;
    };










}







