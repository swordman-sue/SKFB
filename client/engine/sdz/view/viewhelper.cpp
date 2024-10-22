
#include "viewhelper.h"
#include "ui/UIWidget.h"
#include "cocostudio/CocoStudio.h"

#include "luatinker/lua_tinker.h"
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "filesystem/filehelper.h"
#include "resmanager/resmanager.h"
#include "zqcommon/logutil.h"

#include "zip/zip.h"
#include "cocos2dxext/ziputilsex.h"
#include "view/font/fontmanager.h"
#include "view/render/batchrenderer.h"
#include "view/skeletondatacache.h"
#include "view/struct.h"
#include "view/action/actiontimelineex.h"
#include "particle/particledatamanager.h"

#define RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
	(unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
	((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
	((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
	((unsigned)(unsigned char)(va) << 24))

void UnzipImageFileCallback(void *callback_data)
{
	if(callback_data == NULL)
		return;

	ZQ::UINT32 *callback_func_id = (ZQ::UINT32*)callback_data;
	LuaEngine::GetInstance()->CallFunction(*callback_func_id, "");
	LuaEngine::GetInstance()->DelLuaFuncID(*callback_func_id);
	delete callback_func_id;
}

// 异步FileUtils::getDataFromFile
class GetDataFrmFileListener : public FileListener
{
public:
	GetDataFrmFileListener(ViewHelper::GetDataFrmFileType type, const char* file_name, GameType::UINT32 lua_fun_id)
		: m_type(type)
		, m_file_name(file_name)
		, m_lua_fun_id(lua_fun_id)
	{

	}

	~GetDataFrmFileListener()
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_lua_fun_id);
	}

	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		if (file_info.op_result != FileAsync::FOR_SUCC)
		{
			LogUtil::LogError("[GetDataFrmFileListener::OnCallBack] get data from file failed. file_name:%s op_result:%d", file_info.path.c_str(), file_info.op_result);
		}
		ViewHelper::OnGetDataFrmFileFinished(m_type, file_info.file_data, m_file_name, m_lua_fun_id);
	};
	virtual void Free()
	{
		delete this;
	}

private:
	ViewHelper::GetDataFrmFileType m_type;
	std::string m_file_name;
	GameType::UINT32 m_lua_fun_id;
};

cocos2d::ui::Widget* ViewHelper::s_dff_widget = nullptr;
ActionTimelineEx* ViewHelper::s_dff_action = nullptr;

std::string ViewHelper::GetResoursePath(const std::string& path)
{
	return FileSystemEx::GetInstance()->GetResourcePath(path);
}

bool ViewHelper::LoadImageResource(const std::string &image)
{
	if(image == "")
	{
		return false;
	}
	
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image.c_str());
	if(cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(full_name.c_str()))
	{
		//图片已经在内存里，返回
		return true;
	}

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(image.c_str());

	std::string compress_suffix_name = IMAGE_SUFFIX_NAME;
	if(image.rfind(compress_suffix_name) == image.length() - compress_suffix_name.length())
	{
		return UnzipImageFile(image);
	}
	else
	{
		if(!cocos2d::CCTextureCache::sharedTextureCache()->addImage(full_name.c_str()))
		{
			return false;
		}
	}

	return true;
}

void ViewHelper::ReleaseUnusedImageResource(bool print_log, bool for_stop)
{
	FontManager::sharedFontManager()->clearAllFonts();
	BatchRendererManager::sharedRendererManager()->setDirty(true);
	SkeletonDataCache::GetInstance()->RemoveUnusedSkeletonDataCache(print_log);
	cocostudio::timeline::ActionTimelineCache::getInstance()->purge();

	if (for_stop)
	{
		cocos2d::PSManager::GetInstance()->DestoryAllTemplateSystem();
	}
	cocos2d::CCDirector::getInstance()->purgeCachedDataEx(print_log, for_stop);
}

