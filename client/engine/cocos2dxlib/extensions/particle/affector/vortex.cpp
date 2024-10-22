
#include "vortex.h"
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
#include <string>
using namespace std;

NS_CC_BEGIN

const Vec3 CCParticleVortexAffector::DEFAULT_ROTATION_VECTOR (0, 0, 1);
const float CCParticleVortexAffector::DEFAULT_ROTATION_SPEED = 60.0f;

CCParticleVortexAffector::CCParticleVortexAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_rot_vec(DEFAULT_ROTATION_VECTOR)
{
	m_rot.setIdentity();
	m_affector_weight = 5;
	m_type = "Vortex";
	m_dyn_rot_speed = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_rot_speed)->SetValue(DEFAULT_ROTATION_SPEED);
}

CCParticleVortexAffector::~CCParticleVortexAffector()
{
	if (m_dyn_rot_speed)
	{
		delete m_dyn_rot_speed;
		m_dyn_rot_speed = 0;
	}
}

void CCParticleVortexAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;
	float angle = AttributeCalculate(m_dyn_rot_speed, percent, 1.0) * elapsed * kmPIOver180;
	Quaternion::QuaternionRotationAxis(&m_rot, &m_rot_vec, angle);

	if (m_parent->IsLocalSpace())
	{
		Quaternion::QuaternionMultiplyVec3(&particle->m_position, &m_rot, &particle->m_position);
	}
// 	else
// 	{
// 		Vec3 offset (0, 0, 0);
// 		Vec3::subtract(particle->m_position, m_parent->GetOutsidePos(),&offset);
// 		Quaternion::QuaternionMultiplyVec3(&offset, &m_rot, &offset);
// 		//Vec3::add(m_parent->GetOutsidePos(), offset,&particle->m_direction);
// 	}
	Quaternion::QuaternionMultiplyVec3(&particle->m_direction, &m_rot, &particle->m_direction);
}

void CCParticleVortexAffector::SetRotVector(const Vec3& vec)
{
	m_rot_vec = vec;
}

void CCParticleVortexAffector::SetDynRotSpeed(DynamicAttribute* dyn)
{
	if (m_dyn_rot_speed != 0)
	{
		delete m_dyn_rot_speed;
		m_dyn_rot_speed = 0;
	}
	m_dyn_rot_speed = dyn;
}

void CCParticleVortexAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleVortexAffector* pva = static_cast<CCParticleVortexAffector*>(pa);
	pva->SetRotVector(m_rot_vec);
	pva->SetDynRotSpeed(CloneDynamicAttribute(GetDynRotSpeed()));
}

void CCParticleVortexAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;

	if (!(m_dyn_rot_speed->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_rot_speed)->GetValue(), DEFAULT_ROTATION_SPEED)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "vortex_rot_speed");
		m_dyn_rot_speed->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!CCParticleHelper::IsEqual(m_rot_vec, DEFAULT_ROTATION_VECTOR))
	{
		string str = CCParticleHelper::ToString(m_rot_vec);
		child = my_document->NewElement("vortex_vec");
		tinyxml2::XMLText* content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCParticleVortexAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "vortex_rot_speed") == 0)
	{
		SetDynRotSpeed(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

bool CCParticleVortexAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "vortex_rot_speed") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynRotSpeed(dyn);
	}
	else if (strcmp(type, "vortex_vec") == 0)
	{
		CCParticleHelper::ParseVec3(value, &m_rot_vec);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleVortexAffector::GetAttribute(const char* type, char* value, size_t size)
{
	string str;
	if (strcmp(type, "vortex_rot_speed") == 0)
	{
		GetDynAttribute(m_dyn_rot_speed, str);
	}
	else if (strcmp(type, "vortex_vec") == 0)
	{
		str = CCParticleHelper::ToString(m_rot_vec);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}

	strncpy(value, str.c_str(), size);
	return true;
}

CCParticleAffector* CCParticleVortexAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleVortexAffector(pt);
	return pa;
}

NS_CC_END