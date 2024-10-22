@echo off

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/ChineseExtract/fanyi2excel.py -a dl -l chs -m 2 -s ../node_js/skfb/public/develop -d ../node_js/skfb/public/develop"

pause>nul