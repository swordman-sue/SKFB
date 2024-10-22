#include "font.h"

#include "zqcommon/gamedef.h"
#include "zqcommon/strutil.h"
#include "stringconv.h"
#include "fontmanager.h"
#include "filesystem/setting.h"
#include "2d/CCFontFreeType.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCLabel.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCDirector.h"

USING_NS_CC;

#define DEF_FONT_SIZE	24

#define TEXTURE_SIZE		256
#define TEXTURE_SPACING		1
#define STR_CODE_STACK_NUM	128

MidFont::MidFont():
	m_strFontName(Label::DEF_FONT_NAME),
	m_nFontSize(DEF_FONT_SIZE),
	m_nPosX(0),
	m_nPosY(0),
	m_nMaxHeight(0),
	m_nFontHeight(-1),
	m_pCurTexture(nullptr),
	m_pFontAtlas(nullptr),
	m_purgeTextureListener(nullptr),
	m_resetTextureListener(nullptr)
{
	initialize();
}

MidFont::MidFont(const std::string& strFontName, int nFontSize):
	m_strFontName(strFontName),
	m_nFontSize(nFontSize),
	m_nPosX(0),
	m_nPosY(0),
	m_nMaxHeight(0),
	m_nFontHeight(-1),
	m_pCurTexture(nullptr),
	m_pFontAtlas(nullptr),
	m_purgeTextureListener(nullptr),
	m_resetTextureListener(nullptr)
{
	initialize();
}

MidFont::~MidFont()
{
	finalize();
	
	FontAtlasCache::releaseFontAtlas(m_pFontAtlas);
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_purgeTextureListener);
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_resetTextureListener);
}

void MidFont::clear()
{
	finalize();
}

void MidFont::drawTextLine(const std::string& text, const Vec2& position, const Color4B& color, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	drawTextLine(text, position, Color4BRect::FromColor4B(color), fScale, nCursorCol, nShadowCol, nOutlineCol, fOutlineWidth);
}

void MidFont::drawTextLine(const std::string& text, const Vec2& position, const Color4BRect& colRect, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	unsigned int wstr[STR_CODE_STACK_NUM];
	long len = utf8_strlen(text.c_str(), -1);

	assert(len < STR_CODE_STACK_NUM);
	if(len >= STR_CODE_STACK_NUM)
		return;

	utf32_from_utf8(text.c_str(), wstr);

	Vec2 cur_pos = position;
	for(int i = 0; i < len; ++i)
	{
		unsigned int code = wstr[i];
		const FontGlyph* pGlyph = getFontGlyph(code);
		if(pGlyph)
		{
			drawFontGlyph(pGlyph, cur_pos, colRect, fScale, nShadowCol, nOutlineCol, fOutlineWidth);

			cur_pos.x += pGlyph->getContentAdvance() * fScale;
		}
	}

	//Draw Cursor
	drawCursor(cur_pos, nCursorCol, fScale);
}

void MidFont::drawTextLine(const std::string& text, const cocos2d::Rect& rcDest, const cocos2d::Color4B& color, FontAlignment align, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	drawTextLine(text, rcDest, Color4BRect::FromColor4B(color), align, fScale, nCursorCol, nShadowCol, nOutlineCol, fOutlineWidth);
}

void MidFont::drawTextLine(const std::string& text, const cocos2d::Rect& rcDest, const Color4BRect& colRect, FontAlignment align, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	int w, h;
	getTextLineSize(text, w, h, fScale);

	Vec2 pt = Vec2(0, 0);
	if(align == FA_TOPLEFT)
	{
		pt.x = rcDest.getMinX();
		pt.y = rcDest.getMaxY() - h;
	}
	else if(align == FA_TOP)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w) * 0.5f;
		pt.y = rcDest.getMaxY() - h;
	}
	else if(align == FA_TOPRIGHT)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w);
		pt.y = rcDest.getMaxY() - h;
	}

	else if(align == FA_LEFT)
	{
		pt.x = rcDest.getMinX();
		pt.y = rcDest.getMinY() + (rcDest.size.height - h) * 0.5f;
	}
	else if(align == FA_CENTER)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w) * 0.5f;
		pt.y = rcDest.getMinY() + (rcDest.size.height - h) * 0.5f;
	}
	else if(align == FA_RIGHT)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w);
		pt.y = rcDest.getMinY() + (rcDest.size.height - h) * 0.5f;
	}

	else if(align == FA_BOTTOMLEFT)
	{
		pt.x = rcDest.getMinX();
		pt.y = rcDest.getMinY();
	}
	else if(align == FA_BOTTOM)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w) * 0.5f;
		pt.y = rcDest.getMinY();
	}
	else if(align == FA_BOTTOMRIGHT)
	{
		pt.x = rcDest.getMinX() + (rcDest.size.width - w);
		pt.y = rcDest.getMinY();
	}

	drawTextLine(text, pt, colRect, fScale, nCursorCol, nShadowCol, nOutlineCol, fOutlineWidth);
}

void MidFont::drawWrappedText(const std::string& text, const Rect& rcDest, const Color4B& color, FontAlignment align, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	drawWrappedText(text, rcDest, Color4BRect::FromColor4B(color), align, fScale, nCursorCol, nShadowCol, nOutlineCol, fOutlineWidth);
}

void MidFont::drawWrappedText(const std::string& text, const cocos2d::Rect& rcDest, const Color4BRect& colRect, FontAlignment align, float fScale, unsigned int nCursorCol, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	unsigned int wstr[STR_CODE_STACK_NUM];
	long len = utf8_strlen(text.c_str(), -1);

	assert(len < STR_CODE_STACK_NUM);
	if(len >= STR_CODE_STACK_NUM)
		return;

	utf32_from_utf8(text.c_str(), wstr);

	int nTextW, nTextH;
	getWrappedTextSize(wstr, len, nTextW, nTextH, rcDest, fScale);

	int nFontHeight = getFontHeight(fScale);
	Vec2 cur_pos;
	Vec2 start_pos;

	if(align == FA_TOPLEFT)
		start_pos = cur_pos = Vec2(rcDest.getMinX(), rcDest.getMaxY() - nFontHeight);
	else if(align == FA_TOP)
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW) * 0.5f, rcDest.getMaxY() - nFontHeight);
	else if(align == FA_TOPRIGHT)
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW), rcDest.getMaxY() - nFontHeight);
	else if(align == FA_LEFT)
		start_pos = cur_pos = Vec2(rcDest.getMinX(), rcDest.getMaxY() - nFontHeight - (rcDest.size.height - nTextH) * 0.5f);
	else if(align == FA_CENTER)
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW) * 0.5f, rcDest.getMaxY() - nFontHeight - (rcDest.size.height - nTextH) * 0.5f);
	else if(align == FA_RIGHT)
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW), rcDest.getMaxY() - nFontHeight - (rcDest.size.height - nTextH) * 0.5f);
	else if(align == FA_BOTTOMLEFT)
		start_pos = cur_pos = Vec2(rcDest.getMinX(), rcDest.getMinY());
	else if(align == FA_BOTTOM)
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW) * 0.5f, rcDest.getMinY());
	else	//FA_BOTTOMRIGHT
		start_pos = cur_pos = Vec2(rcDest.getMinX() + (rcDest.size.width - nTextW), rcDest.getMinY());

	for(int i = 0; i < len; ++i)
	{
		unsigned int code = wstr[i];
		if(code == '\n')
		{
			cur_pos.x = start_pos.x;
			cur_pos.y -= nFontHeight;
		}
		else
		{
			const FontGlyph* pGlyph = getFontGlyph(code);
			if(pGlyph)
			{
				if(cur_pos.x + pGlyph->getContentAdvance(fScale) > rcDest.getMaxX())
				{
					cur_pos.x = start_pos.x;
					cur_pos.y -= nFontHeight;
				}

				drawFontGlyph(pGlyph, cur_pos, colRect, fScale, nShadowCol, nOutlineCol, fOutlineWidth);

				cur_pos.x += pGlyph->getContentAdvance(fScale);
			}
		}
	}

	//Draw Cursor
	drawCursor(cur_pos, nCursorCol);
}

