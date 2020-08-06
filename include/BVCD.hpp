

namespace BVCD {
	constexpr float One255th = 0.003922f;
	constexpr float One4096th = 0.000244f;
	typedef enum {
		Event_Unspecified,
		Event_Section, Event_Expression, Event_LookAt, Event_MoveTo,
		Event_Speak, Event_Gesture, Event_Sequence, Event_Face,
		Event_FireTrigger, Event_FlexAnimation, Event_SubScene, Event_Loop,
		Event_Interrupt, Event_StopPoint, Event_PermitResponses, Event_Generic,
		Event_TypesCount
	} Event_Type;
	const char Event_TypeNames[Event_TypesCount][16] = {
		"unspecified",
		"section", "expression", "lookat", "moveto",
		"speak", "gesture", "sequence", "face",
		"firetrigger", "flexanimation", "subscene", "loop",
		"interrupt", "stoppoint", "permitresponses", "generic"
	};

}





