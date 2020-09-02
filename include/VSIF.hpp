



#pragma once
#include "pch.hpp"

#include "helper.hpp"
#include "BVCD.hpp"
using namespace Helper;


//ODR
namespace VSIF {
	typedef struct {
		unsigned int ID;
		unsigned int Version;
		unsigned int ScenesCount;
		unsigned int StringsCount;
		unsigned int EntryOffset;

		
	} VSIF_Header;

	template <typename S>
	void serialize(S& s, VSIF_Header& vsif) {
		s.value4b(vsif.ID);
		s.value4b(vsif.Version);
		s.value4b(vsif.ScenesCount);
		s.value4b(vsif.StringsCount);
		s.value4b(vsif.EntryOffset);
	};
	
	typedef struct {
		unsigned int CRC;
		unsigned int Offset, Size;
		unsigned int SummaryOffset;


		
	} VSIF_Entry;
	

	template <typename S>
	void serialize(S& s, VSIF_Entry& vsif) {
		s.value4b(vsif.CRC);
		s.value4b(vsif.Offset);
		s.value4b(vsif.Size);
		s.value4b(vsif.SummaryOffset);
	};

	

	typedef struct {
		unsigned int msecs;
		std::vector<int> soundIDs;


		
		

	} VSIF_SceneSummary;


	template <typename S>
	void serialize(S& s, VSIF_SceneSummary& vsif) {
		s.value4b(vsif.msecs);
		uint32_t tmp;
		s.value4b(tmp); //size of the below vector

		vsif.soundIDs.resize(tmp);
		for (int i = 0; i < tmp; i++) {
			s.object(vsif.soundIDs[i]);
		}
	};




	class CStringPool {
	private:
		std::vector<std::string> pool;

	public:
		std::string getStringByID(unsigned short ID);
		unsigned short findOrAddString(std::string stringToPool);
		void prepareToMultiInsert(int howMuch);

	};
	inline std::string CStringPool::getStringByID(unsigned short ID)
	{
		return pool.at(ID);
	}

	inline unsigned short CStringPool::findOrAddString(std::string stringToPool)
	{
		//first check if we have that element
		std::vector<std::string>::iterator it = std::find(pool.begin(), pool.end(), stringToPool);

		if (it != pool.end())
			return std::distance(pool.begin(), it);

		//Okay. This isn't present in the pool. Add this.
		pool.push_back(stringToPool);
		it = std::find(pool.begin(), pool.end(), stringToPool);
		//no check for invalid ID since we just inserted it. Just get ID.
		return std::distance(pool.begin(), it);
	}

	inline void CStringPool::prepareToMultiInsert(int howMuch)
	{
		pool.reserve(howMuch);
	}



	


	//representaion of scenes.image file
	struct ValveScenesImageFile {
		uint32_t size;
		VSIF_Header header;
		CStringPool stringPool;
		std::vector<VSIF_Entry> entries;
		std::vector<VSIF_SceneSummary> summaries;
		uint32_t dataPos;
		//Cannot make a vector of structs since that data might be compressed
		std::vector<char> sceneBuffer;

        std::vector<BVCD::VCD> vcds;
	
		ValveScenesImageFile() {
			//ValveScenesImageFile("scenes.image");
		}
		ValveScenesImageFile(std::string filePath) {


			using Buffer = std::vector<char>;
			using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
			using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
			std::ifstream fileStream = std::ifstream(filePath, std::ios::in | std::ios::binary);
			fileStream.seekg(0, std::ios_base::end);
			size = fileStream.tellg();

			fileStream.seekg(0, std::ios_base::beg);
			Buffer fileBuf(std::istreambuf_iterator<char>(fileStream), {});
			/*
			fileBuf.reserve(size);
			fileStream.read(fileBuf.data(), size);
			assert(fileBuf.size() != 0);
			*/
			bitsery::quickDeserialization<InputAdapter, ValveScenesImageFile>(InputAdapter{ fileBuf.begin(),fileBuf.end() }, *this);

            vcds.resize(header.ScenesCount);
			
			
			

		};
	};


	//template <typename S>
	template <typename S>
	void serialize(S& s, ValveScenesImageFile& vsif) {
		
		s.object(vsif.header);
		assert(vsif.header.ID==FourCC("VSIF") && vsif.header.Version ==2);
		s.ext
		(
			vsif.stringPool,
			bitsery::ext::SaveAndLoad
			(
				[](auto& ser, const CStringPool& obj) 
				{
				}, // save function
				[vsif](auto& des, CStringPool& obj) 
				{ //friend CStringPool;


					//static_assert(std::is_same<decltype(des), bitsery::InputBufferAdapter<std::vector<char>>,"buffers only.");
					std::vector<int> stringOffsets;
					stringOffsets.resize(vsif.header.StringsCount);
					for (int i = 0; i < vsif.header.StringsCount; i++) {
						des.value4b(stringOffsets[i]);
					}
					for (std::vector<int>::iterator iter = stringOffsets.begin();
						iter != stringOffsets.end();
						++iter)
					{
						int stringSize = 0;
						if (std::next(iter)==stringOffsets.end())
						{ //check if this is the last element

							stringSize = vsif.header.EntryOffset - *(iter); //do NOT use incrementation here.
						}
						else
						{
							stringSize = *(iter + 1) - *(iter); //do NOT use incrementation here.
						};
						char* stringRaw = new char[stringSize];
                        des.adapter().template readBuffer<1, char>(stringRaw, stringSize);
						//des.ext(stringRaw, PointerObserver{});
						obj.findOrAddString(stringRaw);
						delete[] stringRaw;
					};

					//TODO: This might end up misaligned. I cannot use currentReadPos for some reason
					//Reason: des is Deserializer, and I need access to InputAdapter
					//This should work. Might be not needed tho.
					while (des.adapter().currentReadPos() % 4 != 0) {
                        des.adapter().currentReadPos(des.adapter().currentReadPos() + 1);
					}
				}
			)
		);
		//HACK: anonymous struct access erroneously selects container(type,func) overload
		size_t scenesCount = vsif.header.ScenesCount;
		vsif.entries.resize(scenesCount);
		vsif.summaries.resize(scenesCount);
		for (int i = 0; i < scenesCount; i++) {
			s.object(vsif.entries[i]);
		}

		for (int i = 0; i < scenesCount; i++) {
			s.object(vsif.summaries[i]);
		}
		//s.container(vsif.entries, scenesCount);
		//s.container(vsif.summaries, scenesCount);


		/*
		dataPos = fileStream.tellg();
		//copy raw BVCD buffer to memory
		fileStream.seekg(0, fileStream.end);
		unsigned int fileEnd = fileStream.tellg();
		fileStream.seekg(dataPos, fileStream.beg);
		fileStream.read(sceneBuffer, fileEnd - dataPos);
		*/
		vsif.dataPos = s.adapter().currentReadPos();
		uint32_t dataSize = vsif.size - vsif.dataPos;
		//s.adapter().currentReadPos(dataPos);
		vsif.sceneBuffer.resize(dataSize);
		char* tmp = new char[dataSize];
		//HACK: There MUST be faster way. 
        s.adapter().template readBuffer<1,char>(tmp,(size_t)dataSize);
        //assert();
		vsif.sceneBuffer = std::vector<char>(tmp, tmp + dataSize + 1);
		delete[] tmp;
		/*
		for (int i = 0; i < (vsif.size - vsif.dataPos); i++) {
			s.value1b(vsif.sceneBuffer[i]);
		}
		*/
	};

};





