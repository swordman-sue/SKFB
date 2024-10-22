#include "technique.h"
#include "simplespline.h"
#include "particle.h"
#include "dynamicattribute.h"
#include "particlehelper.h"
#include "attribute.h"
#include "render/renderer.h"
#include "emitter/emitter.h"
#include "affector/affector.h"
#include "particlesystem.h"
#include "particledatamanager.h"
#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "cocos/base/CCDirector.h"
#include "tinyxml2/tinyxml2.h"
#include "math/CCMath.h"
#include "cocos/math/TransformUtils.h"
#include <vector>
#include <assert.h>
#include <algorithm>

NS_CC_BEGIN

const float CCParticleTechnique::DEFAULT_WIDTH = 64.0;
const float CCParticleTechnique::DEFAULT_HEIGHT = 64.0;
const float CCParticleTechnique::DEFAULT_DEPTH = 64.0;
const float CCParticleTechnique::DEFAULT_ANGLE = 0.0;
const size_t CCParticleTechnique::DEFAULT_PARTICLE_QUOTA = 1;
const size_t CCParticleTechnique::DEFAULT_EMITTER_QUOTA = 0;
const Vec3 CCParticleTechnique::DEFAULT_AXIS (1, 0, 0);
const Vec3 CCParticleTechnique::DEFAULT_POS (0, 0, 0);

struct CompareAffect
{
	bool operator()(const CCParticleAffector* l, const CCParticleAffector* r)
	{
		return l->GetAffectorWeight() < r->GetAffectorWeight();
	}
};

CCParticleTechnique::CCParticleTechnique(MyParticleSystem* ps) :
m_particle_system(ps),
	m_render(0),
	m_default_width(DEFAULT_WIDTH),
	m_default_height(DEFAULT_HEIGHT),
	m_default_depth(DEFAULT_DEPTH),
	m_emitted_emitter_quota(DEFAULT_EMITTER_QUOTA),
	m_tech_axis(DEFAULT_AXIS),
	m_tech_angle(DEFAULT_ANGLE),
	m_once_init_emitted(false),
	m_visual_particle_quota(DEFAULT_PARTICLE_QUOTA),
	m_enabled(true),
	m_is_local(true),
	m_stop_emitted(false),
	m_emitter_dirty(false),
	m_last_time(0.0167f),
	m_outside_pos(DEFAULT_POS),
	m_is_ps_emit(false)
{
	m_tech_to_world_mat.setIdentity();
	SetVisualParticleQuota(DEFAULT_PARTICLE_QUOTA);
}

CCParticleTechnique::~CCParticleTechnique()
{
	DestoryAllEmitter();
	DestoryAllAffector();

	DestoryAllEmittedEmitters();
	DestoryParticlePool();

	if (m_render)
	{
		m_render->release();
 		m_render = 0;
	}
}

MyParticleSystem* CCParticleTechnique::GetParticleSystem(void) const
{
	return m_particle_system;
}

CCParticleRenderer* CCParticleTechnique::GetRender() const
{
	return m_render;
}

void CCParticleTechnique::SetRender(CCParticleRenderer* render, bool flag)
{
	if (m_render == render)
	{
		return;
	}

	if (m_render != 0)
	{
		DestoryRender();
	}

	m_render = render;
	m_render->retain();
	if (m_render)
	{
		m_render_type = m_render->GetRendererType();
	}
	if (m_render && flag)
	{
		m_render->NotifyParticleQuota(m_visual_particle_quota);
	}
}

CCParticleRenderer* CCParticleTechnique::CreateRender(const std::string& type, bool flag)
{
	if (m_render)
	{
		DestoryRender();
	}

	if (type != "")
	{
		m_render_type = type;
		m_render = PSManager::GetInstance()->CreateRenderer(type, this);
		if (m_render)
		{
			m_render->retain();
			if (flag)
			{
				m_render->NotifyParticleQuota(m_visual_particle_quota);
			}
			return m_render;
		}
	}
	return 0;
}

unsigned int CCParticleTechnique::GetNumEmitter()
{
	return m_emitter_array.size();
}

int CCParticleTechnique::IndexOfEmitter(const std::string& name)
{
	if (name == "")
	{
		return -1;
	}

	size_t size = m_emitter_array.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (strcmp(m_emitter_array[i]->GetName().c_str(), name.c_str()) == 0)
		{
			return i;
		}
	}

	return -1;
}

bool CCParticleTechnique::IsExistEmitter(const std::string& name)
{
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return true;
		}
	}
	return false;
}

CCParticleEmitter* CCParticleTechnique::GetEmitter(const std::string& name)
{
	if (name == "")
	{
		return 0;
	}

	for (EmitterArray::const_iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		if (strcmp((*it)->GetName().c_str(), name.c_str()) == 0)
		{
			return *it;
		}
	}

	return 0;
}

