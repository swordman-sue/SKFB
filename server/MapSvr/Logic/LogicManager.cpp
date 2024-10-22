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
MySql���ݿ�--mysql_real_escape_string()����
unsigned long mysql_real_escape_string(MYSQL *mysql, char *to, const char *from, unsigned long length)

ע�⣬mysql��������Ч�Ŀ���ʽ���ӡ�֮������Ҫ������Ϊ��ת�幦��ȡ���ڷ�����ʹ�õ��ַ�����

����

�ú������ڴ�������SQL�����ʹ�õĺϷ�SQL�ַ�������μ�9.1.1�ڣ����ַ�������

�������ӵĵ�ǰ�ַ���������from���е��ַ�������Ϊת��SQL�ַ�������������ڡ�to���У������1���ս���NULL�ֽڡ�
������ַ�ΪNUL(ASCII 0)����\\n������\\r������\\������'������"�����Լ�Control-Z����μ�9.1�ڣ�������ֵ������
���ϸ�ؽ���MySQL����Ҫ��б�ܺ������ַ�����������ת���ѯ�е��ַ������ú��������������ַ����Ӷ�ʹ����������־�ļ��о��и��õĿɶ��ԣ���

��from��ָ����ַ��������ǳ����ֽڡ�long��������Ϊ��to����������������length * 2 + 1�ֽڡ����������£�
ÿ���ַ�������Ҫʹ��2���ֽڽ��б��룬���һ���Ҫ�ս�Null�ֽڡ���mysql_real_escape_string()����ʱ����to������������Null�ս���ַ�����
����ֵ�Ǳ����ַ����ĳ��ȣ��������ս���Null�ַ���

�����Ҫ�������ӵ��ַ�����Ӧʹ��mysql_set_character_set()������������ִ��SET NAMES(��SET CHARACTER SET)��䡣
mysql_set_character_set()�Ĺ�����ʽ������SET NAMES����������Ӱ��mysql_real_escape_string()��ʹ�õ��ַ�������SET NAMES���ܡ�

ʾ����

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

��ʾ����ʹ�õ�strmov()����������mysqlclient���У�������ʽ��strcpy()���ƣ����᷵��ָ���1�������ս���Null��ָ�롣

����ֵ

���ڡ�to���е�ֵ�ĳ��ȣ��������ս���Null�ַ���

����

�ޡ�
*/

BOOL CheckRoleName(const char* pszRoleName)
{
	char szRoleName[MAX_ROLE_LEN + 1] = { 0 };
	ChangeLocalChar(pszRoleName, szRoleName, sizeof(szRoleName));

	// ��ⳤ��
	int nRoleLength = (int)strlen(szRoleName);
	if (!nRoleLength || nRoleLength >= MAX_ROLE_LEN)
	{
		return FALSE;
	}

	//4 ����ַ����Ƿ�Ϸ�
	if (!IsLegalName(szRoleName, nRoleLength + 1))
	{
		MAPLOG(GUI_LEVEL, "[%s]�����зǷ��ַ�[name:%s]", __FUNCTION__, szRoleName);
		return FALSE;
	}

	// ��������ֿ�
	if (/*g_MapServer.MatchRoleName(pszRoleName) ||*/ g_MapServer.MatchSqlCmd(szRoleName))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������������ֿ�[name:%s]", __FUNCTION__, szRoleName);
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

// �����������
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
		//��DB�����ɫ��������Ϸ��Ϣ todo::��ʱ��ô����
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

// �������Իָ�
void CLogicManager::OnNetPropertyRecover(BYTE byPropertyType)
{
	WORD wFromMsgId = C2S_ROLE_PRO_RECOVER_REQ;

	// ȡ��ɫ���Իָ�����
	const RoleProRecover_Config *pRecoverCfg = g_pCfgMgr->GetRoleProRecover(byPropertyType);
	if (!pRecoverCfg)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Իָ�����[ProType:%d]", __FUNCTION__, byPropertyType);
		return;
	}

	// ȡ��ɫ�ȼ�
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(m_pPlayer->GetLevel());
	if (!pLevelCfg)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫ�ȼ�����[Level:%d]", __FUNCTION__, m_pPlayer->GetLevel());
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
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д�������Իָ�����[type:%d]", __FUNCTION__,byPropertyType);
		return;
	}

	// ����Ƿ��Ѵ�����
	int nCurValue = m_pPlayer->GetProperty(byPropertyType);
	if (nCurValue >= nProValueMax)
	{
		//m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]��ǰ����ֵ�Ѵ�ɻָ�������[type:%d]", __FUNCTION__, byPropertyType);
		return;
	}

	// ����ָ����������»ָ�ʱ��
	int nRecoverTimes = DoRecoveryTimes(*pdwLastRecoverTime, time(NULL), pRecoverCfg->nIntervalTime);
	if (nRecoverTimes)
	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ǰ�ָ�����Ϊ0[type:%d]", __FUNCTION__, byPropertyType);
// 		return;

		// ����ɻָ�ֵ
		int nRecoverValue = nRecoverTimes * pRecoverCfg->nRecoverValue + nCurValue;
		nRecoverValue = min(nRecoverValue, nProValueMax);

		// ���ûָ���ֵ
		m_pPlayer->SetProperty(byPropertyType, nRecoverValue);
	}

	// ��Ӧ
	MSG_S2C_ROLE_PRO_RECOVER_RESP msg;
	msg.m_byType = byPropertyType;
	msg.m_dwRecoverTime = *pdwLastRecoverTime;
	m_pPlayer->SendPacket(&msg);

	//MAPLOG(GUI_LEVEL, "[%s]������������ָ�����ɹ�[type:%d]", __FUNCTION__, byPropertyType);
}

// �滻ͷ��
void CLogicManager::OnNetReplaceHeadImage(DWORD dwHeadImageId)
{
	WORD wFromMsgId = C2S_HEAD_IMAGE_REPLACE_REQ;

	// ����Ƿ����ռ���
	if (!m_pPlayer->GetHeroManager()->IsAlreadyCollection(dwHeadImageId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]��ľ���ռ���Ӣ��,��������Ϊͷ��[HeroInfoId:%d]", __FUNCTION__, dwHeadImageId);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_HEAD_IMAGE, dwHeadImageId);

	// ��Ӧ
	MSG_S2C_HEAD_IMAGE_REPLACE_RESP msg;
	msg.m_dwHeadImageId = dwHeadImageId;
	m_pPlayer->SendPacket(&msg);
}

// ��ɫ�����޸� 
void CLogicManager::OnNetRoleNameModify(const char *pszRoleName)
{
	// ps::���Ƶ����һ��Ҫ���Ʋ����ó��ȳ���32�ֽ�,���������һЩ����

	// �����Ҫ�ĵ���
	DWORD dwCostItemId = g_GetConfigValue(264);
	DWORD dwCostItemNum = g_GetConfigValue(263);
	if (m_pPlayer->GetItemNum(dwCostItemId) < dwCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]�޸�������Ҫ�ĵ��߲���[CostItemId:%d]", __FUNCTION__, dwCostItemId);
		//return;
	}

	MSG_S2C_ROLE_NAME_MODIFY_RESP msg;
	strcpy_s(msg.m_szRoleName, sizeof(msg.m_szRoleName) - 1, pszRoleName);

	// ���й���
	if (!CheckRoleName(pszRoleName))
	{
		msg.m_byResult = 2;
		m_pPlayer->SendPacket(&msg);
		return;
	}

	// ��������Ƿ��ѱ�ʹ�þ� 
	if (PlayerBaseDataManager::Instance()->GetPlayerBaseDataByName(pszRoleName) ||
		g_MapServer.IsReadyToUseRoleName(pszRoleName))
	{
		msg.m_byResult = 1;
		m_pPlayer->SendPacket(&msg);
		return;
	}

	// ����Ƿ񾺼������������� 
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

	// ����׼��ʹ���б�
	g_MapServer.AddReadyToUseRoleName(pszRoleName);

	// ���»������ݹ����� 
	PlayerBaseDataManager::Instance()->RoleNameModify(m_pPlayer->GetServerId(), m_pPlayer->GetRoleName(), pszRoleName);

	// �����������ݹ�����
	PlayerManager::Instance()->RoleNameModify(m_pPlayer, m_pPlayer->GetRoleName(), pszRoleName);

	// ���ĵ���
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

// ��������
void CLogicManager::OnNetGuideUpdate(int nKey, int nValue)
{
	RoleBaseInfo &tRoleInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	bool bAdd = true;

	// �������
	for (WORD i = 0; i < tRoleInfo.wGuideNum; ++i)
	{
		if (tRoleInfo.arGuideList[i].nKey == nKey)
		{
			tRoleInfo.arGuideList[i].nValue = nValue;
			bAdd = false;
		}
	}

	// �������
	if (bAdd)
	{
		if (tRoleInfo.wGuideNum >= MAX_GUIDE_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUIDE_UPDATE_REQ);;
			MAPLOG(GUI_LEVEL, "[%s]���������б�����[num:%d]", __FUNCTION__, tRoleInfo.wGuideNum);
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

	// Ϊ�˸���ϸ�Ĳ鿴��������,���⴦��(1.��ǰ��36,37����¼, 2.֮ǰ��36,37��С�ڵȼ�12����¼)
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
	// ͳ��������������
	if (nKey <= 12 && nKey >= tRoleInfo.nGuideX)
	{
		tRoleInfo.nGuideX = nKey;
		tRoleInfo.nGuideY = nValue;
	}	
}

// �������
void CLogicManager::OnNetLeaveMessage(const char *pszContact, const char *pszMessage)
{
	WORD wFromMsgId = C2S_PLAYER_LEVEL_MESSAGE_REQ;

	UINT64 lCurTime = GetTickCount64();

	// ���洢CD(5��)
	if (lCurTime <= m_lLastLeaveMessageTime + 5000)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]����CD��....", __FUNCTION__);
		return;
	}

	// todo::���������������

	// �������
	if (!pszMessage || !pszMessage[0])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]�������ݲ���Ϊ��", __FUNCTION__);
		return;
	}

	// �����ݿ����
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
	// ��һ����ֵ�����
	if (pBaseData1->dwFightingPower > pBaseData2->dwFightingPower)
		return true;

	// ��һ����ֵ���
	if (pBaseData1->dwFightingPower == pBaseData2->dwFightingPower && 
		pBaseData1->dwFightingPower > pBaseData2->dwFightingPower)
		return true;

	return false;
}

