@echo off  

set svnpath="E:\xampp\htdocs\skfb\console_develop\client_config" 
cd %svnpath%  
svn add * --force
svn ci -m "添加客户端配置文件"

set svnpath="E:\xampp\htdocs\skfb\console_develop\server_config" 
cd %svnpath%  
svn add * --force
svn ci -m "添加服务器配置文件"

pause  