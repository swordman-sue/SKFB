
#include "scale.h"
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

const float CCParticleScaleAffector::DEFAULT_X_SCALE = 64.0f;
const float CCParticleScaleAffector::DEFAULT_Y_SCALE = 64.0f;
const float CCParticleScaleAffector::DEFAULT_Z_SCALE = 64.0f;
const float CCParticleScaleAffector::DEFAULT_XYZ_SCALE = 64.0f;
const bool  CCParticleScaleAffector::DEFAULT_FIXED = false;

CCParticleScaleAffector::CCParticleScaleAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_dyn_set_x(false),
	m_dyn_set_y(false),
	m_dyn_set_z(false),
	m_dyn_set_xyz(true),
	m_fixed(DEFAULT_FIXED)
{
	m_type = "Scale";

	m_dyn_scale_x = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_scale_x)->SetValue(DEFAULT_X_SCALE);

	m_dyn_scale_y = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_scale_y)->SetValue(DEFAULT_Y_SCALE);

	m_dyn_scale_z = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_scale_z)->SetValue(DEFAULT_Z_SCALE);

	m_dyn_scale_xyz = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_scale_xyz)->SetValue(DEFAULT_XYZ_SCALE);
}

CCParticleScaleAffector::~CCParticleScaleAffector()
{
	if (m_dyn_scale_x != 0)
	{
		delete m_dyn_scale_x;
		m_dyn_scale_x = 0;
	}

	if (m_dyn_scale_y != 0)
	{
		delete m_dyn_scale_y;
		m_dyn_scale_y = 0;
	}

	if (m_dyn_scale_z != 0)
	{
		delete m_dyn_scale_z;
		m_dyn_scale_z = 0;
	}

	if (m_dyn_scale_xyz != 0)
	{
		delete m_dyn_scale_xyz;
		m_dyn_scale_xyz = 0;
	}
}

inline void CCParticleScaleAffector::SetDynScaleX(DynamicAttribute* dyn_x)
{
	if (m_dyn_scale_x != 0)
	{
		delete m_dyn_scale_x;
		m_dyn_scale_x = 0;
	}
	m_dyn_scale_x = dyn_x;
	m_dyn_set_x = true;
}

inline void CCParticleScaleAffector::SetDynScaleY(DynamicAttribute* dyn_y)
{
	if (m_dyn_scale_y != 0)
	{
		delete m_dyn_scale_y;
		m_dyn_scale_y = 0;
	}
	m_dyn_scale_y = dyn_y;
	m_dyn_set_y = true;
}

inline void CCParticleScaleAffector::SetDynScaleZ(DynamicAttribute* dyn_z)
{
	if (m_dyn_scale_z != 0)
	{
		delete m_dyn_scale_z;
		m_dyn_scale_z = 0;
	}
	m_dyn_scale_z = dyn_z;
	m_dyn_set_z = true;
}

inline void CCParticleScaleAffector::SetDynScaleXYZ(DynamicAttribute* dyn_xyz)
{
	if (dyn_xyz != 0)
	{
		delete m_dyn_scale_xyz;
		m_dyn_scale_xyz = 0;
	}
	m_dyn_scale_xyz = dyn_xyz;
}

void CCParticleScaleAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleScaleAffector* psa = static_cast<CCParticleScaleAffector*>(pa);
	psa->SetXYZScale(m_dyn_set_xyz);
	psa->SetFixed(m_fixed);

	if (m_dyn_set_x)
	{
		psa->SetDynScaleX(CloneDynamicAttribute(GetDynScaleX()));
	}

	if (m_dyn_set_y)
	{
		psa->SetDynScaleY(CloneDynamicAttribute(GetDynScaleY()));
	}

	if (m_dyn_set_z)
	{
		psa->SetDynScaleZ(CloneDynamicAttribute(GetDynScaleZ()));
	}

	if (m_dyn_set_xyz)
	{
		psa->SetDynScaleXYZ(CloneDynamicAttribute(GetDynScaleXYZ()));
	}
}

bool CCParticleScaleAffector::IsFitParticle(CCParticle* particle)
{
	if(CCParticleAffector::IsFitParticle(particle))
	{
		if (particle->m_type == CCParticle::Visual)
		{
			return true;
		}
	}
	return false;
}

void CCParticleScaleAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float ds = 0;
	float dimension = 0;
	if (particle->m_time_live < elapsed)
	{
		elapsed = particle->m_time_live;
	}

	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;

	if (m_dyn_set_xyz)
	{
		if (!m_fixed)
		{
			ds = AttributeCalculate(m_dyn_scale_xyz, percent) * elapsed;
			particle->m_width = particle->m_width + ds;
			particle->m_height = particle->m_height + ds;
			particle->m_depth = particle->m_depth + ds;
		}
		else
		{
			dimension = AttributeCalculate(m_dyn_scale_xyz, percent);
			particle->m_width = dimension;
			particle->m_height = dimension;
			particle->m_depth = dimension;
		}
	}
	else
	{
		if (m_dyn_set_x)
		{
			if (!m_fixed)
			{
				ds = AttributeCalculate(m_dyn_scale_x, percent) * elapsed;
				particle->m_width = particle->m_width + ds;
			}
			else
			{
				particle->m_width = AttributeCalculate(m_dyn_scale_x, percent);
			}
		}
		if (m_dyn_set_y)
		{
			if (!m_fixed)
			{
				ds = AttributeCalculate(m_dyn_scale_y, percent) * elapsed;
				particle->m_height = particle->m_height + ds;
			}
			else
			{
				particle->m_height = AttributeCalculate(m_dyn_scale_y, percent);
			}
		}
		if (m_dyn_set_z)
		{
			if (!m_fixed)
			{
				ds = AttributeCalculate(m_dyn_scale_z, percent) * elapsed;
				particle->m_depth = particle->m_depth + ds;
			}
			else
			{
				particle->m_depth = AttributeCalculate(m_dyn_scale_z, percent);
			}
		}
	}

	if (particle->m_width < 0)
	{
		particle->m_width = 0;
	}
	if (particle->m_height < 0)
	{
		particle->m_height = 0;
	}
	if (particle->m_depth < 0)
	{
		particle->m_depth = 0;
	}
}


bool CCParticleScaleAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "scale_x") == 0)
	{
		SetDynScaleX(dyn);
	}
	else if (strcmp(type, "scale_y") == 0)
	{
		SetDynScaleY(dyn);
	}
	else if (strcmp(type, "scale_z") == 0)
	{
		SetDynScaleZ(dyn);
	}
	else if (strcmp(type, "scale_xyz") == 0)
	{
		SetDynScaleXYZ(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

bool CCParticleScaleAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "scale_x") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynScaleX(dyn);
	}
	else if (strcmp(type, "scale_y") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynScaleY(dyn);
	}
	else if (strcmp(type, "scale_z") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynScaleZ(dyn);
	}
	else if (strcmp(type, "scale_xyz") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynScaleXYZ(dyn);
	}
	else if (strcmp(type, "is_all_scale") == 0)
	{
		SetXYZScale(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(type, "scale_fixed") == 0)
	{
		m_fixed = CCParticleHelper::ParseBool(value);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleScaleAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "scale_x") == 0)
	{
		GetDynAttribute(m_dyn_scale_x, str);
	}
	else if (strcmp(type, "scale_y") == 0)
	{
		GetDynAttribute(m_dyn_scale_y, str);
	}
	else if (strcmp(type, "scale_z") == 0)
	{
		GetDynAttribute(m_dyn_scale_z, str);
	}
	else if (strcmp(type, "scale_xyz") == 0)
	{
		GetDynAttribute(m_dyn_scale_xyz, str);
	}
	else if (strcmp(type, "is_all_scale") == 0)
	{
		str = CCParticleHelper::ToString(m_dyn_set_xyz);
	}
	else if (strcmp(type, "scale_fixed") == 0)
	{
		str = CCParticleHelper::ToString(m_fixed);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}

	strncpy(value, str.c_str(),size);
	return true;
}

void CCParticleScaleAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!m_dyn_set_xyz)
	{
		str = CCParticleHelper::ToString(m_dyn_set_xyz);
		child = my_document->NewElement("is_all_scale");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_fixed != DEFAULT_FIXED)
	{
		str = CCParticleHelper::ToString(m_fixed);
		child = my_document->NewElement("scale_fixed");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_dyn_set_xyz)
	{
		//判断XYZ是否为固定值
		if (!(m_dyn_scale_xyz->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_scale_xyz)->GetValue(), DEFAULT_XYZ_SCALE)))
		{
			child = my_document->NewElement("dyn");
			child->SetAttribute("type", "scale_xyz");
			m_dyn_scale_xyz->SaveScript(my_document, child);
			element->LinkEndChild(child);
		}
	}
	else
	{
		if (m_dyn_set_x)
		{
			//判断X是否为固定值
			if (!(m_dyn_scale_x->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_scale_x)->GetValue(), DEFAULT_X_SCALE)))
			{
				child = my_document->NewElement("dyn");
				child->SetAttribute("type", "scale_x");
				m_dyn_scale_x->SaveScript(my_document, child);
				element->LinkEndChild(child);
			}
		}
		if (m_dyn_set_y)
		{
			//判断Y是否为固定值
			if (!(m_dyn_scale_y->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_scale_y)->GetValue(), DEFAULT_Y_SCALE)))
			{
				child = my_document->NewElement("dyn");
				child->SetAttribute("type", "scale_y");
				m_dyn_scale_y->SaveScript(my_document, child);
				element->LinkEndChild(child);
			}
		}
		if (m_dyn_set_z)
		{
			//判断Z是否为固定值
			if (!(m_dyn_scale_z->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_scale_z)->GetValue(), DEFAULT_Z_SCALE)))
			{
				child = my_document->NewElement("dyn");
				child->SetAttribute("type", "scale_z");
				m_dyn_scale_z->SaveScript(my_document, child);
				element->LinkEndChild(child);
			}
		}
	}
}

CCParticleAffector* CCParticleScaleAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleScaleAffector(pt);
	return pa;
}

NS_CC_END