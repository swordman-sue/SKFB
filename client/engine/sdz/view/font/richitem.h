#ifndef _CC_RICHITEM_H_
#define _CC_RICHITEM_H_

#include "cocos2d.h"
#include <string>
#include <vector>
#include "zqcommon/strutil.h"
#include "view/render/textureutil.h"

#define RICH_ITEM_FONT_LINE_HEIGHT	5.0f

enum RichItemType
{
	RIT_NONE	= 0,
	RIT_TEXT,
	RIT_COLOR,
	RIT_FONT,
	RIT_IMAGE,
	RIT_FACEIMG,
	RIT_BR,
	RIT_LINEHEIGHT,
	RIT_LOCATEX,
	RIT_LINK,
	RIT_LINESPACE
};

struct LinkRichItem;

struct RichItemContext
{
	RichItemContext():
	m_nFontSize(24),
	m_strFontName("HelveticaNeue-Bold"),
	m_fScale(1.0f),
	m_colRect(0xffffffff),
	m_nShadowColor(0),
	m_nOutlineColor(0),
	m_fOutlineWidth(1.0f),
	m_pLinkItem(NULL),
	m_nLineSpace(0),
	m_nFontSpacing(0)
	{
	}

	int m_nFontSize;
	std::string m_strFontName;
	Color4BRect m_colRect;
	unsigned int m_nShadowColor;
	unsigned int m_nOutlineColor;
	float m_fOutlineWidth;
	float m_fScale;
	LinkRichItem* m_pLinkItem;
	int m_nLineSpace;
	int m_nFontSpacing;
};

struct RichItem
{
	RichItem() : m_pNext(NULL), m_pPre(NULL) {}
	virtual ~RichItem(){}

	virtual bool isEntityItem() const { return false; }

	virtual bool isPropertyItem() const { return false; }

	virtual bool isFormatItem() const { return false; }

	virtual RichItemType getType() const { return RIT_NONE; }
	RichItem* m_pNext;
	RichItem* m_pPre;
};

struct RichItemProperty: public RichItem
{
	virtual void updateContext(RichItemContext* pContext) = 0;

	virtual bool isPropertyItem() const { return true; }
};

struct RichItemEntity: public RichItem
{
	RichItemEntity()
	: m_pos(cocos2d::Vec2(0, 0))
	, m_size(cocos2d::Size(0, 0))
	, m_pLinkItem(NULL)
	, m_offset_x(0)
	, m_offset_y(0)
	{}
	
	virtual void initialize(RichItemContext* pContext)
	{
		m_pLinkItem = pContext->m_pLinkItem;
	}

	virtual void draw(RichItemContext* pContext, int dx, int dy, cocos2d::Rect* pClipRect, GLubyte nRootOpacity = 0) const = 0;

	virtual bool isEntityItem() const { return true; }

	void setPosition(float x, float y)
	{
		m_pos.x = x; m_pos.y = y;
	}

	void setPositionX(float x)
	{
		m_pos.x = x;
	}

	float getPositionX() { return m_pos.x; }

	void setPositionY(float y)
	{
		m_pos.y = y;
	}

	float getPositionY() { return m_pos.y; }

	virtual float getAdvanceX()
	{
		return m_size.width;
	}

	float getOffsetX()
	{
		return m_offset_x;
	}

	float getOffsetY()
	{
		return m_offset_y;
	}

	cocos2d::Vec2 m_pos;
	cocos2d::Size m_size;
	float m_offset_x;
	float m_offset_y;
	LinkRichItem* m_pLinkItem;
};

struct RichItemFormat: public RichItem
{
	virtual bool isFormatItem() const { return true; }
};

struct FontRichItem: public RichItemProperty
{
	FontRichItem():
m_nFontSize(24),
	m_strFontName("HelveticaNeue-Bold"),
	m_nFontSpacing(0)
{
}

virtual RichItemType getType() const { return RIT_FONT; }

virtual void updateContext(RichItemContext* pContext)
{
	pContext->m_nFontSize = m_nFontSize;
	pContext->m_strFontName = m_strFontName;
	pContext->m_nFontSpacing = m_nFontSpacing;
}

int m_nFontSize;
std::string m_strFontName;
int m_nFontSpacing;
};

struct ColorRichItem: public RichItemProperty
{
	ColorRichItem():m_colRect(0xFFFFFFFF), m_nShadowColor(0), m_nOutlineColor(0), m_fOutlineWidth(1.0f)
	{
	}

