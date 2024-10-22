#include <queue>
#include <list>
#include <vector>

#include "SkeletonDataCache.h"
#include "zqcommon/logutil.h"
#include "spine/extension.h"
#include "filesystem/filesystemex.h"
#include "luaengine/luaengine.h"
#include "resmanager/resmanager.h"
#include "view/viewhelper.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCThread.h"
#include "cocos2dxext/ziputilsex.h"

#include <thread>
using namespace cocos2d;

/////////////////////////////////SkeletonDataCahce//////////////////////////////

//lua函数回调
typedef struct _LoadSsdzObjectFunc
{
	GameType::UINT32 lua_func_id;
}LoadSsdzObjectFunc;

//异步加载压缩ssdz文件+加载图片使用的结构
typedef struct _SsdzAsyncLoadImageData{
	std::string ssdz_file;
	std::vector<LoadSsdzObjectFunc>	callback_array;
}SsdzAsyncLoadImageData;

//同步加载atlas使用的结构
typedef struct _SsdzSyncLoadAtlasData
{
	SsdzAsyncLoadImageData *ssdz_async_load_image_data;
	CCImage *exprot_image;
	std::string ssdz_path;
	std::string ssdz_image_path;
	unsigned char* atlas_buf;
	unsigned int atlas_buf_size;
	unsigned char* json_buf;
	unsigned int json_buf_size;
}SsdzSyncLoadAtlasData;

//异步加载json使用的结构
typedef struct _SsdzAsyncLoadJsonData
{
	SsdzAsyncLoadImageData *ssdz_async_load_image_data;
	spAtlas *sp_atlas;
	unsigned char* json_buf;
	unsigned int json_buf_size;
}SsdzAsyncLoadJsonData;

//加入骨骼缓存+同步回调使用的结构
typedef struct _SsdzSyncAddSkeletonData
{
	SsdzAsyncLoadImageData *ssdz_async_load_image_data;
	spAtlas *sp_atlas;
	spSkeletonData *sp_skeleton;
}SsdzSyncAddSkeletonData;

static std::thread		*s_load_image_thread;
static std::thread		*s_load_json_thread;

static std::mutex					s_image_thread_sleep_mutex;
static std::condition_variable		s_image_thread_sleep_condition;

static std::mutex					s_json_thread_sleep_mutex;
static std::condition_variable		s_json_thread_sleep_condition;

static std::mutex      s_load_image_list_mutex;
static std::mutex      s_load_atlas_queue_mutex;
static std::mutex      s_load_json_queue_mutex;
static std::mutex      s_add_skeleton_queue_mutex;
static std::mutex      s_failed_queue_mutex;

static unsigned long s_async_loading_ssdz_count = 0;
static unsigned long s_async_loading_json_count = 0;
static bool s_load_image_thread_quit = false;
static bool s_load_json_thread_quit = false;

static std::map<std::string, SsdzAsyncLoadImageData*>*	s_load_image_map = NULL;
static std::list<SsdzAsyncLoadImageData*>*	s_load_image_list = NULL;
static std::queue<SsdzSyncLoadAtlasData*>*  s_load_atlas_queue = NULL;
static std::queue<SsdzAsyncLoadJsonData*>*  s_load_json_queue = NULL;
static std::queue<SsdzSyncAddSkeletonData*>*  s_add_skeleton_queue = NULL;
static std::queue<std::string>* s_failed_queue_list;

static void AddFailedFile(const char* file_name)
{
	s_failed_queue_mutex.lock();
	s_failed_queue_list->push(file_name);
	s_failed_queue_mutex.unlock();
}

