@ECHO OFF

set svnpath="E:/xampp/htdocs/skfb/console_develop/server_config"
cd %svnpath%  
svn update 

::&echo 服务器配置更新完成

XCOPY E:\xampp\htdocs\skfb\console_develop\server_config\* E:\svn\bin\GameSvr\DBC /s  /e /y

mysql -h192.168.1.111 -p3306 -uroot -pdebug123456 skfb< E:\xampp\htdocs\skfb\console_develop\export_tool\bat\add_gm_event_sql.txt

::暂停
pause
