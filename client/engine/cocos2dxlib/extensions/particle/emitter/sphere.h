#ifndef EMITTER_SPHERE_H
#define EMITTER_SPHERE_H

#include "emitter.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"

NS_CC_BEGIN

class CCParticleTechnique;

class CCParticleSphereEmitter : public CCParticleEmitter
{
public:
	static const float DEFAULT_RADIUS;
	static const bool DEFAULT_AUTO;

	CCParticleSphereEmitter(CCParticleTechnique* pt);

	virtual ~CCParticleSphereEmitter() {}

	virtual void InitParticlePos(CCParticle* particle);

	virtual void CopyAttributesTo (CCParticleEmitter* emitter);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual void InitParticleDirection(CCParticle* particle);

	void SetRadius(float radius)
	{
		m_radius = radius;
	}

	float GetRadius() const
	{
		return m_radius;
	}

	void SetAutoDirection(bool is_auto)
	{
		m_auto_direction = is_auto;
	}

	bool GetAutoDirection() const
	{
		return m_auto_direction;
	}

private:
	float m_radius;
	Vec3 m_random_vec;
	bool m_auto_direction;
};


class CCParticleSphereEmitterFactory : public CCParticleEmitterFactory
{
public:
	CCParticleSphereEmitterFactory() {}

	virtual ~CCParticleSphereEmitterFactory() {}

	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
	virtual std::string GetType() const
	{
		return "Sphere";
	}

	/** Creates a new emitter instance.
	@remarks
	The subclass MUST add a pointer to the created instance to mEmitters.
	*/
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};

NS_CC_END

#endif