//加载、解压ssdz文件，读取json、atlas文件数据，读取解码图像数据
static void LoadSsdzFileData(SsdzAsyncLoadImageData *ssdz_image_load_data)
{
	std::string ssdz_file_fullpath = ssdz_image_load_data->ssdz_file;

	ssize_t file_size;
	unsigned char *file_buf = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(ssdz_file_fullpath.c_str(), "rb", &file_size);
	if(NULL == file_buf)
	{
		AddFailedFile(ssdz_file_fullpath.c_str());
		LogUtil::LogError("[SkeletonDataCahce_ssdz.cpp LoadSsdzFileData] load ssdz file %s failed", ssdz_file_fullpath.c_str());
		return;
	}

	//altas
	unsigned int buf_offset = file_size - 4;
	unsigned int altas_zip_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - 4;
	unsigned int altas_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - altas_zip_buf_size;
	unsigned char *altas_zip_buf = file_buf + buf_offset;
	//json
	buf_offset = buf_offset - 4;
	unsigned int json_zip_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - 4;
	unsigned int json_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - json_zip_buf_size;
	unsigned char *json_zip_buf = file_buf + buf_offset;
	//image
	unsigned char* image_buf = file_buf;
	unsigned int image_buf_size = buf_offset;
	unsigned int image_type = *(unsigned int*)(file_buf);

	std::string::size_type suffix_find_pos = ssdz_file_fullpath.find(SKELETON_SUFFIX_NAME);
	std::string skeleton_path = ssdz_file_fullpath.substr(0, suffix_find_pos);
	std::string skeleton_image_key = skeleton_path + "/skeleton.png";

	bool unzip_image_ret = false;
	cocos2d::CCImage* export_image = new cocos2d::CCImage();
	if(image_type == ViewHelper::RGBAPng)
	{
		unzip_image_ret = export_image->initWithImageData(image_buf + 4, image_buf_size - 4);
	}
	else
	{
		unzip_image_ret = ViewHelper::UnzipImageBuf(image_buf, image_buf_size, export_image, true);
	}
	if(!unzip_image_ret)
	{
		delete[] file_buf;
		delete export_image;
		AddFailedFile(ssdz_file_fullpath.c_str());
		LogUtil::LogError("[SkeletonDataCahce_ssdz.cpp LoadSsdzFileData] error reading image buf %s", skeleton_image_key.c_str());
		return;
	}

	unsigned char* altas_buf = new unsigned char[altas_buf_size];
	ZipUtilsEx::InflateGZipMemory(altas_buf, altas_buf_size, altas_zip_buf, altas_zip_buf_size);

	unsigned char* json_buf = new unsigned char[json_buf_size];
	ZipUtilsEx::InflateGZipMemory(json_buf, json_buf_size, json_zip_buf, json_zip_buf_size);

	delete[] file_buf;

	SsdzSyncLoadAtlasData *ssdz_load_atlas_data = new SsdzSyncLoadAtlasData();
	ssdz_load_atlas_data->ssdz_async_load_image_data = ssdz_image_load_data;
	ssdz_load_atlas_data->ssdz_path = skeleton_path;
	ssdz_load_atlas_data->ssdz_image_path = skeleton_image_key;
	ssdz_load_atlas_data->exprot_image = export_image;
	ssdz_load_atlas_data->atlas_buf = altas_buf;
	ssdz_load_atlas_data->atlas_buf_size = altas_buf_size;
	ssdz_load_atlas_data->json_buf = json_buf;
	ssdz_load_atlas_data->json_buf_size = json_buf_size;

	s_load_atlas_queue_mutex.lock();
	s_load_atlas_queue->push(ssdz_load_atlas_data);
	s_load_atlas_queue_mutex.unlock();
}

//根据json数据，创建spSkeletonJson
static void LoadSsdzJsonData(SsdzAsyncLoadJsonData *ssdz_json_load_data)
{
	spSkeletonJson* json = spSkeletonJson_create(ssdz_json_load_data->sp_atlas);
	json->scale = 1 / cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
	spSkeletonData* sp_skeleton_data = spSkeletonJson_readSkeletonData(json, (const char*)ssdz_json_load_data->json_buf);
	delete[] ssdz_json_load_data->json_buf;
	spSkeletonJson_dispose(json);
	
	if(sp_skeleton_data == NULL)
	{
		spAtlas_dispose(ssdz_json_load_data->sp_atlas);
		delete ssdz_json_load_data;
		AddFailedFile(ssdz_json_load_data->ssdz_async_load_image_data->ssdz_file.c_str());

		LogUtil::LogError("[SkeletonDataCahce_ssdz::LoadSsdzJsonData] Error reading skeleton json buf :%s", ssdz_json_load_data->ssdz_async_load_image_data->ssdz_file.c_str());
		return;
	}

	SsdzSyncAddSkeletonData *ssdz_add_skeleton_data = new SsdzSyncAddSkeletonData();
	ssdz_add_skeleton_data->sp_atlas = ssdz_json_load_data->sp_atlas;
	ssdz_add_skeleton_data->sp_skeleton = sp_skeleton_data;
	ssdz_add_skeleton_data->ssdz_async_load_image_data = ssdz_json_load_data->ssdz_async_load_image_data;
	delete ssdz_json_load_data;

	s_add_skeleton_queue_mutex.lock();
	s_add_skeleton_queue->push(ssdz_add_skeleton_data);
	s_add_skeleton_queue_mutex.unlock();
}

