#ifndef AFFECTOR_VORTEX_H
#define AFFECTOR_VORTEX_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <string>

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;

class CCParticleVortexAffector : public CCParticleAffector
{
public:
	static const Vec3 DEFAULT_ROTATION_VECTOR;
	static const float DEFAULT_ROTATION_SPEED;

	CCParticleVortexAffector(CCParticleTechnique* pt);

	virtual ~CCParticleVortexAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	void SetRotVector(const Vec3& vec);

	const Vec3& GetRotVector() const { return m_rot_vec; }

	void SetDynRotSpeed(DynamicAttribute* dyn);

	DynamicAttribute* GetDynRotSpeed()
	{
		return m_dyn_rot_speed;
	}

protected:
	Vec3 m_rot_vec;
	Quaternion m_rot;
	DynamicAttribute* m_dyn_rot_speed;
};


class CCParticleVortexAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleVortexAffectorFactory() {}

	virtual ~CCParticleVortexAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Vortex";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif