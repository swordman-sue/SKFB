#ifndef GAMESTATE_PREDEPENDENCY_H
#define GAMESTATE_PREDEPENDENCY_H

#include <vector>
#include <set>
#include <string>

typedef unsigned short StateId;
typedef unsigned short SinkId;

#define NULL_STATE_ID 0xffffu
#define NULL_STATE_SINK_ID 0xffffu

#define MAX_STATE_SINK_NUM 16	//最大支持的状态槽位数量
#define MAX_STATE_NUM 64			//最大支持的状态数量

class GameState;

typedef std::vector<GameState*> GameStateArray;
typedef std::set<StateId> StateIdList;
typedef std::set<SinkId> SinkIdList;
typedef std::set<std::string> StateNameList;

enum StateComposeType
{
	SCT_NORMAL=0,		//普通类型
	SCT_SUB=1,			//复合状态的子类型
	SCT_COMPOSE=2,		//复合类型
};

enum StateCallbackType
{
	CALLT_NONE = 0,				//未设置回调
	CALLT_MULIT_FUNC_OBJ = 1,	//多个分散的Lua函数对象
	CALLT_FUNC_IN_TABLE = 2,	//在Table中的Lua函数
	CALLT_C_LISTNER = 3,
};

class IGameStateListener
{
public:
	virtual void OnStateEnter(GameState* pCurState){}
	virtual void OnStateQuit(GameState* pCurState){}
	virtual void OnStateUpdate(GameState* pCurState, float elapseTime){}
	virtual void Free() = 0;
};

#endif 
