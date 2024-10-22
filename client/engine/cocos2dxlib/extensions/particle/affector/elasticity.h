#ifndef AFFECTOR_ELASTICITY_H
#define AFFECTOR_ELASTICITY_H

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

class CCParticleElasticityAffector : public CCParticleAffector
{
public:
	static const float DEFAULT_REVERSE_LIMIT;
	static const float DEFAULT_DISTANCE_FACTOR;
	static const float DEFAULT_REVERSE_FACTOR;
	static const float DEFAULT_START_TIME;
	static const float DEFAULT_OFFSET_RADIUS;

	CCParticleElasticityAffector(CCParticleTechnique* pt);

	virtual ~CCParticleElasticityAffector() {}

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	void SetReverseLimit(float factor)
	{
		m_reverse_limit = factor;
	}

	float GetReverseLimit() const
	{
		return m_reverse_limit;
	}

	void SeDistanceFactor(float distance)
	{
		m_distance_factor = distance;
	}

	float GetDistanceFactor() const
	{
		return m_distance_factor;
	}

	void SetTimeStart(float time_start)
	{
		m_time_start = time_start;
	}

	float GetTimeStart() const
	{
		return m_time_start;
	}

	void SetReverseFactor(float factor)
	{
		m_reverse_factor = factor;
	}

	float GetReverseFactor() const
	{
		return m_reverse_factor;
	}

	void SetOffsetRadius(float radius)
	{
		m_offset_radius = radius;
	}

	float GetOffsetRadius() const
	{
		return m_offset_radius;
	}

protected:
	float m_reverse_limit;				//反向力限制
	float m_distance_factor;			//距离因子
	float m_time_start;					//开始时间
	float m_reverse_factor;				//反向力因子
	float m_offset_radius;				//偏移半径
};


class CCParticleElasticityAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleElasticityAffectorFactory() {}
	virtual ~CCParticleElasticityAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Elasticity";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif