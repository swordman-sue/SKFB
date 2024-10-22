#include "gamescene.h"
#include "gamelayer.h"
using namespace cocos2d;

//����һ���յ���Ϸ����
GameScene* GameScene::Node()
{
	GameScene *game_scene = new GameScene();

	if(NULL != game_scene && game_scene->init())
	{
		game_scene->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(game_scene);
	}
	return game_scene;
}

//ɾ���Ӳ�
void GameScene::RemoveLayer(GameLayer *layer, bool is_clean)
{
	removeChild(layer, is_clean);
}

void GameScene::RemoveLayer(ZQ::INT32 tag, bool is_clean)
{
	removeChildByTag(tag, is_clean);
}

//���UI����߼���
void GameScene::AddLayer(GameLayer* layer)
{
	if(NULL != layer)
	{	
		AddLayer(layer, 0, 0);
	}
}

void GameScene::AddLayer(GameLayer* layer, ZQ::INT32 order, ZQ::INT32 tag)
{
	if (NULL != layer)
	{
		addChild(layer, order, tag);
	}
}

void GameScene::RemoveAllLayer(bool is_clean)
{
	removeAllChildrenWithCleanup(is_clean);
}

//�鿴ĳ���ڵ��Ƿ���ڸ��ڵ���������
bool IsContainsObject(cocos2d::Node* parent, cocos2d::Node *node)
{
	Vector<Node*> children_list = parent->getChildren();

	if (children_list.empty())
	{
		return false;
	}

	//���Ƿ��ڵ�һ�ڵ��
	if (children_list.contains(node))
	{
		return true;
	}
	//�����������
	else
	{
		for (auto &child : children_list)
		{
			if (IsContainsObject(child, node))
			{
				return true;
			}
		}
	}
	return false;
}

//�鿴ĳ�������Ƿ���ڻ������
bool GameScene::IsObjectActive(cocos2d::Node *node)
{
	return IsContainsObject(this, node);
}