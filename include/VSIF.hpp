
#include "pch.hpp"

namespace VSIF {
	typedef struct {
		unsigned int ID;
		unsigned int Version;
		unsigned int ScenesCount;
		unsigned int StringsCount;
		unsigned int EntryOffset;

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version = 1)
		{
			ar& ID;
			ar& Version;
			ar& ScenesCount;
			ar& StringsCount;
			ar& EntryOffset;
		}
	} VSIF_Header;
	typedef struct {
		unsigned int CRC;
		unsigned int Offset, Size;
		unsigned int SummaryOffset;
	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version = 1)
		{
			ar& CRC;
			ar& Offset;
			ar& Size;
			ar& SummaryOffset;
		}
	} VSIF_Entry;
	typedef struct {
		unsigned int msecs;
		std::vector<int> soundIDs;
	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void save(Archive& ar, const unsigned int version = 1)
		{
			ar& msecs;
			ar& (unsigned int)soundIDs.size();
			for (int i = 0; i < soundIDs.size(); i++)
				ar& soundIDs[i];
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version = 1)
		{
			ar& msecs;
			unsigned int size;
			ar& size;
			soundIDs.resize(size);
			for (int i = 0; i < size; i++)
				ar& soundIDs[i];
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER();
	} VSIF_SceneSummary;



	class CStringPool {
	private:
		std::vector<std::string> pool;

		friend class boost::serialization::access;
	public:
		std::string getStringByID(unsigned short ID);
		unsigned short findOrAddString(std::string stringToPool);
		void prepareToMultiInsert(int howMuch);
		template<class Archive>

		inline static CStringPool loadFromArchive(Archive& ar, VSIF_Header header, std::istream* file) {
			std::vector<unsigned int> stringOffsets;
			stringOffsets.resize(header.StringsCount);
			for (int i = 0; i < stringOffsets.size(); i++)
				ar& stringOffsets[i];
			CStringPool poolObj;
			poolObj.prepareToMultiInsert(header.StringsCount);
			int bytesShifted = 0;

			for (int i = 0; i < stringOffsets.size(); i++) {
				std::string stringPresent;

				unsigned int stringStart = stringOffsets[i];
				unsigned int stringEnd;
				if (i + 1 > header.StringsCount - 1) {
					stringEnd = header.EntryOffset;
				}
				else {
					stringEnd = stringOffsets[i + 1];
				}
				bytesShifted += stringEnd - stringStart;
				char* stringRaw{ new char[stringEnd - stringStart]{} };

				poolObj.findOrAddString(std::string(stringRaw));
				assert(bytesShifted % 4 == 0);
				delete[] stringRaw;

				//this should end up with
				//1. Archive seek pointer already at next DWORD
				//2. Null terminated strings with no more than 1 null.
			}
			return poolObj;
		};

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
			ValveScenesImageFile("scenes.image");
		}
		ValveScenesImageFile(std::string filePath) {
			std::ifstream fileBuffer = std::ifstream(filePath);
			boost::archive::binary_iarchive VSIFStream(fileBuffer);
			VSIFStream >> header;
			stringPool = CStringPool::loadFromArchive(VSIFStream, header,&fileBuffer);
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


		};
	};

	


}