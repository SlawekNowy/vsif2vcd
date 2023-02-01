#pragma once

#include "pch.hpp"
#include "map_bsp.hpp"
#include "gameinfo.hpp"
using namespace FileSystem;


namespace Program {

	int doStart(std::string gameDir);

	//gameinfo object
	extern CGameInfo gi;
    //extern VSIF::ValveScenesImageFile vsif;

	void appendHardCodedEntries();
	// TODO move to own class
	void dumpSceneNamesFromItemsGame(std::string tmpDir);
};




