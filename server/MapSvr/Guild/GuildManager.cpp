#include "stdafx.h"
#include "GuildManager.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Rank\RankManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "GuildWarMgr.h"

#define GET_GUILD_SERVER_ID(GuildId) GuildId * 0.000001

DWORD GetGuildServerId(DWORD dwGuildId)
{
	DWORD dwCreateServerId = DWORD(dwGuildId * 0.000001);

	DWORD dwCurServerId = g_MapServer.GetCurServerId(dwCreateServerId);

	return dwCurServerId;
}

CGuildManager::CGuildManager()
{
	m_dwSeriesId = 1;
	m_vLimitGoodsList.clear();

	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(45);
	if (pServerCfg && pServerCfg->vValueList.size())
	{
		for (size_t i = 0; i < pServerCfg->vValueList.size(); ++i)
		{
			m_LimitShopRefreshTimer.AddTriggerTime(pServerCfg->vValueList[i]);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有配置工会限时商店刷新的时间段!", __FUNCTION__);
	}
}

CGuildManager::~CGuildManager()
{

}

void CGuildManager::Init()
{
	m_AllGuildList.clear();
	m_dwSeriesId = 1;
	m_dwLimitGoodsRefreshTime = time(NULL);
	RefreshLimitGoods();
}

void CGuildManager::Release()
{

}

void CGuildManager::Update()
{
	if (m_LimitShopRefreshTimer.IsTrigger())
	{
		RefreshLimitGoods();
		m_LimitShopRefreshTimer.SetCDOverTime();
	}
}

void CGuildManager::OnNewDay()
{
	ALL_GUILD_MAP::iterator iter1 = m_AllGuildList.begin();
	for (; iter1 != m_AllGuildList.end(); ++iter1)
	{
		GUILD_MAP::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2){
			iter2->second->OnNewDay();
		}
	}
}

void CGuildManager::OnNewWeek()
{
	ALL_GUILD_MAP::iterator iter1 = m_AllGuildList.begin();
	for (; iter1 != m_AllGuildList.end(); ++iter1)
	{
		GUILD_MAP::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2){
			iter2->second->OnNewWeek();
		}
	}
}

// 请求加载数据
void CGuildManager::LoadDataRequest()
{
	GuildData_Load* pQuery = GuildData_Load::ALLOC();
	pQuery->SetIndex(DB_GUILD_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	g_MapServer.MapDBQuery(pQuery);
}

// 加载数据
void CGuildManager::LoadDataFromDB(const char *pszData, int nSize)
{
	PB::GuildInfo PBGuildInfo;
	if (!PBGuildInfo.ParseFromArray(pszData, nSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化工会数据出错!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = new CGuild;

	// 拷贝工会数据
	pGuild->LoadDataFromDB(this,&PBGuildInfo);

	AddGuild(pGuild->GetServerId(), pGuild);
	pair<GUILD_NAME_MAP::iterator, bool> ret = m_NameList.insert(make_pair(pGuild->GetGuildName(), pGuild->GetGuildId()));
	if (!ret.second)
	{
		MAPLOG(ERROR_LEVEL, "[%s]添加工会名称失败[guild_id:%d,guild_name:%s]!",
			__FUNCTION__, pGuild->GetGuildId(), pGuild->GetGuildName());
	}

	if (pGuild->GetGuildId())
		m_dwSeriesId = max(m_dwSeriesId, pGuild->GetGuildId() % 1000000);
}

// 取工会数量
int CGuildManager::GetGuildNum()
{
	return m_NameList.size();
}

// 是否有红点
bool CGuildManager::IsHaveRedPoint(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	return pGuild ? pGuild->IsHaveRedPoint(pPlayer) : false;
}

// 请求工会数据
void CGuildManager::OnGuildData(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendGuildData(pPlayer);
	}
	else
	{
		MSG_S2C_GUILD_DATE_RESP msg;
		msg.m_byPosition = 0;
		memset(&msg.m_GuildData, 0, sizeof(msg.m_GuildData));
		msg.m_byMemberNum = 0;
		msg.m_byQYType = 0;
		memset(msg.m_szCaptainName, 0, sizeof(msg.m_szCaptainName));
		msg.m_byQSRewardRecordNum = 0;
		memset(msg.m_arQSRewardRecord, 0, sizeof(msg.m_arQSRewardRecord));
		msg.m_wRedPointNum = 0;
		msg.m_byLastQYType = 0;
		msg.m_dwLastQYValue = 0;
		memset(msg.m_szLastQYTargetName, 0, sizeof(msg.m_szLastQYTargetName));
		pPlayer->SendPacket(&msg);
	}
}

// 请求工会列表
void CGuildManager::OnGuildList(Player *pPlayer)
{
	MSG_S2C_GUILD_LIST_RESP msg;
	msg.m_byGuildNum = 0;
	GUILD_MAP *pGuildList = GetGuildList(pPlayer->GetServerId());
	if (pGuildList)
	{
		std::map<DWORD, CGuild*>::iterator iter = pGuildList->begin();
		for (; iter != pGuildList->end(); ++iter)
		{
			if (msg.m_byGuildNum >= MAX_GUILD_NUM)
				break;

			GuildBaseData &tBaseData = msg.m_GuildList[msg.m_byGuildNum++];
			iter->second->FillGuildData(tBaseData);
			tBaseData.byIsApply = iter->second->IsInApplyList(pPlayer->GetCharID());
		}
	}
	pPlayer->SendPacket(&msg);
}

// 请求工会成员列表
void CGuildManager::OnGuildMemberList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendMemberList(pPlayer);
	}
}

// 请求留言列表
void CGuildManager::OnLeaveMsgList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendLeaveMsgList(pPlayer);

		// 更新新留言状态
		GuildMember *pMember = pGuild->FindMemberData(pPlayer->GetRoleId());
		if (pMember)
			pMember->bHaveNewLeaveMessage = false;
	}
}

// 请求申请列表
void CGuildManager::OnGuildApplyList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendApplyList(pPlayer);
	}
}

// 请求工会事件列表
void CGuildManager::OnGuildEventList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendEventList(pPlayer);
	}
}

