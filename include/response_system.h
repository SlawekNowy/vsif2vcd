#pragma once

#include <pch.hpp>

namespace RRParser
{
class CResponseRulesScript;

extern std::vector<std::string> entryPointsToParse;
extern std::vector<CResponseRulesScript> responseSystems;


void initRules(std::string gameDir); //start parsing scripts
class CResponseRulesScript { //Contains the entry point and its include akin to CDefaultResponseSystem and CInstancedResponseSystem
private:
    std::vector<std::string> includedFiles;
    static void stripQuotes(std::string& quoted);
public:
    bool isGlobal; //true if it's a global talker script (CDefault), false otherwise (CInstanced).

    void parseScript(std::ifstream& file);
};

void dumpSceneNames(); // Iterate over scripts and extract scene filenames
};