//LoadSsdzFileData所在线程函数
static void LoadSsdzImage(void*)
{
	SsdzAsyncLoadImageData *ssdz_image_load_data = NULL;

	while(true)
	{
		s_load_image_list_mutex.lock();
		if (s_load_image_list->empty())
		{
			s_load_image_list_mutex.unlock();
			if (s_load_image_thread_quit) {
				break;
			}
			else {
				std::unique_lock<std::mutex>lk(s_image_thread_sleep_mutex);
				s_image_thread_sleep_condition.wait(lk);
				continue;
			}
		}
		else
		{
			ssdz_image_load_data = s_load_image_list->front();
			s_load_image_list->pop_front();
			s_load_image_list_mutex.unlock();
			LoadSsdzFileData(ssdz_image_load_data);
		}        
	}
}

//LoadSsdzJsonData所在线程函数
static void LoadSsdzJson(void*)
{
	SsdzAsyncLoadJsonData *ssdz_json_load_data = NULL;

	while(true)
	{
		s_load_json_queue_mutex.lock();
		if (s_load_json_queue->empty())
		{
			s_load_json_queue_mutex.unlock();
			if (s_load_json_thread_quit) {
				break;
			}
			else {
				std::unique_lock<std::mutex>lk(s_json_thread_sleep_mutex);
				s_json_thread_sleep_condition.wait(lk);
				continue;
			}
		}
		else
		{
			ssdz_json_load_data = s_load_json_queue->front();
			s_load_json_queue->pop();
			s_load_json_queue_mutex.unlock();
			LoadSsdzJsonData(ssdz_json_load_data);
		}        
	}
}

//根据图像数据，创建spAtlas
void SkeletonDataCache::AddSsdzAtlasAsyncCallBack(float dt)
{
	s_load_atlas_queue_mutex.lock();
	if (s_load_atlas_queue->empty())
	{
		s_load_atlas_queue_mutex.unlock();
		return;
	}

	SsdzSyncLoadAtlasData *ssdz_load_atlas_data = s_load_atlas_queue->front();
	s_load_atlas_queue->pop();
	s_load_atlas_queue_mutex.unlock();

	cocos2d::CCTextureCache::sharedTextureCache()->addUIImage(ssdz_load_atlas_data->exprot_image, ssdz_load_atlas_data->ssdz_image_path.c_str());
	ssdz_load_atlas_data->exprot_image->release();

	spAtlas *atlas = spAtlas_create((const char*)ssdz_load_atlas_data->atlas_buf,
										ssdz_load_atlas_data->atlas_buf_size, 
										ssdz_load_atlas_data->ssdz_path.c_str(), 0);
	delete[] ssdz_load_atlas_data->atlas_buf;
	if(atlas == NULL)
	{
		AddFailedFile(ssdz_load_atlas_data->ssdz_async_load_image_data->ssdz_file.c_str());

		LogUtil::LogError("[SkeletonDataCahce_ssdz::AddSsdzAtlasAsyncCallBack] Error reading atlas buf %s", ssdz_load_atlas_data->ssdz_path.c_str());
		return;
	}

	SsdzAsyncLoadJsonData *ssdz_load_json_data = new SsdzAsyncLoadJsonData();
	ssdz_load_json_data->ssdz_async_load_image_data = ssdz_load_atlas_data->ssdz_async_load_image_data;
	ssdz_load_json_data->sp_atlas = atlas;
	ssdz_load_json_data->json_buf = ssdz_load_atlas_data->json_buf;
	ssdz_load_json_data->json_buf_size = ssdz_load_atlas_data->json_buf_size;
	delete ssdz_load_atlas_data;

	s_load_json_queue_mutex.lock();
	s_load_json_queue->push(ssdz_load_json_data);
	s_load_json_queue_mutex.unlock();
	s_json_thread_sleep_condition.notify_one();
}

void SkeletonDataCache::AddSsdzJsonAsyncCallBack(float dt)
{
	s_add_skeleton_queue_mutex.lock();
	if (s_add_skeleton_queue->empty())
	{
		s_add_skeleton_queue_mutex.unlock();
		return;
	}

	SsdzSyncAddSkeletonData *ssdz_skeleton_data = s_add_skeleton_queue->front();
	s_add_skeleton_queue->pop();
	s_add_skeleton_queue_mutex.unlock();

	if (m_skeleton_data_map.find(ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file) != m_skeleton_data_map.end())
	{
		spSkeletonData_dispose(ssdz_skeleton_data->sp_skeleton);
		spAtlas_dispose(ssdz_skeleton_data->sp_atlas);
		LogUtil::LogError("[SkeletonDataCahce::AddSsdzJsonAsyncCallBack] Error repeat load ssdz cache %s", ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file.c_str());
	}
	else
	{
		SkeletonData *skeleton_data = new SkeletonData(ssdz_skeleton_data->sp_atlas, ssdz_skeleton_data->sp_skeleton);
		skeleton_data->setFilePath(ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file);
		m_skeleton_data_map[ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file] = skeleton_data;
	}
	
	std::vector<LoadSsdzObjectFunc>::iterator iter;
	std::vector<LoadSsdzObjectFunc> *callback_array = &ssdz_skeleton_data->ssdz_async_load_image_data->callback_array;
	for (iter = callback_array->begin(); iter != callback_array->end(); ++iter)
	{
		RunLuaCallback(ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file, true, iter->lua_func_id);
	}

	s_load_image_map->erase(ssdz_skeleton_data->ssdz_async_load_image_data->ssdz_file);
	delete ssdz_skeleton_data->ssdz_async_load_image_data;
	delete ssdz_skeleton_data;

	--s_async_loading_ssdz_count;
	if (0 == s_async_loading_ssdz_count)
	{
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(SkeletonDataCache::CheckSsdzDataAsyncCallBack), this);
	}
}

void SkeletonDataCache::HandleAsyncLoadFailed(float dt)
{
	s_failed_queue_mutex.lock();
	if (s_failed_queue_list->empty())
	{
		s_failed_queue_mutex.unlock();
		return;
	}

	std::string failed_file_name = s_failed_queue_list->front();
	s_failed_queue_list->pop();
	s_failed_queue_mutex.unlock();

	if (NULL != s_load_image_map)
	{
		std::map<std::string, SsdzAsyncLoadImageData*>::iterator ssdz_iter = s_load_image_map->find(failed_file_name);
		if (ssdz_iter != s_load_image_map->end())
		{
			std::vector<LoadSsdzObjectFunc>::iterator iter;
			std::vector<LoadSsdzObjectFunc>& callback_array = ssdz_iter->second->callback_array;
			for (iter = callback_array.begin(); iter != callback_array.end(); ++iter)
			{
				RunLuaCallback(failed_file_name, false, iter->lua_func_id);
			}
			s_load_image_map->erase(failed_file_name);
		}
	}
}

void SkeletonDataCache::CheckSsdzDataAsyncCallBack(float dt)
{
	AddSsdzAtlasAsyncCallBack(dt);
	AddSsdzJsonAsyncCallBack(dt);
	HandleAsyncLoadFailed(dt);
}

