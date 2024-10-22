#!/usr/bin/python
#coding=utf-8

import zipfile
import sys, os, shutil
import codecs
from xml.dom import minidom
from optparse import OptionParser

global start_up_path

global cdnfiles_path
global cdnfiles_resource_path

global package_cdnfiles_path
global package_cdnfiles_path_resource_path

global package_cdnfiles_path_OBB
global package_cdnfiles_path_resource_path_OBB

# 原始资源版本列表
global version_map
# pkg资源版本列表
global pkg_version_map
# obb资源版本列表
global obb_version_map
# 原始资源版本根节点信息
global version_root_info

root_path = os.path.dirname(os.path.abspath(sys.path[0]))

# 读取版本信息文件
def ReadVersionFile():
    global version_map
    version_map = {}

    global pkg_version_map
    pkg_version_map = {}

    global obb_version_map
    obb_version_map = {}

    zip_file = zipfile.ZipFile(cdnfiles_path+'/version.ios', mode = 'r')
    xml_data = zip_file.read('version.xml')
    zip_file.close()

    dom = minidom.parseString(xml_data)
    dom_root = dom.documentElement

    global version_root_info
    version_root_info = {}
    version_root_info['ver'] = dom_root.getAttribute("ver")
    version_root_info['svn_ver'] = dom_root.getAttribute("svn_ver")
    version_root_info['res_tag'] = dom_root.getAttribute("res_tag")

    version_node_list = dom_root.getElementsByTagName("res")
    for version_node in version_node_list:  
        templ_version_info = {}
        templ_version_info['path'] = version_node.getAttribute("path")
        templ_version_info['s'] = version_node.getAttribute("s")
        templ_version_info['l'] = version_node.getAttribute("l")
        templ_version_info['ver'] = version_node.childNodes[0].nodeValue
        version_map[templ_version_info['path']] = templ_version_info
    pass 
pass

# 生成版本信息文件
def WriteVersionFile(is_obb):
    version_map = is_obb and obb_version_map or pkg_version_map
    version_xml_file = is_obb and ("%s/version.xml"%package_cdnfiles_path_OBB) or ("%s/version.xml"%package_cdnfiles_path)

    # 生成资源版本列表文件
    dom = minidom.Document()
    dom_root = dom.createElement("config")
    dom_root.setAttribute('ver', version_root_info['ver'])
    dom_root.setAttribute('svn_ver', version_root_info['svn_ver'])
    dom_root.setAttribute('res_tag', version_root_info['res_tag'])
    dom.appendChild(dom_root)
    for path, version_info in version_map.iteritems():
        version_node = dom.createElement("res")
        version_node.setAttribute('path', version_info['path'])
        version_node.setAttribute('s', version_info['s'])
        version_node.setAttribute('l', version_info['l'])
        version_node_text = dom.createTextNode(version_info['ver'])
        version_node.appendChild(version_node_text)
        dom_root.appendChild(version_node)
    pass
    f = codecs.open(version_xml_file, 'w', 'utf-8') 
    dom.writexml(f, addindent='  ', newl='\n', encoding = 'utf-8')

    if not is_obb:
        version_ios_file = "%s/version.ios"%package_cdnfiles_path
        zipFile = zipfile.ZipFile(version_ios_file, 'w')
        zipFile.write(version_xml_file, 'version.xml', zipfile.ZIP_DEFLATED)
        zipFile.close()
    pass
pass

# 生成obb扩展包
def GenOBBFile():
    # def zip_ya(startdir，file_news):
    obb_src_dir = package_cdnfiles_path_OBB
    obb_file = "%s/main.obb"%package_cdnfiles_path
    z = zipfile.ZipFile(obb_file, 'w', zipfile.ZIP_DEFLATED) #参数一：文件夹名
    for dirpath, dirnames, filenames in os.walk(obb_src_dir):
        fpath = dirpath.replace(obb_src_dir,'') #这一句很重要，不replace的话，就从根目录开始复制
        fpath = fpath and fpath + os.sep or ''#这句话理解我也点郁闷，实现当前文件夹以及包含的所有文件的压缩
        for filename in filenames:
            z.write(os.path.join(dirpath, filename), fpath+filename)
        pass
    pass
    z.close()
pass

# 提取启动资源
def TakeStartupPack(mode):
    src_file_path = start_up_path+"/zhunyuan.ttf"
    dest_file_path = package_cdnfiles_path+"/zhunyuan.ttf"
    shutil.copyfile(src_file_path, dest_file_path)

    src_file_path = start_up_path+"/start_string.cnf"
    dest_file_path = package_cdnfiles_path+"/start_string.cnf"
    shutil.copyfile(src_file_path, dest_file_path)

    src_file_path = start_up_path+"/language_string.cnf"
    dest_file_path = package_cdnfiles_path+"/language_string.cnf"
    shutil.copyfile(src_file_path, dest_file_path)

    if mode != "1":
        src_file_path = start_up_path+"/resource/start"
        dest_file_path = package_cdnfiles_path_resource_path+"/start"
        shutil.copytree(src_file_path, dest_file_path)
    pass
pass

# 提取资源
def TakeResourcePack(area_language_list, language_list):
    is_obb = False
    for path, version_info in version_map.iteritems():
        src_file_path = os.path.join(cdnfiles_path, path)

        if path.find("resource/half_heads") != -1 or path.find("resource/model") != -1 or path.find("resource/sound") != -1 or path.find("resource/ui_res/bigbg") != -1:
            is_obb = True

        elif path.find("resource/effects") != -1:
            is_obb = True
            if not CheckLanguage("effect", path, None, language_list):
                continue
            pass

        elif path.find("resource/items") != -1:
            is_obb = False
            if not CheckLanguage("item", path, None, language_list):
                continue
            pass

        elif path.find("resource/language") != -1:
            is_obb = False
            if not CheckLanguage("config", path, area_language_list, None):
                continue
            pass

        elif path.find("resource/ui_res/language") != -1:
            is_obb = False
            if not CheckLanguage("ui", path, None, language_list):
                continue
            pass
        
        else:
            is_obb = False
        pass

        if is_obb:
            # 提取到obb包
            dest_file_path = os.path.join(package_cdnfiles_path_OBB, path)
            obb_version_map[path] = version_info
        else:
            # 提取到pkg包
            dest_file_path = os.path.join(package_cdnfiles_path, path)
            pkg_version_map[path] = version_info
        pass

        # 创建目录
        dest_file_dir = os.path.dirname(dest_file_path)
        if not os.path.exists(dest_file_dir):
            os.makedirs(dest_file_dir)
        pass

        # 提取文件
        shutil.copyfile(src_file_path, dest_file_path)        
    pass
pass

# 检测语言资源
def CheckLanguage(type, file_name, area_language_list, language_list):
    if type == "effect":
        if file_name.find("_language_") != -1:
            for select_language in language_list:
                language_tag = "_language_" + select_language                
                if file_name.find(language_tag) != -1:
                    return True
                pass
            pass    
            return False        
        pass
                        
    elif type == "item":
        for select_language in language_list:
            if file_name.find("items/%s"%select_language) != -1:
                return True
            pass
        pass               
        return False 

    elif type == "ui":
        for select_language in language_list:
            if file_name.find("ui_res/language/%s"%select_language) != -1:
                return True
            pass
        pass               
        return False 

    elif type == "config":
        for select_arena_language in area_language_list:
            if file_name.find("language/%s"%select_arena_language) != -1:
                return True
            pass
        pass
        return False
    pass    

    return True
pass

########################################################################

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-a", "--area_language",    type="string", dest="op_area_language", action="store", help="必要参数，允许的地区语言列表")
    parser.add_option("-m",    "--mode", type="string", dest="op_mode", action="store", help="可选参数，模式")    
    parser.add_option("-c",    "--cdnfiles_path", type="string", dest="op_cdnfiles_path", action="store", help="必要参数，源CDN路径")
    parser.add_option("-p", "--package_cdnfiles_path", type="string", dest="op_package_cdnfiles_path", action="store", help="必要参数，目标CDN路径")
    parser.add_option("-s", "--start_up_path", type="string", dest="op_start_up_path", action="store", help="必要参数，启动资源目录")
    (options, args) = parser.parse_args()
    if options.op_area_language == None :
        parser.error("缺少必要参数 area_language") 
        pass
    elif options.op_cdnfiles_path == None :
        parser.error("缺少必要参数 cdnfiles_path") 
        pass
    elif options.op_package_cdnfiles_path == None :
        parser.error("缺少必要参数 package_cdnfiles_path") 
        pass
    elif options.op_start_up_path == None :
        parser.error("缺少必要参数 start_up_path") 
        pass

    print "-------------take package start--------------"

    # 初始化语言列表
    language_list = []
    area_language_list = options.op_area_language.split('#')
    for area_language in area_language_list:        
        language = area_language.split('_')[1]
        language_list.append(language)
    pass
    print language_list
    print area_language_list

    # 初始化启动资源目录
    global start_up_path
    start_up_path = "%s/../%s"%(root_path, options.op_start_up_path)
    print start_up_path

    # 初始化CDN资源目录
    global cdnfiles_path
    cdnfiles_path = options.op_cdnfiles_path
    print cdnfiles_path
    global cdnfiles_resource_path
    cdnfiles_resource_path = cdnfiles_path+"/resource"

    # 初始化pkg资源目录
    global package_cdnfiles_path
    package_cdnfiles_path = "%s/%s"%(root_path, options.op_package_cdnfiles_path)
    print package_cdnfiles_path
    global package_cdnfiles_path_resource_path
    package_cdnfiles_path_resource_path = package_cdnfiles_path+"/resource"

    # 初始化obb资源目录
    global package_cdnfiles_path_OBB
    package_cdnfiles_path_OBB = "%s/%s_obb"%(root_path, options.op_package_cdnfiles_path)
    print package_cdnfiles_path_OBB
    global package_cdnfiles_path_resource_path_OBB
    package_cdnfiles_path_resource_path_OBB = package_cdnfiles_path_OBB+"/resource"

    if os.path.exists(package_cdnfiles_path):
        shutil.rmtree(package_cdnfiles_path)
        pass
    os.mkdir(package_cdnfiles_path)
    os.mkdir(package_cdnfiles_path_resource_path)

    if os.path.exists(package_cdnfiles_path_OBB):
        shutil.rmtree(package_cdnfiles_path_OBB)
        pass
    os.mkdir(package_cdnfiles_path_OBB)
    os.mkdir(package_cdnfiles_path_resource_path_OBB)

    # 更新资源
    os.system("svn up %s"%start_up_path)    

    ReadVersionFile()
    TakeStartupPack(options.op_mode)
    TakeResourcePack(area_language_list, language_list)
    WriteVersionFile(None)
    WriteVersionFile(True)
    GenOBBFile()    
    shutil.rmtree(package_cdnfiles_path_OBB)

    os.system("svn add %s/* --force"%package_cdnfiles_path)
    os.system("svn st %s|grep ! |awk '{print $2}'|xargs svn del"%package_cdnfiles_path)
    os.system("svn ci %s -m'更新打包资源'"%(package_cdnfiles_path))

    print "-------------take package end--------------"
pass

if __name__ == "__main__":
    Handle()
pass