#include <algorithm>
#include "labelex.h"
#include "2d/CCLabel.h"
#include "view/struct.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"
#include "platform/systemtools.h"

using namespace cocos2d;

bool LabelEx::s_outline_enable = false;
cocos2d::Color4B LabelEx::s_outline_color = Color4B(0, 0, 0, 164);
ZQ::UINT8 LabelEx::s_outline_width = 1;

bool LabelEx::s_shadow_enable = false;
cocos2d::Color4B LabelEx::s_shadow_color = Color4B(0, 0, 0, 164);

LabelEx::LabelEx()
: _line_width(0),
_height_max(0),
_is_flipx(false),
_is_flipy(false),
_ex_outline_color(s_outline_color),
_outline_width(1),
_ex_shadow_color(s_shadow_color)
{}

LabelEx * LabelEx::Node()
{
	LabelEx *fontlabelex = new LabelEx();
	if ( !fontlabelex->init() )
	{
		CC_SAFE_DELETE(fontlabelex);
		return nullptr;
	}
	fontlabelex->autorelease();

	return fontlabelex;
}

void LabelEx::InitWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height)
{
	setSystemFontName(font_name);
	setSystemFontSize(font_size);
	setDimensions(area_width, area_height);
	SetString(text);
	autorelease();

	if (s_outline_enable)
		SetStroke(true);

	if (s_shadow_enable)
		SetShadow(true);
}

LabelEx * LabelEx::CreateWithString(const std::string &text, const std::string &font_name, float font_size, float area_width, float area_height)
{
	if (text == "")
	{
		CCLOG("[LabelEx::CreateWithString] text is nil");
		return nullptr;
	}

	auto ret = new LabelEx();

#if defined(WP8)
	if (ret)
	{
		ret->InitWithString(text, font_name, font_size, area_width, area_height);
		return ret;
	}
#else
	if (ret)
	{
		// 通过TTF创建系统字
		if (Label::USE_SYSTEM_FONT_BY_TTF)
		{
			ret->InitWithString(text, font_name, font_size, area_width, area_height);
			return ret;
		}
		else if (FileUtils::getInstance()->isFileExist(font_name))
		{
			TTFConfig ttfConfig(font_name.c_str(), font_size, GlyphCollection::DYNAMIC);
			if (ret->setTTFConfig(ttfConfig))
			{
				ret->setDimensions(area_width, area_height);
				ret->SetString(text);
				ret->autorelease();

				if (s_outline_enable)
					ret->SetStroke(true);

				if (s_shadow_enable)
					ret->SetShadow(true);

				return ret;
			}
		}
	}
#endif

	delete ret;
	return nullptr;
}

LabelEx * LabelEx::CreateWithString(const std::string &text, const std::string &font_name, float font_size)
{
	return LabelEx::CreateWithString(text, font_name, font_size, 0, 0);
}

void LabelEx::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	Color3B color(red, green, blue);
	Label::setColor(color);
}

void LabelEx::SetString(const std::string &text)
{
	Label::setString(SystemTools::ConvertUygurToEx(text));
}

void LabelEx::SetFontName(const std::string &font_name)
{
	Label::setSystemFontName(font_name);
}

void LabelEx::SetFontSize(float font_size)
{
	Label::setSystemFontSize(font_size);
}

void LabelEx::SetPosition(float x, float y)
{
	Vec2 p(x, y);
	Label::setPosition(p);
}

const char* LabelEx::GetString()
{
	return Label::getString().c_str();
}

void LabelEx::SetStroke(bool is_stroke)
{
	if (!is_stroke)
		return Label::disableEffect();
	
	Label::enableOutline(_ex_outline_color, _outline_width);
}

void LabelEx::SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	SetStrokeColorEx(red, green, blue, aplha, _outline_width);
}

void LabelEx::SetStrokeColorEx(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width)
{
	//3.2的描边不透显示效果差, 
	//aplha = std::min(128, (int)aplha);
	_ex_outline_color = Color4B(red, green, blue, aplha);
	_outline_width = stroke_width;
	
#ifdef WP8
	//wp8下没有outline效果，用shadow替代
	SetShadowColor(red, green, blue, aplha);
#else
	Label::enableOutline(_ex_outline_color, _outline_width);
#endif // WP8
}

void LabelEx::SetStrokeWidth(float stroke_width)
{
	_outline_width = stroke_width;
	Label::enableOutline(_ex_outline_color, _outline_width);
}

void LabelEx::SetStrokeOpt(bool outline, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	s_outline_enable = outline;
	s_outline_color = Color4B(red, green, blue, aplha);
}

void LabelEx::SetStrokeOptEx(bool outline, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha, ZQ::UINT8 stroke_width)
{
	s_outline_enable = outline;
	s_outline_color = Color4B(red, green, blue, aplha);
	s_outline_width = stroke_width;
}

void LabelEx::SetShadow(bool shadow)
{
	if (!shadow)
		return Label::disableEffect();

	Label::enableShadow(_ex_shadow_color);
}

void LabelEx::SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	_ex_shadow_color = Color4B(red, green, blue, aplha);
	Label::enableShadow(_ex_shadow_color);
}

void LabelEx::SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	s_shadow_color = Color4B(red, green, blue, aplha);
}

void LabelEx::SetLineWidth(unsigned int max_line_width)
{
	_line_width = max_line_width;
	Label::setWidth(_line_width);
}

void LabelEx::SetHeightMax(ZQ::UINT32 height_max)
{
	_height_max = height_max;
	Label::setHeight(_height_max);
}

void LabelEx::SetTextWidthAndHeight(ZQ::UINT32 width, ZQ::UINT32 height)
{
	_line_width = width;
	_height_max = height;

	Label::setDimensions(_line_width, _height_max);
}

void LabelEx::SetTextHorizontalAlign(int align)
{
	Label::setHorizontalAlignment((TextHAlignment)align);
}

void LabelEx::SetTextVertiacalAlign(int align)
{
	Label::setVerticalAlignment((TextVAlignment)align);
}

float LabelEx::GetWidth() const
{
	return Label::getBoundingBox().size.width;
}

float LabelEx::GetHeight() const
{
	return Label::getBoundingBox().size.height;
}

float LabelEx::GetContentWidth() const
{
	return Label::getContentSize().width;
}

float LabelEx::GetContentHeight() const
{
	return Label::getContentSize().height;
}

void LabelEx::SetFlipX(bool is_flipx)
{
	if (is_flipx == _is_flipx)
		return;

	_is_flipx = is_flipx;

	if (is_flipx)
	{
		Label::setScaleX(1.0);
	}
	else
	{
		Label::setScaleX(-1.0);
	}
}

void LabelEx::SetFlipY(bool is_flipy)
{
	if (is_flipy == _is_flipy)
		return;

	_is_flipy = is_flipy;

	if (is_flipy)
	{
		Label::setScaleY(1.0);
	}
	else
	{
		Label::setScaleY(-1.0);
	}
}

bool LabelEx::IsFlipX() const
{
	return _is_flipx;
}

bool LabelEx::IsFlipY() const
{
	return _is_flipy;
}




