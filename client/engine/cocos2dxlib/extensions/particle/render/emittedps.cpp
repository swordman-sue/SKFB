
#include "emittedps.h"
#include "renderer.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "rendererbuffer.h"
#include "particle/technique.h"
#include "particle/particlesystem.h"
#include "particle/particledatamanager.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"


NS_CC_BEGIN

CCEmittedPSParticleRenderer::CCEmittedPSParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix) :
	CCParticleRenderer(pt, texture_suffix),
	m_last_index(0),
	m_can_emitted(false),
	m_is_face_to_direction(false),
	m_is_grey(false)
{
	m_type = "EmittedPS";
}

CCEmittedPSParticleRenderer::~CCEmittedPSParticleRenderer()
{
	Clear();
}

void  CCEmittedPSParticleRenderer::UpdateFromTechnique(std::list<CCParticle*>& current_particles, float time_elapsed)
{
	vector<MyParticleSystem*>::iterator particle = m_ps_list.begin(); 
	for(;particle!=m_ps_list.end();++particle)
	{
		(*particle)->update(time_elapsed);
	}

	size_t count = m_ps_list.size();
	if (m_particle_quota <= count)
	{
		return;
	}

	if (!PSManager::GetInstance()->IsTemplateExist(m_ps_template))
	{
		return;
	}

	std::list<CCParticle*>::iterator iter = current_particles.begin();

	for (size_t index = count; index < m_particle_quota && iter!=current_particles.end(); ++index,++iter)
	{
		MyParticleSystem* ps = MyParticleSystem::Create(m_ps_template.c_str());
		if (ps)
		{
			ps->setVisible(false);
			ps->SetEnabled(false);
			ps->MakeGrey(m_is_grey);
			m_ps_list.push_back(ps);
			ps->retain();

			Vec3 vec3_1 = (*iter)->m_direction;
			Vec3 vec3_2(0,1,0);
			float Angle = Vec3::angle(vec3_1,vec3_2);
			ps->setRotation(Angle);

			int technum = ps->GetNumTechniques();
			for (int i = 0; i < technum; i++)
			{
				ps->GetTechnique(i)->SetIsPSEmit(true);
			}
		}
	}
}

void CCEmittedPSParticleRenderer::Clear()
{
	vector<MyParticleSystem*>::iterator it, ie = m_ps_list.end();
	for (it = m_ps_list.begin(); it != ie; ++it)
	{
		(*it)->release();
	}
	m_ps_list.clear();
}


void CCEmittedPSParticleRenderer::Draw(std::list<CCParticle*>& current_particles)
{
	if (!CanRender())
	{
		return;
	}

	std::list<CCParticle*>::iterator it;
	std::list<CCParticle*>::iterator ie = current_particles.end();
	size_t size = m_ps_list.size();
	Mat4 world_t = m_technique->GetTechToWorldMat();
	//kmMat4 result_t;

	for (it = current_particles.begin(); it != ie; ++it)
	{
		if ((*it)->m_type != CCParticle::Visual)
		{
			continue;
		}

		if ((*it)->m_index < 0)
		{
			int index = FindFirstNoneVisible();
			if (index != -1)
			{
				if (index < (int)size)
				{
					(*it)->m_index = index;
					MyParticleSystem* ps = m_ps_list[(*it)->m_index];
					ps->SetEnabled(true);
				}
				else
				{
					continue;
				}
			}
			else
			{
				break;
			}
		}

		if ((*it)->m_index != -1 && (*it)->m_index < (int)size)
		{
			MyParticleSystem* ps = m_ps_list[(*it)->m_index];
			ps->setVisible(true);

			float temp_angle=0;
			Vec3 directtion = (*it)->m_direction;
			ps->setPosition3D(Vec3((*it)->m_position.x, (*it)->m_position.y, (*it)->m_position.z)); 

			//根据粒子的速度方向，旋转粒子让它指向速度的方向
			if(m_is_face_to_direction)
			{
				Vec3 y(0,1,0);
				if( (directtion.x<0 && directtion.y<0) || (directtion.x<0 && directtion.y>0) )
				{
					directtion.x = -directtion.x;
					directtion.y = -directtion.y;
					temp_angle = Vec3::angle(directtion,y)*kmPIUnder180;
					temp_angle+=180;
				}
				else
				{
					temp_angle = Vec3::angle(directtion,y)*kmPIUnder180;
				}
			}
			else
			{
				temp_angle = (*it)->m_angle;
				ps->SetAnchorPoint(0.5, 0.5);
			}
			ps->setRotation(temp_angle);
		}
	}

	vector<MyParticleSystem*>::iterator particle = m_ps_list.begin(); 

	for(;particle!=m_ps_list.end();++particle)
	{
		Renderer* render = Director::getInstance()->getRenderer();
		Mat4 mat = Mat4::IDENTITY;
		if(m_technique->IsLocalSpace())
		{
			mat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		}
		(*particle)->visit(render,mat,1);
	}
}

