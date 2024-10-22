#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCMyMesh.h"

#include "renderer/ccGLStateCache.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCGLProgram.h"
#include "renderer/CCTexture2D.h"

#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include "CCMySkeleton3D.h"
#include "CCSkeletonInstance3D.h"

#include "CCEnt3D.h"
#include "cocos/base/CCProtocols.h"
#include "CCMeshManager.h"
#include "CCSkeleton3DManager.h"
#include "particle/particlehelper.h"
#include "CCResObjManager.h"
#include <cmath>

NS_CC_BEGIN

typedef struct _ccV3F_T2F_C4B
{
	Vec3		vertices;            // 12 bytes
	Tex2F			texCoords;
	Color4B		colors;                // 4 bytes
} ccV3F_T2F_C4B;

void CMeshTexListener::AsyncLoad(const char* path)
{
	CC_SAFE_RETAIN(this);
	Director::getInstance()->getTextureCache()->addImageAsync(path,CC_CALLBACK_1(CMeshTexListener::OnPreparedCompleted ,this));
}
void CMeshTexListener::OnPreparedCompleted(Ref* resource)
{
	if (resource!=0)
	{
		Texture2D* tex = (Texture2D*)resource;
		if (tex != 0 && m_r3d != 0)
		{
			m_r3d->setTexture(tex);
		}
	}
	CC_SAFE_RELEASE(this);
}
void CMeshTexListener::UnUse()
{
	m_r3d = 0;
}

CCEnt3D::CCEnt3D():
	m_mesh_res(0),
	m_skeleton_res(0),
	m_pShaderProgram(0),
	m_nUniformColor(-1),
	m_nUniformOffsetUV(-1),
	m_nUniformScaleUV(-1),
	m_nUniformRotateUV(-1),
	m_uAddModePower(-1),
	m_skeleton_instance(0),
	m_animation_state_set(0),
	m_u(0.0f),
	m_v(0.0f),
	m_scale_u(1.0f),
	m_scale_v(1.0f),
	m_num_bone_matrices(0),
	m_bone_matrices(0),
	m_dirty_tp(true),
	m_wrap_s(GL_REPEAT),
	m_wrap_t(GL_REPEAT),
	m_pTexture(0),
	m_loader(0),
	m_angle(0)
{
	m_loader = new CMeshTexListener(this);
	m_loader->autorelease();
	m_loader->retain();

	memset(m_pBuffersVBO, 0, sizeof(m_pBuffersVBO));
	m_blend_color.r = m_blend_color.g = m_blend_color.b = m_blend_color.a = 1.0f;
	m_rotate_matrix[0] = m_rotate_matrix[3] = 1.0f;
	m_rotate_matrix[1] = m_rotate_matrix[2] = 0.0f;
}

CCEnt3D::~CCEnt3D()
{
	m_loader->UnUse();
	m_loader->release();
	m_loader=0;

	if (m_pBuffersVBO[0] != 0)
	{
		glDeleteBuffers(2, &m_pBuffersVBO[0]);
	}

	ReleaseMeshAndSkeleton();

	CC_SAFE_RELEASE(m_pShaderProgram);

	if( m_animation_state_set != 0)
	{
		delete m_animation_state_set;
		m_animation_state_set = 0;
	}

	if (m_bone_matrices != 0)
	{
		free(m_bone_matrices);
		m_bone_matrices = 0;
	}
	CC_SAFE_RELEASE(m_pTexture);
	m_pTexture = NULL;
}

void CCEnt3D::setTexture(Texture2D* var)
{
	if (m_pTexture != var)
	{
		CC_SAFE_RELEASE(m_pTexture);
		m_pTexture = var;
		CC_SAFE_RETAIN(m_pTexture);
	}
}

Texture2D * CCEnt3D::getTexture() const
{
	return m_pTexture;
}

 const BlendFunc& CCEnt3D::getBlendFunc()  const
{
	return m_tBlendFunc;
}

 void CCEnt3D::setBlendFunc(const BlendFunc& blendFunc)
{
	if( m_tBlendFunc.src != blendFunc.src || m_tBlendFunc.dst != blendFunc.dst )
	{
		m_tBlendFunc = blendFunc;
	}
}

