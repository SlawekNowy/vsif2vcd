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



#if defined(ENABLE_TESTING) && defined(TESTING_VSIF)
BOOST_AUTO_TEST_CASE(testVSIF) {


	VSIF::ValveScenesImageFile vsif("E:/hl2_tmp/scenes/scenes.image");
	
}
#endif


