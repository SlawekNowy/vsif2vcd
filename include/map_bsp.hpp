#include "pch.hpp"

namespace BSPParser {
constexpr auto Map_MaxName = 32;
constexpr auto Map_MaxKey = 64;
constexpr auto Map_MaxValue = 1024;
	typedef struct {
		unsigned int CRC;
		char Name[Map_MaxValue + 1];
	} Map_Scene;
	typedef struct {
		std::vector<Map_Scene> Scenes;
		char Name[Map_MaxName] = "global"; //default value if this is from response file. Multiplayer games are global
	} Map;
	static char BVCD_Interpolators[16][23] = {
		"default", "catmullrom_normalize_x", "easein", "easeout",
		"easeinout", "bspline", "linear_interp", "kochanek",
		"kochanek_early", "kochanek_late", "simple_cubic", "catmullrom",
		"catmullrom_normalize", "catmullrom_tangent", "exponential_decay", "hold"
	};
	int ExtractNames(std::vector<Map> Target, char* GameDirectory);

}