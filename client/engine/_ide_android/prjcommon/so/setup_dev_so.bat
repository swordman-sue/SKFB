@echo off
setlocal ENABLEDELAYEDEXPANSION

set proj_path=%cd%

echo ------------setup dev_so-----------------
set src_so=%proj_path%\..\..\out_develop\obj\local\armeabi-v7a\libskyx.so
set dest_so=%proj_path%\..\..\..\libs\android\debug
copy %src_so% %dest_so%

set src_so=%proj_path%\..\..\out_develop\libs\armeabi-v7a\libskyx.so
set dest_so=%proj_path%\dev_so\armeabi-v7a\libskyx.so
copy %src_so% %dest_so%

echo ------------copy dev_so to all-----------------
set src_so=%proj_path%\dev_so\armeabi-v7a\libskyx.so
set distri_so=%proj_path%\distri_so\armeabi-v7a\libskyx.so
copy %src_so% %distri_so%

call copy_distri_so.bat
