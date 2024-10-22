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
	// 等待挑战列表
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

	// 战役列表(一次只处理一个)
	vector<PVPBattlefieldData>::iterator battlefield_iter = m_BattlefieldList.begin();
	for (; battlefield_iter != m_BattlefieldList.end(); ++battlefield_iter)
	{

		// 时间已到,判平局
		if (dwCurTime >= battlefield_iter->dwStartTime + m_dwBattleLimitTime)
		{
			PVPRoleBattleData *pRedBattleData = GetRoleBattleData(&*battlefield_iter, (BYTE)PVP_ACT_RED);
			PVPRoleBattleData *pBlueBattleData = GetRoleBattleData(&*battlefield_iter, (BYTE)PVP_ACT_BLUE);
			if (!pRedBattleData || !pBlueBattleData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到战场的角色战斗数据!!!!!!", __FUNCTION__);
				continue;
			}
			// 红方玩家一定是非机器人
			Player *pRedPlayer = PlayerManager::Instance()->FindPlayerByCharID(pRedBattleData->dwRoleId);
			if (!pRedPlayer)
			{
				MAPLOG(ERROR_LEVEL, "[%s]战场里的红方玩家没在线[roleid:%d]", __FUNCTION__, pRedBattleData->dwRoleId);
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
					MAPLOG(ERROR_LEVEL, "[%s]战场里的蓝方玩家没在线[roleid:%d]", __FUNCTION__, pBlueBattleData->dwRoleId);
					continue;
				}
				DoBattleOverWithPlayer(pRedPlayer, pBluePlayer, 0);
			}
			// 一次只处理一个,所以退出
			break;
		}
	}

	// 处理排行奖励
	if (m_RankRewardTimer.IsTrigger())
	{
		DoRankAward();
		m_RankRewardTimer.SetCDOverTime();
	}

	// 称号奖励
	if (m_RankTitleTimer.IsTrigger())
	{
		DoRankTitle();
		m_RankTitleTimer.SetCDOverTime();
	}
}

