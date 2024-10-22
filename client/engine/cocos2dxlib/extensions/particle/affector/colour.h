#ifndef AFFECTOR_COLOUR_H
#define AFFECTOR_COLOUR_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <map>
#include <string>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleColourAffector : public CCParticleAffector
{
public:
	typedef std::map<float, Color4F> ColourMap;
	typedef ColourMap::iterator Iterator;

	CCParticleColourAffector(CCParticleTechnique* pt);

	virtual ~CCParticleColourAffector() {}

	inline void AddColour(float timeFraction, const Color4F& colour);

	ColourMap& GetTimeAndColour(void)
	{
		return m_colour_map;
	}

	void SetColourMap(ColourMap& colourMap)
	{
		m_colour_map = colourMap;
	}

	void ClearColourMap (void)
	{
		m_colour_map.clear(); 
	}

	virtual bool IsFitParticle(CCParticle* particle);

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

private:
	void AddColour(const std::string& value);

protected:
	ColourMap m_colour_map;
};


class  CCParticleColourAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleColourAffectorFactory() {}
	virtual ~CCParticleColourAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Colour";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END
#endif