// ������սĿ���б�
void CLogicManager::OnNetHelpBattleTargetList()
{
	DWORD dwServerId = m_pPlayer->GetServerId();

	WORD wRoleLevel = m_pPlayer->GetLevel();

	WORD wNeedLevel = g_GetConfigValue(137);
	WORD wMinLevel = wRoleLevel > g_GetConfigValue(145) ? wRoleLevel - g_GetConfigValue(145) : 0;
	WORD wMaxLevel = g_GetConfigValue(146) + wRoleLevel;

	WORD wMaxFriendNum = g_GetConfigValue(147);
	WORD wMaxStrangerNum = g_GetConfigValue(148);
	WORD wMaxGuildMemberNum = 1; // todo::�պ�Ҫ����Ϊ�����ļ�

	vector<const PlayerBaseData*> vTmpRandList; // ��ʱ����б�

	// ���ų�Ա
	vector<const PlayerBaseData*> vGuildMemberBaseDataList;
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetRoleId());	
	if (pGuild)
	{
		const GuildData &tGuildData = pGuild->GetGuildData();
		for (WORD i = 0; i < tGuildData.byMemberNum; ++i)
		{
			const GuildMember *pMember = &tGuildData.arMemberList[i];

			// ȡ���ѻ�������
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pMember->dwRoleId);
			if (!pBaseData)
				continue;

			// ��Ҫָ���ȼ�������ս
			if (pBaseData->wLevel < wNeedLevel)
				continue;

			// ȥ���Լ�
			if (m_pPlayer->GetRoleId() == pBaseData->dwRoleId)
				continue;

			vTmpRandList.push_back(pBaseData);
		}
	}
	// ���x�������Ա(Ŀǰ1��)
	while (vTmpRandList.size())
	{
		int nIdxMax = vTmpRandList.size() - 1;
		int nRandIdx = g_Rand(0, nIdxMax);
		vGuildMemberBaseDataList.push_back(vTmpRandList[nRandIdx]);

		// ��������Ա�����ѹ�
		if (vGuildMemberBaseDataList.size() >= wMaxGuildMemberNum)
			break;

		// ɾ�����浽��
		vTmpRandList[nRandIdx] = vTmpRandList[nIdxMax];
		vTmpRandList.pop_back();
	}


	// ���x������(Ŀǰ2��)
	vector<const PlayerBaseData*> vFriendBaseDataList;
	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();
	if (!pFriendMgr)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ѹ�����Ϊ��!", __FUNCTION__);
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

		// ��ս��������
// 		if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
// 			continue;

		// ȡ���ѻ�������
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pFriend->dwFriendId);
		if (!pBaseData)
			continue;

		// ��Ҫָ���ȼ�������ս
		if (pBaseData->wLevel < wNeedLevel)
			continue;

		// ���˵ȼ�û�ڷ�Χ�ڵ����
// 		if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
// 			continue;

		vTmpRandList.push_back(pBaseData);
	}

	// �����������
	while (vTmpRandList.size())
	{
		int nIdxMax = vTmpRandList.size() - 1;
		int nRandIdx = g_Rand(0, nIdxMax);
		vFriendBaseDataList.push_back(vTmpRandList[nRandIdx]);

		// ������������ѹ�
		if (vFriendBaseDataList.size() >= wMaxFriendNum)
			break;

		// ɾ�����浽��
		vTmpRandList[nRandIdx] = vTmpRandList[nIdxMax];
		vTmpRandList.pop_back();
	}

	DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(m_pPlayer->GetRoleId());

	// ���xİ����(Ŀǰ2��)
	vector<const PlayerBaseData*> vStrangerBaseDataList;
	const vector<DWORD>* pIdList = PlayerBaseDataManager::Instance()->GetPlayerIdList(dwServerId);
	if (pIdList && pIdList->size())
	{
		int nDoTimes = 0;
		vector<DWORD> vRandList = *pIdList;
		do 
		{
			// ���1000��ľ���õ�Ŀ����˳�
			if (++nDoTimes >= 1000)
			{
				MAPLOG(GUI_LEVEL, "[%s]ִ��1000��ľ�������İ����!��!��!��!", __FUNCTION__);
				break;
			}

			// ���Ŀ��
			int nRandIndex = g_Rand(0, vRandList.size() - 1);
			DWORD dwTargetId = vRandList[nRandIndex];

			// ���Ѳ���İ����
			if (pFriendMgr->IsFriend(dwTargetId))
				continue;

			// ȥ���Լ�
			if (m_pPlayer->GetRoleId() == dwTargetId)
				continue;

			// ���Ŷ��Ѳ�����İ����
			if (dwMyGuildId && dwMyGuildId == CGuildManager::Instance()->GetGuildIdByRoleId(dwTargetId))
				continue;

			// ȡĿ������
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, dwTargetId);
			if (!pBaseData)
				continue;

			// �ﵽָ���ȼ�������ս
			if (pBaseData->wLevel < wNeedLevel)
				continue;

			// ���˵ȼ�û�ڷ�Χ�ڵ����
			if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
				continue;

			vStrangerBaseDataList.push_back(pBaseData);

			// ���Ѿ����������
			vRandList[nRandIndex] = vRandList[vRandList.size() - 1];
			vRandList.pop_back();

			// Ŀ����������
			if (vStrangerBaseDataList.size() >= wMaxStrangerNum)
				break;

		} while (vRandList.size());
	}

	MSG_S2C_HELP_BATTLE_TARGET_RESP msg;
	msg.m_wTargetNum = 0;
	msg.m_wTodayUsedHelpBattleTimes = m_pPlayer->GetProperty(ROLE_PRO_TODAY_HELP_BATTLE_TIMES);
	// ���ų�Ա
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

	// ����(Ŀǰ2��)
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

	// İ����(Ŀǰ2��)
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