void CCEnt3D::SetMesh(const std::string& mesh_file, int type)
{
	ReleaseMeshAndSkeleton();

	m_mesh_name = mesh_file;
	m_mesh_res = (CCMyMesh*)CCMeshManager::sharedCCMeshManger()->Create(mesh_file);
	m_mesh_res->retain();

	if( m_mesh_res->HasSkeleton())
	{
		m_skeleton_res = (CCMySkeleton*)CCSkeleton3DManager::sharedCCSkeletonManger()->Create(m_mesh_res->GetSkeletonName());
		m_skeleton_res->retain();
		m_animation_state_set = new CCAnimationStateSet();
		m_skeleton_res->_initAnimationState( m_animation_state_set);

		m_skeleton_instance = new SkeletonInstance(m_skeleton_res, m_mesh_res);
		m_num_bone_matrices = m_skeleton_instance->getNumBones();

		m_bone_matrices = (Mat4*)malloc(m_num_bone_matrices * sizeof(Mat4));

		// 因为资源是异步加载，可能在加载完成前就设置过动画，所以在这里激活一次
		if( m_enable_anim_map.size() > 0)
		{
			EnableAnimMap::iterator it_end = m_enable_anim_map.end();
			for( EnableAnimMap::iterator it = m_enable_anim_map.begin(); 
				it != it_end; ++it)
			{
				const std::string& name = it->first;
				CCAnimationState* anim = m_animation_state_set->getAnimationState( name);
				if( anim != 0)
				{
					anim->setEnabled( true);
					anim->setLoop(it->second);
				}
			}
		}
	}

	CreateBufferVBO();
	InitShaderProgram(type);
}


void CCEnt3D::SetTexture(const std::string& texture )
{
	m_loader->AsyncLoad(texture.c_str());
}

void CCEnt3D::SetTextureOffsetUV(float u, float v)
{
	m_u = u;
	m_v = v;
}

void CCEnt3D::SetTextureScaleUV(float scale_u, float scale_v)
{
	m_scale_u = scale_u;
	m_scale_v = scale_v;
}

void CCEnt3D::SetTextureRotateUV(int angle)
{
	if (m_angle == angle)
	{
		return;
	}
	angle = angle % 360;
	float temp = angle * 3.1415 / 180;
	float cos_value = cos(temp);
	float sin_value = sin(temp);
	m_rotate_matrix[0] = m_rotate_matrix[3] = cos_value;
	m_rotate_matrix[1] = -sin_value;
	m_rotate_matrix[2] = sin_value;
}

void CCEnt3D::MakeGrey(bool is_grey, int mat_type)
{
	if (is_grey)
	{
		if (m_mesh_res != 0)
		{
			BlendFunc blendFunc;
			blendFunc.src = GL_SRC_ALPHA;
			blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

			if (m_mesh_res->HasDiffuse())
			{
				SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLEND_GREY));
			}
			else
			{
				SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLENDNC_GREY));
			}
			setBlendFunc(blendFunc);
		}
	}
	else
	{
		InitShaderProgram(mat_type);
	}
}

void CCEnt3D::SetBlendColor( const Color4F& clr )
{
	m_blend_color = clr;
}

void CCEnt3D::SetMaterialType(MaterialType type)
{
	InitShaderProgram(type);
}

