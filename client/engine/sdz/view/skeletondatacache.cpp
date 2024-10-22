#include <queue>
#include <list>
#include <vector>

#include "SkeletonDataCache.h"
#include "zqcommon/logutil.h"
#include "spine/extension.h"
#include "filesystem/filesystemex.h"
#include "luaengine/luaengine.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCThread.h"
#include "view/viewhelper.h"
#include "resmanager/resmanager.h"

#include <thread>

typedef struct _LoadSkeletonImageData
{
	std::string skeleton_file;
	std::string atlas_file;
	GameType::UINT32 lua_func_id;
}LoadSkeletonImageData;

using namespace cocos2d;

SkeletonData::~SkeletonData()
{
	if(m_sp_skeleton_data)
	{
		spSkeletonData_dispose(m_sp_skeleton_data);
	}

	if(m_altas)
	{
		spAtlas_dispose(m_altas);
	}
}

void SkeletonData::release()
{
	unsigned int referenceCount = _referenceCount;
	bool isAutoClearCache = m_isAutoClearCache;

	Ref::release();

	if (isAutoClearCache && --referenceCount == 1)
	{
		SkeletonDataCache::GetInstance()->RemoveSkeletonDataCacheForKey(m_filePath);
	}
}

spSkeletonData* SkeletonData::GetSkeletonData()
{
	return m_sp_skeleton_data;
}

/////////////////////////////////SkeletonDataCahce//////////////////////////////
typedef struct _LoadSkeletonObjectFunc
{
	Ref *object;
	LoadSkeleton_CallBack call_back_func;
	GameType::UINT32 lua_func_id;
}LoadSkeletonObjectFunc;

typedef struct _SkeletonAsyncLoadStruct{
	std::string skeleton_file;
	spAtlas		*sp_atlas;
	std::vector<LoadSkeletonObjectFunc>	callback_array;
}SkeletonAsyncLoadStruct;

typedef struct _SkeletonLoadData
{
	SkeletonAsyncLoadStruct *skeleton_async_load_struct;
	spSkeletonData			*sp_skeleton_data;
}SkeletonLoadData;

static std::thread*			s_skeleton_loading_thread;
static std::mutex			s_skeleton_sleep_mutex;
static std::condition_variable	s_skeleton_sleep_condition;

static std::mutex      s_skeleton_async_list_mutex;
static std::mutex      s_skeletion_data_queue_mutex;

static unsigned long s_skeleton_async_loading_count = 0;
static bool s_skeleton_loading_thread_quit = false;

static std::list<SkeletonAsyncLoadStruct*>*	s_skeleton_async_list = NULL;
static std::queue<SkeletonLoadData*>*  s_skeleton_data_queue = NULL;

//调用lua函数
void RunLuaCallback(std::string skeleton_file, bool success, GameType::UINT32 lua_func_id)
{
	LuaEngine::GetInstance()->CallFunction(lua_func_id, "sb", skeleton_file.c_str(), success);
	LuaEngine::GetInstance()->DelLuaFuncID(lua_func_id);
}

void GetSkeletonDataAsync(void *data)
{
	if(data == NULL)
		return;

	LoadSkeletonImageData *skeleton_image_data = (LoadSkeletonImageData*)data;
	SkeletonDataCache::GetInstance()->AddSkeletonDataAsync(skeleton_image_data->skeleton_file, 
															skeleton_image_data->atlas_file,
															skeleton_image_data->lua_func_id);

	delete skeleton_image_data;
}

static void LoadSkeletonData(_SkeletonAsyncLoadStruct *skeleton_async_load_struct)
{
	spSkeletonJson* json = spSkeletonJson_create(skeleton_async_load_struct->sp_atlas);
	json->scale = (1 / cocos2d::CCDirector::sharedDirector()->getContentScaleFactor());
	spSkeletonData* sp_skeleton_data = spSkeletonJson_readSkeletonDataFile(json, skeleton_async_load_struct->skeleton_file.c_str());
	spSkeletonJson_dispose(json);
	
	if(!sp_skeleton_data)
	{
		LogUtil::LogError("[loadSkeletonData] error path %s", skeleton_async_load_struct->skeleton_file.c_str());
		spAtlas_dispose(skeleton_async_load_struct->sp_atlas);
		delete skeleton_async_load_struct;
		return;
	}

	SkeletonLoadData *skeleton_load_data = new SkeletonLoadData();
	skeleton_load_data->skeleton_async_load_struct = skeleton_async_load_struct;
	skeleton_load_data->sp_skeleton_data = sp_skeleton_data;

	s_skeletion_data_queue_mutex.lock();
	s_skeleton_data_queue->push(skeleton_load_data);
	s_skeletion_data_queue_mutex.unlock();
}

