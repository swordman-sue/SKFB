
#include "line.h"
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"

#include "particle/particlefactory.h"
#include "particle/strconv.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <vector>
#include <string>
using namespace std;

NS_CC_BEGIN

	const bool CCParticleLineEmitter::DEFAULT_RANDOM = true;
	const float CCParticleLineEmitter::DEFAULT_INC = 0.5f;

	CCParticleLineEmitter::CCParticleLineEmitter(CCParticleTechnique* pt) :
		CCParticleEmitter(pt),
		m_randomized(DEFAULT_RANDOM),
		m_increment(DEFAULT_INC)
	{
		m_start_point.set(0, 0, 0);
		m_end_point.set(0, 0, 0);
		m_line_dir.set(0, 0, 0);
		m_last_create_pos.set(0, 0, 0);
		m_line_normalize_dir.set(0, 0, 0);
		m_emitter_type = "Line";
	}

	void CCParticleLineEmitter::InitParticlePos(CCParticle* particle)
	{
		if (m_randomized)
		{
			Vec3 pos;
			Vec3 line_dir_rand;
			line_dir_rand = m_line_dir * CCParticleHelper::RangeRandom(0,1);
			Vec3::add(m_start_point, line_dir_rand,&pos);
			Vec3::add(m_relative_position,pos,&particle->m_position);
		}
		else
		{
			Vec3 increment_dir;
			Vec3 pos;
			increment_dir = m_line_normalize_dir * m_increment ;
			Vec3::add(m_last_create_pos,increment_dir,&pos);
			Vec3::add(m_relative_position, pos,&particle->m_position);
			m_last_create_pos = pos;

			//Vector3 dir = m_last_create_pos - (m_line_dir + m_start_point);
			Vec3 dir;
			Vec3::subtract(m_last_create_pos,m_end_point,&dir);

			bool need_turn_around = true;
			if( dir.length() > 1e-5f)
			{
				float d = Vec3::dot(m_line_normalize_dir, dir);
				if (d <= 0.0 )
				{
					need_turn_around = false;
				}
			}
			if( need_turn_around)
			{
				m_last_create_pos = m_start_point;
			}
		}
	}

	void CCParticleLineEmitter::CopyAttributesTo(CCParticleEmitter* emitter)
	{
		CCParticleEmitter::CopyAttributesTo(emitter);
		CCParticleLineEmitter* le = static_cast<CCParticleLineEmitter*>(emitter);
		le->SetLinePostion(m_start_point, m_line_dir);
		le->SetRandomized(m_randomized);
		le->SetIncrement(m_increment);
	}

	bool CCParticleLineEmitter::SetAttribute(const char* type, const char* value)
	{
		if (strcmp(type, "start_point") == 0)
		{
			CCParticleHelper::ParseVec3(value, &m_start_point);
			m_last_create_pos = m_start_point;
			Vec3::add(m_start_point,m_line_dir,&m_end_point);
		}
		else if (strcmp(type, "line_direction") == 0)
		{
			CCParticleHelper::ParseVec3(value, &m_line_dir);
			Vec3::add(m_start_point,m_line_dir,&m_end_point);
			m_line_normalize_dir = m_line_dir.getNormalized();			
		}
		else if (strcmp(type, "is_randomized") == 0)
		{
			m_randomized = CCParticleHelper::ParseBool(value);
		}
		else if (strcmp(type, "increment") == 0)
		{
			m_increment = CCParticleHelper::ParseFloat(value);
		}
		else
		{
			return CCParticleEmitter::SetAttribute(type, value);
		}
		return true;
	}

	bool CCParticleLineEmitter::GetAttribute(const char* type, char* value, size_t size)
	{
		string str;

		if (strcmp(type, "start_point") == 0)
		{
			str = CCParticleHelper::ToString(m_start_point);
		}
		else if (strcmp(type, "line_direction") == 0)
		{
			str = CCParticleHelper::ToString(m_line_dir);
		}
		else if (strcmp(type, "is_randomized") == 0)
		{
			str = CCParticleHelper::ToString(m_randomized);
		}
		else if (strcmp(type, "increment") == 0)
		{
			str = CCParticleHelper::ToString(m_increment);
		}
		else
		{
			return CCParticleEmitter::GetAttribute(type, value, size);
		}
		strncpy(value, str.c_str(), size);
		return true;
	}

	void CCParticleLineEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
	{
		CCParticleEmitter::SaveScript(my_document, element);

		tinyxml2::XMLElement* child = 0;
		tinyxml2::XMLText* content = 0;
		string str;

		str = CCParticleHelper::ToString(m_start_point);
		child = my_document->NewElement("start_point");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);

		str = CCParticleHelper::ToString(m_line_dir);
		child = my_document->NewElement("line_direction");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);

		if (!CCParticleHelper::IsEqual(m_randomized, DEFAULT_RANDOM))
		{

			str = CCParticleHelper::ToString(m_randomized);
			child = my_document->NewElement("is_randomized");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_increment, DEFAULT_INC))
		{
			str = CCParticleHelper::ToString(m_increment);
			child = my_document->NewElement("increment");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}
	}

	CCParticleEmitter*  CCParticleLineEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
	{
		CCParticleEmitter* pe = new CCParticleLineEmitter(pt);
		return pe;
	}

	void CCParticleLineEmitter::SetLinePostion( const Vec3& startPos, const Vec3& dir )
	{
		m_start_point = startPos;
		Vec3::add(startPos, dir,&m_end_point);
		m_line_dir = dir;
		m_last_create_pos = m_start_point;
		m_line_normalize_dir = m_line_dir.getNormalized();
		//m_line_normalize_dir = m_line_dir.normalisedCopy();
	}

NS_CC_END