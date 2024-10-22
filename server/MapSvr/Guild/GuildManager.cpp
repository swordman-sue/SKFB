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
		MAPLOG(ERROR_LEVEL, "[%s]ľ�����ù�����ʱ�̵�ˢ�µ�ʱ���!", __FUNCTION__);
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

// �����������
void CGuildManager::LoadDataRequest()
{
	GuildData_Load* pQuery = GuildData_Load::ALLOC();
	pQuery->SetIndex(DB_GUILD_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	g_MapServer.MapDBQuery(pQuery);
}

// ��������
void CGuildManager::LoadDataFromDB(const char *pszData, int nSize)
{
	PB::GuildInfo PBGuildInfo;
	if (!PBGuildInfo.ParseFromArray(pszData, nSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ϵ�л��������ݳ���!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = new CGuild;

	// ������������
	pGuild->LoadDataFromDB(this,&PBGuildInfo);

	AddGuild(pGuild->GetServerId(), pGuild);
	pair<GUILD_NAME_MAP::iterator, bool> ret = m_NameList.insert(make_pair(pGuild->GetGuildName(), pGuild->GetGuildId()));
	if (!ret.second)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӹ�������ʧ��[guild_id:%d,guild_name:%s]!",
			__FUNCTION__, pGuild->GetGuildId(), pGuild->GetGuildName());
	}

	if (pGuild->GetGuildId())
		m_dwSeriesId = max(m_dwSeriesId, pGuild->GetGuildId() % 1000000);
}

// ȡ��������
int CGuildManager::GetGuildNum()
{
	return m_NameList.size();
}

// �Ƿ��к��
bool CGuildManager::IsHaveRedPoint(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	return pGuild ? pGuild->IsHaveRedPoint(pPlayer) : false;
}

// ���󹤻�����
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

// ���󹤻��б�
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

// ���󹤻��Ա�б�
void CGuildManager::OnGuildMemberList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendMemberList(pPlayer);
	}
}

// ���������б�
void CGuildManager::OnLeaveMsgList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendLeaveMsgList(pPlayer);

		// ����������״̬
		GuildMember *pMember = pGuild->FindMemberData(pPlayer->GetRoleId());
		if (pMember)
			pMember->bHaveNewLeaveMessage = false;
	}
}

// ���������б�
void CGuildManager::OnGuildApplyList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendApplyList(pPlayer);
	}
}

// ���󹤻��¼��б�
void CGuildManager::OnGuildEventList(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (pGuild)
	{
		pGuild->SendEventList(pPlayer);
	}
}

// ��������
void CGuildManager::OnCreate(Player *pPlayer, WORD wIconId, const char *pszName)
{
	WORD wFromMsgId = C2S_GUILD_CREATE_REQ;

	if (!pszName || !pszName[0])
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����������Ʋ�Ϊ��!", __FUNCTION__);
		return;
	}

	// �����ʯ�Ƿ��㹻
	int nCostDiamond = g_GetConfigValue(CONFIG_GUILD_CREATE_COST_GOLD);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��Ǯ����,��������ʧ��!", __FUNCTION__);
		return;
	}

	// VIP�ȼ�
	if (pPlayer->GetVipValue(VIP_CREATE_GUILD_FLAG) <= 0)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ȼ�����,��������ʧ��!", __FUNCTION__);
		return;
	}

	// �����Ҫ�ĳ�ֵ���
	DWORD dwNeedRechargeAmount = g_GetConfigValue(310);
	if (dwNeedRechargeAmount && pPlayer->GetProperty(ROLE_PRO_RECHARGE_AMOUNT) < dwNeedRechargeAmount)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ֵ��Ȳ���,��������ʧ��!", __FUNCTION__);
		return;
	}

	// todo::�����ֹ���

	// ��������Ƿ��ѱ�ʹ��
	if (IsGuildNameExist(pszName))
	{
		pPlayer->SendErrorMsg(ERROR_GUILD_NAME_EXIST);
		MAPLOG(ERROR_LEVEL, "[%s]���������ѱ�ʹ��,��������ʧ��![%s]", __FUNCTION__, pszName);
		return;
	}

	// ����Ƿ��й���
	if (GetGuildIdByRoleId(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_GUILD_ALREADY_IN_GUILD);
		MAPLOG(ERROR_LEVEL, "[%s]������ڹ���,��������ʧ��", __FUNCTION__);
		return;
	}

	CGuild *pGuild = new CGuild;
	if (!pGuild->Init(pPlayer, this, wIconId, pszName))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���������ʼ��ʧ��!", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_GUILD_CREATE);

	AddFindData(pPlayer->GetCharID(), pGuild->GetGuildId());

	AddGuild(pPlayer->GetServerId(), pGuild);

	pGuild->SendGuildData(pPlayer);

	m_NameList.insert(make_pair(pGuild->GetGuildName(), pGuild->GetGuildId()));

	// todo::��ʱ���������պ����
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, pGuild->GetGuildId());

	// ����Ǹ�������ʱ��
	if (pGuild->IsFBOpenTime())
	{
		pGuild->CreateMonster();
	}

	// ������ʱ��Ʒ�б�
	pGuild->UpdateLimitGoods(m_vLimitGoodsList);

	MSG_S2C_GUILD_CREATE_RESP msg;
	msg.m_wIconId = wIconId;
	strcpy_s(msg.m_szGuildName, sizeof(msg.m_szGuildName), pszName);
	pPlayer->SendPacket(&msg);

	// �������а�
	CRankManager::Instance()->AddGuildRankData(pPlayer->GetServerId(), pGuild->GetGuildId(), GUILD_RANK_LEVEL, pGuild->GetGuildLevel());

	pGuild->SaveDataToDB();

	pPlayer->UpdateFightingPower();
}

// ��ɢ����
void CGuildManager::OnDismiss(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_DISMISS_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]û���ڹ���,��ɢʧ��!", __FUNCTION__);
		return;
	}

	const GuildMember *pCaptainData = pGuild->GetCaptainData();
	if (!pCaptainData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��������᳤����!", __FUNCTION__);
		return;
	}

	// ����Ƿ���Ȩ��
	if (pCaptainData->dwRoleId != pPlayer->GetCharID())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ�޽�ɢ����!", __FUNCTION__);
		return;
	}

	// ����ָ��������Ҳ���ܽ�ɢ(Ŀǰ10��)
	if (pGuild->GetMemberNum() > BYTE(g_GetConfigValue(CONFIG_GUILD_DISMISS_MEMBER_NUM_LIMIT,10)))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����Ա����ָ�����������ܽ�ɢ!", __FUNCTION__);
		return;
	}

	// ����Ƿ񹤻�ս�ڼ� ��������ս�ڼ䣬�����ɢ����
	const GuildBuilding_Config *pBuildingCfg = g_pCfgMgr->GetGuildBuilding(6);
	if (CGuildWarMgr::Instance()->CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE) 
		&& pBuildingCfg 
		&& pGuild->GetGuildLevel() >= pBuildingCfg->wNeedGuildLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��������ս�ڼ䣬�����ɢ����!", __FUNCTION__);
		return;
	}

	DWORD dwServerId = pGuild->GetServerId();
	DWORD dwGuildId = pGuild->GetGuildId();

	pGuild->SaveDataToDB(true);

	pGuild->Dismiss();

	// �����а����õ�
	CRankManager::Instance()->RemoveGuildRandData(dwServerId, dwGuildId, GUILD_RANK_LEVEL);

	RemoveGuild(pGuild->GetGuildId(), pGuild->GetGuildName());

	// ֪ͨ�ͻ�����0
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, 0);

	pPlayer->UpdateFightingPower();

	MSG_S2C_GUILD_DISMISS_RESP msg;
	pPlayer->SendPacket(&msg);

	CGuildWarMgr::Instance()->OnDismissGuild(dwServerId, dwGuildId);
}

// ������� 
void CGuildManager::OnApply(Player *pPlayer, DWORD dwGuildId)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_REQ;

	// �������Ƿ����ڹ���
	if (GetGuildByRoleId(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������й���,����ʧ��!", __FUNCTION__);
		return;
	}

	// ����Ƿ����˳�����CD��
	DWORD dwLeaveTime = pPlayer->GetProperty(ROLE_PRO_LEAVE_GUILD_TIME);
	if (dwLeaveTime)
	{
		DWORD dwPassTime = g_GetPassTime(dwLeaveTime, time(NULL));
		if (dwPassTime < g_Hour2Second(g_GetConfigValue(121)))
		{
			MAPLOG(GUI_LEVEL, "[%s]�˳������,24Сʱ�ڲ�����������µĹ���!", __FUNCTION__);
			pPlayer->SendErrorMsg(ERROR_LEAVE_GUILD_CD);
			return;
		}
	}

	GUILD_APPLY_MAP::iterator apply_iter = m_ApplyList.find(pPlayer->GetCharID());
	// ���������������Ƿ��Ѵ�����
	if (apply_iter != m_ApplyList.end())
	{
		if (apply_iter->second.size() >= g_GetConfigValue(119,5))
		{
			pPlayer->SendErrorMsg(ERROR_GUILD_APPLY_NUM_LIMIT);
			// ��������Ѵ�����
			MAPLOG(ERROR_LEVEL, "[%s]������빤�������Ѵ�����!", __FUNCTION__);
			return;
		}

		// ����Ƿ��������
		vector<DWORD>::iterator iter = apply_iter->second.begin();
		for (; iter != apply_iter->second.end(); ++iter)
		{
			if (*iter == dwGuildId)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]���������б���!", __FUNCTION__);
				return;
			}
		}
	}

	CGuild *pGuild = GetGuildById(dwGuildId);

	// ���Ŀ�깤���Ƿ����
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�������Ĺ��᲻����![guildid:%d]", __FUNCTION__,dwGuildId);
		return;
	}

	// ���Ŀ�깤���Ƿ���������
	if (pGuild->IsMemberFull())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����Ա��������!",__FUNCTION__);
		return;
	}

	// ���������б�
	AddApplyData(pPlayer->GetCharID(), dwGuildId);

	pGuild->AddApplyData(pPlayer);

	MSG_S2C_GUILD_APPLY_RESP msg;
	msg.m_dwGuildId = dwGuildId;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// �������
void CGuildManager::OnApplyCheck(Player *pPlayer, DWORD dwTargetId, BYTE byIsAccept)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_CHECK_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ľ�й���,���ܽ����������!",__FUNCTION__);
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ڹ���ĳ�Ա����!", __FUNCTION__);
		return;
	}

	// todo::����Ƿ������Ȩ��
	if (GUILD_POS_MEMBER == pMember->byPosition)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ�޴���������������!!", __FUNCTION__);
		return;
	}

	if (byIsAccept)
	{
		// ����Ƿ��������
		//GuildApplyData *pApplyData = pGuild->IsInApplyList(dwRoleId);
		if (!pGuild->IsInApplyList(dwTargetId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]Ŀ��û����������빤���б���[RoleId:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// ��⹤�������Ƿ�����
		if (pGuild->IsMemberFull())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]������������", __FUNCTION__);
			return;
		}

		// ���Ŀ���Ƿ����ڹ���
		if (GetGuildByRoleId(dwTargetId))
		{
			pPlayer->SendErrorMsg(ERROR_GUILD_TARGET_HAVE_GUILD);
			pGuild->RemoveApplyData(dwTargetId);
			pGuild->SendApplyList(pPlayer); // ����ͬ������
			MAPLOG(ERROR_LEVEL, "[%s]Ŀ���Ѽ����Ĺ���", __FUNCTION__);
			return;
		}

		// ��ӳ�Ա����
		GuildMember member;
		member.Init();
		member.dwRoleId = dwTargetId;
		member.byPosition = GUILD_POS_MEMBER;
		member.dwLastBattleTimesRecoverTime = time(NULL);
		pGuild->AddMember(&member);

		pGuild->SendMemberList(pPlayer);

		// ��������б���
		AddFindData(dwTargetId, pGuild->GetGuildId());

		// ȥ���������������Ϣ
		RemoveAllApplyData(dwTargetId);

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, false);
		if (pBaseData)
		{
			pGuild->AddEvent(1, pBaseData->szRoleName);
		}

		// ͬ��Ŀ��
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pTarget)                                      
			pTarget->NotifyProperty(ROLE_PRO_GUILD_ID, pGuild->GetGuildId());
	}
	else
	{
		pGuild->RemoveApplyData(dwTargetId);
		RemoveApplyGuildId(dwTargetId, pGuild->GetGuildId());
	}

	// ��Ӧ
	MSG_S2C_GUILD_APPLY_CHECK_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_byIsAccept = byIsAccept;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}


// ȡ������  
void CGuildManager::OnApplyCancel(Player *pPlayer, DWORD dwGuildId)
{
	WORD wFromMsgId = C2S_GUILD_APPLY_CANCEL_REQ;

	CGuild *pGuild = GetGuildById(dwGuildId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ȡ������Ĺ���![GuildId:%s]", __FUNCTION__, dwGuildId);
		return;
	}

	pGuild->RemoveApplyData(pPlayer->GetCharID());
	RemoveApplyGuildId(pPlayer->GetCharID(), dwGuildId);

	MSG_S2C_GUILD_APPLY_CANCEL_RESP msg;
	msg.m_dwGuildId = dwGuildId;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// �߳�����
void CGuildManager::OnKickOut(Player *pPlayer, DWORD dwTargetId)
{
	WORD wFromMsgId = C2S_GUILD_KICKOUT_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ��᲻������", __FUNCTION__);
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�߼��ϳ������ش���,�Ҳ�������ڹ��������!",__FUNCTION__);
		return;
	}

	const GuildMember *pTargetMember = pGuild->GetMemberData(dwTargetId);
	if (!pTargetMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ��ľ���ڹ������޷��ߵ�!",__FUNCTION__);
		return;
	}

	// ����߳����Ǹ��᳤
	if (GUILD_POS_ASSIST == pTargetMember->byPosition)
	{
		// ����Ƿ���Ȩ������
		if (GUILD_POS_CAPTAIN != pMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ���ߵ������Ա!", __FUNCTION__);
			return;
		}
	}
	// ����߳�������ͨ��Ա
	else if (GUILD_POS_MEMBER == pTargetMember->byPosition)
	{
		// ����Ƿ���Ȩ������
		if (GUILD_POS_MEMBER == pMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ���ߵ������Ա!", __FUNCTION__);
			return;
		}
	}
	else
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ܳ��߻᳤!", __FUNCTION__);
		return;
	}

	pGuild->RemoveMember(dwTargetId);
	RemoveFindData(dwTargetId);

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	// ������ֱ������
	if (pTarget)
	{
		// ��¼�뿪����ʱ��
		pTarget->ModifyProperty(ROLE_PRO_LEAVE_GUILD_TIME, time(NULL));
		// ����ս��
		pTarget->UpdateFightingPower();
	}
	// û����
	else
	{
		// �������ݿ�
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

	// �¼���¼
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
		pGuild->AddEvent(3, pBaseData->szRoleName);

	pGuild->SaveDataToDB();

	CGuildWarMgr::Instance()->OnLeave(pGuild, dwTargetId);
}

//�˳�����
void CGuildManager::OnLeave(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_LEAVE_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,�����˳�![name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�߼��ϳ������ش���,�Ҳ�������ڹ��������!", __FUNCTION__);
		return;
	}

	// ����Ƿ����ֱ���˳�
	if (GUILD_POS_CAPTAIN == pMember->byPosition)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����᳤����ֱ���˳�����!", __FUNCTION__);
		return;
	}

	pGuild->RemoveMember(pPlayer->GetCharID());
	RemoveFindData(pPlayer->GetCharID());

	// ֪ͨ�ͻ�����0
	pPlayer->NotifyProperty(ROLE_PRO_GUILD_ID, 0);

	// ��¼�뿪����ʱ��
	pPlayer->ModifyProperty(ROLE_PRO_LEAVE_GUILD_TIME, time(NULL));

	MSG_S2C_GUILD_LEAVE_RESP msg;
	pPlayer->SendPacket(&msg);

	pGuild->AddEvent(2, pPlayer->GetRoleName());

	pGuild->SaveDataToDB();

	pPlayer->UpdateFightingPower();

	CGuildWarMgr::Instance()->OnLeave(pGuild, pPlayer->GetRoleId());
}

// ����
void CGuildManager::OnAppoint(Player *pPlayer, DWORD dwTargetId, BYTE byPos)
{
	WORD wFromMsgId = C2S_GUILD_APPOINT_REQ;

	if (pPlayer->GetCharID() == dwTargetId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ܶ��Լ�������������!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,��������!", __FUNCTION__);
		return;
	}

	const GuildMember *pSrcMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pSrcMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,��������!", __FUNCTION__);
		return;
	}

	const GuildMember *pTargetMember = pGuild->GetMemberData(dwTargetId);
	if (!pTargetMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ��û�ڱ�����,��������!", __FUNCTION__);
		return;
	}

	WORD wEventId;

	// ת�ƻ᳤
	if (GUILD_POS_CAPTAIN == byPos)
	{
		// Ȩ�޼��
		if (GUILD_POS_CAPTAIN != pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ��ת�ƻ᳤", __FUNCTION__);
			return;
		}

		// ��⸱���᳤����
// 		if (pGuild->GetMemberNum(GUILD_POS_ASSIST) >= 2)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]���᳤�����Ѵ�����,����������", __FUNCTION__);
// 			return;
// 		}

		pGuild->SetMemberPos(pSrcMember->dwRoleId, GUILD_POS_MEMBER);
		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_CAPTAIN);

		wEventId = 7;
	}
	// �������᳤
	else if (GUILD_POS_ASSIST == byPos)
	{
		// Ȩ�޼��
		if (GUILD_POS_CAPTAIN != pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ������Ŀ��Ϊ�����᳤", __FUNCTION__);
			return;
		}

		// ���᳤��������
		if (pGuild->GetMemberNumByPos(byPos) >= 2)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�����᳤��������!", __FUNCTION__);
			return;
		}

		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_ASSIST);

		wEventId = 8;
	}
	// ����
	else
	{
		// Ȩ�޼��
		if (GUILD_POS_MEMBER == pSrcMember->byPosition)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]û��Ȩ�ް���Ŀ��", __FUNCTION__);
			return;
		}

		pGuild->SetMemberPos(pTargetMember->dwRoleId, GUILD_POS_MEMBER);

		wEventId = 9;
	}

	// ����ͬ�������ϸ�Ŀ��
	pGuild->SendGuildData(dwTargetId);

	MSG_S2C_GUILD_APPOINT_RESP msg;
	msg.m_byPos = byPos;
	msg.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&msg);

	// ����¼���¼
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
		pGuild->AddEvent(wEventId, pBaseData->szRoleName);

	pGuild->SaveDataToDB();
}

// �޸Ĺ�����Ϣ(�޸Ĺ���,�޸�����,�޸�ͼ��)
void CGuildManager::OnModifyGuildInfo(Player *pPlayer, BYTE byType, const char *pszText, WORD wIconId)
{
	WORD wFromMsgId = C2S_GUILD_INFO_MODIFY_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,�����޸Ĺ�����Ϣ![name:%s]", 
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	// ����
	if (byType == GUILD_PRO_DECLARATION) 
	{
		// todo::�����ֹ���

		if (strlen(pszText) >= MAX_GUILD_DECLARATION_LEN)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�޸Ĺ��ṫ�����ݹ���![name:%s]", __FUNCTION__, pPlayer->GetUTF8RoleName());
			return;
		}

		pGuild->SetDeclaration(pszText);
	}
	// ����
	else if (byType == GUILD_PRO_NOTICE)
	{
		// todo::�����ֹ���

		if (strlen(pszText) >= MAX_GUILD_NOTICE_LEN)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�޸Ĺ��ṫ�����ݹ���![name:%s]",__FUNCTION__, pPlayer->GetUTF8RoleName());
			return;
		}

		pGuild->SetNotice(pszText);
	}
	// ͼ��
	else if (byType == GUILD_PRO_ICON)
	{
		pGuild->SetGuildIconId(wIconId);
	}
	else
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���Ĺ�����������[type:%d]", __FUNCTION__, byType);
		return;
	}

	MSG_S2C_GUILD_INFO_MODIFY_RESP msg;
	msg.m_byType = byType;
	msg.m_wIconId = wIconId;
	strcpy_s(msg.m_szText, sizeof(msg.m_szText), pszText);
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// ����
void CGuildManager::OnLeaveMessage(Player *pPlayer, const char *pszMessage)
{
	WORD wFromMsgId = C2S_GUILD_LEAVE_MESSAGE_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,��������![name:%s]",
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	const GuildMember *pMember = pGuild->GetMemberData(pPlayer->GetCharID());
	if (!pMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,��������![name:%s]",
			__FUNCTION__, pPlayer->GetUTF8RoleName());
		return;
	}

	// todo::�������������ֹ���
	int nMessageLen = strlen(pszMessage);
	if (nMessageLen >= MAX_GUILD_LEAVEMESSAGE_LEN)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�������Թ���![name:%s]",__FUNCTION__, pPlayer->GetUTF8RoleName());
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

// ��������
void CGuildManager::OnSearch(Player *pPlayer, const char *pszGuildName, DWORD dwGuildId)
{
	MSG_S2C_GUILD_SEARCH_RESP msg;
	msg.m_wGuildNum = 0;

	// ID����
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
	// ���Ʋ���
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

// ��Ȫ
void CGuildManager::OnNetQY(Player *pPlayer, BYTE byType)
{
	WORD wFromMsgId = C2S_GUILD_QY_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,���ܱ�Ȫ!",__FUNCTION__);
		return;
	}

	const GuildQY_Config* pQuanYongCfg = CConfigManager::Instance()->GetGuidQY(byType);
	if (!pQuanYongCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ȫ��������![type:%d]",__FUNCTION__, byType);
		return;
	}

	// ���VIP�ȼ�
	if (pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pQuanYongCfg->wNeedVipLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]VIP�ȼ�����,���ܱ�Ȫ![type:%d]", __FUNCTION__, byType);
		return;
	}

	const Guild_Config *pGuildCfg = CConfigManager::Instance()->GetGuild(pGuild->GetGuildLevel());
	if (!pGuildCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������![level:%d]", __FUNCTION__, pGuild->GetGuildLevel());
		return;
	}
	
	// ���Ȫӿ�ܴ���
	if (pGuild->GetGuildQYTimes() >= pGuildCfg->wMaxMember)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������ձ�Ȫ�ܴ����Ѵ�����!", __FUNCTION__);
		return;
	}

	// ���Ȫӿ���˴���
	if (pGuild->GetMemberQYType(pPlayer->GetCharID()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ձ�Ȫ�Ѵ��!", __FUNCTION__);
		return;
	}

	// �������
	for (size_t i = 0; i < pQuanYongCfg->vCostList.size(); ++i)
	{
		const CostData &tCost = pQuanYongCfg->vCostList[i];
		if (pPlayer->GetProperty(tCost.wType) < tCost.nValue)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�����Ȫ���Ĳ���![type:%d,value:%d]", __FUNCTION__, tCost.wType, tCost.nValue);
			return;
		}
	}

	DWORD dwAddExp;
	DWORD dwAddConstribution;

	// ����Ƿ񱩻�
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

	// ��ù��ᾭ�鱩�� 
	pGuild->AddExp(dwAddExp);

	// ��ù���ֵ����(����)
	pGuild->AddContribution(pPlayer->GetCharID(), dwAddConstribution);

	// ��ù���ֵ����(����)
	pPlayer->ModifyProperty(ROLE_PRO_GUILD_CONSTIBUTION, dwAddConstribution);

	pGuild->AddQSValue(pQuanYongCfg->dwQuanShui);

	pGuild->SetMemberQYType(pPlayer->GetCharID(), byType);

	pGuild->AddGuildQYTimes(1);

	// ��������s
	for (size_t i = 0; i < pQuanYongCfg->vCostList.size(); ++i)
	{
		const CostData &tCost = pQuanYongCfg->vCostList[i];
		pPlayer->ModifyProperty(tCost.wType, -tCost.nValue, TRUE, FROM_GUILD_QY);
	}

	// ȡ�¼�ID
	WORD wEventId;
	if (1 == byType)
		wEventId = 4;
	else if (2 == byType)
		wEventId = 5;
	else
		wEventId = 6;

	// ����¼���¼
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

// ��ȡȪˮ����
void CGuildManager::OnNetQSAward(Player *pPlayer, WORD wQuanShui)
{
	WORD wFromMsgId = C2S_GUILD_QS_AWARD_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetCharID());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,���ܱ�Ȫ!",__FUNCTION__);
		return;
	}

	const GuildQSAward_Config* pBQCfg = CConfigManager::Instance()->GetGuildQSAward(wQuanShui, pGuild->GetGuildLevel());
	if (!pBQCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ȫ������������![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// ��⵱ǰ�׶ν����ܷ���ȡ
	if (wQuanShui > pGuild->GetQuanShui())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ�׶ε�Ȫˮ������������ȡ![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// ����Ƿ�����ȡ
	if (pGuild->IsGetBQReward(pPlayer->GetCharID(), wQuanShui))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����ȡ��Ȫˮ����![QuanShui:%d]", __FUNCTION__, wQuanShui);
		return;
	}

	// ��ӽ�����Ʒ
	if (!pPlayer->AddObjectList(pBQCfg->vItemList, TRUE, FROM_GUILD_QS_REWARD))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ӱ�Ȫ������Ʒʧ��!",__FUNCTION__);
		return;
	}

	// ��ӽ�����¼
	pGuild->AddQSRewardRecord(pPlayer->GetRoleId(), wQuanShui);

	pGuild->SendGuildData(pPlayer);

	MSG_S2C_GUILD_QS_AWARD_RESP msg;
	msg.m_wQuanShui = wQuanShui;
	pPlayer->SendPacket(&msg);

	pGuild->SaveDataToDB();
}

// ��������
void CGuildManager::OnNetSkillLevelUp(Player *pPlayer, BYTE byType, WORD wSkillId, WORD wSkillLevel)
{
	WORD wFromMsgId = C2S_GUILD_SKILL_LEVEL_UP_REQ;

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ľ���ڹ����ﲻ�ܲ������Ἴ��!",__FUNCTION__);
		return;
	}

	const GuildSkill_Config *pSkillCfg = g_pCfgMgr->GetGuildSkill(wSkillId, wSkillLevel);
	if (!pSkillCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ἴ������[SkillId:%d,SkillLevel:%d]!", __FUNCTION__, wSkillId, wSkillLevel);
		return;
	}

	// ��������(����)
	if (1 == byType)
	{
		// ��⹤��ȼ�
		if (pSkillCfg->wNeedGuildLevel > pGuild->GetGuildLevel())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]����ȼ�����,������������!", __FUNCTION__);
			return;
		}

		// ��⹤�ᾭ��
		if (pSkillCfg->nCostGuildExp > pGuild->GetGuildData().dwExp)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]���ᾭ�鲻��,������������!", __FUNCTION__);
			return;
		}

		// ��������ĵȼ��Ƿ�Ϸ�
		if (wSkillLevel > 1)
		{
			const SkillData *pSkill = pGuild->GetSkillData(wSkillId);
			if (!pSkill)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]��ľ�л�ô˼��ܲ�������!", __FUNCTION__);
				return;
			}

			if (pSkill->wLevel + 1 != wSkillLevel)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�����ļ��ܵȼ�����!", __FUNCTION__);
				return;
			}
		}

		// �۳��������ľ���
		pGuild->SetExp(pGuild->GetGuildData().dwExp - pSkillCfg->nCostGuildExp);

		// ��Ӽ���
		pGuild->AddSkillData(wSkillId, wSkillLevel);
	}
	// ��������(����)
	else
	{
		// �����������
		for (size_t i = 0; i < pSkillCfg->vCostList.size(); ++i)
		{
			const CostData *pCost = &pSkillCfg->vCostList[i];
			if (pPlayer->GetProperty(pCost->wType) < pCost->nValue)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�����������Ἴ��������������[Type:%d,Value:%d]!",
					__FUNCTION__, pCost->wType, pCost->nValue);
				return;
			}
		}

		// ��������ĵȼ��Ƿ�Ϸ�
		if (wSkillLevel > 1)
		{
			const SkillData *pSkill = pPlayer->GetGuildSkill(wSkillId);
			if (!pSkill)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]��ľ�л�ô˼��ܲ�������!", __FUNCTION__);
				return;
			}

			if (pSkill->wLevel + 1 != wSkillLevel)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�����ļ��ܵȼ�����!", __FUNCTION__);
				return;
			}
		}

		for (size_t i = 0; i < pSkillCfg->vCostList.size(); ++i)
		{
			pPlayer->ModifyProperty(pSkillCfg->vCostList[i].wType, -pSkillCfg->vCostList[i].nValue, TRUE, FROM_GUILD_SKILL_LV_UP);
		}

		// ��Ӽ���
		pPlayer->AddGuildSkill(wSkillId, wSkillLevel);

		pPlayer->UpdateFightingPower();
	}

	MSG_S2C_GUILD_SKILL_LEVEL_UP_RESP msg;
	msg.m_byType = byType;
	msg.m_wSkillId = wSkillId;
	msg.m_wSkillLevel = wSkillLevel;
	pPlayer->SendPacket(&msg);

	// ͬ�������еĳ�Ա
	pGuild->SendSkillDataToAll();

	pGuild->SaveDataToDB();
}

// ���󹤻Ἴ���б�
void CGuildManager::OnNetSkillList(Player *pPlayer)
{
	// Ŀǰ��û�й��ᣬ�ͻ���Ҳ������, ��Ϊ��������Ҫ
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (pGuild)
		pGuild->SendSkillList(pPlayer);
}

// ����
void CGuildManager::OnNetImpeach(Player *pPlayer)
{
	WORD wFromMsgId = C2S_GUILD_IMPEACH_REQ;

	// �������
	int nCostDiamond = g_GetConfigValue(212);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����������ʯ����!", __FUNCTION__);
		return;
	}

	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	GuildMember *pNewCaptain = pGuild->FindMemberData(pPlayer->GetRoleId());
	GuildMember *pOldCaptain = pGuild->FindMemberData(pGuild->GetCaptainId());
	if (!pOldCaptain || !pNewCaptain)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ڹ��������!", __FUNCTION__);
		return;
	}

	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), pOldCaptain->dwRoleId);
	if (!pBaseData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ�������[roleid:%d]!", __FUNCTION__, pOldCaptain->dwRoleId);
		return;
	}

	// ��ⵯ��ʱ��
	if (!pBaseData->dwOfflineTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�᳤����ʱ�䲻��,���ܵ���!", __FUNCTION__);
		return;
	}
	DWORD dwPassTime = time(NULL) - pBaseData->dwOfflineTime;
	if (!dwPassTime || (dwPassTime < g_GetConfigValue(120)*60*60))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�᳤����ʱ�䲻��,���ܵ���!", __FUNCTION__);
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

// ���󹤻��������
void CGuildManager::OnNetMonsterData(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_MONSTER_DATA_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->SendMonsterData(pPlayer);
}

// �����½�����
void CGuildManager::OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_FB_CHAPTER_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetFBChapterReset(pPlayer, byIsHistoryRecord);
}

// ս����ʼ
bool CGuildManager::OnBattleStart(Player *pPlayer, BYTE byCamp)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return false;
	}

	return pGuild->OnBattleStart(pPlayer, byCamp);
}

// ս������
bool CGuildManager::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return false;
	}

	return pGuild->OnBattleOver(pPlayer, pMonsterList, byMonsterNum);
}

// �����½ڹؿ�����״̬
void CGuildManager::OnNetMyMapRewardRecord(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUILD_FB_CHAPTER_RESET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetMyMapRewardRecord(pPlayer);
}

// ���󹤻ḱ������
void CGuildManager::OnNetGuildCopyData(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->SendCopyData(pPlayer);
}

// ��ȡ�����½ڽ���
void CGuildManager::OnNetCopyChapterReward(Player *pPlayer, WORD wChapter)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyChapterReward(pPlayer, wChapter);
}

// ��ȡ�����ؿ�����
void CGuildManager::OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyMapReward(pPlayer, wChapter, byCamp, byBoxIdx);
}

// ����ؿ�������¼
void CGuildManager::OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyMapRewardRecord(pPlayer, wChapter, byCamp);
}

// �����Ա����ս��
void CGuildManager::OnNetCopyBattleRecord(Player *pPlayer)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetCopyBattleRecord(pPlayer);
}

// ���򸱱���ս����
void CGuildManager::OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes)
{
	CGuild *pGuild = GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڹ�����!", __FUNCTION__);
		return;
	}

	pGuild->OnNetBuyCopyBattleTimes(pPlayer, byBuyTimes);
}

// ȡ��������
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

// ͨ����������ȡ��������
CGuild* CGuildManager::GetGuildByName(const char *pszGuildName)
{
	GUILD_NAME_MAP::iterator iter = m_NameList.find(pszGuildName);
	if (iter != m_NameList.end())
	{
		return GetGuildById(iter->second);
	}
	return NULL;
}

// ���������Ƿ��Ѵ���
bool CGuildManager::IsGuildNameExist(const char *pszGuildName)
{
	GUILD_NAME_MAP::iterator iter = m_NameList.find(pszGuildName);
	return iter != m_NameList.end() ? true : false;
}

// ͨ����ɫIDȡ��������
CGuild* CGuildManager::GetGuildByRoleId(DWORD dwRoleId)
{
	DWORD dwGuildId = GetGuildIdByRoleId(dwRoleId);
	return GetGuildById(dwGuildId);
}

// ȡ������ڹ����ID(0=��ʾľ�й���)
DWORD CGuildManager::GetGuildIdByRoleId(DWORD dwRoleId)
{
	GUILD_FIND_MAP::iterator iter = m_FindList.find(dwRoleId);
	return iter != m_FindList.end() ? iter->second : 0;
}

// ɾ����Ա
void CGuildManager::RemoveMember(DWORD dwRoleId)
{
	// �Ӳ����б��õ�
	RemoveFindData(dwRoleId);
}

