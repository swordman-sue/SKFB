#!/bin/sh

# 编译模式
build_mode="Release"

# 输出目录
output_libs_path=../../libs/ios/release

cd $(dirname $0)

###################################################

# 项目导出函数
function HandlePrj()
{
    # 项目名称
    tmp_prj_name=$1

    # 项目目录
    tmp_prj_name_hx=${tmp_prj_name}

    #清理项目
    #xcodebuild -project $tmp_prj_name_hx/$tmp_prj_name_hx.xcodeproj clean

    #编译项目
    xcodebuild -project $tmp_prj_name_hx/$tmp_prj_name_hx.xcodeproj -configuration $build_mode -sdk iphoneos build

    #导出静态库
    build_path=./${tmp_prj_name_hx}/build
    lib_src=${build_path}/${build_mode}-iphoneos/lib${tmp_prj_name}.a
    lib_dst=${output_libs_path}/lib${tmp_prj_name}.a
    cp -f $lib_src $lib_dst

    # lipo -info ${output_libs_path}/lib${tmp_prj_name}.a
}

###################################################
HandlePrj gamelib

