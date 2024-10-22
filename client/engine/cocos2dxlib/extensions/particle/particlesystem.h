#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "technique.h"

#include "cocos2d.h"
#include <vector>
#include <map>
#include <string>

NS_CC_BEGIN

class CCParticle;
class Node;
class Rect;
class Scene;

#define DEFAULT_EMPTY_PARTICLE_SYSTEM_NAME "DEFAULT_EMPTY_PARTICLE_SYSTEM_NAME"

class CC_DLL MyParticleSystem : public Node
{
public:
	static const float DEFAULT_CYCLETIME;

	typedef void (*CycleCallBack)();
	typedef std::vector<CCParticleTechnique*> ParticleTechniqueList;
	
	// 升级到3.2 add by zhuyi 2014-09-01
	virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
	// end
	
public:
	MyParticleSystem(const std::string& name);
	virtual ~MyParticleSystem(void);

	MyParticleSystem& operator=(const MyParticleSystem& ps);

	static MyParticleSystem* Create();
	static MyParticleSystem* Create(const char* template_name);
	static MyParticleSystem* CreateAsync(const char* template_name);
	static MyParticleSystem* CopyData(MyParticleSystem* ps);

	bool InitWithTemplateName(const char* template_name);

	const std::string& GetTemplateName() const { return m_ps_name; }

	void SetIsTemplate(bool is_template) { m_is_template = is_template; }
	bool IsTemplate() { return m_is_template; }

	//粒子模板异步加载完成回调
	void OnTemplateAsyncLoadFinished(const char* template_name);

	void SetPosition(float x, float y, float z);
	void SetScale(float scale_x, float scale_y, float scale_z);
	float GetScaleX() { return _scaleX; }
	float GetScaleY() { return _scaleY; }
	void SetFlipX(bool is_flip_x);
	void SetFlipY(bool is_flip_y);
	float GetScaleZ() const { return m_fScaleZ; }

	void SetCycleTotalTime(float time){ m_cycle_total_time = time; };
	float GetCycleTotalTime() const { return m_cycle_total_time; }
	void SetCycle(bool cycle);
	void SetCycleCallBack(Ref* pSelectorTarget, SEL_CallFunc selector);

	void SetPreTime(float time);
	float GetPreTime(){ return m_pre_time; }
	void SetFramePreSecond(int frame_num);
	int GetFramePreSecond();

	void SetScaleSpeed(float speed);
	float GetScaleSpeed() const { return m_scale_speed; }
	void SetScaleTime(float time);
	float GetScaleTime() const{ return m_scale_time; }

	bool SetAttribute(const char* name, const std::string& value);
	bool GetAttribute(const char* name, std::string& value);

	void SetEnabled(bool enabled);
	float GetSinceStartTime() { return m_live_time; }
	std::string GetWritePath(){return FileUtils::getInstance()->getWritablePath();}
	void SetScreenShotsState(bool screen_short,int num, std::string path);

	void SetClipperExcludeList(std::string exclude_list);
	std::string GetClipperExcludeList();
	void AddClipper(CCParticleTechnique* pt);
	void SetInverted(bool inverted);
	bool GetInverted();
	std::map<std::string, ClippingNode*>& GetClippers();
	Rect& GetClipperRect();
	Vec2& GetClipperPos();
	void SetClippersRect();
	void SetClipperPosition(const Vec2& pos);
	void MakeGrey(bool is_grey); 

	size_t GetNumTechniques() const;
	CCParticleTechnique* CreateTechnique(CCParticleTechnique* template_tech=0);
	bool DestoryTechnique (size_t index);
	int GetIndexTechnique(const std::string& name);
	CCParticleTechnique* GetTechnique (size_t index) const;
	bool MoveTechnique(unsigned int index0, unsigned int index1);

	ClippingNode* CreateClippingNode(const char* type);
	bool DestoryClippingNode();
	void DestroyAllTechniques();

	void SetShowAABB(bool is_show) { m_show_aabb = is_show; }
	bool IsAABBShow(){return m_show_aabb;}
	void SetCustomBoundBox(const Rect& rect) { m_custom_rect = rect; }
	const Rect& GetCustomBox() const { return m_custom_rect; }

	void SetManualUpdate(bool var);
	bool IsManualUpdate(){return m_manual_update;}

	void ResetData();

	void ChangeTexExtension(const std::string& ext);

	size_t GetTotalParticleNum();
	size_t GetTotalActiveParticleNum();

	virtual void setVisible(bool visible);
	void DoScreenShort();

	virtual void update(float fDelta);
	void onUpdate(float fDelta);

	void SetAsyncLoadID(unsigned int var) { m_async_load_id = var; }
	unsigned int GetAsyncLoadID() { return m_async_load_id; }

private:
	bool IsClipperContain(std::string technique_name);

private:
	std::string m_ps_name;
	bool m_is_template;

	ParticleTechniqueList m_techniques;

	bool m_is_cycle;
	float m_cycle_total_time;
	
	cocos2d::CCObject* m_listener;
	cocos2d::SEL_CallFunc m_selector;

	float m_fScaleZ;
	float m_scale_time;
	float m_scale_speed;

	bool m_is_flip_x;
	bool m_is_flip_y;

	bool m_enabled;
	float m_live_time;
	float m_pre_time;
	bool m_is_need_pre_time;

	float m_skip_time;
	//每秒更新帧数
	int m_frame_pre_second;						
	//更新频率时间
	float m_update_delta_time;					
	float m_curr_delta_time;

	bool m_show_aabb;
	Rect m_custom_rect;

	bool m_manual_update;

	bool m_screen_short;
	int m_screen_short_num;
	int m_screen_short_cur_num;
	std::string m_screen_path;

	std::map<std::string, ClippingNode*> m_clippers;
	Rect m_clipper_rect;
	Vec2 m_clipper_pos;

	//裁剪排除technique列表
	std::string m_clipper_technique_list;		

	bool m_inverted;

	//true表示当前粒子系统变灰状态，false表示当前正常状态
	bool m_is_grey;

	//异步加载ID
	unsigned int m_async_load_id;

	//缓存异步加载中发生改变的属性
	struct ParticleSysAsyncCacheInfo
	{
		bool is_cycle;
		float scale_time;
		float scale_x;
		float scale_y;
		float scale_z;
		bool flip_x;
		bool flip_y;
		bool manual_update;
		int frame_num;
		bool is_grey;
	};
	typedef std::set<std::string> ParticleSysAsyncCacheSet;
	typedef ParticleSysAsyncCacheSet::const_iterator ParticleSysAsyncCacheSetCIter;

	ParticleSysAsyncCacheInfo m_async_cache_info;
	ParticleSysAsyncCacheSet m_async_cache_set;
};

NS_CC_END

#endif
