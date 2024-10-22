#include "uisliderex.h"
#include "luaengine/luaengine.h"
#include "zqcommon/logutil.h"
#include "view/struct.h"
#include "view/viewhelper.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UISliderEx)

UISliderEx::UISliderEx()
	:m_slider_fun_id(0)
{}

UISliderEx::~UISliderEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_slider_fun_id);
}

void UISliderEx::OnTouchEvent(cocos2d::Ref *pSender, Slider::EventType event_type)
{
	switch(event_type)
	{
	case Slider::EventType::ON_PERCENTAGE_CHANGED:
		LuaEngine::GetInstance()->CallFunction(m_slider_fun_id,"");
		break;
	}
}

UISliderEx* UISliderEx::create()
{
	UISliderEx* widget = new UISliderEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UISliderEx* UISliderEx::CreateWithImage(const std::string& bar_image_file, const std::string& ball_image_file)
{
	UISliderEx* widget = create();

	if(!widget)
		return NULL;

	widget->SetBarImage(bar_image_file);
	widget->SetBarBallImage(ball_image_file);
	return widget;
}

UISliderEx* UISliderEx::CreateWithImage(const ImageData& bar_image_data, const ImageData& ball_image_data)
{
	UISliderEx* widget = create();

	if(!widget)
		return NULL;

	widget->SetBarImage(bar_image_data);
	widget->SetBarBallImage(ball_image_data);
	return widget;
}

void UISliderEx::SetEventListener(GameType::UINT32 slider_fun_id)
{ 
	m_slider_fun_id = slider_fun_id; 
	addEventListener(CC_CALLBACK_2(UISliderEx::OnTouchEvent, this));
}

void UISliderEx::SetBarImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	Slider::loadBarTexture(full_path.c_str(), TextureResType::LOCAL);
}

void UISliderEx::SetBarImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Slider::loadBarTexture(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UISliderEx::SetBarProgressImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	Slider::loadProgressBarTexture(full_path.c_str(), TextureResType::LOCAL);
}

void UISliderEx::SetBarProgressImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Slider::loadProgressBarTexture(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UISliderEx::SetBarBallImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	Slider::loadSlidBallTextureNormal(full_path.c_str(), TextureResType::LOCAL);
}

void UISliderEx::SetBarBallImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Slider::loadSlidBallTextureNormal(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UISliderEx::SetBarBallPressedImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	Slider::loadSlidBallTexturePressed(full_path.c_str(), TextureResType::LOCAL);
}

void UISliderEx::SetBarBallPressedImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Slider::loadSlidBallTexturePressed(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UISliderEx::SetBarBallDisabledImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	Slider::loadSlidBallTextureDisabled(full_path.c_str(), TextureResType::LOCAL);
}

void UISliderEx::SetBarBallDisabledImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Slider::loadSlidBallTextureDisabled(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UISliderEx::SetPercent(int percent)
{
	Slider::setPercent(percent);
}

int UISliderEx::GetPercent()
{
	return Slider::getPercent();
}

void UISliderEx::SetScale9Enabled(bool enabled)
{
	Slider::setScale9Enabled(enabled);
}

void UISliderEx::SetCapInsets(float x, float y, float w, float h)
{
	Slider::setCapInsets(CCRectMake(x, y, w, h));
}

cocos2d::ui::Widget* UISliderEx::createCloneInstance()
{
	return UISliderEx::create(); 
}

