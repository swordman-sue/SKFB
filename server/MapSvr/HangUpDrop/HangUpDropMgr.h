/**\file
�һ����������
\author Kevin
\date 2019/03/20
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CHangUpDropMgr
{
public:
	CHangUpDropMgr();
	~CHangUpDropMgr();

	void Init(Player *pPlayer);

public:

	// ����һ���������
	void OnNetHangUpDropData();

	// ������ȡ����
	void OnNetObtainReward();

protected:

	// ���͹һ���������
	void SendHangUpDropData();

	// ��ӽ�����Ʒ����
	void AddRewardItem(DWORD dwItemId, DWORD dwItemNum);

	// ��Ⲣ��ʼ����������
	void CheckAndInitDropData();

private:
	Player *m_pPlayer;
	PlayerHangUpDropData *m_pHangUpDropData;

	// ��ʱ������Ʒ�б�
	map<DWORD, DWORD> m_TmpRewardItemList;
};