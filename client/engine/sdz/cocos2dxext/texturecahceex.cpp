#include "texturecahceex.h"
#include "cocos2d.h"
#include "cocos2dxext/ziputilsex.h"
#include <errno.h>
#include <stack>
#include <string>
#include <cctype>
#include <queue>
#include <list>
#include <thread>
#include "view/viewhelper.h"
#include "resmanager/resmanager.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"

using namespace cocos2d;

typedef struct _SdzAsyncCallBack
{
	LUA_CALLBACK_DATA	callback_fun;
	void*				callback_data;
}SdzAsyncCallBack;

typedef struct _SdzAsyncStruct
{
	std::string	filename;
	std::vector<SdzAsyncCallBack> sdzCallBackArray;
	bool bLoadSucceed;
	short hRetryCount;
}SdzAsyncStruct;

typedef struct _SdzImageInfo
{
	SdzAsyncStruct			*asyncStruct;
	CCImage					*image;
}SdzImageInfo;

static std::thread* s_sdzLoadingThread;

static std::mutex		s_SdzSleepMutex;
static std::condition_variable		s_SdzSleepCondition;

static std::mutex      s_sdzAsyncListMutex;
static std::mutex      s_SdzImageQueueMutex;

static unsigned long s_sdzAsyncLoadingCount = 0;
static bool sdz_loading_thread_quit = false;

static std::list<SdzAsyncStruct*>*	s_pSdzAsyncList = NULL;
static std::queue<SdzImageInfo*>*   s_pSdzImageQueue = NULL;

const std::string s_sdz_type_name = ".sdz";

static CCImage::Format computeImageFormatType(std::string& filename)
{
	CCImage::Format ret = CCImage::Format::UNKNOWN;

	if ((std::string::npos != filename.find(".jpg")) || (std::string::npos != filename.find(".jpeg")))
	{
		ret = CCImage::Format::JPG;
	}
	else if ((std::string::npos != filename.find(".png")) || (std::string::npos != filename.find(".PNG")))
	{
		ret = CCImage::Format::PNG;
	}
	else if ((std::string::npos != filename.find(".tiff")) || (std::string::npos != filename.find(".TIFF")))
	{
		ret = CCImage::Format::TIFF;
	}
	else if ((std::string::npos != filename.find(".sdz")) || (std::string::npos != filename.find(".SDZ")))
	{
		ret = CCImage::Format::SDZ;
	}
	else if ( (std::string::npos != filename.find(".tga")) || (std::string::npos != filename.find(".TGA")) )
	{
		ret = Image::Format::TGA;
	}
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT) && (CC_TARGET_PLATFORM != CC_PLATFORM_WP8)
	else if ((std::string::npos != filename.find(".webp")) || (std::string::npos != filename.find(".WEBP")))
	{
		ret = CCImage::Format::WEBP;
	}
#endif

	return ret;
}

static bool UnzipSdzImageData(SdzAsyncStruct *pAsyncStruct, CCImage *pImage)
{
	ssize_t file_size;
	unsigned char *file_buf = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(pAsyncStruct->filename.c_str(), "rb", &file_size);
	if(NULL == file_buf)
	{
		LogUtil::LogError("CCTextureCache::UnzipSdzImageData:load sdz file :%s failed", pAsyncStruct->filename.c_str());
		return false;
	}

	bool unzip_ret = ViewHelper::UnzipImageBuf(file_buf, file_size, pImage, false);
	delete[] file_buf;

	return unzip_ret;
}

