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
BOOST_AUTO_TEST_CASE(testBVCD) {
    VSIF::ValveScenesImageFile vsif("E:/hl2_tmp/scenes/scenes.image");

    VSIF::VSIF_Entry entry = vsif.entries[0];
    //[first,last) - we need [first,last] so effectively [first,last+sizeof(char))
    //TODO - can std::next return end iterator?
    uint32_t startPos = entry.Offset-vsif.dataPos;

    std::vector<char> bvcd(std::next(vsif.sceneBuffer.begin(),startPos),
                           std::next(vsif.sceneBuffer.begin(),startPos+entry.Size+1));


    BVCD::VCD vcdMem = BVCD::getSceneFromBuffer(bvcd);
}
#endif
