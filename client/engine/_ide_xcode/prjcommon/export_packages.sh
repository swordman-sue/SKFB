#!/bin/sh

###################################################

cd $(dirname $0)

#工程名
PROJ_NAME="appstore"

#准备导出的dev包列表
DEV_EXPORT_ARR=()

#准备导出的adhoc包列表
ADHOC_EXPORT_ARR=()

#准备导出的dis包列表
DIS_EXPORT_ARR=()

# 打包
function ExportPackage()
{
	# 渠道名
	SDK_NAME=$1
	# dev，adhoc，appstore
	METHOD_NAME=$2
	# 编译模式
	CONFIGURATION="Release"
	# xc工作空间路径
	XC_WORKSPACE_FILE=../${SDK_NAME}/${PROJ_NAME}.xcworkspace
	# 导出配置路径
	PLIST_FILE=../${SDK_NAME}/"ExportOptions_"${METHOD_NAME}".plist"	
	# archive文件路径
	ARCHIVE_FILE="/Users/dearbox/Desktop/appstore_archive/"${SDK_NAME}"_"${METHOD_NAME}.xcarchive
	# ipa文件路径
	EXPORT_PATH="/Users/dearbox/Desktop/appstore_ipa/"${SDK_NAME}"_"${METHOD_NAME}

	# 清理项目
    xcodebuild -workspace ${XC_WORKSPACE_FILE} -scheme ${SDK_NAME} -configuration ${CONFIGURATION} clean

    # archive项目
    xcodebuild -workspace ${XC_WORKSPACE_FILE} -scheme ${SDK_NAME} -configuration ${CONFIGURATION} -archivePath ${ARCHIVE_FILE} archive

	# 导出ipa文件
	xcodebuild -exportArchive -archivePath ${ARCHIVE_FILE} -exportPath ${EXPORT_PATH} -exportOptionsPlist ${PLIST_FILE}
}

function Main()
{
	# 打dev包
	for var in ${DEV_EXPORT_ARR[@]};  
	do 
		ExportPackage $var "dev"
	done  	

	# 打adhoc包
	for var in ${ADHOC_EXPORT_ARR[@]};  
	do 
		ExportPackage $var "adhoc"
	done  

	# 打dis包
	for var in ${DIS_EXPORT_ARR[@]};  
	do  
		ExportPackage $var "appstore"
	done  
}

Main
