#! /usr/bin/env python   
# -*- coding: utf-8 -*-  

import sys,os,commands,shutil,time
import struct
import hashlib
import re
import zipfile
import xlrd
import xlwt
from optparse import OptionParser

# 切换到文件所在目录
os.chdir(os.path.abspath(sys.path[0]))
os.environ['LANG']="zh_CN.utf8"

CUR_PATH = os.path.dirname(os.path.abspath(sys.path[0]))
CUR_FANYI_PATH = "%s/ChineseExtract"%CUR_PATH
    
# 已翻译文本在本次检测中的引用次数
G_OLD_CHS_TO_FOREIGN_LANG_REF_DICT = {}
# 已翻译文本
G_OLD_CHS_TO_FOREIGN_LANG_DICT = {}
# 已翻译但不合法文本
G_OLD_CHS_TO_FOREIGN_LANG_ILLEGAL_DICT = {}

# 新增、变化文本
G_NEW_CHS_TO_FOREIGN_LANG_DICT = {}

#格式
#p路径，a地区，l语言
#数组第二第四位为该字符串的格式
G_FANYI_PATH = [
    # 前端
    ["%s/script/language/chs", "p", "%s/script/language/%s", "pl"],
    ["%s/cdnfiles/resource/language/dl_chs/~config", "p", "%s/cdnfiles/resource/language/%s_%s/~config", "pal"],
]


#指定Zip期望修改后的时间
TimeForChange = '2016-01-01 00:00:00'
ConverTime = time.mktime(time.strptime(TimeForChange,'%Y-%m-%d %H:%M:%S'))
ZipTimes = (ConverTime, ConverTime)

def IsNeedFanyi(test_str):
    #print test_str
    if test_str[:2]=="--":
        #print test_str
        return False
    match_ret = re.match(ur"(.*)([\u4e00-\u9fa5]+)", test_str.decode("utf8"))
    if match_ret:
        #print "====================>",test_str
        ret_tuple = match_ret.groups()
        if len(ret_tuple)!=2:
            # print test_str
            pass
        if ret_tuple[0].encode("utf-8") == "":
            #print test_str,
            #print len(ret_tuple),ret_tuple[0].encode("utf-8"),"---------",ret_tuple[1].encode("utf-8")
            pass
        return True

    match_ret = re.match(ur"(.*)([\u3002\uff1f\uff01\uff0c\u3001\uff1b\uff1a\u201c\u201d\u2018\uff08\uff09\u300a\u300b\u3008\u3009\u3010\u3011\u300e\u300f\u300c\u300d\ufe43\ufe44\u3014\u3015\u2014\uff5e\ufe4f\uffe5]+)", test_str.decode("utf8"))
    return match_ret != None
    
def CheckEndWithOsSep(path_str):
    if path_str.endswith("/") or path_str.endswith("\\"):
        return path_str
    return path_str+os.path.sep
    
def EndWithUnixLineChar(tmp_str):
    if tmp_str.endswith("\r\n"):
        return tmp_str
    if tmp_str.endswith("\n"):
        return tmp_str[:-1]+"\r\n"
    return tmp_str+"\r\n"
    
def OnlyStr(tmp_str):
    return tmp_str.strip("\r\n").strip("\n").strip("\t").strip()

def GetRealCnfStr(tmp_str):
    beg_pos = tmp_str.find('=')    
    end_pos = tmp_str.rfind(',')
    if end_pos - beg_pos >= 2:
        tmp_str = tmp_str[beg_pos + 1:end_pos]
        return OnlyStr(tmp_str)
    return tmp_str