CCParticleEmitter* CCParticleTechnique::GetEmitter(unsigned int index)
{
	assert(index < m_emitter_array.size() && "Emitter index out of bounds!");
	return m_emitter_array[index];
}

CCParticleEmitter* CCParticleTechnique::CreateEmitter(const std::string& type)
{
	CCParticleEmitter* emitter = PSManager::GetInstance()->CreateEmitter(type, this);
	if (emitter == 0)
	{
		return 0;
	}
	m_emitter_array.push_back(emitter);
	return emitter;
}

bool CCParticleTechnique::DestoryEmitter(unsigned int index)
{
	if (index>= m_str_to_emitter_array_map.size())
	{
		return false;
	}

	EmitterArray::iterator iter = m_emitter_array.begin() + index;
	CCParticleEmitter* emitter = *iter;
	if (emitter != 0)
	{
		delete emitter;
		emitter = 0;
	}
	m_emitter_array.erase(iter);
	return true;
}

bool CCParticleTechnique::DestoryEmitter(const std::string& name)
{
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		CCParticleEmitter* emitter = *it;
		if (emitter==0)
		{
			continue;
		}

		if (emitter->GetName() == name)
		{
			delete emitter;
			emitter = 0;
			m_emitter_array.erase(it);
			return true;
		}
	}
	return false;
}


unsigned int CCParticleTechnique::GetNumAffector()
{
	return m_particle_affector_list.size();
}

int CCParticleTechnique::IndexOfAffector(const std::string& name)
{
	if (name == "")
	{
		return -1;
	}

	size_t size = m_particle_affector_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (strcmp(m_particle_affector_list[i]->GetName().c_str(), name.c_str()) == 0)
		{
			return i;
		}
	}

	return -1;
}
bool CCParticleTechnique::IsExistAffector(const std::string& name)
{
	ParticleAffectorList::iterator it;
	ParticleAffectorList::iterator ie = m_particle_affector_list.end();
	for (it = m_particle_affector_list.begin(); it != ie; ++it)
	{
		if ((*it)->GetName() == name)
		{
			return true;
		}
	}
	return false;
}

CCParticleAffector* CCParticleTechnique::GetAffector(const std::string& name)
{
	if (name == "")
	{
		return 0;
	}

	ParticleAffectorIterator it;
	ParticleAffectorIterator ie = m_particle_affector_list.end();
	for (it = m_particle_affector_list.begin(); it != ie; ++it)
	{
		if (strcmp((*it)->GetName().c_str(), name.c_str()) == 0)
		{
			return *it;
		}
	}

	return 0;
}

CCParticleAffector* CCParticleTechnique::GetAffector(unsigned int index)
{
	assert(index < m_particle_affector_list.size() && "Affector index out of bounds!");
	return m_particle_affector_list[index];
}

CCParticleAffector* CCParticleTechnique::CreateAffector(const std::string& type, size_t& index, bool is_sort)
{
	assert(type != "" && "the affector type is empty!");

	CCParticleAffector* affector = PSManager::GetInstance()->CreateAffector(type, this);

	if (is_sort)
	{
		ParticleAffectorList::iterator iter = std::lower_bound(m_particle_affector_list.begin(),m_particle_affector_list.end(),affector, CompareAffect());
		index = iter - m_particle_affector_list.begin();
		m_particle_affector_list.insert(iter, affector);
	}
	else
	{
		m_particle_affector_list.push_back(affector);
	}

	return affector;
}

bool CCParticleTechnique::DestoryAffector(CCParticleAffector* affector)
{
	assert(affector && "Affector is null!");
	ParticleAffectorList::iterator it;
	ParticleAffectorList::iterator ie = m_particle_affector_list.end();
	for (it = m_particle_affector_list.begin(); it != ie; ++it)
	{
		if (*it == affector)
		{
			delete affector;
			affector = 0;
			m_particle_affector_list.erase(it);
			return true;
		}
	}
	return false;
}

bool CCParticleTechnique::DestoryAffector(unsigned int index)
{
	assert(index < m_particle_affector_list.size() && "Affector index out of bounds!");
	ParticleAffectorList::iterator ei = m_particle_affector_list.begin() + index;
	CCParticleAffector* affector = *ei;
	if (affector != 0)
	{
		delete affector;
		affector = 0;
	}
	m_particle_affector_list.erase(ei);
	return true;
}

bool CCParticleTechnique::DestoryAffector(const std::string& name)
{
	for (ParticleAffectorList::iterator it = m_particle_affector_list.begin(); it != m_particle_affector_list.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			CCParticleAffector* affector = *it;
			if (affector != 0)
			{
				delete affector;
				affector = 0;
			}
			m_particle_affector_list.erase(it);
			return true;
		}
	}
	return false;
}

