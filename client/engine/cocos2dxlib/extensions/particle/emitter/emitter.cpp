
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"
#include "particle/dynamicattribute.h"

#include "particle/simplespline.h"
#include "particle/technique.h"
#include "particle/particlesystem.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <assert.h>
#include <vector>

NS_CC_BEGIN

const float CCParticleEmitter::DEFAULT_WIDTH = 64.0;
const float CCParticleEmitter::DEFAULT_HEIGHT = 64.0;
const float CCParticleEmitter::DEFAULT_DEPTH = 64.0;
const float CCParticleEmitter::DEFAULT_ANGLE = 20.0;
const Vec3 CCParticleEmitter::DEFAULT_POSITION (0, 0, 0);
const Vec3 CCParticleEmitter::DEFAULT_DIRECTION (1, 0, 0);
const float CCParticleEmitter::DEFAULT_VELOCITY = 30.0;
const float CCParticleEmitter::DEFAULT_TIME_LIVE = 3.0;
const float CCParticleEmitter::DEFAULT_EMISSION_RATE = 10.0;
const bool CCParticleEmitter::DEFAULT_FORCE_EMIT = false;
const bool CCParticleEmitter::DEFAULT_PARTICLES_LIVE_FOREVER = false;
const Color4F CCParticleEmitter::DEFAULT_COLOR  (1, 1, 1, 1);
const bool CCParticleEmitter::DEFAULT_CYCLE = true;




CCParticleEmitter::CCParticleEmitter(CCParticleTechnique* pt) :
CCParticle(),
	m_technique(pt),
	m_relative_position(DEFAULT_POSITION),
	m_particle_direction(DEFAULT_DIRECTION),
	m_emission_rate(DEFAULT_EMISSION_RATE),
	m_start_colour(DEFAULT_COLOR),
	m_end_colour(DEFAULT_COLOR),
	m_start_time(0),
	m_cycle(DEFAULT_CYCLE),
	m_remainder(0.0),
	m_emitter_time(0.0),
	m_emitted(false),
	m_force_emit( DEFAULT_FORCE_EMIT),
	m_last_count(0),
	m_use_all_size(false),
	m_particles_live_forever(DEFAULT_PARTICLES_LIVE_FOREVER)
{
	m_up.set(0, 1, 0);
	m_type = CCParticle::Emitter;
	SetEnabled(true);

	m_dyn_plt = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_plt)->SetValue(DEFAULT_TIME_LIVE);

	m_dyn_velocity = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_velocity)->SetValue(DEFAULT_VELOCITY);

	m_dyn_angle = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_angle)->SetValue(DEFAULT_ANGLE);

	m_dyn_width = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_width)->SetValue(DEFAULT_WIDTH);

	m_dyn_height = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_height)->SetValue(DEFAULT_HEIGHT);

	m_dyn_depth = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_depth)->SetValue(DEFAULT_DEPTH);

	m_dyn_size = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_size)->SetValue(DEFAULT_WIDTH);

	m_end_time = m_technique->GetParticleSystem()->GetCycleTotalTime();
}

CCParticleEmitter::~CCParticleEmitter()
{
	if (m_dyn_plt != 0)
	{
		delete m_dyn_plt;
		m_dyn_plt = 0;
	}

	if (m_dyn_velocity != 0)
	{
		delete m_dyn_velocity;
		m_dyn_velocity = 0;
	}

	if (m_dyn_angle != 0)
	{
		delete m_dyn_angle;
		m_dyn_angle = 0;
	}

	if (m_dyn_width != 0)
	{
		delete m_dyn_width;
		m_dyn_width = 0;
	}

	if (m_dyn_height != 0)
	{
		delete m_dyn_height;
		m_dyn_height = 0;
	}

	if (m_dyn_depth != 0)
	{
		delete m_dyn_depth;
		m_dyn_depth = 0;
	}

	if (m_dyn_size != 0)
	{
		delete m_dyn_size;
		m_dyn_size = 0;
	}
}

void CCParticleEmitter::SetCycleTime(float start_time, float end_time)
{
	m_start_time = start_time;
	m_end_time = end_time;
	if (m_start_time > m_end_time)
	{
		float temp = m_start_time;
		m_start_time = m_end_time;
		m_end_time = temp;
	}
}

