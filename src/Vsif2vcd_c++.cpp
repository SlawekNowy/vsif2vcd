    // Vsif2vcd_c++.cpp: definiuje punkt wejścia dla aplikacji.
    //
#include <filesystem>
#include <set>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
    #include "program.h"

    #include "VSIF.hpp"
#include "BVCD.hpp"
    #include "response_system.h"

    #include "hardcoded_entries.h"




    using namespace std;



    int main(int argc, char* argv[]){
        //TODO: PROFILE!
        int ExitCode;
        puts("VSIF2VCD version 2.0 (23 Apr 2012)");

        if (argc < 2|| argc > 2)
        {
            cerr << "By http://steamcommunity.com/id/SiPlus\nUsage: VSIF2VCD [game directory]\n";
            return 1;
        }
        spdlog::set_pattern("[%L] %!: %v");

        auto console = spdlog::stdout_logger_mt("console");
        spdlog::set_default_logger(console);
        //puts("Initializing LZMA uncompression module");
        ExitCode = Program::doStart(argv[1]);
        return ExitCode;
    }






    CGameInfo Program::gi;
    std::map<std::string,std::vector<BSPParser::Map_Scene>> BSPParser::Scenes;
    VSIF::ValveScenesImageFile* Helper::vsif;



    //VSIF::ValveScenesImageFile* Helper::vsif;
    int Program::doStart(std::string gameDir) {

        using std::filesystem::path;
        using std::filesystem::canonical;
        path gameDirPath(gameDir);
        auto absGameDir = canonical(gameDirPath);
        SPDLOG_INFO("Full gameDirPath is {}", absGameDir.string());
        

        //TODO: Response rules parsing.
        //Also static names of scenes added by source code itself
        gi = CGameInfo(gameDir);
        gi.initializeFileSystem();
        std::string tmpDir;
        gi.prepareTmpDirectory(tmpDir);
        VSIF::ValveScenesImageFile vsif = VSIF::ValveScenesImageFile(tmpDir + "/scenes/scenes.image");
        Helper::vsif = &vsif;
        Helper::vsif->fillWithVCDS();

        SPDLOG_INFO("Now extracting scene names from maps.");
    //BSPParser and RRParser should be easy to run at the same time
        BSPParser::ExtractNames(tmpDir);
    //#if 0
        SPDLOG_INFO("Now extracting scene names from response system.");
        RRParser::initRules(tmpDir);
        RRParser::dumpSceneNames();

        //TODO: Wait for the threads

    //#endif
        appendHardCodedEntries();


        std::vector<BSPParser::Map_Scene> sceneSoup;

        for (auto& pair:BSPParser::Scenes) {

            std::copy(pair.second.begin(),pair.second.end(),std::inserter(sceneSoup,sceneSoup.end()));
        }

        SPDLOG_INFO("Now decompiling...");

        for (int i=0;i<Helper::vsif->vcds.size();i++) {
            VSIF::VSIF_Entry entry = Helper::vsif->entries[i];
            std::string strDump = Helper::vsif->vcds[i].dumpText();
            // now find the path this crc is refering to.
            auto strEntryIter = std::find_if(sceneSoup.begin(),sceneSoup.end(),[&entry](const BSPParser::Map_Scene& element){
                return entry.CRC==element.CRC;
            });
            std::string targetPathStr = tmpDir;
            if (strEntryIter!=sceneSoup.end()) {
                SPDLOG_INFO("CRC ({1:#08X}) hit! Path is {0}",strEntryIter->Name,entry.CRC);
                std::string targetFile = strEntryIter->Name;
                targetPathStr = tmpDir+targetFile;
            } else {
                SPDLOG_INFO("CRC ({0:#08X}) miss!");
                std::string targetFile = "/_failed/"+std::to_string(entry.CRC)+".vcd";
                targetPathStr = tmpDir+targetFile;
            }

            std::filesystem::path targetPath(targetPathStr);
            auto targetDir = targetPath;
            targetDir = targetDir.remove_filename();
            std::filesystem::create_directories(targetDir);
            std::ofstream outputFile(targetPath);
            outputFile << strDump;
            outputFile.flush();
            outputFile.close();
        }
        Helper::vsif = nullptr;
        return 0;


    }

    void Program::appendHardCodedEntries()
    {
        std::vector<BSPParser::Map_Scene> hardcodedScenes;
        hardcodedScenes.reserve(hardcodedEntries.size());
        for (auto iter = hardcodedEntries.begin(); iter != hardcodedEntries.end(); ++iter) {
            hardcodedScenes.emplace_back((char*)iter->c_str());
        }
        SPDLOG_INFO("Appended {0} hardcoded entries.", hardcodedEntries.size());
        BSPParser::Scenes.emplace("hardcoded",hardcodedScenes);
    }