void SkeletonDataCache::LoadSsdzSkeletonDataAsync(const std::string& ssdz_file, GameType::UINT32 lua_fun_id)
{
	std::string ssdz_skeleton_fullpath = FileSystemEx::GetInstance()->GetResourcePath(ssdz_file);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(ssdz_file.c_str());

	//如果缓存中有，不再进行读取
	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(ssdz_skeleton_fullpath);
	if (iter != m_skeleton_data_map.end())
	{
		RunLuaCallback(ssdz_file, true, lua_fun_id);
		return;
	}

	// lazy init
	if (s_load_image_list == NULL)
	{
		s_load_image_map = new std::map<std::string, SsdzAsyncLoadImageData*>;
		s_load_image_list = new std::list<SsdzAsyncLoadImageData*>();
		s_load_atlas_queue = new std::queue<SsdzSyncLoadAtlasData*>();
		s_load_json_queue = new std::queue<SsdzAsyncLoadJsonData*>();
		s_add_skeleton_queue = new std::queue<SsdzSyncAddSkeletonData*>();
		s_failed_queue_list = new std::queue<std::string>();

		s_load_image_thread_quit = false;
		s_load_json_thread_quit = false;
		s_load_image_thread = new std::thread(&LoadSsdzImage, this);
		s_load_json_thread = new std::thread(&LoadSsdzJson, this);
	}

	if (0 == s_async_loading_ssdz_count)
	{
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(SkeletonDataCache::CheckSsdzDataAsyncCallBack), this, 0, false);
	}
	++s_async_loading_ssdz_count;

	SsdzAsyncLoadImageData *data = NULL;

	LoadSsdzObjectFunc load_skeleton_object_func;
	load_skeleton_object_func.lua_func_id = lua_fun_id;

	std::map<std::string, SsdzAsyncLoadImageData*>::iterator ssdz_iter = s_load_image_map->find(ssdz_skeleton_fullpath);
	if (ssdz_iter != s_load_image_map->end())
	{
		data = ssdz_iter->second;
		data->callback_array.push_back(load_skeleton_object_func);
		return;
	}

	data = new SsdzAsyncLoadImageData();
	data->ssdz_file = ssdz_skeleton_fullpath;
	data->callback_array.push_back(load_skeleton_object_func);
	(*s_load_image_map)[ssdz_skeleton_fullpath] = data;

	s_load_image_list_mutex.lock();
	s_load_image_list->push_back(data);
	s_load_image_list_mutex.unlock();
	s_image_thread_sleep_condition.notify_one();
}

//骨骼资源是否存在
bool SkeletonDataCache::IsSSdzDataInCache(const std::string& zip_skeleton_file)
{
	std::string zip_skeleton_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(zip_skeleton_file);

	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(zip_skeleton_file_fullpath);
	if(iter != m_skeleton_data_map.end())
	{
		return true;
	}

	return false;
}

