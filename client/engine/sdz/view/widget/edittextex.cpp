#include "edittextex.h"
#include "view/gamelayer.h"
#include "luaengine/luaengine.h"
#include "view/font/stringconv.h"
#include "zqcommon/logutil.h"

using namespace cocos2d;

EditTextEx::EditTextEx()
:m_edit_text(nullptr),
m_cursor_sprite(nullptr),
m_touchListener(nullptr),
m_font_size(12),
m_swallow(true),
m_touchEnabled(false)
{}

EditTextEx::~EditTextEx()
{}

EditTextEx* EditTextEx::Node()
{
	EditTextEx *edit_text = new EditTextEx();
	if (!edit_text->init() || !edit_text->InitNode())
	{
		CC_SAFE_DELETE(edit_text);
		return nullptr;
	}
	
	edit_text->SetFontSize(12);
	edit_text->autorelease();
	return edit_text;
}

//创建一个编辑框控件
EditTextEx* EditTextEx::CreateEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size)
{
	EditTextEx *edit_text = EditTextEx::Node();

	if (edit_text && edit_text->InitEditText(holder_text, width, height, font_name, font_size))
	{
		edit_text->SetAnchorPoint(0.0, 0.0);
		return edit_text;
	}

	CC_SAFE_DELETE(edit_text);
	return nullptr;
}

bool EditTextEx::InitNode()
{
	m_edit_text = EditText::Node();
	if (!m_edit_text)
	{
		CC_SAFE_DELETE(m_edit_text);
		m_edit_text = nullptr;
		return false;
	}
	addChild(m_edit_text);
	return true;
}

//初始化编辑框
bool EditTextEx::InitEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size)
{
	if (!m_edit_text)
	{
		return false;
	}
	m_edit_text->SetHolderText(holder_text);
	m_edit_text->SetContentSize(width, height);
	m_edit_text->SetFontName(font_name);
	m_edit_text->SetFontSize(font_size);
	return true;
}

void EditTextEx::onEnter()
{
	cocos2d::Node::onEnter();
	setTouchEnabled(true);
}

void EditTextEx::onExit()
{
	setTouchEnabled(false);
	cocos2d::Node::onExit();
}

//设置编辑框提示文字
void EditTextEx::SetHolderText(const std::string &holder_text)
{
	m_edit_text->SetHolderText(holder_text);
}

//设置编辑框的文本
void EditTextEx::SetText(const std::string &text)
{
	m_edit_text->SetText(text);
}

//设置编辑框提示文字字体
void EditTextEx::SetFontName(const std::string &font_name)
{
	m_edit_text->SetFontName(font_name);
}

//设置编辑框提示文字大小
void EditTextEx::SetFontSize(float font_size)
{
	m_font_size = font_size;
	m_edit_text->SetFontSize(font_size);
	//CreateCursorSprite();
}

//设置预定义文字颜色
void EditTextEx::SetPlaceHolderColor(GLubyte red, GLubyte green, GLubyte blue)
{
	m_edit_text->SetPlaceHolderColor(red, green, blue);
}

void EditTextEx::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	m_edit_text->SetColor(red, green, blue);
}

void EditTextEx::SetOpacity(GLubyte alpha)
{
	m_edit_text->SetOpacity(alpha);
}

void EditTextEx::SetPosition(float x, float y)
{
	Node::setPosition(Vec2(x, y));
}

//设置文字框的大小
void EditTextEx::SetContentSize(float width, float height)
{
	m_edit_text->SetContentSize(width, height);
}

//设置对位点
void EditTextEx::SetAnchorPoint(float x, float y)
{
	m_edit_text->SetAnchorPoint(x, y);
}

void EditTextEx::SetExpand(ZQ::UINT32 width, ZQ::UINT32 height)
{
	m_edit_text->SetExpand(width, height);
}

void EditTextEx::SetSwallow(bool swallow)
{
	if (swallow == m_swallow)
	{
		return;
	}

	m_swallow = swallow;

	setTouchEnabled(false);
	setTouchEnabled(true);
}

bool EditTextEx::attachWithIME()
{
	return m_edit_text->attachWithIME();
}

bool EditTextEx::detachWithIME()
{
	return m_edit_text->detachWithIME();
}


bool EditTextEx::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (!isVisible())
	{
		return false;
	}

	//必须点击在父窗口的有效区内才生效
	cocos2d::Node *parent = this->getParent();
	if (parent && typeid(*parent) == typeid(GameLayer))
	{
		GameLayer* parent_layer = dynamic_cast<GameLayer*>(parent);
		if (!parent_layer->IsValidTouchPos(touch))
		{
			return false;
		}
	}
	//从屏幕坐标转换成目前的opengl坐标
	Vec2 current_touch_point = touch->getLocation();
	current_touch_point = convertToNodeSpace(current_touch_point);

	//查看落点是否在停留在层上
	Rect rect = m_edit_text->GetTouchRect();
	if (rect.containsPoint(current_touch_point))
	{
		attachWithIME();
	}
	else
	{
		detachWithIME();
	}

	return false;
}

void EditTextEx::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void EditTextEx::setTouchEnabled(bool enabled)
{
	if (m_touchEnabled == enabled)
	{
		return;
	}

	m_touchEnabled = enabled;

	if (enabled)
	{
		auto listener = EventListenerTouchOneByOne::create();

		listener->setSwallowTouches(m_swallow);
		listener->onTouchBegan = CC_CALLBACK_2(EditTextEx::onTouchBegan, this);
		listener->onTouchEnded = CC_CALLBACK_2(EditTextEx::onTouchEnded, this);

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		m_touchListener = listener;
	}
	else
	{
		_eventDispatcher->removeEventListener(m_touchListener);
		m_touchListener = nullptr;
	}
}

//创建光标精灵
void EditTextEx::CreateCursorSprite()
{
	if (m_cursor_sprite)
	{
		this->removeChild(m_cursor_sprite, true);
		m_cursor_sprite = nullptr;
	}

	const GameType::UINT32 cursor_width = 2;
	GameType::UINT32 cursor_height = (ZQ::UINT32)(m_font_size * cocos2d::Director::getInstance()->getContentScaleFactor()) + 15;
	int total = cursor_width * cursor_height;
	unsigned int* data = new unsigned int[total];
	for (int i = 0; i < total; ++i)
	{
		data[i] = 0xfffffff0;
	}

	Texture2D *texture = new Texture2D();
	texture->initWithData(data, total, Texture2D::PixelFormat::RGB888, cursor_width, cursor_height, cocos2d::Size(cursor_width, cursor_height));
	texture->autorelease();

	delete[] data;

	m_cursor_sprite = Sprite::createWithTexture(texture);
	m_cursor_sprite->setAnchorPoint(ccp(0.0, 0.5));
	this->addChild(m_cursor_sprite);
	m_cursor_sprite->setVisible(false);

	RepeatForever *cursor_action = RepeatForever::create((ActionInterval *)Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f), NULL));
	m_cursor_sprite->runAction(cursor_action);

	m_edit_text->SetCursorSprite(m_cursor_sprite);
}