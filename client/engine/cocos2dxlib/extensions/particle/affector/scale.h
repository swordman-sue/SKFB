#ifndef AFFECTOR_SCALE_H
#define AFFECTOR_SCALE_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "particle/dynamicattribute.h"

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleScaleAffector : public CCParticleAffector
{
public:
	static const float DEFAULT_X_SCALE;
	static const float DEFAULT_Y_SCALE;
	static const float DEFAULT_Z_SCALE;
	static const float DEFAULT_XYZ_SCALE;
	static const bool DEFAULT_FIXED;

	CCParticleScaleAffector(CCParticleTechnique* pt);

	virtual ~CCParticleScaleAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	DynamicAttribute* GetDynScaleX(void) const
	{
		return m_dyn_scale_x;
	}

	inline void SetDynScaleX(DynamicAttribute* dynScaleX);

	DynamicAttribute* GetDynScaleY(void) const
	{
		return m_dyn_scale_y;
	}

	inline void SetDynScaleY(DynamicAttribute* dynScaleY);

	DynamicAttribute* GetDynScaleZ(void) const
	{
		return m_dyn_scale_z;
	}

	inline void SetDynScaleZ(DynamicAttribute* dynScaleZ);

	DynamicAttribute* GetDynScaleXYZ(void) const
	{
		return m_dyn_scale_xyz;
	}

	void SetXYZScale(bool is_scale)
	{
		m_dyn_set_xyz = is_scale;
		if (m_dyn_set_xyz)
		{
			m_dyn_set_x = false;
			m_dyn_set_y = false;
			m_dyn_set_z = false;
		}
		else
		{
			m_dyn_set_x = true;
			m_dyn_set_y = true;
			m_dyn_set_z = true;
		}
	}

	bool IsXYZScale()
	{
		return m_dyn_set_xyz;
	}

	void SetFixed(bool fixed) { m_fixed = fixed; }

	bool IsFixed() { return m_fixed; }

	inline void SetDynScaleXYZ(DynamicAttribute* dynScaleXYZ);

	virtual bool IsFitParticle(CCParticle* particle);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

protected:
	DynamicAttribute *m_dyn_scale_x;
	DynamicAttribute *m_dyn_scale_y;
	DynamicAttribute *m_dyn_scale_z;
	DynamicAttribute *m_dyn_scale_xyz;
	bool m_dyn_set_x;
	bool m_dyn_set_y;
	bool m_dyn_set_z;
	bool m_dyn_set_xyz;

	bool m_fixed;
};


class  CCParticleScaleAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleScaleAffectorFactory() {}

	virtual ~CCParticleScaleAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Scale";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif