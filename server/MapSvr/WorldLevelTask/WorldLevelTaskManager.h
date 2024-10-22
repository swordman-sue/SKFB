/**\file
����ȼ����������
\author Kevin
\date 2018/5/25
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CWorldLevelTaskMananger
{
public:
	CWorldLevelTaskMananger();
	~CWorldLevelTaskMananger();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// ��������ȼ���������
	void OnNetWorldLevelTaskData();

	// ��ȡ������
	void OnNetTakeTaskReward(WORD wTaskId);

	// ��ȡ���ս���
	void OnNetTakeFinalReward();

public:
	// ���������¼�
	void UpdateTaskEvent(WORD wTaskType, int nValue, bool isAdd = true);

	// ��������
	void UpdateTask();

	// �Ƿ��к��
	bool IsHaveRedPoint();

protected:

	// ���͵ȼ���������
	void SendWorldLevelTaskData();

	// ��������¼�
	void AddTaskEvent(WorldLevelTaskEvent &tTaskEvent);

	// ȡ�����¼�
	WorldLevelTaskEvent* GetTaskEvent(WORD wTaskId);

private:
	Player *m_pPlayer;

	// ����ȼ���������
	PlayerWorldLevelTaskData *m_PlayerWorldLevelTaskData;
};