#include "edittext.h"
#include "view/gamelayer.h"
#include "luaengine/luaengine.h"
#include "view/font/stringconv.h"
#include "zqcommon/logutil.h"

using namespace cocos2d;

#define MAX_TEXT_LEN 512

bool EditText::is_keyboard_show = false;
int EditText::keyboard_lock_count = 0;

ZQ::UINT32 EditText::current_edittext_fun_id = 0;

EditText::EditText():
m_is_password(false),
m_text_max(MAX_TEXT_LEN),
m_keyboard_willshow_id(0), 
m_keyboard_didshow_id(0),
m_keyboard_willhide_id(0), 
m_keyboard_didhide_id(0), 
m_capture(false), 
m_filter_str(" \n\r\n"),
m_pos_x(0), 
m_pos_y(0), 
m_offset_x(0), 
m_offset_y(0), 
m_show_length(1024), 
m_text(""), 
m_holder_text(""), 
m_text_changed_id(0),
m_touch_width_ex(0),
m_touch_height_ex(0),
m_cursor_sprite(nullptr),
_fontType(FontType::SYSTEM)
{}

EditText::~EditText()
{
	detachWithIME();
	CC_SAFE_RELEASE(m_cursor_sprite);
}

EditText* EditText::Node()
{
	EditText *edit_text = new EditText();
	
	if(!edit_text->init())
	{
		CC_SAFE_DELETE(edit_text);
		return NULL;
	}
	
	edit_text->SetFontSize(12);
	edit_text->autorelease();
	return edit_text;
}

//设置编辑框提示文字
void EditText::SetHolderText(const std::string &holder_text)
{
	m_holder_text = holder_text;
	setPlaceHolder(holder_text.c_str());
}

//设置编辑框提示文字字体
void EditText::SetFontName(const std::string &font_name)
{
	if (FileUtils::getInstance()->isFileExist(font_name))
	{
		TTFConfig config = getTTFConfig();
		config.fontFilePath = font_name;
		setTTFConfig(config);

		_fontType = FontType::TTF;
	}
	else {
		setSystemFontName(font_name);

		_fontType = FontType::SYSTEM;
	}
}

//设置编辑框提示文字大小
void EditText::SetFontSize(float font_size)
{
	if (FontType::TTF == _fontType)
	{
		TTFConfig config = getTTFConfig();
		config.fontSize = font_size;
		setTTFConfig(config);
	}
	else
	{
		setSystemFontSize(font_size);
	}
}

//设置文字框的大小
void EditText::SetContentSize(float width, float height)
{
	m_content_size.width = width;
	m_content_size.height = height;
	SetMaxShowLength(width);
	setDimensions(m_content_size.width, m_content_size.height);
}

//初始化编辑框
bool EditText::InitEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size)
{
	SetHolderText(holder_text);
	SetContentSize(width, height);
	SetFontName(font_name);
	SetFontSize(font_size);
	return true;
}

//创建一个编辑框控件
EditText* EditText::CreateEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size)
{
	EditText *edit_text = Node();

	if(edit_text->InitEditText(holder_text, width, height, font_name, font_size))
	{
		edit_text->SetAnchorPoint(0.0, 0.0);
		return edit_text;
	}

	CC_SAFE_DELETE(edit_text);
	return edit_text;
}

bool EditText::onTextFieldInsertText(TextFieldTTF *sender, const char *text, size_t nLen)
{
	//回车键关闭弹出键盘
	if(*text == '\n')
	{
		detachWithIME();
		return true;
	}

	std::string filter_text;

	ZQ::UINT32 unicode_len = StringConvHelper::unicode_length(text);
	ZQ::UINT32 index = 0;
	while(index < unicode_len) 
	{
		std::string index_str = StringConvHelper::unicode_at(text, index);
		if(m_text.length() + filter_text.length() + index_str.length() > m_text_max)
		{
			break;
		}
		filter_text = filter_text + index_str;
		++index;
	}

	if(filter_text.length() == 0)
	{
		return true;
	}

	std::string::iterator cur_iter = filter_text.begin();

	//过滤字符
	while(cur_iter != filter_text.end())
	{
		if(m_filter_str.find(*cur_iter) != std::string::npos)
		{
			cur_iter = filter_text.erase(cur_iter);
		}
		else
		{
			++cur_iter;
		}
	}

	m_text.append(filter_text);

	if(m_is_password)
	{
		TextFieldTTF::setString(TranslateToPassword(m_text).c_str());
	}
	else
	{
		TextFieldTTF::setString(m_text.c_str());
	}
	ResetCursorPosition();

	LuaEngine::GetInstance()->CallFunction(m_text_changed_id, "s", filter_text.c_str());
	return true;
}