// 创建工会
void CGuildManager::OnCreate(Player *pPlayer, WORD wIconId, const char *pszName)
{
	WORD wFromMsgId = C2S_GUILD_CREATE_REQ;

	if (!pszName || !pszName[0])
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]创建工会名称不为空!", __FUNCTION__);
		return;
	}

	// 检测钻石是否足够
	int nCostDiamond = g_GetConfigValue(CONFIG_GUILD_CREATE_COST_GOLD);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]金钱不足,创建工会失败!", __FUNCTION__);
		return;
	}

	// VIP等级
	if (pPlayer->GetVipValue(VIP_CREATE_GUILD_FLAG) <= 0)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]VIP等级限制,创建工会失败!", __FUNCTION__);
		return;
	}

	// 检测需要的充值额度
	DWORD dwNeedRechargeAmount = g_GetConfigValue(310);
	if (dwNeedRechargeAmount && pPlayer->GetProperty(ROLE_PRO_RECHARGE_AMOUNT) < dwNeedRechargeAmount)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]充值额度不足,创建工会失败!", __FUNCTION__);
		return;
	}

	// todo::敏感字过滤

	// 检测名称是否已被使用
	if (IsGuildNameExist(pszName))
	{
		pPlayer->SendErrorMsg(ERROR_GUILD_NAME_EXIST);
		MAPLOG(ERROR_LEVEL, "[%s]工会名称已被使用,创建工会失败![%s]", __FUNCTION__, pszName);
		return;
	}

	// 检测是否有公会
	if (GetGuildIdByRoleId(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_GUILD_ALREADY_IN_GUILD);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在工会,创建工会失败", __FUNCTION__);
		return;
	}

	CGuild *pGuild = new CGuild;
	if (!pGuild->Init(pPlayer, this, wIconId, pszName))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]创建工会初始化失败!", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_GUILD_CREATE);

	AddFindData(pPlayer->GetCharID(), pGuild->GetGuildId());

	AddGuild(pPlayer->GetServerId(), pGuild);

	pGuild->SendGuildData(pPlayer);

	m_NameList.insert(make_pair(pGuild->GetGuildName(), pGuild->GetGuildId()));

	// todo::暂时这样处理，日后调整
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, pGuild->GetGuildId());

	// 如果是副本开放时间
	if (pGuild->IsFBOpenTime())
	{
		pGuild->CreateMonster();
	}

	// 更新限时商品列表
	pGuild->UpdateLimitGoods(m_vLimitGoodsList);

	MSG_S2C_GUILD_CREATE_RESP msg;
	msg.m_wIconId = wIconId;
	strcpy_s(msg.m_szGuildName, sizeof(msg.m_szGuildName), pszName);
	pPlayer->SendPacket(&msg);

	// 更新排行榜
	CRankManager::Instance()->AddGuildRankData(pPlayer->GetServerId(), pGuild->GetGuildId(), GUILD_RANK_LEVEL, pGuild->GetGuildLevel());

	pGuild->SaveDataToDB();

	pPlayer->UpdateFightingPower();
}

// 解散工会
void CGuildManager::OnDismiss(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_DISMISS_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]没有在工会,解散失败!", __FUNCTION__);
		return;
	}

	const GuildMember *pCaptainData = pGuild->GetCaptainData();
	if (!pCaptainData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]取不到工会会长数据!", __FUNCTION__);
		return;
	}

	// 检测是否有权限
	if (pCaptainData->dwRoleId != pPlayer->GetCharID())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]没有权限解散工会!", __FUNCTION__);
		return;
	}

	// 大于指定人数据也不能解散(目前10人)
	if (pGuild->GetMemberNum() > BYTE(g_GetConfigValue(CONFIG_GUILD_DISMISS_MEMBER_NUM_LIMIT,10)))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]工会成员超过指定的人数不能解散!", __FUNCTION__);
		return;
	}

	// 检测是否工会战期间 星球争夺战期间，请勿解散军团
	const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(6);
	if (CGuildWarMgr::Instance()->CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE) 
		&& pBuildingCfg 
		&& pGuild->GetGuildLevel() >= pBuildingCfg->wNeedGuildLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]星球争夺战期间，请勿解散军团!", __FUNCTION__);
		return;
	}

	DWORD dwServerId = pGuild->GetServerId();
	DWORD dwGuildId = pGuild->GetGuildId();

	pGuild->SaveDataToDB(true);

	pGuild->Dismiss();

	// 从排行榜里拿掉
	CRankManager::Instance()->RemoveGuildRandData(dwServerId, dwGuildId, GUILD_RANK_LEVEL);

	RemoveGuild(pGuild->GetGuildId(), pGuild->GetGuildName());

	// 通知客户端清0
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, 0);

	pPlayer->UpdateFightingPower();

	MSG_S2C_GUILD_DISMISS_RESP msg;
	pPlayer->SendPacket(&msg);

	CGuildWarMgr::Instance()->OnDismissGuild(dwServerId, dwGuildId);
}

// 申请加入 
void CGuildManager::OnApply(Player *pPlayer, DWORD dwGuildId)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_REQ;

	// 检测玩家是否已在工会
	if (GetGuildByRoleId(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已有工会,申请失败!", __FUNCTION__);
		return;
	}

	// 检测是否在退出工会CD中
	DWORD dwLeaveTime = pPlayer->GetProperty(ROLE_PRO_LEAVE_GUILD_TIME);
	if (dwLeaveTime)
	{
		DWORD dwPassTime = g_GetPassTime(dwLeaveTime, time(NULL));
		if (dwPassTime < g_Hour2Second(g_GetConfigValue(121)))
		{
			MAPLOG(GUI_LEVEL, "[%s]退出公会后,24小时内不可申请加入新的公会!", __FUNCTION__);
			pPlayer->SendErrorMsg(ERROR_LEAVE_GUILD_CD);
			return;
		}
	}

	GUILD_APPLY_MAP::iterator apply_iter = m_ApplyList.find(pPlayer->GetCharID());
	// 检测玩家申请数量是否已达上限
	if (apply_iter != m_ApplyList.end())
	{
		if (apply_iter->second.size() >= g_GetConfigValue(119,5))
		{
			pPlayer->SendErrorMsg(ERROR_GUILD_APPLY_NUM_LIMIT);
			// 申请次数已达上限
			MAPLOG(ERROR_LEVEL, "[%s]申请加入工会数量已达上限!", __FUNCTION__);
			return;
		}

		// 检测是否已申请过
		vector<DWORD>::iterator iter = apply_iter->second.begin();
		for (; iter != apply_iter->second.end(); ++iter)
		{
			if (*iter == dwGuildId)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]已在申请列表里!", __FUNCTION__);
				return;
			}
		}
	}

	CGuild *pGuild = GetGuildById(dwGuildId);

	// 检测目标工会是否存在
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]申请加入的工会不存在![guildid:%d]", __FUNCTION__,dwGuildId);
		return;
	}

	// 检测目标工会是否人数已满
	if (pGuild->IsMemberFull())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]工会成员数量已满!",__FUNCTION__);
		return;
	}

	// 加入申请列表
	AddApplyData(pPlayer->GetCharID(), dwGuildId);

	pGuild->AddApplyData(pPlayer);

	MSG_S2C_GUILD_APPLY_RESP msg;
	msg.m_dwGuildId = dwGuildId;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// 审核申请
void CGuildManager::OnApplyCheck(Player *pPlayer, DWORD dwTargetId, BYTE byIsAccept)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_CHECK_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有工会,不能进行申请审核!",__FUNCTION__);
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家在工会的成员数据!", __FUNCTION__);
		return;
	}

	// todo::检测是否有审核权限
	if (GUILD_POS_MEMBER == pMember->byPosition)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]没有权限处理工会申请加入审核!!", __FUNCTION__);
		return;
	}

	if (byIsAccept)
	{
		// 检测是否有申请过
		//GuildApplyData *pApplyData = pGuild->IsInApplyList(dwRoleId);
		if (!pGuild->IsInApplyList(dwTargetId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]目标没有在申请加入工会列表里[RoleId:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// 检测工会人数是否已满
		if (pGuild->IsMemberFull())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]工会人数已满", __FUNCTION__);
			return;
		}

		// 检测目标是否已在工会
		if (GetGuildByRoleId(dwTargetId))
		{
			pPlayer->SendErrorMsg(ERROR_GUILD_TARGET_HAVE_GUILD);
			pGuild->RemoveApplyData(dwTargetId);
			pGuild->SendApplyList(pPlayer); // 主动同步数据
			MAPLOG(ERROR_LEVEL, "[%s]目标已加入别的工会", __FUNCTION__);
			return;
		}

		// 添加成员数据
		GuildMember member;
		member.Init();
		member.dwRoleId = dwTargetId;
		member.byPosition = GUILD_POS_MEMBER;
		member.dwLastBattleTimesRecoverTime = time(NULL);
		pGuild->AddMember(&member);

		pGuild->SendMemberList(pPlayer);

		// 加入查找列表里
		AddFindData(dwTargetId, pGuild->GetGuildId());

		// 去掉所有申请过的信息
		RemoveAllApplyData(dwTargetId);

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, false);
		if (pBaseData)
		{
			pGuild->AddEvent(1, pBaseData->szRoleName);
		}

		// 同步目标
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pTarget)                                      
			pTarget->NotifyProperty(ROLE_PRO_GUILD_ID, pGuild->GetGuildId());
	}
	else
	{
		pGuild->RemoveApplyData(dwTargetId);
		RemoveApplyGuildId(dwTargetId, pGuild->GetGuildId());
	}

	// 响应
	MSG_S2C_GUILD_APPLY_CHECK_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_byIsAccept = byIsAccept;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}


// 取消申请  
void CGuildManager::OnApplyCancel(Player *pPlayer, DWORD dwGuildId)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_CANCEL_REQ;

	CGuild *pGuild = GetGuildById(dwGuildId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到取消申请的工会![GuildId:%s]", __FUNCTION__, dwGuildId);
		return;
	}

	pGuild->RemoveApplyData(pPlayer->GetCharID());
	RemoveApplyGuildId(pPlayer->GetCharID(), dwGuildId);

	MSG_S2C_GUILD_APPLY_CANCEL_RESP msg;
	msg.m_dwGuildId = dwGuildId;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// 踢出工会
void CGuildManager::OnKickOut(Player *pPlayer, DWORD dwTargetId)
{
	WORD wFromMsgId = C2S_GUILD_KICKOUT_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会不能踢人", __FUNCTION__);
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]逻辑上出现严重错误,找不到玩家在工会的数据!",__FUNCTION__);
		return;
	}

	const GuildMember *pTargetMember = pGuild->GetMemberData(dwTargetId);
	if (!pTargetMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]目标木有在工会里无法踢掉!",__FUNCTION__);
		return;
	}

	// 如果踢出的是副会长
	if (GUILD_POS_ASSIST == pTargetMember->byPosition)
	{
		// 检测是否有权限踢人
		if (GUILD_POS_CAPTAIN != pMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]没有权限踢掉工会成员!", __FUNCTION__);
			return;
		}
	}
	// 如果踢出的是普通成员
	else if (GUILD_POS_MEMBER == pTargetMember->byPosition)
	{
		// 检测是否有权限踢人
		if (GUILD_POS_MEMBER == pMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]没有权限踢掉工会成员!", __FUNCTION__);
			return;
		}
	}
	else
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]不能出踢会长!", __FUNCTION__);
		return;
	}

	pGuild->RemoveMember(dwTargetId);
	RemoveFindData(dwTargetId);

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	// 在线则直接设置
	if (pTarget)
	{
		// 记录离开工会时间
		pTarget->ModifyProperty(ROLE_PRO_LEAVE_GUILD_TIME, time(NULL));
		// 更新战力
		pTarget->UpdateFightingPower();
	}
	// 没在线
	else
	{
		// 插入数据库
		RoleTmpData_Update* pQuery = RoleTmpData_Update::ALLOC();
		pQuery->SetIndex(DB_ROLE_TEMP_DATA_UPDATE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		pQuery->SetRoleId(dwTargetId);
		pQuery->SetServerId(pGuild->GetServerId());
		pQuery->AddParam(time(NULL));			// 0
		pQuery->AddParam(time(NULL));			// 1 leave_time
		pQuery->AddParam(pGuild->GetGuildId());	// 2 guild_id
		pQuery->AddParam(pGuild->GetServerId());// 3 server_id
		pQuery->SetDataType(ROLE_TEMP_SET_LEAVE_GUILD_TIME);
		g_MapServer.MapDBQuery(pQuery);
	}

	MSG_S2C_GUILD_KICKOUT_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&msg);

	// 事件记录
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
		pGuild->AddEvent(3, pBaseData->szRoleName);

	pGuild->SaveDataToDB();

	CGuildWarMgr::Instance()->OnLeave(pGuild, dwTargetId);
}

//退出工会
void CGuildManager::OnLeave(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_LEAVE_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能退出![name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]逻辑上出现严重错误,找不到玩家在工会的数据!", __FUNCTION__);
		return;
	}

	// 检测是否可以直接退出
	if (GUILD_POS_CAPTAIN == pMember->byPosition)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]工会会长不能直接退出工会!", __FUNCTION__);
		return;
	}

	pGuild->RemoveMember(pPlayer->GetCharID());
	RemoveFindData(pPlayer->GetCharID());

	// 通知客户端清0
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, 0);

	// 记录离开工会时间
	pPlayer->ModifyProperty(ROLE_PRO_LEAVE_GUILD_TIME, time(NULL));

	MSG_S2C_GUILD_LEAVE_RESP msg;
	pPlayer->SendPacket(&msg);

	pGuild->AddEvent(2, pPlayer->GetRoleName());

	pGuild->SaveDataToDB();

	pPlayer->UpdateFightingPower();

	CGuildWarMgr::Instance()->OnLeave(pGuild, pPlayer->GetRoleId());
}

// 任命
void CGuildManager::OnAppoint(Player *pPlayer, DWORD dwTargetId, BYTE byPos)
{
	WORD wFromMsgId = C2S_GUILD_APPOINT_REQ;

	if (pPlayer->GetCharID() == dwTargetId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]不能对自己进行任命操作!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能任命!", __FUNCTION__);
		return;
	}

	const GuildMember *pSrcMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pSrcMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能任命!", __FUNCTION__);
		return;
	}

	const GuildMember *pTargetMember = pGuild->GetMemberData(dwTargetId);
	if (!pTargetMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]目标没在本工会,不能任命!", __FUNCTION__);
		return;
	}

	WORD wEventId;

	// 转移会长
	if (GUILD_POS_CAPTAIN == byPos)
	{
		// 权限检测
		if (GUILD_POS_CAPTAIN != pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]没有权限转移会长", __FUNCTION__);
			return;
		}

		// 检测副本会长数量
// 		if (pGuild->GetMemberNum(GUILD_POS_ASSIST) >= 2)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]副会长数量已达上限,不能再任命", __FUNCTION__);
// 			return;
// 		}

		pGuild->SetMemberPos(pSrcMember->dwRoleId, GUILD_POS_MEMBER);
		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_CAPTAIN);

		wEventId = 7;
	}
	// 任命副会长
	else if (GUILD_POS_ASSIST == byPos)
	{
		// 权限检测
		if (GUILD_POS_CAPTAIN != pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]没有权限任命目标为副本会长", __FUNCTION__);
			return;
		}

		// 副会长人数已满
		if (pGuild->GetMemberNumByPos(byPos) >= 2)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]副本会长人数已满!", __FUNCTION__);
			return;
		}

		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_ASSIST);

		wEventId = 8;
	}
	// 罢免
	else
	{
		// 权限检测
		if (GUILD_POS_MEMBER == pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]没有权限罢免目标", __FUNCTION__);
			return;
		}

		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_MEMBER);

		wEventId = 9;
	}

	// 主动同步数据上给目标
	pGuild->SendGuildData(dwTargetId);

	MSG_S2C_GUILD_APPOINT_RESP msg;
	msg.m_byPos = byPos;
	msg.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&msg);

	// 添加事件记录
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
		pGuild->AddEvent(wEventId, pBaseData->szRoleName);

	pGuild->SaveDataToDB();
}

// 修改工会信息(修改公告,修改宣言,修改图标)
void CGuildManager::OnModifyGuildInfo(Player *pPlayer, BYTE byType, const char *pszText, WORD wIconId)
{
	WORD wFromMsgId = C2S_GUILD_INFO_MODIFY_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能修改工会信息![name:%s]", 
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	// 宣言
	if (byType == GUILD_PRO_DECLARATION) 
	{
		// todo::敏感字过滤

		if (strlen(pszText) >= MAX_GUILD_DECLARATION_LEN)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]修改公会公告内容过长![name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
			return;
		}

		pGuild->SetDeclaration(pszText);
	}
	// 公告
	else if (byType == GUILD_PRO_NOTICE)
	{
		// todo::敏感字过滤

		if (strlen(pszText) >= MAX_GUILD_NOTICE_LEN)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]修改公会公告内容过长![name:%s]",__FUNCTION__, pPlayer->GetUTF8RoleName());
			return;
		}

		pGuild->SetNotice(pszText);
	}
	// 图标
	else if (byType == GUILD_PRO_ICON)
	{
		pGuild->SetGuildIconId(wIconId);
	}
	else
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的工会属性类型[type:%d]", __FUNCTION__, byType);
		return;
	}

	MSG_S2C_GUILD_INFO_MODIFY_RESP msg;
	msg.m_byType = byType;
	msg.m_wIconId = wIconId;
	strcpy_s(msg.m_szText, sizeof(msg.m_szText), pszText);
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// 留言
void CGuildManager::OnLeaveMessage(Player *pPlayer, const char *pszMessage)
{
	WORD wFromMsgId = C2S_GUILD_LEAVE_MESSAGE_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能留言![name:%s]",
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能留言![name:%s]",
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	// todo::留言内容敏感字过滤
	int nMessageLen = strlen(pszMessage);
	if (nMessageLen >= MAX_GUILD_LEAVEMESSAGE_LEN)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]工会留言过长![name:%s]",__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	GuildLeaveMessage sLeaveMsg;
	sLeaveMsg.dwTime = DWORD(time(NULL));
	sLeaveMsg.byPos = pMember->byPosition;
	strcpy_s(sLeaveMsg.szName, sizeof(sLeaveMsg.szName), pPlayer->GetRoleName());
	strcpy_s(sLeaveMsg.szText, sizeof(sLeaveMsg.szText), pszMessage);
	pGuild->AddLeaveMessage(sLeaveMsg);

	MSG_S2C_GUILD_LEAVE_MESSAGE_RESP msg;
	memcpy_s(&msg.m_LeaveMsg, sizeof(GuildLeaveMessage), &sLeaveMsg, sizeof(GuildLeaveMessage));
	pPlayer->SendPacket(&msg);
}

