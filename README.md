# OUTDATED
## Even if outdated this mod can still be a source of how to do some basic things in dayz modding.

# DayZ Black Raven Mod
This mod is used on DayZ-SA server owned by Black Raven Community.

## Features
- No stamina consumption while sprint/jump and no thirst/hunger
- Log kill/death in a json file (checkout http://black-raven.fr/dayz)
- Kill feed per session in chat game

Other features are done in the init.c file of the mission like set max water and food, define different loadouts, spawns...

### Server setup
- Fresh install dayz 0.63 server
- copy the BlackRavenDM folder into the root of server folder
- edit the serverDZ.cfg file with your own server preferences
- Run server with *-mod=BlackRavenDM* (check BlackRavenDMDayzMod.bat for a debug start/stop/wipe setup)

## Credits
- Dayz Modders discord
- DannyDog for the kill feed
- DaOne for the chat AdminTool
