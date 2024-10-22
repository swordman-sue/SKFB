#ifndef AFFECTOR_PATHFOLLOWER_H
#define AFFECTOR_PATHFOLLOWER_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "particle/simplespline.h"
#include "cocos2d.h"

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticle;
class CCParticleAffectorFactory;

class CCPathFollowerAffector : public CCParticleAffector
{
public:
	CCPathFollowerAffector(CCParticleTechnique* pt);

	virtual ~CCPathFollowerAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	inline void AddPoint(const Vec3& point);

	inline void ClearPoints (void);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

protected:
	CCSimpleSpline m_spline;
};


class  CCPathFollowerAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCPathFollowerAffectorFactory() {}

	virtual ~CCPathFollowerAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "PathFollower";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif