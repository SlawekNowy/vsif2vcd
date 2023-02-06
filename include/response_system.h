#pragma once

#include <pch.hpp>

namespace RRParser
{
    class CResponseRulesScript;
        class CScriptEnumeration;
        class CScriptResponseGroup;
            class CScriptResponse;
        class CScriptCriterion;
        class CScriptRule;

extern std::vector<std::string> entryPointsToParse;
extern std::vector<CResponseRulesScript> responseSystems;




//static void stripQuotes(std::string& quoted);
void initRules(std::string gameDir); //start parsing scripts
class CResponseRulesScript { //Contains the entry point and its include akin to CDefaultResponseSystem and CInstancedResponseSystem
private:
    std::vector<std::string> includedFiles;
public:
    bool isGlobal = false; //true if it's a global talker script (CDefault), false otherwise (CInstanced).
    std::vector<CScriptEnumeration> enums;
    std::vector<CScriptResponseGroup> responseGroups;
    std::vector<CScriptCriterion> criteria;
    std::vector<CScriptRule> rules;

    void parseScript(std::string gameDir,std::ifstream& file);

    static bool isRootToken(std::string token) {
        return !token.compare("#include") |
                !token.compare("enumeration") |
                !token.compare("response")|
                !token.compare("criterion")|
                !token.compare("criteria")|
                !token.compare("rule");
    }
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
        //in reference implementaion, code's parsing flags is repeated beacuse of this.

       private:
        std::string name;
    public:
        //Those flags are not changed for single member groups.
        bool permitRepeats = false;
        bool sequential = false;
        bool noRepeats = true;

        std::vector<CScriptResponse> responses;

        CScriptResponseGroup(std::string name) {
            this->name = name;
        }


        std::string getName() {
            return name;
        }
        void parseResponseGroup(std::ifstream& file);


    };

    enum EResponseType {
        SPEAK = 0,
        SENTENCE,
        SCENE,
        RESPONSE,
        PRINT
    };

    class CScriptResponse {
    public:
        float predelayStart = 0;
        float predelayEnd =0;
        float delayStart =0;
        float delayEnd = 0;
        float respeakDelayStart =0;
        float respeakDelayEnd = 0;
        EResponseType type;
        std::string typeParam;
        bool speakOnce = false;
        bool bypassScene = false;
        short odds = 100;
        float respeakDelay = 0;
        std::string soundlevel = "SNDLVL_TALKING";
        float weight =1 ;
        bool displayFirst = false;
        bool displayLast = false;
        void parseFlags(std::vector<std::string>& flags);
        void parseType(std::vector<std::string>& types);
private:

        static constexpr float DEF_MIN_DELAY = 2.8;
        static constexpr float DEF_MAX_DELAY = 3.2;

    };

    class CScriptCriterion {
    private:
        std::string name;
    public:
        std::string matchKey;
        std::string matchValue;
        float weight = 1.0;
        bool required=false;

        CScriptCriterion() = default;

        CScriptCriterion(std::string name) {
            this->name = name;
        }

        void parseCriterion(std::vector<std::string>& flags);

    };

    class CScriptRule {
    private:
        std::string name;
    public:
        std::vector<std::string> referencedCriteria;
        std::vector<std::string> referencedResponses;
        bool triggerOnce = false;
        std::vector<CScriptCriterion> anonymousCriteria;

        CScriptRule(std::string name) {
            this->name = name;
        }

        void parseRule(std::ifstream& file);
    };

void dumpSceneNames(); // Iterate over scripts and extract scene filenames
};

