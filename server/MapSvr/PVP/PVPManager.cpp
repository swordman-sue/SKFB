#include "stdafx.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "PVPManager.h"
#include "..\Spirit\SpiritManager.h"
#include "..\Rank\RankManager.h"
#include "..\Activity\ActivityManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Title\TitleManager.h"
#include "ToolKit\MD5.h"

CPVPManager::CPVPManager()
{

}

CPVPManager::~CPVPManager()
{

}

void CPVPManager::Init()
{
	m_dwWaitBattleTimeMax = g_GetConfigValue(154);
	m_dwBattleLimitTime = g_Minute2Second(g_GetConfigValue(155));
	
	m_RankTitleTimer.SetWeekDay(g_GetConfigValue(189));
	m_RankTitleTimer.AddTriggerTime(g_GetConfigValue(190));
	
	m_RankRewardTimer.AddTriggerTime(g_GetConfigValue(191));

	//  for test
	//m_dwBattleLimitTime = 20;
}

void CPVPManager::Release()
{

}

void CPVPManager::Update()
{
	// �ȴ���ս�б�
	DWORD dwCurTime = time(NULL);
	deque<PVPWaitBattleRoleData>::iterator iter = m_WaitBattleRoleDataList.begin();
	for (; iter != m_WaitBattleRoleDataList.end(); ++iter)
	{
		if (dwCurTime >= iter->dwTime + m_dwWaitBattleTimeMax)
		{
			ReadyToBattleWithRobot(iter->dwRoleId);
			RemoveWaitBattleRoleData(iter->dwRoleId);
			break;
		}
	}

	// ս���б�(һ��ֻ����һ��)
	vector<PVPBattlefieldData>::iterator battlefield_iter = m_BattlefieldList.begin();
	for (; battlefield_iter != m_BattlefieldList.end(); ++battlefield_iter)
	{

		// ʱ���ѵ�,��ƽ��
		if (dwCurTime >= battlefield_iter->dwStartTime + m_dwBattleLimitTime)
		{
			PVPRoleBattleData *pRedBattleData = GetRoleBattleData(&*battlefield_iter, (BYTE)PVP_ACT_RED);
			PVPRoleBattleData *pBlueBattleData = GetRoleBattleData(&*battlefield_iter, (BYTE)PVP_ACT_BLUE);
			if (!pRedBattleData || !pBlueBattleData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ս���Ľ�ɫս������!!!!!!", __FUNCTION__);
				continue;
			}
			// �췽���һ���Ƿǻ�����
			Player *pRedPlayer = PlayerManager::Instance()->FindPlayerByCharID(pRedBattleData->dwRoleId);
			if (!pRedPlayer)
			{
				MAPLOG(ERROR_LEVEL, "[%s]ս����ĺ췽���û����[roleid:%d]", __FUNCTION__, pRedBattleData->dwRoleId);
				continue;
			}
			if (battlefield_iter->bIsRobot)
			{
				DoBattleOverWithRobot(pRedPlayer, pBlueBattleData->dwFightingPower, BATTLE_RESULT_DRAW);
			}
			else
			{
				Player *pBluePlayer = PlayerManager::Instance()->FindPlayerByCharID(pBlueBattleData->dwRoleId);
				if (!pBluePlayer)
				{
					MAPLOG(ERROR_LEVEL, "[%s]ս������������û����[roleid:%d]", __FUNCTION__, pBlueBattleData->dwRoleId);
					continue;
				}
				DoBattleOverWithPlayer(pRedPlayer, pBluePlayer, 0);
			}
			// һ��ֻ����һ��,�����˳�
			break;
		}
	}

	// �������н���
	if (m_RankRewardTimer.IsTrigger())
	{
		DoRankAward();
		m_RankRewardTimer.SetCDOverTime();
	}

	// �ƺŽ���
	if (m_RankTitleTimer.IsTrigger())
	{
		DoRankTitle();
		m_RankTitleTimer.SetCDOverTime();
	}
}

