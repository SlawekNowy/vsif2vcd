#ifndef CLOOSEMOUNTPATH_H
#define CLOOSEMOUNTPATH_H

#include <filesystem/imountpath.h>
#include <filesystem>

class CLooseMountPath : public IMountPath
{
   // IMountPath interface
public:

    CLooseMountPath(std::string path);

    std::vector<std::shared_ptr<IFile>> Find(std::string substr) override;


public:
    void ListFiles(std::vector<std::string> &files) override;

private:
    std::filesystem::directory_entry dirHandle;


};

#endif // CLOOSEMOUNTPATH_H
