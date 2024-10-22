#!/usr/bin/python
#coding=utf-8

#################################################################################
# desc 同步资源到CDN和资源服务器
#################################################################################

import sys,os,commands
import struct,stat,time,json,base64#,pysvn
import time
import json
from optparse import OptionParser
from cdn import *

# 切换到文件所在目录
os.environ['LANG']="zh_CN.utf8"
os.chdir(os.path.abspath(sys.path[0]))

need_sync = True

CDN_FILE_PATH = ""

# 将本次更新的所有文件同步到资源服务器
def HandleSync2Server(area_lang_list, lang_list):
    os.system("echo 将本次更新的所有文件同步到资源服务器")

    res_server_info = get_res_server_info()
    rsync_info = get_rsync_info()

    # 同步resource(不含脚本、配置、ui_res/language)
    os.system("rsync -rptvz --progress --delete --port=%s --exclude='.svn/' --exclude='resource/script/' --exclude='resource/language/' --exclude='resource/ui_res/language/' --password-file=%s %sresource %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))

    # 同步ui_res/language
    for lang in lang_list:
        os.system("rsync -rptvz --progress --delete --port=%s --exclude='.svn/' --password-file=%s %sresource/ui_res/language/%s %s@%s::%s/resource/ui_res/language/"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], lang, rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))
    pass    

    # 同步脚本
    os.system("rsync -rptvz --progress --delete --port=%s --exclude='.svn/' --password-file=%s %sresource/script %s@%s::%s/resource"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))        

    # 同步配置
    for arena_lang in area_lang_list:
        os.system("rsync -rptvz --progress --delete --port=%s --exclude='.svn/' --exclude='~config_md5' --exclude='~config/' --exclude='~config_beta/' --exclude='~config_precompile/' --password-file=%s %sresource/language/%s %s@%s::%s/resource/language"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], arena_lang, rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))
    pass    

    # 同步clientcppversioin
    os.system("rsync -rptvz --progress --delete --port=%s --password-file=%s %sclientcppversioin %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))
    
    # 同步version.ios、version_md5
    if need_sync or rsync_info['force_all']:
        os.system("rsync -rptvz --progress --delete --port=%s --password-file=%s %sversion.ios %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty']))
        os.system("rsync -rptvz --progress --delete --port=%s --password-file=%s %sversion_md5 %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path_ty'])) 
    pass
pass

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-l", "--lang_list",              type="string",      dest="op_lang_list",            action="store",     help="必要参数，允许的语言列表")
    parser.add_option("-c", "--area_lang_list",         type="string",      dest="op_area_lang_list",       action="store",     help="必要参数，允许的地区语言列表")
    parser.add_option("-p", "--cdn_path",               type="string",      dest="op_cdn_path",             action="store",     help="必要参数，cdn路径")

    (options, args) = parser.parse_args()
    if options.op_lang_list == None :
        parser.error("缺少必要参数 lang_list") 
        pass
    elif options.op_area_lang_list == None :
        parser.error("缺少必要参数 area_lang_list") 
        pass
    elif options.op_cdn_path == None :
        parser.error("缺少必要参数 p") 
        pass    

    global CDN_FILE_PATH
    CDN_FILE_PATH = os.path.abspath(options.op_cdn_path)        

    setup_config(CDN_FILE_PATH)

    # 将本次更新的所有文件同步到资源服务器
    HandleSync2Server(options.op_area_lang_list.split('#'), options.op_lang_list.split('#'))
pass

if __name__ == "__main__":
    Handle()
    pass