# 读取已翻译记录
def LoadExcelData(language):
    file_name = "%s/fanyi_%s.xls"%(CUR_FANYI_PATH, language)
    if not os.path.exists(file_name):
        return

    excel_data = xlrd.open_workbook(file_name)
    sheet_data = excel_data.sheet_by_index(0)
    for r in range(sheet_data.nrows):
        if r >= 2:
            chs_str = OnlyStr(sheet_data.cell(r, 0).value)
            chs_str = chs_str.encode('utf-8')
            foreign_str = sheet_data.cell(r, 1).value
            if isinstance(foreign_str,basestring) == False:
                os.system("echo 已翻译文本类型错误，对应的中文字段是：%s"%chs_str)
            else:
                foreign_str = foreign_str.encode('utf-8')
                if IsNeedFanyi(OnlyStr(foreign_str)):
                    os.system("echo 已翻译文本还遗留中文，对应的中文字段是：%s"%chs_str)
                    G_OLD_CHS_TO_FOREIGN_LANG_ILLEGAL_DICT[chs_str] = foreign_str
                else:
                    G_OLD_CHS_TO_FOREIGN_LANG_DICT[chs_str] = foreign_str
                    G_OLD_CHS_TO_FOREIGN_LANG_REF_DICT[chs_str] = 0
                pass
            pass
        pass
    pass

# 写入已翻译、新增、变化记录
def WriteExcelData(language):
    if len(G_NEW_CHS_TO_FOREIGN_LANG_DICT) <= 0:
        return

    file_name = "%s/fanyi_%s.xls"%(CUR_FANYI_PATH, language)
    excel_data = xlwt.Workbook(encoding='utf-8')

    old_chs_to_foreign_sheet = excel_data.add_sheet("已翻译")
    old_chs_to_foreign_sheet.write(0, 0, u"中文")
    old_chs_to_foreign_sheet.write(0, 1, u"外文")

    new_chs_to_foreign_sheet = excel_data.add_sheet("待翻译")
    new_chs_to_foreign_sheet.write(0, 0, u"中文")
    new_chs_to_foreign_sheet.write(0, 1, u"外文")

    deprecated_chs_to_foreign_sheet = excel_data.add_sheet("已废弃")
    deprecated_chs_to_foreign_sheet.write(0, 0, u"中文")
    deprecated_chs_to_foreign_sheet.write(0, 1, u"外文")

    # 写入已翻译记录
    r = 2
    dr = 2
    g_old_chs_to_foreign_lang_ref_list = G_OLD_CHS_TO_FOREIGN_LANG_REF_DICT.items()
    g_old_chs_to_foreign_lang_ref_list.sort()
    for (chs_str, ref_count) in g_old_chs_to_foreign_lang_ref_list:
        foreign_str = G_OLD_CHS_TO_FOREIGN_LANG_DICT[chs_str]
        if ref_count > 0:
            old_chs_to_foreign_sheet.write(r, 0, chs_str.decode('utf-8').encode('utf-8'))        
            old_chs_to_foreign_sheet.write(r, 1, foreign_str.decode('utf-8').encode('utf-8'))
            r += 1
        else:
            deprecated_chs_to_foreign_sheet.write(dr, 0, chs_str.decode('utf-8').encode('utf-8'))        
            deprecated_chs_to_foreign_sheet.write(dr, 1, foreign_str.decode('utf-8').encode('utf-8'))            
            dr += 1
        pass
    pass

    # 写入新增、变化记录    
    r = 2
    g_new_chs_to_foreign_lang_list = G_NEW_CHS_TO_FOREIGN_LANG_DICT.keys()
    g_new_chs_to_foreign_lang_list.sort()
    for chs_str in g_new_chs_to_foreign_lang_list:
        new_chs_to_foreign_sheet.write(r, 0, chs_str.decode('utf-8').encode('utf-8'))
        if not G_OLD_CHS_TO_FOREIGN_LANG_ILLEGAL_DICT.has_key(chs_str):
            new_chs_to_foreign_sheet.write(r, 1, u" ")
        else:
            illegal_foreign_str = G_OLD_CHS_TO_FOREIGN_LANG_ILLEGAL_DICT[chs_str]
            new_chs_to_foreign_sheet.write(r, 1, illegal_foreign_str.decode('utf-8').encode('utf-8'))
        pass
        r += 1
    pass

    excel_data.save(file_name)    
    
