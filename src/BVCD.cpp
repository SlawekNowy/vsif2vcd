#ifdef ENABLE_TESTING

#define BOOST_TEST_MODULE "testBVCD"

#define BOOST_TEST_MAIN
//vcpkg provides only static libraries
#if 0
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#endif
#include <boost/test/unit_test.hpp>
#endif


#include "BVCD.hpp"
#include "VSIF.hpp"


#include <iomanip>
//#include <indent_facet.hpp>









BVCD::VCD BVCD::getSceneFromBuffer(std::vector<char> buffer) {
        char magic[4] = { buffer[0],buffer[1],buffer[2],buffer[3] };
        uint32_t readMagic = Helper::FourCC(magic);

        using Buffer = std::vector<char>;
        //using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

        if (readMagic == Helper::FourCC("LZMA")) {
            BVCD::CompressedVCD vcdToDecompress;
            bitsery::quickDeserialization<InputAdapter, BVCD::CompressedVCD>(InputAdapter{ buffer.begin(),buffer.end() }, vcdToDecompress);
            //since we use streams we need to copy to one
            /*
            boost::iostreams::filtering_istreambuf in;
            in.push(boost::iostreams::lzma_decompressor());
            in.push(boost::make_iterator_range(vcdToDecompress.compressedBuffer.begin(),vcdToDecompress.compressedBuffer.end()));
            */
            std::vector<char> decompressedBuffer;
            unsigned char* decompressed = new unsigned char[vcdToDecompress.realSize];
            unsigned char* compressed = new unsigned char[vcdToDecompress.compressedSize];
            std::copy(vcdToDecompress.compressedBuffer.begin(), vcdToDecompress.compressedBuffer.end(), compressed);

            size_t destSize=vcdToDecompress.realSize,srcSize=vcdToDecompress.compressedSize;

            LzmaUncompress(decompressed,
                           &destSize,
                           compressed,
                           &srcSize,
                           vcdToDecompress.properties,5);
            decompressedBuffer.assign(decompressed,decompressed+vcdToDecompress.realSize);

            delete[] compressed;
            delete[] decompressed;


    //        boost::iostreams::stream_buffer<boost::iostreams::back_insert_device<std::vector<uint8_t>>> outStream(decompressedBuffer);

            return getSceneFromBuffer(decompressedBuffer);

            }
        assert(BVCD::FourCC("bvcd"));
        BVCD::VCD vcd;
        bitsery::quickDeserialization<InputAdapter, BVCD::VCD>(InputAdapter{ buffer.begin(),buffer.end() }, vcd);


        return vcd;
    };




#ifdef ENABLE_TESTING

//VSIF::ValveScenesImageFile* Helper::vsif;
BOOST_AUTO_TEST_CASE(testBVCD) {
    VSIF::ValveScenesImageFile vsif =VSIF::ValveScenesImageFile("/home/slawomir/Dane/hl2_tmp/scenes/scenes.image");
    Helper::vsif =&vsif;

    for (unsigned int i=0;i<vsif.header.ScenesCount;i++){
    VSIF::VSIF_Entry entry = Helper::vsif->entries[i];
    //[first,last) - we need [first,last] so effectively [first,last+sizeof(char))
    //TODO - can std::next return end iterator?
    uint32_t startPos = entry.Offset-Helper::vsif->dataPos;

    std::vector<char> bvcd(std::next(Helper::vsif->sceneBuffer.begin(),startPos),
                           std::next(Helper::vsif->sceneBuffer.begin(),startPos+entry.Size));


    BVCD::VCD vcdMem = BVCD::getSceneFromBuffer(bvcd);

    std::string vcdText = vcdMem.dumpText();


    std::cout << "Got an object from "<<entry.Size<<" B buffer."<<std::endl; //no reliable way mo measure size of object
    }
};
#endif


//#define ADD_LINE_WITH_INDENTS(x) \
//    stream+=std::string(indentation,'\t')+x;

void BVCD::VCD_Sample::dumpText(std::stringstream& stream)
{
        stream << this->time << " " << this->value << "\n";
}

void BVCD::VCD_Ramp::dumpText(std::stringstream& stream,bool inEvent)
{
     if (inEvent)
         stream << "event_ramp\n";
     else
         stream << "scene_ramp\n";
    if(samples.size()==0) return;
    stream << "{\n";
    for (auto sample=samples.begin();sample!=samples.end();sample++)
        sample->dumpText(stream);
    stream << "}\n";
}

void BVCD::VCD_AbsTags::dumpText(std::stringstream& stream)
{

    return;
}

void BVCD::VCD_CC::dumpText(std::stringstream& stream)
{

}

void BVCD::Flex_Samples::dumpText(std::stringstream& stream)
{

    return;
}

void BVCD::Flex_Tracks::dumpText(std::stringstream& stream)
{
    return;
}

void BVCD::VCD_EventFlex::dumpText(std::stringstream& stream)
{

    if(tracks.size()==0) return;
    stream << "flexanimations samples_use_time\n" << "{\n";
    for (auto track=tracks.begin();track!=tracks.end();track++)
        track->dumpText(stream);
    stream << "}\n";
    return;
}

void BVCD::VCD_RelTags::dumpText(std::stringstream& stream)
{

    return;
}

void BVCD::VCD_RelTag::dumpText(std::stringstream& stream)
{

    return;
}

void BVCD::VCD_FlexTimingTags::dumpText(std::stringstream& stream)
{

    return;
}