bool ViewHelper::ReleaseUnusedImageExcept(lua_tinker::table file_list, ZQ::UINT32 file_list_len, bool print_log, bool for_stop)
{
	if (!lua_tinker::is_table(file_list))
	{
		return false;
	}

	//将保留的资源计数器加1
	lua_State* L = file_list.m_obj->m_L;
	lua_pushnil(L);
	while (0 != lua_next(L, file_list.m_obj->m_index))
	{
		ZQ::INT32 id = lua_tointeger(L, -2);
		std::string file_name = lua_tostring(L, -1);

		std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(file_name);
		std::string skeleton_type = SKELETON_SUFFIX_NAME;

		if (full_name.rfind(skeleton_type) == full_name.length() - skeleton_type.length())
		{
			if (SkeletonDataCache::GetInstance()->IsSSdzDataInCache(full_name))
			{
				SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSsdzSkeletonData(full_name);
				skeleton_data->retain();
			}
		}
		else
		{
			cocos2d::CCTexture2D *texture = cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(full_name.c_str());
			if (texture)
			{
				texture->retain();

				//保留纹理也同时保留其frame
				cocos2d::SpriteFrameCache::getInstance()->retainSpriteFrameFromTexture(texture);
			}
		}
		lua_pop(L, 1);
	}

	ReleaseUnusedImageResource(print_log, for_stop);

	//恢复保留资源的计数器
	lua_pushnil(L);
	while (0 != lua_next(L, file_list.m_obj->m_index))
	{
		ZQ::INT32 id = lua_tointeger(L, -2);
		std::string file_name = lua_tostring(L, -1);

		std::string skeleton_type = SKELETON_SUFFIX_NAME;
		std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(file_name);
		if (full_name.rfind(skeleton_type) == full_name.length() - skeleton_type.length())
		{
			if (SkeletonDataCache::GetInstance()->IsSSdzDataInCache(full_name))
			{
				SkeletonData *skeleton_data = SkeletonDataCache::GetInstance()->LoadSsdzSkeletonData(full_name);
				skeleton_data->setAutoClearCache(false);
				skeleton_data->release();
				skeleton_data->setAutoClearCache(true);
			}
		}
		else
		{
			cocos2d::CCTexture2D *texture = cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(full_name.c_str());
			if (texture)
			{
				texture->setAutoClearCache(false);
				texture->release();
				texture->setAutoClearCache(true);

				//恢复保留纹理的frame计数
				cocos2d::SpriteFrameCache::getInstance()->releaseSpriteFrameFromTexture(texture);
			}
		}

		lua_pop(L, 1);
	}

	return true;
}

bool ViewHelper::ReleaseImageResource(const std::string &image)
{
	if(image == "")
	{
		return false;
	}

	std::string image_file = FileSystemEx::GetInstance()->GetResourcePath(image);
	cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(image_file.c_str());
	return true;
}

//加载UI界面的图片资源
bool ViewHelper::LoadWidgetImageResource(const std::string &image_path, lua_tinker::table frame_set)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_path);
	cocos2d::Texture2D *texture = cocos2d::Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if (texture != NULL && texture->isCreatedFrame())
	{
		return false;
	}

	//读取图片
	if (texture == NULL)
	{
		if (!ViewHelper::LoadImageResource(image_path))
		{
			LogUtil::LogError("[UIManager::LoadUIResource]:LoadImageResource error");
			return false;
		}
		texture = cocos2d::Director::getInstance()->getTextureCache()->textureForKey(full_name.c_str());
	}

	if (!lua_tinker::is_table(frame_set))
	{
		LogUtil::LogError("[UIManager::LoadUIResource]:frame_set is nil or not table");
		return false;
	}

	lua_State* L = frame_set.m_obj->m_L;

	lua_pushnil(L);

	int frame_count = 0;
	while (0 != lua_next(L, frame_set.m_obj->m_index))
	{
		ImageData* image_data = lua_tinker::read<ImageData*>(L, -1);
		lua_pop(L, 1);

		if (!image_data)
			continue;

		if (cocos2d::CCSpriteFrameCache::getInstance()->getSpriteFrameByName(image_data->clip_image_name.c_str()))
		{
			continue;
		}

		cocos2d::SpriteFrame *sprite_frame = cocos2d::SpriteFrame::createWithTexture(texture, cocos2d::Rect(image_data->x, image_data->y, image_data->width, image_data->height));
		cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(sprite_frame, image_data->clip_image_name.c_str());
		frame_count++;
	}
	texture->setCreatedFrame(true);
	if (texture->getTexturePath() == "")
	{
		LogUtil::LogError("[UIManager::LoadUIResource]:texturePath is emtpy");
	}
	cocos2d::SpriteFrameCache::getInstance()->setTextureFrameCount(texture->getTexturePath(), frame_count);

	return true;
}

