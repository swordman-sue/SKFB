@echo off  

set svnpath="E:/xampp/htdocs/skfb/console_develop/client_config" 
cd %svnpath% 
svn update  
svn commit -m "ʹ��������ͻ����������ļ�"

set svnpath="E:/xampp/htdocs/skfb/console_develop/server_config" 
cd %svnpath%  
svn update 
svn commit -m "ʹ��������·����������ļ�"

pause  