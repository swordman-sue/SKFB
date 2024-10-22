#ifndef AFFECTOR_RANDOMISER_H
#define AFFECTOR_RANDOMISER_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <string>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleRandomiserAffector : public CCParticleAffector
{
public:
	static const Vec3 DEFAULT_DEVIATION;
	static const bool DEFAULT_RANDOM;

	CCParticleRandomiserAffector(CCParticleTechnique* pt);

	virtual ~CCParticleRandomiserAffector() {}

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	bool IsRandomDirection()
	{
		return m_random_direction;
	}

	void SetRandomDirection(bool randomDir)
	{
		m_random_direction = randomDir;
	}

	const Vec3& GetMaxDeviation() const
	{
		return m_max_deviation;
	}

	void SetMaxDeviation(const Vec3& vec)
	{
		m_max_deviation = vec;
	}

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

protected:
	bool m_random_direction;
	Vec3 m_max_deviation;
};


class  CCParticleRandomiserAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleRandomiserAffectorFactory() {}

	virtual ~CCParticleRandomiserAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Randomiser";
	}

	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif