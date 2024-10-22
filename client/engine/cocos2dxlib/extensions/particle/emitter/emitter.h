#ifndef EMITTER_H
#define EMITTER_H

#include "particle/particle.h"
#include "particle/attribute.h"

#include "cocos2d.h"
#include <string>


NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;

class CCParticleEmitter : public CCParticle  , public CCParticleAttribute
{
public:
	static const float DEFAULT_WIDTH;
	static const float DEFAULT_HEIGHT;
	static const float DEFAULT_DEPTH;
	static const Vec3 DEFAULT_POSITION;
	static const Vec3 DEFAULT_DIRECTION;
	static const float DEFAULT_VELOCITY;
	static const float DEFAULT_TIME_LIVE;
	static const float DEFAULT_EMISSION_RATE;
	static const bool DEFAULT_FORCE_EMIT;
	static const bool DEFAULT_PARTICLES_LIVE_FOREVER;
	static const Color4F DEFAULT_COLOR;
	static const float DEFAULT_ANGLE;
	static const bool DEFAULT_CYCLE;

	CCParticleEmitter(CCParticleTechnique* pt);

	virtual ~CCParticleEmitter();

	const std::string& GetEmitterType() const
	{
		return m_emitter_type;
	}

	void SetEmitterType(const std::string& type)
	{
		m_emitter_type = type;
	}

	const std::string& GetName() const
	{
		return m_name;
	}

	void SetName(const std::string& name)
	{
		m_name = name;
	}

	const Vec3& getRelativePosition() const
	{
		return m_relative_position;
	}

	void SetRelativePosition(const Vec3& pos)
	{
		m_relative_position = pos;
	}

	const float GetEmissionRate() const
	{
		return m_emission_rate;
	}

	void SetEmissionRate(float rate)
	{
		m_emission_rate = rate;
	}

	const Color4F& GetStartColor() const
	{
		return m_start_colour;
	}

	const Color4F& GetEndColor() const
	{
		return m_end_colour;
	}

	void SetColor(const Color4F& startColor, const Color4F& endColor)
	{
		m_start_colour = startColor;
		m_end_colour = endColor;
	}

	inline DynamicAttribute* GetDynAngle(void) const
	{
		return m_dyn_angle;
	}

	void SetDynAngle(DynamicAttribute* dynAngle);

	DynamicAttribute* GetDynWidth(void) const
	{
		return m_dyn_width;
	}

	void SetDynWidth(DynamicAttribute* dynWidth);

	DynamicAttribute* GetDynHeight(void) const
	{
		return m_dyn_height;
	}

	void SetDynHeight(DynamicAttribute* dynHeight);

	DynamicAttribute* GetDynDepth(void) const
	{
		return m_dyn_depth;
	}

	void SetDynDepth(DynamicAttribute* dynDepth);

	DynamicAttribute* GetDynVelocity(void) const
	{
		return m_dyn_velocity;
	}

	void SetDynVelocity(DynamicAttribute* dynVelocity);

	DynamicAttribute* GetDynLiveTime(void) const
	{
		return m_dyn_plt;
	}

	void SetDynLiveTime(DynamicAttribute* dynLiveTime);

	void SetDynSize(DynamicAttribute* dyn_size);

	bool IsEmitted() const
	{
		return m_emitted;
	}

	void SetEmitted(bool emitted)
	{
		m_emitted = emitted;
	}

	// ��ȡ���䵥Ԫ����
	const std::string& GetEmittedName() const
	{
		return m_emitted_name;
	}

	// ���÷��䵥Ԫ����
	void SetEmittedName(const std::string& name)
	{
		m_emitted_name = name;
	}

	unsigned int GetLastCreateCount() const
	{
		return m_last_count;
	}

	void SetCycleTime(float startTime, float endTime);

	CCParticleTechnique* GetParent() { return m_technique; }
	void SetParent(CCParticleTechnique* pt) { m_technique = pt; }

	bool IsForceEmit() const 
	{
		return m_force_emit;
	}

	bool IsLiveForever() const
	{
		return m_particles_live_forever;
	}

	bool IsUseAllSize() const 
	{
		return m_use_all_size;
	}

	void SetUseAllSize(bool is_use)
	{
		m_use_all_size = is_use;
	}

	void GenerateAngle(float& angle);


	virtual void InitParticle(CCParticle* particle);

	virtual void InitParticlePos(CCParticle* particle);

	virtual void InitParticleColor(CCParticle* particle);

	virtual void InitParticleSpeed(CCParticle* particle);

	virtual void InitParticleDirection(CCParticle* particle);

	virtual void InitParticleLiveTime(CCParticle* particle);

	virtual void InitParticleDimensions(CCParticle* particle);

	unsigned short GetEmissionCount(float timeElapsed, bool is_only_update_emitter_time);

	virtual void CopyAttributesTo(CCParticleEmitter* pe);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	void ResetData();

public:

	//���ӷ�����λ��
	Vec3 m_particle_direction;
	Vec3 m_up;

	//����ڸ�����ϵͳ�����λ��
	Vec3 m_relative_position;

	float m_emission_rate;

	//һ�����ڵĿ�ʼʱ��ͽ���ʱ�䣬��ʱ��Ϊ�ٷֱ�
	float m_start_time;
	float m_end_time;

	//�Ƿ�֧��ѭ��
	bool m_cycle;

	// �Ƿ�ǿ��һ���Է���
	bool m_force_emit;

	// ���ɵ������Ƿ����ô���
	bool m_particles_live_forever;

	//ʣ�෢��������Ŀ
	float m_remainder;

	//һ�������Ѿ������ʱ��
	float m_emitter_time;

	Color4F m_start_colour;
	Color4F m_end_colour;

protected:
	std::string m_emitter_type;
	std::string m_name;

	unsigned int m_last_count;

	//���������䲻��Visual������
	std::string m_emitted_name;		// ���䵥Ԫ����
	bool m_emitted;
	bool m_use_all_size;

	//��ֵ̬
	DynamicAttribute* m_dyn_plt;
	DynamicAttribute* m_dyn_angle;
	DynamicAttribute* m_dyn_width;
	DynamicAttribute* m_dyn_height;
	DynamicAttribute* m_dyn_depth;
	DynamicAttribute* m_dyn_size;
	DynamicAttribute* m_dyn_velocity;

	CCParticleTechnique* m_technique;
};

NS_CC_END

#endif