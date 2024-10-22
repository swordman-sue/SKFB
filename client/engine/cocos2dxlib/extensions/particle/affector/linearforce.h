#ifndef AFFECTOR_LINEARFORCE_H
#define AFFECTOR_LINEARFORCE_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <string>

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticleAffector;
class CCParticle;
class CCParticleAffectorFactory;

class CCParticleLinearForceAffector : public CCParticleAffector
{
public:
	enum ForceApplication
	{
		FA_AVERAGE,
		FA_ADD
	};

	static const Vec3 DEFAULT_FORCE_VEC;
	static const ForceApplication DEFAULT_FORCE_APP;
	static const float DEFAULT_FORCE;

	CCParticleLinearForceAffector(CCParticleTechnique* pt);

	virtual ~CCParticleLinearForceAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	void SetForceApplication(ForceApplication fa)
	{
		m_force_application = fa;
	}

    /** Retrieves how the force vector is applied to a particle. 
    @param fa A member of the ForceApplication enum.
    */
    ForceApplication GetForceApplication(void) const
	{
		return m_force_application;
	}

	/** Sets the force vector to apply to the particles in a system. */
	void SetForceVector(const Vec3& force)
	{
		m_force_vector = force;
	}

	/** Gets the force vector to apply to the particles in a system. */
	const Vec3& GetForceVector(void) const
	{
		return m_force_vector;
	}

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual void SetDynForce(DynamicAttribute* dyn);

	DynamicAttribute* GetDynForce()
	{
		return m_dyn_force;
	}

protected:
	/// Force vector
	Vec3 m_force_vector;

	ForceApplication m_force_application;

	DynamicAttribute* m_dyn_force;
};


class  CCParticleLinearForceAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleLinearForceAffectorFactory() {}

	virtual ~CCParticleLinearForceAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "LinearForce";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END
#endif