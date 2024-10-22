#ifndef EMITTER_BOX_H
#define EMITTER_BOX_H

#include "emitter.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"

#include "math/CCMath.h"
#include <string>

NS_CC_BEGIN
 
class CCParticleTechnique;
class CCParticleEmitter;
 
class CCParticleBoxEmitter : public CCParticleEmitter
{
protected:
 	/// Size of the area
 	Vec3 m_size;
 
 	/// Local axes, not normalised, their magnitude reflects area size
 	Vec3 m_range_x, m_range_y, m_range_z;
 
 	Vec3 m_box_dir;
 	Vec3 m_box_up;
 
public:
 	static const Vec3 DEFAULT_SIZE;
 	static const Vec3 VECTOR3_ZERO;
 	static const Vec3 VECTOR3_UNIT_Z;
 	static const Vec3 VECTOR3_UNIT_Y;
 
 	CCParticleBoxEmitter(CCParticleTechnique* pt);
 
 	virtual ~CCParticleBoxEmitter() {}
 
 	void GenAreaAxes(void);
 
        void SetSize(const Vec3& size)
 	{
 		m_size = size;
 		GenAreaAxes();
 	}
 
        void SetSize(float x, float y, float z)
 	{
 		m_size.x = x;
 		m_size.y = y;
 		m_size.z = z;
 		GenAreaAxes();
 	}
 
        void SetWidth(float width)
 	{
 		m_size.x = width;
 		GenAreaAxes();
 	}
 
        float GetWidth(void) const
 	{
 		return m_size.x;
 	}
 
        void SetHeight(float Height)
 	{
 		m_size.y = Height;
 		GenAreaAxes();
 	}
 
        float GetHeight(void) const
 	{
 		return m_size.y;
 	}
 
        void SetDepth(float Depth)
 	{
 		m_size.z = Depth;
 		GenAreaAxes();
 	}
 
        float GetDepth(void) const
 	{
 		return m_size.z;
 	}
 
 	void SetBoxDir(const Vec3& dir);
 
 	virtual void InitParticlePos(CCParticle* particle);
 
 	virtual void CopyAttributesTo (CCParticleEmitter* emitter);
 
 	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
 
 	virtual bool SetAttribute(const char* type, const char* value);
 
 	virtual bool GetAttribute(const char* type, char* value, size_t size);
};
 
 
class CCParticleBoxEmitterFactory : public CCParticleEmitterFactory
{
public:
 	CCParticleBoxEmitterFactory() {}
 
 	virtual ~CCParticleBoxEmitterFactory() {}
 
 	/** Returns the name of the factory, the name which identifies the particle emitter type this factory creates. */
 	virtual std::string GetType() const
 	{
 		return "Box";
 	}
 
 	/** Creates a new emitter instance.
 	@remarks
 	The subclass MUST add a pointer to the created instance to mEmitters.
 	*/
 	virtual CCParticleEmitter* CreateEmitter(CCParticleTechnique* pt);
};
NS_CC_END
 
#endif