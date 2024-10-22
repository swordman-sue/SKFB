#ifndef UI_BUTTONEX_H
#define UI_BUTTONEX_H

#include <string>
#include "ui/UIWidget.h"
#include "ui/UIButton.h"
#include "zqcommon/typedef.h"

class ImageData;

class UIButtonEx: public cocos2d::ui::Button
{
	DECLARE_CLASS_GUI_INFO
public:
	~UIButtonEx();
	void OnTouchEvent(cocos2d::Ref *pSender, Widget::TouchEventType type);
	bool InitWithImage(const std::string& image_file, int texType = 0);
	bool InitWithImage(const ImageData& image_data);

public:
	static UIButtonEx* create();
	static UIButtonEx* CreateWithImage(const std::string& image_file, int texType = 0);
	static UIButtonEx* CreateWithImage(const ImageData& image_data);

	void SetNormalImage(const std::string &image_file, int texType = 0);
	void SetNormalImage(const ImageData& image_data);
	void SetSelectedImage(const std::string &image_file, int texType = 0);
	void SetSelectedImage(const ImageData& image_data);
	void SetDisabledImage(const std::string &image_file, int texType = 0);
	void SetDisabledImage(const ImageData& image_data);

	void SetScale9Enabled(bool able);
	void SetCapInsets(float x, float y, float w, float h);
	void SetText(const std::string &text);
	std::string GetText();
	void SetFontName(const std::string &font_name);
	void SetFontSize(float font_size);
	void SetTextColor(GameType::UINT8, GameType::UINT8, GameType::UINT8);
	
	void AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id);
	void AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 m_cancle_fun_id);
	void SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id){ AddEventListener(down_fun_id, move_fun_id, up_fun_id); }
	// 修改SetEventListener支持处理touch_cancel事件 add by JiaMiao 2016-3-24
	void SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 m_cancle_fun_id){ AddEventListener(down_fun_id, move_fun_id, up_fun_id, m_cancle_fun_id); }
	virtual void setEnabled(bool enabled);
	bool GetEnable();

	void SetOutline(GLubyte r, GLubyte g, GLubyte b, GLubyte a, int out_line_size);
	void SetShadow(GLubyte r, GLubyte g, GLubyte b, GLubyte a, float w, float h, int blurRadius);
	void DisableEffect();

	virtual std::string getDescription() const { return "UIButtonEx"; }

protected:
	virtual Widget* createCloneInstance();

private:
	UIButtonEx();

private:
	bool m_is_enabled;
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;
	GameType::UINT32 m_cancle_fun_id;
};

#endif
