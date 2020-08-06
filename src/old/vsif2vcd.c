/*	VSIF2VCD
	Version 1.1
	By SiPlus
	http://steamcommunity.com/id/SiPlus */

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
#include <direct.h>
#else
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "v2vlzma.h"


	/*
	--------------------------------------------------------------------------------
	SCENES.IMAGE EXTRACTION - TYPES
	*/
typedef struct {
	unsigned int ID;
	unsigned int Version;
	unsigned int ScenesCount;
	unsigned int StringsCount;
	unsigned int EntryOffset;
} VSIF_Header;


/*
--------------------------------------------------------------------------------
GENERAL PURPOSE FUNCTIONS
*/
/* FourCC - creates 4-byte file identifier */
#define FourCC(A, B, C, D) (A | (B << 8) | (C << 16) | (D << 24))

/* Reinterpret - interprets char pointer as data type */
#define Reinterpret(Pointer, Type) (*(Type *)(Pointer))


/*
--------------------------------------------------------------------------------
CRC CODE
*/
const unsigned int CRC_Table[256] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
#define CRC_SingleXOR CRC = CRC_Table[*(Buffer++) ^ (unsigned char)CRC] ^ (CRC >> 8);
#define CRC_FourXORs CRC ^= *((unsigned int *)Buffer); \
	CRC = CRC_Table[(unsigned char)CRC] ^ (CRC >> 8); \
	CRC = CRC_Table[(unsigned char)CRC] ^ (CRC >> 8); \
	CRC = CRC_Table[(unsigned char)CRC] ^ (CRC >> 8); \
	CRC = CRC_Table[(unsigned char)CRC] ^ (CRC >> 8);
unsigned int CRC_Hash(unsigned char* Buffer, unsigned int Size)
{
	unsigned int CRC = 0xffffffff;
	unsigned int Front;
	unsigned int Main;
	for (;;)
	{
		switch (Size)
		{
		case 7:
			CRC_SingleXOR
		case 6:
			CRC_SingleXOR
		case 5:
			CRC_SingleXOR
		case 4:
			CRC_FourXORs
				return CRC ^ 0xffffffff;
		case 3:
			CRC_SingleXOR
		case 2:
			CRC_SingleXOR
		case 1:
			CRC_SingleXOR
		case 0:
			return CRC ^ 0xffffffff;
		}
		Front = ((unsigned int)Buffer) & 3;
		Size -= Front;
		switch (Front)
		{
		case 3:
			CRC_SingleXOR
		case 2:
			CRC_SingleXOR
		case 1:
			CRC_SingleXOR
		}
		Main = Size >> 3;
		while (Main--)
		{
			CRC_FourXORs
				Buffer += 4;
			CRC_FourXORs
				Buffer += 4;
		}
		Size &= 7;
	}
}


/*
--------------------------------------------------------------------------------
MAP SCENE NAMES PARSING
*/
#define Map_MaxName 32
#define Map_MaxKey 64
#define Map_MaxValue 1024
typedef struct {
	unsigned int CRC;
	char Name[Map_MaxValue + 1];
} Map_Scene;
typedef struct {
	unsigned int ScenesCount;
	Map_Scene* Scenes;
	char Name[Map_MaxName];
} Map;

/* Map_ParseMapList - generates an array of map names from maplist.txt */
unsigned int Map_ParseMapList(Map** Target, char* GameDirectory)
{
	char* TXTPath;
	FILE* TXTFile;
	char* TXT;
	unsigned int TXTSize;
	int Cursor;
	unsigned int MapsCount;
	unsigned int i;

	/* Loading */
	puts("Parsing maplist.txt");
	TXTPath = malloc(strlen(GameDirectory) + 13); /* freed after fopen */
	strcpy(TXTPath, GameDirectory);
	strcat(TXTPath, "/maplist.txt");
	TXTFile = fopen(TXTPath, "rb");
	free(TXTPath);
	if (!TXTFile)
	{
		fprintf(stderr, "Unable to open maplist.txt: %s\n", strerror(errno));
		return 0;
	}
	fseek(TXTFile, 0, SEEK_END);
	TXTSize = ftell(TXTFile);
	if (!TXTSize)
	{
		fputs("Map list is empty\n", stderr);
		fclose(TXTFile);
		return 0;
	}
	rewind(TXTFile);
	TXT = malloc(TXTSize); /* freed before exiting */
	fread(TXT, 1, TXTSize, TXTFile);
	fclose(TXTFile);

	/* Parsing */
	Cursor = 0;
	MapsCount = 0;
	for (i = 0; i < TXTSize; i++)
	{
		if (TXT[i] == '\r')
		{
			Cursor = 0;
			i++;
			continue;
		}
		if (!Cursor)
		{
			MapsCount++;
			*Target = realloc(*Target, (MapsCount) * sizeof(Map));
			memset(&((*Target)[MapsCount - 1]), 0, sizeof(Map));
		}
		if (Cursor < Map_MaxName)
		{
			(*Target)[MapsCount - 1].Name[Cursor] = TXT[i]; /* Index -1 will never be reached */
		}
		Cursor++;
	}

	free(TXT);
	return MapsCount;
}

