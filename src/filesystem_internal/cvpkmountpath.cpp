#include "cvpkmountpath.h"
#include <VPKFile.h>
#include <cstring>
#include <DirectoryItems.h>

#include <HLLib.h>
#include "cvpkinfile.h"
#include <wildcard.h>

CVPKMountPath::CVPKMountPath(std::string path)
{
    hlInitialize();
    packageHandler = HLLib::CPackage::AutoOpen(path.c_str(),HLFileMode::HL_MODE_READ);
    filePath = path;
}

CVPKMountPath::~CVPKMountPath() {
    packageHandler->Close();
    hlShutdown();
}

std::vector<std::shared_ptr<IFile>> CVPKMountPath::Find(std::string substr)
{

    HLLib::CDirectoryFolder* pRoot = packageHandler->GetRoot();

    pRoot->Sort(); //by default this searches alphabetically and recursively



    //TODO: Refactor this to support wild cards.
    HLLib::CDirectoryItem* pSubstrPos = pRoot->GetRelativeItem(substr.c_str());

    std::vector<std::shared_ptr<IFile>> files;
    if (pSubstrPos!=nullptr) {
            char* pathStr = new char[65536];
            std::memset(pathStr,0,65536);
            pSubstrPos->GetPath(pathStr,65536);
            std::shared_ptr<IFile> singleFileOrFolder = std::make_shared<CVPKInFile>(CVPKInFile(filePath,pathStr,pSubstrPos));
            delete[] pathStr;
            files.push_back(singleFileOrFolder); // this will MOVE the pointer to vector
    }



    /*auto neededFile = pRoot->FindFirst(substr.c_str());

    std::vector<IFile*> fileVector;

    while (neededFile!=nullptr) {
        //neededFile->GetPath()
        //IFile* file = new CVPKInFile();

        neededFile = pRoot->FindNext(neededFile,substr.c_str());

    } */





    return files;


}

static void DoList(std::vector<std::string>& files,HLLib::CDirectoryFolder* pDir)
{
  pDir->Sort();
  auto itemCount = pDir->GetCount();
  for (int i=0;i<=itemCount;i++)
    {
      auto item = pDir->GetItem(i);
      auto type = item->GetType();
      switch (type){
        case HL_ITEM_FOLDER:
          {
            auto castedItem = dynamic_cast<HLLib::CDirectoryFolder*>(item);
            DoList(files,castedItem);
            break;
          }
        case HL_ITEM_FILE:
          {

            auto castedItem = dynamic_cast<HLLib::CDirectoryFile*>(item);
            char path[256];
            castedItem->GetPath(path,256);
            files.emplace_back(path);
            break;
          }
        }
    }


}
void CVPKMountPath::ListFiles(std::vector<std::string> &files)
{

  HLLib::CDirectoryFolder* pDir = packageHandler->GetRoot();
    DoList(files,pDir);
}
