// Vsif2vcd_c++.cpp: definiuje punkt wejścia dla aplikacji.
//
#include "program.h"

#include "VSIF.hpp"
#include "response_system.h"

#include "hardcoded_entries.h"


using namespace std;



int main(int argc, char* argv[]){
	int ExitCode;
	puts("VSIF2VCD version 2.0 (23 Apr 2012)");
	if (argc < 2|| argc > 2)
	{
		cerr << "By http://steamcommunity.com/id/SiPlus\nUsage: VSIF2VCD [game directory]\n";
		return 1;
	}
	//puts("Initializing LZMA uncompression module");
	ExitCode = Program::doStart(argv[1]);
	return ExitCode;
}




std::vector<BSPParser::Map_Scene> Program::scenesPerMap;

CGameInfo Program::gi;
std::map<std::string,std::vector<BSPParser::Map_Scene>> BSPParser::Scenes;

//VSIF::ValveScenesImageFile* Helper::vsif;
using boost::filesystem::path;
using boost::filesystem::canonical;
int Program::doStart(std::string gameDir) {
	path gameDirPath(gameDir);
	auto absGameDir = canonical(gameDirPath);

	//TODO: Response rules parsing.
	//Also static names of scenes added by source code itself
    gi = CGameInfo(gameDir);


    //BSPParser and RRParser should be easy to run at the same time
    BSPParser::ExtractNames(gameDir);
//#if 0
    RRParser::initRules(gameDir);
    RRParser::dumpSceneNames();

    //TODO: Wait for the threads

//#endif
    appendHardCodedEntries();
    VSIF::ValveScenesImageFile vsif = VSIF::ValveScenesImageFile(gameDir + "/scenes/scenes.image");
    Helper::vsif = &vsif;
	return 0;


}

void Program::appendHardCodedEntries()
{
    std::vector<BSPParser::Map_Scene> hardcodedScenes;
    hardcodedScenes.reserve(hardcodedEntries.size());
	for (auto iter = hardcodedEntries.begin(); iter != hardcodedEntries.end(); ++iter) {
        hardcodedScenes.emplace_back((char*)iter->c_str());
    }
    BSPParser::Scenes.emplace("hardcoded",hardcodedScenes);
}

