#include "stdafx.h"
#include "SevenDayLogin.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"


void CSevenDayLogin::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pSevenDayLoginData = &pPlayer->GetRoleFullInfo().tActivityInfo.tSevenDayLoginData;
}

void CSevenDayLogin::OnLogin()
{
	WORD wDay = m_pPlayer->GetRoleLoginDays();

	// �Ѿ�����ָ������
	if (!wDay || wDay > MAX_SEVEN_DAY_LOGIN_DAYS)
		return;

	// ����Ƿ��ѵ�¼����������ȡ����
	if (m_pSevenDayLoginData->arDailyDataList[wDay-1])
		return;

	m_pSevenDayLoginData->arDailyDataList[wDay-1] = 1;
}

// ����7���¼����
void CSevenDayLogin::OnNetSevenDayLoginData()
{
	SendSevenDayLoginData();
}

// ��ȡ7���¼����
void CSevenDayLogin::OnNetTakeSevenDayLoginReward(WORD wDay)
{
	// �Ѿ�����ָ������
	if (!wDay || wDay > MAX_SEVEN_DAY_LOGIN_DAYS)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]Ҫ��ȡ���������Ϸ�[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// ȡ��¼����
	const SevenDayLogin_Config *pLoginCfg = g_pCfgMgr->GetSevenDayLogin(wDay);
	if (!pLoginCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ����7���¼����[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// ����Ƿ��ѵ�¼��
	if (!m_pSevenDayLoginData->arDailyDataList[wDay - 1])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����û�е�¼������ȡ[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// ��¼Ϊ����ȡ��
	m_pSevenDayLoginData->arDailyDataList[wDay - 1] = 2;

	// ���Ž���
	m_pPlayer->AddObjectList(pLoginCfg->vRewardList, TRUE, FROM_GET_SEVEN_DAY_LOGIN_REWARD);

	// ����ͬ������
	SendSevenDayLoginData();

	// ��Ӧ
	MSG_S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP msg;
	msg.m_wDay = wDay;
	m_pPlayer->SendPacket(&msg);
}

// ����7���¼����
void CSevenDayLogin::SendSevenDayLoginData()
{
	MSG_S2C_SEVEN_DAY_LOGIN_DATA_RESP msg;
	msg.m_Data = *m_pSevenDayLoginData;
	m_pPlayer->SendPacket(&msg);
}