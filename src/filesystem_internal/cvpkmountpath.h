#ifndef CVPKMOUNTPATH_H
#define CVPKMOUNTPATH_H

#include <filesystem/imountpath.h>

#include <Package.h>

class CVPKMountPath : public IMountPath
{
public:
    CVPKMountPath(std::string path);

    std::vector<std::shared_ptr<IFile>> Find(std::string substr) override;



    virtual ~CVPKMountPath();
private:
    HLLib::CPackage* packageHandler;


    // IMountPath interface
public:
    void ListFiles(std::vector<std::string> &files) override;
};

#endif // CVPKMOUNTPATH_H
