#include "stdafx.h"
#include "FriendManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"


CFriendManager::CFriendManager()
{

}

CFriendManager::~CFriendManager()
{

}

void CFriendManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pFriendInfo = &pPlayer->GetRoleFullInfo().tFriendInfo;
}

void CFriendManager::OnNewDay()
{
	m_pFriendInfo->wGetEnergyTimes = 0;
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		m_pFriendInfo->arFriendList[i].wIsGiveEnergy = false;
	}
}

// 请求好友列表
void CFriendManager::OnFriendList()
{
	SendBlackList();

	MSG_S2C_FRIEND_LIST_RESP msg;
	msg.m_byGetEnergyTimes = m_pFriendInfo->wGetEnergyTimes;
 	msg.m_byFriendNum = 0;
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		FriendData &sSrc = m_pFriendInfo->arFriendList[i];
		const PlayerBaseData *pFriendBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), sSrc.dwFriendId);
		if (!pFriendBaseData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_LIST_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的基本数据[RoleId:%d]", __FUNCTION__, sSrc.dwFriendId);
			return;
		}
		ClientFriendData &sDest = msg.m_FriendList[msg.m_byFriendNum++];
		memcpy_s(&sDest, sizeof(PlayerCommonData), pFriendBaseData, sizeof(PlayerCommonData));
		sDest.byIsGiveEnergy = sSrc.wIsGiveEnergy;
		sDest.byTakeEnergyState = sSrc.wTakeEnergyState;
		//strcpy_s(sDest.szAccount, sizeof(sDest.szAccount), pFriendBaseData->szAccount);
	}
	m_pPlayer->SendPacket(&msg);
}

// 请求黑名单列表
void CFriendManager::OnBlackList()
{
	SendBlackList();
}

