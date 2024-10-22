
#include "particle.h"
#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"

NS_CC_BEGIN

void CCParticle::SetEnabled(bool enable)
{
	m_enabled = enable;
}

void CCParticle::CopyAttributesTo(CCParticle* particle)
{
	particle->m_type = m_type;
	particle->m_position = m_position;
	particle->m_direction = m_direction;
	particle->m_time_live = m_time_live;
	particle->m_total_live = m_total_live;
	particle->m_color = m_color;
	particle->m_parent_emitter = m_parent_emitter;
	particle->m_angle = m_angle;
	particle->m_ZOrder = m_ZOrder;
}

NS_CC_END