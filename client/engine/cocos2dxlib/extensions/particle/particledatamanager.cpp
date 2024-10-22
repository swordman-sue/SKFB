
#include "particledatamanager.h"
#include "particlesystem.h"
#include "simplespline.h"
#include "dynamicattribute.h"
#include "attribute.h"
#include "particle.h"
#include "technique.h"
#include "particlefactory.h"
#include "emitter/emitter.h"
#include "render/renderer.h"
#include "affector/affector.h"

#include "xmlreader.h"

#include "affector/colour.h"
#include "affector/deflector.h"
#include "affector/elasticity.h"
#include "affector/linearforce.h"
#include "affector/pathfollower.h"
#include "affector/randomiser.h"
#include "affector/rotation.h"
#include "affector/scale.h"
#include "affector/suction.h"
#include "affector/texanim.h"
#include "affector/vortex.h"

#include "emitter/point.h"
#include "emitter/box.h"
#include "emitter/line.h"
#include "emitter/circle.h"
#include "emitter/position.h"
#include "emitter/sphere.h"

#include "render/billboard.h"
#include "render/ribbon.h"
#include "render/entity.h"
#include "render/emittedps.h"

#include "strutil.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"

#include "math/CCMath.h"
#include <assert.h>
#include <list>
#include <set>
#include <thread>

NS_CC_BEGIN

//模板异步加载线程
static std::thread*				s_load_templ_thread;
static std::mutex				s_load_templ_thread_sleep_mutex;
static std::condition_variable	s_load_templ_thread_sleep_condition;
static unsigned int s_load_templ_count = 0;
static bool s_load_templ_thread_quit = false;

//模板异步加载队列
typedef std::queue<std::string> TemplateAsyncLoadQueue;
static std::mutex s_load_templ_queue_mutex;
static TemplateAsyncLoadQueue s_load_templ_queue;

//模板异步加载完成队列
typedef struct _TemplateAsyncLoadFinishedInfo
{
	std::string template_name_or_full_name;
	cocos2d::Data file_data;
} TemplateAsyncLoadFinishedInfo;
typedef std::queue<TemplateAsyncLoadFinishedInfo> TemplateAsyncLoadFinishedQueue;
static std::mutex s_load_templ_finished_queue_mutex;
static TemplateAsyncLoadFinishedQueue s_load_templ_finished_queue;

//模板异步加载线程
static void* LoadTemplateThread(void* data)
{
	while (true)
	{
		s_load_templ_queue_mutex.lock();
		if (s_load_templ_queue.empty())
		{
			s_load_templ_queue_mutex.unlock();
			if (s_load_templ_thread_quit) 
			{
				break;
			}
			else 
			{
				std::unique_lock<std::mutex> lk(s_load_templ_thread_sleep_mutex);
				s_load_templ_thread_sleep_condition.wait(lk);
				continue;
			}
		}
		else
		{
			std::string template_name_or_full_name = s_load_templ_queue.front();
			s_load_templ_queue.pop();
			s_load_templ_queue_mutex.unlock();

			std::string file_full_name = PSManager::GetInstance()->GetFullName(template_name_or_full_name);
			TemplateAsyncLoadFinishedInfo info;
			info.template_name_or_full_name = template_name_or_full_name;
			info.file_data = CCFileUtils::getInstance()->getDataFromFile(file_full_name);

			s_load_templ_finished_queue_mutex.lock();
			s_load_templ_finished_queue.push(info);
			s_load_templ_finished_queue_mutex.unlock();
		}
	}

	return 0;
}

static PSManager* s_instance = nullptr;

PSManager *PSManager::GetInstance(void)
{
	if (nullptr == s_instance)
	{
		s_instance = new PSManager;
		return s_instance;
	}
	else
	{
		assert(s_instance->m_is_init && "[PSManager::GetInstance][Must Init First!]");
		return s_instance;
	}
}

void PSManager::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

PSManager::PSManager()
: m_is_init(false)
, m_is_use_mutiple_config_path(false)
, m_async_load_id(0)
{
	m_sys_reader = new PSReader();

	InitEmitterFactory();
	InitAffectorFactory();
	InitRendererFactory();
}

PSManager::~PSManager()
{
}

void PSManager::Init(const std::string& config_path,const std::string& texture_path,const std::string& mesh_path, const std::string& texture_suffix)
{
	m_texture_suffix = texture_suffix;
	if (config_path == USE_MULTIPLE_CONFIG_PATH)
	{
		m_is_use_mutiple_config_path = true;
	}
	else
	{
		m_is_use_mutiple_config_path = false;
		m_config_path = CheckPath(config_path);
	}
	m_texture_path = CheckPath(texture_path);
	m_mesh_path = CheckPath(mesh_path);
	m_is_init = true;
}

