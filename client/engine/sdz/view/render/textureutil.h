#ifndef _CC_TEXTURE_UTIL_H_
#define _CC_TEXTURE_UTIL_H_

#include "cocos2d.h"
#include "renderer/CCTexture2D.h"
#include "view/render/colortype.h"

cocos2d::Texture2D* createEmptyTexture(int w, int h);

cocos2d::Texture2D* createARGB8Texture(int w, int h, unsigned int col);

void setSubTexture(cocos2d::Texture2D* pTex, int x, int y, int w, int h, void* data, cocos2d::Texture2D::PixelFormat format = cocos2d::Texture2D::PixelFormat::RGBA8888);

void setSubTexture(cocos2d::Texture2D* pTex, int x, int y, cocos2d::Image* pImage);

void setSubTexture(cocos2d::Texture2D* pTex, int x, int y, const char *text, const char *fontName, float fontSize,
				   int& w, int& h);

void updateColor(cocos2d::V3F_C4B_T2F_Quad* pQuad, const cocos2d::Color4B& color4);
void updateColor(cocos2d::V3F_C4B_T2F_Quad* pQuad, const Color4BRect& colRect);

void updateVertex(cocos2d::V3F_C4B_T2F_Quad* pQuad, float x1, float y1, float x2, float y2);
void updateTexCoord(cocos2d::V3F_C4B_T2F_Quad* pQuad, float left, float top, float right, float bottom);
void updateQuad(cocos2d::V3F_C4B_T2F_Quad* pQuad, cocos2d::Texture2D* pTex,
	const cocos2d::Rect& rcTex, const cocos2d::Rect& rcDest, const cocos2d::Color4B& color);
void updateQuad(cocos2d::V3F_C4B_T2F_Quad* pQuad, cocos2d::Texture2D* pTex,
				const cocos2d::Rect& rcTex, const cocos2d::Rect& rcDest, const Color4BRect& colRect);

void drawTexture(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex, 
	const cocos2d::Rect& rcDest, const cocos2d::Color4B& color, const cocos2d::Mat4& transform, bool need_transform);

void drawTexture(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex,
	const cocos2d::Rect& rcDest, const cocos2d::Color4B& color);

void drawTexture(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex, 
				 const cocos2d::Rect& rcDest, const Color4BRect& colRect);

void drawScale9Texture(cocos2d::Texture2D* pTex, const cocos2d::Rect& rcTex, const cocos2d::Rect& rcDest, 
	const cocos2d::Rect& rcInner, const cocos2d::Color4B& color, const cocos2d::Mat4& transform = nullptr, bool need_transform = false);

void drawTexQuads(cocos2d::Texture2D* pTex, cocos2d::V3F_C4B_T2F_Quad* pQuads, int count);

void drawTriangle(cocos2d::Texture2D* pTex, const cocos2d::Vec2* ptTex, const cocos2d::Vec2* ptDest, const cocos2d::Color4B& color, unsigned int count);

#endif