#ifndef RENDERER_H
#define RENDERER_H

#include "particle/attribute.h"
#include "particle/particlehelper.h"
#include "cocos2d.h"

#include <list>
#include <vector>
#include <string>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class DynamicAttribute;

class CCParticleRenderer : public CCParticleAttribute, public cocos2d::Ref
{
public:
	static const MaterialType DEFAULT_MAT_TYPE;
	static const float DEFAULT_RATIO;

	CCParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix) :
		m_technique(pt),
		m_mat_type(DEFAULT_MAT_TYPE),
		m_particle_quota(0),
		m_add_mode_power_ratio(DEFAULT_RATIO),
		m_texture_suffix(texture_suffix)
	{
	}

	virtual ~CCParticleRenderer() {}

	const std::string& GetRendererType() const { return m_type; }
	
	void SetTechniqueParent(CCParticleTechnique *pt){m_technique = pt;}
	CCParticleTechnique* GetTechniqueParent() const{return m_technique;}

	virtual void CopyAttributesTo(CCParticleRenderer *render)
	{
		render->SetTexture(m_texture_name);
		render->SetMaterialType(m_mat_type);
		render->m_add_mode_power_ratio = m_add_mode_power_ratio;
	}

	virtual void Draw(std::list<CCParticle*>& current_particles) { }

	const std::string& GetTextureName() const
	{
		return m_texture_name;
	}

	virtual void SetTexture(const std::string& name);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);
	virtual bool GetAttribute(const char* type, char* value, size_t size);

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn) { return false; }

	virtual void SetMaterialType(MaterialType type) { m_mat_type = type; }

	virtual void ResetData() {}

	// 上层调用普通接口
	virtual void NotifyParticleQuota(size_t size) {  m_particle_quota = size;}

	// update时调用的接口
	virtual bool FadeOutParticle(CCParticle* particle, float timeElapsed) { return false; }
	virtual void ParticleExpired(CCParticle* particle) {};
	virtual void UpdateFromTechnique(std::list<CCParticle*>& current_particles, float time_elapsed) {}
	virtual void MakeGrey(bool is_grey){};

protected:
	std::string m_type;
	CCParticleTechnique *m_technique;

	std::string m_texture_name;
	std::string m_texture_suffix;
	std::string m_texture_full_name;

	MaterialType m_mat_type;
	size_t m_particle_quota;
	float m_add_mode_power_ratio;					//渲染，相加模式系数
};

NS_CC_END
#endif
