#!/usr/bin/python
#coding=utf-8

#################################################################################
# desc 创建脚本压缩包、生成资源版本信息表
#################################################################################
import sys,os,commands,shutil
import struct,stat,time,json,base64#,pysvn
import hashlib
from optparse import OptionParser

# 脚本打包文件分布表
SCRIPT_FILE_DIC = {
    "script0.ios":["configadapter/*", ""],
    "script1.ios":["core/* luatools/* cpptools/*", ""],
    "script2.ios":["network/*", ""],
    "script3.ios":["lgui/* gui/*", ""],
    "script4.ios":["resource/*", ""],
    "script5.ios":["macros/*", ""],
    "script6.ios":["language/*", ""],
    "script7.ios":["modules/*", ""],
    "script8.ios":["main.lua startup.lua game.lua", ""],
}

# 脚本压缩包更新记录表
SCRIPT_FILE_UPDATE_DIC = {}
for script_ios_file, match_info in SCRIPT_FILE_DIC.iteritems():
    match_list = match_info[0].split(" ")

    update_info = {}
    update_info["need_update"] = False
    update_info["match_list"] = []
    for match_info in match_list:
        if match_info.endswith(".lua"):
            update_info["check_as_file"] = True
            update_info["match_list"].append(match_info)
        else:
            real_match_info = match_info.rstrip("*")
            update_info["match_list"].append("script/%s"%real_match_info)
        pass
    pass
    SCRIPT_FILE_UPDATE_DIC[script_ios_file] = update_info
pass
#################################################################################

# 切换到文件所在目录
os.environ['LANG']="zh_CN.utf8"
os.chdir(os.path.abspath(sys.path[0]))

TOOLS_PATH = os.path.abspath(sys.path[0])
LUAC_PATH = "%s/luac.exe"%TOOLS_PATH

CDN_FILE_PATH = ""
CDN_FILE_SYNC_PATH = ""
RESOURCE_PATH = ""

# 0：更新全部，1：更新脚本，2：更新配置，3：更新脚本和配置
SPECIAL_UPDATE_MODE = 0
SUM_UNKNOWN = -1
SUM_ALL = 0
SUM_SCRIPT = 1
SUM_CONFIG = 2
SUM_SCRIPT_AND_CONFIG = 3

HAS_COMPILE_SCRIPT = False

# 是否更新脚本
def CanUpdateScript():
    return SPECIAL_UPDATE_MODE != SUM_CONFIG
pass

# 是否更新配置
def CanUpdateConfig():
    return SPECIAL_UPDATE_MODE != SUM_SCRIPT
pass

# 是否更新该文件
def CheckCanUpdateFile(file_path):
    if SPECIAL_UPDATE_MODE == SUM_ALL:
        return True
    pass

    if SPECIAL_UPDATE_MODE == SUM_SCRIPT:
        return file_path.startswith("resource/script/")
    
    elif SPECIAL_UPDATE_MODE == SUM_CONFIG:
        return file_path.endswith(".zcnf")

    elif SPECIAL_UPDATE_MODE == SUM_SCRIPT_AND_CONFIG:
        return file_path.startswith("resource/script/") or file_path.endswith(".zcnf")
    pass

    return False
pass

# 获取更新模式描述
def GetUpdateModeDesc(mode_update):
    if mode_update == SUM_ALL:
        return "全部"
    elif mode_update == SUM_SCRIPT:
        return "脚本"
    elif mode_update == SUM_CONFIG:
        return "配置"
    elif mode_update == SUM_SCRIPT_AND_CONFIG:
        return "脚本和配置"
    pass
pass

# 编译客户端脚本
# script_compile_mode：0 debug(带调试信息、行号) 1 release(不带调试信息、行号)
need_update_scripts_md5 = False
def CompileScript(script_path, script_path_precompile, script_compile_mode, windows_cdn_path):
    if not CanUpdateScript():
        return
    pass

    # 更新脚本
    os.system("svn up %s"%script_path)

    if not os.path.exists(script_path_precompile):
        os.mkdir(script_path_precompile)
    pass                

    # 读取已编译脚本的md5
    scripts_md5_file = "%s/scripts_md5"%script_path_precompile
    if not os.path.exists(scripts_md5_file):
        os.mknod(scripts_md5_file)
    pass
    scripts_md5_fo = open(scripts_md5_file)
    scripts_md5_list = scripts_md5_fo.readlines()
    scripts_md5_fo.close()
    scripts_md5_map = {}
    for md5_str in scripts_md5_list:
        md5_info = md5_str.split('|')
        scripts_md5_map[md5_info[0]] = md5_info[1].rstrip('\n')
    pass

    global need_update_scripts_md5
    need_update_scripts_md5 = False
    
    global HAS_COMPILE_SCRIPT	
    HAS_COMPILE_SCRIPT = False
    def CompileScriptReal(src_path, dst_path):
        if src_path.find(".svn") != -1:
            return
            
        if not os.path.exists(dst_path):
            os.mkdir(dst_path)
        pass                

        for parent_path, dir_names, filenames in os.walk(src_path):
            if parent_path.find(".svn") != -1:
                continue

            dst_parent_path = parent_path.replace("/script", "/script_precompile")

            # 处理文件
            for file_name in filenames:
                src_file_path = os.path.join(parent_path, file_name)
                src_file_key = src_file_path.replace("%s/../"%CDN_FILE_PATH, "")

                # 计算脚本文件的MD5
                with open(src_file_path, 'rb') as fp:
                    data = fp.read()
                script_md5 = hashlib.md5(data).hexdigest()

                if not scripts_md5_map.has_key(src_file_key) or scripts_md5_map[src_file_key] != script_md5:
                    dst_file_path = os.path.join(dst_parent_path, file_name)

                    # 转换为windows下的文件路径
                    src_file_path = src_file_path.replace(CDN_FILE_PATH, windows_cdn_path)
                    dst_file_path = dst_file_path.replace(CDN_FILE_PATH, windows_cdn_path)

                    global HAS_COMPILE_SCRIPT 					 
                    if not HAS_COMPILE_SCRIPT:
                        HAS_COMPILE_SCRIPT = True					
                        os.system("echo 编译脚本中\n")
                    pass

                    if script_compile_mode == 0:
                        # debug
                        # os.system("echo compile script in debug mode: dst_file: %s src_file: %s"%(dst_file_path, src_file_path))
                        os.system("%s -o %s %s"%(LUAC_PATH, dst_file_path, src_file_path))
                    else:
                        # release
                        # os.system("echo compile script in release mode: dst_file: %s src_file: %s"%(dst_file_path, src_file_path))
                        os.system("%s -s -o %s %s"%(LUAC_PATH, dst_file_path, src_file_path))
                    pass

                    scripts_md5_map[src_file_key] = script_md5

                    global need_update_scripts_md5
                    need_update_scripts_md5 = True

                    # 检测脚本压缩包是否需要重新压缩
                    global SCRIPT_FILE_UPDATE_DIC
                    for script_ios_file, update_info in SCRIPT_FILE_UPDATE_DIC.iteritems():
                        if not update_info["need_update"]:
                            for match_info in update_info["match_list"]:
                                if update_info.get("check_as_file"):
                                    if src_file_key.endswith(match_info):
                                        update_info["need_update"] = True
                                        break
                                    pass
                                else:
                                    if src_file_key.startswith(match_info):
                                        update_info["need_update"] = True
                                        break
                                    pass
                                pass
                            pass
                        pass
                    pass                    
                pass
            pass

            # 处理子文件夹
            for sub_dir in dir_names:
                CompileScriptReal(os.path.join(parent_path, sub_dir), os.path.join(dst_parent_path, sub_dir))
            pass
        pass
    pass

    CompileScriptReal(script_path, script_path_precompile)

    os.system("echo 更新脚本文件的MD5记录\n")

    # 更新脚本文件的MD5记录
    if need_update_scripts_md5:    
        scripts_md5_list = []
        for script_file, script_md5 in scripts_md5_map.iteritems():
            scripts_md5_list.append("%s|%s\n"%(script_file, script_md5))
        pass

        scripts_md5_fo = open(scripts_md5_file, "w")
        scripts_md5_fo.writelines(scripts_md5_list)
        scripts_md5_fo.close()                                
    pass        
pass

# 客户端脚本压缩
def HandleScript(script_path, user):        
    if not CanUpdateScript():
        return
    pass

    # 当脚本有变化时，需要提交后，再更新提交一次
    is_need_to_run_again=False
    for (k,v) in SCRIPT_FILE_DIC.items():
        if not SCRIPT_FILE_UPDATE_DIC[k]["need_update"]:
            continue
        pass

        os.system("echo 重新压缩%s && cd %s && rm -f %s/script/%s && zip -q -9 -P zlgamecspw -rX %s/script/%s %s -x %s \"*/.svn/*\"\n"%( v[0], script_path, RESOURCE_PATH, k, RESOURCE_PATH, k, v[0], v[1] ))
        is_need_to_run_again = True
        pass
        
    # 先提交到svn，活动版本的变更
    if is_need_to_run_again:
        os.system("svn ci %s/script/script*ios -m'[%s]  提交脚本压缩包 by %s'"%(RESOURCE_PATH, "CDN", user))
    pass    
pass

def HandleConfigFile(area_lang_list, user, script_compile_mode, windows_cdn_path):
    if not CanUpdateConfig():
        return
    pass    

    os.system("echo 编译并压缩配置文件中\n")
    
    resource_lang_path = "%s/language"%(RESOURCE_PATH)

    for area_lang in area_lang_list:
        area_lang_path = "%s/%s"%(resource_lang_path, area_lang)
        area_lang_config_path = "%s/~config"%(area_lang_path)
        area_lang_config_path_precompile = "%s/~config_precompile"%(area_lang_path)
        area_lang_zconfig_path = "%s/config"%(area_lang_path)

        # 创建预编译后的配置文件路径
        if not os.path.exists(area_lang_config_path_precompile):
            os.mkdir(area_lang_config_path_precompile)
            # os.system("svn add %s"%(area_lang_config_path_precompile))
            pass        

        # 创建压缩后的配置文件路径
        if not os.path.exists(area_lang_zconfig_path):
            os.mkdir(area_lang_zconfig_path)
            os.system("svn add %s"%(area_lang_zconfig_path))
            pass        

        # 读取配置文件MD5记录
        config_md5_file = "%s/~config_md5"%(area_lang_path)
        if not os.path.exists(config_md5_file):
            os.mknod(config_md5_file)
            pass                
        config_md5_fo = open(config_md5_file)
        config_md5_list = config_md5_fo.readlines()    
        config_md5_fo.close()
        config_md5_map = {}
        for md5_str in config_md5_list:
            md5_info = md5_str.split('|')
            config_md5_map[md5_info[0]] = md5_info[1].rstrip('\n')
        pass
        need_update_md5 = False

        # print "旧的MD5记录表", config_md5_map

        config_file_list = os.listdir(area_lang_config_path)
        for config_file in config_file_list:
            if config_file.endswith(".cnf"):
                # 计算配置文件的MD5
                with open(os.path.join(area_lang_config_path, config_file), 'rb') as fp:
                    data = fp.read()
                config_md5 = hashlib.md5(data).hexdigest()

                # 如果配置文件发生变动，则重新压缩到目标路径并生成新的MD5
                if not config_md5_map.has_key(config_file) or config_md5_map[config_file] != config_md5:
                    config_md5_map[config_file] = config_md5
                    need_update_md5 = True

                    config_file_path = "%s/%s"%(area_lang_config_path, config_file)
                    config_file_path_precompile = "%s/%s"%(area_lang_config_path_precompile, config_file)
                    zconfig_file_path = "%s/%s"%(area_lang_zconfig_path, config_file.replace(".cnf", ".zcnf"))
                    need_add_zconfig_file = not os.path.exists(zconfig_file_path)

                    # 预编译配置文件
                    src_file_path_precompile = config_file_path.replace(CDN_FILE_PATH, windows_cdn_path)
                    dst_file_path_precompile = config_file_path_precompile.replace(CDN_FILE_PATH, windows_cdn_path)
                    if script_compile_mode == 0:
                        # debug
                        # os.system("echo compile config in debug mode: dst_file: %s src_file: %s"%(dst_file_path_precompile, src_file_path_precompile))
                        os.system("%s -o %s %s"%(LUAC_PATH, dst_file_path_precompile, src_file_path_precompile))
                    else:
                        # release
                        # os.system("echo compile config in release mode: dst_file: %s src_file: %s"%(dst_file_path_precompile, src_file_path_precompile))
                        os.system("%s -s -o %s %s"%(LUAC_PATH, dst_file_path_precompile, src_file_path_precompile))
                    pass

                    os.system("echo 重新压缩%s && cd %s && rm -f %s && zip -9 -rX %s %s \n"%(config_file_path_precompile, area_lang_config_path_precompile, zconfig_file_path, zconfig_file_path, config_file))                    

                    if need_add_zconfig_file:
                        os.system("svn add %s"%(zconfig_file_path))
                    pass
                pass
            pass
        pass

        # 更新配置文件的MD5记录
        if need_update_md5:
            # print "新的MD5记录表", config_md5_map

            config_md5_list = []
            for config_file, config_md5 in config_md5_map.iteritems():
                config_md5_list.append("%s|%s\n"%(config_file, config_md5))
            pass

            config_md5_fo = open(config_md5_file, "w")
            config_md5_fo.writelines(config_md5_list)
            config_md5_fo.close()                                
        pass    
    pass    
    os.system("svn ci %s -m'[%s]  提交配置文件压缩包 by %s'"%(resource_lang_path, "CDN", user))    