// 升级到3.2 add by zhuyi 2014-09-01
void CCEnt3D::drawEnt3d(const Mat4 &transform, uint32_t transformFlags, float add_mode_power)
{
	if (m_pShaderProgram == 0 || m_pBuffersVBO[0] == 0 || m_mesh_res == 0)
	{
		return;
	}

	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, director->Get2dProjectMat());

	m_pShaderProgram->use();
	m_pShaderProgram->setUniformsForBuiltins(transform);

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	if (m_nUniformOffsetUV != -1)
	{
		m_pShaderProgram->setUniformLocationWith2f(m_nUniformOffsetUV, m_u, m_v);
	}
	if (m_nUniformColor != -1)
	{
		m_pShaderProgram->setUniformLocationWith4f(m_nUniformColor, m_blend_color.r, m_blend_color.g, m_blend_color.b, m_blend_color.a);
	}
	if (m_nUniformScaleUV != -1)
	{
		m_pShaderProgram->setUniformLocationWith2f(m_nUniformScaleUV, m_scale_u, m_scale_v);
	}
	if (m_nUniformRotateUV != -1)
	{
		m_pShaderProgram->setUniformLocationWithMatrix2fv(m_nUniformRotateUV, m_rotate_matrix, 1);
	}
	if (m_uAddModePower != -1)
	{
		m_pShaderProgram->setUniformLocationWith1f(m_uAddModePower, add_mode_power);
	}

	GL::enableVertexAttribs(transformFlags); 

	if (m_pTexture != 0)
	{
		GL::bindTexture2D( m_pTexture->getName());
		if (m_dirty_tp)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  m_wrap_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_t);
			m_dirty_tp = false;
 		}
	}

	GL::blendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );

	size_t len = (size_t)m_mesh_res->GetVertexSize();
	glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
	if (m_mesh_res->HasSkeleton() && m_skeleton_instance != 0)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_skeleton_instance->GetBufferSize(), m_skeleton_instance->GetBuffer());
	}
	// vertices
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, len, (GLvoid*) offsetof( ccV3F_T2F_C4B, vertices));
	// tex coords
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, len, (GLvoid*) offsetof( ccV3F_T2F_C4B, texCoords));
	if (m_mesh_res->HasDiffuse())
	{
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, len, (GLvoid*) offsetof( ccV3F_T2F_C4B, colors));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
	glDrawElements(GL_TRIANGLES, (GLsizei) m_mesh_res->GetIndexCount(), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CC_INCREMENT_GL_DRAWS(1);
	CHECK_GL_ERROR_DEBUG();
}

void CCEnt3D::Draw(float add_mode_power)
{
	unsigned int flags = GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD;
	if (m_mesh_res->HasDiffuse())
	{
		flags |= GL::VERTEX_ATTRIB_FLAG_COLOR;
	}

	Mat4 mat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_customCommant.init(0);
	_customCommant.func = CC_CALLBACK_0(CCEnt3D::drawEnt3d,this,mat,flags, add_mode_power);
	Director::getInstance()->getRenderer()->addCommand(&_customCommant);
}
//end

bool CCEnt3D::SetAnimationEnable( const std::string& name, bool enable, bool loop)
{
	if( enable)
	{
		CCAnimationState* anim = 0;
		if( m_animation_state_set != 0 )
		{
			anim = m_animation_state_set->getAnimationState( name);
			if( anim != 0)
			{
				anim->setEnabled( true);
				anim->setLoop(loop);
			}
		}
		m_enable_anim_map[name] = loop;
		return true;
	}
	else
	{
		EnableAnimMap::iterator it = m_enable_anim_map.find(name);
		if( it != m_enable_anim_map.end())
		{
			if (m_animation_state_set != 0)
			{
				CCAnimationState* anim = m_animation_state_set->getAnimationState( name);
				if (anim != 0)
				{
					anim->setEnabled(false);
					anim->setLoop(loop);
				}
			}
			m_enable_anim_map.erase( it);
			return true;
		}
	}
	return false;
}

void CCEnt3D::ReleaseMeshAndSkeleton()
{
	if (m_mesh_res != 0)
	{
		m_mesh_res->release();
		m_mesh_res = 0;
	}

	if (m_skeleton_res != 0)
	{
		m_skeleton_res->release();
		m_skeleton_res = 0;
	}

	if( m_skeleton_instance != 0)
	{
		delete m_skeleton_instance;
		m_skeleton_instance = 0;
	}
}

