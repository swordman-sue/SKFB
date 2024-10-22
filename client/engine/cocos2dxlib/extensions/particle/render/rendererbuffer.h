#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "renderer/CCGLProgramState.h"
#include "renderer/CCQuadCommand.h"

#include "particle/particlehelper.h"
#include "cocos/base/CCProtocols.h"


NS_CC_BEGIN

class Texture2D;
class GLProgram;
class CCParticleRenderBuffer;
class CCResCspriteLoader;

class ParticleTexListener : public Ref
{
public:
	ParticleTexListener(CCParticleRenderBuffer* buffer):m_buffer(buffer)
	{

	}
	void AsyncLoad(const char* path);
	void OnPreparedCompleted(Ref* resource);
	void Unuse();
	std::string GetPath();

	static void UnzipImageFileCallback(void*);
	void OnTextureCacheCallback(Texture2D* texture);
private:
	CCParticleRenderBuffer* m_buffer;
	std::string m_path;
};

class CCParticleRenderBuffer : public TextureProtocol
{
public:
	CCParticleRenderBuffer(bool index_dynamic);
	~CCParticleRenderBuffer();

public:
	virtual Texture2D* getTexture() const;
	virtual void setTexture(Texture2D* var);
	virtual const BlendFunc& getBlendFunc(void) const;
	virtual void setBlendFunc(const BlendFunc& var);

public:
	void NotitySizeChange(size_t max_v, size_t max_index);
	void DrawBuffer(size_t num_vertex, size_t num_index, float munsual = 0.5f);
	V3F_C4B_T2F* GetVertexData() { return m_pQuads; }
	GLushort* GetIndexData() { return m_pIndices; }
	void SetMaterialType(MaterialType type);
	void SetTexturePath(std::string& path);
	void MakeGrey(bool is_grey);

	// 升级到3.2 add by zhuyi 2014-08-30
	void setGLProgramState(GLProgram* statie);
	// end

private:
	void SetShaderProgram(GLProgram *pShaderProgram);
	void UpdateMaterialType();

private:
	Texture2D*	m_pTexture;
	BlendFunc		m_tBlendFunc;

	// 升级到3.2 add by zhuyi 2014-08-30
	 QuadCommand _quadCommand;
	 GLProgramState *_glProgramState;
	// end

	V3F_C4B_T2F	    *m_pQuads;
	GLushort		*m_pIndices;
	
	size_t			m_max_vertex_count;
	size_t			m_max_index_count;
	MaterialType	m_mat_type;
	GLint			m_nUniformColor;
	bool			m_index_dynamic;
	ParticleTexListener* m_loader;
};

NS_CC_END

#endif