/* Map_ParseEntities - extracts scene names from entity lump */
void Map_ParseEntities(Map* Target, unsigned char* Source, unsigned int Size)
{
	int QuotesCount = 0;
	int Cursor;
	int IsScene, SceneNameFound;
	char Key[Map_MaxKey], Value[Map_MaxValue], SceneName[Map_MaxValue + 1];
	unsigned int i; /* Used in loop through all bytes */
	unsigned int j; /* Used for scene name fixup for platform independence and CRC */

	for (i = 0; i < Size; i++)
	{
		if (Source[i] == '\"') /* String start/end */
		{
			QuotesCount++;
			switch (QuotesCount)
			{
			case 1: /* Key start */
				memset(Key, 0, Map_MaxKey);
				Cursor = 0;
				break;
			case 3: /* Value start */
				memset(Value, 0, Map_MaxValue);
				Cursor = 0;
				break;
			case 4: /* Value end */
				QuotesCount = 0;
				if (!strcmp(Key, "classname"))
				{
					if (!strcmp(Value, "logic_choreographed_scene"))
						IsScene = 1;
				}
				else if (!strcmp(Key, "SceneFile"))
				{
					strcpy(SceneName, Value);
					SceneNameFound = 1;
				}
				break;
			}
			continue;
		}
		if (Source[i] == '{') /* Entity start */
		{
			IsScene = 0;
			SceneNameFound = 0;
			continue;
		}
		if (Source[i] == '}') /* Entity end */
		{
			if (!(IsScene && SceneNameFound))
				continue;
			Target->Scenes = realloc(Target->Scenes, (Target->ScenesCount + 1) * sizeof(Map_Scene));
			/* First, change \ to / to make the name usable on all file systems */
			for (j = 0; j < Map_MaxValue; j++)
			{
				if (SceneName[j] == '\0')
					break;
				if (SceneName[j] == '\\')
					SceneName[j] = '/';
			}
			strcpy(Target->Scenes[Target->ScenesCount].Name, SceneName);
			/* Second, fix the name for CRC hash */
			for (j = 0; j < Map_MaxValue; j++)
			{
				if (SceneName[j] == '\0')
					break;
				if ((SceneName[j] >= 'A') && (SceneName[j] <= 'Z'))
				{
					SceneName[j] = SceneName[j] - 'A' + 'a';
					continue;
				}
				if (SceneName[j] == '/')
					SceneName[j] = '\\';
			}
			Target->Scenes[Target->ScenesCount].CRC = CRC_Hash(SceneName, strlen(SceneName));
			Target->ScenesCount++;
			continue;
		}
		if (QuotesCount == 1)
		{
			/* Read key */
			if (Cursor < Map_MaxKey)
			{
				Key[Cursor] = Source[i];
				Cursor++;
			}
			continue;
		}
		if (QuotesCount == 3)
		{
			/* Read value */
			if (Cursor < Map_MaxValue)
			{
				Value[Cursor] = Source[i];
				Cursor++;
			}
		}
	}
}

/* Map_AddMap - loads map's entity lump to be parsed, and then parses it */
#define Map_AddMap_Error(Text) fprintf(stderr, Text"\n", MapToLoad->Name); \
	fclose(MapFile); \
	return;