//获取发射粒子数目
unsigned short CCParticleEmitter::GetEmissionCount(float time_elapsed, bool is_only_update_emitter_time)
{
	m_last_count = 0;
	if (!m_enabled)
	{
		return 0;
	}

	m_emitter_time += time_elapsed;
	if (is_only_update_emitter_time)
	{
		return 0;
	}

	float total = m_technique->GetParticleSystem()->GetCycleTotalTime();
	unsigned short result_count = 0;

	if( m_force_emit )	// 强制一次性发射
	{
		if(m_emitter_time >= m_start_time)
		{
			result_count = (unsigned short)m_emission_rate;
			m_last_count = result_count;
			SetEnabled( false);
		}
	}
	else
	{
		if (total >= 0.00001f)
		{
			if (m_emitter_time >= m_start_time && m_emitter_time <= m_end_time)
			{
				m_remainder += m_emission_rate * time_elapsed;
				result_count = (unsigned short)m_remainder;
				m_remainder -= result_count;
				m_last_count = result_count;
			}

			// 根据是否循环，决定是否重置m_emitter_time
			if( m_emitter_time > total)
			{
				if (m_cycle)
				{
					m_emitter_time = 0.0;
				}
				else
				{
					SetEnabled(false);
				}
			}
		}
	}

	return result_count;
}

void CCParticleEmitter::InitParticle(CCParticle* particle)
{
	particle->m_parent_emitter = this;
	particle->m_live_forever = m_particles_live_forever;
	particle->m_left = 0.0f;
	particle->m_top = 0.0f;
	particle->m_right = 1.0f;
	particle->m_bottom = 1.0f;
	particle->m_index = -1;
	particle->m_ZOrder = m_ZOrder;
	InitParticlePos(particle);
	InitParticleDirection(particle);
	InitParticleSpeed(particle);
	InitParticleColor(particle);
	InitParticleLiveTime(particle);
	InitParticleDimensions(particle);
}

void CCParticleEmitter::InitParticlePos(CCParticle* particle)
{
	particle->m_position = m_relative_position;
}

void CCParticleEmitter::InitParticleColor(CCParticle* particle)
{
	if (!(m_start_colour.r == m_end_colour.r && m_start_colour.g == m_end_colour.g && m_start_colour.b == m_end_colour.b && m_start_colour.a == m_end_colour.a))
	{
		particle->m_color.r = m_start_colour.r + (CCParticleHelper::UnitRandom() * (m_end_colour.r - m_start_colour.r));
		particle->m_color.g = m_start_colour.g + (CCParticleHelper::UnitRandom() * (m_end_colour.g - m_start_colour.g));
		particle->m_color.b = m_start_colour.b + (CCParticleHelper::UnitRandom() * (m_end_colour.b - m_start_colour.b));
		particle->m_color.a = m_start_colour.a + (CCParticleHelper::UnitRandom() * (m_end_colour.a - m_start_colour.a));
	}
	else
	{
		particle->m_color = m_start_colour;
	}
}

void CCParticleEmitter::InitParticleSpeed(CCParticle* particle)
{
	float t =  m_technique->GetParticleSystem()->GetSinceStartTime() / m_technique->GetParticleSystem()->GetCycleTotalTime();
	int l = (int)t;
	t -= l;
	float s = AttributeCalculate(m_dyn_velocity, t, 0.0f) * m_technique->GetParticleSystem()->GetScaleSpeed();
	particle->m_direction = particle->m_direction * s;
}

void CCParticleEmitter::GenerateAngle(float& angle)
{
	float t =  m_technique->GetParticleSystem()->GetSinceStartTime() / m_technique->GetParticleSystem()->GetCycleTotalTime();
	int l = (int)t;
	t -= l;
	angle= AttributeCalculate(m_dyn_angle, t);
}