// ɾ������
void CGuildManager::RemoveGuild(DWORD dwGuildId, const char *pszGuildName)
{
	m_NameList.erase(pszGuildName);

	GUILD_MAP *pGuildList = GetGuildList(GetGuildServerId(dwGuildId));
	if (pGuildList)
	{
		pGuildList->erase(dwGuildId);
	}
}

// �����������
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

// ��Ӳ�������
void CGuildManager::AddFindData(DWORD dwRoleId, DWORD dwGuildId)
{
	m_FindList.insert(std::make_pair(dwRoleId, dwGuildId));
}

// ɾ����������
void CGuildManager::RemoveFindData(DWORD dwRoleId)
{
	m_FindList.erase(dwRoleId);
}

// ɾ��������Ĺ���ID
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

// ɾ������������Ĺ�����Ϣ
void CGuildManager::RemoveAllApplyData(DWORD dwRoleId)
{
	GUILD_APPLY_MAP::iterator iter = m_ApplyList.find(dwRoleId);
	if (iter == m_ApplyList.end())
		return;

	// ������Ѿ�������Ĺ�����ɾ����ҵ�������Ϣ
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

	// �����ҵ������б�
	m_ApplyList.erase(iter);
}

// ��ӹ���
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

// ȡһ�����õĹ���ID
DWORD CGuildManager::GetFreeGuildId(DWORD dwServerId)
{
	if (m_dwSeriesId + 1 >= 1000000)
		return 0;

	++m_dwSeriesId;

	return dwServerId * 1000000 + m_dwSeriesId;
}

// ˢ����ʱ��Ʒ 
void CGuildManager::RefreshLimitGoodsEx()
{
// 	size_t nRefreshNum = 4; // todo::��ʱ��ô����,�պ����Ϊͨ������
// 	m_LimitGoodsList.clear();
// 	vector<RateDataEx> tGoodsList = CConfigManager::Instance()->GetGuildLimitGoodsList();
// 	if (tGoodsList.size() < nRefreshNum)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ʱ��Ʒ������������(%d)��!", __FUNCTION__, nRefreshNum);
// 		return;
// 	}
// 
// 	for (size_t i = 0; i < nRefreshNum; ++i)
// 	{
// 		// �����ܸ���
// 		int nTotalRate = 0;
// 		for (vector<RateDataEx>::iterator iter = tGoodsList.begin(); iter != tGoodsList.end(); ++iter)
// 		{
// 			nTotalRate += iter->nRate;
// 		}
// 
// 		// ���
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
// 				// �����е����õ�
// 				tGoodsList.erase(iter);
// 				break;
// 			}
// 		}
// 	}
// 
// 	// ���¸����������ʱ��Ʒ�б�
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

// ˢ����ʱ��Ʒ 
void CGuildManager::RefreshLimitGoods()
{
	WORD wShopType = SHOP_GUILD_LIMIT;
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(wShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒ��λ�б�����Ϊ��[shop_type:%d]", __FUNCTION__, wShopType);
		return;
	}

	// �����̵�����
	m_vLimitGoodsList.clear();

	vector<const ShopGoodsData_Config*> vRandList;
	for (size_t i = 0; i < pSlotListCfg->size(); ++i)
	{
		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		vRandList.clear();
		// ȡɸѡ����б���Ʒ
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// ����ܸ��ʼ�����б��Ƿ�Ϸ�
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒɸ����б�Ϊ��![TotalRate:%d,ListSize:%d]",
				__FUNCTION__, wTotalRate, vRandList.size());
			return;
		}

		// �����Ʒ
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

	// ���¸����������ʱ��Ʒ�б�
	ALL_GUILD_MAP::iterator all_iter = m_AllGuildList.begin();
	for (; all_iter != m_AllGuildList.end(); ++all_iter)
	{
		GUILD_MAP::iterator guild_iter = all_iter->second.begin();
		for (; guild_iter != all_iter->second.end(); ++guild_iter)
		{
			guild_iter->second->UpdateLimitGoods(m_vLimitGoodsList);
		}
	}

// 	MAPLOG(ERROR_LEVEL, "[%s]����������ʱˢ����Ʒ![GoodsNum:%d]",
// 		__FUNCTION__, m_vLimitGoodsList.size());
}

// �Ϸ�����
void CGuildManager::OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId)
{
	GUILD_MAP *pGuildList = GetGuildList(dwFromServerId);
	if (pGuildList)
	{
		GUILD_MAP::iterator iter = pGuildList->begin();
		for (; iter != pGuildList->end(); ++iter)
		{
			// ���·�������ǰ����������
			iter->second->SetServerId(dwToServerId);
			iter->second->SaveDataToDB();
		}
	}
}

// ȡ�������б�
GUILD_MAP* CGuildManager::GetGuildList(DWORD dwServerId)
{
	ALL_GUILD_MAP::iterator iter = m_AllGuildList.find(dwServerId);
	return iter != m_AllGuildList.end() ? &iter->second : NULL;
}