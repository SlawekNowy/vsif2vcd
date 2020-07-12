#include "VSIF.hpp"

using namespace VSIF;
template <class Archive>
CStringPool CStringPool::loadFromArchive(Archive& ar, VSIF_Header header,std::istream* file) {
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
}