void CCParticleTechnique::DestoryRender()
{
	if (m_render)
	{
		m_render->release();
 		m_render = 0;
	}
}

void CCParticleTechnique::DestoryAllEmitter()
{
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		CCParticleEmitter* emitter = *it;
		if (emitter != 0)
		{
			delete emitter;
			emitter = 0;
		}
	}
	m_emitter_array.clear();
}

void CCParticleTechnique::DestoryAllAffector()
{
	for (ParticleAffectorList::iterator it = m_particle_affector_list.begin(); it != m_particle_affector_list.end(); ++it)
	{
		CCParticleAffector* affector = *it;
		if (affector != 0)
		{
			delete affector;
			affector = 0;
		}
	}
	m_particle_affector_list.clear();
}

//寻找Free的发射器列表
CCParticleTechnique::EmitterList* CCParticleTechnique::FindFreeEmittedEmitter (const std::string& name)
{
	EmitterStrToListMap::iterator it = m_str_to_list_free_emitter_map.find (name);
	if (it != m_str_to_list_free_emitter_map.end())
	{
		return &it->second;
	}

	return 0;
}

//update时的准备
void CCParticleTechnique::Prepare()
{
	// 准备粒子池
	size_t size = m_particle_array.size();
	if (size < m_visual_particle_quota)
	{
		m_particle_array.resize(m_visual_particle_quota);

		for (size_t i = size; i < m_visual_particle_quota; ++i)
		{
			m_particle_array[i] = new CCParticle();
			m_particle_array[i]->m_index = -1;
			m_free_particle_list.push_back(m_particle_array[i]);
		}
	}

	//发射器做准备, 重置发射器
	if (m_str_to_emitter_array_map.empty() && !m_once_init_emitted)
	{
		for (EmitterArray::iterator iter = m_emitter_array.begin(); iter != m_emitter_array.end(); ++iter)
		{
			CCParticleEmitter *emitter_1 = *iter;
			if (emitter_1!=0 && emitter_1->GetEmittedName() != "")
			{
				EmitterArray empty;
				m_str_to_emitter_array_map.insert(std::make_pair(emitter_1->GetEmittedName(), empty));
			}

			emitter_1->SetEmitted(false);
			for (EmitterArray::iterator iter_2 = m_emitter_array.begin(); iter_2 != m_emitter_array.end(); ++iter_2)
			{
				CCParticleEmitter *emitter_2 = *iter_2;
				if (emitter_1 == emitter_2)
				{
					continue;
				}
				if (emitter_1 !=0 && emitter_2 != 0 && emitter_1->GetName() != "" && emitter_1->GetName() == emitter_2->GetEmittedName())
				{
					emitter_1->SetEmitted(true);
					break;
				}
			}
		}
		m_once_init_emitted = true;
	}

	size_t curr_size = 0;
	for (EmitterStrToArrayMap::iterator i = m_str_to_emitter_array_map.begin(); i != m_str_to_emitter_array_map.end(); ++i)
	{
		curr_size += i->second.size();
	}

	if (curr_size < m_emitted_emitter_quota && !m_str_to_emitter_array_map.empty())
	{
		//增加发射器池发射器数目
		size_t max_num_emitter = m_emitted_emitter_quota / m_str_to_emitter_array_map.size();
		for (EmitterStrToArrayMap::iterator it_pool_emitted = m_str_to_emitter_array_map.begin(); 
			it_pool_emitted != m_str_to_emitter_array_map.end(); ++it_pool_emitted)
		{
			CCParticleEmitter *emitter = GetEmitter(it_pool_emitted->first);
			if (emitter == 0)
			{
				continue;
			}

			EmitterArray *emitter_array = &it_pool_emitted->second;
			size_t old_size = emitter_array->size();
			for (size_t t = old_size; t < max_num_emitter; ++t)
			{
				CCParticleEmitter *cloned_emitter = PSManager::GetInstance()->CreateEmitter(emitter->GetEmitterType(), this);
				emitter->CopyAttributesTo(cloned_emitter);
				cloned_emitter->SetEmitted(emitter->IsEmitted());
				emitter_array->push_back(cloned_emitter);
			}
		}

		//将发射器池里面的内容移到Free里面
		for (EmitterStrToArrayMap::iterator it_pool_emitted = m_str_to_emitter_array_map.begin(); 
			it_pool_emitted != m_str_to_emitter_array_map.end(); ++it_pool_emitted)
		{
			std::string name = it_pool_emitted->first;
			EmitterList *free_emitter = FindFreeEmittedEmitter(name);
			if (free_emitter == 0)
			{
				EmitterList empty;
				m_str_to_list_free_emitter_map.insert(std::make_pair(name, empty));
				free_emitter = FindFreeEmittedEmitter(name);
			}

			if (free_emitter == 0)
			{
				break;
			}

			EmitterArray &emitter_array = it_pool_emitted->second;
			size_t old_size = free_emitter->size();
			for (size_t t = old_size; t < emitter_array.size(); ++t)
			{
				free_emitter->push_back(emitter_array[t]);
			}
		}
	}
}