// 搜索工会
void CGuildManager::OnSearch(Player *pPlayer, const char *pszGuildName, DWORD dwGuildId)
{
	MSG_S2C_GUILD_SEARCH_RESP msg;
	msg.m_wGuildNum = 0;

	// ID查找
	if (dwGuildId)
	{
		CGuild *pGuild = GetGuildById(dwGuildId);
		if (pGuild)
		{
			GuildBaseData &tBaseData = msg.m_GuildList[msg.m_wGuildNum++];
			memset(&tBaseData, 0, sizeof(tBaseData));
			pGuild->FillGuildData(tBaseData);
			tBaseData.byIsApply = pGuild->IsInApplyList(pPlayer->GetCharID());
			tBaseData.byIsFull = pGuild->IsMemberFull();
		}
	}
	// 名称查找
	else
	{
		GUILD_MAP *pGuildList = GetGuildList(pPlayer->GetServerId());
		if (pGuildList)
		{
			string strGuildName;
			GUILD_MAP::iterator iter = pGuildList->begin();
			for (; iter != pGuildList->end(); ++iter)
			{
				if (msg.m_wGuildNum >= MAX_SEARCH_GUILD_NUM)
					break;

				strGuildName = iter->second->GetGuildName();
				int nResult = strGuildName.find(pszGuildName);
				if (nResult >= 0)
				{
					GuildBaseData &tBaseData = msg.m_GuildList[msg.m_wGuildNum++];
					memset(&tBaseData, 0, sizeof(tBaseData));
					iter->second->FillGuildData(tBaseData);
					tBaseData.byIsApply = iter->second->IsInApplyList(pPlayer->GetCharID());
					tBaseData.byIsFull = iter->second->IsMemberFull();
				}
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// 冰泉
void CGuildManager::OnNetQY(Player *pPlayer, BYTE byType)
{
	WORD wFromMsgId = C2S_GUILD_QY_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能冰泉!",__FUNCTION__);
		return;
	}

	const GuildQY_Config* pQuanYongCfg = CConfigManager::Instance()->GetGuidQY(byType);
	if (!pQuanYongCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到冰泉配置数据![type:%d]",__FUNCTION__, byType);
		return;
	}

	// 检测VIP等级
	if (pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pQuanYongCfg->wNeedVipLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]VIP等级不足,不能冰泉![type:%d]", __FUNCTION__, byType);
		return;
	}

	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(pGuild->GetGuildLevel());
	if (!pGuildCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会配置![level:%d]", __FUNCTION__, pGuild->GetGuildLevel());
		return;
	}
	
	// 检测泉涌总次数
	if (pGuild->GetGuildQYTimes() >= pGuildCfg->wMaxMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]工会今日冰泉总次数已达上限!", __FUNCTION__);
		return;
	}

	// 检测泉涌个人次数
	if (pGuild->GetMemberQYType(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]今日冰泉已达过!", __FUNCTION__);
		return;
	}

	// 检测消耗
	for (size_t i = 0; i < pQuanYongCfg->vCostList.size(); ++i)
	{
		const CostData &tCost = pQuanYongCfg->vCostList[i];
		if (pPlayer->GetProperty(tCost.wType) < tCost.nValue)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]工会冰泉消耗不足![type:%d,value:%d]", __FUNCTION__, tCost.wType, tCost.nValue);
			return;
		}
	}

	DWORD dwAddExp;
	DWORD dwAddConstribution;

	// 检测是否暴击
	if (pQuanYongCfg->wCritRate > Rand::Instance()->irand(0, RAND_RANGE_MAX_10000))
	{
		dwAddExp = pQuanYongCfg->dwCritExp;
		dwAddConstribution = pQuanYongCfg->dwCritContribution;
	}
	else
	{
		dwAddExp = pQuanYongCfg->dwExp;
		dwAddConstribution = pQuanYongCfg->dwContribution;
	}

	// 获得工会经验暴击 
	pGuild->AddExp(dwAddExp);

	// 获得贡献值暴击(工会)
	pGuild->AddContribution(pPlayer->GetCharID(), dwAddConstribution);

	// 获得贡献值暴击(个人)
	pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, dwAddConstribution);

	pGuild->AddQSValue(pQuanYongCfg->dwQuanShui);

	pGuild->SetMemberQYType(pPlayer->GetCharID(), byType);

	pGuild->AddGuildQYTimes(1);

	// 处理消耗s
	for (size_t i = 0; i < pQuanYongCfg->vCostList.size(); ++i)
	{
		const CostData &tCost = pQuanYongCfg->vCostList[i];
		pPlayer->ModifyProperty(tCost.wType, -tCost.nValue, TRUE, FROM_GUILD_QY);
	}

	// 取事件ID
	WORD wEventId;
	if (1 == byType)
		wEventId = 4;
	else if (2 == byType)
		wEventId = 5;
	else
		wEventId = 6;

	// 添加事件记录
	pGuild->AddEvent(wEventId, pPlayer->GetRoleName(), dwAddExp);

	pGuild->SetLastQSInfo(pPlayer->GetRoleId(), dwAddExp, byType);

	pGuild->SendGuildData(pPlayer);

	MSG_S2C_GUILD_QY_RESP msg;
	msg.m_byType = byType;
	msg.m_dwExp = dwAddExp;
	msg.m_dwContribution = dwAddConstribution;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// 领取泉水奖励
