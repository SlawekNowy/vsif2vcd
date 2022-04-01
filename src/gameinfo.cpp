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
	if (std::find_if(fsNode->attribs.begin(), fsNode->attribs.end(), CompareFirst<std::string, std::string>("SteamAppId"))->second !="") {
		appID = boost::lexical_cast<int>(std::find_if(fsNode->attribs.begin(), fsNode->attribs.end(), CompareFirst<std::string, std::string>("SteamAppId"))->second);
	}
	
	//We have to use iterators for this one
	auto searchPaths_raw = *(std::find_if(fsNode->childs.begin(), fsNode->childs.end(), CompareFirst<std::string, std::shared_ptr<gameInfoKV>>("SearchPaths"))->second);
	


	
	for (auto iterator = searchPaths_raw.attribs.begin(); iterator != searchPaths_raw.attribs.end(); ++iterator) {
		std::pair<std::string, std::string> path = *iterator;
		PathID pPathID = resolvePathIDs(path.first);
		searchPaths.emplace_back(pPathID,path.second);
	}
}

void FileSystem::CGameInfo::resolveLoadDir()
{
	//TODO this might not have ending slash
	for (auto iterator = searchPaths.begin(); iterator != searchPaths.end(); ++iterator) {
		if ((iterator->second.find(BASEGAME_DIR_TMPL) != std::string::npos)) {
            replace(iterator->second, BASEGAME_DIR_TMPL, baseDir+"/");
		}else if ((iterator->second.find(MODDIR_TMPL) != std::string::npos)) {
            replace(iterator->second, MODDIR_TMPL, modDir+"/");
		}
		else {
            //assume we're loading from |all_source_engine_paths|
            //TODO: Linux's version of portal2 has bogus detection of custom sourcemods. This is diffrent than behavior in windows and must be reported to Valve.
            iterator->second = baseDir+"/" + iterator->second;
		}
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
            std::ifstream txtGI_str;
            this->modDir = modDir;
			SPDLOG_INFO("Loading file {0}", modDir+"/gameinfo.txt");
            txtGI_str.open(modDir+"/gameinfo.txt");
            memGI = tyti::vdf::read< gameInfoKV>(txtGI_str);
            txtGI_str.close();

            //memGI = tyti::vdf::read< tyti::vdf::multikey_object>(txtGI_str);
            txtGI_str.close();
            initGamepaths();
            resolveBaseDir();
            resolveLoadDir();
			SPDLOG_INFO("GameInfo file fully loaded.");
}

FileSystem::CGameInfo::~CGameInfo()
{

};

bool FileSystem::CGameInfo::prepareTmpDirectory(std::string& tmpDir)
{
    /*
        What we need:
        - maps/(x).bsp
        - scenes/scenes.image
        - scripts/talker/(all)
    */

    bool result = true;
    std::string error;
// TODO: Remove duplicates
	tmpDir = baseDir + "/tmp/";
	SPDLOG_INFO("Output dir is now : {0}", tmpDir);

    for (int element = filesAndTargets.size()-1;element>=0;element--){
        std::vector<IFile*> files;
        std::vector<IFile*> filesChunk;

        files.reserve(65536);
        filesChunk = filesAndTargets[element].second->Find("maps/");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        filesChunk = filesAndTargets[element].second->Find("scenes/scenes.image");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        filesChunk = filesAndTargets[element].second->Find("scripts/talker/");
        files.insert(files.end(),filesChunk.begin(),filesChunk.end());
        files.shrink_to_fit();


        filesChunk.clear();
        for (IFile* filePtr:files) {
			SPDLOG_INFO("Now extracting {0} to {1}",filePtr->relPath,tmpDir);
            result &= filePtr->extract(baseDir+"/tmp/",error);
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
                IMountPath* mountPath = IMountPath::Mount(packFile);
                filesAndTargets.push_back(std::make_pair(packFile,mountPath));
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
    steamDir = std::filesystem::canonical(steamPath).native();
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

	FileSystem::PathID out=(FileSystem::PathID)0;
	tokenizer tokens(input, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter) {
		if (*tok_iter == "game" || *tok_iter == "vpk")
            out |= FileSystem::PathID::GAME;
        else if (*tok_iter == "game_write")
            out |= FileSystem::PathID::GAME_WRITE;
		else if (*tok_iter == "gamebin")
			out |= FileSystem::PathID::GAMEBIN;
		else if (*tok_iter == "mod")
			out |= FileSystem::PathID::MOD;
		else if (*tok_iter == "mod_write")
			out |= FileSystem::PathID::MOD_WRITE;
		else if (*tok_iter == "platform")
			out |= FileSystem::PathID::PLATFORM;
		else if (*tok_iter == "download")
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

