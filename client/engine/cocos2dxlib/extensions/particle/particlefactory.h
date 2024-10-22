#ifndef _ParticleFactory_H__
#define _ParticleFactory_H__

#include <string>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticleEmitter;
class CCParticleAffector;
class CCParticleRenderer;

class CCParticleEmitterFactory
{

public:
	CCParticleEmitterFactory() {}
	virtual ~CCParticleEmitterFactory() {}

	virtual std::string GetType() const = 0;
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt) = 0;
};

class CCParticleAffectorFactory
{
public:
	CCParticleAffectorFactory() {}
	virtual ~CCParticleAffectorFactory() {}

	virtual std::string GetType() const = 0;
	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt) = 0;
};

class CCParticleRendererFactory
{
public:
	CCParticleRendererFactory(const std::string& type_str):m_type_str(type_str) {}
	virtual ~CCParticleRendererFactory() {}

	virtual std::string GetType() const { return m_type_str; }
	virtual CCParticleRenderer* CreateRenderer(CCParticleTechnique* pt, const std::string& texture_suffix) = 0;

private:
	std::string m_type_str;
};

NS_CC_END

#endif