void MidFont::drawFontGlyph(const FontGlyph* pGlyph, const Vec2& position, const Color4B& color, float fScale, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	drawFontGlyph(pGlyph, position, Color4BRect::FromColor4B(color), fScale, nShadowCol, nOutlineCol, fOutlineWidth);
}

void MidFont::drawFontGlyph(const FontGlyph* pGlyph, const Vec2& position, const Color4BRect& colRect, float fScale, unsigned int nShadowCol, unsigned int nOutlineCol, float fOutlineWidth)
{
	if (!pGlyph->bIsValid)
	{
		return;
	}

	Texture2D* pTex = pGlyph->pTexture;
	if(pTex == NULL)
		return;

	//Draw Shadow
	if(nShadowCol > 0)
	{
		drawTexture(pTex, 
			cocos2d::Rect((float)pGlyph->x, (float)pGlyph->y, (float)pGlyph->width, (float)pGlyph->height), 
			cocos2d::Rect(position.x + fOutlineWidth * fScale, position.y - fOutlineWidth * fScale, pGlyph->getContentWidth(fScale), pGlyph->getContentHeight(fScale)), nShadowCol);
	}

	fOutlineWidth = fOutlineWidth / 2;

#ifndef WP8
	//Draw Outline
	if(nOutlineCol > 0)
	{
		for(int i = -1; i <= 1; ++i)
			for(int j = -1; j <= 1; ++j)
			{
				if(i != 0 || j != 0)
				{
					drawTexture(pTex, cocos2d::Rect((float)pGlyph->x, (float)pGlyph->y, (float)pGlyph->width, (float)pGlyph->height), 
						cocos2d::Rect(position.x + i * fOutlineWidth * fScale, position.y + j * fOutlineWidth * fScale, pGlyph->getContentWidth(fScale), pGlyph->getContentHeight(fScale)),
						nOutlineCol);
				}
			}
	}
#else
	//wp8下使用shadow效果替代，提高效率
	if (nOutlineCol > 0)
	{
		drawTexture(pTex, cocos2d::Rect((float)pGlyph->x, (float)pGlyph->y, (float)pGlyph->width, (float)pGlyph->height),
			cocos2d::Rect(position.x + fOutlineWidth * fScale, position.y - fOutlineWidth * fScale, pGlyph->getContentWidth(fScale), pGlyph->getContentHeight(fScale)),
			nOutlineCol);
	}
#endif // !WP8

	drawTexture(pTex, 
		cocos2d::Rect((float)pGlyph->x, (float)pGlyph->y, (float)pGlyph->width, (float)pGlyph->height), 
		cocos2d::Rect(position.x, position.y, pGlyph->getContentWidth(fScale), pGlyph->getContentHeight(fScale)), colRect);
}

void MidFont::drawCursor(const cocos2d::Vec2& position, unsigned int nCol, float fScale)
{
	if(nCol > 0)
	{
		Color4B col4B = ARGBToColor4B(nCol);
		drawTexture(FontManager::sharedFontManager()->getPixelTexture(),
			cocos2d::Rect(0.0f,0.0f,1.0f,1.0f), 
			cocos2d::Rect((float)position.x, (float)position.y, 2.0f, getFontHeight() * fScale), 
			col4B);
	}
}

void MidFont::getTextLineSize(const unsigned int* wstr, int len, int& w, int& h, float fScale)
{
	if(len < 0)
		len = utf32_strlen(wstr);

	int nMaxW = 0, nMaxH = getFontHeight(fScale);
	for(int i = 0; i < len; ++i)
	{
		const FontGlyph* pGlyph = getFontGlyph(wstr[i]);
		if(pGlyph)
		{
			nMaxW += (int)pGlyph->getContentAdvance();
			if(pGlyph->getContentHeight() > nMaxH)
				nMaxH = (int)pGlyph->getContentHeight();
		}
	}

	w = (int)(nMaxW * fScale);
	h = (int)(nMaxH * fScale);
}