////加载、解压ssdz文件，读取json、atlas文件数据，读取解码图像数据
SkeletonData* SkeletonDataCache::LoadSsdzSkeletonData(const std::string& zip_skeleton_file)
{
	std::string zip_skeleton_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(zip_skeleton_file);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(zip_skeleton_file.c_str());

	//如果缓存中有，不再从硬盘读取
	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(zip_skeleton_file_fullpath);
	if(iter != m_skeleton_data_map.end())
	{
		return iter->second;
	}

	ssize_t file_size;
	unsigned char *file_buf = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(zip_skeleton_file_fullpath.c_str(), "rb", &file_size);

	if(NULL == file_buf)
	{
		LogUtil::LogError("[SkeletonDataCahce::LoadZipSkeletonData] load skeleton zip file %s failed", zip_skeleton_file.c_str());
		return NULL;
	}

	//altas
	unsigned int buf_offset = file_size - 4;
	unsigned int altas_zip_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - 4;
	unsigned int altas_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - altas_zip_buf_size;
	unsigned char *altas_zip_buf = file_buf + buf_offset;
	//json
	buf_offset = buf_offset - 4;
	unsigned int json_zip_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - 4;
	unsigned int json_buf_size = *(unsigned int*)(file_buf + buf_offset);
	buf_offset = buf_offset - json_zip_buf_size;
	unsigned char *json_zip_buf = file_buf + buf_offset;
	//image
	unsigned char* image_buf = file_buf;
	unsigned int image_buf_size = buf_offset;
	unsigned int image_type = *(unsigned int*)(file_buf);

	std::string skeleton_suffix_name = SKELETON_SUFFIX_NAME;
	std::string::size_type suffix_find_pos = zip_skeleton_file_fullpath.find(SKELETON_SUFFIX_NAME);
	std::string skeleton_path = zip_skeleton_file_fullpath.substr(0, suffix_find_pos);
	std::string skeleton_image_key = skeleton_path + "/skeleton.png";

	bool unzip_image_ret = false;
	cocos2d::CCImage* export_image = new cocos2d::CCImage();
	if(image_type == ViewHelper::RGBAPng)
	{
		unzip_image_ret = export_image->initWithImageData(image_buf + 4, image_buf_size - 4);
	}
	else
	{
		unzip_image_ret = ViewHelper::UnzipImageBuf(image_buf, image_buf_size, export_image, true);
	}
	if(!unzip_image_ret)
	{
		LogUtil::LogError("[SkeletonDataCahce::LoadZipSkeletonData] Error reading image buf %s", skeleton_image_key.c_str());
		delete[] file_buf;
		delete export_image;
		return NULL;
	}
	cocos2d::CCTextureCache::sharedTextureCache()->addUIImage(export_image, skeleton_image_key.c_str());
	export_image->release();

	unsigned char* altas_buf = new unsigned char[altas_buf_size];
	ZipUtilsEx::InflateGZipMemory(altas_buf, altas_buf_size, altas_zip_buf, altas_zip_buf_size);
	spAtlas *atlas = spAtlas_create((const char*)altas_buf, altas_buf_size, skeleton_path.c_str(), 0);
	delete[] altas_buf;
	if(atlas == NULL)
	{
		LogUtil::LogError("[SkeletonDataCahce::LoadZipSkeletonData] Error reading atlas buf %s", skeleton_path.c_str());
		delete[] file_buf;
		return NULL;
	}

	unsigned char* json_buf = new unsigned char[json_buf_size];
	ZipUtilsEx::InflateGZipMemory(json_buf, json_buf_size, json_zip_buf, json_zip_buf_size);	
	spSkeletonJson* json = spSkeletonJson_create(atlas);
	json->scale = 1 / cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
	spSkeletonData* sp_skeleton_data = spSkeletonJson_readSkeletonData(json, (const char*)json_buf);
	delete[] json_buf;
	spSkeletonJson_dispose(json);
	if(sp_skeleton_data == NULL)
	{
		LogUtil::LogError("[SkeletonDataCahce::LoadZipSkeletonData] Error reading skeleton json buf :%s", skeleton_path.c_str());
		delete[] file_buf;
		return NULL;		
	}
	delete[] file_buf;

	SkeletonData *skeleton_data = new SkeletonData(atlas, sp_skeleton_data);
	skeleton_data->setFilePath(zip_skeleton_file_fullpath);
	m_skeleton_data_map[zip_skeleton_file_fullpath] = skeleton_data;
	return skeleton_data;
}

