@echo off
setlocal ENABLEDELAYEDEXPANSION

set proj_path=%cd%
set src_apk=skfb-quickpp-dev.apk
set com_apk=%proj_path%\..\..\..\pkgs\com-szba-skfb-quickpp-dev.apk

E:
cd E:\AndroidDev\android-sdk-eclipse\build-tools\25.0.3
del %com_apk%
zipalign -v -f 4 %proj_path%\bin\%src_apk% %com_apk%
TortoiseProc.exe /command:commit  /path:%com_apk% /closeonend:1

pause