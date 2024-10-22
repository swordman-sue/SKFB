#ifndef EDIT_TEXT_H
#define EDIT_TEXT_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include <string>

//编辑控件
class EditText : public cocos2d::TextFieldTTF, public cocos2d::TextFieldDelegate
{
public:

	static ZQ::UINT32 current_edittext_fun_id;

	//创建一个空的编辑框控件
	static EditText* Node();

	//创建一个编辑框控件
	static EditText* CreateEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);
	//初始化编辑框
	bool InitEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);
	//设置编辑框提示文字
	void SetHolderText(const std::string &holder_text);
	//设置编辑框的文本
	void SetText(const std::string &text);
	//设置编辑框提示文字字体
	void SetFontName(const std::string &font_name);
	//设置编辑框提示文字大小
	void SetFontSize(float font_size);
	//返回编辑框的文本
	const char* GetText(){return m_text.c_str();}
	//设置预定义文字颜色
	void SetPlaceHolderColor(GLubyte red, GLubyte green, GLubyte blue);
	//设置文字颜色
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);
	//设置透明度
	void SetOpacity(GLubyte alpha);
	//设置控件的位置
	//In: x, y 控件的坐标
	virtual void setPosition(const cocos2d::Vec2& pos);
	void SetPosition(float x, float y);
	//设置文字框的大小
	void SetContentSize(float width, float height);
	//设置成密码模式
	void SetPasswordMode(bool is_password){m_is_password = is_password;}
	//设置最大长度
	void SetMaxLength(ZQ::UINT32 len){m_text_max = len;}
	//设置显示的最大长度
	void SetMaxShowLength(float show_length){m_show_length = show_length;}
	//设置对位点
	void SetAnchorPoint(float x, float y);
	//设置过滤字符
	void SetFilterWords(const std::string& words){m_filter_str = words;};
	//设置扩大区域
	void SetExpand(ZQ::UINT32 width, ZQ::UINT32 height);

	//键盘弹出消息的Lua回调
	void SetKeyBoardWillShowCallBack(ZQ::UINT32 call_fun_id){m_keyboard_willshow_id = call_fun_id;}
	void SetKeyBoardDidShowCallBack(ZQ::UINT32 call_fun_id){m_keyboard_didshow_id = call_fun_id;}
	void SetKeyBoardWillHideCallBack(ZQ::UINT32 call_fun_id){m_keyboard_willhide_id = call_fun_id;}
	void SetKeyBoardDidHideCallBack(ZQ::UINT32 call_fun_id){m_keyboard_didhide_id = call_fun_id;}	
	
	//继承自CCTextFieldDelegate
	virtual bool onTextFieldInsertText(TextFieldTTF * sender, const char * text, size_t nLen);
	virtual bool onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen);
	//继承自CCIMEDelegate的虚函数,获取弹出式键盘的消息
	virtual void keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardDidShow(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardWillHide(cocos2d::IMEKeyboardNotificationInfo& info);
	virtual void keyboardDidHide(cocos2d::IMEKeyboardNotificationInfo& info);
	
	virtual void onEnter();
	virtual void onExit();
	~EditText();

	virtual bool attachWithIME();
	virtual bool detachWithIME();

	virtual void visit(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags);

	//设置文字变更时的通知
	void SetTextChangedCallBack(ZQ::UINT32 call_fun_id){m_text_changed_id = call_fun_id;}

	void SetCursorSprite(cocos2d::Sprite *cursor_sprite);
	cocos2d::Rect GetTouchRect();

public:
	static bool is_keyboard_show;
	static int	keyboard_lock_count;

protected:
	EditText();
	//转换成密码串
	std::string TranslateToPassword(const std::string &str);
	//重置光标精灵的位置
	void ResetCursorPosition();
	//重设可视区域
	void ResetVisibleSection();

	void onBeforeVisitScissor();
	void onAfterVisitScissor();

	cocos2d::Size		m_content_size;				//编辑框区域大小
	std::string			m_text;						//编辑框内容
	std::string			m_password;					//密码串
	ZQ::UINT32			m_text_max;					//文字长度限制
	bool				m_is_password;				//是否是密码模式
	cocos2d::Sprite		*m_cursor_sprite;			//光标精灵
	bool				m_capture;					//是否锁定焦点
	std::string			m_filter_str;				//过滤字符集
	float				m_pos_x, m_pos_y;			//位置
	float				m_offset_x, m_offset_y;		//偏移量
	float				m_show_length;				//显示长度
	std::string			m_holder_text;				//编辑框默认文字
	ZQ::UINT32			m_touch_width_ex;			//增加触摸宽度
	ZQ::UINT32			m_touch_height_ex;			//增加触摸高度

	ZQ::UINT32			m_keyboard_willshow_id;		
	ZQ::UINT32			m_keyboard_didshow_id;
	ZQ::UINT32			m_keyboard_willhide_id;
	ZQ::UINT32			m_keyboard_didhide_id;
	ZQ::UINT32			m_text_changed_id;

	GLint m_scissor_section[4];
	GLboolean m_gl_is_scissor;
	cocos2d::CustomCommand _beforeVisitCmdScissor;
	cocos2d::CustomCommand _afterVisitCmdScissor;

private:
	enum class FontType
	{
		SYSTEM,
		TTF
	};
	FontType _fontType;
};

#endif