// �ǳ�
void CPVPManager::OnLogout(Player *pPlayer)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// ����ڵȴ�ս�����б���
	if (FindWaitBattleRoleData(dwRoleId))
	{
		RemoveWaitBattleRoleData(dwRoleId);
		return;
	}

	// �����ս����
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(dwRoleId);
	if (pBattlefieldData)
	{
		DWORD dwTargetId = GetTargetId(pBattlefieldData, dwRoleId);
		PVPRoleBattleData *pRoleBattleData = GetRoleBattleData(pBattlefieldData, dwTargetId);
		if (!pRoleBattleData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫս������[role_id:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// ��ս���ǻ�����
		if (pBattlefieldData->bIsRobot)
		{
			DoBattleOverWithRobot(pPlayer, pRoleBattleData->dwFightingPower, BATTLE_RESULT_LOSE);
		}
		// ��ս�������
		else
		{
			Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
			if (!pTarget)
			{
				// ɾ��ս��
				//RemoveBattlefieldData(dwRoleId);
				MAPLOG(ERROR_LEVEL, "[%s]PVP��սĿ��û����!!![target_id:%d]", __FUNCTION__, dwTargetId);
				return;
			}

			BYTE bySrcActType = GetBattlefieldActType(pBattlefieldData, dwRoleId);
			if (PVP_ACT_RED == bySrcActType)
				DoBattleOverWithPlayer(pPlayer, pTarget, dwTargetId);
			else
				DoBattleOverWithPlayer(pTarget, pPlayer, dwTargetId);
		}
	}

}

// �������PVP����
void CPVPManager::OnNetPlayerPVPData(Player *pPlayer)
{
	SendPlayerPVPData(pPlayer);
}

// ����Ŀ��
void CPVPManager::OnNetSearchTarget(Player *pPlayer, DWORD dwSpiritSkillId)
{
	WORD wFromMsgId = C2S_PVP_SEARCH_TARGET_REQ;

	// ����Ƿ��ڻ�ڼ�
	if (!IsActivityOpen())
	{
		pPlayer->SendErrorMsg(ERROR_PVP_ACTIVITY_CLOSE);
		MAPLOG(GUI_LEVEL, "[%s]PVP���ľ�п���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ��Ѿ��ȴ�ս���б�
	if (FindWaitBattleRoleData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ��ڵȴ���ս�б�[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ��Ѿ���ս��
	if (FindBattlefieldData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ�����ս����...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ��Ӧ
	MSG_S2C_PVP_SEARCH_TARGET_RESP msg;
	msg.m_dwSpiritSkillId = dwSpiritSkillId;
	pPlayer->SendPacket(&msg);

	DWORD dwFightingPower = pPlayer->GetFightingPower();
	DWORD dwTargetId = SelectTargetByFightingPower(dwFightingPower);

	// �ҵ�Ŀ��,�����Ͻ���ս��
	if (dwTargetId)
	{
		if (!ReadyToBattleWithPlayer(pPlayer->GetRoleId(), dwTargetId, dwSpiritSkillId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]׼������ս��ʧ��[role_id:%d,target_id:%d]",
				__FUNCTION__, pPlayer->GetRoleId(), dwTargetId);
		}

		RemoveWaitBattleRoleData(dwTargetId);
		RemoveWaitBattleRoleData(pPlayer->GetRoleId());
	}
	// û���ҵ�Ŀ��,�����ȴ��б�
	else
	{
		PVPWaitBattleRoleData tTargetData;
		tTargetData.dwRoleId = pPlayer->GetRoleId();
		tTargetData.dwTime = time(NULL);
		tTargetData.dwSpiritSkillId = dwSpiritSkillId;
		tTargetData.dwFightingPower = dwFightingPower;
		m_WaitBattleRoleDataList.push_back(tTargetData);
	}
}

// ȡ��ս��
void CPVPManager::OnNetCancelSearchTarget(Player *pPlayer)
{
	WORD wFromMsgId = C2S_PVP_CANCEL_SEARCH_TARGET_REQ;

	MSG_S2C_PVP_CANCEL_SEARCH_TARGET_RESP msg;

	// ����Ƿ���ս����
	if (FindBattlefieldData(pPlayer->GetRoleId()))
	{
		msg.m_byIsInBattle = true;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]����Ѿ�����ս����...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ���ƥ��ս��	
	if (!FindWaitBattleRoleData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]���û����ƥ��ս����...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	RemoveWaitBattleRoleData(pPlayer->GetRoleId());

	// ��Ӧ
	msg.m_byIsInBattle = false;
	pPlayer->SendPacket(&msg);
}

// ׼�����
void CPVPManager::OnNetReadyFinish(Player *pPlayer)
{
	WORD wFromMsgId = C2S_PVP_READY_FINISH_REQ;

	// ����Ƿ���׼��״̬
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û��׼��״̬[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ���ս����
	if (pBattleData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ���ս����[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ��ս������
	if (pBattleData->bIsRobot)
	{
		MSG_S2C_PVP_READY_FINISH_RESP resp;
		pPlayer->SendPacket(&resp);

		pBattleData->bIsInBattle = true;

		// ��ʼս��
		DoBattleStart(pPlayer, NULL);
	}
	// ��ս�������
	else
	{
		// ȡ��սĿ���ID
		DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (!pTarget)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]��սĿ��û����[target_id:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		MSG_S2C_PVP_READY_FINISH_RESP resp;
		pPlayer->SendPacket(&resp);

		bool bStartBattle = true;
		for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
		{
			PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[i];

			// ����׼��״̬
			if (tRoleData.dwRoleId == pPlayer->GetRoleId())
				tRoleData.bIsReady = true;

			// �����Ƿ���Կ�ʼս��
			if (!tRoleData.bIsReady)
				bStartBattle = false;
		}

		// �Ƿ�ʼս��
		if (bStartBattle)
		{
			pBattleData->bIsInBattle = true;

			// ��ʼս��
			DoBattleStart(pPlayer, pTarget);
		}
	}
}

// ʹ�ü���
void CPVPManager::OnNetUseSkill(Player *pPlayer, DWORD dwObjId, DWORD dwSkillId, WORD wX, WORD wY)
{
	WORD wFromMsgId = C2S_PVP_USE_SKILL_REQ;

	// ����Ƿ���׼��״̬
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û����ս����[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��սĿ��û����[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	BYTE byActType = GetBattlefieldActType(pBattleData, pPlayer->GetRoleId());
	if (PVP_ACT_INVALID == byActType)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���������ͳ���[target_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwRealOjbId = dwObjId * 0.1;

	BYTE byOjbType = GetObejctTypeById(dwRealOjbId);

	// ���ʹ��Դ�Ƿ񻹻���(ֻ��Ӣ�۲���Ҫ���)
	if (OBJECT_HERO == byOjbType && IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwRealOjbId))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�ͷż���Ӣ���ѹ�[role_id:%d, hero_info_id:%d]", 
			__FUNCTION__, pPlayer->GetRoleId(), dwRealOjbId);
		return;
	}

	MSG_S2C_PVP_USE_SKILL_RESP resp;
	resp.m_dwObjId = dwObjId;
	resp.m_dwSkillId = dwSkillId;
	resp.m_wX = wX;
	resp.m_wY = wY;
	pPlayer->SendPacket(&resp);

	MSG_S2C_PVP_USE_SKILL_NOTIFY notify;
	notify.m_dwObjId = dwObjId;
	notify.m_dwSkillId = dwSkillId;
	notify.m_wX = wX;
	notify.m_wY = wY;
	pTarget->SendPacket(&notify);
}

// Ѫ���仯
void CPVPManager::OnNetHPChange(Player *pPlayer, DWORD dwSrcObjId, DWORD dwTgtObjId, int nChangeHP, int nDamageType, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(GUI_LEVEL, "OnNetHPChange[RoleId:%d,SrcObjId:%d,TgtObjId:%d,ChangeHP:%d,DamageType:%d,ClientPower:%d]",
		pPlayer->GetRoleId(), dwSrcObjId, dwTgtObjId, nChangeHP, nDamageType, dwClientPower);

	// role + power + hp + cost_time + "HelloWorld"
	char szBuff[128] = { 0 };
	sprintf_s(szBuff, sizeof(szBuff), "%d%d%d%s", pPlayer->GetRoleId(), dwClientPower, nChangeHP, "HelloWorld");
	string strMd5Sign = MD5::Encode(szBuff).HexString();

	// ǩ������,�п��ܱ��İ�
	if (strcmp(strMd5Sign.c_str(), pSign) != 0)
	{
		// ��Ŵ���
		//pPlayer->LockAccount();

		pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]ս�����������ǩ���쳣[role_id:%d,server_sign:%s,client_sign:%s]",
			__FUNCTION__,pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
		return;
	}

	// ���ս��
	DWORD dwServerPower = pPlayer->GetFightingPower();
	if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
	{
		// ��Ŵ���
		pPlayer->LockAccount();
		pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]ս�����������ս���쳣[role_id:%d,server_power:%d,client_power:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), dwServerPower, dwClientPower);
		return;
	}

	// dwSrcObjId ����Ϊ0

	WORD wFromMsgId = C2S_PVP_HP_CHANGE_REQ;

	// ����Ƿ���׼��״̬
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û����ս����[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��սĿ��û����[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	DWORD dwSrcHeroInfoId = dwSrcObjId * 0.1;
	// ����˺�Դ�Ƿ񻹻���
	if (dwSrcHeroInfoId && IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwSrcHeroInfoId))
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����˺���Ӣ���ѹ�[role_id:%d, hero_info_id:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), dwSrcHeroInfoId);
		return;
	}

	// ȡ��������
	BYTE byTargetActType = GetBattlefieldActType(pBattleData, dwTargetId);
	if (PVP_ACT_INVALID == byTargetActType)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���������ͳ���[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// Ŀ��Ӣ����������
	BYTE byHeroActType = dwTgtObjId % 10;
	if (byHeroActType >= 2)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]Ѫ���仯��Ӣ����Ӫ���ͳ���[TgtObjId:%d]", __FUNCTION__, dwTgtObjId);
		return;
	}

	// ����HP(�п��ܶ�Ŀ���˺�,Ҳ�п����Ǹ��Լ�����)
	DWORD dwTgtHeroInfoId = dwTgtObjId * 0.1;
	UpateHeroHP(pBattleData, byHeroActType, dwTgtHeroInfoId, nChangeHP);

	MSG_S2C_PVP_HP_CHANGE_RESP resp;
	resp.m_dwSrcObjId = dwSrcObjId;
	resp.m_dwTgtObjId = dwTgtObjId;
	resp.m_nChangeHP = nChangeHP;
	resp.m_nDamageType = nDamageType;
	pPlayer->SendPacket(&resp);

	MSG_S2C_PVP_HP_CHANGE_NOTIFY notify;
	notify.m_dwSrcObjId = dwSrcObjId;
	notify.m_dwTgtObjId = dwTgtObjId;
	notify.m_nChangeHP = nChangeHP;
	notify.m_nDamageType = nDamageType;
	pTarget->SendPacket(&notify);

	// ���е�Ӣ�۶��ѹҵ�
	if (IsAllHeroDead(pBattleData, byHeroActType/*byTargetActType*/))
	{
		// ����˺���Ӣ����Ŀ��,�����ǻ�ʤ��
		DWORD dwWinnerId = (byHeroActType == byTargetActType) ? pPlayer->GetRoleId() : dwTargetId;

		// ս������
		if (PVP_ACT_RED == byTargetActType)
			DoBattleOverWithPlayer(pTarget, pPlayer, dwWinnerId/*pPlayer->GetRoleId()*/);
		else
			DoBattleOverWithPlayer(pPlayer, pTarget, dwWinnerId/*pPlayer->GetRoleId()*/);
	}

	// for test
// 	BYTE bySrcActType = GetBattlefieldActType(pBattleData, pPlayer->GetRoleId());
// 	MAPLOG(ERROR_LEVEL, "[src_role_id:%d,act_type:%d,tgt_hero_id:%d,hero_act_type:%d,change_hp:%d]",
// 		pPlayer->GetRoleId(), bySrcActType, dwTgtHeroInfoId, byHeroActType, nChangeHP);
// 	PVPRoleBattleData *pTargetRoleData = &pBattleData->arRoleBattleDataList[byTargetActType];
// 	for (WORD i = 0; i < pTargetRoleData->wHeroNum; ++i)
// 	{
// 		PVPHeroData &tHeroData = pTargetRoleData->arHeroList[i];
// 		MAPLOG(ERROR_LEVEL, "Ӣ��[id:%d,hp:%d]", tHeroData.wHeroInfoId, tHeroData.nCurHP);
// 	}
}

// ����buff
void CPVPManager::OnNetCreateBuff(Player *pPlayer, DWORD dwBuffId, DWORD dwSrcObjId, DWORD dwTgtObjId, BYTE byIsFromSpriteBless)
{
	WORD wFromMsgId = C2S_PVP_CREATE_BUFF_REQ;

	// ����Ƿ���׼��״̬
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û����ս����[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��սĿ��û����[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	DWORD dwHeroInfoId = dwSrcObjId * 0.1;

	// ����˺�Դ�Ƿ񻹻���
	if (IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwHeroInfoId))
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����buff��Ӣ���ѹ�[role_id:%d, hero_info_id:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), dwHeroInfoId);
		return;
	}

	MSG_S2C_PVP_CREATE_BUFF_RESP resp;
	resp.m_dwBuffId = dwBuffId;
	resp.m_dwSrcObjId = dwSrcObjId;
	resp.m_dwTgtObjId = dwTgtObjId;
	resp.m_byIsSpriteBlessBuff = byIsFromSpriteBless;
	pPlayer->SendPacket(&resp);

	MSG_S2C_PVP_CREATE_BUFF_NOTIFY notify;
	notify.m_dwBuffId = dwBuffId;
	notify.m_dwSrcObjId = dwSrcObjId;
	notify.m_dwTgtObjId = dwTgtObjId;
	notify.m_byIsSpriteBlessBuff = byIsFromSpriteBless;
	pTarget->SendPacket(&notify);
}

// ս�����(ֻ�д�����˲Żᷢ��)
void CPVPManager::OnNetBattleResult(Player *pPlayer, BYTE byResult)
{
	WORD wFromMsgId = C2S_PVP_BATTLE_RESULT_REQ;

	// ����Ƿ���׼��״̬
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattlefieldData || !pBattlefieldData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û����ս����[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ���ս������
	if (!pBattlefieldData->bIsRobot)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����ս�Ĳ��ǻ�����,����ֱ�ӽ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ������������
	DWORD dwTargetId = GetTargetId(pBattlefieldData, pPlayer->GetRoleId());

	// ȡĿ���������
	PVPRoleBattleData *pTargetBattleData = GetRoleBattleData(pBattlefieldData, dwTargetId);
	if (!pTargetBattleData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ŀ��ս������[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	//BYTE byResult = byIsWin ? BATTLE_RESULT_WIN : BATTLE_RESULT_LOSE;

	// ս������
	DoBattleOverWithRobot(pPlayer, pTargetBattleData->dwFightingPower, byResult);
}

// ��ȡ��ս����
void CPVPManager::OnNetTakeBattleReward(Player *pPlayer, BYTE byType, BYTE *pTimesList, WORD wListNum)
{
	WORD wFromMsgId = C2S_PVP_TAKE_BATTLE_REWARD_REQ;

	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	WORD *pTakeRecordList = NULL;
	WORD *pTakeRecordNum = 0;

	// ÿ�յ�
	if (PVP_BATTLE_REWARD_DAILY == byType)
	{
		pTakeRecordList = tPVPData.arDailyRewardTakeRecord;
		pTakeRecordNum = &tPVPData.wDailyRewardTakeRecordNum;
	}
	// ÿ�ܵ�
	else
	{
		pTakeRecordList = tPVPData.arWeeklyRewardTakeRecord;
		pTakeRecordNum = &tPVPData.wWeeklyRewardTakeRecordNum;
	}

	// ��һ��ת��,Ϊ�˷������
	WORD &wTakeRecordNum = *pTakeRecordNum;

	// ����Ƿ�����ȡ��
	for (WORD i = 0; i < wListNum; ++i)
	{
		for (WORD k = 0; k < wTakeRecordNum; ++k)
		{
			if (pTakeRecordList[k] == pTimesList[i])
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�����Ѿ���ȡ��[times:%d][type:%d]", __FUNCTION__, pTimesList[i], byType);
				return;
			}
		}
	}

	// ������
	for (WORD i = 0; i < wListNum; ++i)
	{
		BYTE byBattleTimes = pTimesList[i];
		// ȡ����
		const PVPBattleReward_Config *pRewardCfg = g_pCfgMgr->GetPVPBattleReward(byType, byBattleTimes);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���PVPս������[times:%d][type:%d]", __FUNCTION__, byBattleTimes, byType);
			continue;
		}

		WORD wCurBattleTimes = PVP_BATTLE_REWARD_DAILY == byType ? tPVPData.wTodayBattleTimes : tPVPData.wWeekWinTimes;

		// �����ս�����Ƿ��㹻
		if (wCurBattleTimes < byBattleTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]��ս��������,������ȡ[times:%d][type:%d]", __FUNCTION__, byBattleTimes, byType);
			return;
		}

		pPlayer->AddObjectList(pRewardCfg->vItemList, TRUE, FROM_PVP_BATTLE_REWARD);
	}

	// ��¼
	for (WORD i = 0; i < wListNum; ++i)
	{
		if (wTakeRecordNum < MAX_PVP_BATTLE_REWARD_NUM)
			pTakeRecordList[wTakeRecordNum++] = pTimesList[i];
	}

	MSG_S2C_PVP_TAKE_BATTLE_REWARD_RESP msg;
	msg.m_byType = byType;
	msg.m_wTakeRewardNum = wListNum;
	memcpy_s(msg.m_arTakeRewardList, wListNum*sizeof(BYTE), pTimesList,wListNum*sizeof(BYTE));
	pPlayer->SendPacket(&msg);
}

bool CPVPManager::ReadyToBattleWithPlayer(DWORD dwRoleId, DWORD dwTargetId, DWORD dwSpiritSkillId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս��û����[role_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս��û����[role_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	PVPWaitBattleRoleData *pWaitBattleData = FindWaitBattleRoleData(dwTargetId);
	if (!pWaitBattleData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ȴ�ս������[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	CSpriteManager *pSrcSpiritMgr = pPlayer->GetSpiritManager();
	CSpriteManager *pTgtSpiritMgr = pTarget->GetSpiritManager();
	if (!pSrcSpiritMgr || !pTgtSpiritMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������Ϊ��", __FUNCTION__);
		return false;
	}

	// ��ʼ��ս������
	PVPBattlefieldData tBattleData;
	memset(&tBattleData, 0, sizeof(tBattleData));
	tBattleData.dwStartTime = time(NULL);
	tBattleData.bIsInBattle = false;
	tBattleData.dwBattleId = 0;
	tBattleData.bIsRobot = false;

	vector<ToBattleHeroData> vToBattleHeroList;
	int nBattleHeroDataSize = sizeof(ToBattleHeroData);

	// Ӣ��Ѫ����ӱ���
	float fHPIncreRate = g_GetConfigValue(207) * PROPERTY_DECREASE_MULTIPLE;

	// �Լ�����
	{
		pPlayer->FillToBattleHeroData(vToBattleHeroList);
		PVPRoleBattleData &tRedRoleData = tBattleData.arRoleBattleDataList[PVP_ACT_RED];
		tRedRoleData.byType = PVP_ACT_RED;
		tRedRoleData.dwRoleId = dwRoleId;
		tRedRoleData.dwFightingPower = pPlayer->GetFightingPower();
		tRedRoleData.dwToBattleSpiritId = pSrcSpiritMgr->GetToBattleSpiritId();
		tRedRoleData.dwSpiritSkillId = dwSpiritSkillId;
		const SpiritData *pSpriteData = pSrcSpiritMgr->GetSpiritEx(tRedRoleData.dwToBattleSpiritId);
		tRedRoleData.wSpriteEnhanceLevel = pSpriteData ? pSpriteData->wEnhanceLevel : 0;
		tRedRoleData.wSpriteTrainStage = pSpriteData ? pSpriteData->wTrainStage : 1;
		tRedRoleData.wSpriteTrainTimes = pSpriteData ? pSpriteData->wTrainTimes : 0;
		tRedRoleData.wSpriteStar = pSpriteData ? pSpriteData->wStarLevel : 0;
		tRedRoleData.dwServerId = pPlayer->GetServerId();
		tRedRoleData.wHeadImageId = pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE);
		strcpy_s(tRedRoleData.szRoleName, sizeof(tRedRoleData.szRoleName), pPlayer->GetRoleName());
		tRedRoleData.wHeroNum = 0;
		for (size_t i = 0; i < vToBattleHeroList.size() && i < MAX_BATTLE_HERO_NUM; ++i)
		{
			ToBattleHeroData &tFromData = vToBattleHeroList[tRedRoleData.wHeroNum++];
			PVPHeroData &tToData = tRedRoleData.arHeroList[i];
			memcpy_s(&tToData, nBattleHeroDataSize, &tFromData, nBattleHeroDataSize);
			// ������HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // ���Ӣ��Ѫ������
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}

		// ȡ�����б�
		tRedRoleData.wSpriteNum = 0;
		WORD wSpriteNum = pPlayer->GetSpiritManager()->GetSpriteListNum();
		const SpiritData *pSpriteList = pPlayer->GetSpiritManager()->GetSpriteList();
		for (WORD i = 0; i < wSpriteNum; ++i)
		{
			tRedRoleData.arSpriteList[tRedRoleData.wSpriteNum++] = pSpriteList[i];
		}
	}


	// Ŀ������
	{
		vToBattleHeroList.clear();
		pTarget->FillToBattleHeroData(vToBattleHeroList);
		PVPRoleBattleData &tBlueRoleData = tBattleData.arRoleBattleDataList[PVP_ACT_BLUE];
		tBlueRoleData.byType = PVP_ACT_BLUE;
		tBlueRoleData.dwRoleId = dwTargetId;
		tBlueRoleData.dwFightingPower = pTarget->GetFightingPower();
		tBlueRoleData.dwToBattleSpiritId = pTgtSpiritMgr->GetToBattleSpiritId();
		tBlueRoleData.dwSpiritSkillId = pWaitBattleData->dwSpiritSkillId;
		const SpiritData *pSpriteData = pTgtSpiritMgr->GetSpiritEx(tBlueRoleData.dwToBattleSpiritId);
		tBlueRoleData.wSpriteEnhanceLevel = pSpriteData ? pSpriteData->wEnhanceLevel : 0;
		tBlueRoleData.wSpriteTrainStage = pSpriteData ? pSpriteData->wTrainStage : 1;
		tBlueRoleData.wSpriteTrainTimes = pSpriteData ? pSpriteData->wTrainTimes : 0;
		tBlueRoleData.wSpriteStar = pSpriteData ? pSpriteData->wStarLevel : 0;
		tBlueRoleData.dwServerId = pTarget->GetServerId();
		tBlueRoleData.wHeadImageId = pTarget->GetProperty(ROLE_PRO_HEAD_IMAGE);
		strcpy_s(tBlueRoleData.szRoleName, sizeof(tBlueRoleData.szRoleName), pTarget->GetRoleName());
		tBlueRoleData.wHeroNum = 0;
		for (size_t i = 0; i < vToBattleHeroList.size() && i < MAX_BATTLE_HERO_NUM; ++i)
		{
			ToBattleHeroData &tFromData = vToBattleHeroList[tBlueRoleData.wHeroNum++];
			PVPHeroData &tToData = tBlueRoleData.arHeroList[i];
			memcpy_s(&tToData, nBattleHeroDataSize, &tFromData, nBattleHeroDataSize);
			// ������HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // ���Ӣ��Ѫ������
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}

		// ȡ�����б�
		tBlueRoleData.wSpriteNum = 0;
		WORD wSpriteNum = pTarget->GetSpiritManager()->GetSpriteListNum();
		const SpiritData *pSpriteList = pTarget->GetSpiritManager()->GetSpriteList();
		for (WORD i = 0; i < wSpriteNum; ++i)
		{
			tBlueRoleData.arSpriteList[tBlueRoleData.wSpriteNum++] = pSpriteList[i];
		}
	}

	AddBattlefieldData(tBattleData);

	// ֪ͨ����׼��ս��
	MSG_S2C_PVP_READY_NOTIFY msg;
	msg.m_byIsRobot = false;
	msg.m_pBattleData = &tBattleData;
	pPlayer->SendPacket(&msg);
	pTarget->SendPacket(&msg);

	return true;
}

// ׼������ս��(������)
bool CPVPManager::ReadyToBattleWithRobot(DWORD dwRoleId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս��û����[role_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	PVPWaitBattleRoleData *pWaitBattleData = FindWaitBattleRoleData(dwRoleId);
	if (!pWaitBattleData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ȴ�ս������[target_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	CSpriteManager *pSpiritMgr = pPlayer->GetSpiritManager();
	if (!pSpiritMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������Ϊ��", __FUNCTION__);
		return false;
	}


	// ��ʼ��ս������
	PVPBattlefieldData tBattleData;
	memset(&tBattleData, 0, sizeof(tBattleData));
	tBattleData.dwStartTime = time(NULL);
	tBattleData.bIsInBattle = false;
	tBattleData.dwBattleId = 0;
	tBattleData.bIsRobot = true;

	vector<ToBattleHeroData> vToBattleHeroList;
	int nBattleHeroDataSize = sizeof(ToBattleHeroData);

	DWORD dwFightingPower = pPlayer->GetFightingPower();

	// Ӣ��Ѫ����ӱ���
	float fHPIncreRate = g_GetConfigValue(207) * PROPERTY_DECREASE_MULTIPLE;

	// �Լ�����
	{
		pPlayer->FillToBattleHeroData(vToBattleHeroList);
		PVPRoleBattleData &tRedRoleData = tBattleData.arRoleBattleDataList[PVP_ACT_RED];
		tRedRoleData.byType = PVP_ACT_RED;
		tRedRoleData.dwRoleId = dwRoleId;
		tRedRoleData.dwFightingPower = dwFightingPower;
		tRedRoleData.dwToBattleSpiritId = pSpiritMgr->GetToBattleSpiritId();
		tRedRoleData.dwSpiritSkillId = pWaitBattleData->dwSpiritSkillId;
		const SpiritData *pSpriteData = pSpiritMgr->GetSpiritEx(tRedRoleData.dwToBattleSpiritId);
		tRedRoleData.wSpriteEnhanceLevel = pSpriteData ? pSpriteData->wEnhanceLevel : 0;
		tRedRoleData.wSpriteTrainStage = pSpriteData ? pSpriteData->wTrainStage : 1;
		tRedRoleData.wSpriteTrainTimes = pSpriteData ? pSpriteData->wTrainTimes : 0;
		tRedRoleData.dwServerId = pPlayer->GetServerId();
		tRedRoleData.wHeadImageId = pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE);
		strcpy_s(tRedRoleData.szRoleName, sizeof(tRedRoleData.szRoleName), pPlayer->GetRoleName());
		tRedRoleData.wHeroNum = 0;
		for (size_t i = 0; i < vToBattleHeroList.size() && i < MAX_BATTLE_HERO_NUM; ++i)
		{
			ToBattleHeroData &tFromData = vToBattleHeroList[tRedRoleData.wHeroNum++];
			PVPHeroData &tToData = tRedRoleData.arHeroList[i];
			memcpy_s(&tToData, nBattleHeroDataSize, &tFromData, nBattleHeroDataSize);
			// ������HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // ���Ӣ��Ѫ������
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}
	}

	// Ŀ������(ʹ���Լ�������)
	{
		// ���Ը���(HP, attck, ����5%)
		float fFloatRange = PROPERTY_DECREASE_MULTIPLE * g_GetConfigValue(172);

		// �Ҳ�����ʹ���Լ�
		vToBattleHeroList.clear();
		pPlayer->FillToBattleHeroData(vToBattleHeroList);
		PVPRoleBattleData &tBlueRoleData = tBattleData.arRoleBattleDataList[PVP_ACT_BLUE];
		tBlueRoleData.byType = PVP_ACT_BLUE;
		tBlueRoleData.dwRoleId = PVP_ROBOT_ID;
		tBlueRoleData.dwFightingPower = dwFightingPower;
		// ���������Χ
		if (g_Rand(0, 1))
			tBlueRoleData.dwFightingPower -= dwFightingPower * fFloatRange;
		else
			tBlueRoleData.dwFightingPower += dwFightingPower * fFloatRange;

		tBlueRoleData.dwToBattleSpiritId = pSpiritMgr->GetToBattleSpiritId();
		tBlueRoleData.dwSpiritSkillId = pWaitBattleData->dwSpiritSkillId;
		const SpiritData *pSpriteData = pSpiritMgr->GetSpiritEx(tBlueRoleData.dwToBattleSpiritId);
		tBlueRoleData.wSpriteEnhanceLevel = pSpriteData ? pSpriteData->wEnhanceLevel : 0;
		tBlueRoleData.wSpriteTrainStage = pSpriteData ? pSpriteData->wTrainStage : 1;
		tBlueRoleData.wSpriteTrainTimes = pSpriteData ? pSpriteData->wTrainTimes : 0;
		tBlueRoleData.dwServerId = pPlayer->GetServerId();
		tBlueRoleData.wHeadImageId = pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE);
		strcpy_s(tBlueRoleData.szRoleName, sizeof(tBlueRoleData.szRoleName), pPlayer->GetRoleName());
		tBlueRoleData.wHeroNum = 0;
		vector<DWORD> vNewHeroList;
		for (size_t i = 0; i < vToBattleHeroList.size() && i < MAX_BATTLE_HERO_NUM; ++i)
		{
			ToBattleHeroData &tFromData = vToBattleHeroList[tBlueRoleData.wHeroNum++];
			PVPHeroData &tToData = tBlueRoleData.arHeroList[i];
			memcpy_s(&tToData, nBattleHeroDataSize, &tFromData, nBattleHeroDataSize);	
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				Property &tPro = tToData.arPorList[k];
				// ���Ե���(hp, attck, ����5%)
				if (tPro.wType == HERO_PRO_BLOOD || tPro.wType == HERO_PRO_ATTACK)
				{
					// ���������Χ
					if (g_Rand(0, 1))
						tPro.nValue -= tPro.nValue * fFloatRange;
					else
						tPro.nValue += tPro.nValue * fFloatRange;
				}

				// ��¼��ǰ����HP
				if (tPro.wType == HERO_PRO_BLOOD)
				{
					tPro.nValue *= fHPIncreRate; // ���Ӣ��Ѫ������
					tToData.dwTotalHP = tToData.nCurHP = tPro.nValue;
					break;
				}
			}

			// �滻Ӣ��(ͬƷ��)
			const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(tToData.wHeroInfoId);
			if (pHeroCfg)
			{
				vector<DWORD> *pPokedexList = g_pCfgMgr->GetHeroPokedexList(pHeroCfg->wInitQuality);
				if (!pPokedexList || !pPokedexList->size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]�ҵ�Ӣ��Ʒ�ʶ�Ӧ��ͼ��[quality:%d]", __FUNCTION__, pHeroCfg->wInitQuality);
					continue;
				}
				
				// todo::�˴����߼���̫����,�պ����
				int nDoTimes = 0;
				while (true)
				{
					// ��ֹ�������������ѭ��
					if (nDoTimes++ >= 1000)
					{
						MAPLOG(ERROR_LEVEL, "[%s]ִ��ѭ������1000�α�ǿ���˳�", __FUNCTION__);
						break;
					}

					// ���һ��ͬƷ�ʵ�Ӣ��
					int nIndex = g_Rand(0, pPokedexList->size() - 1);
					DWORD dwHeroInfoId = pPokedexList->at(nIndex);

					// ����Ƿ�������ͬӢ������
					bool bExist = false;
					for (size_t k = 0; k < vNewHeroList.size(); ++k)
					{
						if (dwHeroInfoId == vNewHeroList[k])
						{
							bExist = true;
							break;
						}
					}

					// �滻
					if (!bExist)
					{
						tToData.wHeroInfoId = dwHeroInfoId;
						vNewHeroList.push_back(dwHeroInfoId);
						break;
					}
				}
			}
		}		
	}

	AddBattlefieldData(tBattleData);

	// ֪ͨ����׼��ս��
	MSG_S2C_PVP_READY_NOTIFY msg;
	msg.m_byIsRobot = true;
	msg.m_pBattleData = &tBattleData;
	pPlayer->SendPacket(&msg);

	return true;
}

// ȡ��������
BYTE CPVPManager::GetBattlefieldActType(DWORD dwRoleId)
{
	PVPBattlefieldData *pBattleData = FindBattlefieldData(dwRoleId);
	if (pBattleData)
	{
		for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
		{
			if (pBattleData->arRoleBattleDataList[i].dwRoleId == dwRoleId)
				return pBattleData->arRoleBattleDataList[i].byType;
		}
	}
	return PVP_ACT_INVALID;
}

// ȡ��������
BYTE CPVPManager::GetBattlefieldActType(const PVPBattlefieldData *pBattleData, DWORD dwRoleId)
{
	for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		if (pBattleData->arRoleBattleDataList[i].dwRoleId == dwRoleId)
			return pBattleData->arRoleBattleDataList[i].byType;
	}
	return PVP_ACT_INVALID;
}

// Ӣ���Ƿ�������
bool CPVPManager::IsHeroDead(const PVPBattlefieldData *pBattleData, DWORD dwRoleId, DWORD dwHeroInfoId)
{
	for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		const PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[i];
		if (tRoleData.dwRoleId != dwRoleId)
			continue;

		for (WORD k = 0; k < tRoleData.wHeroNum; ++k)
		{
			if (tRoleData.arHeroList[k].wHeroInfoId == dwHeroInfoId)
			{
				if (tRoleData.arHeroList[k].nCurHP <= 0)
					return true;
			}
		}
		break;
	}

	return false;
}

// �Ƿ����е�Ӣ�۶��ҵ�
bool CPVPManager::IsAllHeroDead(const PVPBattlefieldData *pBattleData, BYTE byActType)
{
	const PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[byActType];
	for (WORD i = 0; i < tRoleData.wHeroNum; ++i)
	{
		if (tRoleData.arHeroList[i].nCurHP > 0)
			return false;
	}

	return true;
}

// ͨ��ս��ɸѡĿ��
DWORD CPVPManager::SelectTargetByFightingPower(DWORD dwFightingPower)
{
	DWORD dwMinFightingPower = dwFightingPower > 1000000 ? dwFightingPower - 1000000 : 0;
	DWORD dwMaxFightingPower = dwFightingPower + 2000000;

	deque<PVPWaitBattleRoleData>::iterator iter = m_WaitBattleRoleDataList.begin();
	for (; iter != m_WaitBattleRoleDataList.end(); ++iter)
	{
		if (iter->dwFightingPower >= dwMinFightingPower && iter->dwFightingPower <= dwMaxFightingPower)
			return iter->dwRoleId;
	}

	return 0;
}

// ɾ��ս��Ŀ������
void CPVPManager::RemoveWaitBattleRoleData(DWORD dwRoleId)
{
	deque<PVPWaitBattleRoleData>::iterator iter = m_WaitBattleRoleDataList.begin();
	for (; iter != m_WaitBattleRoleDataList.end(); ++iter)
	{
		if (iter->dwRoleId == dwRoleId)
		{
			m_WaitBattleRoleDataList.erase(iter);
			break;
		}
	}

}

// ����Ӣ��HP
int CPVPManager::UpateHeroHP(PVPBattlefieldData *pBattleData, BYTE byActType, DWORD dwHeroInfoId, int nChangeHP)
{
	int nResult = 0;
	PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[byActType];
	for (WORD i = 0; i < tRoleData.wHeroNum; ++i)
	{
		PVPHeroData &tHeroData = tRoleData.arHeroList[i];
		if (tHeroData.nCurHP > 0 && tHeroData.wHeroInfoId == dwHeroInfoId)
		{
			tHeroData.nCurHP += nChangeHP;
			// ���޾���
			tHeroData.nCurHP = min(tHeroData.nCurHP, int(tHeroData.dwTotalHP));
			// ���޾���
			tHeroData.nCurHP = max(tHeroData.nCurHP, 0);
			break;
		}
	}
	return nResult;
}

// ȡս��Ŀ��ID
DWORD CPVPManager::GetTargetId(const PVPBattlefieldData *pBattleData, DWORD dwRoleId)
{
	for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		const PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[i];
		if (tRoleData.dwRoleId != dwRoleId)
			return tRoleData.dwRoleId;
	}
	return 0;
}

// ɾ��ս������
void CPVPManager::RemoveBattlefieldData(DWORD dwRoleId)
{
	vector<PVPBattlefieldData>::iterator iter = m_BattlefieldList.begin();
	for (; iter != m_BattlefieldList.end(); ++iter)
	{
		for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
		{
			if (iter->arRoleBattleDataList[i].dwRoleId == dwRoleId)
			{
				m_BattlefieldList.erase(iter);
				return;
			}
		}
	}
}

// �Ƿ�����
bool CPVPManager::IsActivityOpen()
{
	tm tNow;
	GetTMTime(&tNow,time(NULL));

	const PVPActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetPVPActivityTime(tNow.tm_wday);
	if (!pActivityTimeCfg)
		return false;

	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;

	for (size_t i = 0; i < pActivityTimeCfg->vTimeList.size(); ++i)
	{
		const TimeData &tTimeData = pActivityTimeCfg->vTimeList[i];
		if (nCurTime >= tTimeData.nStart && nCurTime <= tTimeData.nOver)
			return true;
	}

	// ����Ƿ���ָ����Χ�ڵ�ʱ��
// 	if (nCurTime >= pActivityTimeCfg->nStartTime && nCurTime <= pActivityTimeCfg->nOverTime)
// 		return true;

	return false;
}

// �Ƿ��к��
bool CPVPManager::IsHaveRedPoint(Player *pPlayer)
{
	// todo::�Ƿ�ʱ��
// 	if (!pPlayer->GetActivityManager()->IsActivityOpen(SYSTEM_PVP))
// 		return false;
	
	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	map<DWORD, PVPBattleReward_Config>::iterator iter = g_pCfgMgr->m_PVPBattleRewardList.begin();
	for (; iter != g_pCfgMgr->m_PVPBattleRewardList.end(); ++iter)
	{
		// ÿ�ս���
		if (PVP_BATTLE_REWARD_DAILY == iter->second.wType &&
			tPVPData.wTodayBattleTimes >= iter->second.wBattleTimes)
		{
			bool bExist = false;
			for (WORD i = 0; i < tPVPData.wDailyRewardTakeRecordNum; ++i)
			{
				if (tPVPData.arDailyRewardTakeRecord[i] == iter->second.wBattleTimes)
				{
					bExist = true;
					break;
				}
			}
			if (!bExist)
				return true;
		}
		// ÿ�ܽ���
		else if (PVP_BATTLE_REWARD_WEEKLY == iter->second.wType &&
			tPVPData.wWeekWinTimes >= iter->second.wBattleTimes)
		{
			bool bExist = false;
			for (WORD i = 0; i < tPVPData.wWeeklyRewardTakeRecordNum; ++i)
			{
				if (tPVPData.arWeeklyRewardTakeRecord[i] == iter->second.wBattleTimes)
				{
					bExist = true;
					break;
				}
			}
			if (!bExist)
				return true;
		}
	}

	return false;
}

// �������н���
void CPVPManager::DoRankAward()
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_PVP_ALL);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			const PVPRankReward_Config *pRewardCfg = g_pCfgMgr->GetPVPRankReward(iter->wRank);
			if (pRewardCfg && pRewardCfg->vItemList.size())
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 12;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = iter->wRank;
				CRewardCenterManager::AddReward(iter->dwServerId, iter->dwRoleId, tRewardData);

				MAPLOG(ERROR_LEVEL, "[%s]PVP���з��Ž���[role_id:%d,rank:%d,rank_value:%d]",
					__FUNCTION__, iter->dwRoleId, iter->wRank, iter->dwValue);
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]PVP���а��ѷ��Ž���", __FUNCTION__);
}


// �������гƺ�
void CPVPManager::DoRankTitle()
{
	// ȫ�����гƺŽ���
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_PVP_ALL);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			const PVPRankReward_Config *pRewardCfg = g_pCfgMgr->GetPVPRankReward(iter->wRank);
			if (pRewardCfg && pRewardCfg->dwTitleId)
			{
				CTitleManager::AddTitleEx(iter->dwServerId, iter->dwRoleId, pRewardCfg->dwTitleId);

				MAPLOG(ERROR_LEVEL, "[%s]PVP���з��ųƺŽ���[role_id:%d,rank:%d,title_id:%d]",
					__FUNCTION__, iter->dwRoleId, iter->wRank, pRewardCfg->dwTitleId);
			}
				
		}
		CRankManager::Instance()->ResetRoleRank(ALL_SERVER_ID, RANK_PVP_ALL);
	}

	// ������������
	CRankManager::Instance()->ResetRoleRank(RANK_PVP_LOCAL);

	MAPLOG(ERROR_LEVEL, "[%s]PVP���а��ѷ��ųƺŽ���", __FUNCTION__);
}

// �޸Ļ���(GMʹ��)
void CPVPManager::ModifyScoreForGM(Player *pPlayer, DWORD dwScore, WORD wWeekWinTimes)
{
	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;
	tPVPData.dwScore = dwScore;
	tPVPData.wWeekWinTimes = wWeekWinTimes;
	CRankManager::Instance()->AddRoleRankData(RANK_PVP_LOCAL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, MAX_DEFAULT_RANK_MUM);
	CRankManager::Instance()->AddRoleRankData(RANK_PVP_ALL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, 100);
}

