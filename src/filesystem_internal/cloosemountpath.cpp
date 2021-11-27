#include "cloosemountpath.h"
#include "cloosefile.h"

CLooseMountPath::CLooseMountPath(std::string path)
{
    this->dirHandle = *new std::filesystem::directory_entry{path};
    assert(!dirHandle.is_directory());
    this->filePath = path;

}

std::vector<IFile *> CLooseMountPath::Find(std::string substr)
{
    using namespace std::filesystem;
    const path rootPath = dirHandle.path();
    std::vector<IFile*> results;

    for (const auto& file:recursive_directory_iterator{rootPath}) {
        if (file.is_regular_file()) {
            IFile* fileHandle = new CLooseFile(this->filePath,file.path().generic_string());
            results.push_back(fileHandle);
        }
    }

    return results;


}
