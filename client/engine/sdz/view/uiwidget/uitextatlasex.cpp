#include "uitextatlasex.h"
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "2d/CCSpriteFrameCache.h"
#include <stdlib.h>

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UITextAtlasEx)

UITextAtlasEx::UITextAtlasEx(void) :TextAtlas()
, m_tex_type(Widget::TextureResType::LOCAL)
, m_item_width(24)
, m_item_height(32)
{
}

UITextAtlasEx::~UITextAtlasEx(void)
{
	m_num_vec.clear();
}

UITextAtlasEx* UITextAtlasEx::create()
{
	UITextAtlasEx* widget = new UITextAtlasEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UITextAtlasEx* UITextAtlasEx::CreateWithImage(const char* number_value, const char* file_name, int texType)
{
	UITextAtlasEx* widget = UITextAtlasEx::create();
	if (widget)
	{
		widget->InitWithImage(number_value, file_name, 0, 0, texType);
	}
	return widget;
}

UITextAtlasEx* UITextAtlasEx::CreateWithImage(const char* number_value, const char* file_name, int item_width, int item_height, int texType)
{
	UITextAtlasEx* widget = UITextAtlasEx::create();
	if (widget)
	{
		widget->InitWithImage(number_value, file_name, item_width, item_height, texType);
	}
	return widget;
}

void UITextAtlasEx::InitWithImage(const char* number_value, const char* file_name, int item_width, int item_height, int texType)
{
	if (item_width == 0)
	{
		item_width = m_item_width;
	}
	if (item_height == 0)
	{
		item_height = m_item_height;
	}
	m_tex_type = (Widget::TextureResType)texType;
	m_number_value = number_value;
	switch (m_tex_type)
	{
	case Widget::TextureResType::LOCAL:
	{
		SetProperty(number_value, file_name, item_width, item_height, 0);
		break;
	}
	case Widget::TextureResType::PLIST:
	{
		setProperty(number_value, file_name, item_width, item_height, "");
		break;
	}
	default:
		cocos2d::log("TextureResType error");
		break;
	}
}

void UITextAtlasEx::SetProperty(const std::string& str_value, const std::string& image_file, int item_width, int item_height, const std::string& start_char)
{
	std::string full_path = FileSystemEx::GetInstance()->GetResourcePath(image_file);
	if(full_path == "")
		return;

	setProperty(str_value, full_path, item_width, item_height, start_char);
}

void UITextAtlasEx::setProperty(const std::string& stringValue, const std::string& charMapFile, int itemWidth, int itemHeight, const std::string& startCharMap)
{
	m_tex_type = Widget::TextureResType::PLIST;
	m_number_value = stringValue;
	m_num_file = charMapFile;
	m_item_width = itemWidth;
	m_item_height = itemHeight;
	m_start_charMap = startCharMap;
	int len = stringValue.length();
	SpriteFrame *pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(charMapFile.c_str());
	m_rect = pFrame->getRect();
	for (int i = 0; i < len; ++i)
	{
		char num = stringValue[i];
		//int number = atoi(&num);
		int number = num - '0';
		//UIImageViewEx* img_numbers = UIImageViewEx::CreateWithImage(charMapFile.c_str(), (int)Widget::TextureResType::PLIST);
		cocos2d::Sprite* img_numbers = cocos2d::Sprite::createWithSpriteFrameName(charMapFile.c_str());
		m_num_vec.push_back(img_numbers);
		img_numbers->SetAnchorPoint(0.5f, 0.0f);
		img_numbers->setTextureRect(Rect(m_rect.origin.x + number * m_item_width, m_rect.origin.y, m_item_width, m_item_height));
		img_numbers->setPosition(Vec2(i * m_item_width + m_item_width / 2, 0));
		Widget::addChild(img_numbers, 0, Node::INVALID_TAG);
	}

	Node::SetContentSize((float)len * m_item_width, (float)m_item_height);
}

void UITextAtlasEx::SetString(const std::string& curr_value)
{
	if (curr_value == m_number_value)
	{
		return;
	}
	if (m_tex_type == Widget::TextureResType::LOCAL)
	{
		setString(curr_value);
	}
	else
	{
		int curr_len = curr_value.length();
		int vec_size = m_num_vec.size();
		if (curr_len >= vec_size)
		{
			int i = 0;
			for (; i < vec_size; ++i)
			{
				char num = curr_value[i];
				//int number = atoi(&num);
				int number = num - '0';
				m_num_vec[i]->setTextureRect(Rect(m_rect.origin.x + number * m_item_width, m_rect.origin.y, m_item_width, m_item_height));
				m_num_vec[i]->setPosition(Vec2(i * m_item_width + m_item_width / 2, 0));
				m_num_vec[i]->setVisible(true);
			}
			for (i = vec_size; i < curr_len; ++i)
			{
				char num = curr_value[i];
				//int number = atoi(&num);
				int number = num - '0';
				//UIImageViewEx* img_numbers = UIImageViewEx::CreateWithImage(m_num_file.c_str(), (int)Widget::TextureResType::PLIST);
				cocos2d::Sprite* img_numbers = cocos2d::Sprite::createWithSpriteFrameName(m_num_file.c_str());
				m_num_vec.push_back(img_numbers);
				img_numbers->SetAnchorPoint(0.5, 0);
				img_numbers->setTextureRect(Rect(m_rect.origin.x + number * m_item_width, m_rect.origin.y, m_item_width, m_item_height));
				img_numbers->setPosition(Vec2(i * m_item_width + m_item_width / 2, 0));
				Widget::addChild(img_numbers, 0, Node::INVALID_TAG);
			}
		}
		else
		{
			int i = 0;
			for (; i < curr_len; ++i)
			{
				char num = curr_value[i];
				//int number = atoi(&num);
				int number = num - '0';	
				m_num_vec[i]->setTextureRect(Rect(m_rect.origin.x + number * m_item_width, m_rect.origin.y, m_item_width, m_item_height));
				m_num_vec[i]->setPosition(Vec2(i * m_item_width + m_item_width / 2, 0));
				m_num_vec[i]->setVisible(true);
			}
			for (i = curr_len; i < vec_size; ++i)
			{
				m_num_vec[i]->setVisible(false);
			}
		}
		m_number_value = curr_value;
		Node::SetContentSize((float)curr_len * m_item_width, (float)m_item_height);
	}
}

// 修改UITextAtlasEx的GetString方法中对PLIST图片资源的处理 edit by JiaMiao
const char* UITextAtlasEx::GetString()
{
	if (m_tex_type == Widget::TextureResType::LOCAL)
	{
		return TextAtlas::getString().c_str();
	}
	else
	{
		return m_number_value.c_str();
	}
}

void UITextAtlasEx::setOpacity(GLubyte opacity)
{
	Widget::setOpacity(opacity);
	int vec_size = m_num_vec.size();
	for (int i = 0; i < vec_size; ++i)
	{
		m_num_vec[i]->setOpacity(opacity);
	}
}

cocos2d::ui::Widget* UITextAtlasEx::createCloneInstance()
{
	return UITextAtlasEx::create();
}

// 修复UITextAtlasEx克隆出错的问题 add by JiaMiao 2016-1-4
void UITextAtlasEx::copySpecialProperties(Widget *widget)
{
	UITextAtlasEx* labelAtlas = dynamic_cast<UITextAtlasEx*>(widget);
	if (labelAtlas)
	{
		setProperty(labelAtlas->m_number_value, labelAtlas->m_num_file, labelAtlas->m_item_width, labelAtlas->m_item_height, labelAtlas->m_start_charMap);
	}
}

void UITextAtlasEx::copyClonedWidgetChildren(Widget* model)
{

}