void Map_AddMap(Map* MapToLoad, char* GameDirectory)
{
	char* MapPath;
	FILE* MapFile;
	unsigned int ID;
	unsigned char* Lump;
	unsigned int LumpOffset, LumpSize;

	/* .lmp loading */
	MapPath = malloc(strlen(GameDirectory) + strlen(MapToLoad->Name) + 15); /* freed in if/else */
	strcpy(MapPath, GameDirectory);
	strcat(MapPath, "/maps/");
	strcat(MapPath, MapToLoad->Name);
	strcat(MapPath, "_l_0.lmp");
	MapFile = fopen(MapPath, "rb");
	if (MapFile)
	{
		free(MapPath);
		fread(&LumpOffset, 4, 1, MapFile);
		if (LumpOffset != 20)
		{
			Map_AddMap_Error("Lump patch of map %s has incorrect data offset")
		}
		fread(&ID, 4, 1, MapFile); /* ID is lump number */
		if (ID)
		{
			Map_AddMap_Error("Lump patch of map %s has incorrect lump number")
		}
		fread(&ID, 4, 1, MapFile); /* ID is version */
		if (ID)
		{
			Map_AddMap_Error("Lump patch of map %s has incorrect version")
		}
		fread(&LumpSize, 4, 1, MapFile);
	}
	else
	{
		/* .bsp loading */
		strcpy(MapPath, GameDirectory);
		strcat(MapPath, "/maps/");
		strcat(MapPath, MapToLoad->Name);
		strcat(MapPath, ".bsp");
		MapFile = fopen(MapPath, "rb");
		free(MapPath);
		if (!MapFile)
		{
			fprintf(stderr, "Unable to load map %s: %s\n", MapToLoad->Name, strerror(errno));
			return;
		}
		fread(&ID, 4, 1, MapFile); /* ID is FourCC */
		if (ID != FourCC('V', 'B', 'S', 'P'))
		{
			Map_AddMap_Error("Map %s has incorrect FourCC")
		}
		fread(&ID, 4, 1, MapFile); /* ID is version */
		if ((ID != 19) && (ID != 20))
		{
			Map_AddMap_Error("Map %s has incorrect version")
		}
		fread(&LumpOffset, 4, 1, MapFile);
		fread(&LumpSize, 4, 1, MapFile);
		fread(&ID, 4, 1, MapFile);
		if (ID)
		{
			Map_AddMap_Error("Entity lump of map %s has incorrect version")
		}
	}

	/* Code for both .lmp and .bsp */
	fseek(MapFile, LumpOffset, SEEK_SET);
	Lump = malloc(LumpSize); /* freed after parsing */
	fread(Lump, 1, LumpSize, MapFile);
	Map_ParseEntities(MapToLoad, Lump, LumpSize);
	free(Lump);
	fclose(MapFile);
}
/* Map_ExtractNames - calls Map_ subroutines from the entry point */
unsigned int Map_ExtractNames(Map** Target, char* GameDirectory)
{
	unsigned int MapsCount;
	unsigned int i;

	MapsCount = Map_ParseMapList(Target, GameDirectory);
	if (!MapsCount)
		return 0;
	puts("Extracting scene names from maps");
	for (i = 0; i < MapsCount; i++)
		Map_AddMap(&((*Target)[i]), GameDirectory);
	return MapsCount;
}


/*
--------------------------------------------------------------------------------
BINARY VCD DECOMPILATION
*/
/* BVCD_PoolString - gets string from string pool */
char* BVCD_PoolString(unsigned char* Source, unsigned char* Image) /* Inlining this creates errors (at least for me) */
{
	return Image + *((unsigned int*)(Image + sizeof(VSIF_Header) + ((*(unsigned short*)(Source)) << 2)));
}

/* BVCD_Ramp - decompiles scene_ramp and event_ramp */
void BVCD_Ramp(FILE* VCD, unsigned char** Source, int InEvent, int InChannel)
{
	unsigned int Count, i;
	char Tab[7] = "";
	char RampName[6] = "event";

	/* Strings */
	if (InEvent)
		strcpy(Tab, "  ");
	else
		strncpy(RampName, "scene", 5);
	if (InChannel)
		strcat(Tab, "    ");

	/* Writing */
	Count = (*Source)[0];
	(*Source)++;
	if (!Count)
		return;
	fprintf(VCD, "%s%s_ramp\r\n%s{\r\n", Tab, RampName, Tab);
	for (i = 0; i < Count; i++)
	{
		fprintf(VCD, "%s  %.4f %.4f\r\n", Tab, Reinterpret(*Source, float), (float)((*Source)[4]) / 255.0f);
		(*Source) += 5;
	}
	fprintf(VCD, "%s}\r\n", Tab);
}