PVPWaitBattleRoleData* CPVPManager::FindWaitBattleRoleData(DWORD dwRoleId)
{
	deque<PVPWaitBattleRoleData>::iterator iter = m_WaitBattleRoleDataList.begin();
	for (; iter != m_WaitBattleRoleDataList.end(); ++iter)
	{
		if (iter->dwRoleId == dwRoleId)
			return &*iter;
	}

	return NULL;
}

PVPBattlefieldData* CPVPManager::FindBattlefieldData(DWORD dwRoleId)
{
	vector<PVPBattlefieldData>::iterator iter = m_BattlefieldList.begin();
	for (; iter != m_BattlefieldList.end(); ++iter)
	{
		for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
		{
			if (iter->arRoleBattleDataList[i].dwRoleId == dwRoleId)
				return &*iter;
		}
	}

	return NULL;
}

// ȡ��ɫս������
PVPRoleBattleData* CPVPManager::GetRoleBattleData(PVPBattlefieldData *pBattleData, DWORD dwRoleId)
{
	for (WORD i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		if (pBattleData->arRoleBattleDataList[i].dwRoleId == dwRoleId)
			return &pBattleData->arRoleBattleDataList[i];
	}
	return NULL;
}

// ȡ��ɫս������
PVPRoleBattleData* CPVPManager::GetRoleBattleData(DWORD dwRoleId)
{
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(dwRoleId);
	return pBattlefieldData ? GetRoleBattleData(pBattlefieldData, dwRoleId) : NULL;
}

