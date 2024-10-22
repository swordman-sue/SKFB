#include <math.h>
#include "richitemparser.h"
#include "stringconv.h"
#include "view/widget/richtext.h"
#include "zqcommon/logutil.h"

using namespace std;

#define SPACE_UNICODE	32
#define PROP_VALUE_MAX_LEN	100
#define ONEWORD_MAX_LEN 8

#define COLOR_TAG					"COLOR"
#define COLOR_CLOSE_TAG				"/COLOR"
#define COLOR_PROP_VALUE			"VALUE"
#define SHADOW_COLOR_PROP_VALUE		"SHADOW"
#define OUTLINE_COLOR_PROP_VALUE	"OUTLINE"
#define OUTLINE_W_COLOR_PROP_VALUE	"OUTLINE_W"

#define FONT_TAG					"FONT"
#define FONT_CLOSE_TAG				"/FONT"
#define FONT_PROP_NAME				"NAME"
#define FONT_PROP_SIZE				"SIZE"
#define FONT_PROP_SPACING			"SPACING"

#define BR_TAG						"BR"
#define BR_PROP_SPACING				"SPACING"

#define FACEIMG_TAG					"FACEIMG"
#define FACEIMG_PROP_IMG			"IMG"
#define FACEIMG_PROP_TEXRECT		"TEXRECT"
#define FACEIMG_PROP_SCALE			"SCALE"
#define FACEIMG_PROP_SCALEX			"SCALEX"
#define FACEIMG_PROP_SCALEY			"SCALEY"

#define LINEHEIGHT_TAG				"LINEHEIGHT"
#define LINEHEIGHT_PROP_MIN			"MIN"
#define LINEHEIGHT_PROP_MAX			"MAX"

#define LOCATEX_TAG					"LOCATEX"
#define LOCATEX_PROP_SCALE			"SCALE"
#define LOCATEX_PROP_OFFSET			"OFFSET"

#define LINK_TAG					"LINK"
#define LINK_CLOSE_TAG				"/LINK"
#define LINK_PROP_DATA				"DATA"
#define LINK_PROP_TYPE				"TYPE"
#define LINK_PROP_URL				"URL"

#define LINESPACE_TAG				"LINESPACE"
#define LINESPACE_CLOSE_TAG			"/LINESPACE"
#define LINESPACE_PROP_VALUE		"VALUE"

USING_NS_CC;

bool RichItemParser::getTagPropertyValue(const TagInfo& info, const char* name, char* value)
{
	for(unsigned int i = 0; i < info.tag_prop.size(); ++i)
	{
		const cc_utf8_str_len& str_prop = info.tag_prop[i];
		if(utf8_strcmp(str_prop.str, name, strlen(name)) == 0)
		{
			int pos = utf8_find_code(str_prop.str, str_prop.len, '=');
			if(pos > 0)
			{
				int len = str_prop.len - pos - 1;
				memcpy(value, str_prop.str + pos + 1, len * sizeof(char));
				value[len] = 0;

				if(len < PROP_VALUE_MAX_LEN)
					return true;
				else
					return false;
			}
		}
	}
	return false;
}

#define FONT_SIZE_MAX	50
#define FONT_SIZE_MIN	5

static unsigned int StringToARGB(const char* str_val)
{
	unsigned int color = 0xFFFFFFFF;
	if(strlen(str_val) <= 6)	//RGB
	{
		if(sscanf(str_val, "%X", &color) == 0)
		{
			return 0;
		}
		color |= 0xFF000000;
	}
	else	//ARGB
	{
		if(sscanf(str_val, "%X", &color) == 0)
		{
			return 0;
		}
	}
	return color;
}

