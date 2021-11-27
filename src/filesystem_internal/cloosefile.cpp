#include "cloosefile.h"



CLooseFile::CLooseFile(std::string basePath, std::string relPath)
{
    this->basePath = basePath;this->relPath=relPath;
    filePathHandle = *new std::filesystem::path(basePath+relPath);

}

bool CLooseFile::extract(std::string whereTo, std::string &errorStr)
{
    bool bResult = true;
    std::filesystem::path destPath(whereTo+relPath);
    try {
        std::filesystem::copy_file(filePathHandle, destPath); // throws: files do not exist
    }
    catch(std::filesystem::filesystem_error const& ex) {

        std::stringstream error;
        error
            << "what():  " << ex.what() << '\n'
            << "path1(): " << ex.path1() << '\n'
            << "path2(): " << ex.path2() << '\n'
            << "code().value():    " << ex.code().value() << '\n'
            << "code().message():  " << ex.code().message() << '\n'
            << "code().category(): " << ex.code().category().name() << '\n';
        error.flush();
        errorStr = error.str();
        bResult = false;
    }
    return bResult;
}
