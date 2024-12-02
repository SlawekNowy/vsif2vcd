# vsif2vcd

This utility initially written by SiPlus allows for decompiling scenes.image file to original VCD faceposer files.
The expectations are to run on any computer that HLLib supports.


# TODO
- [X] Make LZMALib static and use its API instead.
- [X] Parse scripts/talker/response_rules.txt file to get scene values
- [X] SteamPipe update brought necessary files packed. Unpack those via HLLib.
- [X] We do not wanna dirty the game directory. Make temporary directory, where we do all work.
- [ ] Try to use sourcepp.
- [ ] TF2 item parsing.
- [ ] Due to gameinfo.txt mounting order, and way those files are mounted, scenes.image file contains compound data for all directories mounted. This is problematic beacuse only top mod directory has maps. Most of those games have Half-Life 2 scenes. Episode Two has additionally Episode One scenes. We need to resolve this.


# Source for reference
Reference source is at src/old/Vsif2vcd.c
