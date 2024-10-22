#ifndef PROGRESS_CONTROL_H
#define PROGRESS_CONTROL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/widget/spriteex.h"
class ImageData;

//进度条控件
class ProgressControl : public cocos2d::Layer
{
public:
	~ProgressControl();

	//  创建一个空的进度条
	static ProgressControl* Node();

	// 	使用图片创建进度条
	// 	In:loading_image_file 读取时显示的图片
	// 	In:background_image_file 进度条的背景图片
	// 	Return 返回指向已创建的ProgressControl的指针
	static ProgressControl* CreateWithImage(const std::string &loading_image_file, const std::string &background_image_file);
	static ProgressControl* CreateWithImage(const ImageData &loading_image, const ImageData &background_image);

	// 	使用图片初始化进度条
	// 	In:loading_image_file 读取时显示的图片
	// 	In:background_image_file 进度条的背景图片
	bool InitWithImage(const std::string &loading_image_file, const std::string &background_image_file);
	bool InitWithImage(const ImageData &loading_image, const ImageData &background_image);

	//设置进度条图片
	bool SetProgressImage(const std::string &loading_image_file);
	bool SetProgressImage(const ImageData &loading_image);

	//设置进度条背景图片
	bool SetBkImage(const std::string &background_image_file);
	bool SetBkImage(const ImageData &background_image);

	// 	设置进度条的总量
	// 	In:max 进度条的总量 默认为100.0
	inline void SetProgressMax(float max){
		m_max_volume = max;
		SetCurrentProgress();
	}

	// 	增加进度条进度
	// 	In:volume 增加的进度量
	// 	return: 返回增加之前的进度量
	float AddProgress(float volume);

	// 	重置进度条进度
	// 	In:volume 重置的进度
	void SetProgress(float volume);

	// 	获取当前进度
	// 	return：当前进度
	inline float GetProgress(){return m_current_volume;}
	inline float GetProgressMax(){ return m_max_volume; }

	//设置旋转
	void SetFlip(bool flip);

	virtual void onEnter();
	virtual void onExit();
protected:
	//更新函数，每帧调用一次
	virtual void update(float dt);

protected:
	ProgressControl();
	void SetCurrentProgress();

private:
	float					m_current_volume;				//当前进度量
	float					m_max_volume;					//总进度量
	float					m_dx, m_dy;						//位置偏移
	float					m_width, m_height;				//控件宽高
	SpriteEx				*m_loading_sprite;				//进度条
	SpriteEx				*m_background_sprite;			//进度条背景
	float					m_flip;							//是否翻转进度
};

#endif