void BVCD::VCD_Event::dumpText(std::stringstream& stream)
{
    stream <<"event " << eventTypeToString(eventType) << " \"" <<name<<"\"\n";
    stream << "{\n";
    stream << "time "<<this->eventStart<< " "<<this->eventEnd<<"\n";
    stream << "param \"" << this->param1 << "\"\n";
    if (param2.size()>0)
        stream << "param2 \"" << this->param2 << "\"\n";
    if (param3.size()>0)
        stream << "param3 \"" << this->param3 << "\"\n";

    ramp.dumpText(stream,true);
    if ((bool)(this->flags&VCD_Flags::resumeCondition))
        stream << "resumecondition\n";
    if ((bool)(this->flags&VCD_Flags::lockBodyFacing))
        stream << "lockbodyfacing\n";
    if ((bool)(this->flags&VCD_Flags::fixedLength))
        stream << "fixedlength\n";
    if (!(bool)(this->flags&VCD_Flags::isActive))
        stream << "active \"0\"\n";
    if ((bool)(this->flags&VCD_Flags::playOverScript))
        stream << "playoverscript\n";
    int precision = stream.precision();
    if (distanceToTarget>0)
        stream <<"distancetotarget "<< std::setprecision (2) << std::fixed << distanceToTarget<<"\n";
    stream.unsetf(std::ios_base::floatfield);
    stream<< std::setprecision(precision);

    //Relative Tags
    if (relativeTags.size()!=0)
    {
        stream << "tags\n"<<"{\n";
        for (auto tag=relativeTags.begin();tag!=relativeTags.end();tag++)
            tag->dumpText(stream);
        stream << "}\n";
    }

    //Flex Timing Tags
    if (flexTimingTags.size()!=0)
    {
        stream << "flextimingtags\n"<<"{\n";
        for (auto tag=flexTimingTags.begin();tag!=flexTimingTags.end();tag++)
            tag->dumpText(stream);
        stream << "}\n";
    }

    //Absolute Tags
    std::vector<VCD_AbsTags> shiftedTime;
    std::vector<VCD_AbsTags> playbackTime;
    std::copy_if(absoluteTags.begin(),absoluteTags.end(),
                 std::back_inserter(shiftedTime),[](VCD_AbsTags absTag) {
                    return absTag.type ==VCD_AbsTagType::shifted;
    });
    std::copy_if(absoluteTags.begin(),absoluteTags.end(),
                 std::back_inserter(playbackTime),[](VCD_AbsTags absTag) {
                    return absTag.type ==VCD_AbsTagType::playback;
    });
    if (shiftedTime.size()!=0)
    {
        stream << "flextimingtags\n"<<"{\n";
        for (auto tag=shiftedTime.begin();tag!=shiftedTime.end();tag++)
            tag->dumpText(stream);
        stream << "}\n";
    }
    if (playbackTime.size()!=0)
    {
        stream << "flextimingtags\n"<<"{\n";
        for (auto tag=playbackTime.begin();tag!=playbackTime.end();tag++)
            tag->dumpText(stream);
        stream << "}\n";
    }


    //Sequence duration
    if (eventType==Event_Type::Event_Gesture)
        stream <<"sequenceduration "<< sequenceDuration <<"\n";

    //relative tag
    relativeTag.dumpText(stream);
    //flexAnimation
    flex.dumpText(stream);
    if (eventType==Event_Type::Event_Loop)
        stream << "loopcount \""<<loopCount<<"\"\n";
    //CCs
    if (eventType==Event_Type::Event_Speak)
    {
        closeCaptions.dumpText(stream);
    }

    stream << "}\n";

}

void BVCD::VCD_Channel::dumpText(std::stringstream& stream)
{
    stream << "channel \""<<this->name<<"\"\n";
    stream << "{\n";
    for (auto event = events.begin();event!=events.end();event++)
        event->dumpText(stream);
    if (!isActive)
        stream << "active \"0\"\n";


    stream << "}\n";
}

void BVCD::VCD_Actor::dumpText(std::stringstream& stream)
{
    stream << "actor \"" << this->name <<"\"\n";
    stream << "{\n";
    for (auto channel = channels.begin();channel !=channels.end();channel++) {
        channel->dumpText(stream);
    }
    if (!isActive)
        stream << "active \"0\"\n";
    stream << "}\n";

}

std::string BVCD::VCD::dumpText()
{
    //indentStringStream streamOut;

    std::stringstream stream;

    //int indentation=0;

    //streamOut << incrementIndent;
    std::ios_base::sync_with_stdio(false);
    stream << "//Choreo version 1\n";

    for(auto element=events.begin();element!=events.end();element++) {
        element->dumpText(stream);
    }
    for(auto element=actors.begin();element!=actors.end();element++) {
        element->dumpText(stream);
    }
    ramp.dumpText(stream,false);

    stream << "\"scalesettings\"\n";
    //indentation++;
    stream << "{\n" ;//<<indent_manip::push;

    stream << "\"CChoreoView\" \"100\"\n" ;
    stream << "\"SceneRampTool\" \"100\"\n" ;
    stream << "\"ExpressionTool\" \"100\"\n" ;
    stream << "\"GestureTool\" \"100\"\n" ;
    stream << "\"RampTool\" \"100\"\n" ;//<<indent_manip::pop;
//indentation--;
    stream <<"}\n" ;
    stream <<"fps 60\n";
    stream <<"snap off\n";




    return stream.str();
}

//#undef ADD_LINE_WITH_INDENTS