/* BVCD_CurveName - gets curve name for its number */
static char BVCD_Interpolators[16][23] = {
	"default", "catmullrom_normalize_x", "easein", "easeout",
	"easeinout", "bspline", "linear_interp", "kochanek",
	"kochanek_early", "kochanek_late", "simple_cubic", "catmullrom",
	"catmullrom_normalize", "catmullrom_tangent", "exponential_decay", "hold"
};
char* BVCD_Event_CurveName(int Type, int Edge) /* Edge - 0 is left, 1 is right */
{
	if (!Edge)
		Type >>= 8;
	Type &= 0xff;
	if (Type >= 16)
		return NULL;
	return BVCD_Interpolators[Type];
}

/* BVCD_Event_Flex - decompiles facial animation in events */
#define One255th 0.003922f
#define One4096th 0.000244f
void BVCD_Event_Flex(FILE* VCD, unsigned char** Source, unsigned char* Image, int InChannel)
{
	unsigned int i, j;
	unsigned int Count_Tracks, Count_Samples;
	unsigned int Flags;

	char Tab[5] = "";
	if (InChannel)
		strcpy(Tab, "    ");

	Count_Tracks = (*Source)[0];
	(*Source)++;
	if (!Count_Tracks)
		return;
	/* Header */
	fprintf(VCD, "%s  flexanimations samples_use_time\r\n%s  {\r\n", Tab, Tab);

	/* Tracks */
	for (i = 0; i < Count_Tracks; i++)
	{
		fprintf(VCD, "%s    \"%s\"", Tab, BVCD_PoolString(*Source, Image));
		(*Source) += 2;
		Flags = (*Source)[0];
		(*Source)++;
		if (!(Flags & 1))
			fputs(" disabled", VCD);
		if (Flags & 2)
			fputs(" combo", VCD);
		if ((Reinterpret(*Source, float) != 0.0f) && (Reinterpret((*Source) + 4, float) != 1.0f))
			fprintf(VCD, " range %.1f %.1f", Reinterpret(*Source, float), Reinterpret((*Source) + 4, float));
		(*Source) += 8;

		/* Samples */
		fprintf(VCD, "\r\n%s    {\r\n", Tab);
		Count_Samples = Reinterpret(*Source, unsigned short);
		(*Source) += 2;
		for (j = 0; j < Count_Samples; j++)
		{
			fprintf(VCD, "%s      %.4f %.4f", Tab, Reinterpret(*Source, float), (float)((*Source)[4]) * One255th);
			(*Source) += 5;
			if (Reinterpret(*Source, unsigned short))
			{
				fprintf(VCD, " curve_%s_to_curve_%s",
					BVCD_Event_CurveName(Reinterpret(*Source, unsigned short), 0),
					BVCD_Event_CurveName(Reinterpret(*Source, unsigned short), 1));
			}
			(*Source) += 2;
			fputc('\r', VCD);
			fputc('\n', VCD);
		}
		fprintf(VCD, "%s    }\r\n", Tab);

		/* Combo samples */
		if (Flags & 2)
		{
			fprintf(VCD, "%s    {\r\n", Tab);
			Count_Samples = Reinterpret(*Source, unsigned short);
			(*Source) += 2;
			for (j = 0; j < Count_Samples; j++)
			{
				fprintf(VCD, "%s      %.4f %.4f", Tab, Reinterpret(*Source, float), (float)((*Source)[4]) * One255th);
				(*Source) += 5;
				if (Reinterpret(*Source, unsigned short))
				{
					fprintf(VCD, " curve_%s_to_curve_%s",
						BVCD_Event_CurveName(Reinterpret(*Source, unsigned short), 0),
						BVCD_Event_CurveName(Reinterpret(*Source, unsigned short), 1));
				}
				(*Source) += 2;
				fputc('\r', VCD);
				fputc('\n', VCD);
			}
			fprintf(VCD, "%s    }\r\n", Tab);
		}
	}

	/* Footer */
	fprintf(VCD, "%s  }\r\n", Tab);
}

