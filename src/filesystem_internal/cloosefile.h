#ifndef CLOOSEFILE_H
#define CLOOSEFILE_H

#include <filesystem/ifile.h>
#include <filesystem>

class CLooseFile : public IFile
{
public:
    CLooseFile(std::string basePath,std::string relPath);
    bool extract(std::string whereTo, std::string &errorStr) override;

private:
    std::filesystem::path filePathHandle;
};

#endif // CLOOSEFILE_H
