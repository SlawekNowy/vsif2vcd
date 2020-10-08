#pragma once

#include "pch.hpp"
#include "map_bsp.hpp"
#include "gameinfo.hpp"
#include "VSIF.hpp"

using namespace FileSystem;


namespace Program {

    extern  std::vector<BSPParser::Map_Scene> scenesPerMap;

	int doStart(std::string gameDir);

	//gameinfo object
	extern CGameInfo gi;
    //extern VSIF::ValveScenesImageFile vsif;

	void appendHardCodedEntries();

};