//移除在发射中的发射器
void CCParticleTechnique::RemoveFromActiveEmittedEmitters (CCParticleEmitter* emitter)
{
	if (emitter == 0)
	{
		return;
	}
	for (EmitterList::iterator iter = m_active_emitter_list.begin(); iter != m_active_emitter_list.end(); ++iter)
	{
		if (emitter == (*iter))
		{
			m_active_emitter_list.erase(iter);
			break;
		}
	}
}

//处理死亡
void CCParticleTechnique::Expire(float timeElapsed)
{
	if (m_render == 0)
	{
		return;
	}

	for (ParticleList::iterator iter = m_active_particle_list.begin(); iter != m_active_particle_list.end(); )
	{
		CCParticle *particle = *iter;

		if ( (!particle->m_live_forever && (particle->m_time_live < timeElapsed)) || !IsInRect(particle,IsLocalSpace()))
		{
			if (!m_render->FadeOutParticle(particle, timeElapsed))
			{
				m_render->ParticleExpired(particle);
				if (particle->m_type == CCParticle::Visual)
				{
					m_free_particle_list.splice(m_free_particle_list.end(), m_active_particle_list, iter++);
				}
				else
				{
					CCParticleEmitter *emitter = static_cast<CCParticleEmitter*>(*iter);
					emitter->ResetData();
					EmitterList *free_list = FindFreeEmittedEmitter(emitter->GetName());
					assert(free_list!=0);
					free_list->push_back(emitter);
					RemoveFromActiveEmittedEmitters (emitter);
					iter = m_active_particle_list.erase(iter);
				}
			}
			else
			{
				++iter;
			}
		}
		else
		{
			particle->m_time_live -= timeElapsed;
			if( particle->m_live_forever && particle->m_time_live < 0)
			{
				particle->m_time_live = particle->m_total_live;
			}

			++iter;
		}
	}
}

CCParticle* CCParticleTechnique::CreateParticle(void)
{
	if (m_free_particle_list.empty())
	{
		return 0;
	}

	CCParticle* p = m_free_particle_list.front();
	m_active_particle_list.push_back(p);
	m_free_particle_list.pop_front();
	return p;
}

CCParticle* CCParticleTechnique::CreateEmitterParticle(const std::string& name)
{
	CCParticle* p = 0;
	EmitterList *fee = FindFreeEmittedEmitter(name);
	if (fee && !fee->empty())
	{
		p = fee->front();
		p->m_type = CCParticle::Emitter;
		fee->pop_front();
		m_active_particle_list.push_back(p);
		m_active_emitter_list.push_back(static_cast<CCParticleEmitter*>(p));
	}
	return p;
}

//生成粒子
void CCParticleTechnique::ExecuteTriggerEmitters(CCParticleEmitter* emitter, unsigned int requested, float time_elapsed, bool is_emitted)
{
	if (requested == 0)
	{
		return;
	}

	float time_point = 0.0;
	float inc = time_elapsed / requested;
	for (unsigned int j = 0; j < requested; ++j)
	{
		std::string name = emitter->GetEmittedName();
		CCParticle *p = 0;
		if (name == "")
		{
			p = CreateParticle();
		}
		else
		{
			p = CreateEmitterParticle(name);
		}

		if (p == 0)
		{
			return;
		}

		emitter->InitParticle(p);
		// 对粒子进行排序以ZOrder值
		SortByZorder(m_active_particle_list);
		//if (!is_emitted)
		//{
		//	p->m_position += m_position;
		//}
		// 非本地坐标情况下，把粒子的位置转换到世界坐标
		if (!m_is_local && p->m_type != CCParticle::Emitter)
		{
			Mat4 tech_transf;
			tech_transf=GetTechToWorldMat();
			tech_transf.transformPoint(p->m_position,&p->m_position); //粒子位置转换到世界空间
			tech_transf.transformVector(p->m_direction,&p->m_direction);//粒子方向转换到世界空间
		}

		// 强制发射是瞬间全部发射，不用计算间隔时间
		if( !emitter->IsForceEmit())
		{
			Vec3 offset (0,0,0);
			offset = p->m_direction * time_point;
			Vec3::add(p->m_position, offset,&p->m_position);
		}

		for (ParticleAffectorList::iterator it = m_particle_affector_list.begin(); it != m_particle_affector_list.end(); ++it)
		{
			if ((*it)->IsFitParticle(p))
			{
				(*it)->InitParticle(p);
			}
		}

		time_point += inc;

		//attention: 本地坐标时有些可能变动
		if (p->m_type == CCParticle::Emitter)
		{
			CCParticleEmitter* temp_emitter = static_cast<CCParticleEmitter*>(p);
			temp_emitter->m_relative_position = p->m_position;
		}
	}
}

