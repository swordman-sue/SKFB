@echo off

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/CDNTools/create_script_and_version.py -l chs -c dl_chs -u admin -m 0 -s 0 -p ../node_js/skfb/public/develop/cdnfiles -y ../node_js/skfb/public/develop/cdnfiles_sync"
pause>nul