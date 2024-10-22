#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
class GameLayer;

//游戏场景 由不同的层组成
class GameScene : public cocos2d::Scene
{
public:
	//创建一个空的游戏场景
	static GameScene* Node();

	//添加UI层或逻辑层
	//In: layer新增的层
	//order:层的绘制顺序，order大的会挡住order小的
	//tag:层的标示号
	void AddLayer(GameLayer* layer);
	void AddLayer(GameLayer* layer, ZQ::INT32 order, ZQ::INT32 tag);
	
	//删除子层
	void RemoveLayer(GameLayer *layer, bool is_clean);
	void RemoveLayer(ZQ::INT32 tag, bool is_clean);
	void RemoveAllLayer(bool is_clean);

	//查看某个对象是否存在场景内
	bool IsObjectActive(cocos2d::Node *node);
};

#endif
