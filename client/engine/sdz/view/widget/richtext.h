#ifndef _CC_RICHTEXT_H_
#define _CC_RICHTEXT_H_

#include <string>
#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"
#include "view/font/richitem.h"
#include "view/font/richitemparser.h"
#include "view/render/batchrenderer.h"
#include "view/render/textureutil.h"

class RichText: public cocos2d::Node
{
public:
	RichText();
	~RichText();

	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags);
	virtual void onDraw(const cocos2d::Mat4 &transform, uint32_t transformFlags);

	virtual void onEnter();
	virtual void onExit();

	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	virtual void setOpacity(GLubyte opacity);

	virtual std::string getDescription() const { return "CPPRichText"; }

	static RichText * Node();
	static RichText * CreateRichText(int width, int height, const std::string& fontName, int fontSize);
	static void SetDefaultOutlineColor(unsigned int);
	static void SetDefaultShadowColor(unsigned int);
	static void SetNewlineChar(const std::string &ch_str){ newlineCharStr = ch_str; }
	static std::string GetNewlineChar(){ return newlineCharStr; }
	static void SetDefaultHorizonFontSpace(int font_spcae){ horizon_font_space = font_spcae; }

public:
	void SetText(const std::string& str);
	std::string GetText(){ return getText(); }
	void SetTextWidth(int width);
	int GetTextWidth() const { return m_nTextWidth; }

	void SetFontName(const std::string&);
	void SetFontSize(int);

	void SetTextHeight(int height);
	void SetHeightFixed(bool val);
	bool IsHeightFixed() const { return m_bHeightFixed; }
	
	void ScrollToStart();
	void ScrollToBottom();

	//…Ë÷√æ≈π¨∏Ò±≥æ∞Õº∆¨
	void SetImageData(const ImageData& image_data, float x, float y, float w, float h,
		float fOffsetX, float fOffsetY, float fWidthEx, float fHeightEx);
	float GetContentWidth() const;
	float GetContentHeight() const;
	void SetLuaTouchLink(ZQ::UINT32 link_call_fun){m_lua_link_fun_id = link_call_fun;}

	//…Ë÷√ «∑ÒÕÃ …¥•∆¡œ˚œ¢
	void SetSwallow(bool swallow);
	void SetTouchEnabled(bool value);

	//––º‰æ‡
	void SetFontSpacing(int nFontSpacing) { m_nFontSpacing = nFontSpacing; }
	int GetFontSpacing(){ return m_nFontSpacing; }

	//Œƒ◊÷ÀÆ∆Ωº‰∏Ù
	void SetHorizonFontSpace(int font_spcae){ m_horizon_font_space = font_spcae; }
	int GetHorizonFontSpcae(){ return m_horizon_font_space; }

	int GetRowNum() { return m_vItemRow.size(); }

	// …Ë÷√‘ƒ∂¡À≥–Ú
	void SetReadingOrder(unsigned int var) { m_reading_order = (RichItemParser::ReadingOrder)var;  }

private:
	bool init();
	void clearItem(RichItem* pItem);
	LinkRichItem* getLinkItem(const cocos2d::Vec2& local);
	cocos2d::Vec2 convertToWindowSpace(const cocos2d::Vec2& nodePoint);
	void setText(const char* str);
	const char* getText() const;
	void setItemPosition(const cocos2d::Vec2& pos);
	cocos2d::Vec2 getItemPosition() const { return m_ptItemPos; }

public:
	static unsigned int defaultOutlineCol;
	static unsigned int defaultShadowCol;
	static std::string newlineCharStr;
	static int horizon_font_space;

private:
	std::string m_strFontName;
	int m_nFontSize;
	std::string m_strText;
	RichItem* m_pRoot;
	int m_nTextWidth;
	int m_nTextHeight;
	int m_nItemHeight; 
	bool m_bHeightFixed;
	cocos2d::Vec2 m_ptItemPos;
	cocos2d::Vec2 m_ptCapture;
	bool m_bCapture;
	RichItemRowVec m_vItemRow;
	cocos2d::Texture2D* m_pTexture;
	ImageData m_imageData;
	float m_fOffsetX, m_fOffsetY, m_fWidthEx, m_fHeightEx;
	cocos2d::Rect m_rcTexInner;
	BatchRenderer m_renderer;
	BatchRenderer m_faceRenderer;
	int m_nFontSpacing;
	ZQ::UINT32	m_lua_link_fun_id;
	bool m_swallow;
	bool m_touchEnabled;
	cocos2d::EventListener* m_touchListener;
	cocos2d::CustomCommand _customCommand;
	int m_horizon_font_space;
	GLint m_uEntityTypeLoc;
	RichItemParser::ReadingOrder m_reading_order;
};

#endif
