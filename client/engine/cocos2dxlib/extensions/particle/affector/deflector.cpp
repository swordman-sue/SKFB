
#include "deflector.h"
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

#include <cmath>


NS_CC_BEGIN

const Vec3 CCParticleDeflectorAffector::DEFAULT_POINT(0, 0, 0);
const Vec3 CCParticleDeflectorAffector::DEFAULT_NORMAL(0, -1, 0);
const float  CCParticleDeflectorAffector::DEFAULT_BOUNCE = 1.0f;

CCParticleDeflectorAffector::CCParticleDeflectorAffector(CCParticleTechnique* pt) :
	CCParticleAffector(pt),
	m_plane_point(DEFAULT_POINT),
	m_plane_normal(DEFAULT_NORMAL),
	m_distance(0.0f)
{
	m_type = "Deflector";
	m_normal = m_plane_normal;

	m_dyn_bounce = new DynamicAttributeFixed();
	static_cast<DynamicAttributeFixed*>(m_dyn_bounce)->SetValue(DEFAULT_BOUNCE);

	m_affector_weight = 9;
}

CCParticleDeflectorAffector::~CCParticleDeflectorAffector()
{
	if (m_dyn_bounce != 0)
	{
		delete m_dyn_bounce;
		m_dyn_bounce = 0;
	}
}

void CCParticleDeflectorAffector::SetDynBounce(DynamicAttribute *dyn)
{
	if (m_dyn_bounce != 0)
	{
		delete m_dyn_bounce;
		m_dyn_bounce = 0;
	}
	m_dyn_bounce = dyn;
}

void CCParticleDeflectorAffector::UpdateDistance()
{
	m_distance =  - Vec3::dot(m_plane_normal, m_plane_point) / m_plane_normal.length();
}

void CCParticleDeflectorAffector::SetPlanePoint(const Vec3& point)
{
	m_plane_point = point;
	UpdateDistance();
}

void CCParticleDeflectorAffector::SetPlaneNormal(const Vec3& normal)
{
	m_plane_normal = normal;
	Vec3 zero(0, 0, 0);
	if (CCParticleHelper::IsEqual(m_plane_normal, zero))
	{
		m_plane_normal = DEFAULT_NORMAL;
	}
	m_normal = m_plane_normal.getNormalized();
	UpdateDistance();
}

bool  CCParticleDeflectorAffector::SetDynAttribute(const char* type, DynamicAttribute *dyn)
{
	if (strcmp(type, "plane_bounce") == 0)
	{
		SetDynBounce(dyn);
	}
	else
	{
		return CCParticleAffector::SetDynAttribute(type, dyn);
	}
	return true;
}

void CCParticleDeflectorAffector::EffectParticle(CCParticle *particle, float elapsed)
{
	float percent = (particle->m_total_live - particle->m_time_live) / particle->m_total_live;
	float bounce = AttributeCalculate(m_dyn_bounce, percent);

	Vec3 position = particle->m_position;
	Vec3 direction = particle->m_direction; 

	bool is_local = m_parent->IsLocalSpace();
	if (!is_local && particle->m_type != CCParticle::Emitter)
	{
		Mat4 outside_mat = m_parent->GetTechToWorldMat();
		outside_mat = outside_mat.getInversed();
		outside_mat.transformPoint(&position);
		outside_mat.transformVector(&direction);
	}
	
	direction = direction * elapsed;
	//当前粒子的位置+当前帧会走的距离 来比较是否超过阻面距离
	float b = Vec3::dot(m_normal, direction);
	float curr_direct = Vec3::dot(m_normal, direction);
	float dot_value = Vec3::dot(position, m_normal) + m_distance;
	if (curr_direct < -0.000001 && (dot_value + b) < 0.000001f)
	{
		//求出法线、点的方程
		float a = m_normal.x;
		float b = m_normal.y;
		float e = a * m_plane_point.x + b * m_plane_point.y;

		float c = -direction.y;
		float d = direction.x;
		float f = c * position.x + d * position.y;
		float flag = c * b - a * d;
		if (flag >= 0.000001f || flag <= -0.000001f)
		{

			Vec3 pos((b*f - d*e)/(c*b - a*d), (c*e - a*f)/(c*b - a*d), 0);
			if (!is_local && particle->m_type != CCParticle::Emitter)
			{
				Mat4 outside_mat = m_parent->GetTechToWorldMat();
				outside_mat.transformPoint(&pos); //粒子位置转换到世界空间
			}

			particle->m_position.x = pos.x;
			particle->m_position.y = pos.y;

			if (particle->m_type == CCParticle::Emitter)
			{
				CCParticleEmitter* emitter = static_cast<CCParticleEmitter*>(particle);
				emitter->m_relative_position.x = pos.x;
				emitter->m_relative_position.y = pos.y;
			}

			float d = Vec3::dot(particle->m_direction, m_normal);
			d *= 2.0f;
			Vec3 directionPart = m_normal * d;
			Vec3::subtract(particle->m_direction, directionPart,&particle->m_direction);
			particle->m_direction = particle->m_direction * bounce;

		}
	}
	
}

