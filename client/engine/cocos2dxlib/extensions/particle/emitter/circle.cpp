
#include "circle.h"
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

	const float CCParticleCircleEmitter::DEFAULT_RADIUS = 32.0f;
	const float CCParticleCircleEmitter::DEFAULT_WIDTH = 1.0f;
	const float CCParticleCircleEmitter::DEFAULT_STEP = 10.0f;
	const float CCParticleCircleEmitter::DEFAULT_ANGLE = 0.0f;
	const bool CCParticleCircleEmitter::DEFAULT_RANDOM = true;
	const bool CCParticleCircleEmitter::DEFAULT_AUTO =false;
	const Vec3 CCParticleCircleEmitter::DEFAULT_NORMAL (0, 1, 0);
	const CCParticleCircleEmitter::AxisType CCParticleCircleEmitter::DEFAULT_AXIS = CCParticleCircleEmitter::AXIS_Z;

	const Vec3 CCParticleCircleEmitter::VECTOR3_ZERO (0, 0, 0);
	const Vec3 CCParticleCircleEmitter::VECTOR3_UNIT_X (1, 0, 0);
	const Vec3 CCParticleCircleEmitter::VECTOR3_UNIT_Z (0, 0, 1);
	const Vec3 CCParticleCircleEmitter::VECTOR3_UNIT_Y (0, 1, 0);

	const Quaternion CCParticleCircleEmitter::QUATERNION_IDENTITY (1, 0, 0, 0);

	CCParticleCircleEmitter::CCParticleCircleEmitter(CCParticleTechnique* pt) :
		CCParticleEmitter(pt),
		m_random(DEFAULT_RANDOM),
		m_step(DEFAULT_STEP),
		m_circle_angle(DEFAULT_ANGLE),
		m_normal(DEFAULT_NORMAL),
		m_x_radius(DEFAULT_RADIUS),
		m_z_radius(DEFAULT_RADIUS),
		m_x_width(DEFAULT_WIDTH),
		m_z_width(DEFAULT_WIDTH),
		m_auto_direction(DEFAULT_AUTO),
		m_axis_type(DEFAULT_AXIS),
		m_quaternion(QUATERNION_IDENTITY),
		m_x(0.0f),
		m_z(0.0f),
		m_fan_start_angle(0.0f),
		m_fan_end_angle(360.0f)
	{
		m_emitter_type = "Circle";
	}

	void CCParticleCircleEmitter::SetNormal(const Vec3& normal)
	{
		//m_quaternion = Vector3::UNIT_Y.getRotationTo(normal, Vector3::UNIT_X);
		Quaternion::QuaternionRotationBetweenVec3(&m_quaternion, &VECTOR3_UNIT_Y, &normal, &VECTOR3_UNIT_X);
		m_normal = normal;
	}

	void CCParticleCircleEmitter::CopyAttributesTo(CCParticleEmitter* pe)
	{
		CCParticleEmitter::CopyAttributesTo(pe);
		CCParticleCircleEmitter* circleEmitter = static_cast<CCParticleCircleEmitter*>(pe);
		circleEmitter->SetRandom(m_random);
		circleEmitter->SetStep(m_step);
		circleEmitter->SetXRadius(m_x_radius);
		circleEmitter->SetZRadius(m_z_radius);
		circleEmitter->SetXWidth(m_x_width);
		circleEmitter->SetZWidth(m_z_width);
		circleEmitter->SetCircleAngle(m_circle_angle);
		circleEmitter->SetAutoDirection(m_auto_direction);
		circleEmitter->SetAxisType(m_axis_type);
		circleEmitter->SetFanStartAngle(m_fan_start_angle);
		circleEmitter->SetFanEndAngle(m_fan_end_angle);

		//by zhuyi 2015-02-10 默认的m_normal计算出的m_quaternion不等于默认的m_quaternion(QUATERNION_IDENTITY)的值
		if(!CCParticleHelper::IsEqual(m_normal, DEFAULT_NORMAL))
		{
			circleEmitter->SetNormal(m_normal);
		}
	}

	void  CCParticleCircleEmitter::InitParticlePos(CCParticle* particle)
	{
		float m = m_fan_end_angle;
		float angle = 0,x_off_width=0,z_off_width=0;
		if (m_random)
		{
			// Choose a random position on the circle.
			float temp_angle = m_fan_end_angle - m_fan_start_angle;
			angle = CCParticleHelper::RangeRandom(0, temp_angle) + m_fan_start_angle;
			x_off_width = CCParticleHelper::RangeRandom(0,m_x_width);
			z_off_width = CCParticleHelper::RangeRandom(0,m_z_width);
		}
		else
		{
			// Follow the contour of the circle.
			m_circle_angle += m_step;
			m_circle_angle = m_circle_angle > 360 ? m_circle_angle - 360 : m_circle_angle;
			angle = m_circle_angle;
		}

		angle = angle * kmPIOver180;
		m_x = (m_x_radius - x_off_width) * cosf(angle);
		m_z = (m_z_radius - z_off_width) * sinf(angle) ;

		if (m_axis_type == AXIS_Z)
		{
			m_temp_vec.set(m_x, m_z, 0.0f);
		}
		else if (m_axis_type == AXIS_Y)
		{
			m_temp_vec.set(m_x, 0.0f, m_z);
		}
		else
		{
			m_temp_vec.set(0.0f, m_x, m_z);
		}

		Vec3 tempvec3;
		Quaternion::QuaternionMultiplyVec3(&tempvec3, &m_quaternion, &m_temp_vec);
		Vec3::add(m_relative_position,tempvec3,&particle->m_position);

		Mat4 m_x = Mat4(1,0,0,0,
					0,0,-1,0,
					0,1,0,0,
					0,0,0,1);

		Vec3 vec3;
		vec3.set(0,0,0);
		Quaternion q_x_90;
		Quaternion::createFromRotationMatrix(m_x,&q_x_90);
		Quaternion::QuaternionMultiplyVec3(&vec3, &q_x_90, &VECTOR3_UNIT_Y);

		Mat4 m_y = Mat4(0,0,1,0,
					0,1,0,0,
					-1,0,0,0,
					0,0,0,1);

		Quaternion q_y_90;
		Quaternion::createFromRotationMatrix(m_y,&q_y_90);

		Mat4 m_z = Mat4(0,-1,0,0,
						1,0,0,0,
						0,0,1,0,
						0,0,0,1);

		Quaternion q_z_90;
		Quaternion::createFromRotationMatrix(m_z,&q_z_90);


	}

	void CCParticleCircleEmitter::InitParticleDirection(CCParticle* particle)
	{
		if (m_auto_direction)
		{
			m_temp_vec = m_temp_vec.getNormalized();
			Quaternion::QuaternionMultiplyVec3(&particle->m_direction, &m_quaternion, &m_temp_vec);
		}
		else
		{
			CCParticleEmitter::InitParticleDirection(particle);
		}
	}

	void CCParticleCircleEmitter::SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element)
	{
		CCParticleEmitter::SaveScript(my_document, element);

		tinyxml2::XMLElement* child = 0;
		tinyxml2::XMLText* content = 0;
		std::string str;

		if (!CCParticleHelper::IsEqual(m_random, DEFAULT_RANDOM))
		{
			str = CCParticleHelper::ToString(m_random);
			child = my_document->NewElement("circle_random");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_step, DEFAULT_STEP))
		{
			str = CCParticleHelper::ToString(m_step);
			child = my_document->NewElement("circle_step");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_circle_angle, DEFAULT_ANGLE))
		{
			str = CCParticleHelper::ToString(m_circle_angle);
			child = my_document->NewElement("circle_angle");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_normal, DEFAULT_NORMAL))
		{
			str = CCParticleHelper::ToString(m_normal);
			child = my_document->NewElement("circle_normal");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_x_radius, DEFAULT_RADIUS))
		{
			str = CCParticleHelper::ToString(m_x_radius);
			child = my_document->NewElement("circle_x_radius");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual(m_z_radius, DEFAULT_RADIUS))
		{
			str = CCParticleHelper::ToString(m_z_radius);
			child = my_document->NewElement("circle_z_radius");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if(!CCParticleHelper::IsEqual(m_x_width,DEFAULT_WIDTH))
		{
			str = CCParticleHelper::ToString(m_x_width);
			child = my_document->NewElement("circle_x_width");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}


		if(!CCParticleHelper::IsEqual(m_z_width,DEFAULT_WIDTH))
		{
			str = CCParticleHelper::ToString(m_z_width);
			child = my_document->NewElement("circle_z_width");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}
		if (!CCParticleHelper::IsEqual(m_auto_direction, DEFAULT_AUTO))
		{
			str = CCParticleHelper::ToString(m_auto_direction);
			child = my_document->NewElement("circle_auto_dir");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (!CCParticleHelper::IsEqual((int)m_axis_type, (int)DEFAULT_AXIS))
		{
			str = CCParticleHelper::ToString(m_axis_type);
			child = my_document->NewElement("axis_type");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (m_fan_start_angle != 0.0f)
		{
			str = CCParticleHelper::ToString(m_fan_start_angle);
			child = my_document->NewElement("fan_start_angle");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

		if (m_fan_end_angle != 360.0f)
		{
			str = CCParticleHelper::ToString(m_fan_end_angle);
			child = my_document->NewElement("fan_end_angle");
			content = my_document->NewText(str.c_str());
			element->LinkEndChild(child);
			child->LinkEndChild(content);
		}

	}

	bool CCParticleCircleEmitter::SetAttribute(const char* type, const char* value)
	{
		if (strcmp(type, "circle_random") == 0)
		{
			SetRandom(CCParticleHelper::ParseBool(value));
		}
		else if (strcmp(type, "circle_step") == 0)
		{
			SetStep(CCParticleHelper::ParseFloat(value));
		}
		else if (strcmp(type, "circle_angle") == 0)
		{
			SetCircleAngle(CCParticleHelper::ParseFloat(value));
		}
		else if (strcmp(type, "circle_normal") == 0)
		{
			Vec3 pOut;
			CCParticleHelper::ParseVec3(value, &pOut);
			SetNormal(pOut);
		}
		else if (strcmp(type, "circle_x_radius") == 0)
		{
			SetXRadius(CCParticleHelper::ParseFloat(value));
		}
		else if (strcmp(type, "circle_z_radius") == 0)
		{
			SetZRadius(CCParticleHelper::ParseFloat(value));
		}
		else if(strcmp(type,"circle_x_width")==0)
		{
			if(CCParticleHelper::ParseFloat(value)<=m_x_radius && CCParticleHelper::ParseFloat(value)>=1.0f)
			{
				SetXWidth(CCParticleHelper::ParseFloat(value));
			}
		}
		else if(strcmp(type,"circle_z_width")==0)
		{
			if(CCParticleHelper::ParseFloat(value)<=m_z_radius && CCParticleHelper::ParseFloat(value)>=1.0f)
			{
				SetZWidth(CCParticleHelper::ParseFloat(value));
			}
		}
		else if (strcmp(type, "circle_radius") == 0)
		{
			SetXRadius(CCParticleHelper::ParseFloat(value));
			SetZRadius(CCParticleHelper::ParseFloat(value));
		}
		else if (strcmp(type, "circle_auto_dir") == 0)
		{
			SetAutoDirection(CCParticleHelper::ParseBool(value));
		}
		else if (strcmp(type, "axis_type") == 0)
		{
			SetAxisType(AxisType(CCParticleHelper::ParseInt(value)));
		}
		else if (strcmp(type,"fan_start_angle")==0)
		{
			return SetFanStartAngle(CCParticleHelper::ParseFloat(value));
		}
		else if(strcmp(type,"fan_end_angle")==0)
		{
			return SetFanEndAngle(CCParticleHelper::ParseFloat(value));
		}
		else
		{
			return CCParticleEmitter::SetAttribute(type, value);
		}
		return true;
	}

	bool CCParticleCircleEmitter::GetAttribute(const char* type, char* value, size_t size)
	{
		std::string str;
		if (strcmp(type, "circle_random") == 0)
		{
			str = CCParticleHelper::ToString(m_random);
		}
		else if (strcmp(type, "circle_step") == 0)
		{
			str = CCParticleHelper::ToString(m_step);
		}
		else if (strcmp(type, "circle_angle") == 0)
		{
			str = CCParticleHelper::ToString(m_circle_angle);
		}
		else if (strcmp(type, "circle_normal") == 0)
		{
			str = CCParticleHelper::ToString(m_normal);
		}
		else if (strcmp(type, "circle_x_radius") == 0)
		{
			str = CCParticleHelper::ToString(m_x_radius);
		}
		else if (strcmp(type, "circle_z_radius") == 0)
		{
			str = CCParticleHelper::ToString(m_z_radius);
		}
		else if(strcmp(type,"circle_z_width")==0)
		{
			str = CCParticleHelper::ToString(m_z_width);
		}
		else if(strcmp(type,"circle_x_width")==0)
		{
			str = CCParticleHelper::ToString(m_x_width);
		}
		else if (strcmp(type, "circle_auto_dir") == 0)
		{
			str = CCParticleHelper::ToString(m_auto_direction);
		}
		else if (strcmp(type, "axis_type") == 0)
		{
			str = CCParticleHelper::ToString(int(m_axis_type));
		}else if(strcmp(type, "fan_start_angle") == 0)
		{
			str = CCParticleHelper::ToString(m_fan_start_angle);
		}else if(strcmp(type,"fan_end_angle")==0)
		{
			str = CCParticleHelper::ToString(m_fan_end_angle);
		}
		else
		{
			return CCParticleEmitter::GetAttribute(type, value, size);
		}

		strncpy(value, str.c_str(), size);
		return true;
	}

	bool CCParticleCircleEmitter::SetFanStartAngle(float angle)
	{
		angle = angle < 0 ? 0 : angle;
		angle = angle > 720 ? 720 : angle;
		if (angle < m_fan_end_angle)
		{
			m_fan_start_angle =  angle;
			return true;
		}
		return false;
	}

	bool CCParticleCircleEmitter::SetFanEndAngle(float angle)
	{
		angle = angle < 0 ? 0 : angle;
		angle = angle > 720 ? 720 : angle;
		if (angle > m_fan_start_angle)
		{
			m_fan_end_angle = angle;
			return true;
		}
		return false;
	}

	CCParticleEmitter* CCParticleCircleEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
	{
		CCParticleEmitter* pe = new CCParticleCircleEmitter(pt);
		return pe;
	}
NS_CC_END