void CGuildManager::OnNetQSAward(Player *pPlayer, WORD wQuanShui)
{
	WORD wFromMsgId = C2S_GUILD_QS_AWARD_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能冰泉!",__FUNCTION__);
		return;
	}

	const GuildQSAward_Config* pBQCfg = CConfigManager::Instance()->GetGuildQSAward(wQuanShui, pGuild->GetGuildLevel());
	if (!pBQCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到冰泉奖励配置数据![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// 检测当前阶段奖励能否领取
	if (wQuanShui > pGuild->GetQuanShui())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]当前阶段的泉水奖励还不能领取![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// 检测是否已领取
	if (pGuild->IsGetBQReward(pPlayer->GetCharID(), wQuanShui))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]已领取了泉水奖励![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// 添加奖励物品
	if (!pPlayer->AddObjectList(pBQCfg->vItemList, TRUE, FROM_GUILD_QS_REWARD))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]添加冰泉奖励物品失败!",__FUNCTION__);
		return;
	}

	// 添加奖励记录
	pGuild->AddQSRewardRecord(pPlayer->GetRoleId(), wQuanShui);

	pGuild->SendGuildData(pPlayer);

	MSG_S2C_GUILD_QS_AWARD_RESP msg;
	msg.m_wQuanShui = wQuanShui;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// 技能升级
void CGuildManager::OnNetSkillLevelUp(Player *pPlayer, BYTE byType, WORD wSkillId, WORD wSkillLevel)
{
	WORD wFromMsgId = C2S_GUILD_SKILL_LEVEL_UP_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有在工会里不能操作工会技能!",__FUNCTION__);
		return;
	}

	const GuildSkill_Config *pSkillCfg = g_pCfgMgr->GetGuildSkill(wSkillId, wSkillLevel);
	if (!pSkillCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会技能配置[SkillId:%d,SkillLevel:%d]!", __FUNCTION__, wSkillId, wSkillLevel);
		return;
	}

	// 技能提升(工会)
	if (1 == byType)
	{
		// 检测工会等级
		if (pSkillCfg->wNeedGuildLevel > pGuild->GetGuildLevel())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]工会等级不足,不能提升技能!", __FUNCTION__);
			return;
		}

		// 检测工会经验
		if (pSkillCfg->nCostGuildExp > pGuild->GetGuildData().dwExp)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]工会经验不足,不能提升技能!", __FUNCTION__);
			return;
		}

		// 检测提升的等级是否合法
		if (wSkillLevel > 1)
		{
			const SkillData *pSkill = pGuild->GetSkillData(wSkillId);
			if (!pSkill)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]还木有获得此技能不能提升!", __FUNCTION__);
				return;
			}

			if (pSkill->wLevel + 1 != wSkillLevel)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]提升的技能等级不对!", __FUNCTION__);
				return;
			}
		}

		// 扣除提升消耗经验
		pGuild->SetExp(pGuild->GetGuildData().dwExp - pSkillCfg->nCostGuildExp);

		// 添加技能
		pGuild->AddSkillData(wSkillId, wSkillLevel);
	}
	// 技能升级(个人)
	else
	{
		// 检测消耗条件
		for (size_t i = 0; i < pSkillCfg->vCostList.size(); ++i)
		{
			const CostData *pCost = &pSkillCfg->vCostList[i];
			if (pPlayer->GetProperty(pCost->wType) < pCost->nValue)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]个人升级工会技能消耗条件不足[Type:%d,Value:%d]!",
					__FUNCTION__, pCost->wType, pCost->nValue);
				return;
			}
		}

		// 检测升级的等级是否合法
		if (wSkillLevel > 1)
		{
			const SkillData *pSkill = pPlayer->GetGuildSkill(wSkillId);
			if (!pSkill)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]还木有获得此技能不能升级!", __FUNCTION__);
				return;
			}

			if (pSkill->wLevel + 1 != wSkillLevel)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]升级的技能等级不对!", __FUNCTION__);
				return;
			}
		}

		for (size_t i = 0; i < pSkillCfg->vCostList.size(); ++i)
		{
			pPlayer->ModifyProperty(pSkillCfg->vCostList[i].wType, -pSkillCfg->vCostList[i].nValue, TRUE, FROM_GUILD_SKILL_LV_UP);
		}

		// 添加技能
		pPlayer->AddGuildSkill(wSkillId, wSkillLevel);

		pPlayer->UpdateFightingPower();
	}

	MSG_S2C_GUILD_SKILL_LEVEL_UP_RESP msg;
	msg.m_byType = byType;
	msg.m_wSkillId = wSkillId;
	msg.m_wSkillLevel = wSkillLevel;
	pPlayer->SendPacket(&msg);

	// 同步给所有的成员
	pGuild->SendSkillDataToAll();

	pGuild->SaveDataToDB();
}

// 请求工会技能列表
void CGuildManager::OnNetSkillList(Player *pPlayer)
{
	// 目前是没有工会，客户端也会请求, 因为红点表现需要
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (pGuild)
		pGuild->SendSkillList(pPlayer);
}

// 弹劾
void CGuildManager::OnNetImpeach(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_IMPEACH_REQ;

	// 检测消耗
	int nCostDiamond = g_GetConfigValue(212);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]弹劾消耗钻石不足!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	GuildMember *pNewCaptain = pGuild->FindMemberData(pPlayer->GetRoleId());
	GuildMember *pOldCaptain = pGuild->FindMemberData(pGuild->GetCaptainId());
	if (!pOldCaptain || !pNewCaptain)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家在工会的数据!", __FUNCTION__);
		return;
	}

	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), pOldCaptain->dwRoleId);
	if (!pBaseData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家基础数据[roleid:%d]!", __FUNCTION__, pOldCaptain->dwRoleId);
		return;
	}

	// 检测弹劾时间
	if (!pBaseData->dwOfflineTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]会长离线时间不足,不能弹劾!", __FUNCTION__);
		return;
	}
	DWORD dwPassTime = time(NULL) - pBaseData->dwOfflineTime;
	if (!dwPassTime || (dwPassTime < g_GetConfigValue(120)*60*60))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]会长离线时间不足,不能弹劾!", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_GUILD_IMPEACH);

	pNewCaptain->byPosition = GUILD_POS_CAPTAIN;
	pOldCaptain->byPosition = GUILD_POS_MEMBER;

	MSG_S2C_GUILD_IMPEACH_RESP msg;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();

	MSG_S2C_GUILD_IMPEACH_NOTICE notice;
	notice.m_dwNewCaptainId = pNewCaptain->dwRoleId;
	pGuild->SendMsgToAllMember(&notice);
}