//生成粒子
void CCParticleTechnique::EmitParticles(float time_elapsed)
{
	size_t total = 0;
	for (EmitterArray::iterator iter = m_emitter_array.begin(); iter != m_emitter_array.end(); ++iter)
	{
		total += (*iter)->GetEmissionCount(time_elapsed, (*iter)->IsEmitted());
	}

	for (EmitterList::iterator it_active = m_active_emitter_list.begin(); it_active != m_active_emitter_list.end(); ++it_active)
	{
		total += (*it_active)->GetEmissionCount(time_elapsed, false);
	}

	size_t allow = 0;
	if (m_visual_particle_quota > m_active_particle_list.size())
	{
		allow = m_visual_particle_quota - m_active_particle_list.size();
	}

	if (allow > m_free_particle_list.size())
	{
		allow = m_free_particle_list.size();
	}

	if (allow == 0)
	{
		return;
	}

	float ratio =  1.0f;
	if (total > allow && total!=0)
	{
		ratio = (float)allow / (float)total;
	}

	for (EmitterArray::iterator iter = m_emitter_array.begin(); iter != m_emitter_array.end(); ++iter)
	{
		if (!(*iter)->IsEmitted())
		{
			ExecuteTriggerEmitters(*iter, static_cast<unsigned int>((*iter)->GetLastCreateCount() * ratio), time_elapsed);
		}
	}

	for (EmitterList::iterator it_active = m_active_emitter_list.begin(); it_active != m_active_emitter_list.end(); ++it_active)
	{
		ExecuteTriggerEmitters(*it_active, static_cast<unsigned int>((*it_active)->GetLastCreateCount() * ratio), time_elapsed, true);
	}
}

//影响器
void CCParticleTechnique::TriggerAffectors(float time_elapsed)
{
	for (ParticleAffectorList::iterator it = m_particle_affector_list.begin(); it != m_particle_affector_list.end(); ++it)
	{
		(*it)->ProcessParticles(time_elapsed);
	}
}

//移动
//attention: 本地坐标时有些可能变动
void CCParticleTechnique::ApplyMotion(float time_elapsed)
{
	for (ParticleList::iterator i = m_active_particle_list.begin(); i != m_active_particle_list.end(); ++i)
	{
		CCParticle *pParticle = static_cast<CCParticle*>(*i);
		if (pParticle->m_time_live <= 0.0f)
		{
			continue;
		}

		float t = time_elapsed;
		if (t > pParticle->m_time_live)
		{
			t = pParticle->m_time_live;
		}
		const Vec3& offset = pParticle->m_direction * t;
		Vec3::add(pParticle->m_position,offset,&pParticle->m_position);

		if (pParticle->m_type == CCParticle::Emitter)
		{
			CCParticleEmitter *emitter = static_cast<CCParticleEmitter*>(*i);
			emitter->m_relative_position = pParticle->m_position;
		}
	}
}

// 获取到粒子系统到世界的转换矩阵
void CCParticleTechnique::CaclTechToWorldOnUpdate()
{
	Mat4 system_transfrom4x4 = Mat4::IDENTITY;
	if (m_is_ps_emit)
	{
		system_transfrom4x4 = m_particle_system->getNodeToParentTransform();
	}
	else
	{
		system_transfrom4x4 = GetParticleSystem()->getNodeToWorldTransform();
	}

	m_tech_to_world_mat = system_transfrom4x4 * GetTechToParentMat();
	m_outside_pos.set(m_tech_to_world_mat.m[12], m_tech_to_world_mat.m[13], m_tech_to_world_mat.m[14]);
}

void CCParticleTechnique::UpdateFromPS(float time_elapsed)
{
	m_last_time = time_elapsed;

	Prepare();
	if (m_particle_array.size() == 0 || m_render == 0)
	{
		return;
	}

	CaclTechToWorldOnUpdate();

	Expire(time_elapsed);

	if (!m_stop_emitted)
	{
		EmitParticles(time_elapsed);
	}

	TriggerAffectors(time_elapsed);
	ApplyMotion(time_elapsed);

	if (m_render)
	{
		m_render->UpdateFromTechnique(m_active_particle_list, time_elapsed);
	}
}