	virtual RichItemType getType() const { return RIT_COLOR; }

	virtual void updateContext(RichItemContext* pContext)
	{
		pContext->m_colRect = m_colRect;
		pContext->m_nShadowColor = m_nShadowColor;
		pContext->m_nOutlineColor = m_nOutlineColor;
		pContext->m_fOutlineWidth = m_fOutlineWidth;
	}

	Color4BRect m_colRect;
	unsigned int m_nShadowColor;
	unsigned int m_nOutlineColor;
	float m_fOutlineWidth;
};

struct TextRichItem: public RichItemEntity
{
	TextRichItem() 
	: code(0)
	, m_advance_x(0)
	{ 
		code_string = ""; 
	}

	virtual RichItemType getType() const { return RIT_TEXT; }

	virtual void initialize(RichItemContext* pContext);
	virtual void draw(RichItemContext* pContext, int dx, int dy, cocos2d::Rect* pClipRect, GLubyte nRootOpacity) const;

	virtual float getAdvanceX() { return m_advance_x; }

	virtual bool IsLetterOrNumber()
	{
		return code_string == "" && 
			(ZQ::StrUtil::IsUnicodeEngLetterOrNumber(code) || ZQ::StrUtil::IsUnicodeUygurLetter(code));
	}

	virtual bool IsSpace()
	{
		return code_string == "" && ZQ::StrUtil::IsUnicodeSpace(code);
	}

	virtual bool IsBaseASCII()
	{
		return code_string == "" && ZQ::StrUtil::IsBaseASCII(code);
	}

	unsigned int code;
	std::string code_string;
	float m_advance_x;
};

struct FaceImgRichItem: public RichItemEntity
{
	FaceImgRichItem():m_pTexture(NULL),m_fScale(1.0f),m_fScaleX(1.0f),m_fScaleY(1.0f){}
	virtual ~FaceImgRichItem();

	virtual RichItemType getType() const { return RIT_FACEIMG; }

	virtual void initialize(RichItemContext* pContext);
	virtual void draw(RichItemContext* pContext, int dx, int dy, cocos2d::Rect* pClipRect, GLubyte nRootOpacity) const;
	std::string m_strImg;
	int m_nTexX;
	int m_nTexY;
	int m_nTexH;
	int m_nTexW;
	float m_fScale;
	float m_fScaleX;
	float m_fScaleY;
	cocos2d::Texture2D* m_pTexture;
};

struct BRRichItem: public RichItemFormat
{
	BRRichItem():m_nSpacing(0) {}
	virtual RichItemType getType() const { return RIT_BR; }
	int m_nSpacing;
};

struct LineHeightRichItem: public RichItemFormat
{
	LineHeightRichItem():m_nMinLineHeight(0),m_nMaxLineHeight(1024){}

	virtual RichItemType getType() const { return RIT_LINEHEIGHT; }

	int m_nMinLineHeight;
	int m_nMaxLineHeight;
};

struct LocateXRichItem: public RichItemFormat
{
	LocateXRichItem():m_fScale(0.0f), m_fOffset(0.0f){}

	virtual RichItemType getType() const { return RIT_LOCATEX; }

	float m_fScale;
	float m_fOffset;
};

struct LinkRichItem: public RichItemProperty
{
	LinkRichItem():m_nUserData(0), m_nLinkType(0)
	{

	}

	virtual RichItemType getType() const { return RIT_LINK; }

	virtual void updateContext(RichItemContext* pContext)
	{
		if(m_nUserData != 0 || m_nLinkType != 0 || m_strUrl.length() > 0)
			pContext->m_pLinkItem = this;
		else
			pContext->m_pLinkItem = NULL;
	}

	std::string m_strUrl;
	int m_nUserData;
	int m_nLinkType;
};

struct LineSpaceRichItem: public RichItemProperty
{
	LineSpaceRichItem():
m_nLineSpace(0)
{
}

virtual RichItemType getType() const { return RIT_LINESPACE; }

virtual void updateContext(RichItemContext* pContext)
{
	pContext->m_nLineSpace = m_nLineSpace;
}

int m_nLineSpace;
};

struct RichItemRow
{
	std::vector<RichItem*> item_list;
	int width;
	int height;
};
typedef std::vector<RichItemRow> RichItemRowVec;	

#endif