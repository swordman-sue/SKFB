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

need_sync = False

CDN_FILE_PATH = ""
CDN_FILE_SYNC_PATH = ""

# 0：更新全部，1：更新脚本，2：更新配置，3：更新脚本和配置
SPECIAL_UPDATE_MODE = 0
SUM_UNKNOWN = -1
SUM_ALL = 0
SUM_SCRIPT = 1
SUM_CONFIG = 2
SUM_SCRIPT_AND_CONFIG = 3

# 是否同步脚本
def CanSyncScript():
    return SPECIAL_UPDATE_MODE != SUM_CONFIG
pass

# 是否同步配置
def CanSyncConfig():
    return SPECIAL_UPDATE_MODE != SUM_SCRIPT
pass

# 是否同步资源
def CanSyncResource():
    return SPECIAL_UPDATE_MODE == SUM_ALL
pass

# 检测本次是否有同步的文件并重置待同步列表
def CheckVersionSyncList():
    global need_sync

    version_sync_file = "%s/version_sync_list"%CDN_FILE_SYNC_PATH
    if not os.path.exists(version_sync_file):
        os.mknod(version_sync_file)
    pass    
    version_sync_fo = open(version_sync_file, "r")
    version_sync_list = version_sync_fo.readlines()
    version_sync_fo.close()                                

    need_sync = len(version_sync_list) > 0
pass

# 清空待同步列表
def ClearVersionSyncList():
    version_sync_file = "%s/version_sync_list"%CDN_FILE_SYNC_PATH
    version_sync_fo = open(version_sync_file, "w")
    version_sync_fo.truncate()
    version_sync_fo.close()
pass

# 将本次更新的所有文件同步到资源服务器
def HandleSync2Server(area_lang_list, lang_list, deprecated_mode):
    os.system("echo 将本次更新的所有文件同步到资源服务器")

    res_server_info = get_res_server_info()
    rsync_info = get_rsync_info()
    is_force_sync = rsync_info['force_all']

    # 同步resource(不含脚本、配置、ui_res/language)
    if CanSyncResource() or is_force_sync:        
        os.system("rsync -rptvz --progress %s --port=%s --exclude='.svn/' --exclude='resource/script/' --exclude='resource/language/' --exclude='resource/ui_res/language/' --password-file=%s %sresource %s@%s::%s"%(deprecated_mode, res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
    pass

    # 同步ui_res/language
    if CanSyncResource() or is_force_sync:        
        for lang in lang_list:
            os.system("rsync -rptvz --progress %s --port=%s --exclude='.svn/' --password-file=%s %sresource/ui_res/language/%s %s@%s::%s/resource/ui_res/language/"%(deprecated_mode, res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], lang, rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
        pass    
    pass

    # 同步脚本
    if CanSyncScript() or is_force_sync:
        os.system("rsync -rptvz --progress %s --port=%s --exclude='.svn/' --password-file=%s %sresource/script %s@%s::%s/resource"%(deprecated_mode, res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
    pass

    # 同步配置 
    if CanSyncConfig() or is_force_sync:    
        for arena_lang in area_lang_list:
            os.system("rsync -rptvz --progress %s --port=%s --exclude='.svn/' --exclude='~config_md5' --exclude='~config/' --exclude='~config_beta/' --exclude='~config_precompile/' --password-file=%s %sresource/language/%s %s@%s::%s/resource/language"%(deprecated_mode, res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], arena_lang, rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
        pass    
    pass

    # 同步clientcppversioin
    os.system("rsync -rptvz --progress --port=%s --password-file=%s %sclientcppversioin %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
    
    # 同步version.ios、version_md5
    if need_sync or is_force_sync:
        os.system("rsync -rptvz --progress --port=%s --password-file=%s %sversion.ios %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path']))
        os.system("rsync -rptvz --progress --port=%s --password-file=%s %sversion_md5 %s@%s::%s"%(res_server_info['port'], rsync_info['password_file'], rsync_info['src_path'], rsync_info['user'], res_server_info['ip'], rsync_info['dst_path'])) 
    pass
pass

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-l", "--lang_list",              type="string",      dest="op_lang_list",            action="store",     help="必要参数，允许的语言列表")
    parser.add_option("-c", "--area_lang_list",         type="string",      dest="op_area_lang_list",       action="store",     help="必要参数，允许的地区语言列表")
    parser.add_option("-p", "--cdn_path",               type="string",      dest="op_cdn_path",             action="store",     help="必要参数，cdn路径")
    parser.add_option("-y", "--cdn_sync_path",          type="string",      dest="op_cdn_sync_path",        action="store",     help="必要参数，cdn同步路径")
    parser.add_option("-d", "--deprecated_mode",        type="string",      dest="op_deprecated_mode",      action="store",     help="可选参数，是否移除目标目录中的废弃文件，默认是")

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
    global CDN_FILE_SYNC_PATH
    CDN_FILE_SYNC_PATH = os.path.abspath(options.op_cdn_sync_path)

    # 获取更新模式
    global SPECIAL_UPDATE_MODE
    mode_update_file = "%s/mode_update"%(CDN_FILE_PATH)
    if not os.path.exists(mode_update_file):
        os.mknod(mode_update_file)
    pass
    mode_update_fo = open(mode_update_file, "r")
    mode_update = mode_update_fo.read()
    mode_update_fo.close()
    SPECIAL_UPDATE_MODE = SUM_UNKNOWN if mode_update == '' else int(mode_update)

    # 检测是否更新中
    status_update_file = "%s/status_update"%(CDN_FILE_PATH)
    if not os.path.exists(status_update_file):
        os.mknod(status_update_file)
    pass
    status_update_fo = open(status_update_file, "r")
    status_update = status_update_fo.read()
    status_update = 0 if status_update == '' else int(status_update)
    status_update_fo.close()
    if status_update == 1:
        os.system("echo 资源更新中，请稍后！")
        return
    pass

    # 检查同步目录
    if not os.path.exists(CDN_FILE_SYNC_PATH):
        os.system("echo 找不到同步目录，请检查！")
        return
    pass                

    # 检测是否同步中
    status_sync_file = "%s/status_sync"%(CDN_FILE_SYNC_PATH)    
    if not os.path.exists(status_sync_file):
        os.mknod(status_sync_file)
    pass        
    status_sync_fo = open(status_sync_file, "r+")
    status_sync = status_sync_fo.read()
    status_sync = 0 if status_sync == '' else int(status_sync)
    if status_sync == 1:
        os.system("echo 资源同步中，请稍后！")
        status_sync_fo.close()
        return
    pass

    setup_config(CDN_FILE_PATH)

    # 清空更新模式
    mode_update_fo = open(mode_update_file, "w")
    mode_update_fo.truncate()
    mode_update_fo.close()

    # 记录同步中状态
    status_sync_fo.seek(0, 0)
    status_sync_fo.write("1")
    status_sync_fo.close()

    # 检测本次是否有同步的文件并重置待同步列表
    CheckVersionSyncList()

    # 将本次更新的所有文件同步到资源服务器
    deprecated_mode = "" if options.op_deprecated_mode == 0 else "--delete"
    HandleSync2Server(options.op_area_lang_list.split('#'), options.op_lang_list.split('#'), deprecated_mode)

    # 清空待同步列表 
    ClearVersionSyncList()

    # 还原同步中状态
    status_sync_fo = open(status_sync_file, "w")
    status_sync_fo.write("0")
    status_sync_fo.close()

    os.system("echo 资源同步完毕！\n")    
pass

if __name__ == "__main__":
    Handle()
    pass

