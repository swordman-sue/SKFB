#ifndef RENDERER_RIBBON_H
#define RENDERER_RIBBON_H

#include "particle/particlesystem.h"
#include "particle/technique.h"
#include "renderer.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <vector>
#include <list>
#include <map>
using namespace std;

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class CCParticleRenderBuffer;
class MyParticleSystem;

enum RibbonUVDirection
{
	RUVD_U ,
	RUVD_V
};

class CCRibbonParticleRenderer : public CCParticleRenderer
{
public:
	static const float DEFAULT_WIDTH;
	static const float DEFAULT_CHANGE_WIDTH;
	static const float DEFAULT_TRAIL_LEN;
	static const Color4F DEFAULT_COLOR;
	static const Color4F DEFAULT_CHANGE_COLOR;
	static const size_t DEFAULT_MAX_ELEMENT;
	static const RibbonUVDirection DEFAULT_UV_DIR;
	static const size_t SEGMENT_EMPTY;

public:
	struct RibbonElement
	{
		Color4F color;
		Vec3 pos;
		Vec2 uv0;
		Vec2 uv1;
		float width;
	};

	typedef std::vector<RibbonElement*> ElementList;

	struct ChainSegment
	{
		size_t start;
		size_t head;
		size_t tail;
		Vec3 posR;
	};
	typedef vector<ChainSegment> ChainSegmentList;
	ChainSegmentList m_chain_list;

public:
	CCRibbonParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix);

	virtual ~CCRibbonParticleRenderer();

	virtual void Draw(std::list<CCParticle*>& current_particles);
	virtual void NotifyParticleQuota(size_t size);
	virtual void SetTexture(const std::string& name);
	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
	virtual bool SetAttribute(const char* type, const char* value);
	virtual bool GetAttribute(const char* type, char* value, size_t size);
	virtual void CopyAttributesTo(CCParticleRenderer *render);
	virtual void ParticleExpired(CCParticle* particle);
	virtual void SetMaterialType(MaterialType type);
	virtual void MakeGrey(bool is_grey);
	virtual void ResetData();
	virtual bool FadeOutParticle(CCParticle* particle, float timeElapsed);

	inline void SetWidth(float w);
	inline float GetWidth();
	inline void SetTrailLength(float life);
	inline float GetTrailLength();
	inline void SetColour(const Color4F& color);
	inline const Color4F& GetColour();
	inline void SetMaxElement(size_t max_num);
	inline size_t GetMaxElement();
	inline void SetChangeColour(const Color4F& color);
	inline const Color4F& GetChangeColour();
	inline void SetChangeWidth(float w);
	inline const float GetChangeWidth();
	inline void SetRibbonUVDirection(RibbonUVDirection uv_dir);
	inline RibbonUVDirection GetRibbonUVDirection();

	void Clear();

private:
	size_t GetNumChainElements(size_t chainIndex) const;
	void AddChainElement(size_t chainIndex, const Vec3& pos);
	bool RemoveChainElement(size_t chainIndex);
	void RemoveCountChian(size_t chainIndex, int count);
	void UpdateAllTrail();
	void UpdateUV();
	void UpdateColour(float t);
	void UpdateElementCount();
	bool UpdateBuffer();
	bool UpdateData(float t);
	void ClearChain(size_t chainIndex);
	void UpdateTrail(size_t index);
	void ClearAllNodeVisible();
	int FindFirstNoneVisible();
	bool IsReduceChainLength(int index);
	void ReduceChainLength(int index);

protected:
	size_t m_max_element_count;	

	RibbonUVDirection m_uv_dir;
	Color4F m_color;
	Color4F m_change_color;
	ElementList m_element_list;

	size_t m_max_element;
	size_t m_chain_count;

	float m_trail_length;
	float m_width;
	float m_change_width;
	float m_elem_length;

	bool m_dirty_element;

	map<size_t, Vec3> m_pos_map;

	size_t m_max_vertex_count;
	size_t m_max_index_count;

	size_t m_num_indices;
	size_t m_num_vertex;
	bool m_speed_is_zero;//速度为0时，拖尾随着粒子系统位置改变轨迹
	Vec3 m_posR;
	CCParticleRenderBuffer* m_buffer_data;
};


class  CCRibbonParticleRendererFactory : public CCParticleRendererFactory
{
public:
	CCRibbonParticleRendererFactory():CCParticleRendererFactory("RibbonTrail"){ }
	virtual ~CCRibbonParticleRendererFactory() {}

	virtual CCParticleRenderer* CreateRenderer(CCParticleTechnique *pt, const std::string& texture_suffix)
	{
		CCRibbonParticleRenderer *ribbon_renderer = new CCRibbonParticleRenderer(pt, texture_suffix);
		if (ribbon_renderer)
		{
			ribbon_renderer->autorelease();
			return ribbon_renderer;
		}
		CC_SAFE_DELETE(ribbon_renderer);
		return NULL;
	}
};

NS_CC_END

#endif