
#pragma once
#include "pch.hpp"
#include "CRC.h"
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

            strncpy(this->Name, name, Map_MaxName + 1);
			//this->Name = name;
			normalizeString();
			recalculateCRC();

		}


		

	 private:
		void recalculateCRC() {
            CRC = CRC_Hash((unsigned char*)Name,strlen(Name));
		}
		void normalizeString() {
            std::string name = Name;
            boost::algorithm::to_lower(name);
            std::replace(name.begin(),name.end(),'/','\\');
            //Name = name.c_str();
            strncpy(Name,name.c_str(),Map_MaxValue+1);

		}
    } ;
    extern std::map<std::string,std::vector<Map_Scene>> Scenes;


    static std::vector<BSPParser::Map_Scene> getScenesPerMap(std::string map) {
        return Scenes.find(map)->second;
    }





	int ExtractNames(std::string GameDirectory);

}
