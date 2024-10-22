
#include "pathfollower.h"
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

CCPathFollowerAffector::CCPathFollowerAffector(CCParticleTechnique* pt):
	CCParticleAffector(pt)
{
	m_type = "PathFollower";
	m_affector_weight = 10;
}

CCPathFollowerAffector::~CCPathFollowerAffector()
{
}

void CCPathFollowerAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	if (m_spline.GetNumPoints() > 0)
	{
		float timeLeft = particle->m_total_live - particle->m_time_live;
		float timeFractionPlusDelta = timeLeft / particle->m_total_live;
		timeFractionPlusDelta = timeFractionPlusDelta < 1.0f ? timeFractionPlusDelta : 1.0f;
		particle->m_position = m_spline.Interpolate(timeFractionPlusDelta);
	}
}

void CCPathFollowerAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCPathFollowerAffector* pathFollower = static_cast<CCPathFollowerAffector*>(pa);
	unsigned short numPoints = m_spline.GetNumPoints();
	pathFollower->m_spline.Clear();
	for (unsigned short i = 0; i < numPoints; ++i)
	{
		pathFollower->m_spline.AddPoint(m_spline.GetPoint(i));
	}
}

bool CCPathFollowerAffector::SetAttribute(const char* type, const char* value)
{
	Vec3 point (0, 0, 0);
	if (strcmp(type, "pf_pos") == 0)
	{
		CCParticleHelper::ParseVec3(value, &point);
		m_spline.AddPoint(point);
	}
	else if (strcmp(type, "pf_pos_list") == 0)
	{
		m_spline.Clear();
		std::vector<std::string> pos_list;
		CCParticleHelper::Split(pos_list, value, "|");
		for (size_t i = 0; i < pos_list.size(); ++i)
		{
			CCParticleHelper::Trim(pos_list[i]);
			if (!pos_list[i].empty())
			{
				CCParticleHelper::ParseVec3(pos_list[i], &point);
				m_spline.AddPoint(point);
			}
		}
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCPathFollowerAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str = "";
	if (strcmp(type, "pf_pos_list") == 0)
	{
		unsigned short num = m_spline.GetNumPoints();

		for (unsigned short i = 0; i < num; ++i)
		{
			str += CCParticleHelper::ToString(m_spline.GetPoint(i)) + "|";
		}
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value,str.c_str(), size);
	return true;
}

inline void CCPathFollowerAffector::AddPoint(const Vec3& point)
{
	m_spline.AddPoint(point);
}

inline void CCPathFollowerAffector::ClearPoints (void)
{
	m_spline.Clear();
}

void CCPathFollowerAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	unsigned short num = m_spline.GetNumPoints();
	for (unsigned short i = 0; i < num; ++i)
	{
		str = CCParticleHelper::ToString(m_spline.GetPoint(i));
		child = my_document->NewElement("pf_pos");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

CCParticleAffector* CCPathFollowerAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCPathFollowerAffector(pt);
	return pa;
}

NS_CC_END