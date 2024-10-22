#!/usr/bin/python
#coding=utf-8

import zipfile
import sys, os, shutil
from optparse import OptionParser

global cdnfiles_path
global cdnfiles_resource_path

global package_cdnfiles_path
global package_cdnfiles_path_resource_path

global start_up_path

root_path = os.path.dirname(os.path.abspath(sys.path[0]))

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

# 提取普通资源
def TakeCommonPack():
    #version.ios
    shutil.copyfile(cdnfiles_path+'/version.ios', os.path.join(package_cdnfiles_path, 'version.ios'))

    #version.xml
    zip_file = zipfile.ZipFile(cdnfiles_path+'/version.ios', mode = 'r')
    zip_data = zip_file.read('version.xml')
    zip_file.close()

    extra_file = file(os.path.join(package_cdnfiles_path, 'version.xml'), 'wb')
    extra_file.write(zip_data)
    extra_file.close()

    #ui_res
    src_ui_res_path = cdnfiles_resource_path+"/ui_res"
    dest_ui_res_path = package_cdnfiles_path_resource_path+"/ui_res"
    os.mkdir(dest_ui_res_path)
    for parent_path, dir_names, filenames in os.walk(src_ui_res_path):
        if parent_path != src_ui_res_path:
            continue

        for file_name in filenames:
            src_file_path = os.path.join(parent_path, file_name)
            shutil.copyfile(src_file_path, os.path.join(dest_ui_res_path, file_name))
        pass

        for file_dir in dir_names:
            src_file_path = os.path.join(parent_path, file_dir)
            if file_dir.find("language") == -1:
                shutil.copytree(src_file_path, os.path.join(dest_ui_res_path, file_dir))
                # print src_file_path
            pass
        pass
    pass    

    #other_res
    copy_path_array = (
        "pb",		
        "script",
        "bigicon",
        "default",
        "effects/seqframe",
        "effects/skeleton",
        "half_heads",
        "heads",
        "map",
        "model",
        "sound",
        "ui_layout",
    )
    for copy_path in copy_path_array:
        src_file_path = os.path.join(cdnfiles_resource_path, copy_path)
        dest_file_path = os.path.join(package_cdnfiles_path_resource_path, copy_path)
        shutil.copytree(src_file_path, dest_file_path)
    pass
pass

# 提取语言资源
def TakeLanguagePack(language_list):
    for select_language in language_list:
        #items/
        src_file_path = os.path.join(cdnfiles_resource_path+"/items", select_language)
        dest_file_path = os.path.join(package_cdnfiles_path_resource_path+"/items", select_language)
        shutil.copytree(src_file_path, dest_file_path)    

        #ui_res/language
        src_file_path = os.path.join(cdnfiles_resource_path+"/ui_res/language", select_language)
        dest_file_path = os.path.join(package_cdnfiles_path_resource_path+"/ui_res/language", select_language)
        shutil.copytree(src_file_path, dest_file_path)
    pass

    #effects
    copy_path_array = (
        "effects/particle/config",
        "effects/particle/mesh",
        "effects/particle/texture",
    )
    os.mkdir(os.path.join(package_cdnfiles_path_resource_path, "effects/particle"))

    for copy_path in copy_path_array:
        src_file_path = os.path.join(cdnfiles_resource_path, copy_path)
        dest_file_path = os.path.join(package_cdnfiles_path_resource_path, copy_path)
        os.mkdir(dest_file_path)

        for parent_path, dir_names, filenames in os.walk(src_file_path):
            if parent_path != src_file_path:
                continue

            for file_name in filenames:

                for select_language in language_list:
                    language_tag = "_language_" + select_language                

                    if file_name.find("_language_") == -1 or file_name.find(language_tag) != -1:
                        shutil.copyfile(os.path.join(parent_path, file_name), os.path.join(dest_file_path, file_name))
                    pass
                pass
            pass                
        pass
    pass    
pass

# 提取区域语言资源
def TakeAreaLanguagePack(area_language_list):
    for select_arena_language in area_language_list:
        #config
        language_config_path = cdnfiles_resource_path + "/language/%s/config"%select_arena_language
        dets_language_config_path = package_cdnfiles_path_resource_path + "/language/%s/config"%select_arena_language
        os.makedirs(dets_language_config_path)
        for dir_path, dir_names, filenames in os.walk(language_config_path):
            for file_name in filenames:
                if file_name.find(".zcnf") != -1:
                    src_file_path = os.path.join(dir_path, file_name)
                    dest_file_path = os.path.join(dets_language_config_path, file_name)
                    shutil.copyfile(src_file_path, dest_file_path)
                    pass
                pass
            pass
        pass
    pass
pass

########################################################################

def Handle():
    parser = OptionParser("用法: %prog [-option]")
    parser.add_option("-a", "--area_language",    type="string", dest="op_area_language", action="store", help="必要参数，允许的地区语言列表")
    parser.add_option("-m",    "--mode", type="string", dest="op_mode", action="store", help="可选参数，模式")    
    parser.add_option("-c",    "--cdnfiles_path", type="string", dest="op_cdnfiles_path", action="store", help="必要参数，源CDN路径")
    parser.add_option("-p", "--package_cdnfiles_path", type="string", dest="op_package_cdnfiles_path", action="store", help="必要参数，目标CDN路径")
    parser.add_option("-s", "--start_up_path", type="string", dest="op_start_up_path", action="store", help="必要参数，随包资源目录")
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

    global cdnfiles_path
    cdnfiles_path = options.op_cdnfiles_path
    print cdnfiles_path

    global cdnfiles_resource_path
    cdnfiles_resource_path = cdnfiles_path+"/resource"

    language_list = []
    area_language_list = options.op_area_language.split('#')
    for area_language in area_language_list:        
        language = area_language.split('_')[1]
        language_list.append(language)
    pass
    print language_list
    print area_language_list

    global package_cdnfiles_path
    package_cdnfiles_path = "%s/%s"%(root_path, options.op_package_cdnfiles_path)
    print package_cdnfiles_path

    global package_cdnfiles_path_resource_path
    package_cdnfiles_path_resource_path = package_cdnfiles_path+"/resource"

    global start_up_path
    start_up_path = "%s/../%s"%(root_path,    options.op_start_up_path)

    # 更新资源
    os.system("svn up %s"%start_up_path)    
    os.system("svn up %s"%package_cdnfiles_path)
    # os.system("svn up %s"%cdnfiles_path)

    print "-------------take package start--------------"

    if os.path.exists(package_cdnfiles_path):
        shutil.rmtree(package_cdnfiles_path)
        pass
    os.mkdir(package_cdnfiles_path)
    os.mkdir(package_cdnfiles_path_resource_path)

    TakeStartupPack(options.op_mode)
    TakeCommonPack()
    TakeLanguagePack(language_list)
    TakeAreaLanguagePack(area_language_list)

    os.system("svn add %s/* --force"%package_cdnfiles_path)
    os.system("svn st %s|grep ! |awk '{print $2}'|xargs svn del"%package_cdnfiles_path)
    os.system("svn ci %s -m'更新打包资源'"%(package_cdnfiles_path))

    print "-------------take package end--------------"
pass

if __name__ == "__main__":
    Handle()
pass