void CCParticleDeflectorAffector::CopyAttributesTo(CCParticleAffector* pa)
{
	CCParticleAffector::CopyAttributesTo(pa);
	CCParticleDeflectorAffector* pd = static_cast<CCParticleDeflectorAffector*>(pa);
	pd->SetPlanePoint(m_plane_point);
	pd->SetPlaneNormal(m_plane_normal);
	pd->SetDynBounce(CloneDynamicAttribute(m_dyn_bounce));
}


bool CCParticleDeflectorAffector::SetAttribute(const char* type, const char* value)
{
	if (strcmp(type, "plane_point") == 0)
	{
		CCParticleHelper::ParseVec3(value, &m_plane_point);
	}
	else if (strcmp(type, "plane_normal") == 0)
	{
		Vec3 normal(0, 1, 0);
		CCParticleHelper::ParseVec3(value, &normal);
		SetPlaneNormal(normal);
	}
	else if (strcmp(type, "plane_bounce") == 0)
	{
		DynamicAttribute *dyn = ParseDynamicAttribute(value);
		SetDynBounce(dyn);
	}
	else
	{
		return CCParticleAffector::SetAttribute(type, value);
	}
	return true;
}

bool CCParticleDeflectorAffector::GetAttribute(const char* type, char* value, size_t size)
{
	std::string str;
	if (strcmp(type, "plane_point") == 0)
	{
		str = CCParticleHelper::ToString(m_plane_point);
	}
	else if (strcmp(type, "plane_normal") == 0)
	{
		str = CCParticleHelper::ToString(m_plane_normal);
	}
	else if (strcmp(type, "plane_bounce") == 0)
	{
		GetDynAttribute(m_dyn_bounce, str);
	}
	else
	{
		return CCParticleAffector::GetAttribute(type, value, size);
	}
	strncpy(value, str.c_str(), size);
	return true;
}


void CCParticleDeflectorAffector::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
{
	CCParticleAffector::SaveScript(my_document, element);

	tinyxml2::XMLElement* child = 0;
	tinyxml2::XMLText* content = 0;
	std::string str;

	if (!CCParticleHelper::IsEqual(m_plane_point, DEFAULT_POINT))
	{
		str = CCParticleHelper::ToString(m_plane_point);
		child = my_document->NewElement("plane_point");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!CCParticleHelper::IsEqual(m_plane_normal, DEFAULT_NORMAL))
	{
		str = CCParticleHelper::ToString(m_plane_normal);
		child = my_document->NewElement("plane_normal");
		content = my_document->NewText(str.c_str());
		element->LinkEndChild(child);
		child->LinkEndChild(content);
	}

	if (!(m_dyn_bounce->GetType() == DynamicAttribute::DAT_FIXED && CCParticleHelper::IsEqual(static_cast<DynamicAttributeFixed*>(m_dyn_bounce)->GetValue(), DEFAULT_BOUNCE)))
	{
		child = my_document->NewElement("dyn");
		child->SetAttribute("type", "plane_bounce");
		m_dyn_bounce->SaveScript(my_document, child);
		element->LinkEndChild(child);
	}
}


CCParticleAffector* CCParticleDeflectorAffectorFactory::CreateAffector(CCParticleTechnique* pt)
{
	CCParticleAffector* pd = new CCParticleDeflectorAffector(pt);
	return pd;
}

NS_CC_END