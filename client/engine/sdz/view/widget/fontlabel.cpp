#include "fontlabel.h"
#include "view/struct.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"

using namespace cocos2d;

#define TEXT_WIDTH  0
#define TEXT_HEIGHT 0


FontLabel::FontLabel()
:m_line_width(0), 
m_height_max(0), 
m_is_flipx(false),
m_is_flipy(false),
m_effect(0),
m_shadow_color(Color4B(0, 0, 0, 164)),
m_outline_color(Color4B(0, 0, 0, 96)),
m_shadow_width(1.0f),
m_outline_width(1.0f)
{}

FontLabel* FontLabel::Node()
{
	FontLabel *label = new FontLabel();
	if(!label->init())
	{
		CC_SAFE_DELETE(label);
		return NULL;
	}
	label->autorelease();
	return label;
}

void FontLabel::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	LabelTTF::setColor(Color3B(red, green, blue));
}

void FontLabel::SetString(const std::string &text)
{
	if(text == ""){ return; }
	m_text = text;
	FontLabel::setString(text.c_str());
}

void FontLabel::SetFontName(const std::string &font_name)
{
	if (cocos2d::FileUtils::getInstance()->isFileExist(font_name))
	{
		float font_size = getFontSize();
		cocos2d::TTFConfig ttfConfig(font_name.c_str(), font_size, cocos2d::GlyphCollection::DYNAMIC);
		_renderLabel->setTTFConfig(ttfConfig);
	}
	else
	{
		_renderLabel->setSystemFontName(font_name);
		_contentDirty = true;
	}
	setString(m_text.c_str());
}

void FontLabel::SetFontSize(float font_size)
{
	cocos2d::TTFConfig ttfConfig(_renderLabel->getTTFConfig().fontFilePath.c_str(), font_size, cocos2d::GlyphCollection::DYNAMIC);
	_renderLabel->setTTFConfig(ttfConfig);

	_renderLabel->setSystemFontSize(font_size);
	setString(m_text.c_str());
}

void FontLabel::SetPosition(float x, float y)
{
	LabelTTF::setPosition(Vec2(x, y));
}

const char* FontLabel::GetString()
{
	return LabelTTF::getString().c_str();
}

bool FontLabel::InitWithString(const std::string &text, const std::string &font_name, float font_size)
{
	if(text == "")
	{
		return false;
	}

	bool result = LabelTTF::initWithString(text.c_str(), font_name.c_str(), font_size, Size(TEXT_WIDTH, TEXT_HEIGHT), TextHAlignment::LEFT);

	if(!result) { return false; }

	m_text = text;
	return true;
}

FontLabel* FontLabel::CreateWithString(const std::string &text, const std::string &font_name, float font_size)
{
	FontLabel *label = new FontLabel();

	if(!label->InitWithString(text, font_name, font_size))
	{
		CC_SAFE_DELETE(label);
		return NULL;
	}

	label->autorelease();
	return label;
}

//	����͸����
void FontLabel::SetOpacity(GLubyte var)
{
	LabelTTF::setOpacity(var);
}

// ����ÿ����󳤶�
void FontLabel::SetLineWidth(ZQ::UINT32 width)
{
	m_line_width = width;
	Size new_size = Size(m_line_width, m_height_max);
	LabelTTF::setDimensions(new_size);
}

// �������߶�
void FontLabel::SetHeightMax(ZQ::UINT32 height)
{
	m_height_max = height;
	Size new_size = Size(m_line_width, m_height_max);
	LabelTTF::setDimensions(new_size);
}

// ��������Ⱥ͸߶�
void FontLabel::SetTextWidthAndHeight(ZQ::UINT32 width, ZQ::UINT32 height)
{
	m_line_width = width;
	m_height_max = height;
	Size new_size = Size(m_line_width, m_height_max);
	LabelTTF::setDimensions(new_size);
}

//����ˮƽ���뷽ʽ
void FontLabel::SetTextHorizontalAlign(int align)
{
	LabelTTF::setHorizontalAlignment((cocos2d::TextHAlignment)align);
}