// ȡ��ɫս������
PVPRoleBattleData* CPVPManager::GetRoleBattleData(PVPBattlefieldData *pBattleData, BYTE byActType)
{
	if (byActType >= PVP_BATTLEFIELD_ROLE_NUM)
		return NULL;

	return &pBattleData->arRoleBattleDataList[byActType];
}

void CPVPManager::AddBattlefieldData(const PVPBattlefieldData &tBattleData)
{
	m_BattlefieldList.push_back(tBattleData);
}

// �����ʤ����
DWORD CPVPManager::CalculateWinScore(DWORD dwTargetFightingPower)
{
	// ���� = 50+����ս��/5��
	return  50 + ceil(dwTargetFightingPower * 0.00002);
}

// ����ʧ�ܻ���
DWORD CPVPManager::CalculateLoseScore(DWORD dwWinScore)
{
	//  ʧ�ܻ��� = ʤ������ / 2  ����ȡ��
	return ceil(dwWinScore * 0.5);
}

// ȡƽ�ֻ���
DWORD CPVPManager::CalculateDrawScore(DWORD dwWinScore)
{
	//  ƽ�ֻ��� = ʤ������*0.75 �� ʧ�ܻ���*1.5
	return ceil(dwWinScore * 0.75);
}

// �������
DWORD CPVPManager::CalculateScore(BYTE byResult, DWORD dwTargetFightingPower)
{
	DWORD dwScore = 0;

	DWORD dwWinScore = CalculateWinScore(dwTargetFightingPower);

	// ʤ��
	if (BATTLE_RESULT_WIN == byResult)
	{
		dwScore = dwWinScore;
	}
	// ƽ��
	else if (BATTLE_RESULT_DRAW == byResult)
	{
		dwScore = CalculateDrawScore(dwWinScore);
	}
	// ʧ��
	else
	{
		dwScore = CalculateLoseScore(dwWinScore);
	}

	return dwScore;
}