/* BVCD_Event - decompiles events */
typedef enum {
	BVCD_Event_Unspecified,
	BVCD_Event_Section, BVCD_Event_Expression, BVCD_Event_LookAt, BVCD_Event_MoveTo,
	BVCD_Event_Speak, BVCD_Event_Gesture, BVCD_Event_Sequence, BVCD_Event_Face,
	BVCD_Event_FireTrigger, BVCD_Event_FlexAnimation, BVCD_Event_SubScene, BVCD_Event_Loop,
	BVCD_Event_Interrupt, BVCD_Event_StopPoint, BVCD_Event_PermitResponses, BVCD_Event_Generic,
	BVCD_Event_TypesCount
} BVCD_Event_Type;
const char BVCD_Event_TypeNames[BVCD_Event_TypesCount][16] = {
	"unspecified",
	"section", "expression", "lookat", "moveto",
	"speak", "gesture", "sequence", "face",
	"firetrigger", "flexanimation", "subscene", "loop",
	"interrupt", "stoppoint", "permitresponses", "generic"
};
void BVCD_Event(FILE* VCD, unsigned char** Source, unsigned char* Image, int InChannel)
{
	BVCD_Event_Type EventType;
	unsigned int Flags;
	unsigned int Count;
	unsigned int AbsoluteTagType;
	unsigned int i;
	char Tab[5] = "";
	if (InChannel)
		strcpy(Tab, "    ");

	/* Header */
	EventType = (*Source)[0];
	fprintf(VCD, "%sevent %s \"%s\"\r\n%s{\r\n", Tab,
		BVCD_Event_TypeNames[EventType],
		BVCD_PoolString(*Source + 1, Image), Tab);
	(*Source) += 3;

	/* Time */
	fprintf(VCD, "%s  time %f %f\r\n", Tab,
		*(float*)(*Source),
		*(float*)(*Source + 4));
	(*Source) += 8;

	/* Params */
	fprintf(VCD, "%s  param \"%s\"\r\n", Tab, BVCD_PoolString(*Source, Image));
	(*Source) += 2;
	if (strlen(BVCD_PoolString(*Source, Image)))
		fprintf(VCD, "%s  param2 \"%s\"\r\n", Tab, BVCD_PoolString(*Source, Image));
	(*Source) += 2;
	if (strlen(BVCD_PoolString(*Source, Image)))
		fprintf(VCD, "%s  param3 \"%s\"\r\n", Tab, BVCD_PoolString(*Source, Image));
	(*Source) += 2;

	/* Ramp */
	BVCD_Ramp(VCD, Source, 1, InChannel);

	/* Flags */
	Flags = (*Source)[0];
	(*Source)++;
	if (Flags & 0x1)
		fprintf(VCD, "%s  resumecondition\r\n", Tab);
	if (Flags & 0x2)
		fprintf(VCD, "%s  lockbodyfacing\r\n", Tab);
	if (Flags & 0x4)
		fprintf(VCD, "%s  fixedlength\r\n", Tab);
	if (!(Flags & 0x8))
		fprintf(VCD, "%s  active 0\r\n", Tab);
	if (Flags & 0x10)
		fprintf(VCD, "%s  forceshortmovement\r\n", Tab);
	if (Flags & 0x20)
		fprintf(VCD, "%s  playoverscript\r\n", Tab);

	/* Distance to target */
	if (Reinterpret(*Source, float) > 0.0f)
		fprintf(VCD, "%s  distancetotarget %.2f\r\n", Tab, Reinterpret(*Source, float));
	(*Source) += 4;

	/* Relative tags */
	Count = (*Source)[0];
	(*Source)++;
	if (Count)
	{
		fprintf(VCD, "%s  tags\r\n%s  {\r\n", Tab, Tab);
		for (i = 0; i < Count; i++)
		{
			fprintf(VCD, "%s    \"%s\" %f\r\n", Tab, BVCD_PoolString(*Source, Image), (float)((*Source)[2]) * One255th);
			(*Source) += 3;
		}
		fprintf(VCD, "%s  }\r\n", Tab);
	}

	/* Flex timing tags */
	Count = (*Source)[0];
	(*Source)++;
	if (Count)
	{
		fprintf(VCD, "%s  flextimingtags\r\n%s  {\r\n", Tab, Tab);
		for (i = 0; i < Count; i++)
		{
			fprintf(VCD, "%s    \"%s\" %f 1\r\n", Tab, BVCD_PoolString(*Source, Image), (float)((*Source)[2]) * One255th);
			(*Source) += 3;
		}
		fprintf(VCD, "%s  }\r\n", Tab);
	}

	/* Absolute tags */
	for (AbsoluteTagType = 0; AbsoluteTagType < 2; AbsoluteTagType++)
	{
		Count = (*Source)[0];
		(*Source)++;
		if (Count)
		{
			fprintf(VCD, "%s  absolutetags", Tab);
			if (AbsoluteTagType)
				fputs(" shifted_time\r\n", VCD);
			else
				fputs(" playback_time\r\n", VCD);
			fprintf(VCD, "%s  {\r\n", Tab);
			for (i = 0; i < Count; i++)
			{
				fprintf(VCD, "%s    \"%s\" %f\r\n", Tab, BVCD_PoolString(*Source, Image), (float)(*(unsigned short*)(*Source + 2)) * One4096th);
				(*Source) += 4;
			}
			fprintf(VCD, "%s  }\r\n", Tab);
		}
	}

	/* Sequence duration */
	if (EventType == BVCD_Event_Gesture)
	{
		if (Reinterpret(*Source, float))
			fprintf(VCD, "%s  sequenceduration %f\r\n", Tab, Reinterpret(*Source, float));
		(*Source) += 4;
	}

	/* Relative tag */
	Count = (*Source)[0];
	(*Source)++;
	if (Count)
	{
		fprintf(VCD, "%s  relativetag \"%s\" \"%s\"\r\n", Tab, BVCD_PoolString(*Source, Image), BVCD_PoolString((*Source) + 2, Image));
		(*Source) += 4;
	}

	/* Flex animation */
	BVCD_Event_Flex(VCD, Source, Image, InChannel);

	/* Loop */
	if (EventType == BVCD_Event_Loop)
	{
		fprintf(VCD, "%s  loopcount \"%i\"\r\n", Tab, (*Source)[0]);
		(*Source)++;
	}

	/* Closed captions */
	if (EventType == BVCD_Event_Speak)
	{
		fprintf(VCD, "%s  cctype \"", Tab);
		switch ((*Source)[0])
		{
		case 1:
			fputs("cc_slave\"\r\n", VCD);
		case 2:
			fputs("cc_disabled\"\r\n", VCD);
		default:
			fputs("cc_master\"\r\n", VCD);
		}
		(*Source)++;
		fprintf(VCD, "%s  cctoken \"%s\"\r\n", Tab, BVCD_PoolString(*Source, Image));
		(*Source) += 2;
		Flags = (*Source)[0];
		(*Source)++;
		if (Flags & 1)
			fprintf(VCD, "%s  cc_usingcombinedfile\r\n", Tab);
		if (Flags & 2)
			fprintf(VCD, "%s  cc_combinedusesgender\r\n", Tab);
		if (Flags & 4)
			fprintf(VCD, "%s  cc_noattenuate\r\n", Tab);
	}

	/* Closing curly brace */
	fprintf(VCD, "%s}\r\n", Tab);
}

