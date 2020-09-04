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
};
