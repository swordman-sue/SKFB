#!/usr/bin/python
#coding=utf-8

import sys, os, shutil

root_path = os.path.dirname(os.path.abspath(sys.path[0]))
src_symbol_list_file="%s/prjcommon/dmhxGOD_func.list"%root_path
dst_symbol_list_file="%s/prjcommon/dmhxGOD.h"%root_path

def Handle():
    src_symbol_list_fo = open(src_symbol_list_file)
    src_symbol_list = src_symbol_list_fo.readlines()
    src_symbol_list_fo.close()

    dst_symbol_list = []
    dst_symbol_list.append("#ifndef DMHX_GOD_H\n#define DMHX_GOD_H\n")
    for src_symbol in src_symbol_list:
        dst_symbol = os.popen("openssl rand -base64 64 | tr -cd 'a-zA-Z' |head -c 16").read()
        dst_symbol_list.append("#define %s %s\n"%(src_symbol.strip('\r\n'), dst_symbol))
    pass
    dst_symbol_list.append("#endif")
    dst_symbol_list_fo = open(dst_symbol_list_file, 'w')
    dst_symbol_list_fo.writelines(dst_symbol_list)
    dst_symbol_list_fo.close()                               
pass

if __name__ == "__main__":
    Handle()
pass