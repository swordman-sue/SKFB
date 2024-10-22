#ifndef AFFECTOR_SUCTION_H
#define AFFECTOR_SUCTION_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"

#include <map>
#include <string>

NS_CC_BEGIN

class DynamicAttribute;
class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleSuctionAffector : public CCParticleAffector
{
public:
	static const float DEFAULT_SUCTION;
	static const Vec3 DEFAULT_SUCTION_POS;
	static const bool DEFAULT_COLLIDE_VISIBLE;

	enum SuctionType
	{
		SUC_DIR,
		SUC_POS
	};

	CCParticleSuctionAffector(CCParticleTechnique* pt);

	virtual ~CCParticleSuctionAffector();

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void CopyAttributesTo(CCParticleAffector* pa);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);

	bool IsCollideVisible() { return m_collide_visible; }

	const Vec3& GetSuctionPos() { return m_position; }

	DynamicAttribute* GetDynSuction() { return m_dyn_suction; }

	void SetCollideVisible(bool visible) { m_collide_visible = visible; }

	void SetSuctionPos(const Vec3& pos);

	void SetDynSuction(DynamicAttribute *dyn);

	void SetSucType(SuctionType type)
	{
		m_suc_type = type;
	}

	SuctionType GetSucType() const
	{
		return m_suc_type;
	}

	virtual void PreAffector(float elapsed);
protected:

	Vec3 m_position;
	bool m_collide_visible;					//碰撞是否消失
	DynamicAttribute* m_dyn_suction;

	Vec3 m_derived_pos;
	SuctionType m_suc_type;					//吸力改变类型：位置或方向
};


class CCParticleSuctionAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleSuctionAffectorFactory() {}
	virtual ~CCParticleSuctionAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "Suction";
	}


	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif