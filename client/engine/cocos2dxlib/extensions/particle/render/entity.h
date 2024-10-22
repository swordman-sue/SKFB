#ifndef RENDERER_ENTITY_H
#define RENDERER_ENTITY_H

#include "renderer.h"
#include "particle/particlefactory.h"
#include "particle/simplespline.h"
#include "cocos2d.h"
#include <vector>
#include <list>
#include <string>

NS_CC_BEGIN

class DynamicAttribute;
class CCParticle;
class CCEnt3D;
class CCParticleTechnique;

class  CCEntityParticleRenderer : public CCParticleRenderer
{
public:
	struct Entity
	{
		CCEnt3D* ent;
		float u_offset;
		float v_offset;
		bool visible;

		Entity() :
			ent(0),
			visible(false),
			u_offset(0),
			v_offset(0)
		{
		}
	};

public:
	static const Vec3 DEFAULT_AXIS;

	CCEntityParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix);
	virtual ~CCEntityParticleRenderer();

	virtual void NotifyParticleQuota(size_t size);
	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
	virtual void Draw(std::list<CCParticle*>& current_particles);
	virtual void ParticleExpired(CCParticle* particle);

	virtual void CopyAttributesTo(CCParticleRenderer *render);
	virtual void SetTexture(const std::string& name);
	virtual void SetMaterialType(MaterialType type);
	virtual bool SetAttribute(const char* type, const char* value);
	virtual bool GetAttribute(const char* type, char* value, size_t size);
	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn);
	virtual void MakeGrey(bool is_grey);

	virtual void UpdateFromTechnique(std::list<CCParticle*>& current_particles, float time_elapsed);
	virtual void ResetData();

	const std::string& GetMeshName() const { return m_mesh_name; }

	void SetTexture(Texture2D* texture);
private:
	void Init();
	void Clear();

	int FindFirstNoneVisible();
	bool CanRender();

	void UpdateTexture();
	void UpdateMaterialType();
	void UpdateMeshUVOffset(int index, float time);
	void UpdateMeshUVOffset(size_t index, float t, float time_elapsed);
	void UpdateAnimData();
	void UpdateTexWrap();

	void SetMesh(const std::string& name);
	void SetTextureScrollSpeedU(DynamicAttribute* dyn);
	void SetTextureScrollSpeedV(DynamicAttribute* dyn);
	void SetOffsetU(float u);
	void SetOffsetV(float v);
	void SetScaleU(float scale_u);
	void SetScaleV(float scale_v);
	void SetAngle(int angle);
	void SetULoop(bool loop);
	void SetVLoop(bool loop);
	void SetAxis(const Vec3& vec);
	void SetAnimName(const std::string& name);
	void SetAnimLoop(bool loop);
	void SetIsClipping(bool isclipping);

	static void OnPreparedCompleted(void* resource);
	void OnTextureCacheCallBack(Texture2D* texture);
protected:
	std::string m_mesh_name;
	std::string m_mesh_full_name;
	std::vector<Entity*> m_entity_list;

	DynamicAttribute* m_scroll_speedu;		//U方向速度
	DynamicAttribute* m_scroll_speedv;		//V方向速度
	bool m_u_cycle;
	bool m_v_cycle;

	float m_offset_u;						//U偏移
	float m_offset_v;						//V偏移

	float m_scale_u;						//U方向缩放
	float m_scale_v;						//V方向缩放

	int m_angle;							//纹理旋转角度

	int m_last_index;
	Vec3 m_axis;

	std::string m_anim_name;
	bool m_action_loop;
	bool m_anim_dirty;

	Texture2D* m_pTexture;
};


class  CCEntityParticleRendererFactory : public CCParticleRendererFactory
{
public:
	CCEntityParticleRendererFactory():CCParticleRendererFactory("Entity") {}
	virtual ~CCEntityParticleRendererFactory() {}

	virtual CCParticleRenderer* CreateRenderer(CCParticleTechnique *pt, const std::string& texture_suffix)
	{
		CCEntityParticleRenderer *entity_renderer = new CCEntityParticleRenderer(pt, texture_suffix);
		if (entity_renderer)
		{
			entity_renderer->autorelease();
			return entity_renderer;
		}
		CC_SAFE_DELETE(entity_renderer);
		return NULL;
	}
};

NS_CC_END

#endif