// �������PVP����
void CPVPManager::SendPlayerPVPData(Player *pPlayer)
{
	MSG_S2C_PLAYER_PVP_DATA_RESP msg;
	msg.m_PVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;
	msg.m_wMyRank = 0;
	msg.m_dwMyRankValue = 0;
	msg.m_wRankNum = 0;
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_PVP_ALL);
	if (pRankList)
	{
		for (size_t i = 0; i < pRankList->size() && i < 3/*todo::�ݶ���������*/; ++i)
		{	
			const RoleRankData &tRankData = pRankList->at(i);
			DWORD dwCurServerId = g_MapServer.GetCurServerId(tRankData.dwServerId);
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwCurServerId, tRankData.dwRoleId, false);
			if (pBaseData)
			{
				ClientRoleRankData &tClientRankData = msg.m_RankList[msg.m_wRankNum++];
				tClientRankData = *pBaseData;
				tClientRankData.dwValue = tRankData.dwValue;
			}
		}
	}
	const RoleRankData *pMyRankData = CRankManager::Instance()->GetRoleRankDataEx(ALL_SERVER_ID, RANK_PVP_ALL, pPlayer->GetRoleId());
	if (pMyRankData)
	{
		msg.m_wMyRank = pMyRankData->wRank;
		msg.m_dwMyRankValue = pMyRankData->dwValue;
	}
	else
	{
		PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;
		msg.m_dwMyRankValue = tPVPData.dwScore;
	}
	pPlayer->SendPacket(&msg);
}

