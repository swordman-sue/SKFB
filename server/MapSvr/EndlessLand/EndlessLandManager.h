/**\file
�޾�֮�ع�����
\author Kevin
\date 2017/07/14
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CEndlessLandManager
{
public:
	CEndlessLandManager();
	~CEndlessLandManager();

	void Init(Player *pPlayer);

	////// ������Ϣ
public:
	// ����֮����������
	void OnNetEndlessData();

	// ����ս��
	void OnNetEnter();

	// ��ʼս��
	bool OnNetBattleStart();

	// ��ʼ����
	bool OnNetBattleOver(BYTE byIsWin, WORD wWave);

	// ɱ��
	void OnNetKillMonster(BYTE byIndex, DWORD dwMonsterId);

	// �һ���ײ
	void OnNetCollision();

	// ��ȡ��ɱ����
	void OnNetTakeKeepKillReward(WORD wKeepKillNum);

public:

	// �����޾�֮������
	void SendEndlessLandData();

	// ȡ�����ɱ��¼
	DWORD GetContinueKillNumRecord();

	// ����Ƿ�����ȡ����ɱ����
	//bool IsTakeKeepKillReward(WORD wKeepKillNum);

private:
	Player *m_pPlayer;
	bool m_bIsInBattle; // �Ƿ���ս����

	PlayerEndlessLandData *m_pEndlessLandData;
};