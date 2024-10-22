#ifndef PARTICLEDATAMANAGER_H
#define PARTICLEDATAMANAGER_H

#include "cocos2d.h"
#include <map>
#include <string>

NS_CC_BEGIN

//class ParticleSystemReader;
class CCParticleEmitter;
class CCParticleAffector;
class MyParticleSystem;
class CCParticleRenderer;
class CCParticleTechnique;
class CCParticleEmitterFactory;
class CCParticleAffectorFactory;
class CCParticleRendererFactory;
class PSReader;

#define USE_MULTIPLE_CONFIG_PATH  "USE_MULTIPLE_CONFIG_PATH"

typedef void(*TEMPL_ASYNC_LOAD_FINISHED_CALLBACK)(const char* template_name_or_full_name);

class CC_DLL PSManager : public Ref
{
public:
	typedef std::map<std::string, CCParticleEmitterFactory*> ParticleEmitterFactoryMap;
	typedef std::map<std::string, CCParticleAffectorFactory*> ParticleAffectorFactoryMap;
	typedef std::map<std::string, CCParticleRendererFactory*> ParticleRendererFactoryMap;
	typedef std::map<std::string, std::vector<unsigned int>> TemplAsyncIDVecAssociateMap;
	typedef std::map<std::string, TEMPL_ASYNC_LOAD_FINISHED_CALLBACK> TemplAsyncFinishedCallbackAssociateMap;
	typedef std::map<unsigned int, MyParticleSystem*> ParticleAsyncIDAssociateMap;
	typedef std::map<std::string, MyParticleSystem*> ParticleTemplateMap;

	static PSManager* GetInstance();
	static void DelInstance();
	virtual ~PSManager();
	void Init(const std::string& config_path,const std::string& texture_path,const std::string& mesh_path, const std::string& texture_suffix);
	void Stop();

private:
	PSManager();
	std::string CheckPath(const std::string& path);

	void AddEmitterFactory(CCParticleEmitterFactory* factory);
	void AddAffectorFactory(CCParticleAffectorFactory* factory);
	void AddRendererFactory(CCParticleRendererFactory *factory);

	void InitEmitterFactory();
	void InitAffectorFactory();
	void InitRendererFactory();

public:
	//const std::string& GetConfigPath() const { return m_config_path; }
	const std::string& GetMutiLanguagePath() const { return m_muti_language_texture_path; }
	const std::string& GetTexturePath() const { return m_texture_path; }
	const std::string& GetMeshPath() const { return m_mesh_path; }

public:
	CCParticleEmitter* CreateEmitter(const std::string& type, CCParticleTechnique* pt);
	CCParticleAffector* CreateAffector(const std::string& type, CCParticleTechnique* pt);
	CCParticleRenderer* CreateRenderer(const std::string& type, CCParticleTechnique* pt);

public:
	// 创建模板粒子，当Init设置配置路径为USE_MULTIPLE_CONFIG_PATH时，模板使用绝对路径
	MyParticleSystem* CreateTemplate(const std::string& template_name_or_full_name);
	// 异步创建模板粒子
	void CreateTemplateAsync(const std::string& template_name_or_full_name, MyParticleSystem* new_ps);
	// 异步创建模板粒子
	void CreateTemplateAsync(const std::string& template_name_or_full_name, TEMPL_ASYNC_LOAD_FINISHED_CALLBACK callback_func = nullptr);
	// 中断异步创建模板粒子
	void AbortTemplateAsync(unsigned int async_id);

	bool IsTemplateExist(const std::string& name);
	MyParticleSystem* GetTemplate(const std::string& name);
	void DestoryTemplate(const std::string& name);
	void DestoryAllTemplateSystem();
	void SaveScript(MyParticleSystem *ps, const char *file_name);

	std::string GetFullName(const std::string& template_name_or_full_name);
	std::string GetTemplatePath(const std::string& template_name);

private:
	// 创建模板粒子，当Init设置配置路径为USE_MULTIPLE_CONFIG_PATH时，模板使用绝对路径
	MyParticleSystem* CreateTemplateByData(const std::string& template_name_or_full_name, const cocos2d::Data& file_data);

	void LoadTemplateAsyncCallback(float dt);
	//异步加载模板粒子(内部处理)
	void LoadTemplateAsync(const char* template_name_or_full_name);

private:
	ParticleEmitterFactoryMap m_emitter_factorys;
	ParticleAffectorFactoryMap m_affector_factorys;
	ParticleRendererFactoryMap m_renderer_factorys;

	PSReader *m_sys_reader;

	bool m_is_init;
	std::string m_config_path;
	bool m_is_use_mutiple_config_path;
	std::string m_texture_suffix;
	std::string m_texture_path;
	std::string m_muti_language_texture_path;
	std::string m_mesh_path;

	unsigned int m_async_load_id;

	//模板记录表
	ParticleTemplateMap m_system_templates;

	//模板名与async_id关联表(key:file_full_name value:async_id)
	TemplAsyncIDVecAssociateMap m_templ_async_ids_associate_map;

	//粒子与async_id关联表(key:async_id value:ps)
	ParticleAsyncIDAssociateMap m_ps_async_id_associate_map;

	//模板名与异步加载完成回调关联表(key:file_full_name value:callback)
	TemplAsyncFinishedCallbackAssociateMap m_templ_async_finished_callback_map;
};

NS_CC_END

#endif