// ������սĿ���б�(�ɰ汾)
 void CLogicManager::OnNetHelpBattleTargetListEx()
 {
 	DWORD dwServerId = m_pPlayer->GetServerId();
 
 	WORD wRoleLevel = m_pPlayer->GetLevel();
 
 	WORD wNeedLevel = g_GetConfigValue(137);
 	WORD wMinLevel = wRoleLevel > g_GetConfigValue(145) ? wRoleLevel - g_GetConfigValue(145) : 0;
 	WORD wMaxLevel = g_GetConfigValue(146) + wRoleLevel;
 
 	// ���ų�Ա
 
 
 	// ȡ10��ս����ߵĺ���
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
 
 		// ��ս��������
 		if (pFriend->wHelpBattleTimes >= g_GetConfigValue(122))
 			continue;
 
 		// ȡ���ѻ�������
 		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pFriend->dwFriendId);
 		if (!pBaseData)
 			continue;
 
 		// ��Ҫָ���ȼ�������ս
 		if (pBaseData->wLevel < wNeedLevel)
 			continue;
 
 		// ���˵ȼ�û�ڷ�Χ�ڵ����
 		// 		if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
 		// 			continue;
 
 		vFriendBaseDataList.push_back(pBaseData);
 	}
 
 	// ��ս������
 	std::sort(vFriendBaseDataList.begin(), vFriendBaseDataList.end(), RoleFightingPowerPred);
 
 	// �������İ����
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
 				MAPLOG(GUI_LEVEL, "[%s]ִ��1000��ľ�������İ����!��!��!��!", __FUNCTION__);
 				break;
 			}
 
 			// ���Ŀ��
 			int nRandIndex = g_Rand(0, vRandList.size() - 1);
 			DWORD dwTargetId = vRandList[nRandIndex];
 
 			// ���Ѳ���İ����
 			if (pFriendMgr->IsFriend(dwTargetId))
 				continue;
 
 			// ȥ���Լ�
 			if (m_pPlayer->GetRoleId() == dwTargetId)
 				continue;
 
 			// ȡĿ������
 			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, dwTargetId);
 			if (!pBaseData)
 				continue;
 
 			// �ﵽָ���ȼ�������ս
 			if (pBaseData->wLevel < wNeedLevel)
 				continue;
 
 			// ���˵ȼ�û�ڷ�Χ�ڵ����
 			if (pBaseData->wLevel < wMinLevel || pBaseData->wLevel > wMaxLevel)
 				continue;
 
 			vStrangerBaseDataList.push_back(pBaseData);
 
 			// ���Ѿ����������
 			vRandList[nRandIndex] = vRandList[vRandList.size() - 1];
 			vRandList.pop_back();
 		} while (vStrangerBaseDataList.size() < 2 && vRandList.size());
 	}
 
 	WORD wMaxFriendNum = g_GetConfigValue(147);
 	WORD wMaxStrangerNum = g_GetConfigValue(148);
 
 	MSG_S2C_HELP_BATTLE_TARGET_RESP msg;
 	msg.m_wTargetNum = 0;
 	// ����
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
 
 	// İ����
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

// ȡ����������ID
int GetNumberIdByActivationCode(const char *pszCode)
{
	// ��ⳤ��
	int nLen = strlen(pszCode);
	if (nLen != 8)
		return 0;

	// ȡ�����ֲ���
	char szNumberId[8] = { 0 };
	strncpy_s(szNumberId, sizeof(szNumberId)-1, pszCode, 4);

	// ����Ƿ�����
	for (int i = 0; i < 4; ++i)
	{
		if (!CheckIsNumber(szNumberId[i]))
			return 0;
	}

	int nNumberId = atoi(szNumberId);

	// ����ID��4λ��
	if (nNumberId < 1000)
		return 0;

	return nNumberId;
}