def FanyiFile(src_file_path, dest_file_path, zip_op):
    #print src_file_path, "=>", dest_file_path
    
    tmp_src_file = open(src_file_path,"rb+")
    src_file_info = tmp_src_file.readlines()
    tmp_src_file.close()
    
    tmp_dest_file = open(dest_file_path,"wb")
    tmp_dest_file.truncate(0)
    tmp_dest_file.seek(0,0)
    
    for tmp_str in src_file_info:
        only_tmp_str = OnlyStr(tmp_str)
        if not IsNeedFanyi(only_tmp_str):
            tmp_dest_file.write(tmp_str)
            continue

        only_chs_str = GetRealCnfStr(only_tmp_str)
        if not G_OLD_CHS_TO_FOREIGN_LANG_DICT.has_key(only_chs_str):
            G_NEW_CHS_TO_FOREIGN_LANG_DICT[only_chs_str] = True
            tmp_dest_file.write(tmp_str)
            pass

        else:
            only_foreign_str = G_OLD_CHS_TO_FOREIGN_LANG_DICT[only_chs_str]
            G_OLD_CHS_TO_FOREIGN_LANG_REF_DICT[only_chs_str] += 1

            '''
            # 检查是否翻译格式错误
            tmp_ret_1 = re.sub("%d|%s|%f","@",only_tmp_str)
            tmp_ret_1 = re.sub("，",",",tmp_ret_1)
            #tmp_ret_1 = re.sub("“","\"",tmp_ret_1)
            #tmp_ret_1 = re.sub("”","\"",tmp_ret_1)
            tmp_ret_1 = re.sub("[^[\]{}<>='@#$%\^&*\\\";]","",tmp_ret_1)
            tmp_ret_2 = re.sub("%d|%s|%f","@",only_foreign_str)
            tmp_ret_2 = re.sub("，",",",tmp_ret_2)
            #tmp_ret_2 = re.sub("“","\"",tmp_ret_2)
            #tmp_ret_2 = re.sub("”","\"",tmp_ret_2)
            tmp_ret_2 = re.sub("[^[\]{}<>='@#$%\^&*\\\";]","",tmp_ret_2)
            if (tmp_ret_1!=tmp_ret_2) and not ((tmp_ret_1=="=\"\"" and tmp_ret_2=="=[[]]") or (tmp_ret_2=="=\"\"" and tmp_ret_1=="=[[]]")):
                print "[ErrorFormat]%s\t%s"%(tmp_ret_1,tmp_ret_2)
                print "\t\t%s"%only_tmp_str
                print "\t\t%s"%only_foreign_str
                pass
            '''
            tmp_dest_file.write(tmp_str.replace(only_chs_str, only_foreign_str))
            pass
        pass
    tmp_dest_file.close()

    os.system("svn add %s --force"%dest_file_path)

    # if zip_op:
    #     zcnf_full_path = dest_file_path.replace(".cnf", ".zcnf")
    #     os.utime(dest_file_path, ZipTimes)
    #     cnf_name = os.path.basename(dest_file_path)
    #     zip_cnf_file = zipfile.ZipFile(zcnf_full_path, "w", zipfile.ZIP_DEFLATED)
    #     zip_cnf_file.write(dest_file_path, cnf_name)
    #     zip_cnf_file.close()
    #     os.remove(dest_file_path)
    # pass

def GetPath(conf_path, path, area, language, format_str):
    if format_str == "p" :
        return conf_path%(path)
    elif format_str == "pl" :
        return conf_path%(path, language)
    elif format_str == "pal":
        return conf_path%(path, area, language)
    return conf_path%(path, area)