static void loadSdzImageData(SdzAsyncStruct *pAsyncStruct)
{
	const char *filename = pAsyncStruct->filename.c_str();
	pAsyncStruct->bLoadSucceed = true;

	// compute image type
	CCImage::Format imageType = computeImageFormatType(pAsyncStruct->filename);
	CCImage *pImage = new CCImage();

	switch(imageType)
	{
	case CCImage::Format::UNKNOWN:
		LogUtil::LogError("[loadSdzImageData] unsupported format %s", filename);
		CC_SAFE_RELEASE_NULL(pImage);
		pAsyncStruct->bLoadSucceed = false;
		//delete pAsyncStruct;
		//return;
		break;

	case CCImage::Format::SDZ:
		//sdz格式解压后为png格式
		if(!UnzipSdzImageData(pAsyncStruct, pImage))
		{
			LogUtil::LogError("[loadSdzImageData] UnzipSdzImageData failed %s", filename);
			CC_SAFE_RELEASE_NULL(pImage);
			pAsyncStruct->bLoadSucceed = false;
			//delete pAsyncStruct;
			//return;
		}
		break;

	default:
		if (!pImage->initWithImageFileThreadSafe(filename))
		{
			LogUtil::LogError("[loadSdzImageData] initWithImageFileThreadSafe failed %s", filename);
			CC_SAFE_RELEASE_NULL(pImage);
			pAsyncStruct->bLoadSucceed = false;
			//delete pAsyncStruct;
			//return;
		}
		break;
	}

	// generate image info
	SdzImageInfo *pImageInfo = new SdzImageInfo();
	pImageInfo->asyncStruct = pAsyncStruct;
	pImageInfo->image = pImage;

	// put the image info into the queue
	s_SdzImageQueueMutex.lock();
	s_pSdzImageQueue->push(pImageInfo);
	s_SdzImageQueueMutex.unlock();
}

static void* loadSdzImage(void* data)
{
	SdzAsyncStruct *pAsyncStruct = NULL;

	while (true)
	{
		s_sdzAsyncListMutex.lock();
		if (s_pSdzAsyncList->empty())
		{
			s_sdzAsyncListMutex.unlock();
			if (sdz_loading_thread_quit) {
				break;
			}
			else {
				std::unique_lock<std::mutex> lk(s_SdzSleepMutex);
				s_SdzSleepCondition.wait(lk);
				continue;
			}
		}
		else
		{
			pAsyncStruct = s_pSdzAsyncList->front();
			s_pSdzAsyncList->pop_front();
			s_sdzAsyncListMutex.unlock();
			loadSdzImageData(pAsyncStruct);
		}        
	}

	return 0;
}

static TextureCacheEx* s_instance = nullptr;

TextureCacheEx* TextureCacheEx::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new TextureCacheEx;
	}
	return s_instance;
}

void TextureCacheEx::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

TextureCacheEx::TextureCacheEx()
{
}

TextureCacheEx::~TextureCacheEx()
{
}

bool TextureCacheEx::Init()
{
	cocos2d::TextureCache::getInstance()->setCustomTextureCache(this);
	return true;
}

bool TextureCacheEx::Stop()
{
	// 修复s_sdzLoadingThread未销毁以及线程资源回收问题
	if (NULL != s_sdzLoadingThread)
	{
		sdz_loading_thread_quit = true;
		s_SdzSleepCondition.notify_one();
		s_sdzLoadingThread->join();
		delete s_sdzLoadingThread;
		s_sdzLoadingThread = NULL;
	}

	if (NULL != s_pSdzAsyncList)
	{
		SdzAsyncStruct* pAsyncStruct;
		std::list<SdzAsyncStruct*>::iterator iter;
		for (iter = s_pSdzAsyncList->begin(); iter != s_pSdzAsyncList->end(); ++iter)
		{
			pAsyncStruct = *iter;
			delete pAsyncStruct;
		}
		delete s_pSdzAsyncList;
		s_pSdzAsyncList = NULL;
	}

	if (NULL != s_pSdzImageQueue)
	{
		while (!s_pSdzImageQueue->empty())
		{
			s_pSdzImageQueue->pop();
		}
		delete s_pSdzImageQueue;
		s_pSdzImageQueue = NULL;
	}

	return true;
}