void CCEnt3D::SetShaderProgram(GLProgram *pShaderProgram)
{
	if (m_pShaderProgram != pShaderProgram)
	{
		CC_SAFE_RELEASE(m_pShaderProgram);
		m_pShaderProgram = pShaderProgram;
		CC_SAFE_RETAIN(m_pShaderProgram);

		m_nUniformColor = glGetUniformLocation( m_pShaderProgram->getProgram(), "CC_blend_color");
		m_nUniformRotateUV = glGetUniformLocation(  m_pShaderProgram->getProgram(), "uv_rotate");
		m_nUniformOffsetUV = glGetUniformLocation( m_pShaderProgram->getProgram(), "uv_offset");
		m_nUniformScaleUV = glGetUniformLocation(  m_pShaderProgram->getProgram(), "uv_scale");
		m_uAddModePower = glGetUniformLocation(  m_pShaderProgram->getProgram(), "cc_mansual_alpha");
	}
}

void CCEnt3D::CreateBufferVBO()
{
	if (m_mesh_res == 0 || m_mesh_res->GetVertexBufferSize() == 0 || m_mesh_res->GetIndexBufferSize() == 0)
	{
		return;
	}

	if (m_pBuffersVBO[0] != 0)
	{
		glDeleteBuffers(2, &m_pBuffersVBO[0]);
		memset(m_pBuffersVBO, 0, sizeof(m_pBuffersVBO));
	}

	glGenBuffers(2, &m_pBuffersVBO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
	GLenum usage = GL_STATIC_DRAW;
	if (m_mesh_res->HasSkeleton())
	{
		usage = GL_DYNAMIC_DRAW;
	}

	if (m_mesh_res->HasSkeleton() && m_skeleton_instance != 0)
	{
		glBufferData(GL_ARRAY_BUFFER, m_skeleton_instance->GetBufferSize(), m_skeleton_instance->GetBuffer(), usage);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, m_mesh_res->GetVertexBufferSize(), m_mesh_res->GetVertexBuffer(), usage);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh_res->GetIndexBufferSize(), m_mesh_res->GetIndexBuffer(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR_DEBUG();
}

void CCEnt3D::InitShaderProgram(int mat_type)
{
	if (m_mesh_res != 0)
	{
		BlendFunc blendFunc;
		blendFunc.src = GL_SRC_ALPHA;
		blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

		if (m_mesh_res->HasDiffuse() && mat_type == 0)
		{
			SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLEND));
		}
		else if (m_mesh_res->HasDiffuse() && mat_type == 1)
		{
			blendFunc.src = GL_ONE;
			SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLENDADDMODE));
		}
		else if(!m_mesh_res->HasDiffuse() && mat_type == 1)
		{
			blendFunc.src = GL_ONE;
			SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLENDNCADDMODE));
		}
		else
		{
			SetShaderProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_MODLEBLENDNC));
		}
		setBlendFunc(blendFunc);
	}
}

void CCEnt3D::UpdateEnabledAnimations( float elapsed_time)
{
	if( m_animation_state_set != 0)
	{
		EnableAnimMap::iterator it_end = m_enable_anim_map.end();
		for( EnableAnimMap::iterator it = m_enable_anim_map.begin(); 
			it != it_end; ++it)
		{
			const std::string& name = it->first;
			CCAnimationState* anim = m_animation_state_set->getAnimationState( name);
			if( anim != 0)
			{
				anim->addTime( elapsed_time);
			}
		}
	}
}

bool CCEnt3D::PreDraw(float t)
{
	if(m_mesh_res!=0)
	{
		if( m_skeleton_res != 0)
		{
			// 更新骨骼当前时间
			UpdateEnabledAnimations(t);
			// 使动画对Skeleton生效
			m_skeleton_instance->setAnimationState( *m_animation_state_set);
			// 记录骨骼矩阵
			m_skeleton_instance->_getBoneMatrices(m_bone_matrices);
 			// 使用骨骼矩阵计算顶点坐标
 			m_mesh_res->AnimateVertices( m_bone_matrices, m_num_bone_matrices, m_skeleton_instance->GetBuffer(), m_skeleton_instance->GetBufferSize());
		}

		return true;
	}
	return false;
}

NS_CC_END