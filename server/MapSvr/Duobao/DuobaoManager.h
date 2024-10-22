/**\file
�ᱦ������
\author Kevin
\date 2016/9/19
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

// 
class CDuobaoManager
{
public:
	CDuobaoManager();
	~CDuobaoManager();

	void Init(Player *pPlayer);

public:
	// ����ᱦ����
	void OnNetDuobaoData();

	// ������սĿ���б�(todo::��Щ����)
	void OnNetTargetList(WORD wTreasureInfoId, WORD wTreasureItemId);

	// ����5�ζᱦ
	void OnNetDuoBaoFiveTimes(WORD wTreasureInfoId, WORD wTreasureItemId, WORD wRequestTimes = 5);

	// ��ȡ���Ƚ���
	void OnNetTakePregrossReward(WORD *pPregrossIdList, WORD wPregrossIdNum);
public:
	// ���Ͷᱦ����
	void SendDuobaoData();

	// ��ս��ʼ
	bool BattleStar(DWORD dwTargetId, bool bIsPlayer, WORD wTreasureInfoId, WORD wTreasureItemId);

	// ��ս����
	bool BattleOver(bool bIsWin);

	// ������ս����ʱ�� 
	void SetPeaceOverTime(DWORD dwTime);

	// �Ƿ���ս��
	bool IsInPeace();

protected:
	bool DoBattleOver(DWORD dwTargetId, WORD wTreasureInfoId, WORD wTreasureItemId, BYTE byIsPlayer, bool bIsWin, BYTE &byIsGetItem);

	// ȡ�ᱦ����
	WORD GetDoubaoTimes(WORD wQuality);

	// ���öᱦ����
	void SetDoubaoTimes(WORD wQuality, WORD wTimes);

	// �Ƿ���Ƚ�����¼
	bool IsPregrossRewardRecord(WORD wPregrossId);

	// ��ӽ��Ƚ�����¼
	void AddPregrossRewardRecord(WORD wPregrossId);
private:

	Player *m_pPlayer;
	PlayerDuobaoInfo *m_pPlayerDuobaoInfo;
};