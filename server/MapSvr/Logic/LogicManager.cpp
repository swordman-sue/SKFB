#include "stdafx.h"
#include "LogicManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Friend\FriendManager.h"
#include "..\AttackCity\AttackCityManager.h"
#include "..\Rank\RankManager.h"
#include "..\TeamDungeon\TeamDungeonManager.h"
#include "..\Activity\ActivityManager.h"


/*
MySql数据库--mysql_real_escape_string()函数
unsigned long mysql_real_escape_string(MYSQL *mysql, char *to, const char *from, unsigned long length)

注意，mysql必须是有效的开放式连接。之所以需要它是因为，转义功能取决于服务器使用的字符集。

描述

该函数用于创建可在SQL语句中使用的合法SQL字符串。请参见9.1.1节，“字符串”。

按照连接的当前字符集，将“from”中的字符串编码为转义SQL字符串。将结果置于“to”中，并添加1个终结用NULL字节。
编码的字符为NUL(ASCII 0)、‘\\n’、‘\\r’、‘\\’、‘'’、‘"’、以及Control-Z（请参见9.1节，“文字值”）。
（严格地讲，MySQL仅需要反斜杠和引号字符，用于引用转义查询中的字符串。该函数能引用其他字符，从而使得它们在日志文件中具有更好的可读性）。

“from”指向的字符串必须是长度字节“long”。必须为“to”缓冲区分配至少length * 2 + 1字节。在最坏的情况下，
每个字符或许需要使用2个字节进行编码，而且还需要终结Null字节。当mysql_real_escape_string()返回时，“to”的内容是由Null终结的字符串。
返回值是编码字符串的长度，不包括终结用Null字符。

如果需要更改连接的字符集，应使用mysql_set_character_set()函数，而不是执行SET NAMES(或SET CHARACTER SET)语句。
mysql_set_character_set()的工作方式类似于SET NAMES，但它还能影响mysql_real_escape_string()所使用的字符集，而SET NAMES则不能。

示例：

char query[1000], *end;



end = strmov(query, "INSERT INTO test_table values(");

*end++ = '\\'';

end += mysql_real_escape_string(&mysql, end, "What's this", 11);

*end++ = '\\'';

*end++ = ',';

*end++ = '\\'';

end += mysql_real_escape_string(&mysql, end, "binary data: \\0\\r\\n", 16);

*end++ = '\\'';

*end++ = ')';



if (mysql_real_query(&mysql, query, (unsigned int)(end - query)))

{

fprintf(stderr, "Failed to insert row, Error: %s\\n",

mysql_error(&mysql));

}

该示例中使用的strmov()函数包含在mysqlclient库中，工作方式与strcpy()类似，但会返回指向第1个参数终结用Null的指针。

返回值

置于“to”中的值的长度，不包括终结用Null字符。

错误

无。
*/

BOOL CheckRoleName(const char* pszRoleName)
{
	char szRoleName[MAX_ROLE_LEN + 1] = { 0 };
	ChangeLocalChar(pszRoleName, szRoleName, sizeof(szRoleName));

	// 检测长度
	int nRoleLength = (int)strlen(szRoleName);
	if (!nRoleLength || nRoleLength >= MAX_ROLE_LEN)
	{
		return FALSE;
	}

	//4 检测字符串是否合法
	if (!IsLegalName(szRoleName, nRoleLength + 1))
	{
		MAPLOG(GUI_LEVEL, "[%s]名称有非法字符[name:%s]", __FUNCTION__, szRoleName);
		return FALSE;
	}

	// 检测敏感字库
	if (/*g_MapServer.MatchRoleName(pszRoleName) ||*/ g_MapServer.MatchSqlCmd(szRoleName))
	{
		MAPLOG(ERROR_LEVEL, "[%s]名称属于敏感字库[name:%s]", __FUNCTION__, szRoleName);
		return FALSE;
	}

	return TRUE;
}

CLogicManager::CLogicManager()
{
	m_lLastLeaveMessageTime = 0;
}

CLogicManager::~CLogicManager()
{

}

void CLogicManager::Init(Player *pPlayer)
{
	m_lLastLeaveMessageTime = 0;
	m_pPlayer = pPlayer;
	m_dwLastTakeActivityGiftTime = 0;
}

// 请求玩家详情
void CLogicManager::OnNetRoleDetails(DWORD dwTargetId, BYTE byType)
{
	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	if (pTarget)
	{
		MSG_S2C_ROLE_DETAILS_RESP msg;
		msg.m_byType = byType;
		pTarget->FillCommonData(&msg.m_Details);
		pTarget->GetHeroManager()->FillFormationList(msg.m_Details.arFormationList, msg.m_Details.byFormationNum);
		msg.m_Details.byIsFriend = m_pPlayer->GetFriendManager()->IsFriend(dwTargetId);
		msg.m_Details.byIsBlack = m_pPlayer->GetFriendManager()->IsBlack(dwTargetId);
		m_pPlayer->SendPacket(&msg);
	}
	else
	{
		//向DB请求角色完整的游戏信息 todo::暂时这么处理
		SelectCharInfoQuery* pQuery = SelectCharInfoQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_CHAR_INFO);
		pQuery->SetThreadIndex(m_pPlayer->GetAccountInfo()->GetDBIndex());
		pQuery->SetKey(m_pPlayer->GetGuid());
		pQuery->SetCharID(dwTargetId);
		pQuery->SetSelectType(1);
		pQuery->SetRequestType(byType);
		pQuery->SetChannelType(m_pPlayer->GetChannelType());
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 请求属性恢复
void CLogicManager::OnNetPropertyRecover(BYTE byPropertyType)
{
	WORD wFromMsgId = C2S_ROLE_PRO_RECOVER_REQ;

	// 取角色属性恢复配置
	const RoleProRecover_Config *pRecoverCfg = g_pCfgMgr->GetRoleProRecover(byPropertyType);
	if (!pRecoverCfg)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到属性恢复配置[ProType:%d]", __FUNCTION__, byPropertyType);
		return;
	}

	// 取角色等级
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(m_pPlayer->GetLevel());
	if (!pLevelCfg)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色等级配置[Level:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	DWORD *pdwLastRecoverTime = NULL;
	int nProValueMax = 0;

	switch (byPropertyType)
	{
	case ROLE_PRO_STAMINA:
	{
		pdwLastRecoverTime = &tBaseInfo.dwLastStaminaRecoverTime;
		nProValueMax = pLevelCfg->dwStaminaMax;
	}
	break;
	case ROLE_PRO_ENERGY:
	{
		pdwLastRecoverTime = &tBaseInfo.dwLastEnergyRecoverTime;
		nProValueMax = pLevelCfg->dwEnergyMax;
	}
	break;
	case ROLE_PRO_WANTED:
	{
		pdwLastRecoverTime = &tBaseInfo.dwLastWantedRecoverTime;
		nProValueMax = pLevelCfg->dwWantedMax;
	}
	break;
	default:
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的属性恢复类型[type:%d]", __FUNCTION__,byPropertyType);
		return;
	}

	// 检测是否已达上限
	int nCurValue = m_pPlayer->GetProperty(byPropertyType);
	if (nCurValue >= nProValueMax)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]当前属性值已达可恢复的上限[type:%d]", __FUNCTION__, byPropertyType);
		return;
	}

	// 计算恢复次数及更新恢复时间
	int nRecoverTimes = DoRecoveryTimes(*pdwLastRecoverTime, time(NULL), pRecoverCfg->nIntervalTime);
	if (nRecoverTimes)
	{
// 		MAPLOG(ERROR_LEVEL, "[%s]当前恢复次数为0[type:%d]", __FUNCTION__, byPropertyType);
// 		return;

		// 计算可恢复值
		int nRecoverValue = nRecoverTimes * pRecoverCfg->nRecoverValue + nCurValue;
		nRecoverValue = min(nRecoverValue, nProValueMax);

		// 设置恢复的值
		m_pPlayer->SetProperty(byPropertyType, nRecoverValue);
	}

	// 响应
	MSG_S2C_ROLE_PRO_RECOVER_RESP msg;
	msg.m_byType = byPropertyType;
	msg.m_dwRecoverTime = *pdwLastRecoverTime;
	m_pPlayer->SendPacket(&msg);

	//MAPLOG(GUI_LEVEL, "[%s]玩家请求体力恢复处理成功[type:%d]", __FUNCTION__, byPropertyType);
}

