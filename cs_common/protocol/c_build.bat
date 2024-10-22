::===================================
::   蟑螂科技
::   Swordman Sue
::   2018/10/15 10:13
::===================================

cd cc_proto_parser

::生成协议号定义文件
ProtoParser.exe 2

cd ../

::生成pb数据文件
protoc.exe --proto_path=cs_proto cs_proto/pbhead.proto --descriptor_set_out=../../client/cdnfiles/resource/pb/pbhead.pb
protoc.exe --proto_path=cs_proto cs_proto/pblogin.proto --descriptor_set_out=../../client/cdnfiles/resource/pb/pblogin.pb

pause 