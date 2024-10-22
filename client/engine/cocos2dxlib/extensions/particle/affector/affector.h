#ifndef AFFECTOR_H
#define AFFECTOR_H

#include "particle/attribute.h"

#include <list>
#include <string>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class CCParticleAttribute;

class CCParticleAffector : public CCParticleAttribute
{
public:
	CCParticleAffector(CCParticleTechnique* pt);

	virtual ~CCParticleAffector() {}

	virtual void EffectParticle(CCParticle *particle, float elapsed) = 0;

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	const std::string& GetType() const
	{
		return m_type;
	}

	void SetType(const std::string& type)
	{
		m_type = type;
	}

	const std::string& GetName() const
	{
		return m_name;
	}

	void SetName(const std::string& name)
	{
		m_name = name;
	}

	void SetStartTime(float t)
	{
		m_start_time = t;
		if (m_start_time < 0.0f)
		{
			m_start_time = 0.0f;
		}

		if (m_start_time > m_end_time)
		{
			m_start_time = m_end_time;
		}
	}

	float GetStartTime()
	{
		return m_start_time;
	}

	void SetEndTime(float t)
	{
		m_end_time = t;
		if (m_end_time > 1.0f)
		{
			m_end_time = 1.0f;
		}

		if (m_start_time > m_end_time)
		{
			m_end_time = m_start_time;
		}
	}

	float GetEndTime()
	{
		return m_end_time;
	}

	void SetFristState(bool s)
	{
		m_first_state = s;
	}

	bool GetFristState()
	{
		return m_first_state;
	}

	void SetParent(CCParticleTechnique* pt);

	CCParticleTechnique* GetParent();

	void AddEmitterToExclude(const std::string& name);

	void RemoveEmitterToExclude(const std::string& name);

	void RemoveAllEmittersToExclude(void)
	{
		m_exclude_emitter.clear();
	}

	const std::list<std::string>& GetEmittersToExclude(void) const
	{
		return m_exclude_emitter;
	}

	void SetEmittersToExclude(std::list<std::string>& exclude)
	{
		m_exclude_emitter = std::list<std::string>(exclude);
	}

	virtual bool IsFitParticle(CCParticle* particle);

	virtual void InitParticle(CCParticle* particle) {EffectParticle(particle,0);}

	virtual void PreAffector(float elapsed) {}

	void ProcessParticles(float elapsed);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	void SetEnable(bool enable)
	{
		m_enabled = enable;
	}

	bool IsEnable()
	{
		return m_enabled;
	}

	virtual size_t GetAffectorWeight() const { return m_affector_weight; }

	virtual void ResetData();

protected:
	std::string m_type;
	std::string m_name;
	std::list<std::string> m_exclude_emitter;
	CCParticleTechnique* m_parent;
	float m_affect_time;
	//一个周期的开始时间和结束时间，此时间为百分比
	float m_start_time;
	float m_end_time;
	bool m_first_state;

	bool m_enabled;
	size_t m_affector_weight;
};

NS_CC_END

#endif