// 请求申请列表
void CFriendManager::OnApplyList()
{
	MSG_S2C_FRIEND_APPLY_LIST_RESP msg;
	msg.m_byApplyNum = 0;
	int nCommonDataSize = sizeof(PlayerCommonData);
	for (BYTE i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		DWORD dwFriendId = m_pFriendInfo->arApplyList[i];
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwFriendId);
		if (pBaseData)
		{
			memcpy_s(&msg.m_ApplyList[msg.m_byApplyNum++], nCommonDataSize, pBaseData, nCommonDataSize);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// 请求推荐列表
void CFriendManager::OnRecommendList()
{
	MSG_S2C_FRIEND_RECOMMEND_LIST_RESP msg;
	memset(msg.m_RecommendList, 0, sizeof(msg.m_RecommendList));
	msg.m_byRecommendNum = 0;

	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();

	// todo::暂时这样处理,规则日后应该会调整
	const PlayerBaseDataMap *pPlayerList = PlayerBaseDataManager::Instance()->GetPlayerList(m_pPlayer->GetServerId());
	if (!pPlayerList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到服务器玩家列表[ServerId:%d]", __FUNCTION__, m_pPlayer->GetServerId());
		return;
	}

	for (PlayerBaseDataMap::const_iterator iter = pPlayerList->begin(); iter != pPlayerList->end(); ++iter)
	{
		if (msg.m_byRecommendNum >= MAX_FRIEND_RECOMMEND_NUM)
			break;

		const PlayerBaseData *pTarget = iter->second;

		if (m_pPlayer->GetCharID() != pTarget->dwRoleId &&			// 不是自己
			!pFriendMgr->IsFriend(pTarget->dwRoleId) &&				// 不是好友
			!pFriendMgr->IsBlack(pTarget->dwRoleId))				// 不是单名单
		{
			ClientFriendData &sDest = msg.m_RecommendList[msg.m_byRecommendNum++];
			memcpy_s(&sDest, sizeof(PlayerCommonData), pTarget, sizeof(PlayerCommonData));
		}
	}

	m_pPlayer->SendPacket(&msg);
}

// 添加好友
void CFriendManager::OnFriendAdd(DWORD dwTargetId)
{
	MSG_S2C_FRIEND_ADD_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_wResult = 0;

	// 是否自己
	if (m_pPlayer->GetCharID() == dwTargetId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]不能加自己为好友[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 检测是否已在好友列表
	if (GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标已是好友[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 检测自己好友上限
	if (IsFriendListFull())
	{
		msg.m_wResult = 1;
		m_pPlayer->SendPacket(&msg);
		MAPLOG(ERROR_LEVEL, "[%s]自己的好友的数量已达上限[Num:%d]", __FUNCTION__, GetFriendNum());
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);

	// 目标在线
	if (pTarget)
	{
		CFriendManager *pFriendMgr = pTarget->GetFriendManager();

		if (pFriendMgr->IsFriendListFull())
		{
			msg.m_wResult = 2;
			m_pPlayer->SendPacket(&msg);
			MAPLOG(GUI_LEVEL, "[%s]目标的好友的数量已达上限[Num:%d]", __FUNCTION__, GetFriendNum());
			return;
		}

		// 我是否在目标的黑名单里
		if (!pFriendMgr->IsBlack(m_pPlayer->GetCharID()))
		{
			// 把我添加到目标的申请列表中
			pFriendMgr->AddApplyData(m_pPlayer->GetCharID());
		}

		// 角色信息更新
		pTarget->RoleInfoUpdate();
	}
	// 目标离线
	else
	{
		// 把我添加到目标的申请列表中
		PlayerBaseDataManager::Instance()->AddFriendApplyData(m_pPlayer->GetServerId(), dwTargetId, m_pPlayer->GetCharID());
	}

	m_pPlayer->SendPacket(&msg);
}

// 删除好友 
void CFriendManager::OnFriendDel(DWORD dwTargetId)
{
	// 检测目标是否好友
	if (!GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_DEL_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标不是好友,不能删除[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (pTarget)
	{
		// 目标好友列表删掉我
		pTarget->GetFriendManager()->RemoveFriend(m_pPlayer->GetCharID());

		// 角色信息更新
		pTarget->RoleInfoUpdate();
	}
	else
	{
		// 到数据里去操作删除
		FriendData sDelTargetData;
		sDelTargetData.dwFriendId = m_pPlayer->GetCharID();
		UpateFriendDataInDB(dwTargetId, sDelTargetData, FRIEND_UPDATE_DEL_FRIEND);
	}

	// 我的好友列表把目标删除
	RemoveFriend(dwTargetId);

	MSG_S2C_FRIEND_DEL_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// 申请好友回复
void CFriendManager::OnFriendApplyReply(DWORD dwTargetId, BYTE byIsAccept)
{
	// 检测目标是否在自己的申请列表里
	if (!IsInApplyList(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目前没在申请列表中[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 从申请列表里拿掉
	RemoveApplyData(dwTargetId);

	// 检测目标是否已是我的好友
	if (GetFriendData(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标已是我的好友[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 接受
	if (byIsAccept)
	{
		// 检测自己的列表是否已满
		if (IsFriendListFull())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]当前好友数量已达上限，不能再加好友[targetid:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
		if (pTarget)
		{ // 目标在线
			CFriendManager *pTargetFriendMgr = pTarget->GetFriendManager();
			// 目标的好友列表已满
			if (pTargetFriendMgr->IsFriendListFull())
			{
				m_pPlayer->SendErrorMsg(ERROR_FRIEND_LIST_SIZE_LIMIT);
				return;
			}

			// 目标好友列表加入我
			pTargetFriendMgr->AddFriendData(m_pPlayer->GetRoleId());
			pTargetFriendMgr->RemoveBlackData(m_pPlayer->GetRoleId());
		
			// 我的好友列表加入目标
			AddFriendData(dwTargetId);
			RemoveBlackData(dwTargetId);

			// 角色信息更新
			pTarget->RoleInfoUpdate();

			// 如果我也在目标的申请列表里
			if (pTargetFriendMgr->IsInApplyList(m_pPlayer->GetRoleId()))
				pTargetFriendMgr->RemoveApplyData(m_pPlayer->GetRoleId());
		}
		else
		{ // 目标离线

			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
			if (!pBaseData)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_APPLY_REPLY_REQ);
				MAPLOG(GUI_LEVEL, "[%s]找不好角色基本数据[RoleId:%d]", __FUNCTION__, dwTargetId);
				return;
			}

			// 检测目标列表是否已满
			if (pBaseData->byFriendNum >= GetFriendNumMax(pBaseData->wLevel))
			{
				m_pPlayer->SendErrorMsg(ERROR_FRIEND_LIST_SIZE_LIMIT);
				return;
			}

			// 我的好友列表加入目标
			AddFriendData(dwTargetId);
			RemoveBlackData(dwTargetId);

			// 目标好友列表加入我
			FriendData sMyFriendData;
			memset(&sMyFriendData, 0, sizeof(sMyFriendData));
			sMyFriendData.dwFriendId = m_pPlayer->GetRoleId();
			UpateFriendDataInDB(dwTargetId, sMyFriendData, FRIEND_UPDATE_ADD_FRIEND);

			// 目标好友的数量增加
			PlayerBaseDataManager::Instance()->AddFriendNum(m_pPlayer->GetServerId(), dwTargetId);
		}

		FriendDataUpdateNotify(dwTargetId);

		// 发邮件通知目标(好友)
		EmailInfo tEailInfo(3);
		tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
		EmailManager::AddEmail(dwTargetId, tEailInfo, true);
	}

	MSG_S2C_FRIEND_APPLY_REPLY_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_byAccept = byIsAccept;
	m_pPlayer->SendPacket(&msg);
}

// 添加黑名单
void CFriendManager::OnBlackAdd(DWORD dwTargetId)
{
	// 检测目标是否已在黑名单
	if (IsBlack(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标已在黑名单列表[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
	if (!pBaseData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_ADD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标不存在[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 目标是自己的好友
	if (GetFriendData(dwTargetId))
	{
		Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);

		// 在线
		if (pTarget)
		{
			// 目标好友列表把自己去掉
			pTarget->GetFriendManager()->RemoveFriend(m_pPlayer->GetCharID());

			// 角色信息更新
			pTarget->RoleInfoUpdate();
		}
		// 离线
		else
		{
			// 到数据里操作删除
			FriendData sFriendData;
			sFriendData.dwFriendId = m_pPlayer->GetCharID();
			UpateFriendDataInDB(dwTargetId, sFriendData, FRIEND_UPDATE_ADD_BLACK);
		}

		// 把目标从好友列表删除
		m_pPlayer->GetFriendManager()->RemoveFriend(dwTargetId);
	}

	// 把目标添加黑名单
	AddBlackData(dwTargetId);

	MSG_S2C_BLACK_ADD_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// 删除黑名单
void CFriendManager::OnBlackDel(DWORD dwTargetId)
{
	if (!IsBlack(dwTargetId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_BLACK_DEL_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标木有黑名单里[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	RemoveBlackData(dwTargetId);

	MSG_S2C_BLACK_DEL_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);
}

// 赠送精力
void CFriendManager::OnEnergyGive(DWORD dwTargetId)
{
	// 检测是否好友
	FriendData *pFriendData = GetFriendData(dwTargetId);
	if (!pFriendData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GIVE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]目标不是好友不能赠送精力[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 检测是否已给过
	if (pFriendData->wIsGiveEnergy)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GIVE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]已经给目标赠送过精力[targetid:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (pTarget)
	{
		pTarget->GetFriendManager()->SetGetEnergyState(m_pPlayer->GetCharID(), FIEND_ENERGY_STATE_READY);
		pTarget->GetFriendManager()->FriendDataUpdateNotify(m_pPlayer->GetCharID());

		// 角色信息更新
		pTarget->RoleInfoUpdate();
	}
	else
	{
		// 赠送精力
		PlayerBaseDataManager::Instance()->AddGiveEnergyData(m_pPlayer->GetServerId(), dwTargetId, m_pPlayer->GetRoleId());
	}

	pFriendData->wIsGiveEnergy = true;

	MSG_S2C_FRIEND_ENERGY_GIVE_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	m_pPlayer->SendPacket(&msg);

	// 日常任务
	m_pPlayer->OnUpdateDailyMission(4);

	// 世界等级任务
	//m_pPlayer->OnUpdateWorldTaskEvent(4);
}

// 领取精力 
void CFriendManager::OnTakeEnergy(DWORD *pTargetList, BYTE byTargetNum)
{
	// 检测今日可领取的次数
	if (m_pFriendInfo->wGetEnergyTimes + byTargetNum > GetFriendNumMax())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]好友赠送精力领取次数已达上限[times:%d]", __FUNCTION__, m_pFriendInfo->wGetEnergyTimes);
		return;
	}

	for (BYTE i = 0; i < byTargetNum; ++i)
	{
		DWORD dwTargetId = pTargetList[i];
		// 检测是否好友
		FriendData *pFriendData = GetFriendData(dwTargetId);
		if (!pFriendData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]目标不是好友,不能领取精力[targetid:%d]", __FUNCTION__, dwTargetId);
			return;
		}

		// 检测是否可以领取
		if (FIEND_ENERGY_STATE_READY != pFriendData->wTakeEnergyState)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_FRIEND_ENERGY_GET_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]不能领取目标赠送的精力[targetid:%d,state:%d]", __FUNCTION__,
				dwTargetId, pFriendData->wTakeEnergyState);
			return;
		}

		pFriendData->wTakeEnergyState = FIEND_ENERGY_STATE_TAKE;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, byTargetNum);

	m_pFriendInfo->wGetEnergyTimes += byTargetNum;

	MSG_S2C_FRIEND_ENERGY_GET_RESP msg;
	msg.m_byTargetNum = byTargetNum;
	memcpy_s(msg.m_TargetList, byTargetNum*sizeof(DWORD), pTargetList, byTargetNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);
}

// 查找目标
void CFriendManager::OnNetTargetFind(const char *pszTargetName)
{
	MSG_S2C_FRIEND_TARGET_FIND_RESP msg;
	memset(&msg.m_TargetData, 0, sizeof(PlayerCommonData));
	const PlayerBaseData *pBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(m_pPlayer->GetServerId(), pszTargetName);
	if (pBaseData)
	{
		memcpy_s(&msg.m_TargetData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	}
	m_pPlayer->SendPacket(&msg);
}

// 添加好友申请数据
bool CFriendManager::AddApplyData(DWORD dwTargetId)
{
	if (IsInApplyList(dwTargetId))
		return true;

	if (m_pFriendInfo->wApplyNum >= MAX_FRIEND_APPLY_NUM)
		return false;

	m_pFriendInfo->arApplyList[m_pFriendInfo->wApplyNum++] = dwTargetId;

	return true;
}

// 查询申请数据
bool CFriendManager::IsInApplyList(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		if (m_pFriendInfo->arApplyList[i] == dwRoleId)
		{
			return true;
		}
	}
	return false;
}

// 添加赠送精力数据
void CFriendManager::AddGiveEnergyData(DWORD dwRoleId)
{
	FriendData *pFriendData = GetFriendData(dwRoleId);
	if (pFriendData)
	{
		pFriendData->wIsGiveEnergy = true;
	}
}

// 设置精力领取状态
void CFriendManager::SetGetEnergyState(DWORD dwRoleId, BYTE byState)
{
	FriendData *pFriendData = GetFriendData(dwRoleId);
	if (pFriendData)
	{
		pFriendData->wTakeEnergyState = byState;
	}
}

// 删除好友
void CFriendManager::RemoveFriend(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		if (m_pFriendInfo->arFriendList[i].dwFriendId == dwRoleId)
		{
			//FriendDataUpdateNotify(m_pFriendInfo->arFriendList[i], 2);
			m_pFriendInfo->arFriendList[i] = m_pFriendInfo->arFriendList[--m_pFriendInfo->wFriendNum];

			MSG_S2C_FRIEND_DATA_UPDATE_NOTIFY msg;
			msg.m_byFlag = false;
			msg.m_byType = 1;
			msg.m_FriendData.dwRoleId = dwRoleId;
			m_pPlayer->SendPacket(&msg);
			break;
		}
	}
}

// 删除申请好友数据
void CFriendManager::RemoveApplyData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wApplyNum; ++i)
	{
		if (m_pFriendInfo->arApplyList[i] == dwRoleId)
		{
			m_pFriendInfo->arApplyList[i] = m_pFriendInfo->arApplyList[--m_pFriendInfo->wApplyNum];
			break;
		}
	}
}

// 添加好友数据
void CFriendManager::AddFriendData(DWORD dwFriendId)
{
	if (m_pFriendInfo->wFriendNum >= MAX_FRIEND_NUM)
		return;

	if (GetFriendData(dwFriendId))
		return;

	FriendData &sDest = m_pFriendInfo->arFriendList[m_pFriendInfo->wFriendNum++];
	memset(&sDest, 0, sizeof(sDest));
	sDest.dwFriendId = dwFriendId;
}

// 是否好友列表已满
bool CFriendManager::IsFriendListFull()
{
	return GetFriendNum() >= GetFriendNumMax() ? true : false;
}

// 更新好友数据回调
void CFriendManager::UpdateFriendDataCallback(BYTE byType, const FriendData &tFriendData)
{
	// 删除好友
	if (FRIEND_UPDATE_DEL_FRIEND == byType)
	{

	}
	// 添加黑名单
	else if (FRIEND_UPDATE_ADD_BLACK == byType)
	{

	}
	// 添加好友
	else
	{
		
	}
}

// 好友数据更新通知
void CFriendManager::FriendDataUpdateNotify(DWORD dwFriendRoleId, BYTE byType)
{
	FriendData *pFrindData = GetFriendData(dwFriendRoleId);
	if (pFrindData)
	{
		FriendDataUpdateNotify(*pFrindData);
	}
}

// 好友数据更新通知
void CFriendManager::FriendDataUpdateNotify(const FriendData &tFriendData, BYTE byType)
{
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), tFriendData.dwFriendId);
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的基本数据[RoleId:%d]", __FUNCTION__, tFriendData.dwFriendId);
		return;
	}

	MSG_S2C_FRIEND_DATA_UPDATE_NOTIFY msg;
	msg.m_byFlag = true;
	msg.m_byType = byType;
	memcpy_s(&msg.m_FriendData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	msg.m_FriendData.byIsGiveEnergy = tFriendData.wIsGiveEnergy;
	msg.m_FriendData.byTakeEnergyState = tFriendData.wTakeEnergyState;
	m_pPlayer->SendPacket(&msg);
}

// 是否好友
bool CFriendManager::IsFriend(DWORD dwTargetId)
{
	return GetFriendData(dwTargetId) ? true : false;
}

// 是否黑名单
bool CFriendManager::IsBlack(DWORD dwTargetId)
{
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		if (m_pFriendInfo->arBlackList[i] == dwTargetId)
		{
			return true;
		}
	}
	return false;
}

// 取好友数量上限
BYTE CFriendManager::GetFriendNumMax()
{
	return GetFriendNumMax(m_pPlayer->GetLevel());
}

// 取好友数量上限
BYTE CFriendManager::GetFriendNumMax(WORD wRoleLevel)
{
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(wRoleLevel);
	if (!pLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到角色等级配置[Level:%d]", __FUNCTION__, wRoleLevel);
		return 0;
	}

	return BYTE(pLevelCfg->dwFriendNumMax);
}

// 更新数据
bool CFriendManager::UpdateClientFriendData(ClientFriendData &tFriendData, DWORD dwFriendId)
{
	const PlayerBaseData *pPlayerBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwFriendId);
	if (pPlayerBaseData)
	{
		memcpy_s(&tFriendData, sizeof(PlayerCommonData), pPlayerBaseData, sizeof(PlayerCommonData));
	}
	return true;
}

// 在数据里更新好友数据
void CFriendManager::UpateFriendDataInDB(DWORD dwUpateRoleId, const FriendData &tFriendData, BYTE byOperateType)
{
	// 到数据里操作删除
	FriendDataUpdate_Load* pQuery = FriendDataUpdate_Load::ALLOC();
	pQuery->SetIndex(DB_FRIEND_DATA_UPDATE);
	pQuery->SetThreadIndex(DB_THREAD_FRIEND);
	pQuery->SetOperateType(byOperateType);
	pQuery->SetKey(m_pPlayer->GetCharID());
	pQuery->SetUpdateRoleId(dwUpateRoleId);
	pQuery->SetFriendData(tFriendData);
	g_MapServer.MapDBQuery(pQuery);
}

// 发送黑名单列表
void CFriendManager::SendBlackList()
{
	MSG_S2C_BLACK_LIST_RESP msg;
	msg.m_byBlackNum = 0;
	int nCommonDataSize = sizeof(PlayerCommonData);
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		DWORD dwBlackId = m_pFriendInfo->arBlackList[i];
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwBlackId);
		if (pBaseData)
		{
			memcpy_s(&msg.m_BlackList[msg.m_byBlackNum++], nCommonDataSize, pBaseData, nCommonDataSize);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// 取好友数据
const FriendData* CFriendManager::GetFriendDataEx(DWORD dwRoleId)
{
	return GetFriendData(dwRoleId);
}

// 是否有红点
bool CFriendManager::IsHaveRedPoint()
{
	// 当好友界面中申请列表中有申请的玩家时
	if (m_pFriendInfo->wApplyNum)
		return true;

	// 当好友界面中有可领取的精力没有领取时
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		FriendData *pFriend = &m_pFriendInfo->arFriendList[i];
		if (FIEND_ENERGY_STATE_READY == pFriend->wTakeEnergyState)
			return true;
	}

	return false;
}

// 取好友数据
FriendData* CFriendManager::GetFriendData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wFriendNum; ++i)
	{
		if (m_pFriendInfo->arFriendList[i].dwFriendId == dwRoleId)
		{
			return &m_pFriendInfo->arFriendList[i];
		}
	}
	return NULL;
}

// 删掉黑名单数据
void CFriendManager::RemoveBlackData(DWORD dwRoleId)
{
	for (WORD i = 0; i < m_pFriendInfo->wBlackNum; ++i)
	{
		if (m_pFriendInfo->arBlackList[i] == dwRoleId)
		{
			m_pFriendInfo->arBlackList[i] = m_pFriendInfo->arBlackList[--m_pFriendInfo->wBlackNum];
			break;
		}
	}
}

// 添加黑名单
void CFriendManager::AddBlackData(DWORD dwTargetId)
{
	if (m_pFriendInfo->wBlackNum >= MAX_BLACK_NUM)
		return;

	if (IsBlack(dwTargetId))
		return;

	m_pFriendInfo->arBlackList[m_pFriendInfo->wBlackNum++] = dwTargetId;

	// 通知客户端
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId);
	if (pBaseData)
	{
		FriendData tFriendData;
		memset(&tFriendData, 0, sizeof(tFriendData));
		tFriendData.dwFriendId = pBaseData->dwRoleId;
		FriendDataUpdateNotify(tFriendData, 2);
	}
}