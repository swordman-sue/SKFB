#ifndef RENDERER_BILLBOARD_H
#define RENDERER_BILLBOARD_H

#include "renderer.h"
#include "particle/particlefactory.h"

#include "cocos2d.h"
#include <string>

NS_CC_BEGIN


enum BillboardOrigin
{
	BBO_TOP_LEFT,
	BBO_TOP_CENTER,
	BBO_TOP_RIGHT,
	BBO_CENTER_LEFT,
	BBO_CENTER,
	BBO_CENTER_RIGHT,
	BBO_BOTTOM_LEFT,
	BBO_BOTTOM_CENTER,
	BBO_BOTTOM_RIGHT
};
/** The rotation type of billboard. */
enum BillboardRotationType
{
	/// Rotate the billboard's vertices around their facing direction
	BBR_VERTEX,
	/// Rotate the billboard's texture coordinates
	BBR_TEXCOORD
};
/** The type of billboard to use. */
enum BillboardType
{
	/// Standard point billboard (default), always faces the camera completely and is always upright
	BBT_POINT,					// 指向公告板：总是完全面向相机，并始终直立
	/// Billboards are oriented around a shared direction vector (used as Y axis) and only rotate around this to face the camera
	BBT_ORIENTED_COMMON,		// 公共变化导向公告板：绕着一个公共Y轴旋转
	/// Billboards are oriented around their own direction vector (their own Y axis) and only rotate around this to face the camera
	BBT_ORIENTED_SELF,			// 自变化导向公告板：绕着自身的Y轴旋转
	/// Billboards are perpendicular to a shared direction vector (used as Z axis, the facing direction) and X, Y axis are determined by a shared up-vertor
	BBT_PERPENDICULAR_COMMON,	// 公共变化垂直公告板
	/// Billboards are perpendicular to their own direction vector (their own Z axis, the facing direction) and X, Y axis are determined by a shared up-vertor
	BBT_PERPENDICULAR_SELF,		// 自变化垂直公告板
};

class CCParticleTechnique;
class CCParticleRenderBuffer;
class R3DBillboardset;


class CCBillboardParticleRenderer : public CCParticleRenderer
{
public:
	static const std::string DEFAULT_TYPE;
	static const std::string DEFAULT_ORIGIN;
	static const std::string DEFAULT_ROT;

	CCBillboardParticleRenderer(CCParticleTechnique *pt, const std::string& texture_suffix);

	virtual ~CCBillboardParticleRenderer();

	virtual void NotifyParticleQuota(size_t size);
	virtual void SetTexture(const std::string& name);
	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
	virtual bool SetAttribute(const char* type, const char* value);
	virtual bool GetAttribute(const char* type, char* value, size_t size);
	virtual void CopyAttributesTo(CCParticleRenderer *render);
	virtual void SetMaterialType(MaterialType type);
	virtual void MakeGrey(bool is_grey);

	bool PreDraw();
	virtual void Draw(std::list<CCParticle*>& current_particles);

protected:
	void SetOrigionType(const std::string& type);
	void SetRotationType(const std::string& type);
	void SetBillboardType(const std::string& type);
	void SetCommonUpVector(const Vec3& common_up_vec);
	void SetCommonDirection(const Vec3& common_dir);

protected:
	void InjectBillboard( CCParticle* particle);
	void CalculateCurOffsets( float w, float h);
	void CalculateCurAxes( const CCParticle* particle);	// 计算粒子的旋转轴
	bool IsOrientPerBillboard() const;					// 判断当前的billboard_type是否需要每个粒子都精确的校准方向
	bool IsOrientToCamera() const;						// 判断当前的billboard_type是否朝向摄像机
	bool IsAxisAssociateParticleData() const;			// 判断当前billboard_type的轴向是否与具体的billboard有关

protected:

	BillboardType m_billboard_type;

	std::string m_billboard_type_str;
	std::string m_billboard_origin;
	std::string m_billboard_rot_type;

	Vec3 m_common_direction;
	Vec3 m_common_up_vector;

	Vec3 m_result_direction;
	Vec3 m_result_up_vector;

	BillboardOrigin m_origin_type;
	BillboardRotationType m_rotation_type;

	CCParticleRenderBuffer* m_buffer_data;
	Vec3 m_cur_x_dir;
	Vec3 m_cur_y_dir;

	size_t m_num_vertices;
	size_t m_num_indices;

	float		m_cur_left;		
	float		m_cur_right;
	float		m_cur_top;
	float		m_cur_bottom;
};


class CCBillboardParticleRendererFactory : public CCParticleRendererFactory
{
public:
	CCBillboardParticleRendererFactory():CCParticleRendererFactory("Billboard"){ }
	virtual ~CCBillboardParticleRendererFactory() {}

	virtual CCParticleRenderer* CreateRenderer(CCParticleTechnique *pt, const std::string& texture_suffix)
	{
		CCBillboardParticleRenderer *billboard_renderer = new CCBillboardParticleRenderer(pt, texture_suffix);
		if (billboard_renderer)
		{
			billboard_renderer->autorelease();
			return billboard_renderer;
		}
		CC_SAFE_DELETE(billboard_renderer);
		return NULL;
	}
};

NS_CC_END

#endif