#ifndef EMITTER_POSITION_H
#define EMITTER_POSITION_H

#include "math/CCMath.h"

#include "emitter.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"

#include <vector>
#include <string>
using namespace std;


NS_CC_BEGIN

class CCParticleTechnique;
class CCParticleEmitter;
class CCParticle;

class CCParticlePositionEmitter : public CCParticleEmitter
{
public:
	static const bool DEFAULT_RANDOM;

	public:

		CCParticlePositionEmitter(CCParticleTechnique* pt);

	    virtual ~CCParticlePositionEmitter(void) {}

		const vector<Vec3>& GetPositions(void) const
		{
			return m_position_list;
		}

		/** Add a new position to this emitter
	    */
		void AddPosition(const Vec3& position)
		{
			m_position_list.push_back(position);
		}

		/** Remove all positions from this emitter
	    */
		void RemoveAllPositions(void)
		{
			m_position_list.clear();
		}

		virtual void InitParticlePos(CCParticle* particle);

		/** 
	    */
		virtual void CopyAttributesTo (CCParticleEmitter* emitter);

		virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

		virtual bool SetAttribute(const char* type, const char* value);

		virtual bool GetAttribute(const char* type, char* value, size_t size);

		void SetRandom(bool is_random)
		{
			m_random = is_random;
		}

		bool GetRandom() const
		{
			return m_random;
		}

protected:
	vector<Vec3> m_position_list;
	bool m_random;
	size_t m_index;
};

class CCParticlePositionEmitterFactory : public CCParticleEmitterFactory
{
public:
	CCParticlePositionEmitterFactory() {}

	virtual ~CCParticlePositionEmitterFactory() {}

	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
	virtual std::string GetType() const
	{
		return "Position";
	}

	/** Creates a new emitter instance.
	@remarks
	The subclass MUST add a pointer to the created instance to mEmitters.
	*/
	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};

NS_CC_END

#endif