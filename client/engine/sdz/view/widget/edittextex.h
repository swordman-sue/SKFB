#ifndef EDIT_TEXT_EX_H
#define EDIT_TEXT_EX_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include <string>
#include "edittext.h"

//编辑控件
class EditTextEx : public cocos2d::Node
{
public:
	//创建一个空的编辑框控件
	static EditTextEx* Node();
	//创建一个编辑框控件
	static EditTextEx* CreateEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);

	void SetHolderText(const std::string &holder_text);
	void SetText(const std::string &text);
	void SetFontName(const std::string &font_name);
	void SetFontSize(float font_size);
	const char* GetText(){ return m_edit_text->GetText(); }
	void SetPlaceHolderColor(GLubyte red, GLubyte green, GLubyte blue);
	void SetColor(GLubyte red, GLubyte green, GLubyte blue);
	void SetOpacity(GLubyte alpha);
	void SetPosition(float x, float y);
	void SetContentSize(float width, float height);
	void SetPasswordMode(bool is_password){ m_edit_text->SetPasswordMode(is_password); }
	void SetMaxLength(ZQ::UINT32 len){ m_edit_text->SetMaxLength(len); }
	void SetMaxShowLength(float show_length){ m_edit_text->SetMaxLength(show_length); }
	void SetAnchorPoint(float x, float y);
	void SetFilterWords(const std::string& words){ m_edit_text->SetFilterWords(words); };
	void SetExpand(ZQ::UINT32 width, ZQ::UINT32 height);

	void SetKeyBoardWillShowCallBack(ZQ::UINT32 call_fun_id){ m_edit_text->SetKeyBoardWillShowCallBack(call_fun_id); }
	void SetKeyBoardDidShowCallBack(ZQ::UINT32 call_fun_id){ m_edit_text->SetKeyBoardDidShowCallBack(call_fun_id); }
	void SetKeyBoardWillHideCallBack(ZQ::UINT32 call_fun_id){ m_edit_text->SetKeyBoardWillHideCallBack(call_fun_id); }
	void SetKeyBoardDidHideCallBack(ZQ::UINT32 call_fun_id){ m_edit_text->SetKeyBoardDidHideCallBack(call_fun_id); }

	void SetTextChangedCallBack(ZQ::UINT32 call_fun_id){ m_edit_text->SetTextChangedCallBack(call_fun_id); }
	//设置是否吞噬触屏消息
	void SetSwallow(bool swallow);
	void setTouchEnabled(bool value);

	bool attachWithIME();
	bool detachWithIME();
	//继承自CCNode
	virtual void onEnter();
	virtual void onExit();
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

public:
	~EditTextEx();
protected:
	EditTextEx();
private:
	bool InitNode();
	void CreateCursorSprite();
	bool InitEditText(const std::string &holder_text, float width, float height, const std::string &font_name, float font_size);

private:
	float m_font_size;
	EditText *m_edit_text;
	cocos2d::Sprite	*m_cursor_sprite;
	bool m_swallow;
	bool m_touchEnabled;
	cocos2d::EventListener* m_touchListener;
};

#endif
