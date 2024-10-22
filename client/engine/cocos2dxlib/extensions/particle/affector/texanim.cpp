
#include "texanim.h"
#include "affector.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"
#include "particle/emitter/emitter.h"

#include "particle/simplespline.h"
#include "particle/particlefactory.h"
#include "particle/technique.h"

#include "base/ccTypes.h"
#include "cocos/2d//CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <assert.h>

NS_CC_BEGIN

const CCParticleTexAnimAffector::TextureAnimationType CCParticleTexAnimAffector::DEFAULT_ANIM_TYPE = CCParticleTexAnimAffector::TAT_NORMAL;
const float CCParticleTexAnimAffector::DEFAULT_TIME_STEP = 0.2f;
const bool CCParticleTexAnimAffector::DEFAULT_FRAME_RANDOM = false;
const size_t CCParticleTexAnimAffector::DEFAULT_START_FRAME = 0;

CCParticleTexAnimAffector::CCParticleTexAnimAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_time_step(DEFAULT_TIME_STEP),
	m_anim_type(DEFAULT_ANIM_TYPE),
	m_row(1),
	m_col(1),
	m_num(1), 
	m_is_random_start(DEFAULT_FRAME_RANDOM), 
	m_start_frame(DEFAULT_START_FRAME), 
	m_random_start_frame(0),
	m_play_order(true)
{
	m_type = "TextureAnimator";
	UpdateTexcoords();
}

bool CCParticleTexAnimAffector::IsFitParticle(CCParticle* particle)
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

void CCParticleTexAnimAffector::InitParticle( CCParticle* particle )
{
	particle->m_start_frame = m_is_random_start ? (int)CCParticleHelper::RangeRandom( 0, m_num - 1e-6f) : m_start_frame;
	EffectParticle(particle,0);
}

void CCParticleTexAnimAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	size_t index = size_t((particle->m_total_live - particle->m_time_live) / m_time_step);

	if( m_anim_type == TAT_NORMAL)
	{
		if (index >= m_num)
		{
			index = m_num - 1;
		}
	}

	//Ë³Ðò²¥·Å
	if (m_play_order)
	{
		index = (particle->m_start_frame + index) % m_num;
	}
	else //µ¹Ðð²¥·Å
	{
		index = (particle->m_start_frame - index + m_num) % m_num;
	}

	FloatRect& r = m_texture_coords[index];
	particle->m_left = r.left;
	particle->m_right = r.right;
	particle->m_top = r.top;
	particle->m_bottom = r.bottom;
}

void CCParticleTexAnimAffector::UpdateTexcoords()
{
	m_num = m_row * m_col;
	if (m_num == 0)
	{
		m_num = 1;
	}

	m_texture_coords.clear();
	m_texture_coords.resize(m_row * m_col);

	unsigned int coordIndex = 0;
	for(size_t v = 0; v < m_row; ++v ) {
		float top = (float)v / (float)m_row;
		float bottom = ((float)v + 1) / (float)m_row;
		for(size_t u = 0; u < m_col; ++u ) {
			FloatRect& r = m_texture_coords[coordIndex];
			r.left = (float)u / (float)m_col;
			r.bottom = bottom;
			r.right = ((float)u + 1) / (float)m_col;
			r.top = top;
			++coordIndex;
		}
	}
}

void CCParticleTexAnimAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleTexAnimAffector* pta = static_cast<CCParticleTexAnimAffector*>(pa);
	pta->SetAnimType(m_anim_type);
	pta->SetTimeStep(m_time_step);
	pta->SetRow(m_row);
	pta->SetCol(m_col);
	pta->SetStartFrame( m_start_frame);
	pta->SetIsRandomStart( m_is_random_start);
	pta->SetNumTex(m_num);
	pta->SetPlayOrder(m_play_order);
}


void CCParticleTexAnimAffector::SetPlayOrder(bool play_order)
{
	m_play_order = play_order;
}

bool CCParticleTexAnimAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "time_step") == 0)
	{
		m_time_step = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "amin_type") == 0)
	{
		m_anim_type = TextureAnimationType(CCParticleHelper::ParseInt(value));
	}
	else if( strcmp( type, "start_frame") == 0)
	{
		m_start_frame = CCParticleHelper::ParseInt( value);
	}
	else if( strcmp( type, "random_start") == 0)
	{
		m_is_random_start = CCParticleHelper::ParseBool( value);
	}
	else if (strcmp(type, "row_num") == 0)
	{
		SetRow(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "col_num") == 0)
	{
		SetCol(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "anim_num") == 0)
	{
		SetNumTex(CCParticleHelper::ParseInt(value));
	}
	else if (strcmp(type, "play_order") == 0)
	{
		SetPlayOrder(CCParticleHelper::ParseBool(value));
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleTexAnimAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "time_step") == 0)
	{
		str = CCParticleHelper::ToString(m_time_step);
	}
	else if (strcmp(type, "amin_type") == 0)
	{
		str = CCParticleHelper::ToString((int)m_anim_type);
	}
	else if( strcmp( type, "start_frame") == 0)
	{
		str = CCParticleHelper::ToString((int)m_start_frame);
	}
	else if( strcmp( type, "random_start") == 0)
	{
		str = CCParticleHelper::ToString( m_is_random_start);
	}
	else if (strcmp(type, "row_num") == 0)
	{
		str = CCParticleHelper::ToString((int)m_row);
	}
	else if (strcmp(type, "col_num") == 0)
	{
		str = CCParticleHelper::ToString((int)m_col);
	}
	else if (strcmp(type, "anim_num") == 0)
	{
		str = CCParticleHelper::ToString((int)m_num);
	}
	else if (strcmp(type, "play_order") == 0)
	{
		str = CCParticleHelper::ToString(m_play_order);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size );
	return true;
}

void CCParticleTexAnimAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_time_step, DEFAULT_TIME_STEP))
	{
		str = CCParticleHelper::ToString(m_time_step);
		child = my_document->NewElement("time_step");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_row != 1)
	{
		str = CCParticleHelper::ToString((int)m_row);
		child = my_document->NewElement("row_num");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_col != 1)
	{
		str = CCParticleHelper::ToString((int)m_col);
		child = my_document->NewElement("col_num");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_num != m_row * m_col)
	{
		str = CCParticleHelper::ToString((int)m_num);
		child = my_document->NewElement("anim_num");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_anim_type != DEFAULT_ANIM_TYPE)
	{
		str = CCParticleHelper::ToString((int)m_anim_type);
		child = my_document->NewElement("amin_type");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!m_play_order)
	{
		str = CCParticleHelper::ToString(m_play_order);
		child = my_document->NewElement("play_order");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual((int)m_start_frame, (int)DEFAULT_START_FRAME))
	{
		str = CCParticleHelper::ToString((int)m_start_frame);
		child = my_document->NewElement("start_frame");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_is_random_start, DEFAULT_FRAME_RANDOM))
	{
		str = CCParticleHelper::ToString( m_is_random_start);
		child = my_document->NewElement("random_start");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}


CCParticleAffector* CCParticleTexAnimAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pa = new CCParticleTexAnimAffector(pt);
	return pa;
}

NS_CC_END