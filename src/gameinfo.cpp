#ifdef ENABLE_TESTING

#define BOOST_TEST_MODULE "testGameInfo"

#define BOOST_TEST_MAIN
#if 0
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#endif
#include <boost/test/unit_test.hpp>
#endif


#include <typeinfo>
#include "gameinfo.hpp"
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <vector>

#include <iterator>


#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <boost/algorithm/string/predicate.hpp>




#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#elif __linux__
#include <cstdlib>
#elif __APPLE__ && __MACH__
// TODO
#endif

#ifdef _WIN32
LONG GetDWORDRegKey(HKEY hKey, const std::wstring& strValueName, DWORD& nValue, DWORD nDefaultValue)
{
	nValue = nDefaultValue;
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey,
		strValueName.c_str(),
		0,
		NULL,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		nValue = nResult;
	}
	return nError;
}


LONG GetBoolRegKey(HKEY hKey, const std::wstring& strValueName, bool& bValue, bool bDefaultValue)
{
	DWORD nDefValue((bDefaultValue) ? 1 : 0);
	DWORD nResult(nDefValue);
	LONG nError = GetDWORDRegKey(hKey, strValueName.c_str(), nResult, nDefValue);
	if (ERROR_SUCCESS == nError)
	{
		bValue = (nResult != 0) ? true : false;
	}
	return nError;
}


LONG GetStringRegKey(HKEY hKey, const std::wstring& strValueName, std::wstring& strValue, const std::wstring& strDefaultValue)
{
	strValue = strDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		strValue = szBuffer;
	}
	return nError;
}
#endif

template <typename K,typename V>
struct CompareFirst
{
	CompareFirst(K val) : val_(val) {}
	bool operator()(const std::pair<K, V>& elem) const {
		return val_ == elem.first;
	}
private:
	K val_;
};

namespace FileSystem {
  std::vector<std::uint32_t> implicitLoadGameInfoID{
    //L4D branch
    500, //Left 4 Dead
    550, //Left 4 Dead 2
    201070, //Revelations 2012
    17710, //Nuclear Dawn

    //Alien Swarm branch

    630, //Alien Swarn
    563560, //Alien Swarm: Reactive Drop

    //Portal 2 branch

    620, //Portal 2
    280740, //Aperture Tag
    317400, //Portal Stories: Mel
    1255980, //Portal Reloaded
    70000, //Dino D-Day
    251110, //INFRA
    221910, //The Stanley Parable (why do you want to do this?)
    303210, //The Beginner's Guide

    //CS:GO branch
    730 //CS:GO
    //869480, //
    //Insurgengy, Day of Infamy and Jabroni Brawl: Episode 3 (dat trailer tho) all use SDK 2013 style gameinfos.
    //Note however the inverse order of operations on the last one (loose files, vpks in opposition to vpks, loose files)
  };
}
void FileSystem::CGameInfo::getSteamAppID()
{

  auto fsNode = std::find_if(memGI.childs.begin(),memGI.childs.end(),CompareFirst<std::string, std::shared_ptr<gameInfoKV>>("FileSystem"))->second;
  if (std::find_if(fsNode->attribs.begin(), fsNode->attribs.end(), CompareFirst<std::string, std::string>("SteamAppId"))->second !="") {
          appID = boost::lexical_cast<int>(std::find_if(fsNode->attribs.begin(), fsNode->attribs.end(), CompareFirst<std::string, std::string>("SteamAppId"))->second);
  }
}
void FileSystem::CGameInfo::loadPAKs(std::string atPath)
{
    for (int i=1;i<=99;i++)
      {
        std::string relVPKFilePath = atPath+ "/" +fmt::format("pak{0:02}_dir.vpk",i);

        namespace fileSys=std::filesystem;
        using fileSys::path;

        bool check= false;
        if(path(atPath).is_absolute())
          {
            check = fileSys::exists(path(relVPKFilePath))&&fileSys::is_regular_file(path(relVPKFilePath));
          }
        else
          {
            check = fileSys::exists(path(baseDir+"/"+relVPKFilePath))&&fileSys::is_regular_file(path(baseDir +"/"+relVPKFilePath));
          }
        if (check)
          {
            searchPaths.emplace_back(PathID::GAME,relVPKFilePath);
          }
            else
          break;
      }
}
void FileSystem::CGameInfo::initGamepaths()
{
	assert(memGI.name == "GameInfo"); 
	bool isMultiPlayer =true;
	//check if the key exists and it's singleplayer
	if (std::find_if(memGI.attribs.begin(), memGI.attribs.end(),CompareFirst<std::string,std::string>("type")) != memGI.attribs.end()) {
		auto gameType = std::find_if(memGI.attribs.begin(), memGI.attribs.end(), CompareFirst<std::string, std::string>("type"))->second;
		if (gameType == "singleplayer_only") {
			isMultiPlayer = false;
		}
	}

	auto fsNode = std::find_if(memGI.childs.begin(),memGI.childs.end(),CompareFirst<std::string, std::shared_ptr<gameInfoKV>>("FileSystem"))->second;
	//We have to use iterators for this one
	auto searchPaths_raw = *(std::find_if(fsNode->childs.begin(), fsNode->childs.end(), CompareFirst<std::string, std::shared_ptr<gameInfoKV>>("SearchPaths"))->second);


	//The priority dlc pack order (from leaked code...:( ):
	// - update (and its paks (up to 99))
	// - game_dlcXX - where XX goes from 1 to 99 (and its paks up to 99)
	// - game and its paks (up to 99)
	// Note: paks have trailing zero, dlc's don't
	//Note that this system mounts loose files first, inverse of SDK 2013 philosophy.
	if(!isSDK2013Game)
	  {
	    namespace fileSys=std::filesystem;
	    using fileSys::path;
	    //first "update" directory
	    if(fileSys::exists(path(baseDir+"/update"))&&fileSys::is_directory(path(baseDir+"/update")))
	      {
		searchPaths.emplace_back(PathID::GAME,"update");

		loadPAKs("update");
	      }
	    //then DLCs
	    //part 1: discovery - how many dlcs to mount?
	      int dlcCount =0;
	      for (int i=1;i<=99;i++) {
		  std::string dlcDir=fmt::format("{0}_dlc{1}",fileSys::path(modDir).filename().generic_string(),i);
		  if(fileSys::exists(baseDir+"/"+dlcDir)
		     &&fileSys::is_directory(baseDir+"/"+dlcDir))
		    {
		      dlcCount++;
		    } else
		      break; //we're done here
		}
	    //part 2: preparation. Mount them in reverse order
	      while (dlcCount>0){

		  std::string baseGameDir=fileSys::path(modDir).filename().native();
		  searchPaths.emplace_back(PathID::GAME,fmt::format("{0}_dlc{1}",baseGameDir,dlcCount));

		  loadPAKs(fmt::format("{0}_dlc{1}",baseGameDir,dlcCount));
		  dlcCount--;
		}
	  }

	
	for (auto iterator = searchPaths_raw.attribs.begin(); iterator != searchPaths_raw.attribs.end(); ++iterator) {
		std::pair<std::string, std::string> path = *iterator;
		PathID pPathID = resolvePathIDs(path.first);
		auto pair = std::make_pair(pPathID,path.second);

		resolveLoadDir(pair);
		pair.second = std::filesystem::canonical(pair.second);
		searchPaths.insert(searchPaths.end(),pair);

		if(!isSDK2013Game)
		  {

		    loadPAKs(pair.second);
		  }
	}
}

void FileSystem::CGameInfo::resolveLoadDirs()
{
	//TODO this might not have ending slash
	for (auto iterator = searchPaths.begin(); iterator != searchPaths.end(); ++iterator) {
	    resolveLoadDir(*iterator);

	  }
}

void FileSystem::CGameInfo::resolveLoadDir(std::pair<PathID, std::string>& entry)
{
  if ((entry.second.find(BASEGAME_DIR_TMPL) != std::string::npos)) {
replace(entry.second, BASEGAME_DIR_TMPL, baseDir+"/");
  }else if ((entry.second.find(MODDIR_TMPL) != std::string::npos)) {
replace(entry.second, MODDIR_TMPL, modDir+"/");
  }
  else if (std::filesystem::path(entry.second).is_relative()){ //we do not want to add base dir twice.
//assume we're loading from |all_source_engine_paths|
//TODO: Linux's version of portal2 has bogus detection of custom sourcemods. This is diffrent than behavior in windows and must be reported to Valve.
entry.second = baseDir+"/" + entry.second;
  }
}

bool FileSystem::CGameInfo::replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
    return true;
}

FileSystem::CGameInfo::CGameInfo(std::string modDir)
{
            this->isSDK2013Game = true;
            std::ifstream txtGI_str;
            this->modDir = modDir;
			SPDLOG_INFO("Loading file {0}", modDir+"/gameinfo.txt");
            txtGI_str.open(modDir+"/gameinfo.txt");
            memGI = tyti::vdf::read< gameInfoKV>(txtGI_str);
            txtGI_str.close();

            getSteamAppID();
            resolveBaseDir();
            if(std::find(implicitLoadGameInfoID.begin(),implicitLoadGameInfoID.end(),appID)!=implicitLoadGameInfoID.end())
              {
              isSDK2013Game=false;
              SPDLOG_INFO("Detected implicit loading from AppID: {0}",appID);
              }
            if(!isSDK2013Game) {
                  auto modDir = this->modDir + "/";
                  modDir = std::filesystem::canonical(modDir);
                  assert(std::filesystem::is_directory(modDir));
                  auto modName = std::filesystem::path(modDir).filename().native();
                  if(modName.find("_dlc")!=std::string::npos) {

                      SPDLOG_INFO("Detected DLC to the game! Removing now to be readded later.");
                      if(this->baseDir != std::filesystem::path(modDir).parent_path().native()) {
                          //sanity check failed. bail.

                          throw std::logic_error("DLC not in the same directory as the base game!");
                        }
                          ;
                      //strip it. We're adding this later. The check is here, beacuse we need to reload gameinfo.
                      modName.substr(0,modName.find("_dlc"));
                      memGI = gameInfoKV{}; //reinit
                      modDir = std::filesystem::path(modDir).parent_path().native()+"/"+modName;

                      modDir = std::filesystem::canonical(modDir);
                      this->modDir = modDir;
                      txtGI_str.open(modDir+"/gameinfo.txt");
                      memGI = tyti::vdf::read< gameInfoKV>(txtGI_str);
                      txtGI_str.close();
                    }
              }
            //memGI = tyti::vdf::read< tyti::vdf::multikey_object>(txtGI_str);
            //txtGI_str.close();
            initGamepaths();
            resolveLoadDirs();
			SPDLOG_INFO("GameInfo file fully loaded.");
}



bool FileSystem::CGameInfo::prepareTmpDirectory(std::string& tmpDir)
{
    /*
        What we need:
        - maps/(x).bsp
        - scenes/(all)
        - scripts/talker/(all)
    */

  //TODO:Handle cases, where we have mixed image and vcd integration.

    bool result = true;
    std::string error;
// TODO: Remove duplicates
	tmpDir = baseDir + "/tmp/";
	std::filesystem::create_directory(tmpDir);
	SPDLOG_INFO("Output dir is now : {0}", tmpDir);

    for (int element = filesAndTargets.size()-1;element>=0;element--){
        std::vector<std::shared_ptr<IFile>> files;
        std::vector<std::shared_ptr<IFile>> filesChunk;

        files.reserve(65536);
        filesChunk = filesAndTargets[element].second->Find("maps/");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        filesChunk = filesAndTargets[element].second->Find("scenes/");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        filesChunk = filesAndTargets[element].second->Find("scripts/talker/");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        files.shrink_to_fit();


        filesChunk.clear();
        for (auto filePtr=files.begin();filePtr!=files.end();filePtr++) {
            SPDLOG_INFO("Now extracting {0} to {1}",filePtr->get()->relPath,tmpDir);
            result &= filePtr->get()->extract(baseDir+"/tmp/",error);
        }



    }
    return result;
}

void FileSystem::CGameInfo::initializeFileSystem()
{
    //this->resolveBaseDir();
    //this->resolveLoadDir();

    for (auto searchPaths_iter=searchPaths.begin();searchPaths_iter!=searchPaths.end();searchPaths_iter++){
        if ( ((int)(searchPaths_iter->first) & (int)FileSystem::PathID::GAME ) |
             ((int)(searchPaths_iter->first) & (int)FileSystem::PathID::MOD)) {

            std::string packFile = searchPaths_iter->second;

            //TODO: IMountPath, IDir,IFileEntry
            if (packFile.find("custom")==std::string::npos) {
                std::shared_ptr<IMountPath> mountPath = IMountPath::Mount(packFile);
                mountPath->ListFiles(mountPath->fileList);
                filesAndTargets.push_back(std::make_pair(packFile,std::move(mountPath)));
            }

        }
      }
}



void FileSystem::CGameInfo::resolveBaseDir()
{
	std::string steamDir;
	std::vector<std::string> steamLibDirs;
#ifdef _WIN32
	HKEY hKey;
#if defined(_M_AMD64) || defined(_x86_64_)
	LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
#elif defined(_M_IX86) || defined(_x86_)
	LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\\Valve\\Steam", 0, KEY_READ, &hKey);
#endif
	
	bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
	bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);
	std::wstring w_steamDir;
	GetStringRegKey(hKey, L"InstallPath", w_steamDir, L"bad");
	_bstr_t b(w_steamDir.c_str());
	steamDir = std::string(b);

