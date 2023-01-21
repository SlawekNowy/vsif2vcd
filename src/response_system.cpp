﻿

//std::vector<std::string> includedFiles;

//pasted for reference
// This is the base rule script file for the AI response system for Expressive AI's who speak based on certain "Concepts"
// You can think of a concept as a high level state that the code is trying to convey, such as say hello, or say you're mad, etc.
//
// The format of this file is that there are five main types of commands:
// 1) #include "filename"	// This just causes the included scriptfile to be parsed and added to the database
// 2) enumeration:  this declares an enumerated type so that comparisons can be matched against the string versions of the type
// 3) response:  this specifies a response to issue.  A response consists of a weighted set of options and can recursively reference
//    other responses by name
// 4) criterion:  This is a match condition
// 5) rule:  a rule consists of one or more criteria and a response
//
// In general, the system is presented with a criteria set, which is a set of key value pairs generated by the game code and
//  various entity I/O and keyfields.  For instance, the following criteria set was created in a map with a train terminal
// "speaker" entity wishing to fire random station announcements
//               concept = 'train_speaker' (weight 5.000000)		; the high level concept for the search request
//                   map = 'terminal_pa'							; the name of the map
//             classname = 'speaker'								; the classname and name of the "speaking" entity
//                  name = 'terminal_pa'
//                health = '10'										; the absolute health of the speaking entity
//            healthfrac = '0.000'									; the health fraction (health/maxhealth) of the speaking entity
//          playerhealth = '100'									; similar data related to the current player:
//      playerhealthfrac = '1.000'
//          playerweapon = 'none'									; the name of the weapon the player is carrying
//        playeractivity = 'ACT_WALK'								; animating activity of the player
//           playerspeed = '0.000'									; how fast the player is moving
//
// Based on such a criteria set, the system checks each rule against the set.  To do this, each criterion of the rule is
//  given a numeric score as follows:
// score = 0 if criteria doesn't match or, criterion weight * keyvaliue weight if it does match
// The final score for a rule is the sum of all of the scores of its criteria.  The best rule is the one with the highest
//  score.  Once a best rule is selected, then a response is looked up based on the response definitions and the engine is
//  asked to dispatch that response.
//
// The specific syntax for the various keywords is as follows:
//
// ENUMERATIONS:
//
// enumeration <enumerationname>
// {
//		"key1" "value1"
//		"key2" "value2"
//		...etc.
//	}
//	The code and criteria refer to enumerations with square brackets and a double colon separator, e.g.:
//  [enumerationname::key1]
//
//
// RESPONSES:
//
// Single line:
// response <responsegroupname> [nodelay | defaultdelay | delay interval ] [speakonce] [noscene] [odds nnn] [respeakdelay interval] [soundelvel "SNDLVL_xxx"] responsetype parameters
// Multiple lines
// response <responsegroupname>
// {
//		[permitrepeats]   ; optional parameter, by default we visit all responses in group before repeating any
//		[sequential]	  ; optional parameter, by default we randomly choose responses, but with this we walk through the list starting at the first and going to the last
//		[norepeat]		  ; Once we've run through all of the entries, disable the response group
//		responsetype1 parameters1 [nodelay | defaultdelay | delay interval ] [speakonce] [odds nnn] [respeakdelay interval] [soundelvel "SNDLVL_xxx"] [displayfirst] [ displaylast ] weight nnn
//		responsetype2 parameters2 [nodelay | defaultdelay | delay interval ] [speakonce] [odds nnn] [respeakdelay interval] [soundelvel "SNDLVL_xxx"] [displayfirst] [ displaylast ] weight nnn
//		etc.
// }
// Where:
//   interval = "startnumber,endnumber" or "number" (e.g., "2.8,3.2" or "3.2")
//   responsetype =:
//     speak		; it's an entry in sounds.txt
//     sentence		; it's a sentence name from sentences.txt
//     scene		; it's a .vcd file
//     response		; it's a reference to another response group by name
//     print      ; print the text in developer 2 (for placeholder responses)
//   nodelay = an additional delay of 0 after speaking
//   defaultdelay = an additional delay of 2.8 to 3.2 seconds after speaking
//   delay interval = an additional delay based on a random sample from the interval after speaking
//   speakonce = don't use this response more than one time (default off)
//	 noscene = For an NPC, play the sound immediately using EmitSound, don't play it through the scene system. Good for playing sounds on dying or dead NPCs.
//   odds = if this response is selected, if odds < 100, then there is a chance that nothing will be said (default 100)
//	 respeakdelay = don't use this response again for at least this long (default 0)
//   soundlevel = use this soundlevel for the speak/sentence (default SNDLVL_TALKING)
//   weight = if there are multiple responses, this is a selection weighting so that certain responses are favored over others in the group (default 1)
//   displayfirst/displaylast : this should be the first/last item selected (ignores weight)
//
// CRITERIA:
//
// criterion <criterionname> <matchkey> <matchvalue> weight nnn required
// Where:
//  matchkey matches one of the criteria in the set as shown above
//  matchvalue is a string or number value or a range, the following are all valid:
//  "0"						; numeric match to value 0
//  "1"						; numeric match to value 1
//   "weapon_smg1"			; string match to weapon_smg1 string
//   "[npcstate::idle]"		; match enumeration by looking up numeric value
//   ">0"					; match if greater than zero
//   ">10,<=50"				; match if greater than ten and less than or equal to 50
//   ">0,<[npcstate::alert]"	; match if greater than zer and les then value of enumeration for alert
//   "!=0"					; match if not equal to zero
// weight = floating point weighting for score assuming criteria match (default value 1.0)
// required:  if a rule has one or more criteria with the required flag set, then if any such criteria
//  fail, the entire rule receives a score of zero
//
// RULE:
//
// rule <rulename>
// {
//    criteria name1 [name2 name3 etc.]
//    response responsegroupname [responsegroupname2 etc.]
//    [matchonce]					; optional parameter
//	  [ <matchkey > <matchvalue> weight nnn required ]
// }
// Where:
// criteria just lies one more more criterion names from above and response list one or more of the response
// names from above (usually just one)
// matchonce (off by default): means that the rule is deactivated after the first time it is matched
// Note that additional "unnamed" criteria can be specified inline in the rule using the same syntax
// as for defining a criterion, except for the criterion keyword and the criterion name keys
//
// Interaction with entity I/O system
// CBaseEntity contains an inputfunc called "DispatchResponse" which accepts a string which is a concept name
//  Thus, a game entity can fire this input on another entity with a concept string and a criteria set will
//  be generated and searched against the entities current response system rule set.
// Right now only the speaker entity and NPC_Talker derived NPCs have any response rules loaded
// In addition, map placed entities have up to three "context" keypairs that can be specified.
// They take the form:  "key:value" (key, single colon separator, value)
// When an entity with any such context keypairs is asked to dispatch a response, the keypairs are added to the
//  criteria set passed to the rule system.  Thus, map placed entities and triggers can specify their
//  own context keypairs and these can be hooked up to response rules to do map-specific and appropriate
//  responses
// In addition, entity I/O can be used to add, remove and clear any such context keypairs via the
//   AddContext, RemoveContext, and ClearContext input functions.
// AddContext takes a keypair of the "key:value" format, while RemoveContext take just the "key"
// ClearContext removes all context keypairs
// The game .dll code can enumerate context keypairs and change them via code based methods
//
// The player and the world have their context added with the string player or world as a prefix, e.g.:
//  "playerkey:value" or "worldkey:value" to differentiate world/player context from the context of the
//  responding entity.


//TODO: does reference implementation perform sanity checks?

#include "response_system.h"
#include "map_bsp.hpp"
#include <filesystem>
#include <vector>


#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <iterator>


std::vector<std::string> RRParser::entryPointsToParse = {"scripts/talker/response_rules.txt"} ;//path hardcoded. Why vector? we might have multiple files to parse;
std::vector<RRParser::CResponseRulesScript> RRParser::responseSystems;
void RRParser::initRules(std::string gameDir)
{
    //std::filesystem::create_directory(gameDir+"tmp_rr"); //tmp directory for script parsing.

    for (auto entryPoint = entryPointsToParse.begin();entryPoint != entryPointsToParse.end();++entryPoint) {
        std::string absoluteFile = gameDir+"/";
        absoluteFile += *entryPoint;
        std::ifstream file(absoluteFile.c_str());
        CResponseRulesScript script;
        if (std::strcmp(entryPoint->c_str(),"scripts/talker/response_rules.txt")==0)
            script.isGlobal=true;
        SPDLOG_INFO("Currently parsing: {0}", *entryPoint);
        script.parseScript(gameDir,file);
        responseSystems.push_back(script);
        SPDLOG_INFO("Succesfully parsed {0}", *entryPoint);
        file.close();
    }


    //Valve resolves includes by recursively reading files, see CResponseSystem::ParseInclude for details
    //This method has also implicit include guards


}
void RRParser::dumpSceneNames()
{
    std::vector<BSPParser::Map_Scene> scenes;
    scenes.reserve(65535);
    for (const auto& rrSys: responseSystems) {
        for (const auto& rGroup: rrSys.responseGroups) {
            for (const auto& response: rGroup.responses) {
                if (response.type == RRParser::EResponseType::SCENE) {
                    std::string responseVCD = response.typeParam;
                    if (responseVCD.find("$gender")!=std::string::npos) {
                        std::string maleVCD,femaleVCD;
                        maleVCD = femaleVCD = responseVCD;
                        Helper::ReplaceAll(maleVCD,"$gender","male");
                        Helper::ReplaceAll(femaleVCD,"$gender","female");
                        BSPParser::Map_Scene maleScene(maleVCD.c_str()),femaleScene(femaleVCD.c_str());
                        scenes.emplace_back(maleScene);
                        scenes.emplace_back(femaleScene);
                    } else {


                        BSPParser::Map_Scene scene(responseVCD.c_str());
                        scenes.emplace_back(scene);
                    }
                }
            }
        }
    }
    scenes.shrink_to_fit();
    SPDLOG_INFO("Found {0} scene file names from Response System!", scenes.size());
    BSPParser::Scenes.emplace("responseSystem",scenes);

}

