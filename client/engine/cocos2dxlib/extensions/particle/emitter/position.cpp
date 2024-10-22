
#include "position.h"
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"

#include "particle/particlefactory.h"
#include "particle/strconv.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <string>


NS_CC_BEGIN

const bool CCParticlePositionEmitter::DEFAULT_RANDOM = true;

CCParticlePositionEmitter::CCParticlePositionEmitter(CCParticleTechnique* pt) :
	CCParticleEmitter(pt),
	m_random(DEFAULT_RANDOM),
	m_index(0)
{
	m_emitter_type = "Position";
}

void CCParticlePositionEmitter::InitParticlePos(CCParticle* particle)
{
	if (m_position_list.empty())
	{
		return;
	}

	if (m_random)
	{
		size_t i = (size_t)( CCParticleHelper::RangeRandom(0, 1) * (m_position_list.size()));
		if (i == m_position_list.size())
		{
			i = m_position_list.size() - 1;
		}
		Vec3::add(m_relative_position,m_position_list[i],&particle->m_direction);
	}
	else
	{
		Vec3::add(m_relative_position,m_position_list[m_index],&particle->m_position);
		m_index += 1;
		if (m_index == m_position_list.size())
		{
			m_index = 0;
		}
	}
}

void CCParticlePositionEmitter::CopyAttributesTo(CCParticleEmitter* emitter)
{
	CCParticleEmitter::CopyAttributesTo(emitter);
	CCParticlePositionEmitter* positionEmitter = static_cast<CCParticlePositionEmitter*>(emitter);
	positionEmitter->m_position_list = m_position_list;
	positionEmitter->SetRandom(m_random);
}

bool CCParticlePositionEmitter::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "position") == 0)
	{
		Vec3 Out;
		CCParticleHelper::ParseVec3(value , &Out);
		AddPosition(Out);
	}
	else if (strcmp(type, "pos_random") == 0)
	{
		SetRandom(CCParticleHelper::ParseBool(value));
	}
	else if (strcmp(type, "position_list") == 0)
	{
		m_position_list.clear();
		std::vector<std::string> arr = StrUtil::split(value, "|");
		for (size_t i = 0; i < arr.size(); ++i)
		{
			StrUtil::trim(arr[i]);
			if (!arr[i].empty())
			{
				Vec3 Out;
				CCParticleHelper::ParseVec3(arr[i], &Out);
				AddPosition(Out);	
			}
		}
	}
	else
	{
		return CCParticleEmitter::SetAttribute(type, value);
	}
	return true;
}

bool CCParticlePositionEmitter::GetAttribute(const char* type, char* value, size_t size)
{
	string str = "";
	if (strcmp(type, "position_list") == 0)
	{
		size_t len = m_position_list.size();
		for (size_t i = 0; i < len; ++i)
		{
			str += CCParticleHelper::ToString(m_position_list[i]) + "|";
		}
	}
	else if (strcmp(type, "pos_random") == 0)
	{
		str = StrConv::toString(m_random);
	}
	else
	{
		return CCParticleEmitter::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}

void CCParticlePositionEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleEmitter::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	string str;

	if (!CCParticleHelper::IsEqual(m_random, DEFAULT_RANDOM))
	{
		str = CCParticleHelper::ToString(m_random);
		child = my_document->NewElement("pos_random");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	size_t num = m_position_list.size();
	for (size_t i = 0; i < num; ++i)
	{
		str = CCParticleHelper::ToString(m_position_list[i]);
		child = my_document->NewElement("position");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

CCParticleEmitter* CCParticlePositionEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
{
	CCParticleEmitter* pe = new CCParticlePositionEmitter(pt);
	return pe;
}

NS_CC_END