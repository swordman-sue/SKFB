#ifndef UI_TEXTFIELDEX_H
#define UI_TEXTFIELDEX_H

#include "ui/UITextField.h"
#include "view/struct.h"
#include "zqcommon/typedef.h"
#include "cocos2d.h"

class UITextFieldEx: public cocos2d::ui::TextField
{
	DECLARE_CLASS_GUI_INFO
public:
	~UITextFieldEx();
	virtual void onEnter();
	
public:
	static UITextFieldEx* create();
	static UITextFieldEx* CreateWithString(const std::string &place_holder, const std::string &font_name, int font_size);

	void SetText(const std::string &text);
	void SetFontName(const std::string &font_name);
	void SetFontSize(int font_size);
	void SetTextColor(GameType::UINT8, GameType::UINT8, GameType::UINT8);
	std::string GetText();
	void SetFilterStr(const std::string &filter_str);
	void SetTouchSize(float width, float height);
	void SetOnlyVisibleTouchArea(bool visible);

	void SetTextAreaSize(float width, float height);
	void SetPlaceHolderText(const std::string &text);
	void SetPlaceHolderColor(GameType::UINT8, GameType::UINT8, GameType::UINT8);
	void SetMaxLength(GameType::INT32 max_length);
	void SetPasswordMode(bool is_password);
	void SetTextHorizontalAlign(int align);
	void SetTextVertiacalAlign(int align);
	void SetEventListener(GameType::UINT32 attach_event_id, GameType::UINT32 detach_event_id, GameType::UINT32 insert_event_id, GameType::UINT32 delete_event_id);
	void SetTextChangedCallBack(GameType::UINT32 call_fun_id){ m_text_changed_id = call_fun_id; }

	virtual std::string getDescription() const { return "UITextFieldEx"; }

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();
	virtual bool onTextFieldInsertText(cocos2d::TextFieldTTF *pSender, const char *text, int nLen);

private:
	UITextFieldEx();
	void CreateCursorSprite();
	void ResetCursorPosition();
	void ResetVisibleSection();
	void OnTouchEvent(Ref *target, cocos2d::ui::TextField::EventType type);
	void OnTouchEventEx(cocos2d::Ref* target, cocos2d::ui::Widget::TouchEventType selector);

private:
	GameType::UINT32 m_attach_event_func_id;
	GameType::UINT32 m_detach_event_func_id;
	GameType::UINT32 m_insert_text_event_func_id;
	GameType::UINT32 m_delete_backward_event_func_id;
	GameType::UINT32 m_text_changed_id;

	cocos2d::Sprite* m_cursor_sprite;
	bool is_filter;
	std::string	m_filter_str;
	bool m_only_view_touch_area;
	cocos2d::Vec2 m_origin_pos;
};

#endif
