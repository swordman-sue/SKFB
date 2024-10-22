#include "gamescene.h"
#include "gamelayer.h"
using namespace cocos2d;

//创建一个空的游戏场景
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

//删除子层
void GameScene::RemoveLayer(GameLayer *layer, bool is_clean)
{
	removeChild(layer, is_clean);
}

void GameScene::RemoveLayer(ZQ::INT32 tag, bool is_clean)
{
	removeChildByTag(tag, is_clean);
}

//添加UI层或逻辑层
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

//查看某个节点是否存在父节点的树层次中
bool IsContainsObject(cocos2d::Node* parent, cocos2d::Node *node)
{
	Vector<Node*> children_list = parent->getChildren();

	if (children_list.empty())
	{
		return false;
	}

	//看是否在第一节点层
	if (children_list.contains(node))
	{
		return true;
	}
	//否则遍历子树
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

//查看某个对象是否存在活动场景内
bool GameScene::IsObjectActive(cocos2d::Node *node)
{
	return IsContainsObject(this, node);
}