void TextureCacheEx::AddSdzImageAsync(const char* path, LUA_CALLBACK_DATA callback_func, void *callback_data)
{
	CCAssert(path != NULL, "TextureCache: fileimage MUST not be NULL");    

	// optimization

	std::string fullpath = CCFileUtils::sharedFileUtils()->fullPathForFilename(path);
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->textureForKey(fullpath.c_str());
	if(texture != NULL)
	{
		if (callback_func)
		{
			callback_func(callback_data);
		}
		return;
	}

	// lazy init
	if (s_pSdzAsyncList == NULL)
	{   
		sdz_loading_thread_quit = false;
		s_pSdzAsyncList = new std::list<SdzAsyncStruct*>();
		s_pSdzImageQueue = new std::queue<SdzImageInfo*>();        
		s_sdzLoadingThread = new std::thread(&loadSdzImage, this);
	}
	if (0 == s_sdzAsyncLoadingCount)
	{
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(TextureCacheEx::AddSdzImageAsyncCallBack), this, 0, false);
	}
	++s_sdzAsyncLoadingCount;

	SdzAsyncStruct *data = NULL;
	bool async_struct_exist = false;

	SdzAsyncCallBack callback;
	callback.callback_fun = callback_func;
	callback.callback_data = callback_data;

	s_sdzAsyncListMutex.lock();
	std::list<SdzAsyncStruct*>::iterator iter;
	for(iter = s_pSdzAsyncList->begin(); iter != s_pSdzAsyncList->end(); ++iter)
	{
		if((*iter)->filename == fullpath)
		{
			data = *iter;
			data->sdzCallBackArray.push_back(callback);
			async_struct_exist = true;
			break;
		}
	}
	s_sdzAsyncListMutex.unlock();

	// add async struct into queue
	if (!data)
	{	
		data = new SdzAsyncStruct();
		data->filename = fullpath;
		data->sdzCallBackArray.push_back(callback);
		data->bLoadSucceed = false;
		data->hRetryCount = 0;
	}

	if (!async_struct_exist)
	{
		s_sdzAsyncListMutex.lock();
		s_pSdzAsyncList->push_back(data);
		s_sdzAsyncListMutex.unlock();
	}
	s_SdzSleepCondition.notify_one();
}

void TextureCacheEx::AddSdzImageAsyncCallBack(float dt)
{
	// the image is generated in loading thread
	std::queue<SdzImageInfo*> *imagesQueue = s_pSdzImageQueue;

	s_SdzImageQueueMutex.lock();
	if (imagesQueue->empty())
	{
		s_SdzImageQueueMutex.unlock();
	}
	else
	{
		SdzImageInfo *pImageInfo = imagesQueue->front();
		imagesQueue->pop();
		s_SdzImageQueueMutex.unlock();

		SdzAsyncStruct *pAsyncStruct = pImageInfo->asyncStruct;
		CCImage *pImage = pImageInfo->image;

		std::string filename = pAsyncStruct->filename;

		// cache the texture
		if (nullptr != pImage)
		{
			CCTextureCache::sharedTextureCache()->addUIImage(pImage, filename.c_str());
			pImage->release();
		}

		if (pAsyncStruct->bLoadSucceed || pAsyncStruct->hRetryCount >= 2)
		{
			std::vector<SdzAsyncCallBack>::iterator iter;
			for (iter = pAsyncStruct->sdzCallBackArray.begin(); iter != pAsyncStruct->sdzCallBackArray.end(); ++iter)
			{
				if (iter->callback_fun)
				{
					iter->callback_fun(iter->callback_data);
				}
			}
			delete pAsyncStruct;
		}
		else
		{
			// 加载贴图失败，重试
			++pAsyncStruct->hRetryCount;
			AddSdzImageAsync(pAsyncStruct->filename.c_str(), nullptr, nullptr);
			LogUtil::LogInfo("[TextureCacheEx::AddSdzImageAsyncCallBack] Retry AddSdzImageAsync filename:%s hRetryCount:%d", pAsyncStruct->filename.c_str(), pAsyncStruct->hRetryCount);
		}

		delete pImageInfo;

		--s_sdzAsyncLoadingCount;
		if (0 == s_sdzAsyncLoadingCount)
		{
			CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(TextureCacheEx::AddSdzImageAsyncCallBack), this);
		}
	}
}

// 动态解压随包资源
void TextureCacheEx::unzipInitResDynamically(const char* file_name)
{
	ResManager::GetInstance()->UnzipInitResDynamically(file_name);
}

// 获取纹理key
std::string TextureCacheEx::getTextureKey(const char* full_path)
{
	std::string init_path;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	init_path = "assets/";
#else
	init_path = FileSystemEx::GetInstance()->GetInitialPath();
#endif
	std::string download_path = FileSystemEx::GetInstance()->GetDownLoadPath();
	std::string tex_key = FileSystemEx::GetInstance()->ConvertPathFormatToUnixStyle(full_path);
	tex_key = ZQ::StrUtil::ReplaceAll(tex_key, download_path, "");
	tex_key = ZQ::StrUtil::ReplaceAll(tex_key, init_path, "");

	return tex_key;
}
