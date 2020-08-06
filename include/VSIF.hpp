
#include "pch.hpp"



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
		s.container4b(vsif.soundIDs, tmp);
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

		VSIF_Header header;
		CStringPool stringPool;
		std::vector<VSIF_Entry> entries;
		std::vector<VSIF_SceneSummary> summaries;
		unsigned int dataPos;
		//Cannot make a vector of structs since that data might be compressed
		char* sceneBuffer;



		ValveScenesImageFile() {
			//ValveScenesImageFile("scenes.image");
		}
		ValveScenesImageFile(std::string filePath) {


			using Buffer = std::vector<char>;
			using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
			using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
			std::ifstream fileStream = std::ifstream(filePath, std::ios::in | std::ios::binary);
			fileStream.seekg(0, std::ios_base::end);
			uint32_t size = fileStream.tellg();

			fileStream.seekg(0, std::ios_base::beg);
			Buffer fileBuf(std::istreambuf_iterator<char>(fileStream), {});
			/*
			fileBuf.reserve(size);
			fileStream.read(fileBuf.data(), size);
			assert(fileBuf.size() != 0);
			*/
			bitsery::quickDeserialization<InputAdapter, ValveScenesImageFile>(InputAdapter{ fileBuf.begin(),fileBuf.end() }, *this);

			dataPos = fileStream.tellg();
			//copy raw BVCD buffer to memory
			fileStream.seekg(0, fileStream.end);
			unsigned int fileEnd = fileStream.tellg();
			fileStream.seekg(dataPos, fileStream.beg);
			fileStream.read(sceneBuffer, fileEnd - dataPos);
#if 0
			boost::archive::binary_iarchive VSIFStream(fileBuffer);
			VSIFStream >> header;
			stringPool = CStringPool::loadFromArchive(VSIFStream, header, &fileBuffer);
			//Those two have same indexes
			summaries.resize(header.ScenesCount);
			for (int i = 0; i < header.ScenesCount; i++)
				VSIFStream >> entries[i];
			for (int i = 0; i < header.ScenesCount; i++)
				VSIFStream >> summaries[i];
			dataPos = fileBuffer.tellg();
			//copy raw BVCD buffer to memory
			fileBuffer.seekg(0, fileBuffer.end);
			unsigned int fileEnd = fileBuffer.tellg();
			fileBuffer.seekg(dataPos, fileBuffer.beg);
			fileBuffer.read(sceneBuffer, fileEnd - dataPos);

#endif
		};
	};


	//template <typename S>
	template <typename S>
	void serialize(S& s, ValveScenesImageFile& vsif) {
		s.object(vsif.header);
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
						if (std::next(iter)==stringOffsets.end())
						{ //check if this is the last element

							int stringSize = vsif.header.EntryOffset - *(iter); //do NOT use incrementation here.
							char* stringRaw = new char[stringSize];
							//HACK: very naive C style pointer array, but it should be sufficient.
							for (int i = 0; i < stringSize; i++) {
								des.value1b(*(stringRaw + i));
							}
							//des.ext(stringRaw, PointerObserver{});
							obj.findOrAddString(stringRaw);
							//delete stringRaw[];
						}
						else
						{
							int stringSize = *(iter + 1) - *(iter); //do NOT use incrementation here.
							char* stringRaw = new char[stringSize];
							for (int i = 0; i < stringSize; i++) {
								des.value1b(*(stringRaw + i));
							}
							//des.ext(stringRaw, PointerObserver{});
							obj.findOrAddString(stringRaw);
							//delete stringRaw[];
						};
					};

					//TODO: This ends up misaligned. I cannot use currentReadPos for some reason
				}
			)
		);
		//HACK: anonymous struct access erroneously selects container(type,func) overload
		size_t scenesCount = vsif.header.ScenesCount;
		s.container(vsif.entries, scenesCount);
		s.container(vsif.summaries, scenesCount);
	};

};





