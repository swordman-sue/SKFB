#ifndef PROGRESS_TIMER_H
#define PROGRESS_TIMER_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"

class SpriteEx;
class GraphicRender;

class ProgressTimer : public cocos2d::ProgressTimer
{
public:
	/**
	typedef enum
	{
		TypdRadial = Type::RADIAL,
		TypeBar = Type::BAR,
	}ProgreessTimerType;
	**/
public:
	static ProgressTimer* CreateWithImageLabel(SpriteEx* label);
	static ProgressTimer* CreateWithGraphicRender(GraphicRender* render);
public:
	//设置进度类型
	void SetType(int progress_type)
	{
		ProgressTimer::setType((cocos2d::ProgressTimer::Type)progress_type);
	}

	//获取进度类型
	int GetType()
	{
		return (int)ProgressTimer::getType();
	}

	//设置进度
	void SetPercentage(float percentage)
	{
		ProgressTimer::setPercentage(percentage);
	}

	//获取进度
	float GetPercentage()
	{
		return ProgressTimer::getPercentage();
	}

	//设置反方向显示
	void SetReverseDirection(bool reserve)
	{
		ProgressTimer::setReverseDirection(reserve);
	}

	//是否反方向
	bool IsReverseDirection()
	{
		return ProgressTimer::isReverseDirection();
	}

	//设置颜色混合
	void SetColor(ZQ::UINT8 r, ZQ::UINT8 g, ZQ::UINT8 b)
	{
		ProgressTimer::setColor(cocos2d::Color3B(r, g, b));
	}

	//设置透明度
	void SetOpacity(ZQ::UINT8 opacity)
	{
		ProgressTimer::setOpacity(opacity);
	}

	//获取透明度
	ZQ::UINT8 GetOpacity()
	{
		return ProgressTimer::getOpacity();
	}

protected:	
	static ProgressTimer* CreateWithSprite(cocos2d::Sprite *sp);

};

#endif
