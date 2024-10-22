#ifndef CC_FONT_LABEL_H
#define CC_FONT_LABEL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

//文字显示控件
class FontLabel : public cocos2d::LabelTTF
{
public:
	static const ZQ::UINT32 EFFECT_NONE = 0;
	static const ZQ::UINT32 EFFECT_SHADOW = 0x10;
	static const ZQ::UINT32 EFFECT_OUTLINE = 0x20;

	static FontLabel* Node();

	// 	使用文字创建标签控件
	// 	In: text 标签文字
	//	In: font_name 字体
	//	In: font_size 字体大小
	// 	return : 指向创建的Label对象
	static FontLabel* CreateWithString(const std::string &text, const std::string &font_name, float font_size);
	static FontLabel* CreateWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	//设置文字颜色
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);

	//	设置透明度
	void SetOpacity(GLubyte var);
	
	// 	重置字符串
	// 	In: text 标签文字
	// 	In: font_name 字体
	// 	In: font_size 字体大小	
	void SetString(const std::string &text);
	void SetFontName(const std::string &font_name);
	void SetFontSize(float font_size);

	void SetPosition(float x, float y);
	// 	获取字符串
	// 	return: 当前控件的字符串
	const char* GetString();

	// 	使用文字初始化标签控件
	// 	In: text 标签文字
	// 	In: font_name 字体
	// 	In: font_size 字体大小
	bool InitWithString(const std::string &text, const std::string &font_name, float font_size);
	bool InitWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	// 设置描边效果
	void SetStroke(bool stroke);
	// 设置描边颜色值和透明值
	void SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	//设置阴影效果
	void SetShadow(bool shadow);
	// 设置阴影颜色值和透明值
	void SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	// 设置每行最大长度
	void SetLineWidth(ZQ::UINT32 width);
	// 设置最大高度
	void SetHeightMax(ZQ::UINT32 height);
	// 设置最大宽度和高度
	void SetTextWidthAndHeight(ZQ::UINT32 width, ZQ::UINT32 height);

	//设置水平对齐方式
	void SetTextHorizontalAlign(int align);

	//设置垂直对齐方式
	void SetTextVertiacalAlign(int align);

	//设置描边和阴影选项
	void SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	void SetOutLineOpt(bool outline, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	void SetShadowWidth(float width);
	void SetOutlineWidth(float width);
	//获取宽高
	float GetWidth();
	float GetHeight();

	void SetFlipX(bool is_flipx);
	void SetFlipY(bool is_flipy);
	bool IsFlipX();
	bool IsFlipY();
	void SetSwallow(bool swallow){};

	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
protected:
	FontLabel();

private:
	ZQ::UINT32	m_line_width;			//每行宽度
	ZQ::UINT32	m_height_max;			//最大高度
	std::string	m_text;					//字符串
	bool m_is_flipx;					//X翻转
	bool m_is_flipy;					//y翻转
	ZQ::UINT32	m_effect;				//label效果
	cocos2d::Color4B m_shadow_color;	//阴影颜色
	cocos2d::Color4B m_outline_color;	//描边颜色
	float m_shadow_width;
	float m_outline_width;
};

#endif