void CCParticleTechnique::draw()
{
	if (m_render == 0 || m_active_particle_list.empty())
	{
		return;
	}

	GLboolean pre_depth_mask, pre_depth_test;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &pre_depth_mask);
	glGetBooleanv(GL_DEPTH_TEST, &pre_depth_test);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	m_render->Draw(m_active_particle_list);

	glDepthMask(pre_depth_mask);
	if (pre_depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

void CCParticleTechnique::SetEmittedEmitterQuota(size_t quota)
{
	m_emitted_emitter_quota = quota;
}

void CCParticleTechnique::NotityLocalSpace(bool space)
{
	m_is_local = space;
	m_bTransformDirty = m_bInverseDirty = true;
}

//拷贝
void CCParticleTechnique::CopyAttributesTo (CCParticleTechnique* technique)
{
	technique->DestoryAllEmitter();
	technique->DestoryAllAffector();
	technique->DestoryRender();

	CCParticleRenderer *render = 0;
	if (m_render != 0)
	{
		render = PSManager::GetInstance()->CreateRenderer(m_render->GetRendererType(), technique);
		if (!render)
		{
			return;
		}
		m_render->CopyAttributesTo(render);
	}
	technique->SetRender(render, false);

	technique->SetName(m_name);
	technique->SetDefaultWidth(m_default_width);
	technique->SetDefaultHeight(m_default_height);
	technique->SetDefaultDepth(m_default_depth);
	technique->SetVisualParticleQuota(m_visual_particle_quota);
	technique->SetEmittedEmitterQuota(m_emitted_emitter_quota);
	technique->SetTechAxis(m_tech_axis);
	technique->SetTechAngle(m_tech_angle);
	technique->setPosition(_position.x, _position.y, _positionZ);
	technique->SetEnabled(m_enabled);
	technique->NotityLocalSpace(m_is_local);

	CCParticleEmitter *emitter;
	CCParticleEmitter *clone_emitter;
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		emitter = *it;
		if (emitter != 0)
		{
			clone_emitter = technique->CreateEmitter(emitter->GetEmitterType());
			emitter->CopyAttributesTo(clone_emitter);
			clone_emitter->SetParent(technique);
		}
	}

	CCParticleAffector *affector;
	CCParticleAffector *clone_affector;
	ParticleAffectorList::iterator ita;
	ParticleAffectorList::iterator iea = m_particle_affector_list.end();
	for (ita = m_particle_affector_list.begin(); ita != iea; ++ita)
	{
		affector = *ita;
		if (affector != 0)
		{
			size_t index = 0;
			clone_affector = technique->CreateAffector(affector->GetType(), index, false);
			affector->CopyAttributesTo(clone_affector);
			clone_affector->SetParent(technique);
		}
	}
}

void CCParticleTechnique::SetVisualParticleQuota(size_t quota)
{
	m_visual_particle_quota = quota;
	if (m_render != 0)
	{
		m_render->NotifyParticleQuota(quota);
	}
}

void CCParticleTechnique::DestoryParticlePool()
{
	for (ParticleArray::iterator it = m_particle_array.begin(); it != m_particle_array.end(); ++it)
	{
		delete (*it);
	}
	m_particle_array.clear();
}

//所有作为粒子的发射器
void CCParticleTechnique::DestoryAllEmittedEmitters()
{
	EmitterStrToArrayMap::iterator it_pool_emitted;
	EmitterArray::iterator it_list_emitted;
	EmitterArray *e;

	for (it_pool_emitted = m_str_to_emitter_array_map.begin(); it_pool_emitted != m_str_to_emitter_array_map.end(); ++it_pool_emitted)
	{
		e = &it_pool_emitted->second;
		for (it_list_emitted = e->begin(); it_list_emitted != e->end(); ++it_list_emitted)
		{
			CCParticleEmitter* emitter = *it_list_emitted;
			if (emitter != 0)
			{
				delete emitter;
				emitter = 0;
			}
		}
		e->clear();
	}

	m_str_to_emitter_array_map.clear();
	m_str_to_list_free_emitter_map.clear();
	m_active_emitter_list.clear();
}

bool CCParticleTechnique::HasLiveForeverEmitter()
{
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		if ((*it)->IsLiveForever())
		{
			return true;
		}
	}
	return false;
}

void CCParticleTechnique::ResetData()
{
	for (EmitterArray::iterator it = m_emitter_array.begin(); it != m_emitter_array.end(); ++it)
	{
		(*it)->ResetData();
	}

	for (ParticleList::iterator i = m_active_particle_list.begin(); i != m_active_particle_list.end(); )
	{
		CCParticle *pParticle = static_cast<CCParticle*>(*i);
		if (pParticle->m_type == CCParticle::Visual)
		{
			m_free_particle_list.splice(m_free_particle_list.end(), m_active_particle_list, i++);
		}
		else
		{
			CCParticleEmitter *pParticleEmitter = static_cast<CCParticleEmitter*>(*i);
			pParticleEmitter->ResetData();
			EmitterList *fee = FindFreeEmittedEmitter(pParticleEmitter->GetName());
			fee->push_back(pParticleEmitter);

			RemoveFromActiveEmittedEmitters (pParticleEmitter);
			i = m_active_particle_list.erase(i);
		}
	}

	ParticleAffectorList::iterator ait;
	ParticleAffectorList::iterator aie = m_particle_affector_list.end();
	for (ait = m_particle_affector_list.begin(); ait != aie; ++ait)
	{
		(*ait)->ResetData();
	}

	if (m_render != 0)
	{
		m_render->ResetData();
	}
}

