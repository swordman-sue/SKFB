
#include "linearforce.h"
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
#include <vector>

NS_CC_BEGIN

const Vec3 CCParticleLinearForceAffector::DEFAULT_FORCE_VEC(0.0f, -1.0f, 0.0f);
const float CCParticleLinearForceAffector::DEFAULT_FORCE = 100.0f;
const CCParticleLinearForceAffector::ForceApplication CCParticleLinearForceAffector::DEFAULT_FORCE_APP = CCParticleLinearForceAffector::FA_ADD;

CCParticleLinearForceAffector::CCParticleLinearForceAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_force_vector(DEFAULT_FORCE_VEC),
	m_force_application(DEFAULT_FORCE_APP)
{
	m_type = "LinearForce";

	m_dyn_force = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_force)->SetValue(DEFAULT_FORCE);
}

CCParticleLinearForceAffector::~CCParticleLinearForceAffector()
{
	if (m_dyn_force != 0)
	{
		delete m_dyn_force;
		m_dyn_force = 0;
	}
}

void CCParticleLinearForceAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;
	Vec3 vec (1, 1, 1);
	vec = m_force_vector * AttributeCalculate(m_dyn_force, percent, 100.0f);
	if (m_force_application == FA_ADD)
	{
		if (particle->m_time_live < elapsed)
		{
			elapsed = particle->m_time_live;
		}
		vec =  vec * elapsed;
		
		Vec3::add(particle->m_direction, vec,&particle->m_direction);
	}
	else
	{
		Vec3::add(particle->m_direction, vec,&vec);
		particle->m_direction = vec*0.5f;
	}
}

void CCParticleLinearForceAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleLinearForceAffector* plfa = static_cast<CCParticleLinearForceAffector*>(pa);
	plfa->SetForceVector(m_force_vector);
	plfa->SetForceApplication(m_force_application);
	plfa->SetDynForce(CloneDynamicAttribute(GetDynForce()));
}


bool CCParticleLinearForceAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "force_vector") == 0)
	{
		CCParticleHelper::ParseVec3(value, &m_force_vector);
		m_force_vector.normalize();
	}
	else if (strcmp(type, "froce_app") == 0)
	{
		m_force_application = ForceApplication(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "dyn_force") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynForce(dyn);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleLinearForceAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "force_vector") == 0)
	{
		str = CCParticleHelper::ToString(m_force_vector);
	}
	else if (strcmp(type, "froce_app") == 0)
	{
		str = CCParticleHelper::ToString(int(m_force_application));
	}
	else if (strcmp(type, "dyn_force") == 0)
	{
		GetDynAttribute(m_dyn_force, str);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}


void CCParticleLinearForceAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_force_vector, DEFAULT_FORCE_VEC))
	{
		str = CCParticleHelper::ToString(m_force_vector);
		child = my_document->NewElement("force_vector");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(int(m_force_application), int(DEFAULT_FORCE_APP)))
	{
		str = CCParticleHelper::ToString(int(m_force_application));
		child = my_document->NewElement("froce_app");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!(m_dyn_force->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_force)->GetValue(), DEFAULT_FORCE)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "dyn_force");
		m_dyn_force->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}
}

bool CCParticleLinearForceAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "dyn_force") == 0)
	{
		SetDynForce(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

void CCParticleLinearForceAffector::SetDynForce(DynamicAttribute* dyn)
{
	if (m_dyn_force != 0)
	{
		delete m_dyn_force;
		m_dyn_force = 0;
	}
	m_dyn_force = dyn;
}


CCParticleAffector* CCParticleLinearForceAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleLinearForceAffector(pt);
	return pa;
}

NS_CC_END