void PSManager::Stop()
{
	if (nullptr != s_load_templ_thread)
	{
		s_load_templ_thread_quit = true;
		s_load_templ_thread_sleep_condition.notify_one();
		s_load_templ_thread->join();
		delete s_load_templ_thread;
		s_load_templ_thread = NULL;
	}

	DestoryAllTemplateSystem();

	for (ParticleEmitterFactoryMap::iterator emitter_it = m_emitter_factorys.begin(); emitter_it != m_emitter_factorys.end(); ++emitter_it)
	{
		delete emitter_it->second;
		emitter_it->second = 0;
	}
	m_emitter_factorys.clear();

	for (ParticleAffectorFactoryMap::iterator affector_it = m_affector_factorys.begin(); affector_it != m_affector_factorys.end(); ++affector_it)
	{
		delete affector_it->second;
		affector_it->second = 0;
	}
	m_affector_factorys.clear();

	for (ParticleRendererFactoryMap::iterator renderer_it = m_renderer_factorys.begin(); renderer_it != m_renderer_factorys.end(); ++renderer_it)
	{
		delete renderer_it->second;
		renderer_it->second = 0;
	}
	m_renderer_factorys.clear();

	if (m_sys_reader != 0)
	{
		delete m_sys_reader;
		m_sys_reader = 0;
	}
}

std::string PSManager::CheckPath(const string& path)
{
	std::string str = path;
	size_t size = str.size();
	if (size > 0 && (str[size - 1] != '\\' && str[size - 1] != '/'))
	{
		str += '/';
	}
	return str;
}

void PSManager::AddEmitterFactory(CCParticleEmitterFactory* factory)
{
	assert(factory && "ParticleEmitterFactory is NULL!");
	std::string name = factory->GetType();
	m_emitter_factorys[name] = factory;
}
void PSManager::AddAffectorFactory(CCParticleAffectorFactory* factory)
{
	assert(factory && "ParticleAffectorFactory is NULL!");
	std::string name = factory->GetType();
	m_affector_factorys[name] = factory;
}
void PSManager::AddRendererFactory(CCParticleRendererFactory *factory)
{
	assert(factory && "ParticleRendererFactory is NULL!");
	std::string name = factory->GetType();
	m_renderer_factorys[name] = factory;
}

//初始化Emitter的工厂
void PSManager::InitEmitterFactory()
{
	AddEmitterFactory(new CCParticlePointEmitterFactory);
	AddEmitterFactory(new CCParticleCircleEmitterFactory);
	AddEmitterFactory(new CCParticleBoxEmitterFactory);
	AddEmitterFactory(new CCParticleLineEmitterFactory);
	AddEmitterFactory(new CCParticlePositionEmitterFactory);
	AddEmitterFactory(new CCParticleSphereEmitterFactory);
}

//初始化影响器的工厂
void PSManager::InitAffectorFactory()
{
	AddAffectorFactory(new CCParticleScaleAffectorFactory());
	AddAffectorFactory(new CCParticleColourAffectorFactory());
	AddAffectorFactory(new CCParticleLinearForceAffectorFactory());
	AddAffectorFactory(new CCParticleRandomiserAffectorFactory());
	AddAffectorFactory(new CCParticleRotationAffectorFactory());
	AddAffectorFactory(new CCParticleTexAnimAffectorFactory());
	AddAffectorFactory(new CCPathFollowerAffectorFactory());
	AddAffectorFactory(new CCParticleElasticityAffectorFactory());
	AddAffectorFactory(new CCParticleDeflectorAffectorFactory());
	AddAffectorFactory(new CCParticleVortexAffectorFactory());
	AddAffectorFactory(new CCParticleSuctionAffectorFactory());
}

//初始化Renderer的工厂
void PSManager::InitRendererFactory()
{
	AddRendererFactory(new CCBillboardParticleRendererFactory());
	AddRendererFactory(new CCRibbonParticleRendererFactory());
	AddRendererFactory(new CCEntityParticleRendererFactory());
	AddRendererFactory(new CCEmittedPSParticleRendererFactory());
}

CCParticleEmitter* PSManager::CreateEmitter(const std::string& type, CCParticleTechnique* pt)
{
	ParticleEmitterFactoryMap::iterator it = m_emitter_factorys.find(type);
	if (it == m_emitter_factorys.end())
	{
		return 0;
	}

	return it->second->CreateEmitter(pt);
}

