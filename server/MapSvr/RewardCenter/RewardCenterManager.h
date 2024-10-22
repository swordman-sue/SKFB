/**\file
�������Ĺ�����
\author Kevin
\date 2016/11/4
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;


class CRewardCenterManager
{
public:
	CRewardCenterManager();
	~CRewardCenterManager();

	void Init(Player *pPlayer);

public:
	// �������б�
	void OnNetRewardList();

	// ���ͽ����б�
	void SendRewardList();

	// ��ȡ����
	void OnNetRewardGet(DWORD *pRewardIdxList, BYTE byRewardNum);

	// ��ӽ���
	static void AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward);

	// ��ӽ���
	void AddReward(const RewardData &tReward, bool bNotify = true);

	// ȡ����
	bool GetReward(WORD wRewardInfoId, DWORD dwValue1, DWORD dwValue2, vector<Item> &vItemList, vector<MoneyData> &vCurrencyList, DWORD &dwTitleId);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

protected:
	// ȡ��������
	RewardData* GetRewardData(DWORD dwRewardUId);

	// ɾ����������
	void RemoveRewardData(DWORD dwRewardUId);

	DWORD GetNewId();
private:
	DWORD m_dwSeriesId; // ϵ��ID
	RewardData m_tTmpRewardData;
	Player *m_pPlayer;
	PlayerRewardData *m_pRewardInfo;
};