void CCEmittedPSParticleRenderer::NotifyParticleQuota(size_t size)
{
	if (size == m_particle_quota)
	{
		return;
	}

	if (size > 20)
	{
		m_particle_quota = 20;
	}

	CCParticleRenderer::NotifyParticleQuota( size);
}

void CCEmittedPSParticleRenderer::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleRenderer::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;

	if (m_ps_template != "")
	{
		child = my_document->NewElement("ps_template");
		content = my_document->NewText(m_ps_template.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if(m_is_face_to_direction)
	{
		child = my_document->NewElement("m_is_face_to_direction");
		content = my_document->NewText(CCParticleHelper::ToString(m_is_face_to_direction).c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}


void CCEmittedPSParticleRenderer::ParticleExpired(CCParticle* particle)
{
	if (particle != 0 && particle->m_index >= 0 && particle->m_index < (int)m_ps_list.size())
	{
		m_ps_list[particle->m_index]->ResetData();
		m_ps_list[particle->m_index]->setVisible(false);
		m_ps_list[particle->m_index]->SetEnabled(false);
		particle->m_index = -1;
	}
}

bool CCEmittedPSParticleRenderer::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "ps_template") == 0)
	{
		if (m_technique->IsTemplate())
		{
			m_ps_template = value;
		}
		else
		{
			SetPSTemplate(value);
		}
	}else if(strcmp(type,"m_is_face_to_direction")==0)
	{
		m_is_face_to_direction = CCParticleHelper::ParseBool(value);
	}
	else
	{
		return CCParticleRenderer::SetAttribute(type, value);
	}
	return true;
}

bool CCEmittedPSParticleRenderer::GetAttribute(const char* type, char* value, size_t size)
{
	string str;
	if (strcmp(type, "ps_template") == 0)
	{
		str = m_ps_template;
	}else if(strcmp(type,"m_is_face_to_direction")==0)
	{
		str = CCParticleHelper::ToString(m_is_face_to_direction);
	}
	else 
	{
		return CCParticleRenderer::GetAttribute(type, value, size);;
	}

	strncpy(value, str.c_str(), size);
	return true;
}

void CCEmittedPSParticleRenderer::CopyAttributesTo(CCParticleRenderer *render)
{
	CCParticleRenderer::CopyAttributesTo(render);
	CCEmittedPSParticleRenderer* ps_render = static_cast<CCEmittedPSParticleRenderer*>(render);
	ps_render->SetPSTemplate(m_ps_template);
	ps_render->SetIsFaceDiretion(m_is_face_to_direction);
}

bool CCEmittedPSParticleRenderer::CanRender()
{
	return m_can_emitted;
}

void CCEmittedPSParticleRenderer::MakeGrey(bool is_grey)
{
	if (m_is_grey == is_grey)
	{
		return;
	}

	m_is_grey = is_grey;
	for (MyParticleSystem* ps : m_ps_list)
	{
		if (ps)
		{
			ps->MakeGrey(m_is_grey);
		}
	}
}

int CCEmittedPSParticleRenderer::FindFirstNoneVisible()
{
	size_t size = m_ps_list.size();
	size_t index  = 0;
	while(index < size)
	{
		if (!m_ps_list[m_last_index]->isVisible())
		{
			return m_last_index;
		}
		m_last_index = (m_last_index + 1) % size;
		++index;
	}

	return -1;
}

void CCEmittedPSParticleRenderer::SetIsFaceDiretion(bool is_face)
{
	m_is_face_to_direction = is_face;
}

void CCEmittedPSParticleRenderer::SetPSTemplate(const string& name)
{
	MyParticleSystem* ps = m_technique->GetParticleSystem();
	if (m_ps_template == name || (ps != 0 && ps->GetTemplateName() == name))
	{
		return;
	}

	m_ps_template = name;
	Clear();

	if (m_ps_template == "")
	{
		m_can_emitted = false;
		return;
	}

	if(!PSManager::GetInstance()->IsTemplateExist(m_ps_template))
	{
		if (PSManager::GetInstance()->CreateTemplate(m_ps_template) == 0)
		{
			m_can_emitted = false;
			return;
		}
	}

	m_can_emitted = true;
}

void CCEmittedPSParticleRenderer::ResetData()
{
	m_last_index = 0;
	vector<MyParticleSystem*>::iterator it, ie = m_ps_list.end();
	for (it = m_ps_list.begin(); it != ie; ++it)
	{
		(*it)->ResetData();
		(*it)->setVisible(false);
		(*it)->SetEnabled(false);
	}
}

NS_CC_END