static void ClearSkeletonDataQueue()
{
	if(s_skeleton_async_list)
	{
		s_skeleton_async_list_mutex.lock();
		std::list<SkeletonAsyncLoadStruct*> swap_list;
		std::swap(swap_list, *s_skeleton_async_list);
		s_skeleton_async_list_mutex.unlock();

		while(!swap_list.empty())
		{
			SkeletonAsyncLoadStruct *skeleton_async_load_struct = swap_list.front();
			swap_list.pop_front();
			
			spAtlas_dispose(skeleton_async_load_struct->sp_atlas);
			delete skeleton_async_load_struct;
		}
	}
	
	if(s_skeleton_data_queue)
	{
		s_skeletion_data_queue_mutex.lock();
		std::queue<SkeletonLoadData*> swap_queue;
		std::swap(swap_queue, *s_skeleton_data_queue);
		s_skeletion_data_queue_mutex.unlock();

		while(!swap_queue.empty())
		{
			SkeletonLoadData *skeleton_loaddata = swap_queue.front();
			swap_queue.pop();

			spSkeletonData_dispose(skeleton_loaddata->sp_skeleton_data);
			spAtlas_dispose(skeleton_loaddata->skeleton_async_load_struct->sp_atlas);

			delete skeleton_loaddata->skeleton_async_load_struct;
			delete skeleton_loaddata;
		}
	}
}

static void* loadSkeleton(void* data)
{
	SkeletonAsyncLoadStruct *pAsyncStruct = NULL;

	while (true)
	{
		s_skeleton_async_list_mutex.lock();
		if (s_skeleton_async_list->empty())
		{
			s_skeleton_async_list_mutex.unlock();
			if (s_skeleton_loading_thread_quit) {
				break;
			}
			else {
				
				std::unique_lock<std::mutex>lk(s_skeleton_sleep_mutex);
				s_skeleton_sleep_condition.wait(lk);
				continue;
			}
		}
		else
		{
			pAsyncStruct = s_skeleton_async_list->front();
			s_skeleton_async_list->pop_front();
			s_skeleton_async_list_mutex.unlock();
			LoadSkeletonData(pAsyncStruct);
		}        
	}

	return 0;
}

static SkeletonDataCache* s_instance = nullptr;

SkeletonDataCache* SkeletonDataCache::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new SkeletonDataCache;
	}
	return s_instance;
}

void SkeletonDataCache::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

void SkeletonDataCache::AddSkeletonDataAsyncCallBack(float dt)
{
	s_skeletion_data_queue_mutex.lock();
	if (s_skeleton_data_queue->empty())
	{
		s_skeletion_data_queue_mutex.unlock();
		return;
	}

	SkeletonLoadData *skeleton_loaddata = s_skeleton_data_queue->front();
	s_skeleton_data_queue->pop();
	s_skeletion_data_queue_mutex.unlock();

	SkeletonData *skeleton_data = new SkeletonData(skeleton_loaddata->skeleton_async_load_struct->sp_atlas, skeleton_loaddata->sp_skeleton_data);
	skeleton_data->setFilePath(skeleton_loaddata->skeleton_async_load_struct->skeleton_file);
	m_skeleton_data_map[skeleton_loaddata->skeleton_async_load_struct->skeleton_file] = skeleton_data;

	std::vector<LoadSkeletonObjectFunc>::iterator iter;
	std::vector<LoadSkeletonObjectFunc> *callback_array = &skeleton_loaddata->skeleton_async_load_struct->callback_array;
	for(iter = callback_array->begin(); iter != callback_array->end(); ++iter)
	{
		RunLuaCallback(skeleton_loaddata->skeleton_async_load_struct->skeleton_file, true, iter->lua_func_id);
	}

	delete skeleton_loaddata->skeleton_async_load_struct;
	delete skeleton_loaddata;

	--s_skeleton_async_loading_count;
	if (0 == s_skeleton_async_loading_count)
	{
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(SkeletonDataCache::AddSkeletonDataAsyncCallBack), this);
	}
}

