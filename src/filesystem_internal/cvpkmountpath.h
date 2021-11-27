#ifndef CVPKMOUNTPATH_H
#define CVPKMOUNTPATH_H

#include <filesystem/imountpath.h>

#include <Package.h>

class CVPKMountPath : public IMountPath
{
public:
    CVPKMountPath(std::string path);

    std::vector<IFile*> Find(std::string substr) override;



    ~CVPKMountPath();
private:
    HLLib::CPackage* packageHandler;

};

#endif // CVPKMOUNTPATH_H
