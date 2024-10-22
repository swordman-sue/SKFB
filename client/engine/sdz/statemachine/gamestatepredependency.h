#ifndef GAMESTATE_PREDEPENDENCY_H
#define GAMESTATE_PREDEPENDENCY_H

#include <vector>
#include <set>
#include <string>

typedef unsigned short StateId;
typedef unsigned short SinkId;

#define NULL_STATE_ID 0xffffu
#define NULL_STATE_SINK_ID 0xffffu

#define MAX_STATE_SINK_NUM 16	//���֧�ֵ�״̬��λ����
#define MAX_STATE_NUM 64			//���֧�ֵ�״̬����

class GameState;

typedef std::vector<GameState*> GameStateArray;
typedef std::set<StateId> StateIdList;
typedef std::set<SinkId> SinkIdList;
typedef std::set<std::string> StateNameList;

enum StateComposeType
{
	SCT_NORMAL=0,		//��ͨ����
	SCT_SUB=1,			//����״̬��������
	SCT_COMPOSE=2,		//��������
};

enum StateCallbackType
{
	CALLT_NONE = 0,				//δ���ûص�
	CALLT_MULIT_FUNC_OBJ = 1,	//�����ɢ��Lua��������
	CALLT_FUNC_IN_TABLE = 2,	//��Table�е�Lua����
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