// 替换头像
void CLogicManager::OnNetReplaceHeadImage(DWORD dwHeadImageId)
{
	WORD wFromMsgId = C2S_HEAD_IMAGE_REPLACE_REQ;

	// 检测是否已收集到
	if (!m_pPlayer->GetHeroManager()->IsAlreadyCollection(dwHeadImageId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]还木有收集此英雄,不能设置为头像[HeroInfoId:%d]", __FUNCTION__, dwHeadImageId);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_HEAD_IMAGE, dwHeadImageId);

	// 响应
	MSG_S2C_HEAD_IMAGE_REPLACE_RESP msg;
	msg.m_dwHeadImageId = dwHeadImageId;
	m_pPlayer->SendPacket(&msg);
}

// 角色名称修改 
void CLogicManager::OnNetRoleNameModify(const char *pszRoleName)
{
	// ps::名称的入口一定要控制不能让长度超过32字节,避免后续的一些问题

	// 检测需要的道具
	DWORD dwCostItemId = g_GetConfigValue(264);
	DWORD dwCostItemNum = g_GetConfigValue(263);
	if (m_pPlayer->GetItemNum(dwCostItemId) < dwCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]修改名称需要的道具不足[CostItemId:%d]", __FUNCTION__, dwCostItemId);
		//return;
	}

	MSG_S2C_ROLE_NAME_MODIFY_RESP msg;
	strcpy_s(msg.m_szRoleName, sizeof(msg.m_szRoleName) - 1, pszRoleName);

	// 敏感过滤
	if (!CheckRoleName(pszRoleName))
	{
		msg.m_byResult = 2;
		m_pPlayer->SendPacket(&msg);
		return;
	}

	// 检测名称是否已被使用境 
	if (PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(pszRoleName) ||
		g_MapServer.IsReadyToUseRoleName(pszRoleName))
	{
		msg.m_byResult = 1;
		m_pPlayer->SendPacket(&msg);
		return;
	}

	// 检测是否竞技场机器人名称 
	const vector<ArenaRobot_Config> *pRobotList = g_pCfgMgr->GetArenaRobotList();
	if (pRobotList)
	{
		vector<ArenaRobot_Config>::const_iterator iter =  pRobotList->begin();
		for (; iter != pRobotList->end(); ++iter)
		{
			if (0 == strcmp(iter->strRobotName.c_str(), pszRoleName))
			{
				msg.m_byResult = 1;
				m_pPlayer->SendPacket(&msg);
				return;
			}
		}
	}

	// 加入准备使用列表
	g_MapServer.AddReadyToUseRoleName(pszRoleName);

	// 更新基本数据管理器 
	PlayerBaseDataManager::Instance()->RoleNameModify(m_pPlayer->GetServerId(), m_pPlayer->GetRoleName(), pszRoleName);

	// 更新在线数据管理器
	PlayerManager::Instance()->RoleNameModify(m_pPlayer, m_pPlayer->GetRoleName(), pszRoleName);

	// 消耗道具
	m_pPlayer->RemoveItem(dwCostItemId, dwCostItemNum, FROM_ROLE_NAME_MODIFY);

	msg.m_byResult = 0;
	m_pPlayer->SendPacket(&msg);
}

void CLogicManager::OnNetStartGame()
{
	MSG_S2C_START_GAME_RESP msg;
	msg.m_dwClientVersion = g_MapServer.GetClientVersion();
	m_pPlayer->SendPacket(&msg);
}

// 引导更新
void CLogicManager::OnNetGuideUpdate(int nKey, int nValue)
{
	RoleBaseInfo &tRoleInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	bool bAdd = true;

	// 有则更新
	for (WORD i = 0; i < tRoleInfo.wGuideNum; ++i)
	{
		if (tRoleInfo.arGuideList[i].nKey == nKey)
		{
			tRoleInfo.arGuideList[i].nValue = nValue;
			bAdd = false;
		}
	}

	// 无则插入
	if (bAdd)
	{
		if (tRoleInfo.wGuideNum >= MAX_GUIDE_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUIDE_UPDATE_REQ);;
			MAPLOG(GUI_LEVEL, "[%s]引导数据列表已满[num:%d]", __FUNCTION__, tRoleInfo.wGuideNum);
			return;
		}

		tRoleInfo.arGuideList[tRoleInfo.wGuideNum].nKey = nKey;
		tRoleInfo.arGuideList[tRoleInfo.wGuideNum].nValue = nValue;
		++tRoleInfo.wGuideNum;
	}

	MSG_S2C_GUIDE_UPDATE_RESP msg;
	msg.m_nKey = nKey;
	msg.m_nValue = nValue;
	
	m_pPlayer->SendPacket(&msg);

	// 为了更详细的查看引导进度,特殊处理(1.当前是36,37都记录, 2.之前是36,37且小于等级12都记录)
	if (nKey == 36 || nKey == 37)
	{
		tRoleInfo.nGuideX = nKey;
		tRoleInfo.nGuideY = nValue;
	}
	if ((tRoleInfo.nGuideX == 36 || tRoleInfo.nGuideX == 37) && nKey <= 12)
	{
		tRoleInfo.nGuideX = nKey;
		tRoleInfo.nGuideY = nValue;
	}
	// 统计新手引导进度
	if (nKey <= 12 && nKey >= tRoleInfo.nGuideX)
	{
		tRoleInfo.nGuideX = nKey;
		tRoleInfo.nGuideY = nValue;
	}	
}

// 玩家留言
void CLogicManager::OnNetLeaveMessage(const char *pszContact, const char *pszMessage)
{
	WORD wFromMsgId = C2S_PLAYER_LEVEL_MESSAGE_REQ;

	UINT64 lCurTime = GetTickCount64();

	// 检测存储CD(5秒)
	if (lCurTime <= m_lLastLeaveMessageTime + 5000)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]留言CD中....", __FUNCTION__);
		return;
	}

	// todo::检测留言数量上限

	// 检测内容
	if (!pszMessage || !pszMessage[0])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]留言内容不能为空", __FUNCTION__);
		return;
	}

	// 向数据库插入
	LeaveMessage_Save* pQuery = LeaveMessage_Save::ALLOC();
	pQuery->SetIndex(DB_LEAVE_MESSAGE_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_LEAVE_MESSAGE);
	pQuery->SetRoleId(m_pPlayer->GetRoleId());
	pQuery->SetServerId(m_pPlayer->GetServerId());
	pQuery->SetContack(pszContact ? pszContact : "");
	pQuery->SetMessage(pszMessage);
	g_MapServer.MapDBQuery(pQuery);

	MSG_S2C_PLAYER_LEVEL_MESSAGE_RESP msg;
	m_pPlayer->SendPacket(&msg);

	m_lLastLeaveMessageTime = lCurTime;
}

