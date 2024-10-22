#ifndef _BATCH_RENDERER_H_
#define _BATCH_RENDERER_H_

#include "cocos2d.h"
#include "colortype.h"
#include <vector>
#include <set>

#define BATCH_RENDERER_BEGIN(renderer) \
	BatchRendererManager::sharedRendererManager()->setRenderer(&renderer); \
	if(renderer.isDirty()) \
	{

#define BATCH_RENDERER_END(renderer) \
		renderer.setDirty(false); \
	} \
	BatchRendererManager::sharedRendererManager()->setRenderer(NULL); \
	renderer.flush();

class BatchRenderer
{
private:
	class Tex_QuadList
	{
	public:
		Tex_QuadList():m_pTexture(NULL),m_pQuads(NULL)
		{}

		Tex_QuadList(cocos2d::Texture2D* pTex):m_pTexture(pTex),m_pQuads(NULL)
		{}

		~Tex_QuadList()
		{
			if(m_pQuads)
			{
				delete[] m_pQuads;
				m_pQuads = NULL;
			}
		}

		void insertQuad(const cocos2d::V3F_C4B_T2F_Quad& quad)
		{
			m_quadVec.push_back(quad);
		}

		int getSize() const
		{
			return m_quadVec.size();
		}

		void render();

	private:
		cocos2d::Texture2D* m_pTexture;
		typedef std::vector<cocos2d::V3F_C4B_T2F_Quad> QuadVec;
		QuadVec m_quadVec;
		cocos2d::V3F_C4B_T2F_Quad* m_pQuads;
	};

public:
	BatchRenderer();
	~BatchRenderer();

	void insertTexQuad(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex, 
		const cocos2d::Rect& rcDest, const cocos2d::Color4B& color);
	void insertTexQuad(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex, 
		const cocos2d::Rect& rcDest, const Color4BRect& colRect);

	void clear();

	void flush();

	void setDirty(bool val);
	bool isDirty() const { return m_bDirty; }

	void setBlendColor(unsigned int nColor);
	unsigned int getBlendColor() const { return m_nBlendColor; }

	inline void setShaderKey(const char* key) { m_szShaderKey = key; }
	inline void setUniformInfo(GLint loc, GLint var)
	{
		m_nUniformLoc = loc; m_nUniformVar = var;
	}

private:
	cocos2d::Texture2D* m_pCurTexture;
	typedef std::vector<Tex_QuadList*> Tex_QuadList_Vec;
	Tex_QuadList_Vec m_tex_quadList_Vec;
	Tex_QuadList* m_pCurQuadList;
	bool m_bDirty;
	unsigned int m_nBlendColor;
	std::string m_szShaderKey;
	GLint m_nUniformLoc;
	GLint m_nUniformVar;
};

class BatchRendererManager
{
public:
	BatchRendererManager();
	~BatchRendererManager();

	static BatchRendererManager* sharedRendererManager();

	BatchRenderer* getRenderer() const { return m_pRenderer; }
	void setRenderer(BatchRenderer* pRenderer) { m_pRenderer = pRenderer; }

	void registerRenderer(BatchRenderer* pRenderer);
	void unregisterRenderer(BatchRenderer* pRenderer);

	void setDirty(bool val);

	GLushort* getIndices() const { return m_pIndices; }
private:
	void setupIndices();

private:
	BatchRenderer* m_pRenderer;
	GLushort* m_pIndices;
	typedef std::set<BatchRenderer*> BatchRendererSet;
	BatchRendererSet m_rendererSet;
};

#endif