#ifndef UI_SLIDEREX_H
#define UI_SLIDEREX_H

#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "zqcommon/typedef.h"

class ImageData;

class UISliderEx : public cocos2d::ui::Slider
{
	DECLARE_CLASS_GUI_INFO
public:
	~UISliderEx();
	void OnTouchEvent(cocos2d::Ref *pSender, Slider::EventType type);

public:
	static UISliderEx* create();
	static UISliderEx* CreateWithImage(const std::string& bar_image_file, const std::string& ball_image_file);
	static UISliderEx* CreateWithImage(const ImageData& bar_image_data, const ImageData& ball_image_data);

	//设置滑动条图片
	void SetBarImage(const std::string &image_file);
	void SetBarImage(const ImageData& image_data);
	//设置滑动条进度图片
	void SetBarProgressImage(const std::string &image_file);
	void SetBarProgressImage(const ImageData& image_data);
	//设置滑动球图片
	void SetBarBallImage(const std::string &image_file);
	void SetBarBallImage(const ImageData& image_data);
	//设置滑动球下按图片
	void SetBarBallPressedImage(const std::string &image_file);
	void SetBarBallPressedImage(const ImageData& image_data);
	//设置滑动球无效图片
	void SetBarBallDisabledImage(const std::string &image_file);
	void SetBarBallDisabledImage(const ImageData& image_data);

	//设置进度
	void SetPercent(int percent);
	//获取进度
	int GetPercent();
	//设置是否是九宫格控件
	void SetScale9Enabled(bool enabled);
	//设置九宫格区域
	void SetCapInsets(float x, float y, float w, float h);
	//设置进度改变通知事件
	void SetEventListener(GameType::UINT32 slider_fun_id);

	virtual std::string getDescription() const { return "UISliderEx"; }
protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	UISliderEx();

private:
	GameType::UINT32 m_slider_fun_id;
};

#endif
