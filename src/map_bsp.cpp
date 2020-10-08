#ifdef ENABLE_TESTING
#define BOOST_TEST_MODULE "testMap"

#define BOOST_TEST_MAIN
#if 0
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#endif
#include <boost/test/unit_test.hpp>
#endif




#include "program.h"


int BSPParser::ExtractNames(std::string GameDirectory)
{
	return 0;
}








#ifdef ENABLE_TESTING

std::vector<BSPParser::Map_Scene> Program::scenesPerMap;
std::map<std::string,std::vector<BSPParser::Map_Scene>> BSPParser::Scenes;
BOOST_AUTO_TEST_CASE(testVSIF) {

	//VSIF::ValveScenesImageFile vsif("E:/hl2_tmp/scenes/scenes.image");
    BSPParser::ExtractNames("E:/hl2_tmp");

}
#endif
