#ifndef PARTICLE_H
#define PARTICLE_H

#include "cocos2d.h"

NS_CC_BEGIN

class CCParticleEmitter;

class CCParticle
{
public:
	enum ParticleType
	{
		Visual,
		Emitter,
	};

	ParticleType m_type;
	Vec3 m_position;
	Vec3 m_direction;
	float m_time_live;
	float m_total_live;
	bool m_live_forever;
	float m_angle;
	float m_pre_angle;				//������һ�εĽǶ�ֵ������Զ�������Ա���תӰ�������
	Color4F m_color;
	bool m_enabled;
	float m_width;
	float m_height;
	float m_depth;
	int m_ZOrder;		//����������Ⱦ����ֵԽ������Ⱦ������
	CCParticleEmitter *m_parent_emitter;

	float m_left;
	float m_top;
	float m_right;
	float m_bottom;
	float m_rotation_random_value;	//��תӰ�������ֵ��ÿ�����Ӳ�ͬ

	//���ڴ��е�����
	int m_index;

	int	m_start_frame;		// ���Ŷ�������ʼ֡��ֻ��ParticleTexAnimAffector�Ż��޸�����

	CCParticle()
		: m_type(Visual),m_time_live(10.0), m_total_live(10.0), m_angle(0.0f),
		m_enabled(true), m_parent_emitter(0), m_left(0.0), m_top(0.0), m_right(1.0), m_bottom(1.0)
		, m_start_frame( -1) , m_live_forever( false), m_width(0.0), m_height(0.0), m_depth(0.0)
		, m_index(0), m_pre_angle(0.0),m_ZOrder(0)
	{
	}

	bool IsEnabled() const
	{
		return m_enabled;
	}

	virtual void SetEnabled(bool enable);

	virtual void CopyAttributesTo(CCParticle* particle);
};

NS_CC_END

#endif