RichItem* RichItemParser::createItemFromTagInfo(const TagInfo& info)
{
	char str_val[PROP_VALUE_MAX_LEN] = {0};

	if(utf8_strcmp(info.tag_name.str, COLOR_TAG, info.tag_name.len, strlen(COLOR_TAG)) == 0)
	{
		ColorRichItem* pItem = new ColorRichItem;
		const ColorProperty& colProp = getCurColorProp();
		pItem->m_colRect = colProp.col4BRect;

		if(getTagPropertyValue(info, COLOR_PROP_VALUE, str_val))
		{
			unsigned int color;
			if(strlen(str_val) <= 10)	//ARGB
			{
				color = StringToARGB(str_val);
				pItem->m_colRect = Color4BRect(color);
			}
			else	//{topleft_color, topright_color, bottomleft_color, bottomright_color}
			{
				unsigned int nColTL, nColTR, nColBL, nColBR;
				if(sscanf(str_val, "{%X,%X,%X,%X}", &nColTL, &nColTR, &nColBL, &nColBR) == 0)
				{
					delete pItem;
					return NULL;
				}
				pItem->m_colRect = Color4BRect(nColTL, nColTR, nColBL, nColBR);
			}
		}
		if(getTagPropertyValue(info, SHADOW_COLOR_PROP_VALUE, str_val))
		{
			unsigned int nShadowColor = StringToARGB(str_val);
			pItem->m_nShadowColor = nShadowColor;
		}
		if(getTagPropertyValue(info, OUTLINE_COLOR_PROP_VALUE, str_val))
		{
			unsigned int nOutlineColor = StringToARGB(str_val);
			pItem->m_nOutlineColor = nOutlineColor;
		}
		if(getTagPropertyValue(info, OUTLINE_W_COLOR_PROP_VALUE, str_val))
		{
			float fOutlineWidth;
			sscanf(str_val, "%f", &fOutlineWidth);
			pItem->m_fOutlineWidth = fOutlineWidth;
		}

		pushColorProp(pItem->m_colRect, pItem->m_nShadowColor, pItem->m_nOutlineColor, pItem->m_fOutlineWidth);

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, COLOR_CLOSE_TAG, info.tag_name.len, strlen(COLOR_CLOSE_TAG)) == 0)
	{
		ColorRichItem* pItem = new ColorRichItem;
		popColorProp();
		const ColorProperty& colProp = getCurColorProp();
		pItem->m_colRect = colProp.col4BRect;
		pItem->m_nShadowColor = colProp.nShadowColor;
		pItem->m_nOutlineColor = colProp.nOutlineColor;
		pItem->m_fOutlineWidth = colProp.fOutlineWidth;
		return pItem;
	}
	
	else if(utf8_strcmp(info.tag_name.str, FONT_TAG, info.tag_name.len, strlen(FONT_TAG)) == 0)
	{
		FontRichItem* pItem = new FontRichItem;

		const FontProperty& fontProp = getCurFontProp();
		pItem->m_nFontSize = fontProp.nFontSize;
		pItem->m_strFontName = fontProp.strFontName;
		pItem->m_nFontSpacing = fontProp.nFontSpacing;

		if(getTagPropertyValue(info, FONT_PROP_NAME, str_val))
		{
			pItem->m_strFontName = str_val;
		}
		if(getTagPropertyValue(info, FONT_PROP_SIZE, str_val))
		{
			int nFontSize;
			if(sscanf(str_val, "%d", &nFontSize) == 0)
			{
				delete pItem;
				return NULL;
			}
			if(nFontSize > FONT_SIZE_MAX)
				nFontSize = FONT_SIZE_MAX;
			else if(nFontSize < FONT_SIZE_MIN)
				nFontSize = FONT_SIZE_MIN;
			pItem->m_nFontSize = nFontSize;
		}
		if(getTagPropertyValue(info, FONT_PROP_SPACING, str_val))
		{
			int nFontSpacing;
			if(sscanf(str_val, "%d", &nFontSpacing) == 0)
			{
				delete pItem;
				return NULL;
			}

			pItem->m_nFontSpacing = nFontSpacing;
		}

		pushFontProp(pItem->m_strFontName, pItem->m_nFontSize, pItem->m_nFontSpacing);

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, FONT_CLOSE_TAG, info.tag_name.len, strlen(FONT_CLOSE_TAG)) == 0)
	{
		FontRichItem* pItem = new FontRichItem;

		popFontProp();

		const FontProperty& fontProp = getCurFontProp();
		pItem->m_nFontSize = fontProp.nFontSize;
		pItem->m_strFontName = fontProp.strFontName;
		pItem->m_nFontSpacing = fontProp.nFontSpacing;

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, FACEIMG_TAG, info.tag_name.len, strlen(FACEIMG_TAG)) == 0)
	{
		char strImg[PROP_VALUE_MAX_LEN] = {0};
		int tx = 0, ty = 0, tw = 0, th = 0; 

		FaceImgRichItem* pItem = new FaceImgRichItem;

		if(getTagPropertyValue(info, FACEIMG_PROP_IMG, str_val))
		{
			strcpy(strImg, str_val);
		}

		if(getTagPropertyValue(info, FACEIMG_PROP_TEXRECT, str_val))
		{
			if(sscanf(str_val, "{%d,%d,%d,%d}", &tx, &ty, &tw, &th) != 4)
			{
				delete pItem;
				return NULL;
			}
		}
		if(getTagPropertyValue(info, FACEIMG_PROP_SCALE, str_val))
		{
			sscanf(str_val, "%f", &pItem->m_fScale);
		}
		if(getTagPropertyValue(info, FACEIMG_PROP_SCALEX, str_val))
		{
			sscanf(str_val, "%f", &pItem->m_fScaleX);
		}
		if(getTagPropertyValue(info, FACEIMG_PROP_SCALEY, str_val))
		{
			sscanf(str_val, "%f", &pItem->m_fScaleY);
		}

		pItem->m_strImg = strImg;
		pItem->m_nTexX = tx;
		pItem->m_nTexY = ty;
		pItem->m_nTexW = tw;
		pItem->m_nTexH = th;

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, BR_TAG, info.tag_name.len, strlen(BR_TAG)) == 0)
	{
		BRRichItem* pItem = new BRRichItem;

		if(getTagPropertyValue(info, BR_PROP_SPACING, str_val))
		{
			int nSpacing;
			if(sscanf(str_val, "%d", &nSpacing) == 0)
			{
				delete pItem;
				return NULL;
			}

			pItem->m_nSpacing = nSpacing;
		}

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LINEHEIGHT_TAG, info.tag_name.len, strlen(LINEHEIGHT_TAG)) == 0)
	{
		LineHeightRichItem* pItem = new LineHeightRichItem;

		if(getTagPropertyValue(info, LINEHEIGHT_PROP_MIN, str_val))
		{
			int val;
			sscanf(str_val, "%d", &val);
			pItem->m_nMinLineHeight = val;
		}
		if(getTagPropertyValue(info, LINEHEIGHT_PROP_MAX, str_val))
		{
			int val;
			sscanf(str_val, "%d", &val);
			pItem->m_nMaxLineHeight = val;
		}

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LOCATEX_TAG, info.tag_name.len, strlen(LOCATEX_TAG)) == 0)
	{
		LocateXRichItem* pItem = new LocateXRichItem;

		if(getTagPropertyValue(info, LOCATEX_PROP_OFFSET, str_val))
		{
			float val;
			sscanf(str_val, "%f", &val);
			pItem->m_fOffset = val;
		}

		if(getTagPropertyValue(info, LOCATEX_PROP_SCALE, str_val))
		{
			float val;
			sscanf(str_val, "%f", &val);
			pItem->m_fScale = val;
		}

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LINK_TAG, info.tag_name.len, strlen(LINK_TAG)) == 0)
	{
		LinkRichItem* pItem = new LinkRichItem;

		if(getTagPropertyValue(info, LINK_PROP_URL, str_val))
		{
			pItem->m_strUrl = str_val;
		}
		if(getTagPropertyValue(info, LINK_PROP_DATA, str_val))
		{
			int nUserData;
			if(sscanf(str_val, "%d", &nUserData)  == 0)
			{
				delete pItem;
				return NULL;
			}
			pItem->m_nUserData = nUserData;
		}
		if(getTagPropertyValue(info, LINK_PROP_TYPE, str_val))
		{
			int nLinkType;
			if(sscanf(str_val, "%d", &nLinkType)  == 0)
			{
				delete pItem;
				return NULL;
			}
			pItem->m_nLinkType = nLinkType;
		}

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LINK_CLOSE_TAG, info.tag_name.len, strlen(LINK_CLOSE_TAG)) == 0)
	{
		LinkRichItem* pItem = new LinkRichItem;
		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LINESPACE_TAG, info.tag_name.len, strlen(LINESPACE_TAG)) == 0)
	{
		LineSpaceRichItem* pItem = new LineSpaceRichItem;

		if(getTagPropertyValue(info, LINESPACE_PROP_VALUE, str_val))
		{
			int nVal;
			if(sscanf(str_val, "%d", &nVal)  == 0)
			{
				delete pItem;
				return NULL;
			}
			pItem->m_nLineSpace = nVal;
		}

		return pItem;
	}

	else if(utf8_strcmp(info.tag_name.str, LINESPACE_CLOSE_TAG, info.tag_name.len, strlen(LINESPACE_CLOSE_TAG)) == 0)
	{
		LineSpaceRichItem* pItem = new LineSpaceRichItem;

		return pItem;
	}

	return NULL;
}

void RichItemParser::parseTagInfo(TagInfo& info, const char* str, int len)
{
	const char* str_end = str + len - 1;

	++str;	//跳过'<'

	//定位标签名称
	int pos = utf8_find_code(str, str_end, SPACE_UNICODE);
	if(pos < 0)
		pos =  len - 2;	//没有属性，跳到'>'
	info.tag_name.str = str;
	info.tag_name.len = pos;

	str += pos;

	//定位属性集
	while(str < str_end)
	{
		//跳过空格
		while(*str == SPACE_UNICODE) {++str;}

		pos = utf8_find_code(str, str_end, SPACE_UNICODE);
		if(pos < 0)
		{
			break;
		}

		cc_utf8_str_len str_prop;
		str_prop.str = str;
		str_prop.len = pos;

		info.tag_prop.push_back(str_prop);

		str += pos;
	}
	if(str < str_end)
	{
		cc_utf8_str_len str_prop;
		str_prop.str = str;
		str_prop.len = str_end - str;

		info.tag_prop.push_back(str_prop);
	}
}

RichItem* RichItemParser::parse(const char* str, int len, RichItemContext* pContext)
{
	initPropStack(pContext);

	char code;
	int idx = 0, skip_idx = 0;
	RichItem *pItem = NULL, *pItemOld = NULL, *pItemRoot = NULL;
	while(idx < len)
	{
		code = str[idx];

		if(code == '<')	//标签
		{
			pItem = parseTag(str + idx, &skip_idx);

			if(pItem == NULL)			//标签格式不匹配
			{
				pItem = parseText(str + idx, &skip_idx);	//'<'作为字符处理
			}

			idx += skip_idx;
		}
		else								//文本
		{
			pItem = parseText(str + idx, &skip_idx);
			idx += skip_idx;
		}

		if(pItem)
		{
			if(pItemRoot == NULL)
			{
				pItemRoot = pItem;
			}
			if(pItemOld == NULL)
			{
				pItemOld = pItem;
			}
			else
			{
				pItemOld->m_pNext = pItem;
				pItem->m_pPre = pItemOld;
				pItemOld = pItem;
			}
		}
	}

	if(pItemRoot)
	{
		initializeItem(pItemRoot, pContext);
	}

	return pItemRoot;
}

