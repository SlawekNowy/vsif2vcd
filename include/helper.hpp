#pragma once
#include "pch.hpp"


namespace VSIF {
    class ValveScenesImageFile;
}

#pragma once
namespace Helper {
	static inline uint32_t FourCC(char ID[4]) {
		return (ID[0] | (ID[1] << 8) | (ID[2] << 16) | (ID[3] << 24));
	}

    extern VSIF::ValveScenesImageFile* vsif;


    static inline void ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        //return str;
    }
};
