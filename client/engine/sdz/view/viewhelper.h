#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <string>
#include "zqcommon/typedef.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocos2dxext/texturecahceex.h"

#define IMAGE_SUFFIX_NAME	".sdz"
#define MUSIC_SUFFIX_NAME		".sdzm"
#define SOUND_SUFFIX_NAME		".sdzs"
#define SKELETON_SUFFIX_NAME	".ssdz"

namespace lua_tinker
{
	struct table;
};

class ActionTimelineEx;
namespace cocos2d
{
	namespace ui
	{
		class Widget;
	}
}

class ViewHelper
{
public:
	typedef enum{
		RGBAJpg = 0x0F0E0D01,
		RGBJpgAZip = 0x0F0E0D02,
		RGBJpg = 0x0F0E0D03,
		RGBAPng = 0x0F0E0D04,
	}CompressImageType;

	enum GetDataFrmFileType
	{
		GDFFT_CSB = 0,
		GDFFT_ACTION = 1,
	};

public:
	static std::string GetResoursePath(const std::string& path);

	//图片预加载和释放
	static bool LoadImageResource(const std::string &image);
	static bool ReleaseImageResource(const std::string &image);
	static void ReleaseUnusedImageResource(bool print_log, bool for_stop);
	//删除未使用的资源，带保留名单
	static bool ReleaseUnusedImageExcept(lua_tinker::table file_list, ZQ::UINT32 file_list_len, bool print_log = false, bool for_stop = false);
	//获取某种纹理的内存大小
	static unsigned int GetCachedTextureBytes(const std::string &filter);
	//记录当前纹理信息 add by hongyi 2016-1-8
	static void RecordCurTextureInfo();
	//输出上次记录到目前的纹理变化 add by hongyi 2016-1-8
	static void PrintTextureChange();

	//解压sdz图片文件
	static const char* GetImageSuffix();
	static bool UnzipImageFile(const std::string &image_path);
	static bool UnzipImageBuf(unsigned char* file_buf, ssize_t file_size, cocos2d::Image *export_image, bool multiple_alpha = false);
	static void LoadAsyncImageResource(const std::string &image_path, LUA_CALLBACK_DATA call_back, void *callback_data);
	static void LoadAsyncImageResource(const std::string &image_path, ZQ::UINT32 lua_fun_id);
	//图片是否已在缓存中
	static bool IsImageExistInCache(const std::string &image_path);

	// 音效资源后缀
	static const char* GetSoundSuffix();
	// 音乐资源后缀
	static const char* GetMusicSuffix();
	// 骨骼后缀
	static const char* GetSkeletonSuffix();

	//打印所有贴图内存信息
	static void PrintAllTextureCache();

	//从文件创建UI控件
	static cocos2d::ui::Widget* CreateWidgetFromJsonFile(const std::string &fileName);	//支持cocosstudio 1.6
	static cocos2d::ui::Widget* CreateWidgetFromZJsonFile(const std::string& fileName);	//支持cocosstudio 1.6
	static cocos2d::ui::Widget* CreateWidgetFromBinaryFile(const char* fileName);		//支持cocosstudio 1.6

	//从CSB中读取数据并创建控件 add by swordman sue 2016-10-10 
	static cocos2d::ui::Widget* CreateWidgetFromFlatBuffersFile(const char* fileName);		

	//从CSB中读取数据并创建控件(异步) add by swordman sue 2017-3-22
	static void CreateWidgetFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id);

	//从CSB文件创建ActionTimeline add by swordman sue 2016-12-8
	static ActionTimelineEx* CreateActionTimelineFromFlatBuffersFile(const char* fileName);

	//从CSB文件创建ActionTimeline(异步) add by swordman sue 2017-3-22
	static void CreateActionTimelineFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id);

	//从文件加载数据完成回调 add by swordman sue 2017-3-22
	static void OnGetDataFrmFileFinished(GetDataFrmFileType type, const cocos2d::Data& file_data, const std::string& file_name, GameType::UINT32 lua_fun_id);

	//加载UI界面的图片资源
	static bool LoadWidgetImageResource(const std::string &image_path, lua_tinker::table frame_set);
	//释放UI界面的图片资源
	static bool ReleaseWidgetImageResource(const std::string &image_path, bool print_log);

	//合成图片
	static void CombineImageWithZipAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha);
	static void CombineImageWithJpgAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha);

	static cocos2d::ui::Widget* GetDFFWidget() { return s_dff_widget; }
	static ActionTimelineEx* GetDFFAction() { return s_dff_action; }

private:
	static cocos2d::ui::Widget* s_dff_widget;
	static ActionTimelineEx* s_dff_action;
};

#endif
