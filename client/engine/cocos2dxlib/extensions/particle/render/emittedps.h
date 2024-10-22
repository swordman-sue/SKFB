#ifndef RENDERER_EmittedPS_H
#define RENDERER_EmittedPS_H

#include "renderer.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <vector>
using namespace std;

NS_CC_BEGIN

class DynamicAttribute;
class CCParticle;
class MyParticleSystem;
class CCParticleTechnique;

class  CCEmittedPSParticleRenderer : public CCParticleRenderer
{
public:
	CCEmittedPSParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix);
	virtual ~CCEmittedPSParticleRenderer();

	virtual void UpdateFromTechnique(std::list<CCParticle*>& current_particles, float time_elapsed);

	virtual void Draw(std::list<CCParticle*>& current_particles);

	virtual void NotifyParticleQuota(size_t size);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual void ParticleExpired(CCParticle* particle);

	int FindFirstNoneVisible();

	virtual bool SetAttribute(const char* type, const char* value);
	virtual bool GetAttribute(const char* type, char* value, size_t size);
	virtual void CopyAttributesTo(CCParticleRenderer *render);

	virtual bool CanRender();

	virtual void MakeGrey(bool is_grey);

	const string& GetEmittedPSName() const
	{
		return m_ps_template;
	}

	virtual void ResetData();

	void SetPSTemplate(const string& name);
	void SetIsFaceDiretion(bool is_face);

private:
	void Clear();

protected:
	vector<MyParticleSystem*> m_ps_list;
	std::string m_ps_template;
	int m_last_index;
	bool m_can_emitted;
	bool m_is_face_to_direction;
	bool m_is_grey;
};


class  CCEmittedPSParticleRendererFactory : public CCParticleRendererFactory
{
public:
	CCEmittedPSParticleRendererFactory():CCParticleRendererFactory("EmittedPS") {}
	virtual ~CCEmittedPSParticleRendererFactory() {}

	virtual CCParticleRenderer* CreateRenderer(CCParticleTechnique *pt, const std::string& texture_suffix)
	{
		CCEmittedPSParticleRenderer *emitted_renderer = new CCEmittedPSParticleRenderer(pt, texture_suffix);
		if (emitted_renderer)
		{
			emitted_renderer->autorelease();
			return emitted_renderer;
		}
		CC_SAFE_DELETE(emitted_renderer);
		return NULL;
	}
};

NS_CC_END

#endif