bool EditText::onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen)
{
	if(m_text.size() == 0)
	{
		return true;
	}

	m_text.resize(m_text.length() - nLen);

	if(m_is_password)
	{
		TextFieldTTF::setString(TranslateToPassword(m_text).c_str());
	}
	else
	{
		TextFieldTTF::setString(m_text.c_str());
	}

	ResetCursorPosition();
	LuaEngine::GetInstance()->CallFunction(m_text_changed_id, "s", delText);
	return true;
}

//设置编辑框的文本
void EditText::SetText(const std::string &text)
{
	m_text = text;
	TextFieldTTF::setString(m_text.c_str());
	ResetCursorPosition();
}

void EditText::onEnter()
{
	TextFieldTTF::onEnter();
	this->setDelegate(this);
}

void EditText::onExit()
{
	TextFieldTTF::onExit();
}

void EditText::SetColor(GLubyte red, GLubyte green, GLubyte blue)
{
	TextFieldTTF::setTextColor(Color4B(red, green, blue, 255));
}

void EditText::SetOpacity(GLubyte alpha)
{
	TextFieldTTF::setOpacity(alpha);
}

void EditText::setPosition(const CCPoint& pos)
{
	SetPosition(pos.x, pos.y);
}

void EditText::SetPosition(float x, float y)
{
	m_pos_x = x;
	m_pos_y = y;
	TextFieldTTF::setPosition(ccp(x + m_offset_x, y + m_offset_y));
}

//转换成密码串
std::string EditText::TranslateToPassword(const std::string &str)
{
	std::string password_str = str;

	for(std::string::size_type k=0; k<password_str.length(); ++k)
	{
		password_str[k] = '*';
	}

	return password_str;
}

//设置对位点
void EditText::SetAnchorPoint(float x, float y)
{
	TextFieldTTF::setAnchorPoint(cocos2d::Vec2(x, y));
	SetText(m_text);
}

void EditText::SetExpand(ZQ::UINT32 width, ZQ::UINT32 height)
{
	m_touch_width_ex = width; 
	m_touch_height_ex = height;
}

//重置光标精灵的位置
void EditText::ResetCursorPosition()
{	
	ResetVisibleSection();
	
	float x_pos = 0;

	if(m_text.length() > 0)
	{
		x_pos = getContentSize().width;
	}
	Vec2 local_cursor_pos(x_pos, getContentSize().height / 2);
	Vec2 world_cursor_pos = convertToWorldSpace(local_cursor_pos);
	Vec2 parent_cursor_pos = getParent()->convertToNodeSpace(world_cursor_pos);

	if (m_cursor_sprite)
	{
		m_cursor_sprite->setPosition(parent_cursor_pos.x, parent_cursor_pos.y);
	}
}

//重设可视区域
void EditText::ResetVisibleSection()
{
	if (m_text.length() <= 0)
		setContentSize(Size(getContentSize().width, _systemFontSize));

	float width = getContentSize().width;

	//重置位置
	if(width > m_show_length)
	{
		m_offset_x = (m_show_length - width) * (1 - getAnchorPoint().x);
	}
	else
	{
		m_offset_x = 0;
	}
	SetPosition(m_pos_x, m_pos_y);
}

void EditText::keyboardWillShow(IMEKeyboardNotificationInfo& info)
{
	if(m_capture)
	{
		LuaEngine::GetInstance()->CallFunction(m_keyboard_willshow_id, "dd", info.end.size.width, info.end.size.height);
	}
	is_keyboard_show = true;
}

