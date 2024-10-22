#ifndef AFFECTOR_DEFLECTOR_H
#define AFFECTOR_DEFLECTOR_H

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

class CCParticleDeflectorAffector : public CCParticleAffector
{
public:
	static const Vec3 DEFAULT_POINT;
	static const Vec3 DEFAULT_NORMAL;
	static const float   DEFAULT_BOUNCE;

	CCParticleDeflectorAffector(CCParticleTechnique* pt);

	virtual ~CCParticleDeflectorAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	void SetPlanePoint(const Vec3& point);

	void SetPlaneNormal(const Vec3& normal);

	const Vec3& GetPlanePoint() const
	{
		return m_plane_point;
	}

	const Vec3& GetPlaneNormal() const
	{
		return m_plane_normal;
	}

	void SetDynBounce(DynamicAttribute *dyn);

	DynamicAttribute* GetDynBounce()
	{
		return m_dyn_bounce;
	}

	void UpdateDistance();

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

protected:
	Vec3 m_plane_point;
	Vec3 m_plane_normal;
	Vec3 m_normal;
	DynamicAttribute *m_dyn_bounce;
	float m_distance;
};


class  CCParticleDeflectorAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleDeflectorAffectorFactory() {}

	virtual ~CCParticleDeflectorAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Deflector";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END
#endif