def Handle(area, language, tmp_src_path, tmp_dest_path):            
    # G_SRC_PATH = G_SRC_PATH + options.tag

    for cnf_path in G_FANYI_PATH:
        src_tmp_path = cnf_path[0]
        src_format_str = cnf_path[1]
        dest_tmp_path = cnf_path[2]
        dest_format_str = cnf_path[3]
        
        src_path = GetPath(src_tmp_path, tmp_src_path, area, language, src_format_str)
        dest_path = GetPath(dest_tmp_path, tmp_dest_path, area, language, dest_format_str)
        if not os.path.exists(dest_path):
            os.makedirs(dest_path)
            os.system("svn add %s"%os.path.dirname(dest_path))
            os.system("svn add %s"%dest_path)
            pass

        #src_path = cnf_path[0]%G_SRC_PATH
        #dest_path = cnf_path[1]%G_DEST_PATH

        match_str = ""
        zip_op = False
        #字符匹配
        if len(cnf_path) > 4:
            match_str = cnf_path[4]
            pass
        #压缩操作
        if len(cnf_path) > 5:
            zip_op = True
            pass
        
        # 判断是否为文件夹
        if not os.path.isfile(src_path):
            src_path = CheckEndWithOsSep(src_path)
            if os.path.exists(dest_path):
                shutil.rmtree(dest_path)
                pass
            os.mkdir(dest_path)
            dest_path = CheckEndWithOsSep(dest_path)
            pass
        else:
            if os.path.exists(dest_path):
                os.remove(dest_path)
                pass
            FanyiFile(src_path, dest_path, zip_op)
            continue
        
        for dir_info in os.walk(src_path):
            if dir_info[0].find(".svn")!=-1:
                continue
            
            #print dir_info[0]
            
            tmp_path=dir_info[0][len(src_path):]
            if tmp_path != "":
                tmp_path = CheckEndWithOsSep(tmp_path)
                pass

            if not os.path.exists("%s%s"%(dest_path, tmp_path)):
                os.mkdir("%s%s"%(dest_path, tmp_path))
                pass
            
            for i in dir_info[2]:
                #进行匹配分析
                #print("%s%s%s"%(src_path, tmp_path, i))
                if match_str != "":
                    match_ret = re.match(match_str,"%s%s"%(tmp_path, i))
                    if match_ret and len(match_ret.groups())>0:
                        #print "match key[%s] => %s%s"%(match_ret.groups(),tmp_path, i)
                        FanyiFile("%s%s%s"%(src_path, tmp_path, i), "%s%s%s"%(dest_path, tmp_path, i), zip_op)
                        pass
                    pass
                else:
                    FanyiFile("%s%s%s"%(src_path, tmp_path, i), "%s%s%s"%(dest_path, tmp_path, i), zip_op)
                    pass
            pass
        pass
    pass

if __name__ == "__main__":
    usage = "用法: %prog [-option]"
    parser = OptionParser(usage)

    # 必要参数
    parser.add_option("-a", "--area",          type = "string", dest = "area",         action = "store", help = "必要参数，地区")
    parser.add_option("-l", "--language",    type = "string", dest = "language",     action = "store", help = "必要参数，语言")
    parser.add_option("-m", "--mode",        type = "string", dest = "mode",         action = "store", help = "必要参数，模式")
    parser.add_option("-s",    "--src_path",    type = "string", dest = "src_path",        action = "store", help = "必要参数，源路径")
    parser.add_option("-d",    "--dest_path",    type = "string", dest = "dest_path",    action = "store", help = "必要参数，目标路径")
    
    (options, args) = parser.parse_args()

    if options.area == None :
        parser.error("缺少必要参数 area") 
        pass
    elif options.language == None :
        parser.error("缺少必要参数 language") 
        pass
    elif options.mode == None :
        parser.error("缺少必要参数 mode") 
        pass
    elif options.src_path == None :
        parser.error("缺少必要参数 src_path") 
        pass
    elif options.dest_path == None :
        parser.error("缺少必要参数 dest_path")
        pass

    src_path = "%s/%s"%(CUR_PATH, options.src_path)
    dest_path = "%s/%s"%(CUR_PATH, options.dest_path)

    # 更新翻译后的语言配置
    if options.mode == '2':
        os.system("svn ci %s/cdnfiles -m'更新翻译后的语言配置'"%dest_path)
        os.system("svn ci %s/script -m'更新翻译后的语言配置'"%dest_path)
        os.system("echo 更新翻译后的语言配置")

    else:
        os.system("svn up %s"%CUR_FANYI_PATH)
        os.system("svn up %s/cdnfiles"%dest_path)
        os.system("svn up %s/script"%dest_path)

        LoadExcelData(options.language)
        Handle(options.area, options.language, src_path, dest_path)
        WriteExcelData(options.language)

        os.system("svn ci %s -m'更新翻译记录'"%CUR_FANYI_PATH)
        os.system("echo 更新翻译记录")
    pass
pass