// ��ʼս��֪ͨ
void CPVPManager::DoBattleStart(Player *pPlayer, Player *pTarget)
{
	// ��ʼս��
	MSG_S2C_PVP_BATTLE_START_NOTIFY notify;

	if (pPlayer)
	{
		pPlayer->SendPacket(&notify);
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wTodayBattleTimes++;
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wWeekBattleTimes++;
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.dwTotalBattleTimes++;

		// �ճ�����(����Ӣ�۶Ծ�X��)
		pPlayer->OnUpdateDailyMission(24);

		// �ɾ��¼�(Ӣ�۶Ծ��ۼ�ս���س�)
		pPlayer->OnUpdateAchievement(24);
	}

	if (pTarget)
	{
		pTarget->SendPacket(&notify);
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.wTodayBattleTimes++;
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.wWeekBattleTimes++;
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.dwTotalBattleTimes++;

		// �ճ�����(����Ӣ�۶Ծ�X��)
		pTarget->OnUpdateDailyMission(24);

		// �ɾ��¼�(Ӣ�۶Ծ��ۼ�ս���س�)
		pTarget->OnUpdateAchievement(24);
	}
}

// ����ս������
void CPVPManager::DoBattleOverWithPlayer(Player *pRedPlayer, Player *pBluePlayer, DWORD dwWinnerId)
{
  	MSG_S2C_PVP_BATTLE_OVER_NOTIFY msg;
  	msg.m_dwWinnerId = dwWinnerId;
	msg.m_wRedLastWeekRank = pRedPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wLastWeekRank;
	msg.m_wBlueLastWeekRank = pBluePlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wLastWeekRank;

	WORD wOldRank = 0;
	WORD wNewRank = 0;
	DWORD dwScore = 0;

	DWORD dwRedFightingPower = pRedPlayer->GetFightingPower();
	DWORD dwBlueFightingPower = pBluePlayer->GetFightingPower();
  
  	// ��ʤ��
  	if (dwWinnerId)
  	{
		if (pRedPlayer->GetRoleId() == dwWinnerId)
  		{
			// �췽ʤ��
			msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_WIN, dwBlueFightingPower);
			AddScore(pRedPlayer, dwScore, true);
			msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
			msg.m_wRedScore = dwScore;

			// ����ʧ��
			msg.m_wBlueOldRank = GetRank(pBluePlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_LOSE, dwRedFightingPower);
			AddScore(pBluePlayer, dwScore);
			msg.m_wBlueNewRank = GetRank(pBluePlayer->GetRoleId());
			msg.m_wBlueScore = dwScore;
  		}
  		else
  		{
			// �췽ʧ��
			msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_LOSE, dwBlueFightingPower);
			AddScore(pRedPlayer, dwScore);
			msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
			msg.m_wRedScore = dwScore;

			// ����ʤ��
			msg.m_wBlueOldRank = GetRank(pBluePlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_WIN, dwRedFightingPower);
			AddScore(pBluePlayer, dwScore, true);
			msg.m_wBlueNewRank = GetRank(pBluePlayer->GetRoleId());
			msg.m_wBlueScore = dwScore;
  		}
  	}
  	// ƽ��
  	else
  	{  
		// �췽
		msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
		dwScore = CalculateScore(BATTLE_RESULT_DRAW, dwBlueFightingPower);
		AddScore(pRedPlayer, dwScore);
		msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
		msg.m_wRedScore = dwScore;

		// ����
		msg.m_wBlueOldRank = GetRank(pBluePlayer->GetRoleId());
		dwScore = CalculateScore(BATTLE_RESULT_DRAW, dwRedFightingPower);
		AddScore(pBluePlayer, dwScore);
		msg.m_wBlueNewRank = GetRank(pBluePlayer->GetRoleId());
		msg.m_wBlueScore = dwScore;
  	}

	if (pRedPlayer)
		pRedPlayer->SendPacket(&msg);

	if (pBluePlayer)
		pBluePlayer->SendPacket(&msg);
  
  	// ɾ��ս��
	RemoveBattlefieldData(pRedPlayer->GetRoleId());
}