void CCParticleEmitter::InitParticleDirection(CCParticle* particle)
{
	float angle;
	GenerateAngle(angle);
	angle = CCParticleHelper::UnitRandom() * angle;
	Vec3 director = m_particle_direction;
	if (!m_technique->IsLocalSpace())
	{
		director.x = m_technique->GetParticleSystem()->getScaleX() * m_particle_direction.x;
		director.y = m_technique->GetParticleSystem()->getScaleY() * m_particle_direction.y;
		director.z = m_technique->GetParticleSystem()->GetScaleZ() * m_particle_direction.z;
	}
	//weizhishu
	if (angle != 0.0f)
	{
		CCParticleHelper::RandomDeviant(angle, &m_up, &particle->m_direction, &director);
	}
	else
	{
		// Constant angle
		particle->m_direction = director;
	}
}

void CCParticleEmitter::InitParticleLiveTime(CCParticle* particle)
{
	float t =  m_technique->GetParticleSystem()->GetSinceStartTime() / m_technique->GetParticleSystem()->GetCycleTotalTime();
	int l = (int)t;
	t -= l;
	particle->m_total_live = AttributeCalculate(m_dyn_plt, t, DEFAULT_TIME_LIVE);
	particle->m_time_live = particle->m_total_live;
}

void CCParticleEmitter::InitParticleDimensions(CCParticle* particle)
{
	float t = m_technique->GetParticleSystem()->GetSinceStartTime() / m_technique->GetParticleSystem()->GetCycleTotalTime();
	int l = (int)t;
	t -= l;

	if (m_use_all_size)
	{
		if (m_dyn_size)
		{
			particle->m_width = AttributeCalculate(m_dyn_size, t);
			particle->m_height = particle->m_width;
			particle->m_depth = particle->m_width;
		}
		else
		{
			particle->m_width = m_technique->GetDefaultWidth();
			particle->m_height = m_technique->GetDefaultHeight();
			particle->m_depth = m_technique->GetDefaultDepth();
		}
	}
	else
	{
		if (m_dyn_width)
		{
			particle->m_width = AttributeCalculate(m_dyn_width, t);
		}
		else
		{
			particle->m_width = m_technique->GetDefaultWidth();
		}

		if (m_dyn_height)
		{
			particle->m_height = AttributeCalculate(m_dyn_height, t);
		}
		else
		{
			particle->m_height = m_technique->GetDefaultHeight();
		}

		if (m_dyn_depth)
		{
			particle->m_depth = AttributeCalculate(m_dyn_depth, t);
		}
		else
		{
			particle->m_depth = m_technique->GetDefaultDepth();
		}
	}
}

void CCParticleEmitter::CopyAttributesTo(CCParticleEmitter* pe)
{
	CCParticle::CopyAttributesTo(pe);

	pe->SetEmitterType(m_emitter_type);
	pe->SetName(m_name);
	pe->m_particle_direction = m_particle_direction;
	pe->m_up = m_up;
	pe->m_relative_position = m_relative_position;
	pe->m_emission_rate = m_emission_rate;
	pe->SetColor(m_start_colour, m_end_colour);
	pe->m_start_time = m_start_time;
	pe->m_end_time = m_end_time;
	pe->m_cycle = m_cycle;
	pe->m_remainder = 0.0;
	pe->m_emitter_time = 0.0;
	pe->SetEmittedName(m_emitted_name);
	pe->SetEmitted(m_emitted);
	pe->SetUseAllSize(m_use_all_size);
	pe->m_force_emit = m_force_emit;
	pe->m_particles_live_forever = m_particles_live_forever;

	pe->SetDynAngle(CloneDynamicAttribute(GetDynAngle()));
	pe->SetDynVelocity(CloneDynamicAttribute(GetDynVelocity()));
	pe->SetDynLiveTime(CloneDynamicAttribute(GetDynLiveTime()));

	pe->SetDynWidth(CloneDynamicAttribute(GetDynWidth()));
	pe->SetDynHeight(CloneDynamicAttribute(GetDynHeight()));
	pe->SetDynDepth(CloneDynamicAttribute(GetDynDepth()));

	pe->SetDynSize(CloneDynamicAttribute(m_dyn_size));
}

void CCParticleEmitter::SetDynAngle(DynamicAttribute* dynAngle)
{
	if (m_dyn_angle != 0)
	{
		delete m_dyn_angle;
		m_dyn_angle = 0;
	}

	m_dyn_angle = dynAngle;
}