void RRParser::stripQuotes(std::string& quoted)
{
    if (quoted[0] == '\"' && quoted[quoted.length()-1]=='\"') {
        quoted = quoted.substr(1,quoted.length()-2);
    }

}



void RRParser::CResponseRulesScript::parseScript(std::string gamedir,std::ifstream &file)
{
    using namespace boost; //for tokenizer
    for (std::string line;safeGetline(file,line);) {
        //nuke the comments
        line = line.substr(0,line.find("//"));
        char_separator<char> sep(" \t");

        tokenizer<char_separator<char>> tokens(line,sep);
        auto tok_it= tokens.begin();

        if(tok_it!=tokens.end()) {
            if (tok_it.current_token().compare("#include")==0) {
                //next token is the included file...
                std::string includedFile = (++tok_it).current_token();
                stripQuotes(includedFile);
                includedFile = gamedir+"/scripts/"+includedFile;
                if (std::find(includedFiles.begin(),includedFiles.end(),includedFile)==includedFiles.end()) {

                    std::ifstream includedStream(includedFile);
                    parseScript(gamedir,includedStream);
                    includedFiles.push_back(includedFile);
                    includedStream.close();
                }
                //that's the line!

            }
            if (boost::iequals(tok_it.current_token(), "enumeration")) {
                //next token is enum's name.
                std::string enumName = (++tok_it).current_token();
                stripQuotes(enumName);
                CScriptEnumeration addedEnum(enumName);
                addedEnum.parseEnum(file);
                enums.push_back(addedEnum);

                continue; //we're long past tokenized line (getline state is saved to the stream).
            }

            if(boost::iequals(tok_it.current_token(), "response")) {
                // quite a lot of parsing in a single line...
                //back up line tokens just in case
                std::vector<std::string> responseTokens(tokens.begin(),tokens.end());

                // anyways next token is the groupname
                std::string responseGName = (++tok_it).current_token();
                stripQuotes(responseGName);
                bool isOneLiner = false;
                CScriptResponseGroup rGroup(responseGName);

                //one member group is a oneliner, so check if we have pending tokens

                if (std::next(tok_it) != tokens.end()) {
                    isOneLiner = true;
                }
                if (isOneLiner) {
                    //if so parse Response flags before response proper.
                    //prepare flags partition

                    std::vector<std::string> flagsStrings(responseTokens.begin()+2,responseTokens.end()-2);
                    CScriptResponse response;
                    response.parseFlags(flagsStrings);
                    std::vector<std::string> typeStrings(responseTokens.end()-2,responseTokens.end()); // two last tokens.
                    response.parseType(typeStrings);
                    rGroup.responses.push_back(response);
                    continue; //parse new line.
                } else {
                    //we're in end of the line. Check for next Line until we hit { or root token. If root token was found step back one line
                    //TODO: multiple lines and null response handling
                    rGroup.parseResponseGroup(file);
                }
                responseGroups.push_back(rGroup);
            }

            if(boost::iequals(tok_it.current_token(), "criterion")) {
                //that's a one liner too.
                //make vector of tokens then.
                std::vector<std::string> criterionTokens(tokens.begin(),tokens.end());
                // next token is the name;
                std::string name = criterionTokens[1];
                stripQuotes(name);
                CScriptCriterion criterion(name);
                auto criteronFlags(std::vector<std::string>(criterionTokens.begin()+2,criterionTokens.end()));
                criterion.parseCriterion(criteronFlags);
                criteria.push_back(criterion);
                continue;
            }

            if(boost::iequals(tok_it.current_token(), "rule")) {
                std::string ruleName = (++tok_it).current_token();
                CScriptRule rule(ruleName);
                rule.parseRule(file);
                rules.push_back(rule);
            }

            //tok_it++; //TODO: Is this needed?
        }


    }

}

