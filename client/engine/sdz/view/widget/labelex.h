#ifndef CC_FONT_LABEL_H
#define CC_FONT_LABEL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

//文字显示控件
class LabelEx : public cocos2d::Label
{
public:
	static LabelEx* Node();

	// 	使用文字创建标签控件
	// 	In: text 标签文字
	//	In: font_name 字体
	//	In: font_size 字体大小
	// 	return : 指向创建的Label对象
	static LabelEx* CreateWithString(const std::string &text, const std::string &font_name, float font_size);
	static LabelEx* CreateWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	void InitWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height);

	//设置文字颜色
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);

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

	// 设置描边效果
	void SetStroke(bool stroke);
	// 设置描边宽度
	void SetStrokeWidth(float width);
	// 设置描边颜色值和透明值
	void SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	// 设置全局描边选项
	static void SetStrokeOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

	void SetStrokeColorEx(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width);
    static void SetStrokeOptEx(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width);

	//设置阴影效果
	void SetShadow(bool shadow);
	//设置阴影宽度（暂无实现）
	//void SetShadowWidth(float width){};
	// 设置阴影颜色值和透明值
	void SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);
	//设置全局阴影选项
	static void SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha);

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
	
	//获取宽高
	float GetWidth() const;
	float GetHeight() const;

	//设置翻转
	void SetFlipX(bool is_flipx);
	void SetFlipY(bool is_flipy);
	bool IsFlipX() const;
	bool IsFlipY() const;

	float GetContentWidth() const;
	float GetContentHeight() const;
protected:
	LabelEx();

public:
	static bool s_outline_enable;
	static cocos2d::Color4B s_outline_color;
    static ZQ::UINT8 s_outline_width;

	static bool s_shadow_enable;
	static cocos2d::Color4B s_shadow_color;

private:
	ZQ::UINT32	_line_width;			//每行宽度
	ZQ::UINT32	_height_max;			//最大高度

	bool _is_flipx;					//X翻转
	bool _is_flipy;					//y翻转

	cocos2d::Color4B _ex_outline_color;
	ZQ::UINT8 _outline_width;
	cocos2d::Color4B _ex_shadow_color;
};

#endif
