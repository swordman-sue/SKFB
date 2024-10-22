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

	//ͼƬԤ���غ��ͷ�
	static bool LoadImageResource(const std::string &image);
	static bool ReleaseImageResource(const std::string &image);
	static void ReleaseUnusedImageResource(bool print_log, bool for_stop);
	//ɾ��δʹ�õ���Դ������������
	static bool ReleaseUnusedImageExcept(lua_tinker::table file_list, ZQ::UINT32 file_list_len, bool print_log = false, bool for_stop = false);
	//��ȡĳ��������ڴ��С
	static unsigned int GetCachedTextureBytes(const std::string &filter);
	//��¼��ǰ������Ϣ add by hongyi 2016-1-8
	static void RecordCurTextureInfo();
	//����ϴμ�¼��Ŀǰ������仯 add by hongyi 2016-1-8
	static void PrintTextureChange();

	//��ѹsdzͼƬ�ļ�
	static const char* GetImageSuffix();
	static bool UnzipImageFile(const std::string &image_path);
	static bool UnzipImageBuf(unsigned char* file_buf, ssize_t file_size, cocos2d::Image *export_image, bool multiple_alpha = false);
	static void LoadAsyncImageResource(const std::string &image_path, LUA_CALLBACK_DATA call_back, void *callback_data);
	static void LoadAsyncImageResource(const std::string &image_path, ZQ::UINT32 lua_fun_id);
	//ͼƬ�Ƿ����ڻ�����
	static bool IsImageExistInCache(const std::string &image_path);

	// ��Ч��Դ��׺
	static const char* GetSoundSuffix();
	// ������Դ��׺
	static const char* GetMusicSuffix();
	// ������׺
	static const char* GetSkeletonSuffix();

	//��ӡ������ͼ�ڴ���Ϣ
	static void PrintAllTextureCache();

	//���ļ�����UI�ؼ�
	static cocos2d::ui::Widget* CreateWidgetFromJsonFile(const std::string &fileName);	//֧��cocosstudio 1.6
	static cocos2d::ui::Widget* CreateWidgetFromZJsonFile(const std::string& fileName);	//֧��cocosstudio 1.6
	static cocos2d::ui::Widget* CreateWidgetFromBinaryFile(const char* fileName);		//֧��cocosstudio 1.6

	//��CSB�ж�ȡ���ݲ������ؼ� add by swordman sue 2016-10-10 
	static cocos2d::ui::Widget* CreateWidgetFromFlatBuffersFile(const char* fileName);		

	//��CSB�ж�ȡ���ݲ������ؼ�(�첽) add by swordman sue 2017-3-22
	static void CreateWidgetFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id);

	//��CSB�ļ�����ActionTimeline add by swordman sue 2016-12-8
	static ActionTimelineEx* CreateActionTimelineFromFlatBuffersFile(const char* fileName);

	//��CSB�ļ�����ActionTimeline(�첽) add by swordman sue 2017-3-22
	static void CreateActionTimelineFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id);

	//���ļ�����������ɻص� add by swordman sue 2017-3-22
	static void OnGetDataFrmFileFinished(GetDataFrmFileType type, const cocos2d::Data& file_data, const std::string& file_name, GameType::UINT32 lua_fun_id);

	//����UI�����ͼƬ��Դ
	static bool LoadWidgetImageResource(const std::string &image_path, lua_tinker::table frame_set);
	//�ͷ�UI�����ͼƬ��Դ
	static bool ReleaseWidgetImageResource(const std::string &image_path, bool print_log);

	//�ϳ�ͼƬ
	static void CombineImageWithZipAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha);
	static void CombineImageWithJpgAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha);

	static cocos2d::ui::Widget* GetDFFWidget() { return s_dff_widget; }
	static ActionTimelineEx* GetDFFAction() { return s_dff_action; }

private:
	static cocos2d::ui::Widget* s_dff_widget;
	static ActionTimelineEx* s_dff_action;
};

#endif
