#ifndef _CC_FONT_H_
#define _CC_FONT_H_

#include <string>
#include <map>
#include <vector>

#include "cocos2d.h"

#include "view/render/textureutil.h"

enum FontAlignment
{
	FA_TOPLEFT,
	FA_TOP,
	FA_TOPRIGHT,
	FA_LEFT,
	FA_CENTER,
	FA_RIGHT,
	FA_BOTTOMLEFT,
	FA_BOTTOM,
	FA_BOTTOMRIGHT
};

namespace cocos2d
{
	class FontAtlasCache;
	class EventListenerCustom;
}

struct FontGlyph
{
public:
	FontGlyph(){}
	FontGlyph(int nx, int ny, int nw, int nh, int nAdvace, cocos2d::Texture2D* pTex) :
		x(nx), y(ny), width(nw), height(nh), advance(nAdvace), pTexture(pTex), m_offsetX(0), m_offsetY(0)
	{}
	FontGlyph(int nx, int ny, int nw, int nh, int nAdvace, cocos2d::Texture2D* pTex, float offsetX, float offsetY) :
		x(nx), y(ny), width(nw), height(nh), advance(nAdvace), pTexture(pTex), m_offsetX(offsetX), m_offsetY(offsetY)
	{}

	float getContentWidth(float fScale = 1.0f) const { return width * fScale; }
	float getContentHeight(float fScale = 1.0f) const { return height * fScale; }
	float getContentAdvance(float fScale = 1.0f) const { return advance * fScale; }

	int x;
	int y;
	int width;
	int height;
	int advance;
	bool bIsValid;
	float m_offsetX;
	float m_offsetY;

	cocos2d::Texture2D* pTexture;
};

class MidFont
{
public:
	MidFont();
	MidFont(const std::string& strFontName, int nFontSize);
	~MidFont();

	void clear();

	void drawTextLine(const std::string& text, const cocos2d::Vec2& position, const cocos2d::Color4B& color, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);
	void drawTextLine(const std::string& text, const cocos2d::Vec2& position, const Color4BRect& colRect, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);

	void drawTextLine(const std::string& text, const cocos2d::Rect& rcDest, const cocos2d::Color4B& color, FontAlignment align, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);
	void drawTextLine(const std::string& text, const cocos2d::Rect& rcDest, const Color4BRect& colRect, FontAlignment align, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);

	void drawWrappedText(const std::string& text, const cocos2d::Rect& rcDest, const cocos2d::Color4B& color, FontAlignment align, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);
	void drawWrappedText(const std::string& text, const cocos2d::Rect& rcDest, const Color4BRect& colRect, FontAlignment align, float fScale = 1.0f, unsigned int nCursorCol = 0, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);

	void drawFontGlyph(const FontGlyph* pGlyph, const cocos2d::Vec2& position, const cocos2d::Color4B& color, float fScale = 1.0f, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);
	void drawFontGlyph(const FontGlyph* pGlyph, const cocos2d::Vec2& position, const Color4BRect& colRect, float fScale = 1.0f, unsigned int nShadowCol = 0, unsigned int nOutlineCol = 0, float fOutlineWidth = 1.0f);

	void drawCursor(const cocos2d::Vec2& position, unsigned int nCol, float fScale = 1.0f);

	void getTextLineSize(const unsigned int* text, int len, int& w, int& h, float fScale = 1.0f);
	void getTextLineSize(const std::string& text, int& w, int& h, float fScale = 1.0f);

	void getWrappedTextSize(const unsigned int* text, int len, int& w, int& h, const cocos2d::Rect& rcDest, float fScale = 1.0f);

	virtual const FontGlyph* getFontGlyph(unsigned int code);

	const FontGlyph* getFontGlyph(unsigned int code, const std::string & str_code);

	virtual int getFontHeight(float fScale = 1.0f);

private:
	void initialize();
	void finalize();
	void rasterize(unsigned int code);
	void rasterize(const std::string& str_code);
	void initFontAtlas();
	//渲染FreeType字体
	FontGlyph rasterizeFreeTypeFont(unsigned int code);
	//渲染系统字体
	FontGlyph rasterizeSystemFont(const std::string& str_code);

private:
	std::string m_strFontName;
	int m_nFontSize;
	typedef std::map<unsigned int, FontGlyph> FontGlyphMap;
	FontGlyphMap m_fontGlyphMap;
	typedef std::map<std::string, FontGlyph> FontStrGlyphMap;
	FontStrGlyphMap m_fontStrGlyphMap;
	cocos2d::FontAtlas* m_pFontAtlas;

	typedef std::vector<cocos2d::Texture2D*> TextureVec;
	TextureVec m_textureVec;
	cocos2d::Texture2D* m_pCurTexture;
	int m_nPosX;
	int m_nPosY;
	int m_nMaxHeight;
	int m_nFontHeight;

	cocos2d::EventListenerCustom* m_purgeTextureListener;
	cocos2d::EventListenerCustom* m_resetTextureListener;
};

#endif