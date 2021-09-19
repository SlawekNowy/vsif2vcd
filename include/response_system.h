#pragma once

#include <pch.hpp>

namespace RRParser
{

extern std::vector<std::string> entryPointsToParse;

    static void initRules(std::string gameDir);
    static void dumpSceneNames();
    static void recursiveIncludePass(std::ifstream& file);
};