//解析属性
bool CCParticleTechnique::SetAttribute(const char* name, const char* value)
{
	if (strcmp(name, "name") == 0)
	{
		SetName(value);
	}
	else if (strcmp(name, "particle_quota") == 0)
	{
		SetVisualParticleQuota(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(name, "emitter_quota") == 0)
	{
		SetEmittedEmitterQuota(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(name, "tech_pos") == 0)
	{
		Vec3 pos(0.0f, 0.0f, 0.0f);
		CCParticleHelper::ParseVec3(value, &pos);
		setPosition(pos.x, pos.y, pos.z);
	}
	else if (strcmp(name, "default_width") == 0)
	{
		SetDefaultWidth(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "default_height") == 0)
	{
		SetDefaultHeight(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "default_depth") == 0)
	{
		SetDefaultDepth(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "tech_axis") == 0)
	{
		Vec3 axis (1.0f, 0.0f, 0.0f);
		CCParticleHelper::ParseVec3(value, &axis);
		SetTechAxis(axis);
	}
	else if (strcmp(name, "tech_angle") == 0)
	{
		SetTechAngle(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(name, "tech_enable") == 0)
	{
		SetEnabled(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(name, "is_local") == 0)
	{
		NotityLocalSpace(CCParticleHelper::ParseBool(value));
	}
	else
	{
		log("[CCParticleTechnique::SetAttribute TypeNoExist][Type:%s]",name);
		return false;
	}
	return true;
}

bool CCParticleTechnique::GetAttribute(const char* name, char *value, size_t size)
{
	std::string str;

	if (strcmp(name, "name") == 0)
	{
		str = m_name;
	}
	else if (strcmp(name, "particle_quota") == 0)
	{
		str = CCParticleHelper::ToString((int)m_visual_particle_quota);
	}
	else if (strcmp(name, "emitter_quota") == 0)
	{
		str = CCParticleHelper::ToString((int)m_emitted_emitter_quota);
	}
	else if (strcmp(name, "tech_pos") == 0)
	{
		Vec3 pos (_position.x, _position.y, _positionZ);
		str = CCParticleHelper::ToString(pos);
	}
	else if (strcmp(name, "default_width") == 0)
	{
		str = CCParticleHelper::ToString(m_default_width);
	}
	else if (strcmp(name, "default_height") == 0)
	{
		str = CCParticleHelper::ToString(m_default_height);
	}
	else if (strcmp(name, "default_depth") == 0)
	{
		str = CCParticleHelper::ToString(m_default_depth);
	}
	else if (strcmp(name, "tech_axis") == 0)
	{
		str = CCParticleHelper::ToString(m_tech_axis);
	}
	else if (strcmp(name, "tech_angle") == 0)
	{
		str = CCParticleHelper::ToString(m_tech_angle);
	}
	else if(strcmp(name, "render_type") == 0)
	{
		str = m_render_type;
	}
	else if (strcmp(name, "tech_enable") == 0)
	{
		str = CCParticleHelper::ToString(m_enabled);
	}
	else if (strcmp(name, "is_local") == 0)
	{
		str = CCParticleHelper::ToString(m_is_local);
	}
	else
	{
		log("[CCParticleTechnique::GetAttribute TypeNoExist][Type:%s]",name);
		return false;
	}
	strcpy(value, str.c_str());
	return true;
}

void CCParticleTechnique::setPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_positionZ = z;

	m_bTransformDirty = true;
}

void CCParticleTechnique::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (!m_enabled||!_visible)
	{
		return;
	}
	if (m_stop_emitted && HasLiveForeverEmitter())
	{
		return;
	}
	if (!_parent || !_parent->isVisible())
	{
		return;
	}

	if(!m_is_local)
	{
		if (m_is_ps_emit)
		{
			Mat4 temp;
			temp = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			draw();
			Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,temp);
		}
		else
		{
			// 非本地空间的时候，忽略掉粒子坐标到世界坐标转换
			Mat4 temp = Mat4::IDENTITY;
			Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,temp);
			draw();
			Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		}
	}
	else
	{
		Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, parentTransform * GetTechToParentMat());
		draw();
		Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		//CCNode::visit();
	}
}

// 判断粒子是否在包围盒中
bool CCParticleTechnique::IsInRect(CCParticle* particle,bool is_local_space)
{
	//Rect rect = GetParticleSystem()->GetCustomBox();
	//Vec3 position=particle->m_position;
	//if(is_local_space)
	//{
	//	kmVec3Transform(&position,&particle->m_position,&m_tech_space_transf);
	//}
	//else
	//{
	//	CCAffineTransform system_to_scene = GetParticleSystem()->nodeToParentTransform();
	//	rect= CCRectApplyAffineTransform(GetParticleSystem()->GetCustomBox(),system_to_scene);
	//}
	//return rect.containsPoint(CCPoint(position.x,position.y));
	return true;
}

