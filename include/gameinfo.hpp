//GameInfo parsing.
//TODO Get files to copy to tmpdir for cleaning


//What do we actually need?

//There are two cases sdk-2013 base or L4D base.

//Do we even implement older code? (sdk-2007)
//Determine which is which
#include "thirdparty/vdf_parser.hpp"
#include "enum_bitmask.hpp"
#include "gameinfoKV.hpp"
#include "pch.hpp"
#include <unordered_map>
//this is first-in set for file seek
//TODO: |gameinfo_path| and |all_source_engine_paths|

/*
	What we need:
	- maps/(x).bsp
	- scenes/scenes.image
	- scripts/talker/(all)
*/

#define BASEGAME_DIR_TMPL "|all_source_engine_paths|"
#define MODDIR_TMPL "|gameinfo_path|"
namespace FileSystem {
	enum class PathID : unsigned char
	{
		GAME = 1 << 0,
		GAME_WRITE = 1 << 1,
		GAMEBIN = 1 << 2,
		MOD = 1 << 3,
		MOD_WRITE = 1 << 4,
		DEFAULT_WRITE_PATH = 1 << 5,
		PLATFORM = 1 << 6, //Skip this
		DOWNLOAD = 1 << 7, //download dir for multiplayer

	};
	ENABLE_BITMASK_OPERATORS(PathID);
	class CGameInfo {
		//this class represents gameinfo.txt keyvalues file.
	private:
		std::vector<std::pair<PathID, std::string>> searchPaths;
		std::string baseDir;

		std::string modDir;
		gameInfoKV memGI; //this var holds memory representation of gameinfo.txt
		


		
		void initGamepaths();
		void resolveLoadDir();

		void resolveBaseDir();
		bool replace(std::string& str, const std::string& from, const std::string& to);

	public:
		int appID=0;
		CGameInfo() {};
		CGameInfo(std::string modDir) {
			std::ifstream txtGI_str;
			this->modDir = modDir;
			txtGI_str.open(modDir+"/gameinfo.txt");
			memGI = tyti::vdf::read< gameInfoKV>(txtGI_str);
			txtGI_str.close();
			
			//memGI = tyti::vdf::read< tyti::vdf::multikey_object>(txtGI_str);
			txtGI_str.close();
			initGamepaths();
			resolveBaseDir();
			resolveLoadDir();
		};
		~CGameInfo() {
		};
		void prepareTmpDirectory();

		
	private:
		PathID resolvePathIDs(std::string input);
		std::string getPathFromAppID(int appID, std::vector<std::string> steamLibDirs);
	};
}