RichItem* RichItemParser::parseTag(const char* str, int* skip_idx)
{
	int pos = utf8_find_code(str, '>');
	if(pos < 0)
		return NULL;

	*skip_idx = pos + 1;

	TagInfo info;
	parseTagInfo(info, str, pos + 1);

	RichItem* pItem = createItemFromTagInfo(info);
	return pItem;
}

RichItem* RichItemParser::parseText(const char* str, int* skip_idx)
{
	TextRichItem* pItem = new TextRichItem;
	*skip_idx = utf8_skip(str);
	pItem->code = utf8_get_char(str);
	pItem->code_string = "";

	const char* next_char = str;
	int str_idx = *skip_idx;
	std::string newline_ch_str = RichText::GetNewlineChar();
	while (true)
	{
		next_char = next_char + str_idx;
		int code = utf8_get_char(next_char);

		//没有设置换行符号，则非特殊字符都可以单独处理
		if (newline_ch_str == "")
		{
			bool is_tg_yb_ch = (code < 0x0E31 || code > 0x0E3A) && (code < 0x0E47 || code > 0x0E4E) || (code == 0x0E32);
			if (is_tg_yb_ch)
			{
				break;
			}
		}
		else
		{
			if (newline_ch_str.find(code) != std::string::npos)
				break;
		}

		str_idx = utf8_skip(next_char);
		*skip_idx = *skip_idx + str_idx;
		pItem->code_string.assign(str, *skip_idx);
		if (pItem->code_string.length() >= ONEWORD_MAX_LEN)
			break;
	};

	return pItem;
}