CCParticleAffector* PSManager::CreateAffector(const std::string& type, CCParticleTechnique* pt)
{
	ParticleAffectorFactoryMap::iterator it = m_affector_factorys.find(type);
	if (it == m_affector_factorys.end())
	{
		return 0;
	}

	return it->second->CreateAffector(pt);
}

CCParticleRenderer* PSManager::CreateRenderer(const std::string& type, CCParticleTechnique* pt)
{
	ParticleRendererFactoryMap::iterator it = m_renderer_factorys.find(type);
	if (it == m_renderer_factorys.end())
	{
		return 0;
	}

	return it->second->CreateRenderer(pt, m_texture_suffix);
}

//处理ParticleSystem
MyParticleSystem* PSManager::GetTemplate(const std::string& name)
{
	MyParticleSystem* ps = nullptr;

	ParticleTemplateMap::iterator it = m_system_templates.find(name);
	if (it != m_system_templates.end())
	{
		ps = it->second;
	}

	return ps;
}

void PSManager::DestoryTemplate(const std::string& name)
{
	ParticleTemplateMap::iterator it = m_system_templates.find(name);
	if (it != m_system_templates.end())
	{
		delete it->second;
		m_system_templates.erase(it);
	}
}

//删除所有的模板粒子系统
void PSManager::DestoryAllTemplateSystem()
{
	for (ParticleTemplateMap::iterator it = m_system_templates.begin(); it != m_system_templates.end(); ++it)
	{
		delete it->second;
	}
	m_system_templates.clear();
}

bool PSManager::IsTemplateExist(const std::string& name)
{
	ParticleTemplateMap::iterator it = m_system_templates.find(name);
	bool ret = it != m_system_templates.end();

	return ret;
}

MyParticleSystem* PSManager::CreateTemplate(const std::string& template_name_or_full_name)
{
	std::string base_name;
	std::string ext;
	std::string path;
	std::string file_full_name = GetFullName(template_name_or_full_name);
	StrUtil::splitFullFilename(file_full_name, base_name, ext, path);

	MyParticleSystem *ps = nullptr;

	ParticleTemplateMap::iterator it = m_system_templates.find(base_name);
	if (it != m_system_templates.end())
	{
		ps = it->second;
		return ps;
	}

	if (!FileUtils::getInstance()->isFileExist(file_full_name))
	{
		return ps;
	}

	ps = new MyParticleSystem(base_name);
	ps->SetIsTemplate(true);
	if (m_sys_reader->ParseFile(file_full_name.c_str(), ps))
	{
		m_system_templates.insert(std::make_pair(base_name, ps));
	}
	else
	{
		CC_SAFE_DELETE(ps);
		ps = nullptr;
	}

	return ps;
}

MyParticleSystem* PSManager::CreateTemplateByData(const std::string& template_name_or_full_name, const cocos2d::Data& file_data)
{
	std::string base_name;
	std::string ext;
	std::string path;
	std::string file_full_name = GetFullName(template_name_or_full_name);
	StrUtil::splitFullFilename(file_full_name, base_name, ext, path);

	MyParticleSystem *ps = nullptr;

	ParticleTemplateMap::iterator it = m_system_templates.find(base_name);
	if (it != m_system_templates.end())
	{
		ps = it->second;
		return ps;
	}

	if (file_data.isNull())
	{
		return ps;
	}

	ps = new MyParticleSystem(base_name);
	ps->SetIsTemplate(true);
	if (m_sys_reader->ParseBuffer((const char*)(file_data.getBytes()), file_data.getSize(), ps))
	{
		m_system_templates.insert(std::make_pair(base_name, ps));
	}
	else
	{
		CC_SAFE_DELETE(ps);
		ps = nullptr;
	}

	return ps;
}

//异步创建模板粒子
void PSManager::CreateTemplateAsync(const std::string& template_name_or_full_name, MyParticleSystem* new_ps)
{
	std::string file_full_name = GetFullName(template_name_or_full_name);
	if (file_full_name.empty())
	{
		return;
	}

	if (nullptr == new_ps)
	{
		return;
	}

	++m_async_load_id;
	new_ps->SetAsyncLoadID(m_async_load_id);
	m_ps_async_id_associate_map.insert(std::make_pair(m_async_load_id, new_ps));

	TemplAsyncIDVecAssociateMap::iterator it = m_templ_async_ids_associate_map.find(file_full_name);
	if (it != m_templ_async_ids_associate_map.end())
	{
		it->second.push_back(m_async_load_id);
		return;
	}

	std::vector<unsigned int> async_list;
	async_list.push_back(m_async_load_id);
	m_templ_async_ids_associate_map.insert(std::make_pair(file_full_name, async_list));

	LoadTemplateAsync(template_name_or_full_name.c_str());
}