#elif __linux__
	//under normal conditions check for presence of ~/.steam/root
	// resolve the symlink and set it as steamDir
    char* steamPath = new char[65536];
    const char* tmp = "/.steam/root";
    char *begin = steamPath;
    char *end = begin + 65536;
    std::fill(begin, end, 0);
    strcat(steamPath,getenv("HOME"));
    strcat(steamPath,tmp);
    //char* steamPath = getenv("HOME") + "/.steam/root";
    steamDir = std::filesystem::canonical(steamPath).generic_string();
    delete[] steamPath;
#elif __APPLE__ && __MACH__
	// TODO
	//Assume this is in ~/Applications where instructed.
	//If not... um well.
#endif
	steamLibDirs.push_back(steamDir);
	std::ifstream VDF;
	VDF.open(steamDir+"/steamapps/libraryfolders.vdf");

	auto libraryFoldersVDF = tyti::vdf::read< tyti::vdf::multikey_object>(VDF);
	VDF.close();

    steamLibDirs.reserve(libraryFoldersVDF.childs.size()+1);
    for (auto iterator = libraryFoldersVDF.childs.begin(); iterator != libraryFoldersVDF.childs.end(); ++iterator) {
        std::string path = iterator->second.get()->attribs.find("path")->second;
        steamLibDirs.emplace_back(path);
	}
    sort( steamLibDirs.begin(), steamLibDirs.end() );
    steamLibDirs.erase( unique( steamLibDirs.begin(), steamLibDirs.end() ), steamLibDirs.end() );
    steamLibDirs.shrink_to_fit();
	this->baseDir = getPathFromAppID(this->appID,steamLibDirs);
	SPDLOG_INFO("Found basedir at {0}", this->baseDir);

}

FileSystem::PathID FileSystem::CGameInfo::resolvePathIDs(std::string input) {
	boost::char_separator<char> sep("+");
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;
	using boost::algorithm::iequals;
	FileSystem::PathID out=(FileSystem::PathID)0;
	tokenizer tokens(input, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter) {
	    auto token = *tok_iter;
		if (iequals(token,"game") || iequals(token,"vpk"))
            out |= FileSystem::PathID::GAME;
        else if (iequals(token,"game_write"))
            out |= FileSystem::PathID::GAME_WRITE;
                else if (iequals(token,"gamebin"))
			out |= FileSystem::PathID::GAMEBIN;
		else if (iequals(token,"mod"))
			out |= FileSystem::PathID::MOD;
		else if (iequals(token,"mod_write"))
			out |= FileSystem::PathID::MOD_WRITE;
		else if (iequals(token,"platform"))
			out |= FileSystem::PathID::PLATFORM;
		else if (iequals(token,"download"))
			out |= FileSystem::PathID::DOWNLOAD;

	}

	return out;

}

std::string FileSystem::CGameInfo::getPathFromAppID(int appID, std::vector<std::string> steamLibDirs)
{
	//The game cannot be installed in more than one location.
	for (auto iterator = steamLibDirs.begin(); iterator != steamLibDirs.end(); ++iterator) {
		std::string gamePath = *iterator;
        if (!std::filesystem::exists(gamePath + "/steamapps/" + "appmanifest_" + std::to_string(appID) + ".acf"))
			continue;
		std::ifstream VDF;
		VDF.open(gamePath + "/steamapps/" + "appmanifest_" + std::to_string(appID) + ".acf");
		auto appManifest = tyti::vdf::read< tyti::vdf::multikey_object>(VDF);
		VDF.close();

		//appManifest = *(appManifest.childs.find("AppState")->second);
		return gamePath + "/steamapps/common/" + appManifest.attribs.find("installdir")->second;
	}
	return std::string("");
}


#ifdef ENABLE_TESTING
BOOST_AUTO_TEST_CASE(testGI) {

#ifdef _WIN32
	FileSystem::CGameInfo gameInfo("E:/source-sdk-2013/mp/game/mod_hl2mp");
#else
    FileSystem::CGameInfo gameInfo("/home/slawomir/Dane/SteamLibrary/steamapps/common/Half-Life 2/hl2");
#endif


    gameInfo.initializeFileSystem();
    std::string tmpDir;
    gameInfo.prepareTmpDirectory(tmpDir);
}
#endif

