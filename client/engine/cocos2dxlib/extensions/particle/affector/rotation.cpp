#include "rotation.h"
#include "affector.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"
#include "particle/emitter/emitter.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "particle/technique.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"

NS_CC_BEGIN

const float CCParticleRotationAffector::TWOPIRAD = 360.0f;
const float CCParticleRotationAffector::DEFAULT_SPEED = 0.0f;

CCParticleRotationAffector::CCParticleRotationAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_speed(DEFAULT_SPEED),
	m_dyn_max_value(0.0f)
{
	m_type = "Rotation";
	m_dyn_rot_speed = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_rot_speed)->SetValue(DEFAULT_SPEED);

	m_dyn_start_angle = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_start_angle)->SetValue(0.0f);
}

CCParticleRotationAffector::~CCParticleRotationAffector()
{
	if (m_dyn_rot_speed != 0)
	{
		delete m_dyn_rot_speed;
		m_dyn_rot_speed = 0;
	}

	if (m_dyn_start_angle != 0)
	{
		delete m_dyn_start_angle;
		m_dyn_start_angle = 0;
	}
}

void CCParticleRotationAffector::SetRotationSpeed(DynamicAttribute* dyn)
{
	if (m_dyn_rot_speed != 0)
	{
		delete m_dyn_rot_speed;
		m_dyn_rot_speed = 0;
	}

	m_dyn_rot_speed = dyn;
	if (m_dyn_rot_speed && m_dyn_rot_speed->GetType() != DynamicAttribute::DAT_CURVED)
	{
		m_speed = AttributeCalculate(m_dyn_rot_speed, 0.0f, DEFAULT_SPEED);
	}

	if (m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_CURVED)
	{
		m_dyn_max_value = m_dyn_rot_speed->GetCalculusValue();
	}
}

void CCParticleRotationAffector::SetRotationAngle(DynamicAttribute* dyn)
{
	if (m_dyn_start_angle != 0)
	{
		delete m_dyn_start_angle;
		m_dyn_start_angle = 0;
	}
	m_dyn_start_angle = dyn;
}

void CCParticleRotationAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleRotationAffector* pra = static_cast<CCParticleRotationAffector*>(pa);
	pra->SetRotationSpeed(CloneDynamicAttribute(GetRotationSpeed()));
	pra->SetRotationAngle(CloneDynamicAttribute(GetRotationAngle()));
}

void CCParticleRotationAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	int timeScale = (int)(elapsed / particle->m_total_live);
	elapsed-=particle->m_total_live*timeScale;

	if (particle->m_angle > 360.0f)
	{
		int factor = particle->m_angle / 360;
		particle->m_angle -= factor * 360;
	}

	float maxAngle = m_dyn_start_angle->GetMaxValue(); //粒子每帧可能旋转的最大角度
	float minAngle = m_dyn_start_angle->GetMinValue(); //粒子每帧可能旋转的最小角度

	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;

	if (m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_CURVED)
	{
		if (particle->m_live_forever && particle->m_time_live + elapsed >= particle->m_total_live)
		{
			if (particle->m_angle != particle->m_pre_angle)
			{
				particle->m_angle = particle->m_pre_angle + m_dyn_max_value * particle->m_total_live;
			}
			particle->m_pre_angle = particle->m_angle;
		}

		particle->m_angle += AttributeCalculate(m_dyn_rot_speed, percent, 10.0) * elapsed;

		return;
	}
	else
	{
		if (m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_RANDOM)
		{
			particle->m_angle += particle->m_rotation_random_value * elapsed;
			if (particle->m_rotation_random_value > 0)
			{
				maxAngle += particle->m_rotation_random_value * particle->m_total_live;
			}
			else
			{
				minAngle +=particle->m_rotation_random_value * particle->m_total_live;
			}
		}
		else
		{
			particle->m_angle += m_speed * elapsed;

			if(m_speed>0)
			{
				maxAngle += m_speed * particle->m_total_live;
			}
			else
			{
				minAngle +=m_speed * particle->m_total_live;
			}
		}
	}

	//永久存在的粒子，限制它的角度在可能得范围之内
	if(particle->m_live_forever)
	{
		if(particle->m_angle<minAngle)
		{
			if(minAngle!=0)
			{
				particle->m_angle = (int)particle->m_angle%(int)minAngle;
			}else
			{
				particle->m_angle = minAngle;
			}
			
		}

		if(particle->m_angle>maxAngle)
		{
			if(maxAngle!=0)
			{
				particle->m_angle = (int)particle->m_angle%(int)maxAngle;
			}else
			{
				particle->m_angle = maxAngle;
			}
			
		}
	}
	else
	{
		// 限制粒子角度在一定范围内，避免发生过大的偏差
		if(particle->m_angle<minAngle && !particle->m_live_forever)
			particle->m_angle = minAngle;

		if(particle->m_angle>maxAngle && !particle->m_live_forever)
			particle->m_angle=maxAngle;
	}
}

void CCParticleRotationAffector::InitParticle(CCParticle* particle)
{
	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;
	particle->m_angle = AttributeCalculate(m_dyn_start_angle, percent, 0.0f);

	if (m_dyn_rot_speed && m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_RANDOM)
	{
		particle->m_rotation_random_value = AttributeCalculate(m_dyn_rot_speed, 0.0f, DEFAULT_SPEED);
	}

	if (m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_CURVED)
	{
		particle->m_pre_angle = particle->m_angle;
	}
}

bool CCParticleRotationAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "rot_speed") == 0)
	{
		SetRotationSpeed(dyn);
	}
	else if (strcmp(type, "rot_start_angle") == 0)
	{
		SetRotationAngle(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

bool CCParticleRotationAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "rot_speed") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetRotationSpeed(dyn);
	}
	else if (strcmp(type, "rot_start_angle") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetRotationAngle(dyn);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleRotationAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "rot_speed") == 0)
	{
		GetDynAttribute(m_dyn_rot_speed, str);
	}
	else if (strcmp(type, "rot_start_angle") == 0)
	{
		GetDynAttribute(m_dyn_start_angle, str);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}

	strncpy(value, str.c_str(), size );
	return true;
}

void CCParticleRotationAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;

	if (!(m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_rot_speed)->GetValue(), DEFAULT_SPEED)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "rot_speed");
		m_dyn_rot_speed->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!(m_dyn_start_angle->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_start_angle)->GetValue(), 0.0f)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "rot_start_angle");
		m_dyn_start_angle->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}
}


CCParticleAffector* CCParticleRotationAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleRotationAffector(pt);
	return pa;
}

NS_CC_END