void MidFont::getTextLineSize(const std::string& text, int& w, int& h, float fScale)
{
	unsigned int wstr[STR_CODE_STACK_NUM];
	long len = utf8_strlen(text.c_str(), -1);

	assert(len < STR_CODE_STACK_NUM);
	if(len >= STR_CODE_STACK_NUM)
		return;

	utf32_from_utf8(text.c_str(), wstr);

	getTextLineSize(wstr, -1, w, h, fScale);
}

void MidFont::getWrappedTextSize(const unsigned int* text, int len, int& w, int& h, const cocos2d::Rect& rcDest, float fScale)
{
	int nFontHeight = getFontHeight(fScale);
	int x = 0;
	w = 0;
	h = nFontHeight;
	for(int i = 0; i < len; ++i)
	{
		unsigned int code = text[i];
		if(code == '\n')
		{
			if(x > w)
				w = x;
			x = 0;
			h += nFontHeight;
		}
		else
		{
			const FontGlyph* pGlyph = getFontGlyph(code);
			if(pGlyph)
			{
				if(x + pGlyph->getContentAdvance(fScale) > rcDest.size.width)
				{
					if(x > w)
						w = x;

					x = 0;
					h += nFontHeight;
				}

				x += (int)pGlyph->getContentAdvance(fScale);
			}
		}
	}

	if(x > w)
		w = x;
}

void MidFont::initialize()
{
	if (Label::USE_SYSTEM_FONT_BY_TTF && !ZQ::StrUtil::EndsWith(m_strFontName, ".ttf"))
	{
		m_strFontName += ".ttf";
	}

	initFontAtlas();
}

void MidFont::finalize()
{
	m_fontGlyphMap.clear();
	m_fontStrGlyphMap.clear();

	m_nPosX = 0;
	m_nPosY = 0;
	m_nMaxHeight = 0;
	m_pCurTexture = nullptr;
	for (unsigned int i = 0; i < m_textureVec.size(); ++i)
	{
		Texture2D* pTexture = m_textureVec[i];
		pTexture->release();
	}
	m_textureVec.clear();
}

void MidFont::rasterize(unsigned int code)
{
	FontGlyphMap::iterator it = m_fontGlyphMap.find(code);
	if(it != m_fontGlyphMap.end())
		return;

	m_fontGlyphMap[code] = rasterizeFreeTypeFont(code);
}

void MidFont::rasterize(const std::string& str_code)
{
	FontStrGlyphMap::iterator it = m_fontStrGlyphMap.find(str_code);
	if (it != m_fontStrGlyphMap.end())
		return;

	m_fontStrGlyphMap[str_code] = rasterizeSystemFont(str_code);
}

FontGlyph MidFont::rasterizeFreeTypeFont(unsigned int code)
{
	initFontAtlas();

	char16_t* u16code = (char16_t*)&code;
	m_pFontAtlas->prepareLetterDefinitions(u16code);

	FontLetterDefinition letterDefinition;
	letterDefinition.textureID = -1;
	bool isValidLetter = m_pFontAtlas->getLetterDefinitionForChar(*u16code, letterDefinition);
	
	FontGlyph font_glyph = FontGlyph(
		letterDefinition.U, letterDefinition.V, letterDefinition.width, letterDefinition.height, 
		letterDefinition.xAdvance, m_pFontAtlas->getTexture(letterDefinition.textureID), letterDefinition.offsetX, letterDefinition.offsetY);
	font_glyph.bIsValid = isValidLetter;

	return font_glyph;
}