/* BVCD_Decompile - converts binary VCD to text VCD */
int BVCD_Decompile(FILE* VCD, unsigned char* Source, unsigned char* Image, char* MapName)
{
	unsigned int Count_Events, Count_Actors, Count_Channels;
	unsigned int i, j, k; /* i - actor, j - channel, k - event */

	/* Header */
	if (Reinterpret(Source, unsigned int) != FourCC('b', 'v', 'c', 'd'))
		return 1;
	if (Source[4] != 4)
		return 1;
	Source += 9;
	fputs("// Choreo version 1\r\n", VCD);

	/* Events */
	Count_Events = Source[0];
	Source++;
	for (k = 0; k < Count_Events; k++)
		BVCD_Event(VCD, &Source, Image, 0);

	/* Actors */
	Count_Actors = Source[0];
	Source++;
	for (i = 0; i < Count_Actors; i++)
	{
		fprintf(VCD, "actor \"%s\"\r\n{\r\n", BVCD_PoolString(Source, Image));
		Count_Channels = Source[2];
		Source += 3;
		/* Channels */
		for (j = 0; j < Count_Channels; j++)
		{
			fprintf(VCD, "  channel \"%s\"\r\n  {\r\n", BVCD_PoolString(Source, Image));
			Count_Events = Source[2];
			Source += 3;
			for (k = 0; k < Count_Events; k++)
				BVCD_Event(VCD, &Source, Image, 1);
			if (!Source[0])
				fputs("    active \"0\"\r\n", VCD);
			Source++;
			fputs("  }\r\n", VCD);
		}
		if (!Source[0])
			fputs("  active \"0\"\r\n", VCD);
		Source++;
		fputs("}\r\n", VCD);
	}

	/* Ramp */
	BVCD_Ramp(VCD, &Source, 0, 0);

	/* Footer */
	if (MapName != NULL)
		fprintf(VCD, "mapname \"maps\\%s.bsp\"\r\n", MapName);
	fputs("scalesettings\r\n{\r\n" \
		"  \"CChoreoView\" \"100\"\r\n" \
		"  \"SceneRampTool\" \"100\"\r\n" \
		"  \"ExpressionTool\" \"100\"\r\n" \
		"  \"GestureTool\" \"100\"\r\n" \
		"  \"RampTool\" \"100\"\r\n}\r\n" \
		"fps 60\r\nsnap off\r\n", VCD);
	return 0;
}

