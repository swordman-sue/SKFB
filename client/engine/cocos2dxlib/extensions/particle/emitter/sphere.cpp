
#include "sphere.h"
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"

#include "particle/particlefactory.h"
//#include "common/CCStrConv.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <string>

NS_CC_BEGIN

	const float CCParticleSphereEmitter::DEFAULT_RADIUS = 64.0f;
const bool CCParticleSphereEmitter::DEFAULT_AUTO =false;

CCParticleSphereEmitter::CCParticleSphereEmitter(CCParticleTechnique* pt) :
CCParticleEmitter(pt),
	m_radius(DEFAULT_RADIUS),
	m_auto_direction(DEFAULT_AUTO)
{
	m_random_vec.set(0, 0, 0);
	m_emitter_type = "Sphere";
}

void CCParticleSphereEmitter::InitParticlePos(CCParticle* particle)
{
	m_random_vec.set(CCParticleHelper::RangeRandom(-1, 1) , CCParticleHelper::RangeRandom(-1, 1), CCParticleHelper::RangeRandom(-1, 1));
	m_random_vec.normalize();	
	Vec3 radius_vec;
	radius_vec = m_random_vec * m_radius ;
	Vec3::add(m_relative_position,radius_vec,&particle->m_position);
}

void CCParticleSphereEmitter::CopyAttributesTo (CCParticleEmitter* emitter)
{
	CCParticleEmitter::CopyAttributesTo(emitter);
	CCParticleSphereEmitter* se = static_cast<CCParticleSphereEmitter*>(emitter);
	se->SetRadius(m_radius);
	se->SetAutoDirection(m_auto_direction);
}

void CCParticleSphereEmitter::InitParticleDirection(CCParticle* particle)
{
	if (m_auto_direction)
	{
		float angle;
		GenerateAngle(angle);
		CCParticleHelper::RandomDeviant(angle ,&m_up , &particle->m_direction, &m_random_vec);
	}
	else
	{
		CCParticleEmitter::InitParticleDirection(particle);
	}
}

void CCParticleSphereEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleEmitter::SaveScript(my_document, element);
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_radius, DEFAULT_RADIUS))
	{
		str = CCParticleHelper::ToString(m_radius);
		child = my_document->NewElement("sphere_radius");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_auto_direction, DEFAULT_AUTO))
	{
		str = CCParticleHelper::ToString(m_auto_direction);
		child = my_document->NewElement("sphere_aotu_dir");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}

bool CCParticleSphereEmitter::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "sphere_radius") == 0)
	{
		SetRadius(CCParticleHelper::ParseFloat(value));
	}
	else if (strcmp(type, "sphere_aotu_dir") == 0)
	{
		SetAutoDirection(CCParticleHelper::ParseBool(value));
	}
	else
	{
		return CCParticleEmitter::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleSphereEmitter::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;

	if (strcmp(type, "sphere_radius") == 0)
	{
		str = CCParticleHelper::ToString(m_radius);
	}
	else if (strcmp(type, "sphere_aotu_dir") == 0)
	{
		str = CCParticleHelper::ToString(m_auto_direction);
	}
	else
	{
		return CCParticleEmitter::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}


CCParticleEmitter*  CCParticleSphereEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
{
	CCParticleEmitter* pe = new CCParticleSphereEmitter(pt);
	return pe;
}

NS_CC_END