//释放UI界面
bool ViewHelper::ReleaseWidgetImageResource(const std::string &image_path, bool print_log)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_path);
	cocos2d::CCTexture2D *texture = cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(full_name.c_str());

	//所绑定的图片已经被释放
	if (texture == NULL)
		return true;

	texture->setCreatedFrame(false);
	if (texture->getTexturePath() == "")
	{
		LogUtil::LogError("[UIManager::ReleaseUIResource]:texturePath is emtpy");
	}
	cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->clearTextureFrameCount(texture->getTexturePath());
	cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromTexture(texture, print_log);
	return true;
}

unsigned int ViewHelper::GetCachedTextureBytes(const std::string &filter)
{
	return cocos2d::CCTextureCache::sharedTextureCache()->getCachedTextureBytes(filter);
}

void ViewHelper::RecordCurTextureInfo()
{
	std::string log_str = cocos2d::CCTextureCache::sharedTextureCache()->getCachedTextureInfo();
	LogUtil::LogInfo("%s", log_str.c_str());

	cocos2d::CCTextureCache::sharedTextureCache()->recordCurTextureInfo();
}

void ViewHelper::PrintTextureChange()
{
	cocos2d::CCTextureCache::sharedTextureCache()->printTextureChange();
}

const char* ViewHelper::GetImageSuffix()
{
	return IMAGE_SUFFIX_NAME;
}

const char* ViewHelper::GetSoundSuffix()
{
	return SOUND_SUFFIX_NAME;
}

const char* ViewHelper::GetMusicSuffix()
{
	return MUSIC_SUFFIX_NAME;
}

const char* ViewHelper::GetSkeletonSuffix()
{
	return SKELETON_SUFFIX_NAME;
}

