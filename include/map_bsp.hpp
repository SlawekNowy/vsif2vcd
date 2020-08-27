
#pragma once
#include "pch.hpp"

namespace BSPParser {
constexpr auto Map_MaxName = 32;
constexpr auto Map_MaxKey = 64;
constexpr auto Map_MaxValue = 1024;
	 struct Map_Scene {
		unsigned int CRC;
		char Name[Map_MaxValue + 1];

		Map_Scene() {

		}
		Map_Scene(char* name) {

			memcpy(this->Name, name, Map_MaxName + 1);
			//this->Name = name;
			normalizeString();
			recalculateCRC();

		}

		

	 private:
		void recalculateCRC() {

		}
		void normalizeString() {

		}
	} ;
	struct Map {
        std::unordered_set<Map_Scene> Scenes;
		char Name[Map_MaxName] = "global"; //default value if this is from response file. Multiplayer games are global

    } ;

	int ExtractNames(std::string GameDirectory);

}
