#ifndef UI_IMAGEVIEWEX_H
#define UI_IMAGEVIEWEX_H

#include "ui/UIWidget.h"
#include "ui/UIImageView.h"
#include "zqcommon/typedef.h"
#include "renderer/CCGLProgram.h"

class ImageData;

class UIImageViewEx : public cocos2d::ui::ImageView
{
	DECLARE_CLASS_GUI_INFO

public:
	~UIImageViewEx();

public:
	static UIImageViewEx* create();
	static UIImageViewEx* CreateWithImage(const std::string& image_file, int texType = 0);
	static UIImageViewEx* CreateWithImage(const ImageData& image_data);

	void SetImage(const std::string &image_file, int texType = 0);
	void SetImage(const ImageData& image_data);
	void SetImageRect(const float x, const float y, const float w, const float h);
	void SetScaleSize(float x, float y);
	void SetScale9Enabled(bool enable);
	void SetCapInsets(float x, float y, float w, float h);

	void SetGray(bool grey);
	void SetAliasTexParameters();
	void AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancel_fun_id);

	virtual Node* getVirtualRenderer() override;
	virtual void setGLProgram(cocos2d::GLProgram *pShaderProgram);
	virtual std::string getDescription() const { return "UIImageViewEx"; }
protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;
	GameType::UINT32 m_cancel_fun_id;

	UIImageViewEx();
	void OnTouchEvent(Ref* target, Widget::TouchEventType selector);
};

#endif
