@echo off

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/CDNTools/create_branch.py -s client/script -d branches/xxxx/client/script -p ../node_js/skfb/public/xxxx -m 1 -l chs"
pause>nul