bool RoleFightingPowerPred(const PlayerBaseData *pBaseData1, const PlayerBaseData *pBaseData2)
{
	// 第一排序值不相等
	if (pBaseData1->dwFightingPower > pBaseData2->dwFightingPower)
		return true;

	// 第一排序值相等
	if (pBaseData1->dwFightingPower == pBaseData2->dwFightingPower && 
		pBaseData1->dwFightingPower > pBaseData2->dwFightingPower)
		return true;

	return false;
}

// 请求助战目标列表
void CLogicManager::OnNetHelpBattleTargetList()
{
	DWORD dwServerId = m_pPlayer->GetServerId();

	WORD wRoleLevel = m_pPlayer->GetLevel();

	WORD wNeedLevel = g_GetConfigValue(137);
	WORD wMinLevel = wRoleLevel > g_GetConfigValue(145) ? wRoleLevel - g_GetConfigValue(145) : 0;
	WORD wMaxLevel = g_GetConfigValue(146) + wRoleLevel;

	WORD wMaxFriendNum = g_GetConfigValue(147);
	WORD wMaxStrangerNum = g_GetConfigValue(148);
	WORD wMaxGuildMemberNum = 1; // todo::日后要调整为配置文件

	vector<const PlayerBaseData*> vTmpRandList; // 临时随机列表

	// 军团成员
	vector<const PlayerBaseData*> vGuildMemberBaseDataList;
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetRoleId());	
	if (pGuild)
	{
		const GuildData &tGuildData = pGuild->GetGuildData();
		for (WORD i = 0; i < tGuildData.byMemberNum; ++i)
		{
			const GuildMember *pMember = &tGuildData.arMemberList[i];

			// 取好友基本数据
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pMember->dwRoleId);
			if (!pBaseData)
				continue;

			// 需要指定等级方可助战
			if (pBaseData->wLevel < wNeedLevel)
				continue;

			// 去掉自己
			if (m_pPlayer->GetRoleId() == pBaseData->dwRoleId)
				continue;

			vTmpRandList.push_back(pBaseData);
		}
	}
	// 随机x个工会成员(目前1个)
	while (vTmpRandList.size())
	{
		int nIdxMax = vTmpRandList.size() - 1;
		int nRandIdx = g_Rand(0, nIdxMax);
		vGuildMemberBaseDataList.push_back(vTmpRandList[nRandIdx]);

		// 如果工会成员数量已够
		if (vGuildMemberBaseDataList.size() >= wMaxGuildMemberNum)
			break;

		// 删除被随到的
		vTmpRandList[nRandIdx] = vTmpRandList[nIdxMax];
		vTmpRandList.pop_back();
	}


	// 随机x个好友(目前2个)
	vector<const PlayerBaseData*> vFriendBaseDataList;
	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();
	if (!pFriendMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]好友管理器为空!", __FUNCTION__);
		return;
	}
	WORD wFriendNum = pFriendMgr->GetFriendNum();
	const FriendData *pFriendList = pFriendMgr->GetFrindList();
	vTmpRandList.clear();
	for (WORD i = 0; i < wFriendNum; ++i)
	{
		const FriendData *pFriend = &pFriendList[i];
		if (!pFriend)
			continue;

		// 助战次数已满
// 		if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
// 			continue;

		// 取好友基本数据
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pFriend->dwFriendId);
		if (!pBaseData)
			continue;

		// 需要指定等级方可助战
		if (pBaseData->wLevel < wNeedLevel)
			continue;

		// 过滤等级没在范围内的玩家
// 		if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
// 			continue;

		vTmpRandList.push_back(pBaseData);
	}

	// 随机两个好友
	while (vTmpRandList.size())
	{
		int nIdxMax = vTmpRandList.size() - 1;
		int nRandIdx = g_Rand(0, nIdxMax);
		vFriendBaseDataList.push_back(vTmpRandList[nRandIdx]);

		// 如果好友数量已够
		if (vFriendBaseDataList.size() >= wMaxFriendNum)
			break;

		// 删除被随到的
		vTmpRandList[nRandIdx] = vTmpRandList[nIdxMax];
		vTmpRandList.pop_back();
	}

	DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(m_pPlayer->GetRoleId());

	// 随机x陌生人(目前2个)
	vector<const PlayerBaseData*> vStrangerBaseDataList;
	const vector<DWORD>* pIdList = PlayerBaseDataManager::Instance()->GetPlayerIdList(dwServerId);
	if (pIdList && pIdList->size())
	{
		int nDoTimes = 0;
		vector<DWORD> vRandList = *pIdList;
		do 
		{
			// 随机1000次木有拿到目标就退出
			if (++nDoTimes >= 1000)
			{
				MAPLOG(GUI_LEVEL, "[%s]执行1000还木有随机到陌生人!囧!囧!囧!", __FUNCTION__);
				break;
			}

			// 随机目标
			int nRandIndex = g_Rand(0, vRandList.size() - 1);
			DWORD dwTargetId = vRandList[nRandIndex];

			// 好友不算陌生人
			if (pFriendMgr->IsFriend(dwTargetId))
				continue;

			// 去掉自己
			if (m_pPlayer->GetRoleId() == dwTargetId)
				continue;

			// 军团队友不算了陌生人
			if (dwMyGuildId && dwMyGuildId == CGuildManager::Instance()->GetGuildIdByRoleId(dwTargetId))
				continue;

			// 取目标数据
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, dwTargetId);
			if (!pBaseData)
				continue;

			// 达到指定等级方可助战
			if (pBaseData->wLevel < wNeedLevel)
				continue;

			// 过滤等级没在范围内的玩家
			if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
				continue;

			vStrangerBaseDataList.push_back(pBaseData);

			// 拿已经被随机到的
			vRandList[nRandIndex] = vRandList[vRandList.size() - 1];
			vRandList.pop_back();

			// 目标人数已满
			if (vStrangerBaseDataList.size() >= wMaxStrangerNum)
				break;

		} while (vRandList.size());
	}

	MSG_S2C_HELP_BATTLE_TARGET_RESP msg;
	msg.m_wTargetNum = 0;
	msg.m_wTodayUsedHelpBattleTimes = m_pPlayer->GetProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES);
	// 军团成员
	for (size_t i = 0; i < vGuildMemberBaseDataList.size(); ++i)
	{
		if (msg.m_wTargetNum >= MAX_HELP_BATTLE_TARGET_NUM)
			break;

		HelpBattleTargetData &tTargetData = msg.m_arTargetList[msg.m_wTargetNum++];
		const PlayerBaseData *pBaseData = vGuildMemberBaseDataList[i];

		tTargetData.dwRoleId = pBaseData->dwRoleId;
		tTargetData.wLevel = pBaseData->wLevel;
		tTargetData.byVipLevel = pBaseData->byVipLevel;
		tTargetData.dwFightingPower = pBaseData->dwFightingPower;
		strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
		tTargetData.byTargetType = 2;
		tTargetData.dwTitleId = pBaseData->dwTitleId;
		if (pBaseData->vToBattleHeroList.size())
			tTargetData.dwHeroInfoId = pBaseData->vToBattleHeroList[0].wHeroInfoId;
		tTargetData.byHelpBattleTimes = 0;
	}

	// 好友(目前2人)
	for (size_t i = 0; i < vFriendBaseDataList.size(); ++i)
	{
		if (msg.m_wTargetNum >= MAX_HELP_BATTLE_TARGET_NUM)
			break;

		HelpBattleTargetData &tTargetData = msg.m_arTargetList[msg.m_wTargetNum++];
		const PlayerBaseData *pBaseData = vFriendBaseDataList[i];

		tTargetData.dwRoleId = pBaseData->dwRoleId;
		tTargetData.wLevel = pBaseData->wLevel;
		tTargetData.byVipLevel = pBaseData->byVipLevel;
		tTargetData.dwFightingPower = pBaseData->dwFightingPower;
		strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
		tTargetData.byTargetType = 1;
		tTargetData.dwTitleId = pBaseData->dwTitleId;
		if (pBaseData->vToBattleHeroList.size())
			tTargetData.dwHeroInfoId = pBaseData->vToBattleHeroList[0].wHeroInfoId;
		tTargetData.byHelpBattleTimes = 0;
// 		const FriendData *pFriend = m_pPlayer->GetFriendManager()->GetFriendDataEx(pBaseData->dwRoleId);
// 		if (pFriend)
// 			tTargetData.byHelpBattleTimes = pFriend->wHelpBattleTimes;
	}

	// 陌生人(目前2人)
	for (size_t i = 0; i < vStrangerBaseDataList.size(); ++i)
	{
		if (msg.m_wTargetNum >= MAX_HELP_BATTLE_TARGET_NUM)
			break;

		HelpBattleTargetData &tTargetData = msg.m_arTargetList[msg.m_wTargetNum++];
		const PlayerBaseData *pBaseData = vStrangerBaseDataList[i];

		tTargetData.dwRoleId = pBaseData->dwRoleId;
		tTargetData.wLevel = pBaseData->wLevel;
		tTargetData.byVipLevel = pBaseData->byVipLevel;
		tTargetData.dwFightingPower = pBaseData->dwFightingPower;
		strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
		tTargetData.byTargetType = 0;
		tTargetData.dwTitleId = pBaseData->dwTitleId;
		if (pBaseData->vToBattleHeroList.size())
			tTargetData.dwHeroInfoId = pBaseData->vToBattleHeroList[0].wHeroInfoId;
		tTargetData.byHelpBattleTimes = 0;
	}

	m_pPlayer->SendPacket(&msg);
}