// ��ȡ����
void CLogicManager::OnNetTakeActivityGiftEx(const char *pszCode)
{
// 	// todo::�˴��߼��п�����һ�£��ö�Ӳ����
// 
// 	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;
// 
// 	// ����Ƿ���Ч�ļ�����
// 	const ActivationCode_Config *pCodeCfg = g_pCfgMgr->GetActivationCode(pszCode);
// 	if (!pCodeCfg)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
// 		MAPLOG(ERROR_LEVEL, "[%s]�����벻����[code:%s]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	// ȡ��������
// 	const ActivationCodeReward_Config *pRewardCfg = g_pCfgMgr->GetActivationCodeReward(pCodeCfg->dwRewardId);
// 	if (!pRewardCfg)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������뽱������[reward_id:%d]!", __FUNCTION__, pCodeCfg->dwRewardId);
// 		return;
// 	}
// 
// 	tm tmNow;
// 	GetTMTime(&tmNow, time(NULL));
// 
// 	// ��ǰ����
// 	int nCurDate = (tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday;
// 
// 	// ��������Ƿ��ѹ���
// 	if (nCurDate > pRewardCfg->nValidDate)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]�������ѹ���[code:%d]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	// ��ǰʱ��
// 	int nCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;
// 
// 	// �����0��ʾ24Сʱ����
// 	int nValidTime = pRewardCfg->nValidTime ? pRewardCfg->nValidTime : 2359;
// 
// 	// ���ʱ���Ƿ��ѹ���
// 	if (nCurDate == pRewardCfg->nValidDate && nCurTime > nValidTime)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]�������ѹ���[code:%d]!", __FUNCTION__, pszCode);
// 		return;
// 	}
// 
// 	int nNumberId = GetNumberIdByActivationCode(pszCode);
// 
// 	// ��⼤�����Ƿ�Ϸ�
// 	if (!nNumberId)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]���������ò��Ϸ�[code:%d,RecordId:%d]!", __FUNCTION__, pszCode, nNumberId);
// 		return;
// 	}
// 
// 	// ȡ�����������
// 	int nType = nNumberId * 0.001;
// 
// 	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;
// 
// 	// ����Ƿ�����ȡ��ͬһ���ļ�����
// 	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
// 	{
// 		if (tBaseInfo.arActivationTakeRecord[i] == nNumberId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
// 			MAPLOG(ERROR_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, pszCode);
// 			return;
// 		}
// 	}
// 
// 	// ����б��Ƿ�����
// 	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]��������ȡ��¼�б�����!", __FUNCTION__);
// 		return;
// 	}
// 
// 	// ���˼�����
// 	if (2 == nType)
// 	{
// 		if (g_MapServer.IsActivationCodeRecord(pszCode))
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
// 			MAPLOG(ERROR_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, pszCode);
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
// 	MAPLOG(ERROR_LEVEL, "[%s]�����ȡ������[RoleId:%d,Code:%s,ItemId:%d]!", __FUNCTION__,
// 		m_pPlayer->GetRoleId(), pszCode, pRewardCfg->dwItemId);
}

// ��ȡ����
void CLogicManager::OnNetTakeActivityGift(const char *pszCode)
{
	// todo::�˴��߼��п�����һ�£��ö�Ӳ����
	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;

	char szCode[MAX_ACTIVATE_CODE_LEN * 2 + 1] = {0};
	memset(szCode, 0, sizeof(szCode));
	ChangeLocalChar(pszCode, szCode, sizeof(szCode)); // ��ֹ��������,��ת��

	// ����Ƿ���Ч�ļ�����
	const ActivationCode_Config *pCodeCfg = g_pCfgMgr->GetActivationCode(szCode);
	if (!pCodeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]�����벻����[code:%s]!", __FUNCTION__, szCode);
		return;
	}

	// ȡ��������
	const ActivationCodeReward_Config *pRewardCfg = g_pCfgMgr->GetActivationCodeReward(pCodeCfg->dwRewardId);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������뽱������[reward_id:%d]!", __FUNCTION__, pCodeCfg->dwRewardId);
		return;
	}

	tm tmNow;
	GetTMTime(&tmNow, time(NULL));

	// ��������Ƿ��ѹ���
	int nCurDate = (tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday;
	if (nCurDate > pRewardCfg->nValidDate)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�������ѹ���[code:%d]!", __FUNCTION__, szCode);
		return;
	}

	// ���ʱ���Ƿ��ѹ���
	int nCurTime = tmNow.tm_hour * 100 + tmNow.tm_min;
	int nValidTime = pRewardCfg->nValidTime ? pRewardCfg->nValidTime : 2359;  // �����0��ʾ24Сʱ����
	if (nCurDate == pRewardCfg->nValidDate && nCurTime > nValidTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�������ѹ���[code:%d]!", __FUNCTION__, szCode);
		return;
	}

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// ����б��Ƿ�����
	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��������ȡ��¼�б�����!", __FUNCTION__);
		return;
	}

	// ����Ƿ�����ȡ��ͬһ���ļ�����(�������ս�������Ʒ���ж�)
	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
	{
		if (tBaseInfo.arActivationTakeRecord[i] == pCodeCfg->dwRewardId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(ERROR_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, szCode);
			return;
		}
	}

	// ��¼����ȡ��
	tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = pCodeCfg->dwRewardId;

	// ������
	m_pPlayer->AddObject(pRewardCfg->dwItemId, 1, TRUE, FROM_ACTIVATION_CODE_RECORD);

	// ��Ӧ
	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
	msg.m_dwGiftId = pRewardCfg->dwItemId;
	m_pPlayer->SendPacket(&msg);

	// todo::�洢�����ݿ�
	

	MAPLOG(ERROR_LEVEL, "[%s]�����ȡ������[RoleId:%d,Code:%s,ItemId:%d]!", __FUNCTION__,
		m_pPlayer->GetRoleId(), szCode, pRewardCfg->dwItemId);
}

// ��ȡ�����
void CLogicManager::OnNetTakeActivityGift_New(const char *pszCode)
{
	// �����ȡ�Ƿ�Ƶ��(�ݶ�1��)
	DWORD dwCurTime = time(NULL);
	if (dwCurTime <= m_dwLastTakeActivityGiftTime + 1)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]Ƶ����ȡ���[code:%s]!", __FUNCTION__, pszCode);
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

