@echo off  

set svnpath="E:/xampp/htdocs/skfb/console_develop/client_config" 
cd %svnpath% 
svn update  
svn commit -m "使用命令更客户端新配置文件"

set svnpath="E:/xampp/htdocs/skfb/console_develop/server_config" 
cd %svnpath%  
svn update 
svn commit -m "使用命令更新服务器配置文件"

pause  