
#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "particle/affector/affector.h"
#include "particle/emitter/emitter.h"

#include "particle/simplespline.h"
#include "particle/technique.h"
#include "particle/particlesystem.h"

#include "base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "tinyxml2/tinyxml2.h"
#include "math/CCmath.h"

#include <vector>
#include <algorithm>

NS_CC_BEGIN

	CCParticleAffector::CCParticleAffector(CCParticleTechnique* pt) :
m_parent(pt),
	m_affect_time(0.0f),
	m_start_time(0.0f),
	m_end_time(1.0f),
	m_enabled(true),
	m_first_state(true)
{
	m_affector_weight = 0;
}

void CCParticleAffector::SetParent(CCParticleTechnique* pt)
{
	m_parent = pt;
}

CCParticleTechnique* CCParticleAffector::GetParent()
{
	return m_parent;
}

void CCParticleAffector::AddEmitterToExclude(const std::string& name)
{
	std::list<std::string>::iterator it;
	it = std::find(m_exclude_emitter.begin(), m_exclude_emitter.end(), name);
	if (it == m_exclude_emitter.end())
	{
		m_exclude_emitter.push_back(name);
	}
}

void CCParticleAffector::RemoveEmitterToExclude(const std::string& name)
{
	std::list<std::string>::iterator it;
	it = std::find(m_exclude_emitter.begin(), m_exclude_emitter.end(), name);
	if (it != m_exclude_emitter.end())
	{
		m_exclude_emitter.erase(it);
	}
}

bool CCParticleAffector::IsFitParticle(CCParticle* particle)
{
	if (!m_enabled)
	{
		return false;
	}

	if (!m_exclude_emitter.empty() && particle->m_parent_emitter)
	{
		// Return if the emitter which emits this particle is part of the list
		std::string name = particle->m_parent_emitter->GetName();
		std::list<std::string>::iterator it;
		it = std::find(m_exclude_emitter.begin(), m_exclude_emitter.end(), name);
		if (it != m_exclude_emitter.end())
		{
			return false;
		}
	}

	if (particle->m_time_live <= 0.0f)
	{
		return false;
	}

	return true;
}

void CCParticleAffector::ProcessParticles(float elapsed)
{
	if (m_parent == 0 || !m_enabled)
	{
		return;
	}

	float total = m_parent->GetParticleSystem()->GetCycleTotalTime();
	m_affect_time += elapsed;
	float findex = m_affect_time / total;
	int index = (int)findex;
	m_affect_time -= index * total;

	float start = m_start_time * total;
	float end = m_end_time * total;
	if (m_affect_time < start || m_affect_time > end)
	{
		return;
	}

	PreAffector(elapsed);

	CCParticleTechnique::ParticleList* activeList = m_parent->GetActiveList();
	for (CCParticleTechnique::ParticleList::iterator it = activeList->begin(); it != activeList->end(); ++it)
	{
		CCParticle* particle = *it;
		if (m_first_state && particle->m_time_live == particle->m_total_live)
		{
			continue;
		}

		if (IsFitParticle(particle))
		{
			EffectParticle(particle, elapsed);
		}
	}
}

void CCParticleAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	pa->SetName(m_name);
	pa->SetParent(m_parent);
	pa->SetType(m_type);
	pa->SetStartTime(m_start_time);
	pa->SetEndTime(m_end_time);
	pa->SetEmittersToExclude(m_exclude_emitter);
	pa->SetEnable(m_enabled);
}

void CCParticleAffector::ResetData()
{
	m_affect_time = 0.0f;
}

bool CCParticleAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	return false;
}

bool CCParticleAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "name") == 0)
	{
		m_name = value;
	}
	else if (strcmp(type, "exclude_emitter") == 0)
	{
		m_exclude_emitter.push_back(value);
	}
	else if (strcmp(type, "affect_start") == 0)
	{
		SetStartTime(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(type, "affect_end") == 0)
	{
		SetEndTime(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(type, "frist_state") == 0)
	{
		SetFristState(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(type, "affect_enable") == 0)
	{
		m_enabled = CCParticleHelper::ParseBool(value);
	}
	else if (strcmp(type, "exclude_list") == 0)
	{
		m_exclude_emitter.clear();
		std::vector<std::string> arr;
		CCParticleHelper::Split(arr, value, "|");
		for (size_t i = 0; i < arr.size(); ++i)
		{
			CCParticleHelper::Trim(arr[i]);
			if (!arr[i].empty())
			{
				m_exclude_emitter.push_back(arr[i]);
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool CCParticleAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str = "";
	if (strcmp(type, "name") == 0)
	{
		str = m_name;
	}
	else if (strcmp(type, "type") == 0)
	{
		str = m_type;
	}
	else if (strcmp(type, "affect_start") == 0)
	{
		str = CCParticleHelper::ToString(m_start_time);
	}
	else if (strcmp(type, "affect_end") == 0)
	{
		str = CCParticleHelper::ToString(m_end_time);
	}
	else if (strcmp(type, "frist_state") == 0)
	{
		str = CCParticleHelper::ToString(m_first_state);
	}
	else if (strcmp(type, "affect_enable") == 0)
	{
		str = CCParticleHelper::ToString(m_enabled);
	}
	else if (strcmp(type, "exclude_list") == 0)
	{
		std::list<std::string>::iterator it;
		std::list<std::string>::iterator ie = m_exclude_emitter.end();
		for (it = m_exclude_emitter.begin(); it != ie; ++it)
		{
			str += (*it) + "|";
		}
	}
	else
	{
		return false;
	}

	strncpy(value, str.c_str(), size);
	return true;
}

void CCParticleAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	child = my_document->NewElement("name");
	content = my_document->NewText(m_name.c_str());
	element->LinkEndChild(child);
	child->LinkEndChild(content);

	if (!CCParticleHelper::IsEqual(m_start_time, 0.0f))
	{
		str = CCParticleHelper::ToString(m_start_time);
		child = my_document->NewElement("affect_start");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_end_time, 1.0f))
	{
		str = CCParticleHelper::ToString(m_end_time);
		child = my_document->NewElement("affect_end");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	std::list<std::string>::iterator it;
	std::list<std::string>::iterator ie = m_exclude_emitter.end();
	for (it = m_exclude_emitter.begin(); it != ie; ++it)
	{
		child = my_document->NewElement("exclude_emitter");
		content = my_document->NewText((*it).c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

NS_CC_END