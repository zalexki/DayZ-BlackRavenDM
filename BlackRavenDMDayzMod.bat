@echo off
set PBOManager="C:\Program Files\PBO Manager v.1.4 beta\PBOConsole.exe"
title BlackRavenDM DayZ-Server Launcher
cls

D:
CD "D:\Program Files (x86)\Steam\steamapps\common\DayZServer"

pause

:reboot

echo Killing DayZServer_x64.exe
taskkill /F /IM DayZServer_x64.exe /T

timeout /t 1

echo Delete mission database
del /Q /S "D:\Program Files (x86)\Steam\steamapps\common\DayZServer\mpmissions\dayzOffline.chernarusplus\storage_1"

echo PBO the mod
%PBOManager% -pack "D:\Program Files (x86)\Steam\steamapps\common\DayZServer\BlackRavenDM\Addons\BlackRavenDM" "D:\Program Files (x86)\Steam\steamapps\common\DayZServer\BlackRavenDM\Addons\BlackRavenDM.pbo"

timeout /t 1

echo start server
start DayZServer_x64.exe -mod=BlackRavenDM -config=serverDZ.cfg -adminlog -dologs -freezecheck -scriptDebug=true -name=BlackRavenDM -port=2302

Echo Waiting for server kill

pause

goto reboot
