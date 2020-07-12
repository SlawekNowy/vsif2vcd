#include "gameinfo.hpp"
#include <boost/lexical_cast.hpp>

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#elif __linux__

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

void GI::CGameInfo::initGamepaths()
{
	assert(memGI.name == "GameInfo"); 
	bool isMultiPlayer =true;
	//check if the key exists and it's singleplayer
	if (memGI.attribs.find("type")!= memGI.attribs.end()) {
		auto gameType = memGI.attribs.find("type")->second;
		if (gameType == "singleplayer_only") {
			isMultiPlayer = false;
		}
	}
	auto fsNode = memGI.childs.find("FileSystem")->second;
	if (fsNode->attribs.find("SteamAppID")->second !="") {
		appID = boost::lexical_cast<int>(fsNode->attribs.find("SteamAppID")->second);
	}
	
	auto searchPaths_raw = *(fsNode->childs.find("SearchPaths")->second);
	


	//We have to use iterators for this one
	for (auto iterator = searchPaths_raw.attribs.begin(); iterator != searchPaths_raw.attribs.end(); ++iterator) {
		std::pair<std::string, std::string> path = *iterator;
		PathID pPathID = resolvePathIDs(path.first);
		searchPaths.insert(std::make_pair(pPathID,path.second));
	}

}

void GI::CGameInfo::resolveLoadDir()
{
	//TODO this might not have ending slash
	for (auto iterator = searchPaths.begin(); iterator != searchPaths.end(); ++iterator) {
		if ((iterator->second.find(BASEGAME_DIR_TMPL) != std::string::npos)) {
			replace(iterator->second, BASEGAME_DIR_TMPL, baseDir);
		}else if ((iterator->second.find(MODDIR_TMPL) != std::string::npos)) {
			replace(iterator->second, MODDIR_TMPL, modDir);
		}
		else {
			//assume we're loading from |gameinfo_path|
			iterator->second = modDir + iterator->second;
		}
	}
}

bool GI::CGameInfo::replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void GI::CGameInfo::prepareTmpDirectory()
{
}

void GI::CGameInfo::resolveBaseDir()
{
	std::string steamDir;
	std::vector<std::string> steamLibDirs;
#ifdef _WIN32
	HKEY hKey;
#ifdef _M_AMD64 ||_x86_64_
	LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
#elif  	_M_IX86 || _x86_
	LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\\Valve\\Steam", 0, KEY_READ, &hKey);
#endif
	
	bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
	bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);
	std::wstring w_steamDir;
	GetStringRegKey(hKey, L"InstallPath", w_steamDir, L"bad");
	_bstr_t b(w_steamDir.c_str());
	steamDir = std::string(b);

#elif __linux__

#elif __APPLE__ && __MACH__
	// TODO
#endif
	steamLibDirs.push_back(steamDir);
	std::ifstream VDF;
	VDF.open(steamDir+"/steamapps/libraryfolders.vdf");
	auto libraryFoldersVDF = tyti::vdf::read< tyti::vdf::multikey_object>(VDF);
	VDF.close();

	libraryFoldersVDF = *(libraryFoldersVDF.childs.find("LibraryFolders")->second);
	steamLibDirs.reserve(libraryFoldersVDF.attribs.size() - 2);
	for (auto iterator = libraryFoldersVDF.attribs.begin(); iterator != libraryFoldersVDF.attribs.end(); ++iterator) {
		std::pair<std::string, std::string> pair = *iterator;
		try {
			int i = boost::lexical_cast<int>(pair.first);
			steamLibDirs.push_back(pair.second);
		}
		catch (boost::bad_lexical_cast) {
			continue;//discard rest
		}
	}

	this->baseDir = getPathFromAppID(this->appID,steamLibDirs);

}

GI::PathID GI::CGameInfo::resolvePathIDs(std::string input) {
	boost::char_separator<char> sep("+");
	typedef boost::tokenizer<boost::char_separator<char> >
		tokenizer;

	GI::PathID out=(GI::PathID)0;
	tokenizer tokens(input, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter) {
		if (*tok_iter == "game" || *tok_iter == "vpk")
			out |= GI::PathID::GAME;
		else if (*tok_iter == "game_write")
			out |= GI::PathID::GAME_WRITE;
		else if (*tok_iter == "gamebin")
			out |= GI::PathID::GAMEBIN;
		else if (*tok_iter == "mod")
			out |= GI::PathID::MOD;
		else if (*tok_iter == "mod_write")
			out |= GI::PathID::MOD_WRITE;
		else if (*tok_iter == "platform")
			out |= GI::PathID::PLATFORM;
		else if (*tok_iter == "download")
			out |= GI::PathID::DOWNLOAD;

	}

	return out;

}

std::string GI::CGameInfo::getPathFromAppID(int appID, std::vector<std::string> steamLibDirs)
{
	//The game cannot be installed in more than one location.
	for (auto iterator = steamLibDirs.begin(); iterator != steamLibDirs.end(); ++iterator) {
		std::string gamePath = *iterator;
		if (!boost::filesystem::exists(gamePath + "/steamapps/" + "appmanifest_" + std::to_string(appID) + ".acf"))
			continue;
		std::ifstream VDF;
		VDF.open(gamePath + "/steamapps/" + "appmanifest_" + std::to_string(appID) + ".acf");
		auto appManifest = tyti::vdf::read< tyti::vdf::multikey_object>(VDF);
		VDF.close();

		appManifest = *(appManifest.childs.find("AppState")->second);
		return gamePath + "/steamapps/common/" + appManifest.attribs.find("installdir")->second;
	}
	
}