void CCParticleEmitter::SetDynWidth(DynamicAttribute* dynWidth)
{
	if (m_dyn_width != 0)
	{
		delete m_dyn_width;
		m_dyn_width = 0;
	}
	m_dyn_width = dynWidth;
}

void CCParticleEmitter::SetDynHeight(DynamicAttribute* dynHeight)
{
	if (m_dyn_height != 0)
	{
		delete m_dyn_height;
		m_dyn_height = 0;
	}

	m_dyn_height = dynHeight;
}

void CCParticleEmitter::SetDynDepth(DynamicAttribute* dynDepth)
{
	if (m_dyn_depth != 0)
	{
		delete m_dyn_depth;
		m_dyn_depth = 0;
	}

	m_dyn_depth = dynDepth;
}

void CCParticleEmitter::SetDynVelocity(DynamicAttribute* dynVelocity)
{
	if (m_dyn_velocity != 0)
	{
		delete m_dyn_velocity;
		m_dyn_velocity = 0;
	}

	m_dyn_velocity = dynVelocity;
}

void CCParticleEmitter::SetDynSize(DynamicAttribute* dyn_size)
{
	if (m_dyn_size != 0)
	{
		delete m_dyn_size;
		m_dyn_size = 0;
	}

	m_dyn_size = dyn_size;
}

void CCParticleEmitter::SetDynLiveTime(DynamicAttribute* dynLiveTime)
{
	if (m_dyn_plt != 0)
	{
		delete m_dyn_plt;
		m_dyn_plt = 0;
	}

	m_dyn_plt = dynLiveTime;
}

void CCParticleEmitter::ResetData()
{
	m_enabled = true;
	m_last_count = 0;
	m_emitter_time = 0.0f;
	m_remainder = 0.0f;
}

bool CCParticleEmitter:: SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "live_time") == 0)
	{
		SetDynLiveTime(dyn);
	}
	else if (strcmp(type, "angle") == 0)
	{
		SetDynAngle(dyn);
	}
	else if (strcmp(type, "width") == 0)
	{
		SetDynWidth(dyn);
	}
	else if (strcmp(type, "height") == 0)
	{
		SetDynHeight(dyn);
	}
	else if (strcmp(type, "depth") == 0)
	{
		SetDynDepth(dyn);
	}
	else if (strcmp(type, "velocity") == 0)
	{
		SetDynVelocity(dyn);
	}
	else if (strcmp(type, "all_xyz") == 0)
	{
		SetDynSize(dyn); 
	}
	else
	{
		return false;
	}
	return true;
}

bool CCParticleEmitter::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "name") == 0)
	{
		SetName(value);
	}
	else if (strcmp(type, "direction") == 0)
	{
		Vec3 dir (1,0,0);
		CCParticleHelper::ParseVec3(value, &dir);
		m_particle_direction = dir.getNormalized();
		CCParticleHelper::Perpendicular(&m_up, &m_particle_direction);
	}
	else if (strcmp(type, "position") == 0)
	{
		Vec3 pos (1,0,0);
		CCParticleHelper::ParseVec3(value, &pos);
		SetRelativePosition(pos);
	}
	else if (strcmp(type, "cycle_time") == 0)
	{
		Vec2 time(0,1);
		CCParticleHelper::ParseVec2(value, &time);
		SetCycleTime(time.x, time.y);
	}
	else if (strcmp(type, "emitter_start_time") == 0)
	{
		m_start_time = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "emitter_end_time") == 0)
	{
		m_end_time = CCParticleHelper::ParseFloat(value);
	}
	else if (strcmp(type, "emission_rate") == 0)
	{
		SetEmissionRate(CCParticleHelper::ParseFloat(value));
	}
	else if( strcmp(type, "force_emit") == 0)
	{
		m_force_emit = CCParticleHelper::ParseBool( value);
	}
	else if( strcmp( type, "live_forever") == 0)
	{
		m_particles_live_forever = CCParticleHelper::ParseBool( value);
	}
	else if (strcmp(type, "emitted_name") == 0)
	{
		SetEmittedName(value);
	}
	else if (strcmp(type, "is_cycle") == 0)
	{
		m_cycle = CCParticleHelper::ParseBool(value);
		if( m_cycle )
		{
			SetEnabled( true);
		}
	}
	else if (strcmp(type, "start_color") == 0)
	{
		CCParticleHelper::ParseColor(value, &m_start_colour);
	}
	else if (strcmp(type, "end_color") == 0)
	{
		CCParticleHelper::ParseColor(value, &m_end_colour);
	}
	else if (strcmp(type, "emitter_color") == 0)
	{
		CCParticleHelper::ParseColor(value, &m_end_colour);
		m_start_colour = m_end_colour;
	}
	else if (strcmp(type, "use_all_size") == 0)
	{
		m_use_all_size = CCParticleHelper::ParseBool(value);
	}
	else if (strcmp(type, "live_time") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynLiveTime(dyn);
	}
	else if (strcmp(type, "angle") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynAngle(dyn);
	}
	else if (strcmp(type, "width") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynWidth(dyn);
	}
	else if (strcmp(type, "height") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynHeight(dyn);
	}
	else if (strcmp(type, "depth") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynDepth(dyn);
	}
	else if (strcmp(type, "all_xyz") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynSize(dyn);
	}
	else if (strcmp(type, "velocity") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynVelocity(dyn);
	}
	else if (strcmp(type, "z_order") == 0)
	{
		m_ZOrder = CCParticleHelper::ParseInt(value);
	}
	else 
	{
		return false;
	}

	return true;
}

