
#include "box.h"
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"
#include "particle/attribute.h"

#include "particle/particlefactory.h"
#include "particle/technique.h"
#include "particle/strconv.h"

#include "cocos/base/ccTypes.h"
#include "cocos/2d/CCNode.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <string>

NS_CC_BEGIN

const Vec3 CCParticleBoxEmitter::DEFAULT_SIZE (50, 50, 50);

const Vec3 CCParticleBoxEmitter::VECTOR3_ZERO (0, 0, 0);
const Vec3 CCParticleBoxEmitter::VECTOR3_UNIT_Z (0, 0, 1);
const Vec3 CCParticleBoxEmitter::VECTOR3_UNIT_Y (0, 1, 0);

CCParticleBoxEmitter::CCParticleBoxEmitter(CCParticleTechnique* pt) :
 	CCParticleEmitter(pt), m_size(VECTOR3_ZERO), m_range_x(VECTOR3_ZERO),
 	m_range_y(VECTOR3_ZERO), m_range_z(VECTOR3_ZERO),
 	m_box_dir(VECTOR3_UNIT_Z), m_box_up(VECTOR3_UNIT_Y)
{
 	m_emitter_type = "Box";
 	GenAreaAxes();
}
 
void CCParticleBoxEmitter::GenAreaAxes(void)
{
 	Vec3 left ;//= m_box_up.crossProduct(m_box_dir);
	
	Vec3::cross(m_box_up, m_box_dir,&left);
 	
	m_range_x = left * (m_size.x*0.5f);
 	m_range_y =  m_box_up * (m_size.y * 0.5f);
 	m_range_z = m_box_dir * (m_size.z * 0.5f);
}
 
void CCParticleBoxEmitter::SetBoxDir(const Vec3& dir)
{
 	m_box_dir = dir;
	m_box_dir.normalize();
 	CCParticleHelper::Perpendicular(&m_box_up, &m_box_dir);
 	m_box_up.normalize();
 	GenAreaAxes();
}
 
void CCParticleBoxEmitter::InitParticlePos(CCParticle* particle)
{
 	Vec3 xOff, yOff, zOff;
	xOff = m_range_x * CCParticleHelper::RangeRandom(-1, 1);
	yOff = m_range_y * CCParticleHelper::RangeRandom(-1, 1);
	zOff = m_range_z * CCParticleHelper::RangeRandom(-1, 1);
 
	Vec3 v1, offset;
	Vec3::add(xOff, yOff,&v1);
	Vec3::add(v1, zOff,&offset);
	Vec3::add(m_relative_position, offset,&particle->m_position);
}
 
void CCParticleBoxEmitter::CopyAttributesTo(CCParticleEmitter* emitter)
{
 	CCParticleEmitter::CopyAttributesTo(emitter);
 	CCParticleBoxEmitter* boxEmitter = static_cast<CCParticleBoxEmitter*>(emitter);
 	boxEmitter->SetSize(m_size);
}
 
bool CCParticleBoxEmitter::SetAttribute(const char* type, const char* value)
{
 	if (strcmp(type, "size") == 0)
 	{
		Vec3 pOut;
		CCParticleHelper::ParseVec3(value, &pOut);
 		SetSize(pOut);
 	}
 	else if (strcmp(type, "box_width") == 0)
 	{
 		SetWidth(CCParticleHelper::ParseFloat(value));
 	}
 	else if (strcmp(type, "box_height") == 0)
 	{
 		SetHeight(CCParticleHelper::ParseFloat(value));
 	}
 	else if (strcmp(type, "box_depth") == 0)
 	{
 		SetDepth(CCParticleHelper::ParseFloat(value));
 	}
 	else if (strcmp(type, "box_dir") == 0)
 	{
		Vec3 pOut;
		CCParticleHelper::ParseVec3(value, &pOut);
 		SetBoxDir(pOut);
 	}
 	else
 	{
 		return CCParticleEmitter::SetAttribute(type, value);
 	}
 	return true;
}
 
bool CCParticleBoxEmitter::GetAttribute(const char* type, char* value, size_t size)
{
 	std::string str;
 	if (strcmp(type, "size") == 0)
 	{
 		str = CCParticleHelper::ToString(m_size);
 	}
 	else if (strcmp(type, "box_dir") == 0)
 	{
 		str = CCParticleHelper::ToString(m_box_dir);
 	}
 	else
 	{
 		return CCParticleEmitter::GetAttribute(type, value, size);
 	}
 
 	strncpy(value, str.c_str(), size);
 	return true;
}
 
void CCParticleBoxEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
 	CCParticleEmitter::SaveScript(my_document, element);
 
	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_size, DEFAULT_SIZE))
	{
		str = CCParticleHelper::ToString(m_size);
		child = my_document->NewElement("size");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_box_dir, VECTOR3_UNIT_Z))
	{
		str = CCParticleHelper::ToString(m_box_dir);
		child = my_document->NewElement("box_dir");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}
}
 
CCParticleEmitter* CCParticleBoxEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
{
 	CCParticleEmitter* pe = new CCParticleBoxEmitter(pt);
 	return pe;
}

NS_CC_END