// ��ȡ����
void CLogicManager::DoTakeActivityGift(const char *pszCode, bool bIsExist, DWORD dwLimitTime, DWORD dwRewardId, WORD wStatus)
{
	// ���������8λ: 0[����] 000[����] 0000[�����]

	WORD wFromMsgId = C2S_TAKE_ACTIVITY_GIFT_REQ;

	// ��⼤�����Ƿ�Ϸ�(id�����ɹ���)
	int nNumberId = GetNumberIdByActivationCode(pszCode);
	if (!nNumberId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]���������ò��Ϸ�[code:%d,RecordId:%d]!", __FUNCTION__, pszCode, nNumberId);
		return;
	}

	// ��⼤�����Ƿ���Ч(�˼������Ƿ����)
	if (!bIsExist)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]�����벻����[code:%s]!", __FUNCTION__, pszCode);
	}

	// ��ǰ����
	int nCurDate = g_Time2Date(time(NULL));

	// ��⼤�����Ƿ���� 20180223
	if (nCurDate > dwLimitTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]�������ѹ���[code:%d]!", __FUNCTION__, pszCode);
		return;
	}

	// ȡ�����������
	int nType = nNumberId * 0.001;

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// ����Ƿ�����ȡ��ͬһ���ļ�����
	for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
	{
		if (tBaseInfo.arActivationTakeRecord[i] == nNumberId)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, pszCode);
			return;
		}
	}

	// ����б��Ƿ�����
	if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(GUI_LEVEL, "[%s]��������ȡ��¼�б�����!", __FUNCTION__);
		return;
	}

	// ���˼�����
	if (2 == nType)
	{
		if (g_MapServer.IsActivationCodeRecord(pszCode))
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, pszCode);
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

// ��������
void CLogicManager::OnNetActivityNotice()
{
	MSG_S2C_ACTIVITY_NOTICE_RESP msg;
	msg.m_wNoticeId = g_GetConfigValue(126);
	m_pPlayer->SendPacket(&msg);
}