bool SkeletonDataCache::IsSkeletonDataInCache(const std::string& skeleton_file)
{
	std::string skeleton_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(skeleton_file);

	//如果缓存中有，不再从硬盘读取
	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(skeleton_file_fullpath);
	if(iter != m_skeleton_data_map.end())
	{
		return true;
	}

	return false;
}

//同步读取骨骼数据
SkeletonData* SkeletonDataCache::LoadSkeletonData(const std::string& skeleton_file, const std::string& atlas_file)
{
	std::string skeleton_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(skeleton_file);
	std::string atlas_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(atlas_file);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(skeleton_file.c_str());
	ResManager::GetInstance()->UnzipInitResDynamically(atlas_file.c_str());

	//如果缓存中有，不再从硬盘读取
	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(skeleton_file_fullpath);
	if(iter != m_skeleton_data_map.end())
	{
		return iter->second;
	}
	
	spAtlas *atlas = spAtlas_createFromFile(atlas_file_fullpath.c_str(), 0);
	if(atlas == NULL)
	{
		LogUtil::LogError("[SkeletonDataCahce::LoadSkeletonData] Error reading atlas file %s", atlas_file_fullpath.c_str());
		return NULL;
	}

	spSkeletonJson* json = spSkeletonJson_create(atlas);
	json->scale = 1 / cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
	spSkeletonData* sp_skeleton_data = spSkeletonJson_readSkeletonDataFile(json, skeleton_file_fullpath.c_str());
	spSkeletonJson_dispose(json);

	if(sp_skeleton_data == NULL)
	{
		LogUtil::LogError("[SkeletonData::ScaleSkeletonData] Error reading skeleton data");
		return NULL;		
	}

	SkeletonData *skeleton_data = new SkeletonData(atlas, sp_skeleton_data);
	skeleton_data->setFilePath(skeleton_file_fullpath);
	m_skeleton_data_map[skeleton_file_fullpath] = skeleton_data;
	return skeleton_data;
}

//异步读取骨骼数据
void SkeletonDataCache::AddSkeletonDataAsync(const std::string& skeleton_file, const std::string& atlas_file, GameType::UINT32 lua_fun_id)
{
	std::string skeleton_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(skeleton_file);
	std::string atlas_file_fullpath = FileSystemEx::GetInstance()->GetResourcePath(atlas_file);

	//动态解压随包资源
	ResManager::GetInstance()->UnzipInitResDynamically(skeleton_file.c_str());
	ResManager::GetInstance()->UnzipInitResDynamically(atlas_file.c_str());

	//如果缓存中有，不再进行读取
	std::map<std::string, SkeletonData*>::iterator iter = m_skeleton_data_map.find(skeleton_file_fullpath);
	if(iter != m_skeleton_data_map.end())
	{
		RunLuaCallback(skeleton_file_fullpath, true, lua_fun_id);
		return;
	}

	// lazy init
	if (s_skeleton_async_list == NULL)
	{   
		s_skeleton_async_list = new std::list<SkeletonAsyncLoadStruct*>();
		s_skeleton_data_queue = new std::queue<SkeletonLoadData*>();

		s_skeleton_loading_thread_quit = false;
		s_skeleton_loading_thread = new std::thread(&loadSkeleton, this);
	}
	if (0 == s_skeleton_async_loading_count)
	{
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(SkeletonDataCache::AddSkeletonDataAsyncCallBack), this, 0, false);
	}
	++s_skeleton_async_loading_count;

	SkeletonAsyncLoadStruct *data = NULL;
	bool is_skeleton_struct_find = false;

	LoadSkeletonObjectFunc load_skeleton_object_func;
	load_skeleton_object_func.lua_func_id = lua_fun_id;

	s_skeleton_async_list_mutex.lock();
	std::list<SkeletonAsyncLoadStruct*>::iterator skeleton_iter;
	for(skeleton_iter = s_skeleton_async_list->begin(); skeleton_iter != s_skeleton_async_list->end(); ++skeleton_iter)
	{
		if((*skeleton_iter)->skeleton_file == skeleton_file_fullpath)
		{
			data = *skeleton_iter;
			data->callback_array.push_back(load_skeleton_object_func);
			is_skeleton_struct_find = true;
			break;
		}
	}
	s_skeleton_async_list_mutex.unlock();

	if (!data)
	{
		data = new SkeletonAsyncLoadStruct();
		data->skeleton_file = skeleton_file_fullpath;
		data->sp_atlas = spAtlas_createFromFile(atlas_file_fullpath.c_str(), 0);
		data->callback_array.push_back(load_skeleton_object_func);
	}

	if (!is_skeleton_struct_find)
	{
		s_skeleton_async_list_mutex.lock();
		s_skeleton_async_list->push_back(data);
		s_skeleton_async_list_mutex.unlock();
	}
	s_skeleton_sleep_condition.notify_one();
}

