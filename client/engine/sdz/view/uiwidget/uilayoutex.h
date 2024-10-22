#ifndef UI_LAYOUTEX_H
#define UI_LAYOUTEX_H

#include "ui/UIWidget.h"
#include "ui/UILayout.h"
#include "zqcommon/typedef.h"

class ImageData;

class UILayoutEx : public cocos2d::ui::Layout
{
	DECLARE_CLASS_GUI_INFO

public:
	~UILayoutEx();
	void OnTouchEvent(cocos2d::Ref* target, cocos2d::ui::Widget::TouchEventType selector);
	virtual void onExit() override;

public:
	static UILayoutEx* create();
	void SetBackGroundColor(GameType::UINT32 red, GameType::UINT32 green, GameType::UINT32 blue);
	void SetBackGroundImage(const char* fileName, int texType = 0);
	void SetBackGroundImage(const ImageData& image_data);
	void SetBackGroundColorType(int type);
	void SetBackGroundImageCapInsets(float x, float y, float w, float h);

	void SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id);
	void SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancle_fun_id);
	void SetClippingSize(float width, float height);
	void SetClippingType(int clipping_type);
	void SetClippingEnable(bool enable);
	void SetLayoutType(int layout_type);

	void SetSizeType(int size_type);
	void SetSizePercent(const float percent_w, const float percent_h);

	void SetLuaUpdate(GameType::UINT32 update_fun_id, float interval = 0.0f);
	void Update(float dt);

	virtual std::string getDescription() const { return "UILayoutEx"; }

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	UILayoutEx();
private:
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;
	GameType::UINT32 m_cancle_fun_id;
	GameType::UINT32 m_update_fun_id;
};

#endif