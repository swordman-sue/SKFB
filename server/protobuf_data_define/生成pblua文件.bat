@echo off

for %%i in (*.proto) do (
..\gen-lua-pb\protoc.exe --plugin=protoc-gen-lua="..\gen-lua-pb\protoc-gen-lua\plugin\protoc-gen-lua.bat" %%i --lua_out=.\
)

pause