void RRParser::CScriptEnumeration::parseEnum(std::ifstream &file)
{
    using namespace boost; //tokenizer again :)

    for (std::string line; safeGetline(file,line);) {
        line = line.substr(0,line.find("//"));
        char_separator<char> sep(" \t");

        tokenizer<char_separator<char>> tokens(line,sep);
        // we should expect 2 tokens at most


        std::vector<std::string> tokenList(tokens.begin(),tokens.end()); //we should have list of tokens here

        if(tokenList[0]=="{")
            continue; //start of the enum
        if(tokenList[0]=="}")
            break; //end of the enum

        //We're in keyvalues now

        std::string key = tokenList[0],value = tokenList[1];
        stripQuotes(key);stripQuotes(value);
        KV.emplace(key,value);


    }

    return;

}

void RRParser::CScriptResponseGroup::parseResponseGroup(std::ifstream &file)
{
    using namespace boost;
    auto oldPointer = file.tellg();


    bool foundBlock=false;
    for(std::string line; safeGetline(file,line);) {
        line = line.substr(0,line.find("//"));
        char_separator<char> sep(" \t");

        noRepeats = false; // this piece of code is here since default value of noRepeats is true.

        tokenizer<char_separator<char>> tokens(line,sep);
        // we should expect 2 tokens at most
        std::vector<std::string> tokenList(tokens.begin(),tokens.end()); //we should have list of tokens here



        if (tokenList.size()>0) {
            if(!foundBlock && RRParser::CResponseRulesScript::isRootToken(tokenList[0])) {
                file.seekg(oldPointer);
                return; //Force back the pointer and relinquish our control.
            }
            if (tokenList[0]=="{") {
                //that's our block! update the state.
                foundBlock=true;
            }

            if (tokenList[0]=="}") {
                //we are done.
                break;

            }

            //group-wide flags

            if(tokenList[0]=="permitrepeats"){
                permitRepeats=true;
            }

            if (tokenList[0]=="norepeat"){
                noRepeats=true;
            }

            if (tokenList[0]=="sequential") {
                sequential=true;
            }


            if (tokenList.size()>1) {
                CScriptResponse response;
                std::vector<std::string> typeStrings(tokenList.begin(),tokenList.begin()+2);
                std::vector<std::string> flagStrings(tokenList.begin()+2,tokenList.end());
                response.parseType(typeStrings);
                response.parseFlags(flagStrings);
                responses.push_back(response);
            }
            if (!foundBlock){
                oldPointer = file.tellg();//keep searching
            }

        }
        }
}






