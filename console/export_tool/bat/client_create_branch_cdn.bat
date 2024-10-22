@echo off

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/CDNTools/create_branch.py -s client/cdnfiles/resource -d branches/xxxx/client/cdnfiles/resource -p ../node_js/skfb/public/xxxx -m 2 -a dl_chs"
pause>nul