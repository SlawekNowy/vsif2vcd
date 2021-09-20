

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


#include "response_system.h"
#include <filesystem>


std::vector<std::string> RRParser::entryPointsToParse = {"scripts/talker/response_rules.txt"} ;//path hardcoded. Why vector? we might have multiple files to parse;
std::vector<RRParser::CResponseRulesScript> responseSystems;
void RRParser::initRules(std::string gameDir)
{
    std::filesystem::create_directory(gameDir+"tmp_rr"); //tmp directory for script parsing.

    for (auto entryPoint = entryPointsToParse.begin();entryPoint != entryPointsToParse.end();++entryPoint) {
        std::ifstream file(entryPoint.base()->c_str());
        CResponseRulesScript script;
        script.parseScript(file);
        responseSystems.push_back(script);
        file.close();
    }


    //Valve resolves includes by recursively reading files, see CResponseSystem::ParseInclude for details
    //This method has also implicit include guards


}
void RRParser::dumpSceneNames()
{

}

void RRParser::CResponseRulesScript::stripQuotes(std::string &quoted)
{
    if (quoted[0] == '\"' & quoted[quoted.length()]=='\"') {
        quoted = quoted.substr(1,quoted.length()-1);
    }

}

void RRParser::CResponseRulesScript::parseScript(std::ifstream &file)
{
    using namespace boost; //for tokenizer
    for (std::string line;std::getline(file,line);) {
        //nuke the comments
        line = line.substr(0,line.find("//"));
        char_separator<char> sep(" ");

        tokenizer<char_separator<char>> tokens(line,sep);
        auto tok_it= tokens.begin();

        if(tok_it!=tokens.end()) {
            if (tok_it.current_token().compare("#include")) {
                //next token is the included file...
                std::string includedFile = (++tok_it).current_token();
                stripQuotes(includedFile);
                if (std::find(includedFiles.begin(),includedFiles.end(),includedFile)!=includedFiles.end()) {

                    std::ifstream includedStream(includedFile);
                    parseScript(includedStream);
                    includedFiles.push_back(includedFile);
                    includedStream.close();
                }

            }
            if (tok_it.current_token().compare("enumeration")) {
                //next token is enum's name.
            }
        }

    }

}