void RRParser::CScriptResponse::parseFlags(std::vector<std::string> &flags)
{
    for (auto token=flags.begin();token!=flags.end();token++) {
        stripQuotes(*token); //does this change element in place? if so are current iterators still valid?
        //delay
        if (token->compare("nodelay")==0) {
            this->delayStart = this->delayEnd=0;
        } else if (token->compare("defaultdelay")==0) {
            this->delayStart = DEF_MIN_DELAY;this->delayEnd = DEF_MAX_DELAY;
        } else if (token->compare("delay")==0) {
            ++token;// next token is the range.
            std::string range = *token;
            stripQuotes(range);
            std::string rangeBegin = range.substr(0,range.find(','));
            std::string rangeEnd = range.substr(range.find(',')+1);
            this->delayStart = std::stof(rangeBegin);this->delayEnd = std::stof(rangeEnd);

        } else if (token->compare("speakonce")==0) {

            this->speakOnce = true;
        } else if (token->compare("noscene")==0) {
            //TODO: check if scene is asked to bypass scene system. Meaning scene type with noscene flag
            this->bypassScene = true;
        } else if (token->compare("odds")==0) {
            ++token;
            std::string odds = *token;
            stripQuotes(odds);
            this->odds = std::stoi(odds);
        } else if (token->compare("respeakdelay")==0) {
            ++token;// next token is the range.
            std::string range = *token;
            stripQuotes(range);
            std::string rangeBegin = range.substr(0,range.find(','));
            std::string rangeEnd = range.substr(range.find(',')+1);
            this->respeakDelayStart = std::stof(rangeBegin);this->respeakDelayEnd = std::stof(rangeEnd);

        } else if (token->compare("soundlevel")==0) {
            ++token;
            std::string soundlvl = *token;
            stripQuotes(soundlvl);
            this->soundlevel = soundlvl;
        }
        // first check if element is forcibly shoved to head or tail. Those flags ignore weight.
        else if (token->compare("displayfirst")==0) {
            this->displayFirst = true;
       } else if (token->compare("displaylast")==0) {
            this->displayLast = true;
       }
        // weight
        else if (token->compare("weight")==0) {
            ++token;
            std::string weight = *token;
            stripQuotes(weight);
            this->weight = std::stof(weight);
        }
    }

}

void RRParser::CScriptResponse::parseType(std::vector<std::string> &types)
{
    std::string type = types[0],argument = types[1];
    stripQuotes(type);stripQuotes(argument);
    this->typeParam=argument;

    if (type=="speak") {
        this->type = EResponseType::SPEAK;
    }if (type=="sentence") {
        this->type = EResponseType::SENTENCE;
    }if (type=="scene") {
        this->type = EResponseType::SCENE;
    }if (type=="response") {
        this->type = EResponseType::RESPONSE;
    }if (type=="print") {
        this->type = EResponseType::PRINT;
    }
}

void RRParser::CScriptRule::parseRule(std::ifstream &file)
{
    using namespace boost; //tokenizer again :)

    for (std::string line; safeGetline(file,line);) {
        line = line.substr(0,line.find("//"));
        char_separator<char> sep(" \t");

        tokenizer<char_separator<char>> tokens(line,sep);


        std::vector<std::string> tokenList(tokens.begin(),tokens.end()); //we should have list of tokens here

        if (tokenList.size()>0) {
            if(tokenList[0]=="{")
                continue; //start of the rule
            if(tokenList[0]=="}")
                break; //end of the rule

            //We're in rule definiton now

            if(tokenList[0]=="criteria") {
                for (auto token=tokenList.begin()+1;token!=tokenList.end();++token) {
                    std::string name = *(token);
                    stripQuotes(name);
                    referencedCriteria.push_back(name);
                }

            } else if(tokenList[0]=="response") {
                for (auto token=tokenList.begin()+1;token!=tokenList.end();++token) {
                    std::string name = *(token);
                    stripQuotes(name);
                    referencedResponses.push_back(name);
                }
            } else if(tokenList[0]=="matchonce") {
                triggerOnce=true;
            } else {
                CScriptCriterion criterion;
                criterion.parseCriterion(tokenList);
                anonymousCriteria.push_back(criterion);
            }

        }


    }

    return;
}

void RRParser::CScriptCriterion::parseCriterion(std::vector<std::string> &flags)
{
    // matchKey and Match value in that order.

    std::string matchKey = flags[0],matchValue = flags[1];
    stripQuotes(matchKey);stripQuotes(matchValue);
    this->matchKey=matchKey;this->matchValue=matchValue;
    std::vector optionals(flags.begin()+2,flags.end());
    for(auto token = optionals.begin();token!=optionals.end();token++) {
        std::string flag = *token;
        stripQuotes(flag);
        if (flag.compare("weight")==0) {
            this->weight = std::stof(*(++token));
        }
        if (flag.compare("required")==0) {
            this->required=true;
        }
    }
}
