
#include "elasticity.h"
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

const float CCParticleElasticityAffector::DEFAULT_REVERSE_LIMIT = 32.0f;
const float CCParticleElasticityAffector::DEFAULT_DISTANCE_FACTOR = 0.0075f;
const float CCParticleElasticityAffector::DEFAULT_REVERSE_FACTOR = 0.95f;
const float CCParticleElasticityAffector::DEFAULT_START_TIME = 0.4f;
const float CCParticleElasticityAffector::DEFAULT_OFFSET_RADIUS = 48.0f;

CCParticleElasticityAffector::CCParticleElasticityAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_reverse_limit(DEFAULT_REVERSE_LIMIT),
	m_distance_factor(DEFAULT_DISTANCE_FACTOR),
	m_time_start(DEFAULT_START_TIME),
	m_reverse_factor(DEFAULT_REVERSE_FACTOR),
	m_offset_radius(DEFAULT_OFFSET_RADIUS)
{
	m_type = "Elasticity";
}

void CCParticleElasticityAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	//未到影响器开始时间直接返回
	if ((particle->m_total_live - particle->m_time_live) < m_time_start)
	{
		return;
	}

	//判断是否本地空间
	bool local = m_parent->IsLocalSpace();
	Vec3 diff (0 , 0, 0);
	if (!local)
	{
		 Vec3::subtract(m_parent->GetOutsidePos(), particle->m_position,&diff);
	}
	else
	{
		diff.x = -particle->m_position.x;
		diff.y = -particle->m_position.y;
		diff.z = -particle->m_position.z;
	}

	//随机偏移半径
	Vec3 offset (CCParticleHelper::RangeRandom(-1, 1), CCParticleHelper::RangeRandom(-1, 1), CCParticleHelper::RangeRandom(-1, 1));
	offset = offset * m_offset_radius;

	Vec3::add( diff, offset,&diff);
	float distance = diff.length();
	diff.normalize();
	float speed = m_distance_factor * distance * distance;
	float l = particle->m_direction.length();
		
	float k = Vec3::dot(particle->m_direction, diff);
	if (l > m_reverse_limit && k <= 0.0f)
	{
		particle->m_direction = particle->m_direction*m_reverse_factor;
	}
	diff = diff * (speed * elapsed);
	
	Vec3::add( particle->m_direction, diff,&particle->m_direction);
}

void CCParticleElasticityAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleElasticityAffector* pea = static_cast<CCParticleElasticityAffector*>(pa);
	pea->SetReverseLimit(m_reverse_limit);
	pea->SeDistanceFactor(m_distance_factor);
	pea->SetReverseFactor(m_reverse_factor);
	pea->SetTimeStart(m_time_start);
	pea->SetOffsetRadius(m_offset_radius);
}

void CCParticleElasticityAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_reverse_limit, DEFAULT_REVERSE_LIMIT))
	{
		str = CCParticleHelper::ToString(m_reverse_limit);
		child = my_document->NewElement("reverse_limit");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_distance_factor, DEFAULT_DISTANCE_FACTOR))
	{
		str = CCParticleHelper::ToString(m_distance_factor);
		child = my_document->NewElement("distance_factor");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_time_start, DEFAULT_START_TIME))
	{
		str = CCParticleHelper::ToString(m_time_start);
		child = my_document->NewElement("time_start");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_reverse_factor, DEFAULT_REVERSE_FACTOR))
	{
		str =CCParticleHelper::ToString(m_reverse_factor);
		child = my_document->NewElement("reverse_factor");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_offset_radius, DEFAULT_OFFSET_RADIUS))
	{
		str = CCParticleHelper::ToString(m_offset_radius);
		child = my_document->NewElement("offset_radius");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCParticleElasticityAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "reverse_limit") == 0)
	{
		m_reverse_limit = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "distance_factor") == 0)
	{
		m_distance_factor = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "time_start") == 0)
	{
		m_time_start = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "reverse_factor") == 0)
	{
		m_reverse_factor = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "offset_radius") == 0)
	{
		m_offset_radius = CCParticleHelper::ParseFloat(value);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleElasticityAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "reverse_limit") == 0)
	{
		str = CCParticleHelper::ToString(m_reverse_limit);
	}
	else if (strcmp(type, "distance_factor") == 0)
	{
		str = CCParticleHelper::ToString(m_distance_factor);
	}
	else if (strcmp(type, "time_start") == 0)
	{
		str = CCParticleHelper::ToString(m_time_start);
	}
	else if (strcmp(type, "reverse_factor") == 0)
	{
		str = CCParticleHelper::ToString(m_reverse_factor);
	}
	else if (strcmp(type, "offset_radius") == 0)
	{
		str = CCParticleHelper::ToString(m_offset_radius);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}


CCParticleAffector* CCParticleElasticityAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleElasticityAffector(pt);
	return pa;
}

NS_CC_END