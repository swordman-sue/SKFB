#ifndef PROGRESS9_CONTROL_H
#define PROGRESS9_CONTROL_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/widget/scale9spriteex.h"

class ImageData;

namespace cocos2d
{
	class DrawNode;
	class ClippingNode;
}

//进度条控件
class Progress9Control : public cocos2d::Layer
{
public:
	~Progress9Control();

	//创建一个空的进度条
	static Progress9Control* Node();

	//使用图片创建进度条
	//In:loading_image_file 读取时显示的图片
	//In:background_image_file 进度条的背景图片
	//Return 返回指向已创建的ProgressControl的指针
	static Progress9Control* CreateWithImage(const std::string &loading_image_file);
	static Progress9Control* CreateWithImage(const ImageData &loading_image);

	//设置进度条图片
	bool SetProgressImage(const std::string &loading_image_file);
	bool SetProgressImage(const ImageData &loading_image);

	//设置进度条的总量
	//In:max 进度条的总量 默认为100.0
	inline void SetProgressMax(float max) 
	{ 
		m_max_volume = max; 
	}

	//重置进度条进度
	//In:volume 重置的进度
	inline void SetProgress(float volume)
	{
		if (volume != m_current_volume)
		{
			m_bVolumeDirty = true;
			m_current_volume = volume;
		}
	}

	//增加进度条进度
	//In:volume 增加的进度量
	//return: 返回增加之前的进度量
	float AddProgress(float volume);

	//获取当前进度
	//return：当前进度
	inline float GetProgress() { return m_current_volume; }

protected:
	//更新函数，每帧调用一次
	virtual void update(float dt);

protected:
	Progress9Control();

private:
	float					m_current_volume;				//当前进度量
	float					m_max_volume;					//总进度量
	Scale9SpriteEx*			m_loading_sprite;				//进度条

	//添加模板裁剪处理进度变化的模式 add by swordman sue 2016-11-5
	bool					m_bVolumeDirty;					//进度脏位
	cocos2d::DrawNode*		m_pClippingStencil;				//模板节点
	cocos2d::ClippingNode*	m_pClippingNode;
};

#endif
