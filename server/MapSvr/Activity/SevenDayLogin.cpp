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

	// 已经超过指定天数
	if (!wDay || wDay > MAX_SEVEN_DAY_LOGIN_DAYS)
		return;

	// 检测是否已登录过或者已领取奖励
	if (m_pSevenDayLoginData->arDailyDataList[wDay-1])
		return;

	m_pSevenDayLoginData->arDailyDataList[wDay-1] = 1;
}

// 请求7天登录数据
void CSevenDayLogin::OnNetSevenDayLoginData()
{
	SendSevenDayLoginData();
}

// 领取7天登录奖励
void CSevenDayLogin::OnNetTakeSevenDayLoginReward(WORD wDay)
{
	// 已经超过指定天数
	if (!wDay || wDay > MAX_SEVEN_DAY_LOGIN_DAYS)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]要领取的天数不合法[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// 取登录配置
	const SevenDayLogin_Config *pLoginCfg = g_pCfgMgr->GetSevenDayLogin(wDay);
	if (!pLoginCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到7天登录配置[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// 检测是否已登录过
	if (!m_pSevenDayLoginData->arDailyDataList[wDay - 1])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当天没有登录不能领取[days:%d]", __FUNCTION__, wDay);
		return;
	}

	// 记录为已领取过
	m_pSevenDayLoginData->arDailyDataList[wDay - 1] = 2;

	// 发放奖励
	m_pPlayer->AddObjectList(pLoginCfg->vRewardList, TRUE, FROM_GET_SEVEN_DAY_LOGIN_REWARD);

	// 重新同步数据
	SendSevenDayLoginData();

	// 响应
	MSG_S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP msg;
	msg.m_wDay = wDay;
	m_pPlayer->SendPacket(&msg);
}

// 发送7天登录数据
void CSevenDayLogin::SendSevenDayLoginData()
{
	MSG_S2C_SEVEN_DAY_LOGIN_DATA_RESP msg;
	msg.m_Data = *m_pSevenDayLoginData;
	m_pPlayer->SendPacket(&msg);
}