void EditText::keyboardDidShow(IMEKeyboardNotificationInfo& info)
{
	if(m_capture)
	{
		LuaEngine::GetInstance()->CallFunction(m_keyboard_didshow_id, "");
	}
}

void EditText::keyboardWillHide(IMEKeyboardNotificationInfo& info)
{
	if(m_capture)
	{
		LuaEngine::GetInstance()->CallFunction(m_keyboard_willhide_id, "");
	}
}

void EditText::keyboardDidHide(IMEKeyboardNotificationInfo& info)
{
	LuaEngine::GetInstance()->CallFunction(m_keyboard_didhide_id, "");
	is_keyboard_show = false;
}

//设置预定义文字颜色
void EditText::SetPlaceHolderColor(GLubyte red, GLubyte green, GLubyte blue)
{
	CCTextFieldTTF::setColorSpaceHolder(ccc3(red, green, blue));
}

bool EditText::attachWithIME()
{
	if(!m_capture)
	{
		++keyboard_lock_count;
	}

	m_capture = true;
	if (m_cursor_sprite)
	{
		m_cursor_sprite->setVisible(true);
	}
	TextFieldTTF::setPlaceHolder("");
	ResetCursorPosition();

	if(is_keyboard_show)
	{
		LuaEngine::GetInstance()->CallFunction(m_keyboard_didshow_id, "");
	}
	
	return TextFieldTTF::attachWithIME();
}

bool EditText::detachWithIME()
{
	if (m_cursor_sprite)
	{
		m_cursor_sprite->setVisible(false);
	}
	//在不显示光标时且没有输入文字时显示提示文字,需要重置文字
	if(m_is_password)
	{
		TextFieldTTF::setString(TranslateToPassword(m_text).c_str());
	}
	else
	{
		TextFieldTTF::setString(m_text.c_str());
	}
	TextFieldTTF::setPlaceHolder(m_holder_text.c_str());

	if(!m_capture)
	{
		return false;
	}

	m_capture = false;
	--keyboard_lock_count;
	
	if(keyboard_lock_count <= 0)
	{
		keyboard_lock_count = 0;
		return TextFieldTTF::detachWithIME();
	}

	return true;
}

void EditText::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
	if (!_visible || (_utf8Text.empty() && _children.empty()))
	{
		return;
	}

	if (_systemFontDirty || _contentDirty)
	{
		updateContent();
	}

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	if (!_utf8Text.empty() && _shadowEnabled && (_shadowDirty || (flags & FLAGS_DIRTY_MASK)))
	{
		_position.x += _shadowOffset.width;
		_position.y += _shadowOffset.height;
		_transformDirty = _inverseDirty = true;

		_shadowTransform = transform(parentTransform);

		_position.x -= _shadowOffset.width;
		_position.y -= _shadowOffset.height;
		_transformDirty = _inverseDirty = true;

		_shadowDirty = false;
	}

	bool visibleByCamera = isVisitableByVisitingCamera();
	if (_children.empty() && !_textSprite && !visibleByCamera)
	{
		return;
	}

	// IMPORTANT:
	// To ease the migration to v3.0, we still support the Mat4 stack,
	// but it is deprecated and your code should not rely on it
	_director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

	_beforeVisitCmdScissor.init(_globalZOrder);
	_beforeVisitCmdScissor.func = CC_CALLBACK_0(EditText::onBeforeVisitScissor, this);
	renderer->addCommand(&_beforeVisitCmdScissor);

	if (!_children.empty())
	{
		sortAllChildren();

		int i = 0;
		// draw children zOrder < 0
		for (; i < _children.size(); i++)
		{
			auto node = _children.at(i);

			if (node && node->getLocalZOrder() < 0)
				node->visit(renderer, _modelViewTransform, flags);
			else
				break;
		}

		this->drawSelf(visibleByCamera, renderer, flags);

		for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
		{
			(*it)->visit(renderer, _modelViewTransform, flags);
		}

	}
	else
	{
		this->drawSelf(visibleByCamera, renderer, flags);
	}

	_afterVisitCmdScissor.init(_globalZOrder);
	_afterVisitCmdScissor.func = CC_CALLBACK_0(EditText::onAfterVisitScissor, this);
	renderer->addCommand(&_afterVisitCmdScissor);
	
	_director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}


