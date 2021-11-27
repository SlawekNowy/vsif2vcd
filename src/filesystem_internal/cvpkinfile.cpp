#include "cvpkinfile.h"
#include <HLTypes.h>
#include <HLLib.h>

CVPKInFile::CVPKInFile(std::string basePath,std::string relPath,HLLib::CDirectoryItem* fileHandle)
{
    this->basePath = basePath;
    this->relPath = relPath.substr(5);
    this->fileHandle = fileHandle;
}

bool  CVPKInFile::extract(std::string whereTo, std::string &errorStr)
{
    bool success = this->fileHandle->Extract(whereTo.c_str());
    if (!success) {
        errorStr= HLLib::LastError.GetLongFormattedErrorMessage();
        HLLib::LastError.Clear();
    }
    return success;
}
