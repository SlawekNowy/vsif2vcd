#pragma once

#include <pch.hpp>

namespace RRParser
{

extern std::vector<std::string> entryPointsToParse;

    void initRules(std::string gameDir);
    void dumpSceneNames();
    void recursiveIncludePass(std::ifstream& file);
};

