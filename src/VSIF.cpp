#ifdef ENABLE_TESTING

#define BOOST_TEST_MODULE "testGameInfo"

#define BOOST_TEST_MAIN
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#endif
#include "VSIF.hpp"



#ifdef ENABLE_TESTING
BOOST_AUTO_TEST_CASE(testVSIF) {

	VSIF::ValveScenesImageFile vsif("E:/hl2_tmp/scenes/scenes.image");
	
}
#endif