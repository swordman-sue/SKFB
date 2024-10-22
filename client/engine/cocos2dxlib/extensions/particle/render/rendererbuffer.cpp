
#include "rendererbuffer.h"
#include "particle/particlehelper.h"

#include "renderer/CCTexture2D.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCGLProgram.h"
#include "cocos/2d/CCSprite.h"

#include "base/ccTypes.h"
#include "cocos/base/ccMacros.h"

#include "math/CCMath.h"

NS_CC_BEGIN

void ParticleTexListener::UnzipImageFileCallback(void *callback_data)
{
	if (callback_data == NULL)
		return;

	auto particle_listener = static_cast<ParticleTexListener*>(callback_data);
	if (particle_listener->getReferenceCount() == 1)
	{
		particle_listener->release();
		return;
	}

	std::string texture_path = particle_listener->GetPath();
	auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(texture_path);
	particle_listener->OnPreparedCompleted(texture);
}

void ParticleTexListener::OnTextureCacheCallback(Texture2D* texture)
{
	if (texture == nullptr)
	{
		return;
	}
	OnPreparedCompleted(texture);
}

void ParticleTexListener::AsyncLoad(const char* path)
{
	m_path = path;
	auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(m_path);
	if (texture != nullptr && m_buffer != nullptr)
	{
		m_buffer->setTexture(texture);
	}
	else
	{
		CC_SAFE_RETAIN(this);
		TextureCache::getInstance()->addImageAsync(m_path, CC_CALLBACK_1(ParticleTexListener::OnTextureCacheCallback, this));
	}
}

void ParticleTexListener::OnPreparedCompleted(Ref* resource)
{
	if (resource != 0)
	{
		Texture2D* tex = (Texture2D*)resource;
		if (tex != 0 && m_buffer != 0)
		{
			m_buffer->setTexture(tex);
		}
	}
	CC_SAFE_RELEASE(this);
}

std::string ParticleTexListener::GetPath()
{
	return m_path;
}

void ParticleTexListener::Unuse()
{
	m_buffer = 0;
}

CCParticleRenderBuffer::CCParticleRenderBuffer(bool index_dynamic) :
	m_max_vertex_count(0),
	m_max_index_count(0),
	m_pQuads(0),
	m_pIndices(0),
	m_pTexture(0),
	m_mat_type(MAT_ALPHA_BLEND),
	m_nUniformColor(-1),
	m_index_dynamic(index_dynamic),
	_glProgramState(nullptr)
{
	m_loader = new ParticleTexListener(this);
	m_loader->autorelease();
	m_loader->retain();

	m_tBlendFunc.src = GL_SRC_ALPHA;
	m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

	UpdateMaterialType();
}

CCParticleRenderBuffer::~CCParticleRenderBuffer()
{
	m_loader->Unuse();
	m_loader->release();
	m_loader = 0;

	CC_SAFE_FREE(m_pQuads);
	CC_SAFE_FREE(m_pIndices);
	CC_SAFE_RELEASE(_glProgramState);
	CC_SAFE_RELEASE(m_pTexture);
}

Texture2D * CCParticleRenderBuffer::getTexture() const
{
	return m_pTexture;
}

void CCParticleRenderBuffer::setTexture(Texture2D* var)
{
	if (m_pTexture != var)
	{
		CC_SAFE_RELEASE(m_pTexture);
		m_pTexture = var;
		CC_SAFE_RETAIN(m_pTexture);
	}
}

const BlendFunc& CCParticleRenderBuffer::getBlendFunc() const
{
	return m_tBlendFunc;
}

void CCParticleRenderBuffer::setBlendFunc(const BlendFunc& blendFunc)
{
	if( m_tBlendFunc.src != blendFunc.src || m_tBlendFunc.dst != blendFunc.dst )
	{
		m_tBlendFunc = blendFunc;
	}
}

void CCParticleRenderBuffer::NotitySizeChange(size_t max_v, size_t max_index)
{
	m_max_vertex_count = max_v;
	m_max_index_count = max_index;

	CC_SAFE_FREE(m_pQuads);
	m_pQuads = (V3F_C4B_T2F*)malloc(m_max_vertex_count * sizeof(V3F_C4B_T2F));
	memset(m_pQuads, 0, m_max_vertex_count * sizeof(V3F_C4B_T2F));

	CHECK_GL_ERROR_DEBUG();
}

void CCParticleRenderBuffer::DrawBuffer(size_t num_vertex, size_t num_index, float munsual)
{
	if (num_vertex >  m_max_vertex_count)
	{
		return;
	}

	if (m_mat_type == MAT_ADD && m_nUniformColor != -1)
	{
		_glProgramState->setUniformFloat(m_nUniformColor,munsual);
	}
	
	// 升级到3.2 add by zhuyi 2014-09-01
	auto director = Director::getInstance();
	const Mat4& mat = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_quadCommand.init(0,m_pTexture->getName(),_glProgramState,m_tBlendFunc,(V3F_C4B_T2F_Quad*)m_pQuads,num_vertex/4,director->Get2dProjectMat() * mat);
	Director::getInstance()->getRenderer()->addCommand(&_quadCommand);
	// end
}

// 升级到3.2 设置opengl state add by zhuyi 2014-08-30
void CCParticleRenderBuffer::setGLProgramState(GLProgram *glProgram)
{
	if (_glProgramState == nullptr || (_glProgramState && _glProgramState->getGLProgram() != glProgram))
	{
		CC_SAFE_RELEASE(_glProgramState);
		_glProgramState = GLProgramState::create(glProgram);
		_glProgramState->retain();
	}
}
//end

void CCParticleRenderBuffer::SetMaterialType(MaterialType type)
{
	if (m_mat_type != type)
	{
		m_mat_type = type;
		UpdateMaterialType();
	}
}

void CCParticleRenderBuffer::UpdateMaterialType()
{
	BlendFunc blendFunc;
	blendFunc.src = GL_SRC_ALPHA;
	blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
	m_nUniformColor = -1;
	switch( m_mat_type)
	{
	case MAT_ALPHA_BLEND: 
		setGLProgramState(CCShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_PARTICLENOMVP));
		break;
	case MAT_ADD:
		blendFunc.src = GL_ONE;
		setGLProgramState(CCShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_PARTICLEADDBLEND));
		m_nUniformColor = glGetUniformLocation(_glProgramState->getGLProgram()->getProgram(),"cc_mansual_alpha");
		break;
	}

	setBlendFunc(blendFunc);
}

void CCParticleRenderBuffer::SetTexturePath(std::string& path)
{
	m_loader->AsyncLoad(path.c_str());
}

void CCParticleRenderBuffer::MakeGrey(bool is_grey)
{
	if (is_grey)
	{
		BlendFunc blendFunc;
		blendFunc.src = GL_SRC_ALPHA;
		blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
		m_nUniformColor = -1;
		setGLProgramState(CCShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_BLACK_NO_MVP));
		setBlendFunc(blendFunc);
	}
	else
	{
		UpdateMaterialType();
	}
}

NS_CC_END