bool CCParticleTechnique::IsTemplate()
{
	return m_particle_system->IsTemplate();
}

void CCParticleTechnique::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	if (my_document == 0 || element == 0)
	{
		return;
	}

	tinyxml2::XMLElement *child;
	tinyxml2::XMLText *content;
	std::string str;

	if (!m_name.empty())
	{
		child = my_document->NewElement("name");
		element->LinkEndChild(child);
		content = my_document->NewText(m_name.c_str());
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual((int)m_visual_particle_quota, (int)CCParticleTechnique::DEFAULT_PARTICLE_QUOTA))
	{
		str = CCParticleHelper::ToString((int)m_visual_particle_quota);
		child = my_document->NewElement("particle_quota");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual((int)m_emitted_emitter_quota, (int)CCParticleTechnique::DEFAULT_EMITTER_QUOTA))
	{
		str = CCParticleHelper::ToString((int)m_emitted_emitter_quota);
		child = my_document->NewElement("emitter_quota");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	Vec3 pos (_position.x, _position.y, _positionZ);
	if (!CCParticleHelper::IsEqual(pos, CCParticleTechnique::DEFAULT_POS))
	{
		str = CCParticleHelper::ToString(pos);
		child = my_document->NewElement("tech_pos");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_default_width, CCParticleTechnique::DEFAULT_WIDTH))
	{
		str = CCParticleHelper::ToString(m_default_width);
		child = my_document->NewElement("default_width");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_default_height, CCParticleTechnique::DEFAULT_HEIGHT))
	{
		str = CCParticleHelper::ToString(m_default_height);
		child = my_document->NewElement("default_height");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_default_depth, CCParticleTechnique::DEFAULT_DEPTH))
	{
		str = CCParticleHelper::ToString(m_default_depth);
		child = my_document->NewElement("default_depth");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_tech_axis, CCParticleTechnique::DEFAULT_AXIS))
	{
		str = CCParticleHelper::ToString(m_tech_axis);
		child = my_document->NewElement("tech_axis");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_tech_angle, CCParticleTechnique::DEFAULT_ANGLE))
	{
		str = CCParticleHelper::ToString(m_tech_angle);
		child = my_document->NewElement("tech_angle");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!m_is_local)
	{
		str = CCParticleHelper::ToString(m_is_local);
		child =my_document->NewElement("is_local");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_render != 0)
	{
		child = my_document->NewElement("render");
		child->SetAttribute("type", m_render->GetRendererType().c_str());
		element->LinkEndChild(child);
		m_render->SaveScript(my_document, child);
	}

	size_t size = GetNumEmitter();
	for (size_t i = 0; i < size; ++i)
	{
		CCParticleEmitter *emitter = GetEmitter(i);
		child = my_document->NewElement("Emitter");
		child->SetAttribute("type", emitter->GetEmitterType().c_str());
		element->LinkEndChild(child);
		emitter->SaveScript(my_document, child);
	}

	size = GetNumAffector();
	for (size_t i = 0; i < size; ++i)
	{
		CCParticleAffector *affector = GetAffector(i);
		child = my_document->NewElement("Affector");
		child->SetAttribute("type", affector->GetType().c_str());
		element->LinkEndChild(child);
		affector->SaveScript(my_document, child);
	}
}

void CCParticleTechnique::MakeGrey(bool is_grey)
{
	if (m_render != nullptr)
	{
		m_render->MakeGrey(is_grey);
	}
}

void CCParticleTechnique::SortByZorder(std::list<CCParticle*>& active_particles)
{
	if (active_particles.size() < 1)
	{
		return;
	}

	CCParticle* particle = active_particles.back();
	auto it = active_particles.begin();
	for (it; it != active_particles.end(); it++)
	{
		if (particle->m_ZOrder <= (*it)->m_ZOrder)
		{
			m_active_particle_list.insert(it, particle);
			active_particles.pop_back();
			break;
		}
	}
}

const Mat4& CCParticleTechnique::GetTechToParentMat()
{
	if (m_bTransformDirty)
	{
		m_tech_to_parent_mat.setIdentity();
		Mat4::createRotation(m_tech_axis, m_tech_angle* kmPIOver180, &m_tech_to_parent_mat);
		m_tech_to_parent_mat.m[12] = _position.x;
		m_tech_to_parent_mat.m[13] = _position.y;
		m_tech_to_parent_mat.m[14] = _positionZ;
		m_bTransformDirty = false;
	}
	return m_tech_to_parent_mat;
}

NS_CC_END