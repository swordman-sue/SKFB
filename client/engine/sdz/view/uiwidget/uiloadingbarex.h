#ifndef UI_LOADINGBAREX_H
#define UI_LOADINGBAREX_H

#include "zqcommon/typedef.h"
#include "ui/UILoadingBar.h"
class ImageData;

class UILoadingBarEx: public cocos2d::ui::LoadingBar
{
	DECLARE_CLASS_GUI_INFO
public:
	~UILoadingBarEx();

public:
	static UILoadingBarEx* create();
	static UILoadingBarEx* CreateWithImage(const std::string& image_file);
	static UILoadingBarEx* CreateWithImage(const ImageData& image_data);

	//设置图片
	void SetImage(const std::string &image_file);
	void SetImage(const ImageData& image_data);
	//设置进度
	void SetPercent(int percent);
	//获取进度
	int GetPercent();
	//设置显示的最少进度
	void SetShowMinPercent(GameType::INT32 min_show_progress){m_min_show_progress = m_min_show_progress;}
	//设置是否是九宫格控件
	void SetScale9Enabled(bool enabled);
	//设置九宫格区域
	void SetCapInsets(float x, float y, float w, float h);
	//设置进度条的方向
	void SetDirection(int loadingbar_dir);

	virtual std::string getDescription() const { return "UILoadingBarEx"; }
protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
		UILoadingBarEx();

private:
	GameType::INT32 m_min_show_progress;
};

#endif
