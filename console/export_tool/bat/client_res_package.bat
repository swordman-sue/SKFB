@echo off

C:
chdir C:\cygwin64\bin
bash --login -i -c  "python E:/pkgres/skfb/tools/take_package.py -a dl_chs -c E:/node_js/skfb/public/develop/cdnfiles -p develop -s ../startres/skfb/develop"
pause>nul