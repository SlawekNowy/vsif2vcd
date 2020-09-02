#include "pch.hpp"
#include "helper.hpp"
#include <typeinfo>


#include <LzmaLib.h>
#pragma once
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

    struct VCD_Event {

    };

    template <typename S>
    void serialize(S& s, VCD_Event& e) {
}


    struct VCD_Actor {

    };
    template <typename S>
    void serialize(S& s, VCD_Actor& a) {
}

    struct VCD_Channel {

    };
    template <typename S>
    void serialize(S& s, VCD_Channel& c) {
}

    class VCD {
    public:
        uint32_t magic; // bvcd
        uint8_t version;
        uint32_t CRC;
        std::vector<VCD_Event> events;
        std::vector<VCD_Actor> actors;
        uint8_t ignorePhonemes;

    };

    template <typename S>
    void serialize(S& s, VCD& vcd) {

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


        return VCD();
    };










}





