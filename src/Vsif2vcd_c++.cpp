// Vsif2vcd_c++.cpp: definiuje punkt wejścia dla aplikacji.
//
#include "program.h"


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




std::vector<BSPParser::Map> Program::scenesPerMap;

CGameInfo Program::gi;

VSIF::ValveScenesImageFile Program::vsif;
using boost::filesystem::path;
using boost::filesystem::canonical;
int Program::doStart(std::string gameDir) {
	path gameDirPath(gameDir);
	auto absGameDir = canonical(gameDirPath);
	BSPParser::ExtractNames(gameDir);
#if 0
	RRParser::getSceneNames( gameDir);
	
#endif
	appendHardCodedEntries();
	//TODO: Response rules parsing.
	//Also static names of scenes added by source code itself
	gi = CGameInfo::CGameInfo(gameDir);
	vsif = VSIF::ValveScenesImageFile(gameDir + "/scenes/scenes.image");
	return 0;


}

void Program::appendHardCodedEntries()
{

	for (auto iter = hardcodedEntries.begin(); iter != hardcodedEntries.end(); ++iter) {

	}
}

