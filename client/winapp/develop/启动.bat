
::set cur_path=%cd%
::set cdn_path=%cur_path%\..\..\cdnfiles
::set script_path=%cur_path%\..\..\script
::TortoiseProc.exe /command:update  /path:%cur_path% /closeonend:1
::TortoiseProc.exe /command:update  /path:%cdn_path% /closeonend:1
::TortoiseProc.exe /command:update  /path:%script_path% /closeonend:1

cd sdz_app
start ../release/game.exe