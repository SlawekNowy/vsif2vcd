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



#ifdef ENABLE_TESTING

//VSIF::ValveScenesImageFile* Helper::vsif;
BOOST_AUTO_TEST_CASE(testBVCD) {
    VSIF::ValveScenesImageFile vsif =VSIF::ValveScenesImageFile("/home/slawomir/Dane/hl2_tmp/scenes/scenes.image");
    Helper::vsif =&vsif;

    VSIF::VSIF_Entry entry = Helper::vsif->entries[0];
    //[first,last) - we need [first,last] so effectively [first,last+sizeof(char))
    //TODO - can std::next return end iterator?
    uint32_t startPos = entry.Offset-Helper::vsif->dataPos;

    std::vector<char> bvcd(std::next(Helper::vsif->sceneBuffer.begin(),startPos),
                           std::next(Helper::vsif->sceneBuffer.begin(),startPos+entry.Size));


    BVCD::VCD vcdMem = BVCD::getSceneFromBuffer(bvcd);


    std::cout << "Got "<< sizeof(vcdMem)<< " B object from "<<entry.Size<<" B buffer.";
}
#endif
