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
	packageHandler = HLLib::CPackage::AutoOpen(path.c_str(), HLFileMode::HL_MODE_READ);
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
    //lazy init
    if(this->fileList.empty())
    {
        this->ListFiles(fileList);
        std::for_each(fileList.begin(),fileList.end(),[](std::string& element){
            Helper::ReplaceAll(element,"\\","/");
        });
    }
	std::vector<std::string> matchingPaths;
	matchingPaths = filter(this->fileList, [substr](std::string path) {
		return wildcard(substr.c_str(), path.c_str());
		});
	std::vector<std::shared_ptr<IFile>> results;


	for (const auto& path : matchingPaths) {
		HLLib::CDirectoryItem* pSubstrPos = pRoot->GetRelativeItem(path.c_str());
		std::shared_ptr<IFile> fileHandle = std::make_shared<CVPKInFile>(CVPKInFile(this->filePath, path, pSubstrPos));
		results.push_back(std::move(fileHandle));
	}

	

	return results;
}

static void DoList(std::vector<std::string>& files, HLLib::CDirectoryFolder* pDir)
{
	pDir->Sort();
	auto itemCount = pDir->GetCount();
	for (int i = 0; i < itemCount; i++)
	{
		auto item = pDir->GetItem(i);
		auto type = item->GetType();
		switch (type) {
			case HL_ITEM_FOLDER:
			{
				auto castedItem = dynamic_cast<HLLib::CDirectoryFolder*>(item);
				DoList(files, castedItem);
				break;
			}
			case HL_ITEM_FILE:
			{
				auto castedItem = dynamic_cast<HLLib::CDirectoryFile*>(item);
				char path[256];
				castedItem->GetPath(path, 256); //Does this include "root/"?
				std::string strPath{ path };
				strPath = strPath.substr(5);
				files.emplace_back(strPath);
				break;
			}
		}
	}


}
void CVPKMountPath::ListFiles(std::vector<std::string>& files)
{
	HLLib::CDirectoryFolder* pDir = packageHandler->GetRoot();
	DoList(files, pDir);
}
