#include "response_system.h"
#include <filesystem>



std::vector<std::string> RRParser::entryPointsToParse = {"scripts/talker/response_rules.txt"} ;//path hardcoded. Why vector? we might have multiple files to parse;
static void RRParser::initRules(std::string gameDir)
{
    std::filesystem::create_directory(gameDir+"tmp_rr"); //tmp directory for script parsing.

    for (auto entryPoint = entryPointsToParse.begin();entryPoint != entryPointsToParse.end();++entryPoint) {
        std::ifstream file(entryPoint.base()->c_str());
        recursiveIncludePass(file);
    }
}

static void RRParser::dumpSceneNames()
{

}

static void recursiveIncludePass(std::ifstream &file)
{
    // get rid of the includes by creating a file without includes.
    //do so until we don't have any includes
}
