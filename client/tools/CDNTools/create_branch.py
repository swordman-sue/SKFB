#!/usr/bin/python
#coding=utf-8

#################################################################################
# desc 同步资源到CDN和资源服务器
#################################################################################

import sys,os,commands,shutil
import struct,stat,time,json,base64#,pysvn
import time
import json
from optparse import OptionParser

# 切换到文件所在目录
os.environ['LANG']="zh_CN.utf8"
os.chdir(os.path.abspath(sys.path[0]))
svn_root_path="https://192.168.1.111:8443/svn/zhanglang/"

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-s", "--src_svn_path",         type="string",      dest="op_src_svn_path",         action="store",     help="必要参数，源路径(svn路径)")
    parser.add_option("-d", "--dst_svn_path",         type="string",      dest="op_dst_svn_path",         action="store",     help="必要参数，目标路径(svn路径)")
    parser.add_option("-p", "--dst_local_path",       type="string",      dest="op_dst_local_path",       action="store",     help="必要参数，目标路径(本地路径)")
    parser.add_option("-m", "--mode",                 type="int",         dest="op_mode",                 action="store",     help="必要参数，模式")
    parser.add_option("-l", "--lang_list",            type="string",      dest="op_lang_list",            action="store",     help="必要参数，允许的语言列表")
    parser.add_option("-a", "--arena_lang_list",      type="string",      dest="op_arena_lang_list",      action="store",     help="必要参数，允许的区域语言列表")

    (options, args) = parser.parse_args()
    if options.op_src_svn_path == None :
        parser.error("缺少必要参数 src_svn_path") 
        pass
    elif options.op_dst_svn_path == None :
        parser.error("缺少必要参数 dst_svn_path")
        pass
    elif options.op_dst_local_path == None :
        parser.error("缺少必要参数 dst_local_path")
        pass
    elif options.op_mode == None :
        parser.error("缺少必要参数 mode")
        pass
    pass

    src_svn_path = "%s%s"%(svn_root_path, options.op_src_svn_path)
    dst_svn_path = "%s%s"%(svn_root_path, options.op_dst_svn_path)
    dst_local_path = os.path.abspath(options.op_dst_local_path)

    lang_list = None
    if options.op_lang_list != None:
        lang_list = options.op_lang_list.split('#')
    pass

    arena_lang_list = None
    if options.op_arena_lang_list != None:
        arena_lang_list = options.op_arena_lang_list.split('#')
    pass

    # 打script分支
    if options.op_mode == 1:
        os.system("svn up %s/script"%dst_local_path)
	
        # 备份script/language/xxx....
        tmp_path = "%s/tmp"%dst_local_path
        if lang_list != None:
            if os.path.exists(tmp_path):
                shutil.rmtree(tmp_path)
            pass
            os.mkdir(tmp_path)		
            for lang in lang_list:
                shutil.copytree("%s/script/language/%s"%(dst_local_path, lang), "%s/%s"%(tmp_path, lang))
            pass
        pass
        
        os.system("svn del -m '打script分支' %s"%dst_svn_path)
        os.system("svn cp -m '打script分支' %s %s"%(src_svn_path, dst_svn_path))
        os.system("svn up %s/script"%dst_local_path)

        if lang_list != None:
            for lang in lang_list:
                shutil.copytree("%s/%s"%(tmp_path, lang), "%s/script/language/%s"%(dst_local_path, lang))
                os.system("svn add %s/script/language/%s"%(dst_local_path, lang))			
            pass
            os.system("svn ci -m '打script分支' %s/script/language"%dst_local_path)
            shutil.rmtree(tmp_path)
        pass

        os.system("echo 处理完成")

    # 打resource分支
    elif options.op_mode == 2:
        os.system("svn up %s/cdnfiles/resource"%dst_local_path)
        os.system("svn del --force %s/cdnfiles/resource"%dst_local_path)
        os.system("svn ci -m '打resource分支' %s/cdnfiles/resource"%dst_local_path)
        os.system("svn cp -m '打resource分支' %s %s"%(src_svn_path, dst_svn_path))
        os.system("svn up %s/cdnfiles"%dst_local_path)

        if arena_lang_list != None:
            for arena_lang in arena_lang_list:
                config_md5_file = "%s/cdnfiles/resource/language/%s/~config_md5"%(dst_local_path, arena_lang)
                if os.path.exists(config_md5_file):
                    os.remove(config_md5_file)
                pass
            pass
        pass

        os.system("echo 处理完成")
    pass
pass

if __name__ == "__main__":
    Handle()
    pass

