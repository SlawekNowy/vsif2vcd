#pragma once

#include "pch.hpp"
#include "map_bsp.hpp"
#include "gameinfo.hpp"
#include "VSIF.hpp"

using namespace GI;


namespace Program {

	std::vector<BSPParser::Map> scenesPerMap;

	int doStart(std::string gameDir);

	//gameinfo object
	CGameInfo gi;
	//
	VSIF::ValveScenesImageFile vsif;

};