pass

def HandleVersionFile(lang_list, area_lang_list, user):
    os.system("echo 检索SVN版本信息列表\n")

    # print "启用语言列表：",lang_list
    # print "启用地区语言列表：",area_lang_list

    # 读取客户端版本号
    cpp_version_fo = open("%s/clientcppversioin"%CDN_FILE_PATH)
    cpp_version = cpp_version_fo.read()
    cpp_version_fo.close()

    svnversion=0
    tmp_path_version_list={}
    ret_str_list=os.popen("cd %s && svn info -R . | grep -E \"路径:|最后修改的版本:|Path:|Last Changed Rev:|节点种类:|Node Kind:\""%(CDN_FILE_PATH)).readlines()

    # 读取SVN版本信息列表
    i=0
    while i < len(ret_str_list):
        node_path=ret_str_list[i]
        node_type=ret_str_list[i+1]
        node_version=ret_str_list[i+2]
        i+=3

        file_path = node_path[8:-1]
        file_version = node_version[23:-1]

        if node_type.find("节点种类: 目录") != -1 \
            or node_path.find(": version.xml") != -1 \
            or node_path.find("respack") != -1 \
            or node_path.find("~") != -1 \
            or file_path.find("/") == -1:
            continue

        if file_path.find("resource/ui_res/language/")!=-1 :
            pos_1=len("resource/ui_res/language/")
            pos_2=file_path[pos_1:].find("/")
            if file_path[pos_1:pos_1+pos_2] in lang_list:
                # os.system("echo 保留%s"%file_path)
                pass
            else:
                #os.system("echo \t\t排除%s"%file_path)
                continue
                pass
            pass

        if file_path.find("resource/language/")!=-1 :
            pos_1=len("resource/language/")
            pos_2=file_path[pos_1:].find("/")
            if file_path[pos_1:pos_1+pos_2] in area_lang_list:
                # os.system("echo 保留%s"%file_path)
                pass
            else:
                #os.system("echo \t\t排除%s"%file_path)
                continue
                pass
            pass

        if file_path.find("_language_")!=-1 :
            pos_1=file_path.rfind('_') + 1
            pos_2=file_path.rfind('.')
            if file_path[pos_1:pos_2] in lang_list:
                # os.system("echo 保留%s"%file_path)
                pass
            else:
                #os.system("echo \t\t排除%s"%file_path)
                continue
                pass
            pass

        if file_path.find("resource/items/")!=-1 :
            pos_1=len("resource/items/")
            pos_2=file_path[pos_1:].find("/")
            if file_path[pos_1:pos_1+pos_2] in lang_list:
                # os.system("echo 保留%s"%file_path)
                pass
            else:
                #os.system("echo \t\t排除%s"%file_path)
                continue
                pass
            pass

        tmp_path_version_list[file_path] = file_version
        if svnversion < int(file_version):
            svnversion = int(file_version)
            pass
        pass
    tmp_path_list=tmp_path_version_list.keys()
    tmp_path_list.sort()

    # 获取最后的svn版本号
    os.system("echo 资源最后svn版本号：【%s】\n"%str(svnversion))

    # 获取文件的对于等级
    file_level_dict={}

    # 读取版本信息记录
    version_record_file = "%s/version_record"%CDN_FILE_PATH
    if not os.path.exists(version_record_file):
        os.mknod(version_record_file)
    pass
    version_record_fo = open(version_record_file)
    version_record_list = version_record_fo.readlines()    
    version_record_fo.close()
    version_record_map = {}
    for record_str in version_record_list:
        record_info_list = record_str.split('|')
        record_info = {}
        record_info["version"] = record_info_list[1]
        record_info["size"] = record_info_list[2]
        record_info["level"] = record_info_list[3].rstrip('\n')
        version_record_map[record_info_list[0]] = record_info
    pass

    # 读取版本同步列表
    version_sync_file = "%s/version_sync_list"%CDN_FILE_SYNC_PATH
    if not os.path.exists(version_sync_file):
        os.mknod(version_sync_file)
    pass
    version_sync_fo = open(version_sync_file)
    version_sync_list = version_sync_fo.readlines()
    version_sync_fo.close()
    version_sync_map = {}
    for update_str in version_sync_list:
        update_info = update_str.split('|')
        version_sync_map[update_info[0]] = update_info[1].rstrip('\n')
    pass

    need_update_version_record = False
    need_update_version_sync = False

    os.system("echo 生成版本信息列表\n")

    # 生成最终版本信息列表
    tmp_xml_file = open("%s/version.xml"%CDN_FILE_PATH, 'w')
    tmp_xml_file.write('<?xml version="1.0" encoding="utf-8"?>\r\n')
    tmp_xml_file.write('<config ver="%s" svn_ver="%s">\r\n'%(cpp_version, str(svnversion)))
    for k in tmp_path_list:
        file_path = "%s"%k
        file_size = os.stat(CDN_FILE_PATH+"/"+file_path)[stat.ST_SIZE]
        try:
            level=file_level_dict[file_path]
            pass
        except Exception, e:
            level=-1
            pass

        record_info = version_record_map.get(file_path)
        record_version = None if record_info == None else record_info["version"]
        new_version = tmp_path_version_list[file_path]
        
        if CheckCanUpdateFile(file_path):
            # 新增或修改文件
            if record_version == None or record_version != new_version:
                split_info_list = os.path.splitext(file_path)

                # 移除同步目录中的旧版本文件
                if record_version != None:
                    file_path_for_sync = "%s/%s_v%s%s"%(CDN_FILE_SYNC_PATH, split_info_list[0], record_version, split_info_list[1]) 
                    if os.path.exists(file_path_for_sync):
                        os.remove(file_path_for_sync)
                    pass
                pass

                # 拷贝新版本文件到同步目录
                file_path_for_sync = "%s/%s_v%s%s"%(CDN_FILE_SYNC_PATH, split_info_list[0], new_version, split_info_list[1]) 
                file_parent_path_for_sync = os.path.dirname(file_path_for_sync)
                if not os.path.exists(file_parent_path_for_sync):
                    os.makedirs(file_parent_path_for_sync)
                pass
                shutil.copyfile("%s/%s"%(CDN_FILE_PATH, file_path), file_path_for_sync)
    
                # 更新版本信息记录
                need_update_version_record = True
                if record_info == None:
                    record_info = {}
                    version_record_map[file_path] = record_info
                pass
                record_info["version"] = new_version
                record_info["size"] = file_size
                record_info["level"] = level

                # 更新版本同步列表
                need_update_version_sync = True
                version_sync_map[file_path] = new_version
            pass

            # 将新版本插入到最终版本信息列表
            tmp_xml_file.write('\t<res path="%s" s="%s" l="%s">%s</res>\r\n'%(file_path, file_size, level, new_version))
        
        else:
            # 将旧版本插入到最终版本信息列表
            if record_info:
                record_file_size = record_info["size"]
                record_file_level = record_info["level"]
                tmp_xml_file.write('\t<res path="%s" s="%s" l="%s">%s</res>\r\n'%(file_path, record_file_size, record_file_level, record_version))            
            pass
        pass
    pass
    tmp_xml_file.write('</config>\r\n')
    tmp_xml_file.close()

    # 移除已废弃文件的记录
    for file_path, record_info in version_record_map.items():
        if CheckCanUpdateFile(file_path):
            # 删除文件
            new_version = tmp_path_version_list.get(file_path)
            if None == new_version:
                # 移除同步目录中的旧版本文件
                split_info_list = os.path.splitext(file_path)
                file_path_for_sync = "%s/%s_v%s%s"%(CDN_FILE_SYNC_PATH, split_info_list[0], record_info["version"], split_info_list[1]) 
                if os.path.exists(file_path_for_sync):
                    os.remove(file_path_for_sync)
                pass

                # 更新版本信息记录
                need_update_version_record = True
                del version_record_map[file_path]

                # 更新版本同步列表
                if version_sync_map.has_key(file_path):
                    need_update_version_sync = True
                    del version_sync_map[file_path]
                pass
            pass
        pass
    pass
    
    # 更新版本信息记录(文件)
    if need_update_version_record:
        version_record_list = []
        for file_path, record_info in version_record_map.iteritems():
            new_version = tmp_path_version_list.get(file_path)
            if None != new_version:                
                version_record_list.append("%s|%s|%s|%s\n"%(file_path, record_info["version"], record_info["size"], record_info["level"]))
            pass
        pass
        version_record_fo = open(version_record_file, 'w')
        version_record_fo.writelines(version_record_list)
        version_record_fo.close()                                
    pass    

    # 更新版本同步列表(文件)
    if need_update_version_sync :
        version_sync_list = []
        for file_path, version in version_sync_map.iteritems():
            version_sync_list.append("%s|%s\n"%(file_path, version))
        pass
        version_sync_fo = open(version_sync_file, 'w')
        version_sync_fo.writelines(version_sync_list)
        version_sync_fo.close()                                
    pass    

    # 生成version.xml压缩包
    os.system("touch -c -m -d\"2016-01-01 00:00:00\" %s/version.xml "%(CDN_FILE_PATH))
    os.system("cd %s && rm -f version.ios && zip -9 -rX version.ios version.xml "%(CDN_FILE_PATH))
    version_md5 = os.popen("md5sum %s/version.ios | cut -d ' ' -f1 | head -1 "%(CDN_FILE_PATH)).readlines()[0][:-1]
    version_md5_fo = open("%s/version_md5"%(CDN_FILE_PATH), "w")
    version_md5_fo.write(version_md5)
    version_md5_fo.close()                                

    # 拷贝version.ios、version_md5、clientcppversioin到同步目录
    shutil.copyfile("%s/version.ios"%(CDN_FILE_PATH), "%s/version.ios"%(CDN_FILE_SYNC_PATH))
    shutil.copyfile("%s/version_md5"%(CDN_FILE_PATH), "%s/version_md5"%(CDN_FILE_SYNC_PATH))
    shutil.copyfile("%s/clientcppversioin"%CDN_FILE_PATH, "%s/clientcppversioin"%CDN_FILE_SYNC_PATH)
    
    # 清理
    os.system("rm -fv %s/version.xml "%(CDN_FILE_PATH))
    os.system("rm -fv %s/md5 "%(CDN_FILE_PATH))
    os.system("rm -fv %s/md5_apple"%(CDN_FILE_PATH))
    os.system("rm -fv %s/version_apple.ios"%(CDN_FILE_PATH))
    
    #提交
    os.system("svn ci %s/version.ios -m'[%s]  提交资源版本号文件 by %s'"%(CDN_FILE_PATH, "CDN", user))
    os.system("svn ci %s/version_md5 -m'[%s]  提交资源版本号文件MD5 by %s'"%(CDN_FILE_PATH, "CDN", user))
    pass
pass

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-l",    "--lang_list",                type="string",        dest="op_lang_list",            action="store",        help="必要参数，允许的语言列表")
    parser.add_option("-c",    "--area_lang_list",            type="string",        dest="op_area_lang_list",        action="store",        help="必要参数，允许的地区语言列表")
    parser.add_option("-u",    "--user",                    type="string",        dest="op_user",                    action="store",        help="必要参数，操作人")
    parser.add_option("-m", "--script_compile_mode",    type="int",            dest="op_script_compile_mode",    action="store",        help="必要参数，脚本编译模式")
    parser.add_option("-p", "--cdn_path",                type="string",        dest="op_cdn_path",                action="store",        help="必要参数，cdn路径")
    parser.add_option("-y", "--cdn_sync_path",            type="string",        dest="op_cdn_sync_path",        action="store",     help="必要参数，cdn同步路径")
    parser.add_option("-s", "--special_update_mode",    type="string",        dest="op_special_update_mode",    action="store",        help="可选参数，单独更新模式")

    (options, args) = parser.parse_args()
    if options.op_lang_list == None :
        parser.error("缺少必要参数 lang_list") 
        pass
    elif options.op_area_lang_list == None :
        parser.error("缺少必要参数 area_lang_list") 
        pass
    elif options.op_user == None :
        parser.error("缺少必要参数 user") 
        pass
    elif options.op_script_compile_mode == None :
        parser.error("缺少必要参数 m") 
        pass    
    elif options.op_cdn_path == None :
        parser.error("缺少必要参数 p") 
        pass    
    
    global CDN_FILE_PATH
    global CDN_FILE_SYNC_PATH
    global RESOURCE_PATH
    CDN_FILE_PATH = os.path.abspath(options.op_cdn_path)
    CDN_FILE_SYNC_PATH = os.path.abspath(options.op_cdn_sync_path)
    RESOURCE_PATH = "%s/resource"%CDN_FILE_PATH

    # 创建同步目录
    if not os.path.exists(CDN_FILE_SYNC_PATH):
        os.mkdir(CDN_FILE_SYNC_PATH)
        pass                

    # 检测是否同步中
    status_sync_file = "%s/status_sync"%(CDN_FILE_SYNC_PATH)
    if not os.path.exists(status_sync_file):
        os.mknod(status_sync_file)
    pass    
    status_sync_fo = open(status_sync_file)
    status_sync = status_sync_fo.read()
    status_sync = 0 if status_sync == '' else int(status_sync)
    status_sync_fo.close()
    if status_sync == 1:
        os.system("echo 资源同步中，请稍后！")
        return
    pass

    # 获取更新模式
    global SPECIAL_UPDATE_MODE
    SPECIAL_UPDATE_MODE = 0 if options.op_special_update_mode == None else int(options.op_special_update_mode)    

    # 检测已更新模式与待更新模式是否匹配
    mode_update_file = "%s/mode_update"%(CDN_FILE_PATH)
    if not os.path.exists(mode_update_file):
        os.mknod(mode_update_file)
    pass
    mode_update_fo = open(mode_update_file, "r+")
    mode_update = mode_update_fo.read()
    mode_update = SUM_UNKNOWN if mode_update == '' else int(mode_update)
    if SUM_UNKNOWN != mode_update and SPECIAL_UPDATE_MODE != mode_update and SPECIAL_UPDATE_MODE != SUM_ALL:
        os.system("echo 已经处于%s更新模式，请先选择对应同步模式同步后再继续，或者选择更新全部！"%(GetUpdateModeDesc(mode_update)))
        mode_update_fo.close()
        return
    pass
    # 设置更新模式
    mode_update_fo.seek(0, 0)
    mode_update_fo.write(str(SPECIAL_UPDATE_MODE))
    mode_update_fo.close()

    # 检测是否更新中
    status_update_file = "%s/status_update"%(CDN_FILE_PATH)
    if not os.path.exists(status_update_file):
        os.mknod(status_update_file)
    pass
    status_update_fo = open(status_update_file, "r+")
    status_update = status_update_fo.read()
    status_update = 0 if status_update == '' else int(status_update)
    if status_update == 1:
        os.system("echo 资源更新中，请稍后！")
        status_update_fo.close()
        return
    pass

    # 记录更新中状态
    status_update_fo.seek(0, 0)
    status_update_fo.write("1")
    status_update_fo.close()

    # 更新
    os.system("svn up %s"%CDN_FILE_PATH)
    # 还原
    os.system("svn revert %s"%RESOURCE_PATH)

    # 设置客户端脚本路径
    script_path = CDN_FILE_PATH + "/../script"
    script_path_precompile = CDN_FILE_PATH + "/../script_precompile"

    # 编译客户端脚本
    cur_time = time.clock()
    CompileScript(script_path, script_path_precompile, options.op_script_compile_mode, options.op_cdn_path)
    os.system("echo CompileScript cost time %d"%(time.clock() - cur_time))

    # 打包客户端脚本
    cur_time = time.clock()
    HandleScript(script_path_precompile, options.op_user)
    os.system("echo HandleScript cost time %d"%(time.clock() - cur_time))

    # 打包客户端配置
    cur_time = time.clock()
    HandleConfigFile(options.op_area_lang_list.split('#'), options.op_user, options.op_script_compile_mode, options.op_cdn_path)        
    os.system("echo HandleConfigFile cost time %d"%(time.clock() - cur_time))

    # 生成版本信息列表
    cur_time = time.clock()
    HandleVersionFile(options.op_lang_list.split('#'), options.op_area_lang_list.split('#'), options.op_user)    
    os.system("echo HandleVersionFile cost time %d"%(time.clock() - cur_time))

    # 还原更新中状态
    status_update_fo = open(status_update_file, "w")
    status_update_fo.write("0")
    status_update_fo.close()

    os.system("echo 资源更新完毕！\n")    
pass

if __name__ == "__main__":
    Handle()
pass

