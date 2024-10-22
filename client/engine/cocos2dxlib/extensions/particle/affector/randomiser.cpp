
#include "randomiser.h"
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

const Vec3 CCParticleRandomiserAffector::DEFAULT_DEVIATION (1, 1, 1);
const bool CCParticleRandomiserAffector::DEFAULT_RANDOM = true;

CCParticleRandomiserAffector::CCParticleRandomiserAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_random_direction(DEFAULT_RANDOM),
	m_max_deviation(DEFAULT_DEVIATION)
{
	m_type = "Randomiser";
}

void CCParticleRandomiserAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleRandomiserAffector* pra = static_cast<CCParticleRandomiserAffector*>(pa);
	pra->SetRandomDirection(m_random_direction);
	pra->SetMaxDeviation(m_max_deviation);
}

void CCParticleRandomiserAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float x = CCParticleHelper::RangeRandom(-m_max_deviation.x, m_max_deviation.x) * m_parent->GetDefaultWidth();
	float y = CCParticleHelper::RangeRandom(-m_max_deviation.y, m_max_deviation.y) * m_parent->GetDefaultHeight();
/*	float z = CCParticleHelper::RangeRandom(-m_max_deviation.z, m_max_deviation.z) * m_parent->GetDefaultDepth();*/
	float z = 0;		//目前粒子系统暂支持2d，z轴不随机
	Vec3 vec (x * elapsed * 100, y * elapsed * 100, z * elapsed * 100);
	if (m_random_direction)
	{
		Vec3::add(particle->m_direction, vec,&particle->m_direction);
	}
	else
	{
		Vec3::add(particle->m_position, vec,&particle->m_position);
	}
}


	bool CCParticleRandomiserAffector::SetAttribute(const char* type, const char* value)
	{
		if (strcmp(type, "is_random_direction") == 0)
		{
			m_random_direction = CCParticleHelper::ParseBool(value);
		}
		else if (strcmp(type, "max_deviation") == 0)
		{
			CCParticleHelper::ParseVec3(value, &m_max_deviation);
		}
		else if (strcmp(type, "max_deviation_x") == 0)
		{
			m_max_deviation.x = CCParticleHelper::ParseFloat(value);
		}
		else if (strcmp(type, "max_deviation_y") == 0)
		{
			m_max_deviation.y = CCParticleHelper::ParseFloat(value);
		}
		else if (strcmp(type, "max_deviation_z") == 0)
		{
			m_max_deviation.z = CCParticleHelper::ParseFloat(value);
		}
		else
		{
			return CCParticleAffector::SetAttribute(type, value);
		}
		return true;
	}

	bool CCParticleRandomiserAffector::GetAttribute(const char* type, char* value, size_t size)
	{
		std::string str;
		if (strcmp(type, "is_random_direction") == 0)
		{
			str = CCParticleHelper::ToString(m_random_direction);
		}
		else if (strcmp(type, "max_deviation") == 0)
		{
			str = CCParticleHelper::ToString(m_max_deviation);
		}
		else
		{
			return CCParticleAffector::GetAttribute(type, value, size);
		}
		strncpy(value, str.c_str(), size);
		return true;
	}

	void CCParticleRandomiserAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
	{
		CCParticleAffector::SaveScript(my_document, element);

		tinyxml2::XMLElement* child = 0;
		tinyxml2::XMLText* content = 0;
		std::string str;

		if (!CCParticleHelper::IsEqual(m_random_direction, DEFAULT_RANDOM))
		{
			str = CCParticleHelper::ToString(m_random_direction);
			child = my_document->NewElement("is_random_direction");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_max_deviation, DEFAULT_DEVIATION))
		{
			str = CCParticleHelper::ToString(m_max_deviation);
			child = my_document->NewElement("max_deviation");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}
	}

CCParticleAffector* CCParticleRandomiserAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleRandomiserAffector(pt);
	return pa;
}

NS_CC_END