SkeletonDataCache::~SkeletonDataCache()
{
	RemoveAllSkeletonDataCache(); 
}

void SkeletonDataCache::LoadSkeletonDataAsync(const std::string& skeleton_data_file, const std::string& atlas_file, const std::string& image_file, GameType::UINT32 lua_fun_id)
{
	LoadSkeletonImageData *skeleton_image_data = new LoadSkeletonImageData();
	skeleton_image_data->skeleton_file = skeleton_data_file;
	skeleton_image_data->atlas_file = atlas_file;
	skeleton_image_data->lua_func_id = lua_fun_id;
	ViewHelper::LoadAsyncImageResource(image_file, GetSkeletonDataAsync, (void*)skeleton_image_data);
}

//移除指定的骨骼缓存
void SkeletonDataCache::RemoveSkeletonDataCacheForKey(const std::string& skeleton_file)
{
	std::map<std::string, SkeletonData*>::iterator map_iter = m_skeleton_data_map.find(skeleton_file);
	if (map_iter != m_skeleton_data_map.end())
	{
		map_iter->second->release();
		m_skeleton_data_map.erase(map_iter->first);
	}
}

//移除未被引用的骨骼缓存
void SkeletonDataCache::RemoveUnusedSkeletonDataCache(bool print_log)
{
	std::vector<std::string> erase_array;
	std::map<std::string, SkeletonData*>::iterator map_iter;
	for(map_iter = m_skeleton_data_map.begin(); map_iter != m_skeleton_data_map.end(); ++map_iter)
	{
		if(map_iter->second->getReferenceCount() == 1)
		{
			map_iter->second->release();
			erase_array.push_back(map_iter->first);
		}
	}
	
	std::vector<std::string>::iterator vector_iter;
	for(vector_iter = erase_array.begin(); vector_iter != erase_array.end(); ++vector_iter)
	{
		m_skeleton_data_map.erase(*vector_iter);
		if (print_log)
		{
			CCLOG("[SkeletonDataCahce] removing unused skeletondata %s", (*vector_iter).c_str());
		}
	}
}

//移除全部的骨骼缓存
void SkeletonDataCache::RemoveAllSkeletonDataCache()
{
	std::map<std::string, SkeletonData*>::iterator iter;
	for (iter = m_skeleton_data_map.begin(); iter != m_skeleton_data_map.end(); ++iter)
	{
		delete iter->second;
	}
	m_skeleton_data_map.clear();
}

void SkeletonDataCache::StopSkeletonThread()
{
	ClearSkeletonDataQueue();

	if (s_skeleton_loading_thread)
	{
		s_skeleton_loading_thread_quit = true;
		s_skeleton_sleep_condition.notify_one();
		s_skeleton_loading_thread->join();
		delete s_skeleton_loading_thread;
		s_skeleton_loading_thread = nullptr;
	}

	if (s_skeleton_async_list != NULL)
	{
		delete s_skeleton_async_list;
		s_skeleton_async_list = NULL;
		delete s_skeleton_data_queue;
		s_skeleton_data_queue = NULL;
	}
}

bool SkeletonDataCache::Stop()
{
	StopSkeletonThread();
	StopSsdzThread();

	return true;
}