void RichItemParser::initializeItem(RichItem* pRoot, RichItemContext* pContext)
{
	for(RichItem* pItem = pRoot; pItem != NULL; pItem = pItem->m_pNext)
	{
		if(pItem->isEntityItem())
		{
			RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(pItem);
			if(pItemEntity)
			{
				pItemEntity->initialize(pContext);
			}
		}
		else if(pItem->isPropertyItem())
		{
			RichItemProperty* pItemProperty = dynamic_cast<RichItemProperty*>(pItem);
			if (pItemProperty)
			{
				pItemProperty->updateContext(pContext);
			}
		}
	}
}

// 布局处理
Size RichItemParser::layoutItem(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace, ReadingOrder reading_order)
{
	if (ReadingOrder::RO_RTL == reading_order && maxLineWidth > 0)
	{
		return layoutItemRTL(pRoot, maxLineWidth, vItemRow, horzFontSpace);
	}
	else
	{
		return layoutItemLTR(pRoot, maxLineWidth, vItemRow, horzFontSpace);
	}
}

// 布局处理(从左往右的语序)
Size RichItemParser::layoutItemLTR(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace /* = 0 */)
{
	if (pRoot == NULL)
		return Size(0, 0);

	vItemRow.clear();

	int x = 0, y = 0;
	int nMaxWidth = 0;
	int nMaxHeight = 0, nMinLineHeight = 0, nMaxLineHeight = 1024;
	bool bNewLine = true;
	std::vector<RichItemEntity*> vWholeItems;
	for (RichItem* pItem = pRoot; pItem != NULL; pItem = pItem->m_pNext)
	{
		if (pItem->isEntityItem())
		{
			RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(pItem);
			if (pItemEntity)
			{
				float wholeItemsAdvanceX;
				float wholeItemsHeight = 0;
				vWholeItems.clear();

				// 若检测到字母或数字，则往下搜索完整串（以空白字符为分隔符）
				TextRichItem* pTextItem = dynamic_cast<TextRichItem*>(pItemEntity);
				if (nullptr != pTextItem && pTextItem->IsLetterOrNumber())
				{
					wholeItemsAdvanceX = 0;
					do
					{
						wholeItemsAdvanceX += (pTextItem->getAdvanceX() + horzFontSpace);
						wholeItemsHeight = max(wholeItemsHeight, pTextItem->m_size.height);
						if (maxLineWidth > 0 && wholeItemsAdvanceX - horzFontSpace > maxLineWidth)
						{
							wholeItemsAdvanceX -= pTextItem->getAdvanceX();
							break;
						}
						vWholeItems.push_back(pTextItem);
						pTextItem = dynamic_cast<TextRichItem*>(pTextItem->m_pNext);
					} while (nullptr != pTextItem && !pTextItem->IsSpace() && (pTextItem->IsBaseASCII() || pTextItem->IsLetterOrNumber()));

					wholeItemsAdvanceX -= horzFontSpace;
				}
				else
				{
					wholeItemsAdvanceX = pItemEntity->getAdvanceX();
					wholeItemsHeight = pItemEntity->m_size.height;
					vWholeItems.push_back(pItemEntity);
				}

				// 换行检测
				if (maxLineWidth > 0 && x + wholeItemsAdvanceX > maxLineWidth)
				{
					bNewLine = true;
					if (vItemRow.size() > 0)
					{
						// 记录本行的宽度
						RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
						curRow.width = max(0, x - horzFontSpace);
					}
				}

				// 换行处理
				if (bNewLine)
				{
					x = 0;
					bNewLine = false;

					RichItemRow row;
					row.width = 0; row.height = min(max((int)wholeItemsHeight, nMinLineHeight), nMaxLineHeight);
					vItemRow.push_back(row);
				}

				RichItemRow& curRow = vItemRow[vItemRow.size() - 1];

				if (curRow.height < wholeItemsHeight)
				{
					// 记录本行的高度
					curRow.height = min(max((int)wholeItemsHeight, nMinLineHeight), nMaxLineHeight);
				}

				// 将vWholeItems中的实体插入到本行item_list中
				RichItemEntity* pTmpItemEntity;
				std::vector<RichItemEntity*>::iterator it;
				std::vector<RichItemEntity*>::iterator ite = vWholeItems.end();
				for (it = vWholeItems.begin(); it != ite; ++it)
				{
					pTmpItemEntity = *it;
					curRow.item_list.push_back(pTmpItemEntity);

					pTmpItemEntity->setPositionX((float)x + pTmpItemEntity->getOffsetX());

					x += (int)pTmpItemEntity->getAdvanceX() + horzFontSpace;
				}
				pItem = vWholeItems[vWholeItems.size() - 1];
			}
		}
		else if (pItem->isFormatItem())
		{
			if (pItem->getType() == RIT_BR)
			{
				bNewLine = true;
				if (vItemRow.size() > 0)
				{
					// 记录本行的宽度
					RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
					curRow.width = max(0, x - horzFontSpace);
				}

				BRRichItem* pBRItem = dynamic_cast<BRRichItem*>(pItem);
				if (pBRItem->m_nSpacing > 0)
				{
					RichItemRow row;
					row.width = 0; row.height = pBRItem->m_nSpacing;
					vItemRow.push_back(row);
				}
			}
			else if (pItem->getType() == RIT_LINEHEIGHT)
			{
				LineHeightRichItem* pLineHeightItem = dynamic_cast<LineHeightRichItem*>(pItem);
				if (pLineHeightItem)
				{
					nMinLineHeight = pLineHeightItem->m_nMinLineHeight;
					nMaxLineHeight = pLineHeightItem->m_nMaxLineHeight;
				}
			}
			else if (pItem->getType() == RIT_LOCATEX)
			{
				LocateXRichItem* pLocateXItem = dynamic_cast<LocateXRichItem*>(pItem);
				if (pLocateXItem)
				{
					x = (int)(maxLineWidth * pLocateXItem->m_fScale + pLocateXItem->m_fOffset);
				}
			}
		}
	}

	if (vItemRow.size() > 0)
	{
		// 记录本行宽度
		RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
		if (curRow.width <= 0)
		{
			curRow.width = x;
		}

		// 设置实体位置
		for (unsigned short i = 0; i < vItemRow.size(); ++i)
		{
			RichItemRow& row = vItemRow[i];

			for (unsigned int j = 0; j < row.item_list.size(); ++j)
			{
				RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(row.item_list[j]);
				if (pItemEntity)
				{
					pItemEntity->setPositionY((float)y);
				}
			}

			// 计算下一行的y坐标
			y -= row.height;

			// 计算富文本宽高
			nMaxHeight += row.height;
			if (row.width > nMaxWidth)
				nMaxWidth = row.width;
		}
	}

	return Size((float)nMaxWidth, (float)nMaxHeight);
}

