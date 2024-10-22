#ifndef __SKELETON_DATACACHE_H__
#define __SKELETON_DATACACHE_H__

#include <string>
#include <map>
#include "spine/spine-cocos2dx.h"
#include "zqcommon/typedef.h"

class SkeletonData : public cocos2d::Ref
{
public:
	SkeletonData(spAtlas *altas, spSkeletonData *sp_skeleton_data)
		: m_altas(altas)
		, m_sp_skeleton_data(sp_skeleton_data)
		, m_isAutoClearCache(true)
	{}
	~SkeletonData();

public:
	spSkeletonData* GetSkeletonData();

	// add by swordman sue 2016-6-20
	// �洢�ļ�·��
	void setFilePath(const std::string& path) { m_filePath = path; };

	// �Զ������������
	void setAutoClearCache(bool var) { m_isAutoClearCache = var; }

	// ��дrelease
	virtual void release();

private:
	std::string m_filePath;

	// �Զ������������
	bool m_isAutoClearCache;
	// end by swordman sue 

private:
	spAtlas*			m_altas;
	spSkeletonData*		m_sp_skeleton_data;
};

typedef void (cocos2d::Ref::*LoadSkeleton_CallBack)(std::string skeleton_file, GameType::UINT32 lua_func_id);

class SkeletonDataCache : public cocos2d::Ref
{
public:
	static SkeletonDataCache* GetInstance();
	static void DelInstance();
	~SkeletonDataCache();

	//���ӹ������ݻ���
	void LoadSkeletonDataAsync(const std::string& skeleton_data_file, const std::string& atlas_file, const std::string& image_file, GameType::UINT32 lua_fun_id);
	void LoadSsdzSkeletonDataAsync(const std::string& ssdz_file, GameType::UINT32 lua_fun_id);

	//ͬ����ȡ��������
	SkeletonData* LoadSkeletonData(const std::string& skeleton_data_file, const std::string& atlas_file);
	SkeletonData* LoadSsdzSkeletonData(const std::string& zip_skeleton_data_file);
	
	//������Դ�Ƿ����
	bool IsSkeletonDataInCache(const std::string& skeleton_data_file);
	bool IsSSdzDataInCache(const std::string& ssdz_file);
	
	//�Ƴ�ָ���Ĺ������� add by swordman sue 2016-6-20
	void RemoveSkeletonDataCacheForKey(const std::string& skeleton_file);
	//end by swordman sue

	//�Ƴ�δ�����õĹ�������
	void RemoveUnusedSkeletonDataCache(bool print_log = false);

	//�Ƴ�ȫ���Ĺ�������
	void RemoveAllSkeletonDataCache();

	bool Stop();

public:
	//�첽��ȡ��������
	void AddSkeletonDataAsync(const std::string& skeleton_data_file, const std::string& atlas_file, GameType::UINT32 lua_fun_id);

private:
	SkeletonDataCache(){};
	void AddSkeletonDataAsyncCallBack(float dt);
	void CheckSsdzDataAsyncCallBack(float dt);
	void AddSsdzAtlasAsyncCallBack(float dt);
	void AddSsdzJsonAsyncCallBack(float dt);
	void HandleAsyncLoadFailed(float dt);
	void StopSkeletonThread();
	void StopSsdzThread();

private:	
	std::map<std::string, SkeletonData*>		m_skeleton_data_map;
};

//����lua����
extern void RunLuaCallback(std::string skeleton_file, bool success, GameType::UINT32 lua_func_id);

#endif