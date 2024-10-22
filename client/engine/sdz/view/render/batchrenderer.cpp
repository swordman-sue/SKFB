#include "BatchRenderer.h"
#include "TextureUtil.h"

USING_NS_CC;

#define QUAD_CAPACITY	256

void BatchRenderer::Tex_QuadList::render()
{
	if(m_pQuads == NULL)
	{
		m_pQuads = new cocos2d::V3F_C4B_T2F_Quad[m_quadVec.size()];
		for(unsigned int i = 0; i < m_quadVec.size(); ++i)
		{
			m_pQuads[i] = m_quadVec[i];
		}
	}
	if(m_pTexture->hasPremultipliedAlpha())
	{
		ccGLBlendFunc( CC_BLEND_SRC, CC_BLEND_DST);	}
	else	{
		ccGLBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	drawTexQuads(m_pTexture, m_pQuads, m_quadVec.size());
}

BatchRenderer::BatchRenderer():
	m_pCurTexture(NULL),
	m_pCurQuadList(NULL),
	m_bDirty(true),
	m_nBlendColor(0xffffffff),
	m_szShaderKey(""),
	m_nUniformLoc(-1),
	m_nUniformVar(-1)
{
}

BatchRenderer::~BatchRenderer()
{
	clear();
}

void BatchRenderer::insertTexQuad(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4B& color)
{
	if(pTex == NULL)
		return;

	if(m_pCurTexture != pTex || (m_pCurQuadList && m_pCurQuadList->getSize() >= QUAD_CAPACITY))
	{
		m_pCurQuadList = new Tex_QuadList(pTex);
		m_tex_quadList_Vec.push_back(m_pCurQuadList);
		m_pCurTexture = pTex;
	}
	Color4B finalCol4B = color;
	if(m_nBlendColor != 0xffffffff)
	{
		finalCol4B = Color4BMultARGB(color, m_nBlendColor);
	}
	if(pTex->hasPremultipliedAlpha())
	{
		finalCol4B = Color4BPremultiAlpha(finalCol4B);
	}

	V3F_C4B_T2F_Quad quad;
	updateQuad(&quad, pTex, rcTex, rcDest, finalCol4B);
	
	m_pCurQuadList->insertQuad(quad);
}

void BatchRenderer::insertTexQuad(Texture2D* pTex, const Rect& rcTex, const Rect& rcDest, const Color4BRect& colRect)
{
	if(pTex == NULL)
		return;

	if(m_pCurTexture != pTex || (m_pCurQuadList && m_pCurQuadList->getSize() >= QUAD_CAPACITY))
	{
		m_pCurQuadList = new Tex_QuadList(pTex);
		m_tex_quadList_Vec.push_back(m_pCurQuadList);
		m_pCurTexture = pTex;
	}

	V3F_C4B_T2F_Quad quad;
	if(m_nBlendColor == 0xffffffff)
	{
		updateQuad(&quad, pTex, rcTex, rcDest, colRect);
	}
	else
	{
		Color4BRect finalColRect;
		finalColRect.colTopLeft = Color4BMultARGB(colRect.colTopLeft, m_nBlendColor);
		finalColRect.colTopRight = Color4BMultARGB(colRect.colTopRight, m_nBlendColor);
		finalColRect.colBottomLeft = Color4BMultARGB(colRect.colBottomLeft, m_nBlendColor);
		finalColRect.colBottomRight = Color4BMultARGB(colRect.colBottomRight, m_nBlendColor);
		updateQuad(&quad, pTex, rcTex, rcDest, finalColRect);
	}
	m_pCurQuadList->insertQuad(quad);
}

void BatchRenderer::clear()
{
	for(Tex_QuadList_Vec::iterator it = m_tex_quadList_Vec.begin(); it != m_tex_quadList_Vec.end(); ++it)
	{
		Tex_QuadList* pQuadList = *it;
		delete pQuadList;
	}
	m_tex_quadList_Vec.clear();

	m_pCurTexture = NULL;
	m_pCurQuadList = NULL;
}

void BatchRenderer::flush()
{
	if (m_nUniformLoc != -1 && m_nUniformVar != -1)
	{
		auto gl_program = GLProgramCache::getInstance()->getGLProgram(m_szShaderKey);
		if (gl_program)
		{
			gl_program->setUniformLocationWith1i(m_nUniformLoc, m_nUniformVar);
		}
	}

	for(Tex_QuadList_Vec::iterator it = m_tex_quadList_Vec.begin(); it != m_tex_quadList_Vec.end(); ++it)
	{
		Tex_QuadList* pQuadList = *it;
		pQuadList->render();
	}
}

void BatchRenderer::setDirty(bool val)
{
	if(val)
	{
		clear();
	}
	m_bDirty = val;
}

void BatchRenderer::setBlendColor(unsigned int nColor)
{
	m_nBlendColor = nColor;
}

static BatchRendererManager s_rendererManager;

BatchRendererManager::BatchRendererManager():
	m_pRenderer(NULL),
	m_pIndices(NULL)
{
	setupIndices();
}

BatchRendererManager::~BatchRendererManager()
{
	if(m_pIndices)
	{
		delete[] m_pIndices;
		m_pIndices = NULL;
	}
}

BatchRendererManager* BatchRendererManager::sharedRendererManager()
{
	return &s_rendererManager;
}

void BatchRendererManager::registerRenderer(BatchRenderer* pRenderer)
{
	m_rendererSet.insert(pRenderer);
}

void BatchRendererManager::unregisterRenderer(BatchRenderer* pRenderer)
{
	BatchRendererSet::iterator it = m_rendererSet.find(pRenderer);
	if(it != m_rendererSet.end())
	{
		m_rendererSet.erase(it);
	}
}

void BatchRendererManager::setDirty(bool val)
{
	for(BatchRendererSet::iterator it = m_rendererSet.begin(); it != m_rendererSet.end(); ++it)
	{
		BatchRenderer* pRenderer = *it;
		pRenderer->setDirty(val);
	}
}

void BatchRendererManager::setupIndices()
{
	m_pIndices = new GLushort[QUAD_CAPACITY * 6];
	for( unsigned int i = 0; i < QUAD_CAPACITY; ++i)
	{
		m_pIndices[i*6+0] = i*4+0;
		m_pIndices[i*6+1] = i*4+1;
		m_pIndices[i*6+2] = i*4+2;

		m_pIndices[i*6+3] = i*4+3;
		m_pIndices[i*6+4] = i*4+2;
		m_pIndices[i*6+5] = i*4+1;        
	}
}