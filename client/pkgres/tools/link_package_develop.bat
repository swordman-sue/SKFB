@echo off

set targetPath=.\..\..\engine\_ide_android\quickpp\assets\
set resPath=develop

TortoiseProc.exe /command:update  /path:../%resPath% /closeonend:1

xcopy /s/y/i/f ".\..\%resPath%\language_string.cnf" "%targetPath%"
xcopy /s/y/i/f ".\..\%resPath%\start_string.cnf" "%targetPath%"
xcopy /s/y/i/f ".\..\%resPath%\version.ios" "%targetPath%"
xcopy /s/y/i/f ".\..\%resPath%\version.xml" "%targetPath%"
xcopy /s/y/i/f ".\..\%resPath%\zhunyuan.ttf" "%targetPath%"

rd "%targetPath%resource" /s/q
mklink /J "%targetPath%resource" ".\..\%resPath%\resource"

pause