void PSManager::CreateTemplateAsync(const std::string& template_name_or_full_name, TEMPL_ASYNC_LOAD_FINISHED_CALLBACK callback_func)
{
	std::string file_full_name = GetFullName(template_name_or_full_name);
	if (file_full_name.empty())
	{
		if (callback_func)
		{
			callback_func(template_name_or_full_name.c_str());
		}
		return;
	}

	TemplAsyncFinishedCallbackAssociateMap::iterator it = m_templ_async_finished_callback_map.find(file_full_name);
	if (it != m_templ_async_finished_callback_map.end())
	{
		return;
	}

	m_templ_async_finished_callback_map.insert(std::make_pair(file_full_name, callback_func));

	LoadTemplateAsync(template_name_or_full_name.c_str());
}

//异步加载模板粒子(内部处理)
void PSManager::LoadTemplateAsync(const char* template_name_or_full_name)
{
	if (nullptr == s_load_templ_thread)
	{
		s_load_templ_thread = new std::thread(&LoadTemplateThread, this);
	}
	if (0 == s_load_templ_count)
	{
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(PSManager::LoadTemplateAsyncCallback), this, 0, false);
	}
	++s_load_templ_count;

	s_load_templ_queue_mutex.lock();
	s_load_templ_queue.push(template_name_or_full_name);
	s_load_templ_queue_mutex.unlock();

	s_load_templ_thread_sleep_condition.notify_one();
}

//模板粒子加载完成处理
void PSManager::LoadTemplateAsyncCallback(float dt)
{
	s_load_templ_finished_queue_mutex.lock();
	if (s_load_templ_finished_queue.empty())
	{
		s_load_templ_finished_queue_mutex.unlock();
	}
	else
	{
		TemplateAsyncLoadFinishedInfo info = s_load_templ_finished_queue.front();
		s_load_templ_finished_queue.pop();
		s_load_templ_finished_queue_mutex.unlock();

		CreateTemplateByData(info.template_name_or_full_name, info.file_data);

		std::string file_full_name = GetFullName(info.template_name_or_full_name);
		TemplAsyncIDVecAssociateMap::iterator it = m_templ_async_ids_associate_map.find(file_full_name);
		if (it != m_templ_async_ids_associate_map.end())
		{
			std::vector<unsigned int> async_list = it->second;
			int async_num = async_list.size();
			for (int i = 0; i < async_num; ++i)
			{
				ParticleAsyncIDAssociateMap::iterator it_ps = m_ps_async_id_associate_map.find(async_list[i]);
				if (it_ps != m_ps_async_id_associate_map.end())
				{
					//粒子模板异步加载完成回调
					it_ps->second->SetAsyncLoadID(0);
					it_ps->second->OnTemplateAsyncLoadFinished(info.template_name_or_full_name.c_str());
					m_ps_async_id_associate_map.erase(it_ps);
				}
			}
			
			m_templ_async_ids_associate_map.erase(it);
		}

		TemplAsyncFinishedCallbackAssociateMap::iterator it_fc = m_templ_async_finished_callback_map.find(file_full_name);
		if (it_fc != m_templ_async_finished_callback_map.end())
		{
			if (it_fc->second)
			{
				//预加载粒子模板回调
				it_fc->second(info.template_name_or_full_name.c_str());
			}
			m_templ_async_finished_callback_map.erase(it_fc);
		}

		--s_load_templ_count;
		if (0 == s_load_templ_count)
		{
			CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(PSManager::LoadTemplateAsyncCallback), this);
		}
	}
}

//中断异步创建模板粒子
void PSManager::AbortTemplateAsync(unsigned int async_id)
{
	ParticleAsyncIDAssociateMap::iterator it = m_ps_async_id_associate_map.find(async_id);
	if (it != m_ps_async_id_associate_map.end())
	{
		m_ps_async_id_associate_map.erase(it);
	}
}

void PSManager::SaveScript(MyParticleSystem *ps, const char *file_name)
{
	m_sys_reader->SaveScript(ps, file_name);
}

std::string PSManager::GetFullName(const std::string& template_name_or_full_name)
{
	std::string full_name = template_name_or_full_name;
	if (!m_is_use_mutiple_config_path)
	{
		full_name = m_config_path + template_name_or_full_name + ".xml";
	}

	return FileUtils::getInstance()->fullPathForFilename(full_name.c_str());
}

std::string PSManager::GetTemplatePath(const std::string& template_name)
{
	std::string template_path = template_name;
	if (!m_is_use_mutiple_config_path)
	{
		template_path = m_config_path + template_name + ".xml";
	}

	return template_path;
}

NS_CC_END
