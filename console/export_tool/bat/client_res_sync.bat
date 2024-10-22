@echo off

Set /p var=确定同步到开发服(y/n)？
if /i "%var%"=="n" exit

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/CDNTools/synchronize_to_cdn_and_server.py -l chs -c dl_chs -d 1 -p ../node_js/skfb/public/develop/cdnfiles -y ../node_js/skfb/public/develop/cdnfiles_sync"
pause>nul