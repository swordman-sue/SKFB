/**\file
���������
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CSpriteManager
{
public:
	CSpriteManager();
	~CSpriteManager();

	void Init(Player *pPlayer);

public:
	// ����������
	void OnNetSpiritDataReq();

	// ����ǿ��
	void OnNetSpiritEnhance(DWORD dwSpiritId);

	// ����ϳ�
	void OnNetCompose(DWORD dwFragmentId);

	// ��������(todo::Ŀǰ�Ѳ�ʹ��)
	void OnNetTrainEx(DWORD dwSpriteId);

	// ��������
	void OnNetTrain(DWORD dwSpriteId, WORD wTrainTimes);

	// ��������
	void OnNetStarUp(DWORD dwSpriteId);

	// ����ף��
	void OnNetBless(WORD wBlessType, WORD wFormationPos, DWORD dwSpiritId);

	// ����ʹ��
	void OnNetSpiritUse(DWORD dwSpiritId);

	// ����֮����������
	void OnNetHeartOfWorldData();

	// ����֮������
	void OnNetHeartOfWorldLevelUp();


public:

	// �Ƿ��д˾���
	bool HaveSpirit(DWORD dwSpiritId);

	// ��Ӿ���
	bool AddSpirit(DWORD dwSpiritId, WORD wFromId = 0);

	// ��Ӿ���
	bool AddSpirit(const SpiritData &tSpriteData);

	// ���;�������
	void SendSpiritData();

	// ȡͼ���ȼ�
	WORD GetPokedexLevel(WORD wPokedexId);

	// ȡͼ��ǿ���ȼ�
	WORD GetEnhanceLevel(WORD wPokedexId);

	// �Ƿ�ͼ���Ѽ���
	bool IsPokedexActivate(WORD wPokedexId);

	// ���ͼ��
	void AddPokedex(WORD wPokedexId);

	// ��������֮������
	void SendHeartOfWorldData();

	// ȡ��ս����ID
	DWORD GetToBattleSpiritId();

	// ȡ����ǿ���ȼ�
	WORD GetSpiritEnhanceLevel(DWORD dwSpiritId);

	// ȡ������������
	WORD GetSpiritTrainTimes(DWORD dwSpiritId);

	// ȡ��������
	const SpiritData* GetSpiritEx(DWORD dwSpiritId);

	// ȡף����������
	WORD GetBlessSpriteNum();

	// ȡ��ǰף��������������
	WORD GetCurMaxBlessSpriteNum();

	// ȡף��Ӣ���б�
	void GetBlessSpriteList(vector<SpiritData> &vSpriteList);

	// ȡף������
	WORD GetBlessType(DWORD dwOldSpriteId, DWORD dwSpiritId);

	// ȡ�����б�
	const SpiritData* GetSpriteList() { return m_pSpriteInfo->arSpiritList; };

	// ȡ�����б�����
	WORD GetSpriteListNum() { return m_pSpriteInfo->wSpiritNum; };

	// ȡ���⽱���ĵ�����Ʒ��Ϣ
	void GetExtraRewardDropItemInfo(WORD wFBType, map<DWORD, int> &mItemInfoList);

	// ȡ���⽱���Ļ�����Ϣ
	void GetExtraRewardCurrencyInfo(WORD wFBType, map<int, int> &mCurrencyInfoList);

	// ȡ���⽱���Ļ���ֵ
	int GetExtraRewardCurrencyValue(const map<int, int> &mCurrencyInfoList, int nCurrencyType, int nCurrencyValue);

protected:

	// ȡ��������
	SpiritData* GetSpirit(DWORD dwSpiritId);

	// ȡ����ͼ��
	SpiritPokedex* GetPokedex(WORD wPokedexId);

private:
	Player *m_pPlayer;
	PlayerSpriteInfo *m_pSpriteInfo;
};