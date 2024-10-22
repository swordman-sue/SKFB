#ifndef EMITTER_POINT_H
#define EMITTER_POINT_H

#include "emitter.h"
#include "particle/particlefactory.h"
#include "math/CCMath.h"

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticleEmitter;
class CCParticle;

class CCParticlePointEmitter : public CCParticleEmitter
{
public:
	CCParticlePointEmitter(CCParticleTechnique* pt);

	virtual ~CCParticlePointEmitter(void) {}

	virtual void CopyAttributesTo(CCParticleEmitter* pe);
};


class CCParticlePointEmitterFactory : public CCParticleEmitterFactory
{
public:
	CCParticlePointEmitterFactory() {}

	virtual ~CCParticlePointEmitterFactory() {}

	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
	virtual std::string GetType() const
	{
		return "Point";
	}

	/** Creates a new emitter instance.
	@remarks
	The subclass MUST add a pointer to the created instance to mEmitters.
	*/
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};

NS_CC_END

#endif