// 请求助战目标列表(旧版本)
 void CLogicManager::OnNetHelpBattleTargetListEx()
 {
 	DWORD dwServerId = m_pPlayer->GetServerId();
 
 	WORD wRoleLevel = m_pPlayer->GetLevel();
 
 	WORD wNeedLevel = g_GetConfigValue(137);
 	WORD wMinLevel = wRoleLevel > g_GetConfigValue(145) ? wRoleLevel - g_GetConfigValue(145) : 0;
 	WORD wMaxLevel = g_GetConfigValue(146) + wRoleLevel;
 
 	// 军团成员
 
 
 	// 取10个战力最高的好友
 	vector<const PlayerBaseData*> vFriendBaseDataList;
 	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();
 	CHECK_POINTER(pFriendMgr);
 	WORD wFriendNum = pFriendMgr->GetFriendNum();
 	const FriendData *pFriendList = pFriendMgr->GetFrindList();
 	for (WORD i = 0; i < wFriendNum; ++i)
 	{
 		const FriendData *pFriend = &pFriendList[i];
 		if (!pFriend)
 			continue;
 
 		// 助战次数已满
 		if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
 			continue;
 
 		// 取好友基本数据
 		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pFriend->dwFriendId);
 		if (!pBaseData)
 			continue;
 
 		// 需要指定等级方可助战
 		if (pBaseData->wLevel < wNeedLevel)
 			continue;
 
 		// 过滤等级没在范围内的玩家
 		// 		if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
 		// 			continue;
 
 		vFriendBaseDataList.push_back(pBaseData);
 	}
 
 	// 按战力排序
 	std::sort(vFriendBaseDataList.begin(), vFriendBaseDataList.end(), RoleFightingPowerPred);
 
 	// 随机两个陌生人
 	vector<const PlayerBaseData*> vStrangerBaseDataList;
 	const vector<DWORD>* pIdList = PlayerBaseDataManager::Instance()->GetPlayerIdList(dwServerId);
 	if (pIdList && pIdList->size())
 	{
 		int nDoTimes = 0;
 		vector<DWORD> vRandList = *pIdList;
 		do
 		{
 			if (++nDoTimes >= 1000)
 			{
 				MAPLOG(GUI_LEVEL, "[%s]执行1000还木有随机到陌生人!囧!囧!囧!", __FUNCTION__);
 				break;
 			}
 
 			// 随机目标
 			int nRandIndex = g_Rand(0, vRandList.size() - 1);
 			DWORD dwTargetId = vRandList[nRandIndex];
 
 			// 好友不算陌生人
 			if (pFriendMgr->IsFriend(dwTargetId))
 				continue;
 
 			// 去掉自己
 			if (m_pPlayer->GetRoleId() == dwTargetId)
 				continue;
 
 			// 取目标数据
 			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, dwTargetId);
 			if (!pBaseData)
 				continue;
 
 			// 达到指定等级方可助战
 			if (pBaseData->wLevel < wNeedLevel)
 				continue;
 
 			// 过滤等级没在范围内的玩家
 			if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
 				continue;
 
 			vStrangerBaseDataList.push_back(pBaseData);
 
 			// 拿已经被随机到的
 			vRandList[nRandIndex] = vRandList[vRandList.size() - 1];
 			vRandList.pop_back();
 		} while (vStrangerBaseDataList.size() < 2 && vRandList.size());
 	}
 
 	WORD wMaxFriendNum = g_GetConfigValue(147);
 	WORD wMaxStrangerNum = g_GetConfigValue(148);
 
 	MSG_S2C_HELP_BATTLE_TARGET_RESP msg;
 	msg.m_wTargetNum = 0;
 	// 好友
 	for (size_t i = 0; i < vFriendBaseDataList.size(); ++i)
 	{
 		if (msg.m_wTargetNum >= MAX_HELP_BATTLE_TARGET_NUM || msg.m_wTargetNum >= wMaxFriendNum)
 			break;
 
 		HelpBattleTargetData &tTargetData = msg.m_arTargetList[msg.m_wTargetNum++];
 		const PlayerBaseData *pBaseData = vFriendBaseDataList[i];
 
 		tTargetData.dwRoleId = pBaseData->dwRoleId;
 		tTargetData.wLevel = pBaseData->wLevel;
 		tTargetData.dwFightingPower = pBaseData->dwFightingPower;
 		strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
		//tTargetData.byIsFriend = true;
 		tTargetData.byTargetType = true;
 		tTargetData.dwTitleId = pBaseData->dwTitleId;
 		if (pBaseData->vToBattleHeroList.size())
 			tTargetData.dwHeroInfoId = pBaseData->vToBattleHeroList[0].wHeroInfoId;
 
 		const FriendData *pFriend = m_pPlayer->GetFriendManager()->GetFriendDataEx(pBaseData->dwRoleId);
 		if (pFriend)
 			tTargetData.byHelpBattleTimes = pFriend->wHelpBattleTimes;
 	}
 
 	// 陌生人
 	for (size_t i = 0; i < vStrangerBaseDataList.size(); ++i)
 	{
 		if (msg.m_wTargetNum >= MAX_HELP_BATTLE_TARGET_NUM /*|| msg.m_wTargetNum >= wMaxStrangerNum*/)
 			break;
 
 		HelpBattleTargetData &tTargetData = msg.m_arTargetList[msg.m_wTargetNum++];
 		const PlayerBaseData *pBaseData = vStrangerBaseDataList[i];
 
 		tTargetData.dwRoleId = pBaseData->dwRoleId;
 		tTargetData.wLevel = pBaseData->wLevel;
 		tTargetData.dwFightingPower = pBaseData->dwFightingPower;
 		strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
 		//tTargetData.byIsFriend = false;
		tTargetData.byTargetType = false;
 		tTargetData.dwTitleId = pBaseData->dwTitleId;
 		if (pBaseData->vToBattleHeroList.size())
 			tTargetData.dwHeroInfoId = pBaseData->vToBattleHeroList[0].wHeroInfoId;
 		tTargetData.byHelpBattleTimes = 0;
 	}
 
 	m_pPlayer->SendPacket(&msg);
 }