bool CCParticleEmitter::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;

	if (strcmp(type, "name") == 0)
	{
		str = m_name;
	}
	else if (strcmp(type, "type") == 0)
	{
		str = m_emitter_type;
	}
	else if (strcmp(type, "direction") == 0)
	{
		str = CCParticleHelper::ToString(m_particle_direction);
	}
	else if (strcmp(type, "position") == 0)
	{
		str = CCParticleHelper::ToString(m_relative_position);
	}
	else if (strcmp(type, "cycle_time") == 0)
	{
		Vec2 time (m_start_time, m_end_time);
		str = CCParticleHelper::ToString(time);
	}
	else if (strcmp(type, "emission_rate") == 0)
	{
		str = CCParticleHelper::ToString(m_emission_rate);
	}
	else if( strcmp(type, "force_emit") == 0)
	{
		str = CCParticleHelper::ToString( m_force_emit);
	}
	else if( strcmp(type, "live_forever") == 0)
	{
		str = CCParticleHelper::ToString( m_particles_live_forever);
	}
	else if (strcmp(type, "emitted_name") == 0)
	{
		str = m_emitted_name;
	}
	else if (strcmp(type, "is_cycle") == 0)
	{
		str = CCParticleHelper::ToString(m_cycle);
	}
	else if (strcmp(type, "start_color") == 0)
	{
		str = CCParticleHelper::ToString(m_start_colour);
	}
	else if (strcmp(type, "end_color") == 0)
	{
		str = CCParticleHelper::ToString(m_end_colour);
	}
	else if (strcmp(type, "use_all_size") == 0)
	{
		str = CCParticleHelper::ToString(m_use_all_size);
	}
	else if (strcmp(type, "live_time") == 0)
	{
		GetDynAttribute(m_dyn_plt, str);
	}
	else if (strcmp(type, "angle") == 0)
	{
		GetDynAttribute(m_dyn_angle, str);
	}
	else if (strcmp(type, "width") == 0)
	{
		GetDynAttribute(m_dyn_width, str);
	}
	else if (strcmp(type, "height") == 0)
	{
		GetDynAttribute(m_dyn_height, str);
	}
	else if (strcmp(type, "depth") == 0)
	{
		GetDynAttribute(m_dyn_depth, str);
	}
	else if (strcmp(type, "velocity") == 0)
	{
		GetDynAttribute(m_dyn_velocity, str);
	}
	else if (strcmp(type, "all_xyz") == 0)
	{
		GetDynAttribute(m_dyn_size, str);
	}
	else if (strcmp(type, "z_order") == 0)
	{
		str = CCParticleHelper::ToString(m_ZOrder);
	}
	else 
	{
		return false;
	}
	strncpy(value, str.c_str(), size);
	return true;
}


void CCParticleEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	child = my_document->NewElement("name");
	content = my_document->NewText(m_name.c_str());
	element->LinkEndChild(child);
	child->LinkEndChild(content);

	if (!CCParticleHelper::IsEqual(m_particle_direction, DEFAULT_DIRECTION))
	{
		str = CCParticleHelper::ToString(m_particle_direction);;
		child = my_document->NewElement("direction");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_relative_position, DEFAULT_POSITION))
	{
		str = CCParticleHelper::ToString(m_relative_position);
		child = my_document->NewElement("position");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	Vec2 temp (m_start_time, m_end_time);
	Vec2 cycle_time (0, m_technique->GetParticleSystem()->GetCycleTotalTime());
	if (!CCParticleHelper::IsEqual(temp, cycle_time))
	{
		str = CCParticleHelper::ToString(temp);
		child = my_document->NewElement("cycle_time");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_emission_rate, DEFAULT_EMISSION_RATE))
	{
		str = CCParticleHelper::ToString(m_emission_rate);
		child = my_document->NewElement("emission_rate");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if( !CCParticleHelper::IsEqual( m_force_emit, DEFAULT_FORCE_EMIT))
	{
		str = CCParticleHelper::ToString( m_force_emit);
		child = my_document->NewElement("force_emit");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if( !CCParticleHelper::IsEqual( m_particles_live_forever, DEFAULT_PARTICLES_LIVE_FOREVER))
	{
		str = CCParticleHelper::ToString( m_particles_live_forever);
		child = my_document->NewElement("live_forever");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (m_emitted_name != "")
	{
		child = my_document->NewElement("emitted_name");
		content = my_document->NewText(m_emitted_name.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_start_colour, DEFAULT_COLOR))
	{
		str = CCParticleHelper::ToString(m_start_colour);
		child = my_document->NewElement("start_color");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_end_colour, DEFAULT_COLOR))
	{
		str = CCParticleHelper::ToString(m_end_colour);
		child = my_document->NewElement("end_color");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_cycle, DEFAULT_CYCLE))
	{
		str = CCParticleHelper::ToString(m_cycle);
		child = my_document->NewElement("is_cycle");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!(m_dyn_plt->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_plt)->GetValue(), DEFAULT_TIME_LIVE)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "live_time");
		m_dyn_plt->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (!(m_dyn_angle->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_angle)->GetValue(), DEFAULT_ANGLE)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "angle");
		m_dyn_angle->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}

	if (m_use_all_size)
	{
		str = CCParticleHelper::ToString(m_use_all_size);
		child = my_document->NewElement("use_all_size");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);

		if (!(m_dyn_size->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_size)->GetValue(), DEFAULT_WIDTH)))
		{
			child = my_document->NewElement("dyn");
			child->SetAttribute("type", "all_xyz");
			m_dyn_size->SaveScript(my_document, child);
			element->LinkEndChild(child);
		}
	}
	else
	{
		if (!(m_dyn_width->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_width)->GetValue(), DEFAULT_WIDTH)))
		{
			child = my_document->NewElement("dyn");
			child->SetAttribute("type", "width");
			m_dyn_width->SaveScript(my_document, child);
			element->LinkEndChild(child);
		}

		if (!(m_dyn_height->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_height)->GetValue(), DEFAULT_HEIGHT)))
		{
			child = my_document->NewElement("dyn");
			child->SetAttribute("type", "height");
			m_dyn_height->SaveScript(my_document, child);
			element->LinkEndChild(child);
		}

		if (!(m_dyn_depth->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_depth)->GetValue(), DEFAULT_HEIGHT)))
		{
			child = my_document->NewElement("dyn");
			child->SetAttribute("type", "depth");
			m_dyn_depth->SaveScript(my_document, child);
			element->LinkEndChild(child);
		}
	}

	if (m_ZOrder != 0)
	{
		str = CCParticleHelper::ToString(m_ZOrder);
		child = my_document->NewElement("z_order");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!(m_dyn_velocity->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_velocity)->GetValue(), DEFAULT_VELOCITY)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "velocity");
		m_dyn_velocity->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}
}

NS_CC_END

