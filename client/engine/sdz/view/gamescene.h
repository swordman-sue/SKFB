#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
class GameLayer;

//��Ϸ���� �ɲ�ͬ�Ĳ����
class GameScene : public cocos2d::Scene
{
public:
	//����һ���յ���Ϸ����
	static GameScene* Node();

	//���UI����߼���
	//In: layer�����Ĳ�
	//order:��Ļ���˳��order��ĻᵲסorderС��
	//tag:��ı�ʾ��
	void AddLayer(GameLayer* layer);
	void AddLayer(GameLayer* layer, ZQ::INT32 order, ZQ::INT32 tag);
	
	//ɾ���Ӳ�
	void RemoveLayer(GameLayer *layer, bool is_clean);
	void RemoveLayer(ZQ::INT32 tag, bool is_clean);
	void RemoveAllLayer(bool is_clean);

	//�鿴ĳ�������Ƿ���ڳ�����
	bool IsObjectActive(cocos2d::Node *node);
};

#endif