// 取激活码数字ID
int GetNumberIdByActivationCode(const char *pszCode)
{
	// 检测长度
	int nLen = strlen(pszCode);
	if (nLen != 8)
		return 0;

	// 取纯数字部分
	char szNumberId[8] = { 0 };
	strncpy_s(szNumberId, sizeof(szNumberId)-1, pszCode, 4);

	// 检测是否纯数字
	for (int i = 0; i < 4; ++i)
	{
		if (!CheckIsNumber(szNumberId[i]))
			return 0;
	}

	int nNumberId = atoi(szNumberId);

	// 数字ID是4位数
	if (nNumberId < 1000)
		return 0;

	return nNumberId;
}

// 领取活动礼包
void CLogicManager::OnNetTakeActivityGiftEx(const char *pszCode)
{
// 	// todo::此处逻辑有空整理一下，好多硬代码
// 
// 	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;
// 
// 	// 检测是否有效的激活码
// 	const ActivationCode_Config *pCodeCfg = g_pCfgMgr->GetActivationCode(pszCode);
// 	if (!pCodeCfg)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
// 		MAPLOG(ERROR_LEVEL, "[%s]激活码不存在[code:%s]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	// 取奖励配置
// 	const ActivationCodeReward_Config *pRewardCfg = g_pCfgMgr->GetActivationCodeReward(pCodeCfg->dwRewardId);
// 	if (!pRewardCfg)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到激活码奖励配置[reward_id:%d]!", __FUNCTION__, pCodeCfg->dwRewardId);
// 		return;
// 	}
// 
// 	tm tmNow;
// 	GetTMTime(&tmNow, time(NULL));
// 
// 	// 当前日期
// 	int nCurDate = (tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday;
// 
// 	// 检测日期是否已过期
// 	if (nCurDate > pRewardCfg->nValidDate)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]激活码已过期[code:%d]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	// 当前时间
// 	int nCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;
// 
// 	// 如果填0表示24小时结束
// 	int nValidTime = pRewardCfg->nValidTime ? pRewardCfg->nValidTime : 2359;
// 
// 	// 检测时间是否已过期
// 	if (nCurDate == pRewardCfg->nValidDate && nCurTime > nValidTime)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]激活码已过期[code:%d]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	int nNumberId = GetNumberIdByActivationCode(pszCode);
// 
// 	// 检测激活码是否合法
// 	if (!nNumberId)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]激活码配置不合法[code:%d,RecordId:%d]!", __FUNCTION__, pszCode, nNumberId);
// 		return;
// 	}
// 
// 	// 取激活码的类型
// 	int nType = nNumberId * 0.001;
// 
// 	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;
// 
// 	// 检测是否有领取过同一批的激活码
// 	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
// 	{
// 		if (tBaseInfo.arActivationTakeRecord[i] == nNumberId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
// 			MAPLOG(ERROR_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, pszCode);
// 			return;
// 		}
// 	}
// 
// 	// 检测列表是否已满
// 	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]激活码领取记录列表已满!", __FUNCTION__);
// 		return;
// 	}
// 
// 	// 个人激活码
// 	if (2 == nType)
// 	{
// 		if (g_MapServer.IsActivationCodeRecord(pszCode))
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
// 			MAPLOG(ERROR_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, pszCode);
// 			return;
// 		}
// 		g_MapServer.AddActivationCodeRecord(pszCode, m_pPlayer->GetRoleId());
// 	}
// 
// 	tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = nNumberId;
// 
// 	m_pPlayer->AddObject(pRewardCfg->dwItemId, 1);
// 
// 	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
// 	msg.m_dwGiftId = pRewardCfg->dwItemId;
// 	m_pPlayer->SendPacket(&msg);
// 
// 	MAPLOG(ERROR_LEVEL, "[%s]玩家领取激活码[RoleId:%d,Code:%s,ItemId:%d]!", __FUNCTION__,
// 		m_pPlayer->GetRoleId(), pszCode, pRewardCfg->dwItemId);
}

// 领取活动礼包
void CLogicManager::OnNetTakeActivityGift(const char *pszCode)
{
	// todo::此处逻辑有空整理一下，好多硬代码
	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;

	char szCode[MAX_ACTIVATE_CODE_LEN * 2 + 1] = {0};
	memset(szCode, 0, sizeof(szCode));
	ChangeLocalChar(pszCode, szCode, sizeof(szCode)); // 防止出现中文,做转换

	// 检测是否有效的激活码
	const ActivationCode_Config *pCodeCfg = g_pCfgMgr->GetActivationCode(szCode);
	if (!pCodeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]激活码不存在[code:%s]!", __FUNCTION__, szCode);
		return;
	}

	// 取奖励配置
	const ActivationCodeReward_Config *pRewardCfg = g_pCfgMgr->GetActivationCodeReward(pCodeCfg->dwRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到激活码奖励配置[reward_id:%d]!", __FUNCTION__, pCodeCfg->dwRewardId);
		return;
	}

	tm tmNow;
	GetTMTime(&tmNow, time(NULL));

	// 检测日期是否已过期
	int nCurDate = (tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday;
	if (nCurDate > pRewardCfg->nValidDate)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]激活码已过期[code:%d]!", __FUNCTION__, szCode);
		return;
	}

	// 检测时间是否已过期
	int nCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;
	int nValidTime = pRewardCfg->nValidTime ? pRewardCfg->nValidTime : 2359;  // 如果填0表示24小时结束
	if (nCurDate == pRewardCfg->nValidDate && nCurTime > nValidTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]激活码已过期[code:%d]!", __FUNCTION__, szCode);
		return;
	}

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// 检测列表是否已满
	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]激活码领取记录列表已满!", __FUNCTION__);
		return;
	}

	// 检测是否有领取过同一批的激活码(根据最终奖励的物品来判断)
	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
	{
		if (tBaseInfo.arActivationTakeRecord[i] == pCodeCfg->dwRewardId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(ERROR_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, szCode);
			return;
		}
	}

	// 记录已领取过
	tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = pCodeCfg->dwRewardId;

	// 发奖励
	m_pPlayer->AddObject(pRewardCfg->dwItemId, 1, TRUE, FROM_ACTIVATION_CODE_RECORD);

	// 响应
	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
	msg.m_dwGiftId = pRewardCfg->dwItemId;
	m_pPlayer->SendPacket(&msg);

	// todo::存储到数据库
	

	MAPLOG(ERROR_LEVEL, "[%s]玩家领取激活码[RoleId:%d,Code:%s,ItemId:%d]!", __FUNCTION__,
		m_pPlayer->GetRoleId(), szCode, pRewardCfg->dwItemId);
}

