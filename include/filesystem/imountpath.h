#ifndef IMOUNTPATH_H
#define IMOUNTPATH_H
#include <string>
#include <vector>
#include "ifile.h"


class IMountPath
{
public:
    //mount file or directory
    //virtual bool Mount(std::string path) =0;
    // return file pointers which names match

    static IMountPath* Mount(std::string path);
    virtual std::vector<IFile*> Find(std::string substr) =0;
protected:
    std::string filePath;


};


#endif // IMOUNTPATH_H
