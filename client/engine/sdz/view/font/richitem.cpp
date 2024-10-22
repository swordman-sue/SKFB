#include "richitem.h"
#include "fontmanager.h"
#include "stringconv.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"

USING_NS_CC;

void TextRichItem::initialize(RichItemContext* pContext)
{
	RichItemEntity::initialize(pContext);

	MidFont* pFont = FontManager::sharedFontManager()->getValidFont(pContext->m_strFontName, pContext->m_nFontSize);
	const FontGlyph* pFontGlyph = NULL;
	if (strcmp(code_string.c_str(), "") == 0)
	{
		pFontGlyph = pFont->getFontGlyph(code);
	}
	else
	{
		pFontGlyph = pFont->getFontGlyph(code, code_string);
	}
	
	if(pFontGlyph)
	{
		m_size.width = pFontGlyph->getContentWidth(pContext->m_fScale);
		m_size.height = pFontGlyph->getContentHeight(pContext->m_fScale) + pContext->m_nLineSpace + pContext->m_nFontSpacing;
		m_advance_x = pFontGlyph->getContentAdvance(pContext->m_fScale);
		m_offset_x = pFontGlyph->m_offsetX * pContext->m_fScale;
		m_offset_y = pFontGlyph->m_offsetY * pContext->m_fScale;
	}
	else
	{
		m_size.width = 0;
		m_size.height = 0;
	}
}

void TextRichItem::draw(RichItemContext* pContext, int dx, int dy, cocos2d::Rect* pClipRect, GLubyte nRootOpacity) const
{
	unsigned int text[2];
	text[0] = code;
	text[1] = 0;

	MidFont* pFont = FontManager::sharedFontManager()->getValidFont(pContext->m_strFontName, pContext->m_nFontSize);
	const FontGlyph* pFontGlyph = NULL;
	if (strcmp(code_string.c_str(), "") == 0)
	{
		pFontGlyph = pFont->getFontGlyph(code);
	}
	else
	{
		pFontGlyph = pFont->getFontGlyph(code, code_string);
	}
	if(pFontGlyph)
	{
		pContext->m_colRect.SetOpacity(nRootOpacity);

		pFont->drawFontGlyph(pFontGlyph, 
			Vec2(dx + m_pos.x, dy + m_pos.y - pFontGlyph->height - pFontGlyph->m_offsetY), 
			pContext->m_colRect, pContext->m_fScale, pContext->m_nShadowColor, pContext->m_nOutlineColor, pContext->m_fOutlineWidth);

		if(m_pLinkItem)
		{
			drawTexture(
				FontManager::sharedFontManager()->getPixelTexture(), 
				Rect(0.0f,0.0f,1.0f,1.0f), 
				Rect(float(dx + m_pos.x - 0.5f), float(dy + m_pos.y - pFont->getFontHeight() - RICH_ITEM_FONT_LINE_HEIGHT), float(m_size.width + 1.0f), 1.5f), pContext->m_colRect);
		}
	}
}

FaceImgRichItem::~FaceImgRichItem()
{
	CC_SAFE_RELEASE(m_pTexture);
}

void FaceImgRichItem::initialize(RichItemContext* pContext)
{
	RichItemEntity::initialize(pContext);
	if(m_strImg.length() == 0)
		return;

	if(!ViewHelper::LoadImageResource(m_strImg))
		return;

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(m_strImg);
	Texture2D *pTexture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if(pTexture == NULL)
		return;

	pTexture->retain();
	m_pTexture = pTexture;

	//³ý2ÏòÉÏ¼æÈÝ
	m_size = Size((float)m_nTexW * pContext->m_fScale * m_fScale * m_fScaleX / 2, ((float)m_nTexH * pContext->m_fScale * m_fScale * m_fScaleY) / 2  + pContext->m_nLineSpace);
}


void FaceImgRichItem::draw(RichItemContext* pContext, int dx, int dy, cocos2d::Rect* pClipRect, GLubyte nRootOpacity) const
{
	if(m_pTexture == NULL)
		return;

	Color4B col(255, 255, 255, nRootOpacity);
	drawTexture(m_pTexture, Rect((float)m_nTexX, (float)m_nTexY, (float)m_nTexW, (float)m_nTexH),
		Rect(dx + m_pos.x, dy + m_pos.y - m_size.height, m_size.width, m_size.height), col);
}