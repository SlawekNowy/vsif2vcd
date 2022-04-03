#ifndef CLOOSEMOUNTPATH_H
#define CLOOSEMOUNTPATH_H

#include <filesystem/imountpath.h>
#include <filesystem>

class CLooseMountPath : public IMountPath
{
public:

    CLooseMountPath(std::string path);

    std::vector<std::shared_ptr<IFile>> Find(std::string substr) override;

private:
    std::filesystem::directory_entry dirHandle;
};

#endif // CLOOSEMOUNTPATH_H