bool ViewHelper::UnzipImageBuf(unsigned char* file_buf, ssize_t file_size, cocos2d::Image *export_image, bool multiple_alpha)
{
	unsigned char* image_buf = file_buf;

	//跳过标志位
	unsigned int signature = *(unsigned int*)(file_buf);
	if(signature == RGBJpg || signature == RGBAPng || signature == RGBAJpg || signature == RGBJpgAZip)
	{
		image_buf = file_buf + 4;
		file_size = file_size - 4;
	}

	switch(signature)
	{
	//jpg
	case RGBJpg:
		if (!export_image->initWithImageData(image_buf, file_size))
		{
			LogUtil::LogError("[ViewHelper::UnzipImageBuf]:convert sdz to jpg data failed");
			return false;
		}
		break;
	//png
	case RGBAPng:
		if (!export_image->initWithImageData(image_buf, file_size))
		{
			LogUtil::LogError("[ViewHelper::UnzipImageBuf]:convert sdz to png data failed");
			return false;
		}
		break;
		//双jpg合成
	case RGBAJpg:
		{
			unsigned int width, height;
			unsigned char *export_data;

			ZQ::UINT32 jpg2_size =  *((ZQ::UINT32*)(image_buf + file_size - 4));
			ZQ::UINT32 jpg1_size = file_size - jpg2_size - 4;
			unsigned char *jpg2_buf = image_buf + jpg1_size;

			cocos2d::Image jpg1_image;
			if (!jpg1_image.initWithImageData(image_buf, jpg1_size))
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf]:load sdz rgb jpg data failed");
				return false;
			}

			width = jpg1_image.getWidth();
			height = jpg1_image.getHeight();

			cocos2d::Image jpg2_image;
			if (!jpg2_image.initWithImageData(jpg2_buf, jpg2_size))
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf]:load sdz alpha jpg data failed");
				return false;
			}

			//jpg2为压缩的alpha数据
			unsigned char *source_data = jpg1_image.getData();
			unsigned char *alpha_data = jpg2_image.getData();
			export_data = new unsigned char[width * height * 4];
			CombineImageWithJpgAlpha(width, height, source_data, alpha_data, export_data, multiple_alpha);

			//导出合成后的png数据
			if (!export_image->initWithRawData(export_data, width * height * 4, width, height, 8))
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf]:initWithImageData sdz image error");
				delete[] export_data;
				return false;
			}
			delete[] export_data;
			break;
		}
	//普通sdz压缩，alpha值使用zip压缩	 RGBJpgAZip 与无格式标识
	//RGBJpgAZip
	default:
		{
			unsigned int width, height;
			unsigned char *export_data;

			ZQ::UINT32 alpha_data_size = *((ZQ::UINT32*)(image_buf + file_size - 4));
			ZQ::UINT32 jpg_data_size = file_size - alpha_data_size - 4;
			unsigned char *alpha_buf = image_buf + jpg_data_size;

			//读取jpg数据
			cocos2d::Image jpg_image;

			if (!jpg_image.initWithImageData(image_buf, jpg_data_size))
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf] load sdz rgb jpg data failed");
				return false;
			}

			width = jpg_image.getWidth();
			height = jpg_image.getHeight();

			//解压并获取alpha数据
			unsigned char *source_data = jpg_image.getData();
			unsigned char *unzip_alpha_data = new unsigned char[width * height];
			export_data = new unsigned char[width * height * 4];
			int unzip_size = ZipUtilsEx::InflateGZipMemory(unzip_alpha_data, width * height, alpha_buf, alpha_data_size);
			if(unzip_size != width * height)
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf] load sdz alpha data error");
				delete[] unzip_alpha_data;
				delete[] export_data;
				return false;
			}

			CombineImageWithZipAlpha(width, height, source_data, unzip_alpha_data, export_data, multiple_alpha);
			delete[] unzip_alpha_data;

			//导出合成后的png数据
			if (!export_image->initWithRawData(export_data, width * height * 4, width, height, 8))
			{
				LogUtil::LogError("[ViewHelper::UnzipImageBuf]:initWithImageData sdz image error");
				delete[] export_data;
				return false;
			}
			delete[] export_data;
		}
	}

	return true;
}

bool ViewHelper::UnzipImageFile(const std::string &image_path)
{
	std::string type_name = IMAGE_SUFFIX_NAME;
	ZQ::UINT32 typename_pos = image_path.rfind(type_name.c_str());
	
	//非压缩文件
	if(typename_pos != image_path.length() - type_name.length())
	{
		LogUtil::LogError("ViewHelper::UnzipImageFile:not sdz zip type");
		return false;
	}
	
	//查看是否已经在缓存内
	std::string png_image_path = FileSystemEx::GetInstance()->GetResourcePath(image_path);
	if(cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(png_image_path.c_str()))
	{
		return true;
	}

	//加载压缩文件
	std::string full_image_path = FileSystemEx::GetInstance()->GetResourcePath(image_path);
	ssize_t file_size;
	unsigned char *file_buf = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(full_image_path.c_str(), "rb", &file_size);
	
	if(NULL == file_buf)
	{
		LogUtil::LogError("[ViewHelper::UnzipImageFile] load sdz file %s failed", image_path.c_str());
		return false;
	}

	cocos2d::CCImage* export_image = new cocos2d::CCImage();
	bool unzip_ret = UnzipImageBuf(file_buf, file_size, export_image, false);
	delete[] file_buf;

	bool ret = true;
	if(!cocos2d::CCTextureCache::sharedTextureCache()->addUIImage(export_image, png_image_path.c_str()))
	{
		LogUtil::LogError("[ViewHelper::UnzipImageBuf] addUIImage %s failed", png_image_path.c_str());
		ret = false;
	}
	export_image->release();

	return unzip_ret;
}

void ViewHelper::LoadAsyncImageResource(const std::string &image_path, LUA_CALLBACK_DATA call_back, void* callback_data)
{
	std::string full_image_path = FileSystemEx::GetInstance()->GetResourcePath(image_path);
	TextureCacheEx::GetInstance()->AddSdzImageAsync(full_image_path.c_str(), call_back, callback_data);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(image_path.c_str());
}