FontGlyph MidFont::rasterizeSystemFont(const std::string& str_code)
{
	//生成字模
	auto textDef = FontDefinition();
	auto contentScaleFactor = CC_CONTENT_SCALE_FACTOR();
	textDef._fontSize = m_nFontSize * contentScaleFactor;
	textDef._fontName = m_strFontName;

#ifdef WP8
	m_pCurTexture = new Texture2D;
	m_pCurTexture->initWithString(str_code.c_str(), textDef);
	m_textureVec.push_back(m_pCurTexture);

	return FontGlyph(0, 0, m_pCurTexture->getContentSizeInPixels().width, m_pCurTexture->getContentSizeInPixels().height, m_pCurTexture->getContentSizeInPixels().width, m_pCurTexture);
#else
	int imageWidth;
	int imageHeight;
	bool hasPremultipliedAlpha;
	Data outData = Device::getTextureDataForText(str_code.c_str(), textDef, Device::TextAlign::TOP_LEFT, imageWidth, imageHeight, hasPremultipliedAlpha);

	if (m_pCurTexture == NULL)
	{
		m_pCurTexture = createEmptyTexture(TEXTURE_SIZE, TEXTURE_SIZE);
		m_textureVec.push_back(m_pCurTexture);
	}

	//计算字模在纹理中的位置
	if (m_nMaxHeight < imageHeight)
	{
		m_nMaxHeight = imageHeight;
	}
	if (TEXTURE_SIZE - m_nPosX < imageWidth)	//换行
	{
		m_nPosX = 0;
		m_nPosY += (m_nMaxHeight + TEXTURE_SPACING);
		m_nMaxHeight = imageHeight;
	}
	if (TEXTURE_SIZE - m_nPosY < imageHeight)	//换页
	{
		m_nPosX = 0;
		m_nPosY = 0;
		m_nMaxHeight = imageHeight;

		m_pCurTexture = createEmptyTexture(TEXTURE_SIZE, TEXTURE_SIZE);
		m_textureVec.push_back(m_pCurTexture);
	}
	setSubTexture(m_pCurTexture, m_nPosX, m_nPosY, imageWidth, imageHeight, outData.getBytes());
	FontGlyph font_glyph = FontGlyph(m_nPosX, m_nPosY, imageWidth, imageHeight, imageWidth, m_pCurTexture);
	m_nPosX += (imageWidth + TEXTURE_SPACING);
	return font_glyph;
#endif 
}

const FontGlyph* MidFont::getFontGlyph(unsigned int code)
{
	if(FontManager::sharedFontManager()->isFilterChar(code))
		return NULL;

	rasterize(code);

	FontGlyphMap::iterator it = m_fontGlyphMap.find(code);
	return it != m_fontGlyphMap.end() ? &it->second : NULL;
}

const FontGlyph* MidFont::getFontGlyph(unsigned int code, const std::string & strCode)
{
	if (FontManager::sharedFontManager()->isFilterChar(code))
		return NULL;

	rasterize(strCode);

	FontStrGlyphMap::iterator it = m_fontStrGlyphMap.find(strCode);
	return it != m_fontStrGlyphMap.end() ? &it->second : NULL;
}

int MidFont::getFontHeight(float fScale)
{
	if(m_nFontHeight < 0)
	{
		const FontGlyph* pGlyph = getFontGlyph('A');
		m_nFontHeight = (int)pGlyph->getContentHeight();
	}

	return (int)(m_nFontHeight * fScale);
}

void MidFont::initFontAtlas()
{
	if (nullptr == m_pFontAtlas)
	{
		TTFConfig ttfConfig;
		ttfConfig.fontFilePath = m_strFontName;
		ttfConfig.fontSize = m_nFontSize;
		m_pFontAtlas = FontAtlasCache::getFontAtlasTTF(&ttfConfig);
	}

	// 监听CMD_PURGE_FONTATLAS，相关处理
	if (nullptr == m_purgeTextureListener)
	{
		m_purgeTextureListener = EventListenerCustom::create(FontAtlas::CMD_PURGE_FONTATLAS, [this](EventCustom* event){
			if (m_pFontAtlas && event->getUserData() == m_pFontAtlas)
			{
				FontAtlasCache::releaseFontAtlas(m_pFontAtlas);
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_purgeTextureListener, 1);
	}

	// 监听CMD_RESET_FONTATLAS，相关处理
	if (nullptr == m_resetTextureListener)
	{
		m_resetTextureListener = EventListenerCustom::create(FontAtlas::CMD_RESET_FONTATLAS, [this](EventCustom* event){
			if (m_pFontAtlas && event->getUserData() == m_pFontAtlas)
			{
				m_pFontAtlas = nullptr;
				m_fontGlyphMap.clear();
				m_fontStrGlyphMap.clear();
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_resetTextureListener, 2);
	}
}