/* BVCD_OpenVCDForWriting - finds scene name by CRC and opens the .vcd file for writing */
int BVCD_OpenVCDForWriting(FILE** VCD, Map* Maps, unsigned int MapsCount, char** TargetMapName, unsigned int CRC, char* GameDirectory)
{
	unsigned int VCDNameLen;
	char* VCDName = NULL;
	char* DirPath;
	int PathFound = 0;
	unsigned int i, j; /* Looping through each map and each scene in a map */
	unsigned int k; /* Directory creation */

	*TargetMapName = NULL;
	VCDNameLen = strlen(GameDirectory) + Map_MaxValue;
	VCDName = malloc(VCDNameLen + 1); /* freed before exiting */
	DirPath = malloc(VCDNameLen + 1); /* freed after searching */
	for (i = 0; i < MapsCount; i++)
	{
		for (j = 0; j < Maps[i].ScenesCount; j++)
		{
			if (CRC == Maps[i].Scenes[j].CRC)
			{
				*TargetMapName = Maps[i].Name;
				strcpy(VCDName, GameDirectory);
				strcat(VCDName, "/");
				strcat(VCDName, Maps[i].Scenes[j].Name);
				for (k = 0; k < VCDNameLen; k++)
				{
					if (VCDName[k] == '\0')
						break;
					if (VCDName[k] != '/')
						continue;
					strncpy(DirPath, VCDName, k);
					DirPath[k] = '\0';
#ifdef _WIN32
					mkdir(DirPath);
#else
					mkdir(DirPath, 0755);
#endif

				}
				PathFound = 1;
				break;
			}
		}
		if (PathFound)
			break;
	}
	free(DirPath);
	if (!PathFound)
		sprintf(VCDName, "%s/scenes/%08x.vcd", GameDirectory, CRC);
	*VCD = fopen(VCDName, "wb");
	if (!(*VCD))
	{
		fprintf(stderr, "Unable to open output scene file %s: %s\n", VCDName, strerror(errno));
		free(VCDName);
		return errno;
	}
	free(VCDName);
	return 0;
}


/*
--------------------------------------------------------------------------------
SCENES.IMAGE EXTRACTION
*/
typedef struct {
	unsigned int CRC;
	unsigned int Offset, Size;
	unsigned int SummaryOffset;
} VSIF_Entry;

/* VSIF_UncompressScene - uncompresses a scene if needed, returns real size */
unsigned int VSIF_UncompressScene(unsigned char* Source, unsigned char** Target, unsigned int Size)
{
	unsigned int RealSize;
	if (Reinterpret(Source, unsigned int) == FourCC('L', 'Z', 'M', 'A'))
	{
		RealSize = Reinterpret(Source + 4, unsigned int);
		*Target = malloc(RealSize); /* freed in ExtractScene */
		LZMA_Uncompress(RealSize, Reinterpret(Source + 8, unsigned int), Source + 12, Source + 17, *Target);
		return RealSize;
	}
	else if (Reinterpret(Source, unsigned int) == FourCC('b', 'v', 'c', 'd'))
	{
		*Target = malloc(Size); /* freed in ExtractScene */
		memcpy(*Target, Source, Size);
		return Size;
	}
	return 0;
}

