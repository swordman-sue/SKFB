#ifndef DreamEnt3D_h__
#define DreamEnt3D_h__

#include "particle/particlehelper.h"
#include "cocos/base/CCProtocols.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCQuadCommand.h"
#include "renderer/CCCustomCommand.h"
#include <string>

NS_CC_BEGIN

class Texture2D;
class CCMyMesh;
class CCMySkeleton;
class CCResCspriteLoader;
class CCAnimationStateSet;
class SkeletonInstance;
class CCEnt3D;

class CMeshTexListener : public Ref
{
public:
	CMeshTexListener(CCEnt3D* r3d):m_r3d(r3d)
	{
	}
	void AsyncLoad(const char* path);
	void OnPreparedCompleted(Ref* resource);
	void UnUse();

private:
	CCEnt3D* m_r3d;
};

class CCEnt3D : public TextureProtocol
{
public:
	CCEnt3D();
	virtual ~CCEnt3D();

	void SetMesh( const std::string& mesh_file, int type);
	void SetTexture(const std::string& texture);
	void SetMaterialType(MaterialType type);

	virtual void SetBlendColor( const Color4F& clr);

	bool SetAnimationEnable( const std::string& name, bool enable, bool loop = false);

    void Draw(float add_mode_power);

	// 升级到3.2 add by zhuyi 2014-09-01
	void drawEnt3d(const Mat4 &transform, uint32_t transformFlags, float add_mode_power);
	bool HaveMesh(){ return m_mesh_res!=0;}
	// end

	bool PreDraw(float t);

	void SetTextureOffsetUV(float u, float v);
	void SetTextureScaleUV(float scale_u, float scale_v);
	void SetTextureRotateUV(int angle);
	void SetWrapS(GLint wrap_s) { m_wrap_s = wrap_s; m_dirty_tp = true; }
	void SetWrapT(GLint wrap_t) { m_wrap_t = wrap_t; m_dirty_tp = true; }
	void MakeGrey(bool is_grey, int mat_type);

public:
	 virtual const BlendFunc &getBlendFunc() const;
	virtual Texture2D* getTexture() const;
	virtual void setTexture(Texture2D* var);
	virtual void setBlendFunc(const BlendFunc& var);

private:
	void ReleaseMeshAndSkeleton();
	void SetShaderProgram(GLProgram *pShaderProgram);
	void CreateBufferVBO();
	void InitShaderProgram(int mat_type);
	void UpdateEnabledAnimations( float elapsed_time);

protected:
	Texture2D* m_pTexture;
	BlendFunc m_tBlendFunc;

private:
	Color4F m_blend_color;
	float m_u;
	float m_v;
	float m_scale_u;
	float m_scale_v;
	int m_angle;
	GLfloat m_rotate_matrix[4];

	std::string m_mesh_name;
	CCMyMesh* m_mesh_res;
	std::string m_skeleton_name;
	CCMySkeleton* m_skeleton_res;

	Mat4* m_bone_matrices;
	unsigned short	m_num_bone_matrices;

	GLProgram* m_pShaderProgram;

	// 升级到3.2 add by zhuyi 2014-08-31
	GLProgramState* _glProgramState;
	CustomCommand _customCommant;
	// end
	
	GLuint m_pBuffersVBO[2];
	GLint m_nUniformColor;
	GLint m_nUniformOffsetUV;
	GLint m_nUniformScaleUV;
	GLint m_nUniformRotateUV;
	GLint m_uAddModePower;
	
	bool m_dirty_tp;
	GLint m_wrap_s;
	GLint m_wrap_t;

	SkeletonInstance* m_skeleton_instance;
	CCAnimationStateSet* m_animation_state_set;

	typedef std::map<std::string, bool> EnableAnimMap;	// 当前激活的动画列表
	EnableAnimMap	m_enable_anim_map;

	CMeshTexListener* m_loader;
};

NS_CC_END

#endif