#if defined(ENABLE_TESTING) && defined(TESTING_VSIF)

#define BOOST_TEST_MODULE "testVSIF"

#define BOOST_TEST_MAIN
//vcpkg provides only static libraries
#if 0
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#endif
#include <boost/test/unit_test.hpp>
#endif


#include "VSIF.hpp"
#include "BVCD.hpp"




#if defined(ENABLE_TESTING) && defined(TESTING_VSIF)
BOOST_AUTO_TEST_CASE(testVSIF) {


	VSIF::ValveScenesImageFile vsif("/home/slawomir/Dane/hl2_tmp/scenes/scenes.image");
	
}
#endif


#if !defined(ENABLE_TESTING)
void VSIF::ValveScenesImageFile::fillWithVCDS()
{

    vcds.resize(header.ScenesCount);
    for (unsigned int i=0;i<Helper::vsif->header.ScenesCount;i++){
        VSIF::VSIF_Entry entry = Helper::vsif->entries[i];
        //[first,last) - we need [first,last] so effectively [first,last+sizeof(char))
        //TODO - can std::next return end iterator?
        uint32_t startPos = entry.Offset-Helper::vsif->dataPos;

        std::vector<char> bvcd(std::next(Helper::vsif->sceneBuffer.begin(),startPos),
                           std::next(Helper::vsif->sceneBuffer.begin(),startPos+entry.Size));


        BVCD::VCD vcdMem = BVCD::getSceneFromBuffer(bvcd);
        BVCD::VCD* vcdMemPtr = &vcdMem;
        vcds.push_back(vcdMemPtr);

    }
}
#endif
