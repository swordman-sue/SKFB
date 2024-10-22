
#include "point.h"
#include "emitter.h"

#include "particle/particle.h"
#include "particle/particlehelper.h"

#include "particle/particlefactory.h"
#include "particle/strutil.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "tinyxml2/tinyxml2.h"
#include <string>

NS_CC_BEGIN

	CCParticlePointEmitter::CCParticlePointEmitter(CCParticleTechnique* pt) :
		CCParticleEmitter(pt)
	{
		m_emitter_type = "Point";
	}

	void CCParticlePointEmitter::CopyAttributesTo(CCParticleEmitter* pe)
	{
		CCParticleEmitter::CopyAttributesTo(pe);
	}

	CCParticleEmitter* CCParticlePointEmitterFactory::CreateEmitter(CCParticleTechnique* pt)
	{
		CCParticleEmitter* pe = new CCParticlePointEmitter(pt);
		return pe;
	}

NS_CC_END