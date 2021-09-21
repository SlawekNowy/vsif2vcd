#pragma once

#include <pch.hpp>

namespace RRParser
{
    class CResponseRulesScript;
        class CScriptEnumeration;
        class CScriptResponseGroup;
        class CScriptCriterion;
        class CScriptRule;

extern std::vector<std::string> entryPointsToParse;
extern std::vector<CResponseRulesScript> responseSystems;


static void stripQuotes(std::string& quoted);
void initRules(std::string gameDir); //start parsing scripts
class CResponseRulesScript { //Contains the entry point and its include akin to CDefaultResponseSystem and CInstancedResponseSystem
private:
    std::vector<std::string> includedFiles;
public:
    bool isGlobal = false; //true if it's a global talker script (CDefault), false otherwise (CInstanced).
    std::vector<CScriptEnumeration> enums;

    void parseScript(std::string gameDir,std::ifstream& file);
};
    class CScriptEnumeration {
    private:
        std::string name;
    public:
        std::map<std::string,std::string> KV;
        CScriptEnumeration (std::string name) {
            this->name = name;
        }

        void parseEnum(std::ifstream& file);

        std::string getName() {
            return name;
        }
    };
    class CScriptResponseGroup {

        //does reference implementaion diffrentiates single responsetype from multiple?
        //It does not. Both paths call ParseOneResponse later.
        //However parameters "permitrepeats","sequential","norepeat" are valid only for multiple.
        //rest of the parameters are parsed as flags.

        //flags are per response in response group, however in one element group are located *before* responsetype,
        // but in multi element group they are located after it.

    };

void dumpSceneNames(); // Iterate over scripts and extract scene filenames
};

