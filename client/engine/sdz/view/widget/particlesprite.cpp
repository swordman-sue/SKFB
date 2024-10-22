#include "particlesprite.h"
#include "luaengine/luaengine.h"
#include "particle/particledatamanager.h"
#include "resmanager/resmanager.h"

USING_NS_CC;

static GameType::UINT32 s_async_load_fun_id = 0;
static GameType::UINT32 s_async_load_num = 0;

void TemplateAsyncLoadFinishedCallback(const char* template_name_or_full_name)
{
	if (s_async_load_num <= 0)
	{
		return;
	}

	LuaEngine::GetInstance()->CallFunction(s_async_load_fun_id, "s", template_name_or_full_name);

	--s_async_load_num;
	if (s_async_load_num <= 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(s_async_load_fun_id);
		s_async_load_fun_id = 0;
	}
}

ParticleSprite::ParticleSprite() :m_particle(NULL)
	,m_play_end_listener_id(0)
	,m_down_fun_id(0)
	,m_move_fun_id(0)
	,m_up_fun_id(0)
{
}

ParticleSprite::~ParticleSprite(void)
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_play_end_listener_id);
	if(m_particle)
	{
		this->removeChild(m_particle,true);
	}
}

ParticleSprite* ParticleSprite::Create()
{
	ParticleSprite* widget = new ParticleSprite();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

ParticleSprite* ParticleSprite::Create(const char* template_name)
{
	ParticleSprite* widget = ParticleSprite::Create();
	if(widget)
	{
		widget->InitWithTemplate(template_name);
	}else
	{
		CC_SAFE_DELETE(widget);
	}
	return widget;
}

ParticleSprite* ParticleSprite::CreateAsync(const char* template_name)
{
	ParticleSprite* widget = ParticleSprite::Create();
	if (widget)
	{
		widget->InitWithTemplate(template_name, true);
	}
	else
	{
		CC_SAFE_DELETE(widget);
	}

	//动态解压随包资源
	std::string template_path = cocos2d::PSManager::GetInstance()->GetTemplatePath(template_name);
	ResManager::GetInstance()->UnzipInitResDynamically(template_path.c_str());

	return widget;
}

void ParticleSprite::InitWithTemplate(const char* template_name, bool is_async)
{
	m_template_name = template_name;

	if (is_async)
	{
		m_particle = cocos2d::MyParticleSystem::CreateAsync(template_name);
	}
	else
	{
		m_particle = cocos2d::MyParticleSystem::Create(template_name);
	}
	if(m_particle)
	{
		this->addChild(m_particle);
	}
}

bool ParticleSprite::PreloadTemplates(lua_tinker::table t, GameType::UINT32 lua_fun_id)
{
	if (s_async_load_num > 0)
	{
		LogUtil::LogError("[ParticleSprite::CreateTemplate] already in async load!");
		return false;
	}

	if (!lua_tinker::is_table(t))
	{
		return false;
	}

	s_async_load_num = 0;
	s_async_load_fun_id = lua_fun_id;

	lua_State *L = t.m_obj->m_L;
	int index = t.m_obj->m_index;
	lua_pushnil(L);
	while (0 != lua_next(L, index))
	{
		++s_async_load_num;
		const char* template_name = lua_tostring(L, -1);
		cocos2d::PSManager::GetInstance()->CreateTemplateAsync(template_name, TemplateAsyncLoadFinishedCallback);
		lua_pop(L, 1);
	}

	return true;
}

bool ParticleSprite::DeleteTemplates(lua_tinker::table t)
{
	if (!lua_tinker::is_table(t))
	{
		return false;
	}

	lua_State *L = t.m_obj->m_L;
	int index = t.m_obj->m_index;
	lua_pushnil(L);
	while (0 != lua_next(L, index))
	{
		const char* template_name = lua_tostring(L, -1);
		cocos2d::PSManager::GetInstance()->DestoryTemplate(template_name);
		lua_pop(L, 1);
	}

	return true;
}

void ParticleSprite::SetPlayEndListener(GameType::UINT32 lua_fun_id)
{
	using namespace cocos2d;
	m_play_end_listener_id = lua_fun_id;
	if(m_particle)
	{
		m_particle->SetCycleCallBack(this, callfunc_selector(ParticleSprite::OnCycleCallback));
	}
}

void ParticleSprite::OnCycleCallback()
{
	if (m_play_end_listener_id)
	{
		LuaEngine::GetInstance()->CallFunction(m_play_end_listener_id,"");
	}
}

void ParticleSprite::SetCycle(bool cycle)
{
	if(m_particle)
	{
		m_particle->SetCycle(cycle);
	}
}

void ParticleSprite::SetEnabled(bool enabled)
{
	if (m_particle)
	{
		m_particle->SetEnabled(enabled);
	}
}

float ParticleSprite::GetSinceStartTime()
{ 
	if (m_particle)
	{
		return m_particle->GetSinceStartTime();
	}
	return 0;
}

float ParticleSprite::GetTimeScale()
{
	if (m_particle)
	{
		return m_particle->GetScaleTime();
	}
	return 0;
}
void ParticleSprite::SetTimeScale(float time)
{
	if (m_particle)
	{
		m_particle->SetScaleTime(time);
	}
}

void ParticleSprite::SetFramePreSecond(int frame_num)
{
	if (m_particle)
	{
		m_particle->SetFramePreSecond(frame_num);
	}
}

void ParticleSprite::SetMansualUpdate(bool is_stop)
{
	if (m_particle)
	{
		m_particle->SetManualUpdate(is_stop);
	}
}

void ParticleSprite::SetScale(float x, float y, float z)
{
	if(m_particle)
	{
		m_particle->SetScale(x,y,z);
	}
}

void ParticleSprite::SetFlipX(bool is_flip)
{
	if (m_particle)
	{
		m_particle->SetFlipX(is_flip);
	}
}

void ParticleSprite::SetFlipY(bool is_flip)
{
	if (m_particle)
	{
		m_particle->SetFlipY(is_flip);
	}
}

cocos2d::Size ParticleSprite::GetContentSize()
{
	if(m_particle)
	{
		return m_particle->getContentSize();
	}else
	{
		return cocos2d::Size(0,0);
	}
}

void ParticleSprite::MakeGrey(bool is_grey)
{
	if (m_particle != nullptr)
	{
		m_particle->MakeGrey(is_grey);
	}
}

std::string ParticleSprite::getDescription() const
{
	return "ParticleSprite";
}

cocos2d::Node* ParticleSprite::createCloneInstance()
{
	return ParticleSprite::Create();
}

void ParticleSprite::copySpecialProperties(cocos2d::Node* model)
{
	ParticleSprite *particle = (ParticleSprite*)model;
	this->InitWithTemplate(particle->m_template_name);
}