// 请求工会怪物数据
void CGuildManager::OnNetMonsterData(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_MONSTER_DATA_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->SendMonsterData(pPlayer);
}

// 副本章节重置
void CGuildManager::OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_FB_CHAPTER_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetFBChapterReset(pPlayer, byIsHistoryRecord);
}

// 战斗开始
bool CGuildManager::OnBattleStart(Player *pPlayer, BYTE byCamp)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return false;
	}

	return pGuild->OnBattleStart(pPlayer, byCamp);
}

// 战斗结束
bool CGuildManager::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return false;
	}

	return pGuild->OnBattleOver(pPlayer, pMonsterList, byMonsterNum);
}

// 请求章节关卡奖励状态
void CGuildManager::OnNetMyMapRewardRecord(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_FB_CHAPTER_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetMyMapRewardRecord(pPlayer);
}

// 请求工会副本数据
void CGuildManager::OnNetGuildCopyData(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->SendCopyData(pPlayer);
}

// 领取副本章节奖励
void CGuildManager::OnNetCopyChapterReward(Player *pPlayer, WORD wChapter)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyChapterReward(pPlayer, wChapter);
}

// 领取副本关卡奖励
void CGuildManager::OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyMapReward(pPlayer, wChapter, byCamp, byBoxIdx);
}

// 请求关卡奖励记录
void CGuildManager::OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyMapRewardRecord(pPlayer, wChapter, byCamp);
}

// 请求成员副本战绩
void CGuildManager::OnNetCopyBattleRecord(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyBattleRecord(pPlayer);
}

// 购买副本挑战次数
void CGuildManager::OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家木有在工会里!", __FUNCTION__);
		return;
	}

	pGuild->OnNetBuyCopyBattleTimes(pPlayer, byBuyTimes);
}

// 取工会数据
CGuild* CGuildManager::GetGuildById(DWORD dwGuildId)
{
	if (!dwGuildId)
		return NULL;

	GUILD_MAP *pGuildList = GetGuildList(GetGuildServerId(dwGuildId));
	if (pGuildList)
	{
		GUILD_MAP::iterator iter = pGuildList->find(dwGuildId);
		if (iter != pGuildList->end())
			return iter->second;
	}
	return NULL;
}

// 通过工会名称取工会数据
CGuild* CGuildManager::GetGuildByName(const char *pszGuildName)
{
	GUILD_NAME_MAP::iterator iter = m_NameList.find(pszGuildName);
	if (iter != m_NameList.end())
	{
		return GetGuildById(iter->second);
	}
	return NULL;
}

// 工会名称是否已存在
bool CGuildManager::IsGuildNameExist(const char *pszGuildName)
{
	GUILD_NAME_MAP::iterator iter = m_NameList.find(pszGuildName);
	return iter != m_NameList.end() ? true : false;
}

// 通过角色ID取工会数据
CGuild* CGuildManager::GetGuildByRoleId(DWORD dwRoleId)
{
	DWORD dwGuildId = GetGuildIdByRoleId(dwRoleId);
	return GetGuildById(dwGuildId);
}

// 取玩家所在工会的ID(0=表示木有工会)
DWORD CGuildManager::GetGuildIdByRoleId(DWORD dwRoleId)
{
	GUILD_FIND_MAP::iterator iter = m_FindList.find(dwRoleId);
	return iter != m_FindList.end() ? iter->second : 0;
}

// 删除成员
void CGuildManager::RemoveMember(DWORD dwRoleId)
{
	// 从查找列表拿掉
	RemoveFindData(dwRoleId);
}

// 删除工会
void CGuildManager::RemoveGuild(DWORD dwGuildId, const char *pszGuildName)
{
	m_NameList.erase(pszGuildName);

	GUILD_MAP *pGuildList = GetGuildList(GetGuildServerId(dwGuildId));
	if (pGuildList)
	{
		pGuildList->erase(dwGuildId);
	}
}

// 添加申请数据
void CGuildManager::AddApplyData(DWORD dwRoleId, DWORD dwGuildId)
{
	GUILD_APPLY_MAP::iterator iter = m_ApplyList.find(dwRoleId);
	if (iter != m_ApplyList.end())
	{
		iter->second.push_back(dwGuildId);
	}
	else
	{
		vector<DWORD> list;
		list.push_back(dwGuildId);
		m_ApplyList.insert(make_pair(dwRoleId, list));
	}
}

// 添加查找数据
void CGuildManager::AddFindData(DWORD dwRoleId, DWORD dwGuildId)
{
	m_FindList.insert(std::make_pair(dwRoleId, dwGuildId));
}

// 删除查找数据
void CGuildManager::RemoveFindData(DWORD dwRoleId)
{
	m_FindList.erase(dwRoleId);
}

// 删除已申请的工会ID
void CGuildManager::RemoveApplyGuildId(DWORD dwRoleId, DWORD dwGuildId)
{
	GUILD_APPLY_MAP::iterator iter = m_ApplyList.find(dwRoleId);
	if (iter == m_ApplyList.end())
		return;

	vector<DWORD>::iterator id_iter = iter->second.begin();
	for (; id_iter != iter->second.end(); ++id_iter)
	{
		if (*id_iter == dwGuildId)
		{
			iter->second.erase(id_iter);
			return;
		}
	}
}

