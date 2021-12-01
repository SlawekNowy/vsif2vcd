#ifndef IFILE_H
#define IFILE_H
#include <string>


class IFile {

    //files path must be initialized at implementation's constructor!!
protected:
    std::string basePath;
    std::string relPath;
public:
    virtual bool extract(std::string whereTo,std::string& errorStr)=0; //this extracts or copies the file.

};

#endif // IFILE_H
