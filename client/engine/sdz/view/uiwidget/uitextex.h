#ifndef UI_LABELEX_H
#define UI_LABELEX_H

#include "ui/UIText.h"
#include "zqcommon/typedef.h"

class UITextEx : public cocos2d::ui::Text
{
	DECLARE_CLASS_GUI_INFO
public:
	~UITextEx();
public:
	static UITextEx* create();
	static UITextEx* CreateWithString(const std::string &text, const std::string &font_name, int font_size);

	void SetText(const std::string &text);
	void SetFontName(const std::string &font_name);
	void SetFontSize(int font_size);
	void SetTextColor(GameType::UINT8, GameType::UINT8, GameType::UINT8);
	std::string GetText();

	void SetStroke(bool stroke);
	void SetStrokeColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue, GameType::UINT8 aplha);
	void SetShadow(bool shadow);
	void SetShadowColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue, GameType::UINT8 aplha);
	void SetTextHorizontalAlign(int align);
	void SetTextVertiacalAlign(int align);
	void SetTextAreaSize(float width, float height);

	int GetLength();
	void DisableEffect();
	void AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancel_fun_id);

	virtual std::string getDescription() const { return "UITextEx"; }
private:
	UITextEx();
	bool InitWithString(const std::string &text, const std::string &font_name, int font_size);

protected:
	 virtual cocos2d::ui::Widget* createCloneInstance();

private:
	bool m_stroke;
	bool m_shadow;

private:
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;
	GameType::UINT32 m_cancel_fun_id;
	void addTouchEventListener(Ref* target, Widget::TouchEventType selector);
};

#endif
