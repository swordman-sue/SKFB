
#include "suction.h"
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

const float CCParticleSuctionAffector::DEFAULT_SUCTION = 64.0f;
const Vec3 CCParticleSuctionAffector::DEFAULT_SUCTION_POS (0.0f, 0.0f, 0.0f);
const bool CCParticleSuctionAffector::DEFAULT_COLLIDE_VISIBLE = false;

CCParticleSuctionAffector::CCParticleSuctionAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_position(DEFAULT_SUCTION_POS),
	m_collide_visible(DEFAULT_COLLIDE_VISIBLE),
	m_derived_pos(DEFAULT_SUCTION_POS),
	m_suc_type(SUC_POS)
{
	m_affector_weight = 6;
	m_type = "Suction";
	m_dyn_suction = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_suction)->SetValue(DEFAULT_SUCTION);
}

CCParticleSuctionAffector::~CCParticleSuctionAffector()
{
	if (m_dyn_suction)
	{
		delete m_dyn_suction;
		m_dyn_suction = 0;
	}
}

void CCParticleSuctionAffector::SetSuctionPos(const Vec3& pos)
{
	m_position = pos;
}

void CCParticleSuctionAffector::SetDynSuction(DynamicAttribute *dyn)
{
	if (m_dyn_suction != 0)
	{
		delete m_dyn_suction;
		m_dyn_suction = 0;
	}
	m_dyn_suction = dyn;
}

void CCParticleSuctionAffector::PreAffector(float elapsed)
{
	//m_derived_pos = m_position;
	//if (m_parent)
	//{
	//	// kmVec3Transform(&m_derived_pos, &m_position, &m_parent->GetDerivedTransform());
	//}
}

void CCParticleSuctionAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;
	float force = AttributeCalculate(m_dyn_suction, percent, 64.0f);
	Vec3 dir (0, 0, 0);
	Vec3::subtract(m_position,particle->m_position,&dir);
	float len = dir.length();
	dir.normalize();
	if (percent <= m_end_time && percent >= m_start_time && len < 64.0f && !m_collide_visible)
	{
		particle->m_time_live = -1.0f;
		return;
	}

	dir = dir * (force*elapsed);
	if (m_suc_type == SUC_DIR)
	{
		Vec3::add(particle->m_direction,dir,&particle->m_direction);
	}
	else
	{
		Vec3::add(particle->m_position,dir,&particle->m_position);
	}
		
}

void CCParticleSuctionAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleSuctionAffector* psa = static_cast<CCParticleSuctionAffector*>(pa);
	psa->SetCollideVisible(m_collide_visible);
	psa->SetSuctionPos(m_position);
	psa->SetDynSuction(CloneDynamicAttribute(GetDynSuction()));
	psa->SetSucType(m_suc_type);
}

void CCParticleSuctionAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	string str;

	if (!(m_dyn_suction->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_suction)->GetValue(), DEFAULT_SUCTION)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "suction_force");
		m_dyn_suction->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!CCParticleHelper::IsEqual(m_position, DEFAULT_SUCTION_POS))
	{
		str = CCParticleHelper::ToString(m_position);
		child = my_document->NewElement("suction_pos");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_collide_visible, DEFAULT_COLLIDE_VISIBLE))
	{
		str = CCParticleHelper::ToString(m_collide_visible);
		child = my_document->NewElement("suction_cv");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_suc_type != SUC_POS)
	{
		str = CCParticleHelper::ToString(m_suc_type);
		child = my_document->NewElement("suction_type");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCParticleSuctionAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "suction_force") == 0)
	{
		SetDynSuction(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

bool CCParticleSuctionAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "suction_force") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynSuction(dyn);
	}
	else if (strcmp(type, "suction_pos") == 0)
	{
		CCParticleHelper::ParseVec3(value, &m_position);
	}
	else if (strcmp(type, "suction_cv") == 0)
	{
		m_collide_visible = CCParticleHelper::ParseBool(value);
	}
	else if (strcmp(type, "suction_type") == 0)
	{
		m_suc_type = (SuctionType)CCParticleHelper::ParseInt(value);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleSuctionAffector::GetAttribute(const char* type, char* value, size_t size)
{
	string str;
	if (strcmp(type, "suction_force") == 0)
	{
		GetDynAttribute(m_dyn_suction, str);
	}
	else if (strcmp(type, "suction_pos") == 0)
	{
		str = CCParticleHelper::ToString(m_position);
	}
	else if (strcmp(type, "suction_cv") == 0)
	{
		str = CCParticleHelper::ToString(m_collide_visible);
	}
	else if (strcmp(type, "suction_type") == 0)
	{
		str = CCParticleHelper::ToString((int)m_suc_type);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}

	strncpy(value, str.c_str(),size);
	return true;
}

CCParticleAffector* CCParticleSuctionAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleSuctionAffector(pt);
	return pa;
}

NS_CC_END