// 领取礼包码
void CLogicManager::OnNetTakeActivityGift_New(const char *pszCode)
{
	// 检测领取是否频繁(暂定1秒)
	DWORD dwCurTime = time(NULL);
	if (dwCurTime <= m_dwLastTakeActivityGiftTime + 1)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]频繁领取礼包[code:%s]!", __FUNCTION__, pszCode);
		return;
	}

	ActivationCode_Select* pQuery = ActivationCode_Select::ALLOC();
	pQuery->SetActivationCode(pszCode);
	pQuery->SetRoleID(m_pPlayer->GetRoleId());
	pQuery->SetIndex(DB_ACTIVATION_CODE_SELECT);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	g_MapServer.MapDBQuery(pQuery);

	m_dwLastTakeActivityGiftTime = dwCurTime;
}

// 领取活动礼包
void CLogicManager::DoTakeActivityGift(const char *pszCode, bool bIsExist, DWORD dwLimitTime, DWORD dwRewardId, WORD wStatus)
{
	// 激活码规则8位: 0[类型] 000[批数] 0000[随机数]

	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;

	// 检测激活码是否合法(id的生成规则)
	int nNumberId = GetNumberIdByActivationCode(pszCode);
	if (!nNumberId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]激活码配置不合法[code:%d,RecordId:%d]!", __FUNCTION__, pszCode, nNumberId);
		return;
	}

	// 检测激活码是否有效(此激活码是否存在)
	if (!bIsExist)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]激活码不存在[code:%s]!", __FUNCTION__, pszCode);
	}

	// 当前日期
	int nCurDate = g_Time2Date(time(NULL));

	// 检测激活码是否过期 20180223
	if (nCurDate > dwLimitTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]激活码已过期[code:%d]!", __FUNCTION__, pszCode);
		return;
	}

	// 取激活码的类型
	int nType = nNumberId * 0.001;

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// 检测是否有领取过同一批的激活码
	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
	{
		if (tBaseInfo.arActivationTakeRecord[i] == nNumberId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, pszCode);
			return;
		}
	}

	// 检测列表是否已满
	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]激活码领取记录列表已满!", __FUNCTION__);
		return;
	}

	// 个人激活码
	if (2 == nType)
	{
		if (g_MapServer.IsActivationCodeRecord(pszCode))
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, pszCode);
			return;
		}
		g_MapServer.AddActivationCodeRecord(pszCode, m_pPlayer->GetRoleId());
	}

	tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = nNumberId;

	m_pPlayer->AddObject(dwRewardId, 1, TRUE, FROM_ACTIVATION_CODE_RECORD);

	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
	msg.m_dwGiftId = dwRewardId;
	m_pPlayer->SendPacket(&msg);
}

// 请求活动公告
void CLogicManager::OnNetActivityNotice()
{
	MSG_S2C_ACTIVITY_NOTICE_RESP msg;
	msg.m_wNoticeId = g_GetConfigValue(126);
	m_pPlayer->SendPacket(&msg);
}