//异步解压图片
void ViewHelper::LoadAsyncImageResource(const std::string& image_path, ZQ::UINT32 lua_fun_id)
{
	ZQ::UINT32 *callback_func_id = new ZQ::UINT32(lua_fun_id);
	if(IsImageExistInCache(image_path))
	{
		UnzipImageFileCallback((void*)callback_func_id);
		return;
	}

	LoadAsyncImageResource(image_path, UnzipImageFileCallback, (void*)callback_func_id);
}

//图片是否已在缓存中
bool ViewHelper::IsImageExistInCache(const std::string &image_path)
{
	if(image_path == "")
		return false;

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(image_path.c_str());
	
	//图片不在内存里
	if(!cocos2d::CCTextureCache::sharedTextureCache()->textureForKey(full_name.c_str()))
	{	
		return false;
	}
	
	return true;
}

//打印所有贴图内存信息
void ViewHelper::PrintAllTextureCache()
{
	// 添加调试输出 add by hongxiang 
	cocos2d::Director::getInstance()->getTextureCache()->getCachedTextureInfo();
}

//从JSON文件创建UI控件
cocos2d::ui::Widget* ViewHelper::CreateWidgetFromJsonFile(const std::string &fileName)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	return cocostudio::GUIReader::getInstance()->widgetFromJsonFile(full_name.c_str());
}

//从JSON压缩文件创建UI控件
cocos2d::ui::Widget* ViewHelper::CreateWidgetFromZJsonFile(const std::string& fileName)
{
	std::string::size_type last_index = fileName.find_last_of("/");
	std::string suffix_name = ".zjson";
	std::string json_file_name = fileName.substr(last_index + 1, fileName.length() - last_index - suffix_name.length() - 1);
	json_file_name = json_file_name + ".ExportJson";

	Zip zip;
	void *buffer = 0;
	int unzip_buf_size;
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	if (!zip.Unzip(full_name.c_str(), 0))
	{
		LogUtil::LogError("CreateWidgetFromZJsonFile un zip %s file fail", full_name.c_str());
		return NULL;
	}
	if (!zip.GetUzFileData(json_file_name, &buffer, &unzip_buf_size))
	{
		LogUtil::LogError("CreateWidgetFromZJsonFile get zip data %s fail,unzip_buf_size:%d", json_file_name.c_str(), unzip_buf_size);
		return NULL;
	}
	return cocostudio::GUIReader::getInstance()->widgetFromZJsonFile(std::string((const char*)buffer, unzip_buf_size), full_name.c_str());
}

//从CSB文件创建UI控件
cocos2d::ui::Widget* ViewHelper::CreateWidgetFromFlatBuffersFile(const char* fileName)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(fileName);

	return cocostudio::GUIReader::getInstance()->widgetFromFlatBuffersFile(full_name);
}

//从CSB中读取数据并创建控件(异步)
void ViewHelper::CreateWidgetFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	FileSystemEx::GetInstance()->GetDataFrmFileAsync(full_name, new GetDataFrmFileListener(GDFFT_CSB, full_name.c_str(), lua_fun_id));

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(fileName);

}

//从二进制文件创建UI控件
cocos2d::ui::Widget* ViewHelper::CreateWidgetFromBinaryFile(const char* fileName)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	return cocostudio::GUIReader::getInstance()->widgetFromBinaryFile(full_name.c_str());
}

//从CSB文件创建ActionTimeline
ActionTimelineEx* ViewHelper::CreateActionTimelineFromFlatBuffersFile(const char* fileName)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	ActionTimelineEx* pAction = dynamic_cast<ActionTimelineEx*>(cocostudio::GUIReader::getInstance()->actionTimelineFromFlatBuffersFile(full_name));
	return pAction;
}

//从CSB文件创建ActionTimeline(异步)
void ViewHelper::CreateActionTimelineFromFlatBuffersFileAsync(const char* fileName, GameType::UINT32 lua_fun_id)
{
	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(fileName);
	FileSystemEx::GetInstance()->GetDataFrmFileAsync(full_name, new GetDataFrmFileListener(GDFFT_ACTION, full_name.c_str(), lua_fun_id));
}

