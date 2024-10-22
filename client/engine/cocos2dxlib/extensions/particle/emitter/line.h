#ifndef EMITTER_LINE_H
#define EMITTER_LINE_H

#include "emitter.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include "math/CCMath.h"


NS_CC_BEGIN

class CCParticleEmitter;
class CCParticleTechnique;
class CCParticle;

class CCParticleLineEmitter : public CCParticleEmitter
{
public:
	static const bool DEFAULT_RANDOM;
	static const float DEFAULT_INC;

	CCParticleLineEmitter(CCParticleTechnique* pt);

	virtual ~CCParticleLineEmitter() {}

	void SetLinePostion(const Vec3& startPos, const Vec3& dir);

	const Vec3& GetStartPosition() const
	{
		return m_start_point;
	}

	const Vec3& GetEndPosition() const
	{
		return m_end_point;
	}

	const Vec3& GetDirection() const
	{
		return m_line_dir;
	}

	bool IsRandomized() const
	{
		return m_randomized;
	}

	void SetRandomized(bool randomized)
	{
		m_randomized = randomized;
	}

	float GetIncrement()
	{
		return m_increment;
	}

	void SetIncrement(float inc)
	{
		m_increment = inc;
	}

	virtual void InitParticlePos(CCParticle* particle);

	virtual void CopyAttributesTo (CCParticleEmitter* emitter);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

protected:
	Vec3 m_start_point;
	Vec3 m_end_point;
	Vec3 m_line_dir;
	Vec3 m_line_normalize_dir;
	Vec3 m_last_create_pos;
	bool m_randomized;
	float m_increment;
};


class CCParticleLineEmitterFactory : public CCParticleEmitterFactory
{
public:
	CCParticleLineEmitterFactory() {}

	virtual ~CCParticleLineEmitterFactory() {}

	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
	virtual std::string GetType() const
	{
		return "Line";
	}

	/** Creates a new emitter instance.
	@remarks
	The subclass MUST add a pointer to the created instance to mEmitters.
	*/
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};

NS_CC_END

#endif