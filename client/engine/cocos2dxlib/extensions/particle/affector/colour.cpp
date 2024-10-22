
#include "colour.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "particle/affector/affector.h"
#include "particle/emitter/emitter.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "particle/technique.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCmath.h"
#include "tinyxml2/tinyxml2.h"

#include <vector>

NS_CC_BEGIN

CCParticleColourAffector::CCParticleColourAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt)
{
	m_type = "Colour";
}

void CCParticleColourAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleColourAffector* pca = static_cast<CCParticleColourAffector*>(pa);
	pca->SetColourMap(m_colour_map);
}

inline void CCParticleColourAffector::AddColour(float timeFraction, const Color4F& colour)
{
	m_colour_map[timeFraction] = colour;
}

bool CCParticleColourAffector::IsFitParticle(CCParticle* particle)
{
	if(CCParticleAffector::IsFitParticle(particle))
	{
		if (!m_colour_map.empty())
		{
			return true;
		}
	}
	return false;
}


void CCParticleColourAffector::EffectParticle(CCParticle *particle, float elapsed)
{	
	float timeFraction = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;

	Iterator it;
	for (it = m_colour_map.begin(); it != m_colour_map.end(); ++it)
	{
		if (timeFraction < it->first)
		{
			if (it == m_colour_map.begin())
			{
				break;
			}
			else
			{
				--it;
				break;
			}
		}
	}
	if (it == m_colour_map.end())
	{
		--it;
	}

	Iterator it2 = it;
	it2++;
	Color4F colour (1, 1, 1, 1);
	if (it2 != m_colour_map.end())
	{
		CCParticleHelper::SubColour(&colour, &it2->second, &it->second);
		CCParticleHelper::ScaleColour(&colour, (timeFraction - it->first)/(it2->first - it->first));
		CCParticleHelper::AddColour(&colour, &colour, &it->second);
	}
	else
	{
		colour = it->second;
	}

	particle->m_color = colour;
}

bool CCParticleColourAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "time_colour") == 0)
	{
		AddColour(value);
	}
	else if (strcmp(type, "time_colour_list") == 0)
	{
		m_colour_map.clear();
		std::vector<std::string> color_list;
		CCParticleHelper::Split(color_list, value, "|");
		for (size_t i = 0; i < color_list.size(); ++i)
		{
			CCParticleHelper::Trim(color_list[i]);
			if (!color_list[i].empty())
			{
				AddColour(color_list[i]);
			}
		}
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleColourAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str = "";
	if (strcmp(type, "time_colour_list") == 0)
	{
		Iterator it;
		Iterator ie = m_colour_map.end();

		for (it = m_colour_map.begin(); it != ie; ++it)
		{
			str += CCParticleHelper::ToString(it->first) + " " + CCParticleHelper::ToString(it->second) + "|";
		}
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}

void CCParticleColourAffector::AddColour(const std::string& value)
{
	std::vector<std::string> vecparams;
	CCParticleHelper::Split(vecparams, value, "\t ", 1);
	if (vecparams.size() != 2)
	{
		return;
	}
	float time = CCParticleHelper::ParseFloat(vecparams[0]);
	Color4F color(1, 1, 1, 1);
	CCParticleHelper::ParseColor(vecparams[1], &color);
	AddColour(time, color);
}


void CCParticleColourAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	Iterator it;
	Iterator ie = m_colour_map.end();
	for (it = m_colour_map.begin(); it != ie; ++it)
	{
		str = CCParticleHelper::ToString(it->first);
		str += " ";
		str += CCParticleHelper::ToString(it->second);
		child = my_document->NewElement("time_colour");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

CCParticleAffector* CCParticleColourAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleColourAffector(pt);
	return pa;
}

NS_CC_END