// ���볡��
void CLogicManager::OnNetEnterScene(WORD wSceneType, DWORD dwContext)
{
	// ��Ӹ�����س���
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetEnterScene(wSceneType, dwContext, m_pPlayer);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]û����ĳ�������[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// �뿪����
void CLogicManager::OnNetLeaveScene(WORD wSceneType)
{
	// ��Ӹ�����س���
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetLeaveScene(wSceneType,m_pPlayer);
	}
	else 
	{
		MAPLOG(ERROR_LEVEL, "[%s]û����ĳ�������[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// ����
void CLogicManager::OnNetGoTo(WORD wSceneType, MapPos tSrcPos, MapPos tDestPos)
{
	// ��Ӹ�������
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetGoto(wSceneType,m_pPlayer, tSrcPos, tDestPos);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]û�������������[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// ��Ʒ����
void CLogicManager::OnNetLockItem(WORD wSceneType, DWORD dwSceneItemUId)
{
	// ��Ӹ�������
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetLockItem(m_pPlayer, dwSceneItemUId);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]û�������������[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// ��Ʒʰȡ
void CLogicManager::OnNetPickUpItem(WORD wSceneType, DWORD dwSceneItemUId)
{
	// ��Ӹ�������
	if (wSceneType >= TEAM_HALL && wSceneType <= TEAM_BOSS)
	{
		CTeamDungeonManager::Instance()->OnNetPickUpItem(m_pPlayer, dwSceneItemUId);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]û�������������[type:%d]!", __FUNCTION__, wSceneType);
	}
}

// ������������
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

// ��ȡ�����ص�
void CLogicManager::OnTakeActivityGiftCallback(const char *pszCode, int nResult, int nStatus, DWORD dwRewardId, DWORD dwExpireTime, int nBatch, int nType)
{
	// ��ѯ���(0=���� 1=�����벻���� 2=���������� 3=����ȡ��ͬ��������)
	if (nResult)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]�����벻�����������[code:%s,batch:%d,result:%d]!", __FUNCTION__, pszCode, nBatch, nResult);
		return;
	}

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	// ͨ����
	if (nType)
	{
		// ����Ƿ�����ȡ��ͬһ���ļ����� 
		for (WORD i = 0; i < tBaseInfo.wActivationTakeRecordNum; ++i)
		{
			if (tBaseInfo.arActivationTakeRecord[i] == nBatch)
			{
				m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
				MAPLOG(GUI_LEVEL, "[%s]�˼������ѱ���ȡ��[code:%s]!", __FUNCTION__, pszCode);
				return;
			}
		}
	}
	// ������
	else
	{
		// ״̬(0=����ȡ 1=�ѱ���ȡ)
		if (nStatus)
		{
			m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_ALREADY_TAKE);
			MAPLOG(GUI_LEVEL, "[%s]�������ѱ���ȡ[code:%s]!", __FUNCTION__, pszCode);
			return;
		}
	}

	// �����Чʱ��
	if (time(NULL) > dwExpireTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_TIME_LIMIT);
		DATETIME curTime(dwExpireTime);
		char szCurTime[32] = { 0 };
		curTime.toString(szCurTime);
		MAPLOG(GUI_LEVEL, "[%s]�������ѹ���[code:%s,batch:%d,time:%s]!", __FUNCTION__, pszCode, nBatch, szCurTime);
		return;
	}

	// ȡ��������
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(dwRewardId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_ACTIVATION_CODE_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ��������뽱����Ʒ[code:%s,batch:%d,ItemId:%d]!", __FUNCTION__, pszCode, nBatch, dwRewardId);
		return;
	}

	// ������
	m_pPlayer->AddObject(dwRewardId, 1, TRUE, FROM_ACTIVATION_CODE_RECORD);

	// ��Ӧ
	MSG_S2C_TAKE_ACTIVITY_GIFT_RESP msg;
	msg.m_dwGiftId = dwRewardId;
	m_pPlayer->SendPacket(&msg);

	// ͨ����
	if (nType)
	{
		if (tBaseInfo.wActivationTakeRecordNum >= MAX_ACTIVATION_RECORD_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]ͨ�ü������¼�б�����[%d]!", __FUNCTION__, tBaseInfo.wActivationTakeRecordNum);
			return;
		}

		// ��¼����ȡ��
		tBaseInfo.arActivationTakeRecord[tBaseInfo.wActivationTakeRecordNum++] = nBatch;
	}
	// ������
	else
	{
		// ���¼���������
		ActivationCode_Update* pQuery = ActivationCode_Update::ALLOC();
		pQuery->SetBatch(nBatch);
		pQuery->SetActivationCode(pszCode);
		pQuery->SetRoleID(m_pPlayer->GetRoleId());
		pQuery->SetIndex(DB_ACTIVATION_CODE_UPDATE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// ����ؿ����ۼ�¼
void CLogicManager::OnNetMapCommentList(WORD dwChapter)
{
	SendMapCommentList(dwChapter);
}
// �ؿ�����
void CLogicManager::OnNetMapComment(WORD wChapter, const char *pszText)
{
	WORD wFromMsgId = C2S_MAP_COMMENT_REQ;

	// ������������Ƿ�Ϊ��
	if (!pszText || !pszText[0])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��������Ϊ��[map_id:%d]!", __FUNCTION__, wChapter);
		return;
	}

	// todo::�������������

	// ���˹ؿ��Ƿ��������
// 	const Map_Config *pMapCfg = g_pCfgMgr->GetMap(wChapter);
// 	if (!pMapCfg || !pMapCfg->byIsCanComment)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]�˹ؿ���������[map_id:%d]!", __FUNCTION__, wChapter);
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
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ���������[map_id:%d]!", __FUNCTION__, wChapter);
			return;
		}
	}

	WORD wMaxCommentId = 0;

	// ȡ������������
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

	// �������CD
	if (pComment && time(NULL) < pComment->dwTime + g_GetConfigValue(138))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�ؿ�������CD��...[map_id:%d]!", __FUNCTION__, wChapter);
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

	// ���������Ѵ�����
	if (pCommentList->size() > MAX_MAP_COMMENT_NUM)
	{
		// �õ�����ʱ������,�����������ٵ�
		vector<MapComment>::iterator iter_del = pCommentList->begin();
		for (iter = pCommentList->begin(); iter != pCommentList->end(); ++iter)
		{
			if (iter->wLikeTimes < iter_del->wLikeTimes ||  // ���޴����ٵ�
				(iter->wLikeTimes == iter_del->wLikeTimes && iter->dwTime < iter_del->dwTime)) // ���޴�����ͬ,��ʱ�������
				iter_del = iter;
		}
		pCommentList->erase(iter_del);
	}

	// ������Ӧ
	MSG_S2C_MAP_COMMENT_RESP msg;
	msg.m_wMapId = wChapter;
	msg.m_wCommentId = tComment.wCommentId;
	msg.m_dwTime = tComment.dwTime;
	strcpy_s(msg.m_szText, sizeof(msg.m_szText), pszText);
	m_pPlayer->SendPacket(&msg);

	// �洢�������ݿ�
	g_MapServer.SaveMapCommentData(m_pPlayer->GetServerId(), wChapter);
}

// �ؿ����۵���
void CLogicManager::OnNetMapCommentLike(WORD wChapter, WORD wCommentId)
{
	WORD wFromMsgId = C2S_MAP_COMMENT_LIKE_REQ;

	vector<MapComment> *pCommentList = g_MapServer.GetMapCommentList(m_pPlayer->GetServerId(), wChapter);
	if (!pCommentList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�˹ؿ���ľ������[map_id:%d]!", __FUNCTION__, wChapter);
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
		MAPLOG(ERROR_LEVEL, "[%s]������ľ�д���[map_id:%d,comment_id:%d]!", __FUNCTION__, wChapter, wCommentId);
		return;
	}

	// ���ܸ��Լ�����
	if (m_pPlayer->GetRoleId() == pComment->dwRoleId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]���ܸ��Լ�����[map_id:%d,comment_id:%d]!", __FUNCTION__, wChapter, wCommentId);
		return;
	}

	// ����Ƿ������۹�
	for (WORD i = 0; i < pComment->wLikePlayerNum; ++i)
	{
		if (pComment->arLikePlayerList[i] == m_pPlayer->GetRoleId())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]���Ѿ����޹�������[map_id:%d,comment_id:%d,role_id:%d]!",
				__FUNCTION__, wChapter, wCommentId, m_pPlayer->GetRoleId());
			return;
		}
	}

	// ��ӵ��޴���
	pComment->wLikeTimes++;

	// ��¼�ѵ���
	if (pComment->wLikePlayerNum < MAX_MAP_COMMENT_LIKE_NUM)
	{
		pComment->arLikePlayerList[pComment->wLikePlayerNum++] = m_pPlayer->GetRoleId();
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ؿ����۵����б�����[Chapter:%d,comment_id:%d,role_id:%d]!",
			__FUNCTION__, wChapter, wCommentId, m_pPlayer->GetRoleId());
	}

	MSG_S2C_MAP_COMMENT_LIKE_RESP msg;
	msg.m_wCommentId = wCommentId;
	msg.m_wMapId = wChapter;
	m_pPlayer->SendPacket(&msg);

	// �洢�������ݿ�(todo::�˴�Ҫ�Ż�)
	g_MapServer.SaveMapCommentData(m_pPlayer->GetServerId(), wChapter);
}