void EditText::onBeforeVisitScissor()
{
	glGetIntegerv(GL_SCISSOR_BOX, m_scissor_section);
	m_gl_is_scissor = glIsEnabled(GL_SCISSOR_TEST);

	GLint sx, sy, tx, ty;

	float start_x_pos = 0;
	//重置位置
	if (getContentSize().width > m_show_length)
	{
		start_x_pos = (m_show_length - getContentSize().width);
	}

	Point pt1 = ccp(-start_x_pos, 0);
	Point pt2 = ccp(-start_x_pos + m_content_size.width, m_content_size.height);
	pt1 = convertToWorldSpace(pt1);
	pt2 = convertToWorldSpace(pt2);

#if defined(__APPLE__) || defined(__ANDROID__)
	//do nothing
#elif defined(WP8)
	CCPoint translate_pt1, translate_pt2;
	cocos2d::CCSize win_size = cocos2d::Director::getInstance()->getWinSize();
	translate_pt1.x = pt1.y;
	translate_pt1.y = win_size.width - pt1.x - (pt2.x - pt1.x);
	translate_pt2.x = pt2.y - pt1.y;
	translate_pt1.y = pt2.x - pt1.x;
	pt1 = translate_pt1;
	pt2 = translate_pt2;
#else //(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	float scale_factor = cocos2d::Director::getInstance()->getOpenGLView()->getFrameZoomFactor();
	Vec2 translate_pt1, translate_pt2;
	translate_pt1.x = pt1.x * scale_factor;
	translate_pt1.y = pt1.y * scale_factor;
	translate_pt2.x = pt1.x + (pt2.x - pt1.x) * scale_factor;
	translate_pt2.y = pt1.y + (pt2.y - pt1.y) * scale_factor;
	pt1 = translate_pt1;
	pt2 = translate_pt2;

#endif

	if (m_gl_is_scissor)
	{
		sx = m_scissor_section[0] > GLint(pt1.x) ? m_scissor_section[0] : GLint(pt1.x);
		sy = m_scissor_section[1] < GLint(pt1.y) ? m_scissor_section[1] : GLint(pt1.y);
		tx = (m_scissor_section[0] + m_scissor_section[2]) > GLint(pt2.x) ? (m_scissor_section[0] + m_scissor_section[2]) : GLint(pt2.x);
		ty = (m_scissor_section[1] + m_scissor_section[3]) < GLint(pt2.y) ? (m_scissor_section[1] + m_scissor_section[3]) : GLint(pt2.y);
	}
	else
	{
		sx = GLint(pt1.x);
		sy = GLint(pt1.y);
		tx = GLint(pt2.x);
		ty = GLint(pt2.y);
	}

	if (tx - sx <= 0 || ty - sy <= 0)
		return;

	glEnable(GL_SCISSOR_TEST);
	glScissor(sx, sy, tx - sx, ty - sy);
}

void EditText::onAfterVisitScissor()
{
	//恢复裁剪状态
	if (m_gl_is_scissor)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(m_scissor_section[0], m_scissor_section[1], m_scissor_section[2], m_scissor_section[3]);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
}

cocos2d::Rect EditText::GetTouchRect()
{
	float start_x = -getAnchorPoint().x * m_content_size.width - m_offset_x - m_touch_width_ex / 2;
	float start_y = -getAnchorPoint().y * m_content_size.height - m_touch_height_ex / 2;
	float rect_w = m_content_size.width + m_touch_width_ex;
	float rect_h = m_content_size.height + m_touch_height_ex;

	return cocos2d::Rect(start_x, start_y, rect_w, rect_h);
}

void EditText::SetCursorSprite(cocos2d::Sprite *cursor_sprite)
{ 
	CC_SAFE_RELEASE(m_cursor_sprite);
	cursor_sprite->retain();
	m_cursor_sprite = cursor_sprite; 
}