// 布局处理(从右往左的语序)
Size RichItemParser::layoutItemRTL(RichItem* pRoot, int maxLineWidth, RichItemRowVec& vItemRow, int horzFontSpace /* = 0 */)
{
	if (pRoot == NULL)
		return Size(0, 0);

	RichItem* pLast = pRoot;
	while (NULL != pLast->m_pNext)
	{
		pLast = pLast->m_pNext;
	}

	vItemRow.clear();

	int x = 0, y = 0;
	int nMaxWidth = 0;
	int nMaxHeight = 0, nMinLineHeight = 0, nMaxLineHeight = 1024;
	bool bNewLine = true;
	std::vector<RichItemEntity*> vWholeItems;
	for (RichItem* pItem = pLast; pItem != NULL; pItem = pItem->m_pPre)
	{
		if (pItem->isEntityItem())
		{
			RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(pItem);
			if (pItemEntity)
			{
				float wholeItemsAdvanceX;
				float wholeItemsHeight = 0;
				vWholeItems.clear();

				// 若检测到字母或数字，则往上搜索完整串（以空白字符为分隔符）
				TextRichItem* pTextItem = dynamic_cast<TextRichItem*>(pItemEntity);
				if (nullptr != pTextItem && pTextItem->IsLetterOrNumber())
				{
					wholeItemsAdvanceX = 0;
					do
					{
						wholeItemsAdvanceX += (pTextItem->getAdvanceX() + horzFontSpace);
						wholeItemsHeight = max(wholeItemsHeight, pTextItem->m_size.height);
						if (maxLineWidth > 0 && wholeItemsAdvanceX - horzFontSpace > maxLineWidth)
						{
							wholeItemsAdvanceX -= pTextItem->getAdvanceX();
							break;
						}
						vWholeItems.push_back(pTextItem);
						pTextItem = dynamic_cast<TextRichItem*>(pTextItem->m_pPre);
					} while (nullptr != pTextItem && !pTextItem->IsSpace() && (pTextItem->IsBaseASCII() || pTextItem->IsLetterOrNumber()));

					wholeItemsAdvanceX -= horzFontSpace;
				}
				else
				{
					wholeItemsAdvanceX = pItemEntity->getAdvanceX();
					wholeItemsHeight = pItemEntity->m_size.height;
					vWholeItems.push_back(pItemEntity);
				}

				// 换行检测
				if (maxLineWidth > 0 && x + wholeItemsAdvanceX > maxLineWidth)
				{
					bNewLine = true;
					if (vItemRow.size() > 0)
					{
						// 记录本行的宽度
						RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
						curRow.width = max(0, x - horzFontSpace);
					}
				}

				// 换行处理
				if (bNewLine)
				{
					x = 0;
					bNewLine = false;

					RichItemRow row;
					row.width = 0; row.height = min(max((int)wholeItemsHeight, nMinLineHeight), nMaxLineHeight);
					vItemRow.push_back(row);
				}

				RichItemRow& curRow = vItemRow[vItemRow.size() - 1];

				if (curRow.height < wholeItemsHeight)
				{
					// 记录本行的高度
					curRow.height = min(max((int)wholeItemsHeight, nMinLineHeight), nMaxLineHeight);
				}

				// 将vWholeItems中的实体插入到本行item_list中
				RichItemEntity* pTmpItemEntity;
				std::vector<RichItemEntity*>::iterator it;
				std::vector<RichItemEntity*>::iterator ite = vWholeItems.end();
				for (it = vWholeItems.begin(); it != ite; ++it)
				{
					pTmpItemEntity = *it;
					curRow.item_list.push_back(pTmpItemEntity);

					pTmpItemEntity->setPositionX((float)x);

					x += (int)pTmpItemEntity->getAdvanceX() + horzFontSpace;
				}
				pItem = vWholeItems[vWholeItems.size() - 1];
			}
		}
		else if (pItem->isFormatItem())
		{
			if (pItem->getType() == RIT_BR)
			{
				bNewLine = true;
				if (vItemRow.size() > 0)
				{
					// 记录本行的宽度
					RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
					curRow.width = max(0, x - horzFontSpace);
				}

				BRRichItem* pBRItem = dynamic_cast<BRRichItem*>(pItem);
				if (pBRItem->m_nSpacing > 0)
				{
					RichItemRow row;
					row.width = 0; row.height = pBRItem->m_nSpacing;
					vItemRow.push_back(row);
				}
			}
			else if (pItem->getType() == RIT_LINEHEIGHT)
			{
				LineHeightRichItem* pLineHeightItem = dynamic_cast<LineHeightRichItem*>(pItem);
				if (pLineHeightItem)
				{
					nMinLineHeight = pLineHeightItem->m_nMinLineHeight;
					nMaxLineHeight = pLineHeightItem->m_nMaxLineHeight;
				}
			}
			else if (pItem->getType() == RIT_LOCATEX)
			{
				LocateXRichItem* pLocateXItem = dynamic_cast<LocateXRichItem*>(pItem);
				if (pLocateXItem)
				{
					x = (int)(maxLineWidth * pLocateXItem->m_fScale + pLocateXItem->m_fOffset);
				}
			}
		}
	}

	if (vItemRow.size() > 0)
	{
		// 记录本行宽度
		RichItemRow& curRow = vItemRow[vItemRow.size() - 1];
		if (curRow.width <= 0)
		{
			curRow.width = x;
		}

		// 设置实体位置
		for (unsigned short i = 0; i < vItemRow.size(); ++i)
		{
			RichItemRow& row = vItemRow[i];

			for (unsigned int j = 0; j < row.item_list.size(); ++j)
			{
				RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(row.item_list[j]);
				if (pItemEntity)
				{
					// 设置实体坐标
					pItemEntity->setPositionX((float)maxLineWidth - pItemEntity->getPositionX() - pItemEntity->getAdvanceX() + pItemEntity->getOffsetX());
					pItemEntity->setPositionY((float)y);
				}
			}

			// 计算下一行的y坐标
			y -= row.height;

			// 计算富文本宽高
			nMaxHeight += row.height;
			if (row.width > nMaxWidth)
				nMaxWidth = row.width;
		}
	}

	return Size((float)nMaxWidth, (float)nMaxHeight);
}

