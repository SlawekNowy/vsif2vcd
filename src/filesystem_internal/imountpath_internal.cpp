#include <filesystem/imountpath.h>
#include "cvpkmountpath.h"
#include "cloosemountpath.h"
#include <VPKFile.h>
#include <Package.h>
#include <HLLib.h>
#include <filesystem>


std::shared_ptr<IMountPath> IMountPath::Mount(std::string path) {

    if (path.find(".vpk")==path.size()-4) {
        //first check if file exists. If it doesn't it's likely to be multichunk.
        if (!std::filesystem::exists(path)) {
            path = path.substr(0,path.find(".vpk"));
            path.append("_dir.vpk");
        }

        // Still doesn't exist? Return nullptr
        if (!std::filesystem::exists(path)) {
            return nullptr;
        }

        return std::shared_ptr<CVPKMountPath>(new CVPKMountPath(path));
        /*assert(std::filesystem::exists(path));
        opaquePtr = (void*)package;
        */
    } else {
        //this is a dir
        std::filesystem::path loosePath(path);
        assert(std::filesystem::is_directory(loosePath));
        return std::shared_ptr<CLooseMountPath>(new CLooseMountPath(path));
    }


}