// ���������б�
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

	// �����λ
void CLogicManager::OnNetAcivateNobility()
{
	WORD wBobilityLevel = m_pPlayer->GetProperty(ROLE_PRO_NOBILITY_LEVEL);

	// ׼��Ҫ����ĵȼ�
	wBobilityLevel++;

	// ���ȼ��Ƿ��Ѵ�����
	const Nobility_Config *pNobilityCfg = g_pCfgMgr->GetBobility(wBobilityLevel);
	if (!pNobilityCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����λ���û��Ѵ�����[level:%d]!", __FUNCTION__, wBobilityLevel);
		return;
	}

	// ����Ƿ��Ѵ�����
// 	if (!pNobilityCfg->dwNeedPower)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��λ�Ѵ�����[max_level:%d]!", __FUNCTION__, wBobilityLevel);
// 		return;
// 	}

	// �����Ҫ��ս��
	if (m_pPlayer->GetFightingPower() < pNobilityCfg->dwNeedPower)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����λ��Ҫ��ս������[need_power:%d]!", __FUNCTION__, pNobilityCfg->dwNeedPower);
		return;
	}

	m_pPlayer->SetProperty(ROLE_PRO_NOBILITY_LEVEL, wBobilityLevel);

	// ��Ӧ
	MSG_S2C_NOBILITY_ACTIVATE_RESP msg;
	msg.m_wNobilityLevel = wBobilityLevel;
	m_pPlayer->SendPacket(&msg);

	// ����ս��
	m_pPlayer->UpdateFightingPower();
}

// Ť���齱
void CLogicManager::OnNetNDLottery(WORD wTimes, BYTE byIsCostItem)
{
	// ����Ƿ���
	DWORD dwActivityDataId = m_pPlayer->GetActivityManager()->GetActivityDataId(131);
	if (!dwActivityDataId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�齱�û�п�����[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	// ���齱�����ĺϷ���
	if (!wTimes || wTimes > MAX_ND_LOTTERY_TIMES)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�齱�������Ϸ�[times:%d]!", __FUNCTION__, wTimes);
		return;
	}

	const NDLotteryCost_Config *pLotteryCostCfg = g_pCfgMgr->GetNDLotteryCost(dwActivityDataId);
	if (!pLotteryCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ť���齱��������[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	int nCostDiamond = 0;

	// ��������
	if (byIsCostItem)
	{
		// ������ĵ���Ʒ
		if (m_pPlayer->GetItemNum(pLotteryCostCfg->dwCostItemId) < wTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�齱���ĵ���Ʒ����[ItemId:%d]!", __FUNCTION__, pLotteryCostCfg->dwCostItemId);
			return;
		}
	}
	// ��ʯ����
	else
	{
		nCostDiamond = wTimes > 1 ? pLotteryCostCfg->nOenKeyCostDiamond : pLotteryCostCfg->nSingleCostDiamond;
	}

	// ȡ�齱�б�
	const vector<NDLottery_Config> *pLotteryList = g_pCfgMgr->GetNDLotteryList(dwActivityDataId);
	if (!pLotteryList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ť���齱�б�[ActivityDataId:%d]!", __FUNCTION__, dwActivityDataId);
		return;
	}

	// �����ܵ�Ȩֵ
	int nTotalWeight = 0;
	vector<NDLottery_Config>::const_iterator iter = pLotteryList->begin();
	for (; iter != pLotteryList->end(); ++iter)
	{
		nTotalWeight += iter->nWeight;
	}

	// ������е��߼�
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

				// �Ƿ���Ҫ��¼
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

	// ��������
	if (byIsCostItem)
	{
		// �۳����ĵ���Ʒ
		m_pPlayer->RemoveItem(pLotteryCostCfg->dwCostItemId, wTimes, FROM_ND_LOTTERY_REWARD);
	}
	// ��ʯ����
	else
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ND_LOTTERY_REWARD);
	}

	// ��ӳ��е���Ʒ
	m_pPlayer->AddObjectList(vItemList, TRUE, FROM_ND_LOTTERY_REWARD);

	// ��Ӧ
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

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 37, wTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 37, wTimes, true);
}

// ����齱��¼
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

	// �������
	for (WORD i = 0; i < tRoleInfo.wGuideNum; ++i)
	{
		if (tRoleInfo.arGuideList[i].nKey == nKey)
		{
			tRoleInfo.arGuideList[i].nValue = nValue;
			bAdd = false;
		}
	}

	// �������
	if (bAdd)
	{
		if (tRoleInfo.wGuideNum >= MAX_GUIDE_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_GUIDE_UPDATE_REQ);;
			MAPLOG(GUI_LEVEL, "[%s]���������б�����[num:%d]", __FUNCTION__, tRoleInfo.wGuideNum);
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