static RichItemParser s_richItemParser;
RichItemParser* RichItemParser::sharedRichItemParser()
{
	return &s_richItemParser;
}

void RichItemParser::initPropStack(RichItemContext* pContext)
{
	m_colorPropVec.clear();
	m_fontPropVec.clear();

	pushFontProp(pContext->m_strFontName, pContext->m_nFontSize, pContext->m_nFontSpacing);
	pushColorProp(pContext->m_colRect, pContext->m_nShadowColor, pContext->m_nOutlineColor, pContext->m_fOutlineWidth);
}

void RichItemParser::pushFontProp(const std::string& strFontName, int nFontSize, int nFontSpacing)
{
	FontProperty fontProp;
	fontProp.strFontName = strFontName;
	fontProp.nFontSize = nFontSize;
	fontProp.nFontSpacing = nFontSpacing;
	m_fontPropVec.push_back(fontProp);
}

void RichItemParser::popFontProp()
{
	//始终保证栈有元素，防止非法调用导致空栈。
	if(m_fontPropVec.size() > 1)
	{
		m_fontPropVec.pop_back();
	}
}

const RichItemParser::FontProperty& RichItemParser::getCurFontProp() const
{
	return m_fontPropVec.back();
}

void RichItemParser::pushColorProp(const Color4BRect& col4BRect, unsigned int nShadowColor, unsigned int nOutlineColor, float fOutlineWidth)
{
	ColorProperty colProp;
	colProp.col4BRect = col4BRect;
	colProp.nShadowColor = nShadowColor;
	colProp.nOutlineColor = nOutlineColor;
	colProp.fOutlineWidth = fOutlineWidth;
	m_colorPropVec.push_back(colProp);
}

void RichItemParser::popColorProp()
{
	//始终保证栈有元素，防止非法调用导致空栈。
	if(m_colorPropVec.size() > 1)
	{
		m_colorPropVec.pop_back();
	}
}

const RichItemParser::ColorProperty& RichItemParser::getCurColorProp() const
{
	return m_colorPropVec.back();
}