// 进入场景
void CLogicManager::OnNetEnterScene(WORD wSceneType, DWORD dwContext)
{
	// 组队副本相关场景
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetEnterScene(wSceneType, dwContext, m_pPlayer);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没处理的场景类型[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// 离开场景
void CLogicManager::OnNetLeaveScene(WORD wSceneType)
{
	// 组队副本相关场景
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetLeaveScene(wSceneType,m_pPlayer);
	}
	else 
	{
		MAPLOG(ERROR_LEVEL, "[%s]没处理的场景类型[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// 行走
void CLogicManager::OnNetGoTo(WORD wSceneType, MapPos tSrcPos, MapPos tDestPos)
{
	// 组队副本大厅
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetGoto(wSceneType,m_pPlayer, tSrcPos, tDestPos);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没处理的行走类型[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// 物品锁定
void CLogicManager::OnNetLockItem(WORD wSceneType, DWORD dwSceneItemUId)
{
	// 组队副本大厅
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetLockItem(m_pPlayer, dwSceneItemUId);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没处理的行走类型[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// 物品拾取
void CLogicManager::OnNetPickUpItem(WORD wSceneType, DWORD dwSceneItemUId)
{
	// 组队副本大厅
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetPickUpItem(m_pPlayer, dwSceneItemUId);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没处理的行走类型[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// 新手引导更新
void CLogicManager::OnNetNewRoleGuideUpdate(int nKey, int nValue)
{
	// xxxxxx * 1000 + xx
	int nGuideValue = nKey * 1000 + nValue;

	NewRoleGuide_Update* pQuery = NewRoleGuide_Update::ALLOC();
	pQuery->SetIndex(DB_NEW_ROLE_GUIDE_UPDATE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetValue(nGuideValue);
	pQuery->SetRoleId(m_pPlayer->GetRoleId());
	g_MapServer.MapDBQuery(pQuery);
}

// 领取礼包码回调
void CLogicManager::OnTakeActivityGiftCallback(const char *pszCode, int nResult, int nStatus, DWORD dwRewardId, DWORD dwExpireTime, int nBatch, int nType)
{
	// 查询结果(0=正常 1=激活码不存在 2=奖励不存在 3=已领取了同批激活码)
	if (nResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]激活码不存或奖励不存在[code:%s,batch:%d,result:%d]!", __FUNCTION__, pszCode, nBatch, nResult);
		return;
	}

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// 通用码
	if (nType)
	{
		// 检测是否有领取过同一批的激活码 
		for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
		{
			if (tBaseInfo.arActivationTakeRecord[i] == nBatch)
			{
				m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
				MAPLOG(GUI_LEVEL, "[%s]此激活码已被领取过[code:%s]!", __FUNCTION__, pszCode);
				return;
			}
		}
	}
	// 个人码
	else
	{
		// 状态(0=可领取 1=已被领取)
		if (nStatus)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]激活码已被领取[code:%s]!", __FUNCTION__, pszCode);
			return;
		}
	}

	// 检测有效时间
	if (time(NULL) > dwExpireTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_TIME_LIMIT);
		DATETIME curTime(dwExpireTime);
		char szCurTime[32] = { 0 };
		curTime.toString(szCurTime);
		MAPLOG(GUI_LEVEL, "[%s]激活码已过期[code:%s,batch:%d,time:%s]!", __FUNCTION__, pszCode, nBatch, szCurTime);
		return;
	}

	// 取奖励配置
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(dwRewardId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]找不到激活码奖励物品[code:%s,batch:%d,ItemId:%d]!", __FUNCTION__, pszCode, nBatch, dwRewardId);
		return;
	}

	// 发奖励
	m_pPlayer->AddObject(dwRewardId, 1, TRUE, FROM_ACTIVATION_CODE_RECORD);

	// 响应
	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
	msg.m_dwGiftId = dwRewardId;
	m_pPlayer->SendPacket(&msg);

	// 通用码
	if (nType)
	{
		if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]通用激活码记录列表已满[%d]!", __FUNCTION__, tBaseInfo.wActivationTakeRecordNum);
			return;
		}

		// 记录已领取过
		tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = nBatch;
	}
	// 个人码
	else
	{
		// 更新激活码数据
		ActivationCode_Update* pQuery = ActivationCode_Update::ALLOC();
		pQuery->SetBatch(nBatch);
		pQuery->SetActivationCode(pszCode);
		pQuery->SetRoleID(m_pPlayer->GetRoleId());
		pQuery->SetIndex(DB_ACTIVATION_CODE_UPDATE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 请求关卡评论记录
void CLogicManager::OnNetMapCommentList(WORD dwChapter)
{
	SendMapCommentList(dwChapter);
}
// 关卡评论
void CLogicManager::OnNetMapComment(WORD wChapter, const char *pszText)
{
	WORD wFromMsgId = C2S_MAP_COMMENT_REQ;

	// 检测评论内容是否为空
	if (!pszText || !pszText[0])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]评论内容为空[map_id:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// todo::检测评论敏感字

	// 检测此关卡是否可以评论
// 	const Map_Config *pMapCfg = g_pCfgMgr->GetMap(wChapter);
// 	if (!pMapCfg || !pMapCfg->byIsCanComment)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]此关卡不能评论[map_id:%d]!", __FUNCTION__, wChapter);
// 		return;
// 	}

	vector<MapComment> *pCommentList = g_MapServer.GetMapCommentList(m_pPlayer->GetServerId(), wChapter);
	if (!pCommentList)
	{
		g_MapServer.AddComment(m_pPlayer->GetServerId(), wChapter);
		pCommentList = g_MapServer.GetMapCommentList(m_pPlayer->GetServerId(), wChapter);
		if (!pCommentList)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到关卡评论数据[map_id:%d]!", __FUNCTION__, wChapter);
			return;
		}
	}

	WORD wMaxCommentId = 0;

	// 取玩家最近的评论
	MapComment *pComment = NULL;
	vector<MapComment>::iterator iter = pCommentList->begin();
	for (; iter != pCommentList->end(); ++iter)
	{
		if (iter->dwRoleId == m_pPlayer->GetRoleId() && (!pComment || iter->dwTime > pComment->dwTime))
		{
			pComment = &*iter;
		}
		wMaxCommentId = max(wMaxCommentId, iter->wCommentId);
	}

	// 检测评论CD
	if (pComment && time(NULL) < pComment->dwTime + g_GetConfigValue(138))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]关卡评论在CD中...[map_id:%d]!", __FUNCTION__, wChapter);
		return;
	}

	MapComment tComment;
	tComment.dwRoleId = m_pPlayer->GetRoleId();
	tComment.wCommentId = ++wMaxCommentId;
	tComment.dwTime = time(NULL);
	tComment.wLikeTimes = 0;
	tComment.wLikePlayerNum = 0;
	strcpy_s(tComment.szText, sizeof(tComment.szText), pszText);

	pCommentList->push_back(tComment);

	// 评论数量已达上限
	if (pCommentList->size() > MAX_MAP_COMMENT_NUM)
	{
		// 拿掉评论时间最早,点赞数量最少的
		vector<MapComment>::iterator iter_del = pCommentList->begin();
		for (iter = pCommentList->begin(); iter != pCommentList->end(); ++iter)
		{
			if (iter->wLikeTimes < iter_del->wLikeTimes ||  // 点赞次数少的
				(iter->wLikeTimes == iter_del->wLikeTimes && iter->dwTime < iter_del->dwTime)) // 点赞次数相同,则时间最早的
				iter_del = iter;
		}
		pCommentList->erase(iter_del);
	}

	// 数据响应
	MSG_S2C_MAP_COMMENT_RESP msg;
	msg.m_wMapId = wChapter;
	msg.m_wCommentId = tComment.wCommentId;
	msg.m_dwTime = tComment.dwTime;
	strcpy_s(msg.m_szText, sizeof(msg.m_szText), pszText);
	m_pPlayer->SendPacket(&msg);

	// 存储数据数据库
	g_MapServer.SaveMapCommentData(m_pPlayer->GetServerId(), wChapter);
}

// 关卡评论点赞
void CLogicManager::OnNetMapCommentLike(WORD wChapter, WORD wCommentId)
{
	WORD wFromMsgId = C2S_MAP_COMMENT_LIKE_REQ;

	vector<MapComment> *pCommentList = g_MapServer.GetMapCommentList(m_pPlayer->GetServerId(), wChapter);
	if (!pCommentList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]此关卡还木有评论[map_id:%d]!", __FUNCTION__, wChapter);
		return;
	}

	MapComment *pComment = NULL;
	vector<MapComment>::iterator iter = pCommentList->begin();
	for (; iter != pCommentList->end(); ++iter)
	{
		if (iter->wCommentId == wCommentId)
		{
			pComment = &*iter;
			break;
		}
	}
	if (!pComment)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]此评论木有存在[map_id:%d,comment_id:%d]!", __FUNCTION__, wChapter, wCommentId);
		return;
	}

	// 不能给自己点赞
	if (m_pPlayer->GetRoleId() == pComment->dwRoleId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]不能给自己点赞[map_id:%d,comment_id:%d]!", __FUNCTION__, wChapter, wCommentId);
		return;
	}

	// 检测是否已评论过
	for (WORD i = 0; i < pComment->wLikePlayerNum; ++i)
	{
		if (pComment->arLikePlayerList[i] == m_pPlayer->GetRoleId())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]你已经点赞过此评论[map_id:%d,comment_id:%d,role_id:%d]!",
				__FUNCTION__, wChapter, wCommentId, m_pPlayer->GetRoleId());
			return;
		}
	}

	// 添加点赞次数
	pComment->wLikeTimes++;

	// 记录已点赞
	if (pComment->wLikePlayerNum < MAX_MAP_COMMENT_LIKE_NUM)
	{
		pComment->arLikePlayerList[pComment->wLikePlayerNum++] = m_pPlayer->GetRoleId();
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]关卡评论点赞列表已满[Chapter:%d,comment_id:%d,role_id:%d]!",
			__FUNCTION__, wChapter, wCommentId, m_pPlayer->GetRoleId());
	}

	MSG_S2C_MAP_COMMENT_LIKE_RESP msg;
	msg.m_wCommentId = wCommentId;
	msg.m_wMapId = wChapter;
	m_pPlayer->SendPacket(&msg);

	// 存储数据数据库(todo::此处要优化)
	g_MapServer.SaveMapCommentData(m_pPlayer->GetServerId(), wChapter);
}

// 发送评论列表
void CLogicManager::SendMapCommentList(WORD wChapter)
{
	MSG_S2C_MAP_COMMENT_LIST_RESP msg;
	msg.m_wMapId = wChapter;
	msg.m_wCommentNum = 0;

	const vector<MapComment> *pCommentList = g_MapServer.GetMapCommentList(m_pPlayer->GetServerId(), wChapter);
	if (pCommentList)
	{
		vector<MapComment>::const_iterator iter = pCommentList->begin();
		for (; iter != pCommentList->end(); ++iter)
		{
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), iter->dwRoleId, true);
			if (!pBaseData)
				continue;

			MapCommentClient &tComment = msg.m_CommentList[msg.m_wCommentNum++];		
			memcpy_s(&tComment, sizeof(MapComment), &*iter, sizeof(MapComment));
			strcpy_s(tComment.szRoleName, sizeof(tComment.szRoleName), pBaseData->szRoleName);
			tComment.byVipLevel = pBaseData->byVipLevel;
		}
	}

	m_pPlayer->SendPacket(&msg);
}

	// 激活爵位
