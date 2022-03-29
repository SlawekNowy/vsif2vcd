#include "cloosefile.h"
#include <sstream>


CLooseFile::CLooseFile(std::string basePath, std::string relPath)
{
    this->basePath = basePath+"/";this->relPath=relPath;
    filePathHandle = *new std::filesystem::path(this->basePath+relPath);

}

bool CLooseFile::extract(std::string whereTo, std::string &errorStr)
{
    bool bResult = true;
    std::filesystem::path destPath(whereTo+relPath);
    try {
        std::filesystem::path destDir = destPath;
        destDir= destDir.remove_filename();
        std::filesystem::create_directories(destDir);
        std::filesystem::copy_file(filePathHandle, destPath,std::filesystem::copy_options::overwrite_existing); // throws: files do not exist
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
