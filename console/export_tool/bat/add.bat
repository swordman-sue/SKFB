@echo off  

set svnpath="E:\xampp\htdocs\skfb\console_develop\client_config" 
cd %svnpath%  
svn add * --force
svn ci -m "��ӿͻ��������ļ�"

set svnpath="E:\xampp\htdocs\skfb\console_develop\server_config" 
cd %svnpath%  
svn add * --force
svn ci -m "��ӷ����������ļ�"

pause  