//从文件加载数据完成回调
void ViewHelper::OnGetDataFrmFileFinished(GetDataFrmFileType type, const cocos2d::Data& file_data, const std::string& file_name, GameType::UINT32 lua_fun_id)
{
	switch (type)
	{
	case GDFFT_CSB:
		{
			s_dff_widget = !file_data.isNull() ? cocostudio::GUIReader::getInstance()->widgetFromFlatBuffersFileData(file_data) : nullptr;
			if (nullptr == s_dff_widget)
			{
				LogUtil::LogError("[ViewHelper::OnGetDataFrmFileFinished] widgetFromFlatBuffersFileData failed. file_name:%s file is null:%d", file_name.c_str(), file_data.isNull() ? 1 : 0);
			}
			LuaEngine::GetInstance()->CallFunction(lua_fun_id, "");
			s_dff_widget = nullptr;
		}
		break;

	case GDFFT_ACTION:
		{
			s_dff_action = !file_data.isNull() ? dynamic_cast<ActionTimelineEx*>(cocostudio::GUIReader::getInstance()->actionTimelineFromFlatBuffersFileData(file_data, file_name)) : nullptr;
			if (nullptr == s_dff_action)
			{
				LogUtil::LogError("[ViewHelper::OnGetDataFrmFileFinished] actionTimelineFromFlatBuffersFileData failed. file_name:%s", file_name.c_str());
			}
			LuaEngine::GetInstance()->CallFunction(lua_fun_id, "");
			s_dff_action = nullptr;
		}
		break;
	}
}

void ViewHelper::CombineImageWithZipAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha)
{
	if(rgb_data == NULL || alpha_data == NULL || export_data == NULL)
		return;

	if(multiple_alpha)
	{
		unsigned int *target_data = (unsigned int*)export_data;
		unsigned char color_r, color_g, color_b, color_a;

		//混合alpha数据
		unsigned int index = 0;
		unsigned int data_len = width * height;
		while(index < data_len)
		{
			color_r = *rgb_data++;
			color_g = *rgb_data++;
			color_b = *rgb_data++;
			color_a = *alpha_data++;
			*target_data++ = RGB_PREMULTIPLY_ALPHA(color_r, color_g, color_b, color_a);
			++index;
		}
	}
	else
	{
		unsigned char *target_data = export_data;

		//混合alpha数据
		unsigned int index = 0;
		unsigned int data_len = width * height;
		while(index < data_len)
		{
			*target_data++ = *rgb_data++;
			*target_data++ = *rgb_data++;
			*target_data++ = *rgb_data++;
			*target_data++ = *alpha_data++;
			++index;
		}
	}
}

void ViewHelper::CombineImageWithJpgAlpha(unsigned int width, unsigned int height, unsigned char* rgb_data, unsigned char* alpha_data, unsigned char* export_data, bool multiple_alpha)
{
	if(rgb_data == NULL || alpha_data == NULL || export_data == NULL)
		return;

	if(multiple_alpha)
	{
		unsigned int *target_data = (unsigned int*)export_data;
		unsigned char color_r, color_g, color_b, color_a;

		//混合alpha数据
		unsigned int index = 0;
		unsigned int data_len = width * height;
		while(index < data_len)
		{
			color_r = *rgb_data++;
			color_g = *rgb_data++;
			color_b = *rgb_data++;
			color_a = *alpha_data++;
			*target_data++ = RGB_PREMULTIPLY_ALPHA(color_r, color_g, color_b, color_a);
			++alpha_data;
			++alpha_data;
			++index;
		}
	}
	else
	{
		unsigned char *target_data = export_data;

		//混合alpha数据
		unsigned int index = 0;
		unsigned int data_len = width * height;
		while(index < data_len)
		{
			*target_data++ = *rgb_data++;
			*target_data++ = *rgb_data++;
			*target_data++ = *rgb_data++;
			*target_data++ = *alpha_data++;
			++alpha_data;
			++alpha_data;
			++index;
		}
	}
}

