#ifndef CVPKINFILE_H
#define CVPKINFILE_H

#include <filesystem/ifile.h>
#include <DirectoryItem.h>

class CVPKInFile : public IFile
{
public:
    CVPKInFile(std::string basePath,std::string relPath,HLLib::CDirectoryItem* fileHandle);
     bool extract(std::string whereTo, std::string &errorStr) override;
private:
     HLLib::CDirectoryItem* fileHandle;
};

#endif // CVPKINFILE_H