static void ClearSsdzDataQueue()
{
	//释放未加载的s_load_image_list
	if(s_load_image_list)
	{
		s_load_image_list_mutex.lock();
		std::list<SsdzAsyncLoadImageData*> swap_list;
		std::swap(swap_list, *s_load_image_list);
		s_load_image_list_mutex.unlock();

		while(!swap_list.empty())
		{
			SsdzAsyncLoadImageData* ssdz_load_image_data = swap_list.front();
			swap_list.pop_front();
			delete ssdz_load_image_data;
		}
	}

	//释放未加载的s_load_atlas_queue队列数据
	if(s_load_atlas_queue)
	{
		s_load_atlas_queue_mutex.lock();
		std::queue<SsdzSyncLoadAtlasData*> swap_queue;
		std::swap(swap_queue, *s_load_atlas_queue);
		s_load_atlas_queue_mutex.unlock();

		while(!swap_queue.empty())
		{
			SsdzSyncLoadAtlasData *load_atlas_data = swap_queue.front();
			swap_queue.pop();

			delete load_atlas_data->ssdz_async_load_image_data;
			delete load_atlas_data->exprot_image;
			delete load_atlas_data->atlas_buf;
			delete load_atlas_data->json_buf;
			delete load_atlas_data;
		}
	}

	//释放未加载的s_add_skeleton_queue队列数据
	if(s_load_json_queue)
	{
		s_load_json_queue_mutex.lock();
		std::queue<SsdzAsyncLoadJsonData*> swap_queue;
		std::swap(swap_queue, *s_load_json_queue);
		s_load_json_queue_mutex.unlock();

		while(!swap_queue.empty())
		{
			SsdzAsyncLoadJsonData* ssdz_load_json_data = swap_queue.front();
			swap_queue.pop();

			spAtlas_dispose(ssdz_load_json_data->sp_atlas);
			delete ssdz_load_json_data->ssdz_async_load_image_data;
			delete[] ssdz_load_json_data->json_buf;
			delete ssdz_load_json_data;
		}
	}

	//释放未加载的s_add_skeleton_queue队列数据
	if(s_add_skeleton_queue)
	{
		s_add_skeleton_queue_mutex.lock();
		std::queue<SsdzSyncAddSkeletonData*> swap_queue;
		std::swap(swap_queue, *s_add_skeleton_queue);
		s_add_skeleton_queue_mutex.unlock();

		while(!swap_queue.empty())
		{
			SsdzSyncAddSkeletonData *sync_skeleton_data = swap_queue.front();
			swap_queue.pop();

			spSkeletonData_dispose(sync_skeleton_data->sp_skeleton);
			spAtlas_dispose(sync_skeleton_data->sp_atlas);
			delete sync_skeleton_data->ssdz_async_load_image_data;
			delete sync_skeleton_data;
		}
	}

	//处理失败列表
	if (s_failed_queue_list)
	{
		s_failed_queue_mutex.lock();
		std::queue<std::string> swap_queue;
		std::swap(swap_queue, *s_failed_queue_list);
		s_failed_queue_mutex.unlock();

		std::string failed_file_name;
		while(!swap_queue.empty())
		{
			failed_file_name = swap_queue.front();
			swap_queue.pop();

			std::map<std::string, SsdzAsyncLoadImageData*>::iterator ssdz_iter = s_load_image_map->find(failed_file_name);
			if (ssdz_iter != s_load_image_map->end())
			{
				std::vector<LoadSsdzObjectFunc>::iterator iter;
				std::vector<LoadSsdzObjectFunc>& callback_array = ssdz_iter->second->callback_array;
				for (iter = callback_array.begin(); iter != callback_array.end(); ++iter)
				{
					RunLuaCallback(failed_file_name, false, iter->lua_func_id);
				}
				s_load_image_map->erase(failed_file_name);
			}
		}
	}

	if(s_load_image_map)
	{
		delete s_load_image_map;
		s_load_image_map = NULL;
	}
}

void SkeletonDataCache::StopSsdzThread()
{
	ClearSsdzDataQueue();

	if (s_load_image_thread)
	{
		s_load_image_thread_quit = true;
		s_image_thread_sleep_condition.notify_one();
		s_load_image_thread->join();
		delete s_load_image_thread;
		s_load_image_thread = nullptr;
	}

	if (s_load_json_thread)
	{
		s_load_json_thread_quit = true;
		s_json_thread_sleep_condition.notify_one();
		s_load_json_thread->join();
		delete s_load_json_thread;
		s_load_json_thread = nullptr;
	}

	if (s_load_image_list != NULL)
	{
		delete s_load_image_list;
		s_load_image_list = NULL;
	}

	if (s_load_atlas_queue != NULL)
	{
		delete s_load_atlas_queue;
		s_load_atlas_queue = NULL;
	}

	if (s_load_json_queue != NULL)
	{
		delete s_load_json_queue;
		s_load_json_queue = NULL;
	}

	if (s_add_skeleton_queue != NULL)
	{
		delete s_add_skeleton_queue;
		s_add_skeleton_queue = NULL;
	}

	if (s_failed_queue_list != NULL)
	{
		delete s_failed_queue_list;
		s_failed_queue_list = NULL;
	}
}
