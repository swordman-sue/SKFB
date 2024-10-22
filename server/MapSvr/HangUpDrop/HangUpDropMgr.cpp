#include "stdafx.h"
#include "HangUpDropMgr.h"
#include "..\Player.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"

CHangUpDropMgr::CHangUpDropMgr()
{


}

CHangUpDropMgr::~CHangUpDropMgr()
{

}

void CHangUpDropMgr::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pHangUpDropData = &pPlayer->GetRoleFullInfo().tSecondInfo.tHangUpDropData;
}

// ����һ���������
void CHangUpDropMgr::OnNetHangUpDropData()
{
	CheckAndInitDropData();

	SendHangUpDropData();
}

// ������ȡ����
void CHangUpDropMgr::OnNetObtainReward()
{
	// ���ϵͳ���ŵȼ�
	int nOpenLevel = g_pCfgMgr->GetSystemOpenLevel(160);
	if (m_pPlayer->GetLevel() < nOpenLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ŵȼ�!", __FUNCTION__);
		return;
	}

	const vector<HangUpDrop_Config>* pHangUpDroListCfg = g_pCfgMgr->GetHangUpDropList();
	if (!pHangUpDroListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����һ���������!", __FUNCTION__);
		return;
	}

	CheckAndInitDropData();

	DWORD dwMaxRewardTime = g_Hour2Second(g_GetConfigValue(316));
	DWORD dwCurTime = time(NULL);
	DWORD dwPassTime = dwCurTime - m_pHangUpDropData->dwLastObtainRewardTime;

	// �ջ��ʱ�������Ϊ12Сʱ,��������������ݾ���
	if (dwPassTime > dwMaxRewardTime){
		dwPassTime = dwMaxRewardTime;
	}

	int nRewardTimes = g_DivideBy(dwPassTime, 60);

	// ���������Ʒ
	m_TmpRewardItemList.clear();
	for (int i = 0; i < nRewardTimes; ++i)
	{
		vector<HangUpDrop_Config>::const_iterator iter = pHangUpDroListCfg->begin();
		for (; iter != pHangUpDroListCfg->end(); ++iter)
		{
			if (g_Rand() <= iter->nDropRate){
				AddRewardItem(iter->dwDropItemId, iter->dwDropItemNum);
			}
		}
	}

	// ��ӻ�õĽ���
	for (map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.begin(); iter != m_TmpRewardItemList.end(); ++iter)
	{
		m_pPlayer->AddObject(iter->first, iter->second, TRUE, FROM_HANG_UP_DROP_OBTAIN);
	}

	// ��ֹû��60������������ʱ��ᱻ����
	if (nRewardTimes)
		m_pHangUpDropData->dwLastObtainRewardTime = dwCurTime;

	SendHangUpDropData();

	// ��Ӧ
	MSG_S2C_HANG_UP_DROP_REWARD_OBTAIN_RESP msg;
	for (map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.begin(); iter != m_TmpRewardItemList.end(); ++iter)
	{
		msg.m_vRewardItemList.push_back(Item(iter->first, iter->second));
	}
	m_pPlayer->SendPacket(&msg);
}

// ���͹һ���������
void CHangUpDropMgr::SendHangUpDropData()
{
	MSG_S2C_HANG_UP_DROP_DATA_RESP msg;
	msg.m_HangUpDropData = *m_pHangUpDropData;
	m_pPlayer->SendPacket(&msg);
}

// ��ӽ�����Ʒ����
void CHangUpDropMgr::AddRewardItem(DWORD dwItemId, DWORD dwItemNum)
{
	map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.find(dwItemId);
	if (iter != m_TmpRewardItemList.end()){
		iter->second += dwItemNum;
	}
	else{
		m_TmpRewardItemList.insert(make_pair(dwItemId, dwItemNum));
	}
}

// ��Ⲣ��ʼ����������
void CHangUpDropMgr::CheckAndInitDropData()
{
	// �������ݳ�ʼ��
	if (!m_pHangUpDropData->dwLastObtainRewardTime && 
		m_pPlayer->GetLevel() >= g_pCfgMgr->GetSystemOpenLevel(160))
	{
		m_pHangUpDropData->dwLastObtainRewardTime = time(NULL);
	}
}