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
	//���ý�������
	void SetType(int progress_type)
	{
		ProgressTimer::setType((cocos2d::ProgressTimer::Type)progress_type);
	}

	//��ȡ��������
	int GetType()
	{
		return (int)ProgressTimer::getType();
	}

	//���ý���
	void SetPercentage(float percentage)
	{
		ProgressTimer::setPercentage(percentage);
	}

	//��ȡ����
	float GetPercentage()
	{
		return ProgressTimer::getPercentage();
	}

	//���÷�������ʾ
	void SetReverseDirection(bool reserve)
	{
		ProgressTimer::setReverseDirection(reserve);
	}

	//�Ƿ񷴷���
	bool IsReverseDirection()
	{
		return ProgressTimer::isReverseDirection();
	}

	//������ɫ���
	void SetColor(ZQ::UINT8 r, ZQ::UINT8 g, ZQ::UINT8 b)
	{
		ProgressTimer::setColor(cocos2d::Color3B(r, g, b));
	}

	//����͸����
	void SetOpacity(ZQ::UINT8 opacity)
	{
		ProgressTimer::setOpacity(opacity);
	}

	//��ȡ͸����
	ZQ::UINT8 GetOpacity()
	{
		return ProgressTimer::getOpacity();
	}

protected:	
	static ProgressTimer* CreateWithSprite(cocos2d::Sprite *sp);

};

#endif
