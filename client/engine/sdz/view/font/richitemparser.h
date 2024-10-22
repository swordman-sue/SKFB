#ifndef _CC_RICHITEM_PARSER_H_
#define _CC_RICHITEM_PARSER_H_

#include "richitem.h"

#include <string>
#include <vector>

#include "view/render/textureutil.h"

class RichItemParser
{
public:
	enum class ReadingOrder
	{
		RO_LTR = 0,
		RO_RTL = 1,
	};

	struct FontProperty
	{
		std::string strFontName;
		int nFontSize;
		int nFontSpacing;
	};
	typedef std::vector<FontProperty> FontPropertyVec;

	struct ColorProperty
	{
		ColorProperty():col4BRect(0xFFFFFFFF), nShadowColor(0), nOutlineColor(0), fOutlineWidth(1.0f)
		{
		}
		Color4BRect col4BRect;
		unsigned int nShadowColor;
		unsigned int nOutlineColor;
		float fOutlineWidth;
	};
	typedef std::vector<ColorProperty> ColorPropertyVec;

	struct cc_utf8_str_len		//指定长度的字符串，非0结束。
	{
		const char* str;		//NOTE:只不过是作为外部地址的引用，不要进行任何赋值操作。
		int len;				//NOTE:由于str是外部地址引用，非0结束字符串，需明确指出长度。
	};

	struct TagInfo
	{
		cc_utf8_str_len tag_name;
		std::vector<cc_utf8_str_len> tag_prop;
	};

public:
	RichItem* parse(const char* str, int len, RichItemContext* pContext);

	void initializeItem(RichItem* pRoot, RichItemContext* pContext);

	cocos2d::Size layoutItem(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace = 0, ReadingOrder reading_order = ReadingOrder::RO_LTR);

public:
	static RichItemParser* sharedRichItemParser();

private:
	RichItem* parseTag(const char* str, int* skip_idx);
	RichItem* parseText(const char* str, int* skip_idx);
	RichItem* createItemFromTagInfo(const TagInfo& info);
	bool getTagPropertyValue(const TagInfo& info, const char* name, char* value);
	void parseTagInfo(TagInfo& info, const char* str, int len);

	void initPropStack(RichItemContext* pContext);

	void pushFontProp(const std::string& strFontName, int nFontSize, int nFontSpacing);
	void popFontProp();
	const FontProperty& getCurFontProp() const;

	void pushColorProp(const Color4BRect& col4BRect, unsigned int nShadowColor, unsigned int nOutlineColor, float fOutlineWidth);
	void popColorProp();
	const ColorProperty& getCurColorProp() const;

	cocos2d::Size layoutItemLTR(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace = 0);
	cocos2d::Size layoutItemRTL(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace = 0);

private:
	FontPropertyVec m_fontPropVec;
	ColorPropertyVec m_colorPropVec;
};

#endif