// 删除所有已申请的工会信息
void CGuildManager::RemoveAllApplyData(DWORD dwRoleId)
{
	GUILD_APPLY_MAP::iterator iter = m_ApplyList.find(dwRoleId);
	if (iter == m_ApplyList.end())
		return;

	// 从玩家已经申请过的工会里删除玩家的申请信息
	vector<DWORD>::iterator id_iter = iter->second.begin();
	for (; id_iter != iter->second.end(); ++id_iter)
	{
		CGuild *pGuild = GetGuildById(*id_iter);
		if (pGuild)
		{
			pGuild->RemoveApplyData(dwRoleId);
			pGuild->SaveDataToDB();
		}
	}

	// 清空玩家的申请列表
	m_ApplyList.erase(iter);
}

// 添加工会
void CGuildManager::AddGuild(DWORD dwServerId, CGuild *pGuild)
{
	GUILD_MAP *pGuildList = GetGuildList(dwServerId);
	if (pGuildList)
	{
		pGuildList->insert(make_pair(pGuild->GetGuildId(), pGuild));
	}
	else
	{
		GUILD_MAP GuildList;
		GuildList.insert(make_pair(pGuild->GetGuildId(), pGuild));
		m_AllGuildList.insert(make_pair(dwServerId, GuildList));
	}
}

// 取一个可用的工会ID
DWORD CGuildManager::GetFreeGuildId(DWORD dwServerId)
{
	if (m_dwSeriesId + 1 >= 1000000)
		return 0;

	++m_dwSeriesId;

	return dwServerId * 1000000 + m_dwSeriesId;
}

// 刷新限时商品 
void CGuildManager::RefreshLimitGoodsEx()
{
// 	size_t nRefreshNum = 4; // todo::暂时这么处理,日后调整为通用配置
// 	m_LimitGoodsList.clear();
// 	vector<RateDataEx> tGoodsList = CConfigManager::Instance()->GetGuildLimitGoodsList();
// 	if (tGoodsList.size() < nRefreshNum)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]限时商品配置数量少于(%d)个!", __FUNCTION__, nRefreshNum);
// 		return;
// 	}
// 
// 	for (size_t i = 0; i < nRefreshNum; ++i)
// 	{
// 		// 计算总概率
// 		int nTotalRate = 0;
// 		for (vector<RateDataEx>::iterator iter = tGoodsList.begin(); iter != tGoodsList.end(); ++iter)
// 		{
// 			nTotalRate += iter->nRate;
// 		}
// 
// 		// 随机
// 		int nRandRate = Rand::Instance()->irand(1, nTotalRate);
// 		int nTempRate = 0;
// 		for (vector<RateDataEx>::iterator iter = tGoodsList.begin(); iter != tGoodsList.end(); ++iter)
// 		{
// 			nTempRate += iter->nRate;
// 			if (nRandRate <= nTempRate)
// 			{
// 				Goods tGoods;
// 				tGoods.wId = iter->nValue;
// 				tGoods.wNum = 0;
// 				m_LimitGoodsList.insert(make_pair(tGoods.wId, tGoods));
// 
// 				// 被随中的则拿掉
// 				tGoodsList.erase(iter);
// 				break;
// 			}
// 		}
// 	}
// 
// 	// 更新各个工会的限时商品列表
// 	ALL_GUILD_MAP::iterator all_iter = m_AllGuildList.begin();
// 	for (; all_iter != m_AllGuildList.end(); ++all_iter)
// 	{
// 		GUILD_MAP::iterator guild_iter = all_iter->second.begin();
// 		for (; guild_iter != all_iter->second.end(); ++guild_iter)
// 		{
// 			guild_iter->second->UpdateLimitGoods(m_LimitGoodsList);
// 		}
// 	}
}

// 刷新限时商品 
void CGuildManager::RefreshLimitGoods()
{
	WORD wShopType = SHOP_GUILD_LIMIT;
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(wShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品槽位列表配置为空[shop_type:%d]", __FUNCTION__, wShopType);
		return;
	}

	// 重置商店数据
	m_vLimitGoodsList.clear();

	vector<const ShopGoodsData_Config*> vRandList;
	for (size_t i = 0; i < pSlotListCfg->size(); ++i)
	{
		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		vRandList.clear();
		// 取筛选随机列表商品
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// 检测总概率及随机列表是否合法
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品筛随机列表为空![TotalRate:%d,ListSize:%d]",
				__FUNCTION__, wTotalRate, vRandList.size());
			return;
		}

		// 随机商品
		int nRandValue = g_Rand(1, wTotalRate); 
		int nCurRate = 0;
		for (size_t j = 0; j < vRandList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = vRandList[j];
			nCurRate += pGoodsCfg->wRefreshRate;
			if (nRandValue <= nCurRate)
			{
				//AddGoodsBuyRecord(byShopType, pGoodsCfg->wGoodsId, 0);
				Goods goods;
				goods.wId = pGoodsCfg->wGoodsId;
				goods.wNum = 0;
				m_vLimitGoodsList.push_back(goods);
				break;
			}
		}
	}

	// 更新各个工会的限时商品列表
	ALL_GUILD_MAP::iterator all_iter = m_AllGuildList.begin();
	for (; all_iter != m_AllGuildList.end(); ++all_iter)
	{
		GUILD_MAP::iterator guild_iter = all_iter->second.begin();
		for (; guild_iter != all_iter->second.end(); ++guild_iter)
		{
			guild_iter->second->UpdateLimitGoods(m_vLimitGoodsList);
		}
	}

// 	MAPLOG(ERROR_LEVEL, "[%s]创建工会限时刷新商品![GoodsNum:%d]",
// 		__FUNCTION__, m_vLimitGoodsList.size());
}

// 合服处理
void CGuildManager::OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId)
{
	GUILD_MAP *pGuildList = GetGuildList(dwFromServerId);
	if (pGuildList)
	{
		GUILD_MAP::iterator iter = pGuildList->begin();
		for (; iter != pGuildList->end(); ++iter)
		{
			// 更新服务器当前所属服务器
			iter->second->SetServerId(dwToServerId);
			iter->second->SaveDataToDB();
		}
	}
}

// 取服务器列表
GUILD_MAP* CGuildManager::GetGuildList(DWORD dwServerId)
{
	ALL_GUILD_MAP::iterator iter = m_AllGuildList.find(dwServerId);
	return iter != m_AllGuildList.end() ? &iter->second : NULL;
}