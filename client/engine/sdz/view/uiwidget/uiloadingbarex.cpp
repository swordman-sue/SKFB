#include "uiloadingbarex.h"
#include "view/viewhelper.h"
#include "view/struct.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UILoadingBarEx)

UILoadingBarEx::UILoadingBarEx()
:m_min_show_progress(5)
{}

UILoadingBarEx::~UILoadingBarEx(){}

UILoadingBarEx* UILoadingBarEx::create()
{
	UILoadingBarEx* widget = new UILoadingBarEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

UILoadingBarEx* UILoadingBarEx::CreateWithImage(const std::string& image_file)
{
	UILoadingBarEx* widget = create();

	if(!widget)
		return nullptr;

	widget->SetImage(image_file);
	return widget;
}

UILoadingBarEx* UILoadingBarEx::CreateWithImage(const ImageData& image_data)
{
	UILoadingBarEx* widget = create();

	if(!widget)
		return nullptr;

	widget->SetImage(image_data);
	return widget;
}

void UILoadingBarEx::SetImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	LoadingBar::loadTexture(full_path.c_str(), TextureResType::LOCAL);
}

void UILoadingBarEx::SetImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	UILoadingBarEx::loadTexture(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UILoadingBarEx::SetPercent(int percent)
{	
	setVisible(percent >= m_min_show_progress);
	LoadingBar::setPercent(percent);
}

int UILoadingBarEx::GetPercent()
{
	return LoadingBar::getPercent();
}

void UILoadingBarEx::SetScale9Enabled(bool enabled)
{
	LoadingBar::setScale9Enabled(enabled);
}

void UILoadingBarEx::SetCapInsets(float x, float y, float w, float h)
{
	LoadingBar::setCapInsets(cocos2d::CCRectMake(x,y,w,h));
}

//设置进度条的方向
void UILoadingBarEx::SetDirection(int loadingbar_dir)
{
	LoadingBar::setDirection((LoadingBar::Direction)loadingbar_dir);
}

cocos2d::ui::Widget* UILoadingBarEx::createCloneInstance()
{
	return UILoadingBarEx::create(); 
}



