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
#include "response_system.h"
#include <valve-bsp-parser/bsp_parser.hpp>
#include <set>


int BSPParser::ExtractNames(std::string GameDirectory)
{
    std::ifstream maplistTxt(GameDirectory+"/maplist.txt"); //that might be insufficient...
    std::vector<std::string> maplist;
    std::string line;
    while (safeGetline(maplistTxt,line)) {
        maplist.emplace_back(line);
    }

    //TODO: check if we dont miss anything...

    for (auto iter = maplist.begin();iter!=maplist.end();++iter) {
        //load map.
        using namespace rn;
        bsp_parser map;
        map.load_map(GameDirectory+"/maps/",*iter.base() + ".bsp");

        //Map is fully loaded. Check for logic_choreographed_scene

        std::vector<Map_Scene> scenes;

        for (auto entity = map.entities.begin();entity!=map.entities.end();++entity) {
            if (entity.base()->keyvalues["classname"] =="logic_choreographed_scene"){
                std::string sceneName = entity.base()->keyvalues["SceneFile"];
                Map_Scene scene = *new Map_Scene(sceneName.c_str());
                scenes.emplace_back(scene);
            }
            if (entity.base()->keyvalues["classname"] == "env_speaker") {//HL2: env_speaker uses its own script. Parse them separately if possible.
                //TODO: append value of rulescript to scripts to parse.
                    RRParser::entryPointsToParse.push_back(entity.base()->keyvalues["rulescript"]);


            }

        }
        //Scenes.insert(Scenes.end(),scenes.begin(),scenes.end());
        Scenes.emplace(*iter.base(),scenes);
    }

    std::sort(RRParser::entryPointsToParse.begin(),RRParser::entryPointsToParse.end());
    RRParser::entryPointsToParse.erase(std::unique(RRParser::entryPointsToParse.begin(),RRParser::entryPointsToParse.end()),RRParser::entryPointsToParse.end());
//move globalscript to front
    auto globalScript = std::find_if(RRParser::entryPointsToParse.begin(),RRParser::entryPointsToParse.end(),
                                     [](std::string& script) -> bool {
                        return script == "scripts/talker/response_rules.txt";
    });
    std::rotate(RRParser::entryPointsToParse.begin(),globalScript,globalScript+1);
    RRParser::entryPointsToParse.shrink_to_fit();
	return 0;
}










#ifdef ENABLE_TESTING

std::map<std::string,std::vector<BSPParser::Map_Scene>> BSPParser::Scenes;
BOOST_AUTO_TEST_CASE(testVSIF) {

	//VSIF::ValveScenesImageFile vsif("E:/hl2_tmp/scenes/scenes.image");
    BSPParser::ExtractNames("/home/slawomir/Dane/hl2_tmp");
    RRParser::initRules("/home/slawomir/Dane/hl2_tmp");

}
#endif
