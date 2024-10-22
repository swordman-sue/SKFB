#ifndef EMITTER_CIRCLE_H
#define EMITTER_CIRCLE_H

#include "emitter.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include "math/CCMath.h"

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticleEmitter;
class CCParticle;


class CCParticleCircleEmitter : public CCParticleEmitter
{
public:
	enum AxisType
	{
		AXIS_X = 0,
		AXIS_Y,
		AXIS_Z
	};

public:
	static const float DEFAULT_RADIUS;
	static const float DEFAULT_WIDTH;
	static const float DEFAULT_STEP;
	static const float DEFAULT_ANGLE;
	static const bool DEFAULT_RANDOM;
	static const bool DEFAULT_AUTO;
	static const Vec3 DEFAULT_NORMAL;
	static const AxisType DEFAULT_AXIS;

	static const Vec3 VECTOR3_ZERO;
	static const Vec3 VECTOR3_UNIT_X;
	static const Vec3 VECTOR3_UNIT_Z;
	static const Vec3 VECTOR3_UNIT_Y;

	static const Quaternion QUATERNION_IDENTITY;

public:
	CCParticleCircleEmitter(CCParticleTechnique* pt);

	virtual ~CCParticleCircleEmitter(void) {}

	virtual void InitParticlePos(CCParticle* particle);

	virtual void CopyAttributesTo(CCParticleEmitter* pe);

	void SetRandom(bool is_random)
	{
		m_random = is_random;
	}

	bool GetRandom() const
	{
		return m_random;
	}

	void SetStep(float step)
	{
		m_step = step;
	}

	float GetSetp() const
	{
		return m_step;
	}

	bool SetFanStartAngle(float angle);

	bool SetFanEndAngle(float angle);

	void SetNormal(const Vec3& normal);

	const Vec3& GetNormal() const
	{
		return m_normal;
	}

	void SetXRadius(float radius)
	{
		if(radius<0.0f || radius<m_x_width)
		{
			return;
		}
		else
		{
			m_x_radius = radius;
		}
	}

	float GetXRadius() const
	{
		return m_x_radius;
	}

	void SetZRadius(float radius)
	{
		if (radius < 0.0f || radius<m_z_width)
		{
			return;
		}
		else
		{
			m_z_radius = radius;
		}
	}

	float GetZRadius() const
	{
		return m_z_radius;
	}

	void SetXWidth(float width)
	{
		if(width>m_x_radius || width<0)
		{
			m_x_width = DEFAULT_WIDTH;
		}
		else
		{
			m_x_width = width;
		}
	}

	float GetXWidth()
	{
		return m_x_width;
	}

	void SetZWidth(float width)
	{
		if(width>m_z_radius || width<0)
		{
			m_z_width = DEFAULT_WIDTH;
		}
		else
		{
			m_z_width = width;
		}
	}

	float GetZWidth()
	{
		return m_z_width;
	}

	void SetCircleAngle(float angle)
	{
		m_circle_angle = angle;
	}

	float GetCircleAngle()
	{
		return m_circle_angle;
	}

	void SetAutoDirection(bool is_auto)
	{
		m_auto_direction = is_auto;
	}

	bool GetAutoDirection() const
	{
		return m_auto_direction;
	}

	void SetAxisType(AxisType type)
	{
		m_axis_type = type;
	}

	AxisType GetAxisType() const
	{
		return m_axis_type;
	}

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual void InitParticleDirection(CCParticle* particle);
		
private:
	bool m_random;
	float m_step;
	float m_circle_angle;
	Vec3 m_normal;
	float m_x_radius;
	float m_z_radius;
	float m_x_width;
	float m_z_width;
	Quaternion m_quaternion;
	float m_x;
	float m_z;
	bool m_auto_direction;
	AxisType m_axis_type;
	Vec3 m_temp_vec;
	float m_fan_start_angle;
	float m_fan_end_angle;
};


class  CCParticleCircleEmitterFactory : public CCParticleEmitterFactory
{
public:
	CCParticleCircleEmitterFactory() {}

	virtual ~CCParticleCircleEmitterFactory() {}

	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
	virtual std::string GetType() const
	{
		return "Circle";
	}

	/** Creates a new emitter instance.
	@remarks
	The subclass MUST add a pointer to the created instance to mEmitters.
	*/
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};
NS_CC_END

#endif