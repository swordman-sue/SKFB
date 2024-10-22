#ifndef AFFECTOR_ROTATION_H
#define AFFECTOR_ROTATION_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <string>

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleRotationAffector : public CCParticleAffector
{
public:
	static const float TWOPIRAD;
	static const float DEFAULT_SPEED;

	CCParticleRotationAffector(CCParticleTechnique* pt);

	virtual ~CCParticleRotationAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	void SetRotationSpeed(DynamicAttribute* dyn);
		
	DynamicAttribute* GetRotationSpeed()
	{
		return m_dyn_rot_speed;
	}

	void SetRotationAngle(DynamicAttribute* dyn);

	DynamicAttribute* GetRotationAngle()
	{
		return m_dyn_start_angle;
	}

	virtual void InitParticle(CCParticle* particle);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

protected:
	DynamicAttribute* m_dyn_start_angle;

	DynamicAttribute* m_dyn_rot_speed;

	float m_speed;

private:
	float m_dyn_max_value;			//一个周期最大的偏移
};


class CCParticleRotationAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleRotationAffectorFactory() {}

	virtual ~CCParticleRotationAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Rotation";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif