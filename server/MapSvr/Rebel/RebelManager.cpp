#include "stdafx.h"
#include "RebelManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "ToolKit\Singleton.h"
#include "..\Rank\RankManager.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\RedPoint\RedPointManager.h"

CRebelManager::CRebelManager()
{
	m_dwSeriesId = 1;
	m_lProcessTime = GetTickCount64();
}

CRebelManager::~CRebelManager()
{

}

void CRebelManager::Init()
{
	m_dwSeriesId = 1;
	m_nRebelShareMax = 3;
	m_lProcessTime = GetTickCount64();
}

void CRebelManager::Update()
{
	if (!m_RebelList.size())
		return;

	DWORD curTime = time(NULL);
	REBEL_MAP::iterator iter = m_RebelList.begin();
	for (; iter != m_RebelList.end(); ++iter)
	{
		RebelData *pRebel = &iter->second;
		if (!pRebel->byIsRemove && curTime >= pRebel->dwCreateTime + pRebel->dwLiveTime && !pRebel->IsDead())
		{
			RemoveRebelData(pRebel->dwRebelId);
			break;
		}

		// ���ʱ���Ѿ����ܾõ�(10����)
		if (pRebel->byIsRemove && curTime >= pRebel->dwCreateTime + pRebel->dwLiveTime + 60 * 1)
		{
			m_RebelList.erase(iter);
			break;
		}
	}

	// todo::���ڵĸ����Ѿ��б�
}

// �����Ѿ����� 
void CRebelManager::OnRebelDataReq(Player *pPlayer)
{
	SendRebelData(pPlayer);
}

// ��ȡ�Ѿ���ѫ����
void CRebelManager::OnRebelAwardReq(Player *pPlayer, WORD *pAwardList, BYTE byAwardNum)
{
	WORD wFromMsgId = C2S_REBEL_EXPLOIT_AWARD_REQ;

	if (!pAwardList || !byAwardNum)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ�Ѿ���ѫ�����������Ϸ�", __FUNCTION__);
		return;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	WORD wInitRoleLevel = pPlayer->GetDailyInitLevel();

	const RoleRebel_Config *pRoleRebelCfg = CConfigManager::Instance()->GetRoleRebel(wInitRoleLevel);
	if (!pRoleRebelCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫ���Ѿ���������[Level:%d]", __FUNCTION__, wInitRoleLevel);
		return;
	}

	MSG_S2C_REBEL_EXPLOIT_AWARD_RESP msg;
	msg.m_wAwardNum = 0;

	for (BYTE i = 0; i < byAwardNum; ++i)
	{
		WORD wAwardId = pAwardList[i];
		const RebelExploit_Config *pExploitCfg = CConfigManager::Instance()->GetRebelExploit(pRoleRebelCfg->wAwardListId, wAwardId);
		if (!pExploitCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ���ѫ����[AwardListId:%d, AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			continue;
		}

		// ��⹦ѫ�Ƿ��㹻
		if (pPlayerRebelData->dwTodayExploit < pExploitCfg->nNeedExploit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ѫ����,������ȡ����[AwardListId:%d, AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			continue;
		}

		// ����Ƿ�����ȡ��
		if (pPlayerRebelData->IsInAwardRecordList(wAwardId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ѫ��������ȡ��[AwardListId:%d,AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			return;
		}

		// ��Ʒ����
		if (pExploitCfg->wAwardItemId)
		{
			if (!pPlayer->AddObject(pExploitCfg->wAwardItemId, pExploitCfg->wAwardItemNum, TRUE, FROM_REBEL_REWARD))
			{
				MAPLOG(ERROR_LEVEL, "[%s]��ӹ�ѫ������Ʒʧ��[AwardId:%d,AwardNum:%d]",
					__FUNCTION__, pExploitCfg->wAwardItemId, pExploitCfg->wAwardItemNum);
				continue;
			}
		}
		// ��ҽ���
		else
		{
			pPlayer->ModifyProperty(ROLE_PRO_GOLD, pExploitCfg->nAwardGold, TRUE, FROM_REBEL_REWARD);
		}


		// ��¼�Ѿ���ȡ���Ľ���
		pPlayerRebelData->AddAwardRecord(wAwardId);

		if (msg.m_wAwardNum < MAX_EXPLOIT_AWARD_NUM)
			msg.m_AwardList[msg.m_wAwardNum++] = wAwardId;
	}

	pPlayer->SendPacket(&msg);
}

// ����ѫ������¼
void CRebelManager::OnNetAwardRecordList(Player *pPlayer)
{
	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	MSG_S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP msg;
	msg.m_byAwardNum = pPlayerRebelData->wAwardRecordNum;

	int nCopySize = sizeof(WORD)* pPlayerRebelData->wAwardRecordNum;
	memcpy_s(msg.m_AwardList, nCopySize, pPlayerRebelData->arAwardRecordList, nCopySize);

	pPlayer->SendPacket(&msg);
}

// �����Ѿ�
void CRebelManager::OnNetRebelShare(Player *pPlayer)
{
	WORD wFromMsgId = C2S_REBEL_SHARE_REQ;

	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	if (!dwRebelId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ҵ�ǰľ���Ѿ�������ʧ��[RebelId:%d]",
			__FUNCTION__, dwRebelId);
		return;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	// ����Ƿ��Ѿ������
	if (pPlayerRebelData->wIsAlreadyShare)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ��Ѿ������[RebelId:%d]",
			__FUNCTION__, pPlayerRebelData->dwTodayDamage);
		return;
	}

	CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
	CHECK_POINTER(pFriendMgr);

	BYTE byFriendNum = pFriendMgr->GetFriendNum();
	const FriendData *pFrendList = pFriendMgr->GetFrindList();

	for (BYTE i = 0; i < byFriendNum; ++i)
	{
		const FriendData &tFriendData = pFrendList[i];
		AddShareData(tFriendData.dwFriendId, dwRebelId);

		// ���Ŀ��������֪ͨ
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(tFriendData.dwFriendId);
		if (pPlayer && pPlayer->GetRedPointManager())
			pPlayer->GetRedPointManager()->OnRedPointNotify(SYSTEM_REBEL);
	}

	pPlayerRebelData->wIsAlreadyShare = true;

	MSG_S2C_REBEL_SHARE_RESP msg;
	pPlayer->SendPacket(&msg);

	// �ճ�����
	pPlayer->OnUpdateDailyMission(17);
}

// ��ʼս��
bool CRebelManager::OnBattleStart(Player *pPlayer, DWORD dwRebelId, WORD wAttackType)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�������[RebelId:%d]",__FUNCTION__, dwRebelId);
		return false;
	}

	const RebelBattle_Config *pRebelBattleCfg = CConfigManager::Instance()->GetRebelBattle(pRebelData->wLevel);
	if (!pRebelBattleCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ��ȼ�����[RebelLv:%d]", __FUNCTION__, pRebelData->wLevel);
		return false;
	}

	// �Ƿ�������
	if (pRebelData->IsDead())
	{
		SendRebelData(pPlayer); // �������������������Ѿ���ͬ��
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ��Ѿ��ҵ�[RebelId:%d]", __FUNCTION__, dwRebelId);
		return false;
	}

	int nCostWanted = 0;

	// ��ͨ����
	if (1 == wAttackType)
	{
		nCostWanted = g_GetConfigValue(38);
	}
	// ȫ��һ��
	else
	{
		nCostWanted = g_GetConfigValue(39);
		if (IsCostHalveActivity())
			nCostWanted *= 0.5;
	}

	// ���ͨ����
	if (pPlayer->GetProperty(ROLE_PRO_WANTED) < nCostWanted)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ͨ�����,����ս��[AttackType:%d]", __FUNCTION__, wAttackType);
		return false;
	}

	// ��ӹ�������
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	for (size_t i = 0; i < pRebelData->vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pRebelData->vMonsterList[i]);
	}

	pFBMgr->SetBattleInfo(0, wAttackType, pRebelData->wMapId);
	pFBMgr->SetTargetId(dwRebelId);

	return true;
}

// ս������
bool CRebelManager::OnBattleOver(Player *pPlayer, DWORD dwRebelId, WORD wAttackType, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	// todo::��Ϊ����ʹ��
// 	MAPLOG(ERROR_LEVEL, "��ʼ����˺�......");
// 	for (BYTE i = 0; i < byMonsterNum; ++i)
// 	{
// 		MonsterData *pMonster = &pMonsterList[i];
// 		MAPLOG(ERROR_LEVEL, "�����˺���Ϣ[Index:%d,Damage:%d]", pMonster->byIndex, pMonster->dwDamage);
// 	}
// 	MAPLOG(ERROR_LEVEL, "��������˺�......");


	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�������[RebelId:%d]", __FUNCTION__, dwRebelId);
		return false;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	int nCostWanted = 0;

	// ��ͨ����
	if (1 == wAttackType)
	{
		nCostWanted = g_GetConfigValue(38);
	}
	// ȫ��һ��
	else
	{
		nCostWanted = g_GetConfigValue(39);
		if (IsCostHalveActivity())
			nCostWanted *= 0.5;
	}

	// ���ս��=���ĵ�������*50
	int nRebelValue = nCostWanted * 50;
	
	// ��ľ������
	if (!pRebelData->IsDead())
	{
		// �����˺���¼
		UpdateRebelDamage(dwRebelId, pMonsterList, byMonsterNum);

		if (pRebelData->IsDead())
		{
			// �Լ����Ѿ�
			if (pRebelData->dwRoleId == pPlayer->GetCharID())
			{
				pPlayerRebelData->wRebelLevel++;
			}
			// ���ѵ��Ѿ�
			else
			{
				Player *pDiscoverer = PlayerManager::Instance()->FindPlayerByCharID(pRebelData->dwRoleId);
				if (pDiscoverer)
					pDiscoverer->ModifyProperty(ROLE_PRO_REBEL_LEVEL, 1, false);
			}

			// ��ɱ�߽���
			RewardData tKillReward;
			tKillReward.wInfoId = 2;
			tKillReward.dwTime = time(NULL);
			tKillReward.dwValue1 = pRebelData->wMapId;
			tKillReward.dwValue2 = pRebelData->byQuality;
			pPlayer->GetRewardCenterManager()->AddReward(tKillReward);

			// �����߽���
			RewardData tDiscoverReward;
			tDiscoverReward.wInfoId = 3;
			tDiscoverReward.dwTime = time(NULL);
			tDiscoverReward.dwValue1 = pRebelData->wMapId;
			tDiscoverReward.dwValue2 = pRebelData->byQuality;
			//strcpy_s(tDiscoverReward.szText, sizeof(tDiscoverReward.szText) - 1, pPlayer->GetRoleName());
			CRewardCenterManager::AddReward(pPlayer->GetServerId(), pRebelData->dwRoleId, tDiscoverReward);

			RemoveRebelData(dwRebelId);
		}
	}

	pPlayer->ModifyProperty(ROLE_PRO_WANTED, -nCostWanted);

	DWORD dwTotalDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		dwTotalDamage += pMonsterList[i].dwDamage;
	}
	pPlayerRebelData->dwTodayDamage += dwTotalDamage;
	
	DWORD dwTodayExploit = 0;

	// ��ù�ѫ=(��������˺�/1000)����ȡ��
	if (dwTotalDamage)
		dwTodayExploit = ceil(double(dwTotalDamage * 0.001));

	// ��ڼ�˫������
	if (IsDoubleAwardActivity())
		dwTodayExploit = dwTodayExploit * 2;

	pPlayerRebelData->dwTodayExploit += dwTodayExploit;

	pPlayer->ModifyProperty(ROLE_PRO_REBEL_VALUE, nRebelValue);

	WORD wOldDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	WORD wOldExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);

	// �������а�
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_REBEL_DAMAGE, pPlayer->GetRoleId());
	if (!pRankData || dwTotalDamage > pRankData->dwValue)	
		CRankManager::Instance()->AddRoleRankData(RANK_REBEL_DAMAGE, pPlayer, dwTotalDamage, 0, 0, 200);  // �˺����ǵ�������˺�
	CRankManager::Instance()->AddRoleRankData(RANK_REBEL_EXPLOIT, pPlayer, pPlayerRebelData->dwTodayExploit,0,0, 200);

	WORD wNewDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	WORD wNewExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);

	// todo::��ʱ��������
	CFubenManager *pMapMgr = pPlayer->GetFubenManager();
	if (pMapMgr)
	{
		BYTE byIndex = 0;
		pMapMgr->SetParam(byIndex++, wOldDamageRank);
		pMapMgr->SetParam(byIndex++, wOldExploitRank);
		pMapMgr->SetParam(byIndex++, wNewDamageRank);
		pMapMgr->SetParam(byIndex++, wNewExploitRank);

		pMapMgr->SetParam(byIndex++, nRebelValue);
	}

	// �ճ�����
	pPlayer->OnUpdateDailyMission(18);

	// �ɾ��¼�
	pPlayer->OnUpdateAchievement(16);

	// ���ջ(�ۼƹ����Ѿ�����)
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 16, 1, true);

	// ���ջ�ԾĿ��(�ۼƹ����Ѿ�����)
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 16, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 16, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 16, 1, true);

	// �������ˢս��
	if (dwTodayExploit)
		pPlayer->OnUpdateSevenDayTarget(10, dwTodayExploit, true);

	return true;
}

// ����Ѿ�
DWORD CRebelManager::CreateRebel(Player *pPlayer)
{
	// ����Ƿ񿪷�
	if (g_pCfgMgr->GetSystemOpenLevel(SYSTEM_REBEL) > pPlayer->GetLevel())
		return 0;

	// ��ǰ�����Ѿ�
	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	if (dwRebelId)
		return 0;

	int nRate = CConfigManager::Instance()->GetServerCfg(29);
	if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < nRate)
	{
		PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

		WORD wRebelLevel = pPlayerRebelData->wRebelLevel;

		// todo::��ʱ�����������鵽BUG�Ժ���ȥ��
// 		if (!pPlayerRebelData->wRebelLevel)
// 			pPlayerRebelData->wRebelLevel = 1;
		if (!wRebelLevel)
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ���ǰ�ȼ�[Level:%d]", __FUNCTION__, wRebelLevel);

		WORD wDailyInitRoleLevel = pPlayer->GetDailyInitLevel();

		const RoleRebel_Config *pRoleRebelCfg = CConfigManager::Instance()->GetRoleRebel(wDailyInitRoleLevel);
		if (!pRoleRebelCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫ���Ѿ���������[Level:%d]", __FUNCTION__, wDailyInitRoleLevel);
			return 0;
		}

		// ����Ѿ�Ʒ��
		int byRandValue = Rand::Instance()->irand(0, pRoleRebelCfg->GetTotalRate());
		BYTE byRandQuality = pRoleRebelCfg->GetQuality(byRandValue);
		if (QUALITY_INVALID == byRandQuality)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�Ʒ�ʼ�������[rate:%d]", __FUNCTION__, byRandValue);
			return 0;
		}

		// �Ѿ�Ʒ������
		const RebelQuality_Config *pQualityCfg = CConfigManager::Instance()->GetRebelQuality(byRandQuality);
		if (!pQualityCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�Ʒ������[quality:%d]", __FUNCTION__, byRandQuality);
			return 0;
		}

		// ����ؿ�
		const RebelBattle_Config *pBattleCfg = CConfigManager::Instance()->GetRebelBattle(wRebelLevel);
		if (!pBattleCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�ս������[RebelLv:%d]", __FUNCTION__, wRebelLevel);
			return 0;
		}
		if (!pBattleCfg->vRandMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�����ؿ��б�Ϊ��[RebelLv:%d]", __FUNCTION__, wRebelLevel);
			return 0;
		}
		WORD wMapId = pBattleCfg->vRandMapList[g_Rand(1, pBattleCfg->vRandMapList.size())-1];

		dwRebelId = m_dwSeriesId++;

		RebelData tRebelData;
		tRebelData.dwRebelId = dwRebelId;
		tRebelData.wLevel = wRebelLevel;
		tRebelData.byQuality = byRandQuality;
		tRebelData.dwRoleId = pPlayer->GetCharID();
		strcpy_s(tRebelData.szRoleName, sizeof(tRebelData.szRoleName) - 1, pPlayer->GetRoleName());
		tRebelData.dwCreateTime = time(NULL);
		//tRebelData.isDead = false;
		tRebelData.dwLiveTime = pQualityCfg->dwLiveTime * 60;
		tRebelData.wMapId = wMapId;

		// ȡ�ؿ���������
		if (!g_pCfgMgr->GetMapMonsterList(wMapId, &tRebelData.vMonsterList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ�����Ѿ��ؿ��Ĺ����б�[MapId:%d]", __FUNCTION__, wMapId);
			return 0;
		}

		// Ʒ��Ӱ���
		float fBloodIncRate = 0;
		for (size_t i = 0; i < pQualityCfg->vProList.size(); ++i)
		{
			if (HERO_PRO_BLOOD == pQualityCfg->vProList[i].wType)
			{
				fBloodIncRate = pQualityCfg->vProList[i].nValue;
				break;
			}
		}
		// ս��Ӱ���
		float fBloodRate = 0;
		for (size_t i = 0; i < pBattleCfg->vAddProList.size(); ++i)
		{
			if (HERO_PRO_BLOOD_RATE == pBattleCfg->vAddProList[i].wType)
			{
				fBloodRate = pBattleCfg->vAddProList[i].nValue;
				break;
			}
		}
		// Ѫ������ʽ����
		for (size_t i = 0; i < tRebelData.vMonsterList.size(); ++i)
		{
			DWORD &dwTotalHP = tRebelData.vMonsterList[i].dwTotalHP;
			dwTotalHP *= (1 + fBloodRate * PROPERTY_DECREASE_MULTIPLE);
			dwTotalHP *= (1 + fBloodIncRate * PROPERTY_DECREASE_MULTIPLE);
		}

		AddRebelData(tRebelData);

		pPlayerRebelData->wIsAlreadyShare = false;

		MSG_S2C_REBEL_DISCOVER_NOTIFY msg;
		msg.m_byQaulity = byRandQuality;
		msg.m_wLevel = wRebelLevel;
		msg.m_wMapId = wMapId;
		msg.m_dwRebelId = dwRebelId;
		pPlayer->SendPacket(&msg);

		MAPLOG(GUI_LEVEL, "[%s]�����Ѿ�[RebleId:%d]", __FUNCTION__, dwRebelId);

		// ���ͺ����ʾ
		if (pPlayer->GetRedPointManager())
			pPlayer->GetRedPointManager()->OnRedPointNotify(SYSTEM_REBEL);
	}

	return dwRebelId;
}

// ����Ѿ�����
void CRebelManager::AddRebelData(const RebelData &tRebelData)
{
	if (GetSelfRebelId(tRebelData.dwRoleId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ�����Ѿ�����,���ʧ��[RoleId:%d]", __FUNCTION__, tRebelData.dwRoleId);
		return;
	}

	m_RebelList.insert(make_pair(tRebelData.dwRebelId, tRebelData));

	m_RebelDiscoverList.insert(make_pair(tRebelData.dwRoleId, tRebelData.dwRebelId));
}

// ��ӷ�������
void CRebelManager::AddShareData(DWORD dwShareTargetId, DWORD dwRebelId)
{
	RebelData *pRebel = GetRebelData(dwRebelId);
	if (!pRebel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�����[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	// ��¼�Ѿ������ȥ��
	pRebel->vSharePlayerList.push_back(dwShareTargetId);

	// ��¼����յ��ķ���
	vector<DWORD> *pRebelList = GetShareRebelList(dwShareTargetId);
	if (pRebelList)
	{
		pRebelList->push_back(dwRebelId);
	}
	else
	{
		vector<DWORD> RebelList;
		RebelList.push_back(dwRebelId);
		m_RebelShareList.insert(make_pair(dwShareTargetId, RebelList));
	}
}

// ɾ����������
void CRebelManager::RemoveShareData(DWORD dwShareTargetId, DWORD dwRebelId)
{
	vector<DWORD> *pRebelList = GetShareRebelList(dwShareTargetId);
	if (!pRebelList)
		return;

	for (vector<DWORD>::iterator iter = pRebelList->begin(); iter != pRebelList->end(); ++iter)
	{
		if (*iter == dwRebelId)
		{
			pRebelList->erase(iter);
			break;
		}
	}
}

// ɾ���Ѿ�����
void CRebelManager::RemoveRebelData(DWORD dwRebelId)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData/* && pRebelData->IsDead()*/)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ��Ѳ�����[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	// ɾ�������ߵ�����
	m_RebelDiscoverList.erase(pRebelData->dwRoleId);

	// ɾ�������ȥ������
	for (int i = 0; i < pRebelData->vSharePlayerList.size(); ++i)
	{
		RemoveShareData(pRebelData->vSharePlayerList[i], dwRebelId);
	}

	pRebelData->byIsRemove = true;
}

// ȡ�Ѿ�����
RebelData* CRebelManager::GetRebelData(DWORD dwRebelId)
{
	REBEL_MAP::iterator iter = m_RebelList.find(dwRebelId);
	return iter != m_RebelList.end() ? &iter->second : NULL;
}

// ȡ�Լ����ֵ��Ѿ�ID
DWORD CRebelManager::GetSelfRebelId(DWORD dwRoleId)
{
	REBEL_DISCOVER_MAP::iterator iter =  m_RebelDiscoverList.find(dwRoleId);
	return iter != m_RebelDiscoverList.end() ? iter->second : NULL;
}

// �����Ѿ�����
void CRebelManager::SendRebelData(Player *pPlayer)
{
	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	MSG_S2C_REBEL_DATA_RESP msg;
	memset(&msg.m_RebelData, 0, sizeof(msg.m_RebelData));
	msg.m_RebelData.wExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_REBEL_DAMAGE, pPlayer->GetRoleId());
	//msg.m_RebelData.wDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	msg.m_RebelData.wDamageRank = pRankData ? pRankData->wRank : 0;
	msg.m_RebelData.dwTodayDamage = pRankData ? pRankData->dwValue : 0;
	msg.m_RebelData.dwTodayExploit = pPlayerRebelData->dwTodayExploit;
	//msg.m_RebelData.dwTodayDamage = pPlayerRebelData->dwTodayDamage;
	msg.m_RebelData.byIsAlreadyShare = pPlayerRebelData->wIsAlreadyShare;
	msg.m_RebelData.wInitRoleLevel = pPlayer->GetDailyInitLevel();

	// �Լ����ֵ��Ѿ�
	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	RebelData *pRebelData = dwRebelId ? GetRebelData(dwRebelId) : NULL;
	if (pRebelData)
	{
		ClientRebelData tRebelData;
		tRebelData.Copy(*pRebelData);
		msg.m_RebelData.AddRebelData(tRebelData);
	}

	// ���˷�����Ѿ�
	vector<DWORD>* pRebelIdList = GetShareRebelList(pPlayer->GetCharID());
	if (pRebelIdList)
	{
		vector<DWORD>::iterator iter = pRebelIdList->begin();
		for (; iter != pRebelIdList->end(); ++iter)
		{
			pRebelData = GetRebelData(*iter);
			if (!pRebelData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]���Ҳ������˷�����Ѿ�����[RebelId:%d]", __FUNCTION__, *iter);
				continue;
			}

			ClientRebelData tRebelData;
			tRebelData.Copy(*pRebelData);
			msg.m_RebelData.AddRebelData(tRebelData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// �Ƿ��к��
bool CRebelManager::IsHaveRedPoint(Player *pPlayer)
{
	// ���Ѿ��������пɻ�ɱ���Ѿ�ʱ(�Լ����ֵ�)
	if (GetSelfRebelId(pPlayer->GetRoleId()))
		return true;

	// ���Ѿ��������пɻ�ɱ���Ѿ�ʱ(���ѷ����)
	vector<DWORD> *pFriendRebelList = GetShareRebelList(pPlayer->GetRoleId());
	if (pFriendRebelList && pFriendRebelList->size())
		return true;

	// ���Ѿ����������й�ѫ��������ȡʱ
	const PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;
	if (!pPlayerRebelData->dwTodayExploit)
		return false;
	WORD wInitRoleLevel = pPlayer->GetDailyInitLevel();
	const RoleRebel_Config *pRoleRebelCfg = g_pCfgMgr->GetRoleRebel(wInitRoleLevel);
	if (!pRoleRebelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ɫ���Ѿ���������[Level:%d]", __FUNCTION__, wInitRoleLevel);
		return false;
	}
	const map<DWORD, RebelExploit_Config> &ExpoitCfgList = g_pCfgMgr->GetRebelExpoitList();
	map<DWORD, RebelExploit_Config>::const_iterator iter = ExpoitCfgList.begin();
	for (; iter != ExpoitCfgList.end(); ++iter)
	{
		if (iter->second.dwListId == pRoleRebelCfg->wAwardListId &&				// ͬһ���б�
			iter->second.nNeedExploit <= pPlayerRebelData->dwTodayExploit &&	// ��ѫ����
			!pPlayerRebelData->IsInAwardRecordList(iter->second.dwAwardId))		// ��ľ��ȡ��
			return true;
	}

	return false;
}

// ȡ���˷�����Լ����Ѿ��б�
vector<DWORD>* CRebelManager::GetShareRebelList(DWORD dwRoleId)
{
	REBEL_SHARE_MAP::iterator iter = m_RebelShareList.find(dwRoleId);
	return iter != m_RebelShareList.end() ? &iter->second : NULL;
}

// �Ƿ����ļ���
bool CRebelManager::IsCostHalveActivity()
{
	int nConfigKey = 36;
	const Server_Config *pServerCfg = CConfigManager::Instance()->GetCommonCfg(nConfigKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ����������[key:%d]", __FUNCTION__, nConfigKey);
		return false;
	}

	if (pServerCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȫ��һ�����ļ���ʱ�����ó���", __FUNCTION__);
		return false;
	}

	int nBeginTime = pServerCfg->vValueList[0];
	int nEndTime = pServerCfg->vValueList[1];

	DATETIME datetime(time(NULL));
	int nCurTime = datetime.hour * 100 + datetime.minute;

	return nCurTime >= nBeginTime && nCurTime <= nEndTime ? true : false;
}

// �Ƿ��������
bool CRebelManager::IsDoubleAwardActivity()
{
	int nConfigKey = 37;
	const Server_Config *pServerCfg = CConfigManager::Instance()->GetCommonCfg(nConfigKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ����������[key:%d]", __FUNCTION__, nConfigKey);
		return false;
	}

	if (pServerCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����Ѿ���ѫ����ʱ�����ó���", __FUNCTION__);
		return false;
	}

	int nBeginTime = pServerCfg->vValueList[0];
	int nEndTime = pServerCfg->vValueList[1];

	DATETIME datetime(time(NULL));
	int nCurTime = datetime.hour * 100 + datetime.minute;


	return nCurTime >= nBeginTime && nCurTime <= nEndTime ? true : false;
}

// �����Ѿ��˺�
void CRebelManager::UpdateRebelDamage(DWORD dwRebelId, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�������[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		MonsterData &tMonster = pMonsterList[i];
		if (tMonster.byIndex >= MAX_BATTLE_MONSTER_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����վλ����![pos:%d]", __FUNCTION__, tMonster.byIndex);
			continue;
		}
		for (size_t k = 0; k < pRebelData->vMonsterList.size(); ++k)
		{
			if (pRebelData->vMonsterList[k].byIndex == tMonster.byIndex)
			{
				pRebelData->vMonsterList[k].dwDamage += tMonster.dwDamage;
				break;
			}
		}
	}
}