//���ô�ֱ���뷽ʽ
void FontLabel::SetTextVertiacalAlign(int align)
{
	LabelTTF::setVerticalAlignment((cocos2d::TextVAlignment)align);
}

//��ȡ���
float FontLabel::GetWidth()
{
	return LabelTTF::getContentSize().width;
}

//��ȡ�߶�
float FontLabel::GetHeight()
{
	return LabelTTF::getContentSize().height;
}

void FontLabel::SetFlipX(bool is_flipx)
{
	m_is_flipx = is_flipx;
	LabelTTF::setFlippedX(is_flipx);
}

void FontLabel::SetFlipY(bool is_flipy)
{
	m_is_flipy = is_flipy;
	LabelTTF::setFlippedY(is_flipy);
}

bool FontLabel::IsFlipX()
{
	return m_is_flipx;
}

bool FontLabel::IsFlipY()
{
	return m_is_flipy;
}

void FontLabel::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
	Point originPos = getPosition();
	GLubyte opacity = getOpacity();
	Color3B color = getColor();
	BlendFunc origin_blend = getBlendFunc();

	//ͶӰЧ��
	if (m_effect & EFFECT_SHADOW)
	{
		//����ͶӰ
		setColor(Color3B(m_shadow_color.r, m_shadow_color.g, m_shadow_color.b));
		GLubyte transparent = m_shadow_color.a * opacity / 255;
		setOpacity(transparent);
		setPositionX(originPos.x + (m_shadow_width / Director::getInstance()->getContentScaleFactor()));
		setPositionY(originPos.y - (m_shadow_width / Director::getInstance()->getContentScaleFactor()));
		LabelTTF::visit(renderer, parentTransform, parentFlags);
	}
	//���
	if (m_effect & EFFECT_OUTLINE)
	{
		Point outline_pt_list[] = {
			Point(-m_outline_width, 0),
			Point(0, -m_outline_width),
			Point(0, m_outline_width),
			Point(m_outline_width, 0),
		};
		setColor(Color3B(m_outline_color.r, m_outline_color.g, m_outline_color.b));
		GLubyte transparent = m_outline_color.a * opacity / 255;
		setOpacity(transparent);

		int k;
		int pt_len = sizeof(outline_pt_list) / sizeof(cocos2d::Vec2);
		for (k = 0; k < pt_len; ++k)
		{
			setPositionX(originPos.x + (outline_pt_list[k].x));
			setPositionY(originPos.y - (outline_pt_list[k].y));
			LabelTTF::visit(renderer, parentTransform, parentFlags);
		}
	}
	setColor(color);
	setOpacity(opacity);
	//setBlendFunc(origin_blend);
	setPosition(originPos);
	LabelTTF::visit(renderer, parentTransform, parentFlags);
}

//������ӰЧ��
void FontLabel::SetShadow(bool shadow)
{
	if (shadow)
	{
		m_effect = m_effect | EFFECT_SHADOW;
	}
	else
	{
		m_effect = m_effect & (~EFFECT_SHADOW);
	}
}

// ���������ɫֵ��͸��ֵ
void FontLabel::SetShadowColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	m_shadow_color = Color4B(red, green, blue, aplha);
}

//�������Ч��
void FontLabel::SetStroke(bool is_stroke)
{
	if (is_stroke)
	{
		m_effect = m_effect | EFFECT_OUTLINE;
	}
	else
	{
		m_effect = m_effect & (~EFFECT_OUTLINE);
	}
}

// ���������ɫֵ��͸��ֵ
void FontLabel::SetStrokeColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	m_outline_color = Color4B(red, green, blue, aplha);
}

void FontLabel::SetShadowOpt(bool stroke, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	SetShadowColor(red, green, blue, aplha);
	SetShadow(stroke);
}

void FontLabel::SetOutLineOpt(bool outline, ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue, ZQ::UINT8 aplha)
{
	SetStrokeColor(red, green, blue, aplha);
	SetStroke(outline);
}

void FontLabel::SetShadowWidth(float width)
{
	m_shadow_width = width;
}

void FontLabel::SetOutlineWidth(float width)
{
	m_outline_width = width;
}