// 登出
void CPVPManager::OnLogout(Player *pPlayer)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 如果在等待战斗的列表中
	if (FindWaitBattleRoleData(dwRoleId))
	{
		RemoveWaitBattleRoleData(dwRoleId);
		return;
	}

	// 如果在战斗中
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(dwRoleId);
	if (pBattlefieldData)
	{
		DWORD dwTargetId = GetTargetId(pBattlefieldData, dwRoleId);
		PVPRoleBattleData *pRoleBattleData = GetRoleBattleData(pBattlefieldData, dwTargetId);
		if (!pRoleBattleData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到角色战斗数据[role_id:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// 挑战的是机器人
		if (pBattlefieldData->bIsRobot)
		{
			DoBattleOverWithRobot(pPlayer, pRoleBattleData->dwFightingPower, BATTLE_RESULT_LOSE);
		}
		// 挑战的是玩家
		else
		{
			Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
			if (!pTarget)
			{
				// 删除战场
				//RemoveBattlefieldData(dwRoleId);
				MAPLOG(ERROR_LEVEL, "[%s]PVP挑战目标没在线!!![target_id:%d]", __FUNCTION__, dwTargetId);
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

// 请求玩家PVP数据
void CPVPManager::OnNetPlayerPVPData(Player *pPlayer)
{
	SendPlayerPVPData(pPlayer);
}

// 查找目标
void CPVPManager::OnNetSearchTarget(Player *pPlayer, DWORD dwSpiritSkillId)
{
	WORD wFromMsgId = C2S_PVP_SEARCH_TARGET_REQ;

	// 检测是否在活动期间
	if (!IsActivityOpen())
	{
		pPlayer->SendErrorMsg(ERROR_PVP_ACTIVITY_CLOSE);
		MAPLOG(GUI_LEVEL, "[%s]PVP活动还木有开放[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已经等待战斗列表
	if (FindWaitBattleRoleData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在等待挑战列表[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已经在战场
	if (FindBattlefieldData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经已在战场中...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 响应
	MSG_S2C_PVP_SEARCH_TARGET_RESP msg;
	msg.m_dwSpiritSkillId = dwSpiritSkillId;
	pPlayer->SendPacket(&msg);

	DWORD dwFightingPower = pPlayer->GetFightingPower();
	DWORD dwTargetId = SelectTargetByFightingPower(dwFightingPower);

	// 找到目标,则马上进入战斗
	if (dwTargetId)
	{
		if (!ReadyToBattleWithPlayer(pPlayer->GetRoleId(), dwTargetId, dwSpiritSkillId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]准备进入战斗失败[role_id:%d,target_id:%d]",
				__FUNCTION__, pPlayer->GetRoleId(), dwTargetId);
		}

		RemoveWaitBattleRoleData(dwTargetId);
		RemoveWaitBattleRoleData(pPlayer->GetRoleId());
	}
	// 没有找到目标,则进入等待列表
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

// 取消战斗
void CPVPManager::OnNetCancelSearchTarget(Player *pPlayer)
{
	WORD wFromMsgId = C2S_PVP_CANCEL_SEARCH_TARGET_REQ;

	MSG_S2C_PVP_CANCEL_SEARCH_TARGET_RESP msg;

	// 检测是否在战斗中
	if (FindBattlefieldData(pPlayer->GetRoleId()))
	{
		msg.m_byIsInBattle = true;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]玩家已经已在战场中...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否在匹配战斗	
	if (!FindWaitBattleRoleData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]玩家没有在匹配战斗中...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	RemoveWaitBattleRoleData(pPlayer->GetRoleId());

	// 响应
	msg.m_byIsInBattle = false;
	pPlayer->SendPacket(&msg);
}

// 准备完成
void CPVPManager::OnNetReadyFinish(Player *pPlayer)
{
	WORD wFromMsgId = C2S_PVP_READY_FINISH_REQ;

	// 检测是否在准备状态
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在准备状态[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否在战斗中
	if (pBattleData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在战斗中[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 挑战机器人
	if (pBattleData->bIsRobot)
	{
		MSG_S2C_PVP_READY_FINISH_RESP resp;
		pPlayer->SendPacket(&resp);

		pBattleData->bIsInBattle = true;

		// 开始战斗
		DoBattleStart(pPlayer, NULL);
	}
	// 挑战在线玩家
	else
	{
		// 取挑战目标的ID
		DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (!pTarget)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]挑战目标没在线[target_id:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		MSG_S2C_PVP_READY_FINISH_RESP resp;
		pPlayer->SendPacket(&resp);

		bool bStartBattle = true;
		for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
		{
			PVPRoleBattleData &tRoleData = pBattleData->arRoleBattleDataList[i];

			// 更新准备状态
			if (tRoleData.dwRoleId == pPlayer->GetRoleId())
				tRoleData.bIsReady = true;

			// 计算是否可以开始战斗
			if (!tRoleData.bIsReady)
				bStartBattle = false;
		}

		// 是否开始战斗
		if (bStartBattle)
		{
			pBattleData->bIsInBattle = true;

			// 开始战斗
			DoBattleStart(pPlayer, pTarget);
		}
	}
}

// 使用技能
void CPVPManager::OnNetUseSkill(Player *pPlayer, DWORD dwObjId, DWORD dwSkillId, WORD wX, WORD wY)
{
	WORD wFromMsgId = C2S_PVP_USE_SKILL_REQ;

	// 检测是否在准备状态
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有在战斗中[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]挑战目标没在线[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	BYTE byActType = GetBattlefieldActType(pBattleData, pPlayer->GetRoleId());
	if (PVP_ACT_INVALID == byActType)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]扮演者类型出错[target_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwRealOjbId = dwObjId * 0.1;

	BYTE byOjbType = GetObejctTypeById(dwRealOjbId);

	// 检测使用源是否还活着(只有英雄才需要检测)
	if (OBJECT_HERO == byOjbType && IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwRealOjbId))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]释放技能英雄已挂[role_id:%d, hero_info_id:%d]", 
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

// 血量变化
void CPVPManager::OnNetHPChange(Player *pPlayer, DWORD dwSrcObjId, DWORD dwTgtObjId, int nChangeHP, int nDamageType, DWORD dwClientPower, const char *pSign)
{
	MAPLOG(GUI_LEVEL, "OnNetHPChange[RoleId:%d,SrcObjId:%d,TgtObjId:%d,ChangeHP:%d,DamageType:%d,ClientPower:%d]",
		pPlayer->GetRoleId(), dwSrcObjId, dwTgtObjId, nChangeHP, nDamageType, dwClientPower);

	// role + power + hp + cost_time + "HelloWorld"
	char szBuff[128] = { 0 };
	sprintf_s(szBuff, sizeof(szBuff), "%d%d%d%s", pPlayer->GetRoleId(), dwClientPower, nChangeHP, "HelloWorld");
	string strMd5Sign = MD5::Encode(szBuff).HexString();

	// 签名不对,有可能被改包
	if (strcmp(strMd5Sign.c_str(), pSign) != 0)
	{
		// 封号处理
		//pPlayer->LockAccount();

		pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的签名异常[role_id:%d,server_sign:%s,client_sign:%s]",
			__FUNCTION__,pPlayer->GetRoleId(), strMd5Sign.c_str(), pSign);
		return;
	}

	// 检测战力
	DWORD dwServerPower = pPlayer->GetFightingPower();
	if (!g_MapServer.CheckPowerIsValid(dwServerPower, dwClientPower))
	{
		// 封号处理
		pPlayer->LockAccount();
		pPlayer->SendErrorMsg(ERROR_ACCOUNT_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]战斗结束请求的战力异常[role_id:%d,server_power:%d,client_power:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), dwServerPower, dwClientPower);
		return;
	}

	// dwSrcObjId 精灵为0

	WORD wFromMsgId = C2S_PVP_HP_CHANGE_REQ;

	// 检测是否在准备状态
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有在战斗中[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]挑战目标没在线[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	DWORD dwSrcHeroInfoId = dwSrcObjId * 0.1;
	// 检测伤害源是否还活着
	if (dwSrcHeroInfoId && IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwSrcHeroInfoId))
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]产生伤害的英雄已挂[role_id:%d, hero_info_id:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), dwSrcHeroInfoId);
		return;
	}

	// 取扮演类型
	BYTE byTargetActType = GetBattlefieldActType(pBattleData, dwTargetId);
	if (PVP_ACT_INVALID == byTargetActType)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]扮演者类型出错[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 目标英雄所属类型
	BYTE byHeroActType = dwTgtObjId % 10;
	if (byHeroActType >= 2)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]血量变化的英雄阵营类型出错[TgtObjId:%d]", __FUNCTION__, dwTgtObjId);
		return;
	}

	// 更新HP(有可能对目标伤害,也有可能是给自己加奶)
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

	// 所有的英雄都已挂掉
	if (IsAllHeroDead(pBattleData, byHeroActType/*byTargetActType*/))
	{
		// 如果伤害的英雄是目标,则我是获胜者
		DWORD dwWinnerId = (byHeroActType == byTargetActType) ? pPlayer->GetRoleId() : dwTargetId;

		// 战斗结束
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
// 		MAPLOG(ERROR_LEVEL, "英雄[id:%d,hp:%d]", tHeroData.wHeroInfoId, tHeroData.nCurHP);
// 	}
}

// 创建buff
void CPVPManager::OnNetCreateBuff(Player *pPlayer, DWORD dwBuffId, DWORD dwSrcObjId, DWORD dwTgtObjId, BYTE byIsFromSpriteBless)
{
	WORD wFromMsgId = C2S_PVP_CREATE_BUFF_REQ;

	// 检测是否在准备状态
	PVPBattlefieldData *pBattleData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattleData || !pBattleData->bIsInBattle)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有在战斗中[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	DWORD dwTargetId = GetTargetId(pBattleData, pPlayer->GetRoleId());

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pTarget)
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]挑战目标没在线[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	DWORD dwHeroInfoId = dwSrcObjId * 0.1;

	// 检测伤害源是否还活着
	if (IsHeroDead(pBattleData, pPlayer->GetRoleId(), dwHeroInfoId))
	{
		//pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]产生buff的英雄已挂[role_id:%d, hero_info_id:%d]",
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

// 战斗结果(只有打机器人才会发生)
void CPVPManager::OnNetBattleResult(Player *pPlayer, BYTE byResult)
{
	WORD wFromMsgId = C2S_PVP_BATTLE_RESULT_REQ;

	// 检测是否在准备状态
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(pPlayer->GetRoleId());
	if (!pBattlefieldData || !pBattlefieldData->bIsInBattle)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有在战斗中[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否挑战机器人
	if (!pBattlefieldData->bIsRobot)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家挑战的不是机器人,不能直接结束[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 机器人属于蓝
	DWORD dwTargetId = GetTargetId(pBattlefieldData, pPlayer->GetRoleId());

	// 取目标玩家数据
	PVPRoleBattleData *pTargetBattleData = GetRoleBattleData(pBattlefieldData, dwTargetId);
	if (!pTargetBattleData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到目标战斗数据[target_id:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	//BYTE byResult = byIsWin ? BATTLE_RESULT_WIN : BATTLE_RESULT_LOSE;

	// 战斗结束
	DoBattleOverWithRobot(pPlayer, pTargetBattleData->dwFightingPower, byResult);
}

// 领取挑战奖励
void CPVPManager::OnNetTakeBattleReward(Player *pPlayer, BYTE byType, BYTE *pTimesList, WORD wListNum)
{
	WORD wFromMsgId = C2S_PVP_TAKE_BATTLE_REWARD_REQ;

	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	WORD *pTakeRecordList = NULL;
	WORD *pTakeRecordNum = 0;

	// 每日的
	if (PVP_BATTLE_REWARD_DAILY == byType)
	{
		pTakeRecordList = tPVPData.arDailyRewardTakeRecord;
		pTakeRecordNum = &tPVPData.wDailyRewardTakeRecordNum;
	}
	// 每周的
	else
	{
		pTakeRecordList = tPVPData.arWeeklyRewardTakeRecord;
		pTakeRecordNum = &tPVPData.wWeeklyRewardTakeRecordNum;
	}

	// 做一层转换,为了方便操作
	WORD &wTakeRecordNum = *pTakeRecordNum;

	// 检测是否已领取过
	for (WORD i = 0; i < wListNum; ++i)
	{
		for (WORD k = 0; k < wTakeRecordNum; ++k)
		{
			if (pTakeRecordList[k] == pTimesList[i])
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]奖励已经领取过[times:%d][type:%d]", __FUNCTION__, pTimesList[i], byType);
				return;
			}
		}
	}

	// 处理奖励
	for (WORD i = 0; i < wListNum; ++i)
	{
		BYTE byBattleTimes = pTimesList[i];
		// 取配置
		const PVPBattleReward_Config *pRewardCfg = g_pCfgMgr->GetPVPBattleReward(byType, byBattleTimes);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到PVP战斗奖励[times:%d][type:%d]", __FUNCTION__, byBattleTimes, byType);
			continue;
		}

		WORD wCurBattleTimes = PVP_BATTLE_REWARD_DAILY == byType ? tPVPData.wTodayBattleTimes : tPVPData.wWeekWinTimes;

		// 检测挑战次数是否足够
		if (wCurBattleTimes < byBattleTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]挑战次数不足,不能领取[times:%d][type:%d]", __FUNCTION__, byBattleTimes, byType);
			return;
		}

		pPlayer->AddObjectList(pRewardCfg->vItemList, TRUE, FROM_PVP_BATTLE_REWARD);
	}

	// 记录
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
		MAPLOG(ERROR_LEVEL, "[%s]挑战者没在线[role_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战者没在线[role_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	PVPWaitBattleRoleData *pWaitBattleData = FindWaitBattleRoleData(dwTargetId);
	if (!pWaitBattleData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到等待战斗数据[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	CSpriteManager *pSrcSpiritMgr = pPlayer->GetSpiritManager();
	CSpriteManager *pTgtSpiritMgr = pTarget->GetSpiritManager();
	if (!pSrcSpiritMgr || !pTgtSpiritMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精灵管理器为空", __FUNCTION__);
		return false;
	}

	// 初始化战斗数据
	PVPBattlefieldData tBattleData;
	memset(&tBattleData, 0, sizeof(tBattleData));
	tBattleData.dwStartTime = time(NULL);
	tBattleData.bIsInBattle = false;
	tBattleData.dwBattleId = 0;
	tBattleData.bIsRobot = false;

	vector<ToBattleHeroData> vToBattleHeroList;
	int nBattleHeroDataSize = sizeof(ToBattleHeroData);

	// 英雄血量添加比例
	float fHPIncreRate = g_GetConfigValue(207) * PROPERTY_DECREASE_MULTIPLE;

	// 自己数据
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
			// 计算总HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // 添加英雄血量比例
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}

		// 取精灵列表
		tRedRoleData.wSpriteNum = 0;
		WORD wSpriteNum = pPlayer->GetSpiritManager()->GetSpriteListNum();
		const SpiritData *pSpriteList = pPlayer->GetSpiritManager()->GetSpriteList();
		for (WORD i = 0; i < wSpriteNum; ++i)
		{
			tRedRoleData.arSpriteList[tRedRoleData.wSpriteNum++] = pSpriteList[i];
		}
	}


	// 目标数据
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
			// 计算总HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // 添加英雄血量比例
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}

		// 取精灵列表
		tBlueRoleData.wSpriteNum = 0;
		WORD wSpriteNum = pTarget->GetSpiritManager()->GetSpriteListNum();
		const SpiritData *pSpriteList = pTarget->GetSpiritManager()->GetSpriteList();
		for (WORD i = 0; i < wSpriteNum; ++i)
		{
			tBlueRoleData.arSpriteList[tBlueRoleData.wSpriteNum++] = pSpriteList[i];
		}
	}

	AddBattlefieldData(tBattleData);

	// 通知进入准备战斗
	MSG_S2C_PVP_READY_NOTIFY msg;
	msg.m_byIsRobot = false;
	msg.m_pBattleData = &tBattleData;
	pPlayer->SendPacket(&msg);
	pTarget->SendPacket(&msg);

	return true;
}

// 准备进入战斗(机器人)
bool CPVPManager::ReadyToBattleWithRobot(DWORD dwRoleId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (!pPlayer)
	{
		MAPLOG(ERROR_LEVEL, "[%s]挑战者没在线[role_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	PVPWaitBattleRoleData *pWaitBattleData = FindWaitBattleRoleData(dwRoleId);
	if (!pWaitBattleData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到等待战斗数据[target_id:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	CSpriteManager *pSpiritMgr = pPlayer->GetSpiritManager();
	if (!pSpiritMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精灵管理器为空", __FUNCTION__);
		return false;
	}


	// 初始化战斗数据
	PVPBattlefieldData tBattleData;
	memset(&tBattleData, 0, sizeof(tBattleData));
	tBattleData.dwStartTime = time(NULL);
	tBattleData.bIsInBattle = false;
	tBattleData.dwBattleId = 0;
	tBattleData.bIsRobot = true;

	vector<ToBattleHeroData> vToBattleHeroList;
	int nBattleHeroDataSize = sizeof(ToBattleHeroData);

	DWORD dwFightingPower = pPlayer->GetFightingPower();

	// 英雄血量添加比例
	float fHPIncreRate = g_GetConfigValue(207) * PROPERTY_DECREASE_MULTIPLE;

	// 自己数据
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
			// 计算总HP
			for (BYTE k = 0; k < tFromData.byProNum; ++k)
			{
				if (tFromData.arPorList[k].wType == HERO_PRO_BLOOD)
				{
					tToData.arPorList[k].nValue = tFromData.arPorList[k].nValue * fHPIncreRate; // 添加英雄血量比例
					tToData.dwTotalHP = tToData.nCurHP = tToData.arPorList[k].nValue;
					break;
				}
			}
		}
	}

	// 目标数据(使用自己的数据)
	{
		// 属性浮动(HP, attck, 上下5%)
		float fFloatRange = PROPERTY_DECREASE_MULTIPLE * g_GetConfigValue(172);

		// 找不到则使用自己
		vToBattleHeroList.clear();
		pPlayer->FillToBattleHeroData(vToBattleHeroList);
		PVPRoleBattleData &tBlueRoleData = tBattleData.arRoleBattleDataList[PVP_ACT_BLUE];
		tBlueRoleData.byType = PVP_ACT_BLUE;
		tBlueRoleData.dwRoleId = PVP_ROBOT_ID;
		tBlueRoleData.dwFightingPower = dwFightingPower;
		// 随机浮动范围
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
				// 属性调整(hp, attck, 上下5%)
				if (tPro.wType == HERO_PRO_BLOOD || tPro.wType == HERO_PRO_ATTACK)
				{
					// 随机浮动范围
					if (g_Rand(0, 1))
						tPro.nValue -= tPro.nValue * fFloatRange;
					else
						tPro.nValue += tPro.nValue * fFloatRange;
				}

				// 记录当前及总HP
				if (tPro.wType == HERO_PRO_BLOOD)
				{
					tPro.nValue *= fHPIncreRate; // 添加英雄血量比例
					tToData.dwTotalHP = tToData.nCurHP = tPro.nValue;
					break;
				}
			}

			// 替换英雄(同品质)
			const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(tToData.wHeroInfoId);
			if (pHeroCfg)
			{
				vector<DWORD> *pPokedexList = g_pCfgMgr->GetHeroPokedexList(pHeroCfg->wInitQuality);
				if (!pPokedexList || !pPokedexList->size())
				{
					MAPLOG(ERROR_LEVEL, "[%s]找到英雄品质对应的图鉴[quality:%d]", __FUNCTION__, pHeroCfg->wInitQuality);
					continue;
				}
				
				// todo::此处理逻辑不太满意,日后调整
				int nDoTimes = 0;
				while (true)
				{
					// 防止由于配置造成死循环
					if (nDoTimes++ >= 1000)
					{
						MAPLOG(ERROR_LEVEL, "[%s]执行循环超过1000次被强行退出", __FUNCTION__);
						break;
					}

					// 随机一个同品质的英雄
					int nIndex = g_Rand(0, pPokedexList->size() - 1);
					DWORD dwHeroInfoId = pPokedexList->at(nIndex);

					// 检测是否已有相同英雄上阵
					bool bExist = false;
					for (size_t k = 0; k < vNewHeroList.size(); ++k)
					{
						if (dwHeroInfoId == vNewHeroList[k])
						{
							bExist = true;
							break;
						}
					}

					// 替换
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

	// 通知进入准备战斗
	MSG_S2C_PVP_READY_NOTIFY msg;
	msg.m_byIsRobot = true;
	msg.m_pBattleData = &tBattleData;
	pPlayer->SendPacket(&msg);

	return true;
}

// 取扮演类型
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

// 取扮演类型
BYTE CPVPManager::GetBattlefieldActType(const PVPBattlefieldData *pBattleData, DWORD dwRoleId)
{
	for (int i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		if (pBattleData->arRoleBattleDataList[i].dwRoleId == dwRoleId)
			return pBattleData->arRoleBattleDataList[i].byType;
	}
	return PVP_ACT_INVALID;
}

// 英雄是否已死亡
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

// 是否所有的英雄都挂掉
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

// 通过战力筛选目标
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

// 删除战斗目标数据
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

// 更新英雄HP
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
			// 上限纠正
			tHeroData.nCurHP = min(tHeroData.nCurHP, int(tHeroData.dwTotalHP));
			// 下限纠正
			tHeroData.nCurHP = max(tHeroData.nCurHP, 0);
			break;
		}
	}
	return nResult;
}

// 取战斗目标ID
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

// 删除战场数据
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

// 是否活动开放
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

	// 检测是否在指定范围内的时间
// 	if (nCurTime >= pActivityTimeCfg->nStartTime && nCurTime <= pActivityTimeCfg->nOverTime)
// 		return true;

	return false;
}

// 是否有红点
bool CPVPManager::IsHaveRedPoint(Player *pPlayer)
{
	// todo::是否活动时间
// 	if (!pPlayer->GetActivityManager()->IsActivityOpen(SYSTEM_PVP))
// 		return false;
	
	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	map<DWORD, PVPBattleReward_Config>::iterator iter = g_pCfgMgr->m_PVPBattleRewardList.begin();
	for (; iter != g_pCfgMgr->m_PVPBattleRewardList.end(); ++iter)
	{
		// 每日奖励
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
		// 每周奖励
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

// 处理排行奖励
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

				MAPLOG(ERROR_LEVEL, "[%s]PVP排行发放奖励[role_id:%d,rank:%d,rank_value:%d]",
					__FUNCTION__, iter->dwRoleId, iter->wRank, iter->dwValue);
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]PVP排行榜已发放奖励", __FUNCTION__);
}


// 处理排行称号
void CPVPManager::DoRankTitle()
{
	// 全服排行称号奖励
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

				MAPLOG(ERROR_LEVEL, "[%s]PVP排行发放称号奖励[role_id:%d,rank:%d,title_id:%d]",
					__FUNCTION__, iter->dwRoleId, iter->wRank, pRewardCfg->dwTitleId);
			}
				
		}
		CRankManager::Instance()->ResetRoleRank(ALL_SERVER_ID, RANK_PVP_ALL);
	}

	// 单服排行重置
	CRankManager::Instance()->ResetRoleRank(RANK_PVP_LOCAL);

	MAPLOG(ERROR_LEVEL, "[%s]PVP排行榜已发放称号奖励", __FUNCTION__);
}

// 修改积分(GM使用)
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

// 取角色战斗数据
PVPRoleBattleData* CPVPManager::GetRoleBattleData(PVPBattlefieldData *pBattleData, DWORD dwRoleId)
{
	for (WORD i = 0; i < PVP_BATTLEFIELD_ROLE_NUM; ++i)
	{
		if (pBattleData->arRoleBattleDataList[i].dwRoleId == dwRoleId)
			return &pBattleData->arRoleBattleDataList[i];
	}
	return NULL;
}

// 取角色战斗数据
PVPRoleBattleData* CPVPManager::GetRoleBattleData(DWORD dwRoleId)
{
	PVPBattlefieldData *pBattlefieldData = FindBattlefieldData(dwRoleId);
	return pBattlefieldData ? GetRoleBattleData(pBattlefieldData, dwRoleId) : NULL;
}

// 取角色战斗数据
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

// 计算获胜积分
DWORD CPVPManager::CalculateWinScore(DWORD dwTargetFightingPower)
{
	// 积分 = 50+对手战力/5万
	return  50 + ceil(dwTargetFightingPower * 0.00002);
}

// 计算失败积分
DWORD CPVPManager::CalculateLoseScore(DWORD dwWinScore)
{
	//  失败积分 = 胜利积分 / 2  向上取整
	return ceil(dwWinScore * 0.5);
}

// 取平局积分
DWORD CPVPManager::CalculateDrawScore(DWORD dwWinScore)
{
	//  平局积分 = 胜利积分*0.75 或 失败积分*1.5
	return ceil(dwWinScore * 0.75);
}

// 计算积分
DWORD CPVPManager::CalculateScore(BYTE byResult, DWORD dwTargetFightingPower)
{
	DWORD dwScore = 0;

	DWORD dwWinScore = CalculateWinScore(dwTargetFightingPower);

	// 胜利
	if (BATTLE_RESULT_WIN == byResult)
	{
		dwScore = dwWinScore;
	}
	// 平局
	else if (BATTLE_RESULT_DRAW == byResult)
	{
		dwScore = CalculateDrawScore(dwWinScore);
	}
	// 失败
	else
	{
		dwScore = CalculateLoseScore(dwWinScore);
	}

	return dwScore;
}

// 发送玩家PVP数据
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
		for (size_t i = 0; i < pRankList->size() && i < 3/*todo::暂定这样处理*/; ++i)
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

// 开始战斗通知
void CPVPManager::DoBattleStart(Player *pPlayer, Player *pTarget)
{
	// 开始战斗
	MSG_S2C_PVP_BATTLE_START_NOTIFY notify;

	if (pPlayer)
	{
		pPlayer->SendPacket(&notify);
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wTodayBattleTimes++;
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.wWeekBattleTimes++;
		pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData.dwTotalBattleTimes++;

		// 日常任务(参与英雄对决X次)
		pPlayer->OnUpdateDailyMission(24);

		// 成就事件(英雄对决累计战斗Ｘ场)
		pPlayer->OnUpdateAchievement(24);
	}

	if (pTarget)
	{
		pTarget->SendPacket(&notify);
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.wTodayBattleTimes++;
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.wWeekBattleTimes++;
		pTarget->GetRoleFullInfo().tSecondInfo.tPVPData.dwTotalBattleTimes++;

		// 日常任务(参与英雄对决X次)
		pTarget->OnUpdateDailyMission(24);

		// 成就事件(英雄对决累计战斗Ｘ场)
		pTarget->OnUpdateAchievement(24);
	}
}

// 处理战斗结束
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
  
  	// 分胜负
  	if (dwWinnerId)
  	{
		if (pRedPlayer->GetRoleId() == dwWinnerId)
  		{
			// 红方胜利
			msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_WIN, dwBlueFightingPower);
			AddScore(pRedPlayer, dwScore, true);
			msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
			msg.m_wRedScore = dwScore;

			// 蓝方失败
			msg.m_wBlueOldRank = GetRank(pBluePlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_LOSE, dwRedFightingPower);
			AddScore(pBluePlayer, dwScore);
			msg.m_wBlueNewRank = GetRank(pBluePlayer->GetRoleId());
			msg.m_wBlueScore = dwScore;
  		}
  		else
  		{
			// 红方失败
			msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_LOSE, dwBlueFightingPower);
			AddScore(pRedPlayer, dwScore);
			msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
			msg.m_wRedScore = dwScore;

			// 蓝方胜利
			msg.m_wBlueOldRank = GetRank(pBluePlayer->GetRoleId());
			dwScore = CalculateScore(BATTLE_RESULT_WIN, dwRedFightingPower);
			AddScore(pBluePlayer, dwScore, true);
			msg.m_wBlueNewRank = GetRank(pBluePlayer->GetRoleId());
			msg.m_wBlueScore = dwScore;
  		}
  	}
  	// 平局
  	else
  	{  
		// 红方
		msg.m_wRedOldRank = GetRank(pRedPlayer->GetRoleId());
		dwScore = CalculateScore(BATTLE_RESULT_DRAW, dwBlueFightingPower);
		AddScore(pRedPlayer, dwScore);
		msg.m_wRedNewRank = GetRank(pRedPlayer->GetRoleId());
		msg.m_wRedScore = dwScore;

		// 蓝方
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
  
  	// 删除战场
	RemoveBattlefieldData(pRedPlayer->GetRoleId());
}

// 处理战斗结束(机器人)
void CPVPManager::DoBattleOverWithRobot(Player *pPlayer, DWORD dwTargetFightinPower, BYTE byResult)
{
 	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;
 	DWORD dwSrcScore = 0;
	DWORD dwTgtScore = 0;
	DWORD dwSrcFightingPower = pPlayer->GetFightingPower();

	// 胜利
	if (BATTLE_RESULT_WIN == byResult)
	{
		// 获得积分
		dwSrcScore = CalculateScore(BATTLE_RESULT_WIN,dwTargetFightinPower);
		dwTgtScore = CalculateScore(BATTLE_RESULT_LOSE, dwSrcFightingPower);
	}
	// 平局
	else if (BATTLE_RESULT_DRAW == byResult)
	{
		// 获得积分
		dwSrcScore = CalculateScore(BATTLE_RESULT_DRAW, dwTargetFightinPower);
		dwTgtScore = CalculateScore(BATTLE_RESULT_DRAW, dwSrcFightingPower);
	}
	// 失败
	else
	{
		// 获得积分
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

	// 删除战场
	RemoveBattlefieldData(pPlayer->GetRoleId());
}

// 添加积分
void CPVPManager::AddScore(Player *pPlayer, DWORD dwScore, BYTE byWinner)
{
	bool bNeedUpdateRank = false;

	PlayerPVPData &tPVPData = pPlayer->GetRoleFullInfo().tSecondInfo.tPVPData;

	// 挑战奖励次数上限(超过此次数则无奖励)
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
		
	// 更新排名榜
	if (bNeedUpdateRank)
	{
		CRankManager::Instance()->AddRoleRankData(RANK_PVP_LOCAL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, MAX_DEFAULT_RANK_MUM);
		CRankManager::Instance()->AddRoleRankData(RANK_PVP_ALL, pPlayer, tPVPData.dwScore, tPVPData.wWeekWinTimes, 0, 100);
	}
}

// 取排名
WORD CPVPManager::GetRank(DWORD dwRoleId)
{
	return CRankManager::Instance()->GetRoleRank(dwRoleId, ALL_SERVER_ID, RANK_PVP_ALL);
}
