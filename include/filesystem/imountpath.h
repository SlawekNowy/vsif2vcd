#ifndef IMOUNTPATH_H
#define IMOUNTPATH_H
#include <string>
#include <vector>
#include <memory>
#include "ifile.h"


class IMountPath
{
public:
    //mount file or directory
    //virtual bool Mount(std::string path) =0;
    // return file pointers which names match

    static std::shared_ptr<IMountPath> Mount(std::string path);
    virtual std::vector<std::shared_ptr<IFile>> Find(std::string substr) =0;
    virtual void ListFiles(std::vector<std::string>& files) =0;

    std::vector<std::string> fileList;
protected:
    std::string filePath;


};


#endif // IMOUNTPATH_H
