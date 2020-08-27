#include "pch.hpp"
#include "helper.hpp"

#pragma once
namespace BVCD {
    using namespace Helper;
    static  constexpr float One255th = 0.003922f;
    static  constexpr float One4096th = 0.000244f;
    enum Event_Type{
		Event_Unspecified,
		Event_Section, Event_Expression, Event_LookAt, Event_MoveTo,
		Event_Speak, Event_Gesture, Event_Sequence, Event_Face,
		Event_FireTrigger, Event_FlexAnimation, Event_SubScene, Event_Loop,
        Event_Interrupt, Event_StopPoint, Event_PermitResponses, Event_Generic,
        Event_Invalid =-1
    } ;
    static  const char Event_TypeNames[17][16] = {
		"unspecified",
		"section", "expression", "lookat", "moveto",
		"speak", "gesture", "sequence", "face",
		"firetrigger", "flexanimation", "subscene", "loop",
		"interrupt", "stoppoint", "permitresponses", "generic"
	};
    static  char Interpolators[16][23] = {
        "default", "catmullrom_normalize_x", "easein", "easeout",
        "easeinout", "bspline", "linear_interp", "kochanek",
        "kochanek_early", "kochanek_late", "simple_cubic", "catmullrom",
        "catmullrom_normalize", "catmullrom_tangent", "exponential_decay", "hold"
    };
    static inline std::string eventTypeToString(Event_Type event){
        return Event_TypeNames[event];
    };

    static inline Event_Type eventTypeFromString(std::string event) {
        for (int i=0;i<17;i++) {
            if (event == Event_TypeNames[i]){
                return (Event_Type)i;
            }
        }
        return Event_Invalid;
    }

    struct VCD_Event {

    };

    struct VCD_Actor {

    };
    class VCD {
    public:
        uint32_t magic; // bvcd
        uint8_t version;
        uint32_t CRC;
        std::vector<VCD_Event> events;
        std::vector<VCD_Actor> actors;
        uint8_t ignorePhonemes;

    };


    class CompressedVCD {
        uint32_t magic; //LZMA
        uint32_t realSize;
        uint32_t compressedSize;
        char properties[5];
        std::vector<char> compressedBuffer;
    };



    inline VCD getSceneFromBuffer(std::vector<char> buffer) {
        uint32_t readMagic = FourCC(new char[4]{buffer[0],buffer[1],buffer[2],buffer[3]});

    if (readMagic == FourCC("LZMA")) {

        } else if (readMagic == FourCC("bvcd")) {

        }

        return VCD();
    };







}