/* VSIF_ExtractScene - extracts single scene */
void VSIF_ExtractScene(unsigned char* Image, unsigned int Index, Map* Maps, unsigned int MapsCount, char* GameDirectory)
{
	VSIF_Entry* Entry = (VSIF_Entry*)(Image + ((VSIF_Header*)Image)->EntryOffset + Index * sizeof(VSIF_Entry));
	unsigned int SceneBufferSize;
	unsigned char* SceneBuffer = NULL;
	FILE* VCD = NULL;
	char* MapName = NULL;

	/* Uncompressing */
	SceneBufferSize = VSIF_UncompressScene(Image + Entry->Offset, &SceneBuffer, Entry->Size);
	if (!SceneBufferSize)
	{
		fprintf(stderr, "Failed to uncompress scene with CRC %08x\n", Entry->CRC);
		free(SceneBuffer);
		return;
	}

	/* Decompilation */
	if (BVCD_OpenVCDForWriting(&VCD, Maps, MapsCount, &MapName, Entry->CRC, GameDirectory))
	{
		free(SceneBuffer);
		return;
	}
	if (BVCD_Decompile(VCD, SceneBuffer, Image, MapName))
		fprintf(stderr, "Failed to decompile scene with CRC %08x\n", Entry->CRC);
	fflush(VCD);
	fclose(VCD);
	free(SceneBuffer);
}

/* VSIF_Extract - extract all scenes */
int VSIF_Extract(Map* Maps, unsigned int MapsCount, char* GameDirectory)
{
	char* ImagePath;
	FILE* ImageFile;
	unsigned int ImageSize;
	unsigned char* Image;
	VSIF_Header* Header;
	unsigned int i;

	/* Loading scenes.image file */
	ImagePath = malloc(strlen(GameDirectory) + 21); /* freed after fopen */
	strcpy(ImagePath, GameDirectory);
	strcat(ImagePath, "/scenes/scenes.image");
	ImageFile = fopen(ImagePath, "rb");
	free(ImagePath);
	if (!ImageFile)
	{
		fprintf(stderr, "Unable to open scenes.image: %s\n", strerror(errno));
		return errno;
	}
	fseek(ImageFile, 0, SEEK_END);
	ImageSize = ftell(ImageFile);
	Image = malloc(ImageSize); /* freed before exiting - used all time when program lives */
	rewind(ImageFile);
	fread(Image, 1, ImageSize, ImageFile);
	fclose(ImageFile);

	/* Validation */
	Header = (VSIF_Header*)Image;
	if (Header->ID != FourCC('V', 'S', 'I', 'F'))
	{
		fputs("Scenes.image has incorrect FourCC\n", stderr);
		return 1;
	}
	if (Header->Version != 2)
	{
		fputs("Scenes.image has incorrect version\n", stderr);
		return 1;
	}
	if (!Header->ScenesCount)
	{
		fputs("Scenes.image is empty\n", stderr);
		return 1;
	}
	printf("Extracting scenes.image (%u scenes)\n", Header->ScenesCount);

	/* Extraction */
	for (i = 0; i < Header->ScenesCount; i++)
		VSIF_ExtractScene(Image, i, Maps, MapsCount, GameDirectory);

	free(Image);
	printf("Finished extracting scenes from game %s\n", GameDirectory);
	return 0;
}

/*
--------------------------------------------------------------------------------
CONVERSION ENTRY POINT
*/
int Convert(char* GameDirectory)
{
	Map* Maps = NULL;
	unsigned int MapsCount;

	/* Extracting scene names from maps */
	MapsCount = Map_ExtractNames(&Maps, GameDirectory);
	if (!MapsCount)
		fputs("Warning: Extracted scenes will be unnamed\n", stderr);

	/* Loading scenes.image */
	return VSIF_Extract(Maps, MapsCount, GameDirectory);
}


/*
--------------------------------------------------------------------------------
PROGRAM ENTRY POINT
*/
int main(int argc, char** argv)
{
	int ExitCode;
	puts("VSIF2VCD version 1.1 (23 Apr 2012)");
	if (argc < 2)
	{
		fputs("By http://steamcommunity.com/id/SiPlus\nUsage: VSIF2VCD [game directory]\n", stderr);
		return 1;
	}
	puts("Initializing LZMA uncompression module");
	if (LZMA_Init())
		return 1;
	ExitCode = Convert(argv[1]);
	LZMA_Shutdown();
	return ExitCode;
}