void CLogicManager::OnNetAcivateNobility()
{
	WORD wBobilityLevel = m_pPlayer->GetProperty(ROLE_PRO_NOBILITY_LEVEL);

	// 准备要激活的等级
	wBobilityLevel++;

	// 检测等级是否已达上限
	const Nobility_Config *pNobilityCfg = g_pCfgMgr->GetBobility(wBobilityLevel);
	if (!pNobilityCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到爵位配置或已达上限[level:%d]!", __FUNCTION__, wBobilityLevel);
		return;
	}

	// 检测是否已达上限
// 	if (!pNobilityCfg->dwNeedPower)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]爵位已达上限[max_level:%d]!", __FUNCTION__, wBobilityLevel);
// 		return;
// 	}

	// 检测需要的战力
	if (m_pPlayer->GetFightingPower() < pNobilityCfg->dwNeedPower)
	{
		MAPLOG(ERROR_LEVEL, "[%s]激活爵位需要的战力不足[need_power:%d]!", __FUNCTION__, pNobilityCfg->dwNeedPower);
		return;
	}

	m_pPlayer->SetProperty(ROLE_PRO_NOBILITY_LEVEL, wBobilityLevel);

	// 响应
	MSG_S2C_NOBILITY_ACTIVATE_RESP msg;
	msg.m_wNobilityLevel = wBobilityLevel;
	m_pPlayer->SendPacket(&msg);

	// 更新战力
	m_pPlayer->UpdateFightingPower();
}

// 扭蛋抽奖
void CLogicManager::OnNetNDLottery(WORD wTimes, BYTE byIsCostItem)
{
	// 检测活动是否开启
	DWORD dwActivityDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(131);
	if (!dwActivityDataId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]抽奖活动没有开启中[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	// 检测抽奖次数的合法性
	if (!wTimes || wTimes > MAX_ND_LOTTERY_TIMES)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]抽奖次数不合法[times:%d]!", __FUNCTION__, wTimes);
		return;
	}

	const NDLotteryCost_Config *pLotteryCostCfg = g_pCfgMgr->GetNDLotteryCost(dwActivityDataId);
	if (!pLotteryCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到扭蛋抽奖消耗配置[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	int nCostDiamond = 0;

	// 道具消耗
	if (byIsCostItem)
	{
		// 检测消耗的物品
		if (m_pPlayer->GetItemNum(pLotteryCostCfg->dwCostItemId) < wTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]抽奖消耗的物品不足[ItemId:%d]!", __FUNCTION__, pLotteryCostCfg->dwCostItemId);
			return;
		}
	}
	// 钻石消耗
	else
	{
		nCostDiamond = wTimes > 1 ? pLotteryCostCfg->nOenKeyCostDiamond : pLotteryCostCfg->nSingleCostDiamond;
	}

	// 取抽奖列表
	const vector<NDLottery_Config> *pLotteryList = g_pCfgMgr->GetNDLotteryList(dwActivityDataId);
	if (!pLotteryList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到扭蛋抽奖列表[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	// 计算总的权值
	int nTotalWeight = 0;
	vector<NDLottery_Config>::const_iterator iter = pLotteryList->begin();
	for (; iter != pLotteryList->end(); ++iter)
	{
		nTotalWeight += iter->nWeight;
	}

	// 处理抽中的逻辑
	vector<Item> vItemList;
	for (WORD i = 0; i < wTimes; ++i)
	{
		int nRandValue = g_Rand(1, nTotalWeight);
		int nCurWeight = 0;
		for (iter = pLotteryList->begin(); iter != pLotteryList->end(); ++iter)
		{
			nCurWeight += iter->nWeight;
			if (nCurWeight >= nRandValue)
			{
				vItemList.push_back(Item(iter->dwItemId, iter->dwItemNum));

				// 是否需要记录
				if (iter->byIsNeedRecord)
				{
					NDLotteryRecord tLotteryRecord;
					memset(&tLotteryRecord, 0, sizeof(tLotteryRecord));
					tLotteryRecord.wServerId = m_pPlayer->GetServerId();
					tLotteryRecord.dwItemId = iter->dwItemId;
					tLotteryRecord.dwItemNum = iter->dwItemNum;
					strcpy_s(tLotteryRecord.szRoleName, sizeof(tLotteryRecord.szRoleName), m_pPlayer->GetRoleName());
					g_MapServer.AddNDLotteryRecord(m_pPlayer->GetServerId(), tLotteryRecord);
				}
				break;
			}
		}
	}

	// 道具消耗
	if (byIsCostItem)
	{
		// 扣除消耗的物品
		m_pPlayer->RemoveItem(pLotteryCostCfg->dwCostItemId, wTimes, FROM_ND_LOTTERY_REWARD);
	}
	// 钻石消耗
	else
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ND_LOTTERY_REWARD);
	}

	// 添加抽中的物品
	m_pPlayer->AddObjectList(vItemList, TRUE, FROM_ND_LOTTERY_REWARD);

	// 响应
	MSG_S2C_ND_LOTTERY_RESP msg;
	msg.m_wTimes = wTimes;
	msg.m_wItemNum = 0;
	for (size_t i = 0; i < vItemList.size(); ++i)
	{
		if (msg.m_wItemNum >= MAX_ND_LOTTERY_TIMES)
			break;
		msg.m_ItemList[msg.m_wItemNum++] = vItemList[i];
	}
	m_pPlayer->SendPacket(&msg);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 37, wTimes, true);
}

// 请求抽奖记录
void CLogicManager::OnNetNDLotteryRecordList()
{
	MSG_S2C_ND_LOTTERY_RECORD_RESP msg;
	msg.m_wLotteryRecordNum = 0;
	msg.m_LotteryRecordList.clear();

	const deque<NDLotteryRecord> *pRecordList = g_MapServer.GetNDLotteryRecord(m_pPlayer->GetServerId());
	if (pRecordList)
	{
		deque<NDLotteryRecord>::const_iterator iter = pRecordList->begin();
		for (; iter != pRecordList->end(); ++iter)
		{
			msg.m_LotteryRecordList.push_back(*iter);
		}
	}

	m_pPlayer->SendPacket(&msg);
}

void CLogicManager::SetGuide(int nKey, int nValue)
{
	RoleBaseInfo &tRoleInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	bool bAdd = true;

	// 有则更新
	for (WORD i = 0; i < tRoleInfo.wGuideNum; ++i)
	{
		if (tRoleInfo.arGuideList[i].nKey == nKey)
		{
			tRoleInfo.arGuideList[i].nValue = nValue;
			bAdd = false;
		}
	}

	// 无则插入
	if (bAdd)
	{
		if (tRoleInfo.wGuideNum >= MAX_GUIDE_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUIDE_UPDATE_REQ);;
			MAPLOG(GUI_LEVEL, "[%s]引导数据列表已满[num:%d]", __FUNCTION__, tRoleInfo.wGuideNum);
			return;
		}

		tRoleInfo.arGuideList[tRoleInfo.wGuideNum].nKey = nKey;
		tRoleInfo.arGuideList[tRoleInfo.wGuideNum].nValue = nValue;
		++tRoleInfo.wGuideNum;
	}

// 	MSG_S2C_GUIDE_UPDATE_RESP msg;
// 	msg.m_nKey = nKey;
// 	msg.m_nValue = nValue;
// 
// 	m_pPlayer->SendPacket(&msg);

	// for test
	if (nKey <= 12 && nKey >= tRoleInfo.nGuideX)
	{
		tRoleInfo.nGuideX = nKey;
		tRoleInfo.nGuideY = nValue;
	}
}