// ����ս������(������)
void CPVPManager::DoBattleOverWithRobot(Player *pPlayer, DWORD dwTargetFightinPower, BYTE byResult)
{
 	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;
 	DWORD dwSrcScore = 0;
	DWORD dwTgtScore = 0;
	DWORD dwSrcFightingPower = pPlayer->GetFightingPower();

	// ʤ��
	if (BATTLE_RESULT_WIN == byResult)
	{
		// ��û���
		dwSrcScore = CalculateScore(BATTLE_RESULT_WIN,dwTargetFightinPower);
		dwTgtScore = CalculateScore(BATTLE_RESULT_LOSE, dwSrcFightingPower);
	}
	// ƽ��
	else if (BATTLE_RESULT_DRAW == byResult)
	{
		// ��û���
		dwSrcScore = CalculateScore(BATTLE_RESULT_DRAW, dwTargetFightinPower);
		dwTgtScore = CalculateScore(BATTLE_RESULT_DRAW, dwSrcFightingPower);
	}
	// ʧ��
	else
	{
		// ��û���
		dwSrcScore = CalculateScore(BATTLE_RESULT_LOSE, dwTargetFightinPower);
		dwTgtScore = CalculateScore(BATTLE_RESULT_WIN, dwSrcFightingPower);
	}

	WORD wOldRank = GetRank(pPlayer->GetRoleId());
	AddScore(pPlayer, dwSrcScore, BATTLE_RESULT_WIN == byResult ? true : false);
	WORD wNewRank = GetRank(pPlayer->GetRoleId());

	MSG_S2C_PVP_BATTLE_RESULT_RESP msg;
	msg.m_byResult = byResult;
	msg.m_wRedOldRank = wOldRank;
	msg.m_wRedNewRank = wNewRank;
	msg.m_wRedLastWeekRank = tPVPData.wLastWeekRank;
	msg.m_wRedScore = dwSrcScore;
	msg.m_wBlueOldRank = 0;
	msg.m_wBlueNewRank = 0;
	msg.m_wBlueLastWeekRank = 0;
	msg.m_wBlueScore = dwTgtScore;
	pPlayer->SendPacket(&msg);

	// ɾ��ս��
	RemoveBattlefieldData(pPlayer->GetRoleId());
}

// ��ӻ���
void CPVPManager::AddScore(Player *pPlayer, DWORD dwScore, BYTE byWinner)
{
	bool bNeedUpdateRank = false;

	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	// ��ս������������(�����˴������޽���)
	if (tPVPData.wTodayBattleTimes <= g_GetConfigValue(153))
	{
		tPVPData.dwScore += dwScore;
		bNeedUpdateRank = true;
	}
		
	if (byWinner)
	{
		tPVPData.wWeekWinTimes++;
		bNeedUpdateRank = true;
	}
		
	// ����������
	if (bNeedUpdateRank)
	{
		CRankManager::Instance()->AddRoleRankData(RANK_PVP_LOCAL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, MAX_DEFAULT_RANK_MUM);
		CRankManager::Instance()->AddRoleRankData(RANK_PVP_ALL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, 100);
	}
}

// ȡ����
WORD CPVPManager::GetRank(DWORD dwRoleId)
{
	return CRankManager::Instance()->GetRoleRank(dwRoleId, ALL_SERVER_ID, RANK_PVP_ALL);
}
