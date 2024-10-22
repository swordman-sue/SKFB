#include "stdafx.h"
#include "RewardCenterManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Arena\ArenaManager.h"
#include "..\PlayerManager.h"
#include "..\Title\TitleManager.h"
#include "..\ResourceWar\ResourceWarManager.h"
#include "CommonLogic.h"
#include "..\Recharge\RechargeManager.h"

// �콱���Ľ�������(�ɹ���)
enum
{
	REWARD_CENTER_ARENA_RANK = 1,			// 1=��������������	�Ѵ���
	REWARD_CENTER_KILL_RABEL,				// 2=��ɱ�Ѿ�
	REWARD_CENTER_DICOVER,					// 3=���ֵ��Ѿ�����ɱ
	REWARD_CENTER_WORLD_BOSS_HONOR_RANK,	// 4=����BOSS������������
	REWARD_CENTER_WORLD_BOSS_DAMAGE_RANK,	// 5=����BOSS�˺���������

	REWARD_CENTER_GM,						// 6=GM���Ž���
	REWARD_CENTER_REBEL_DAMAGE_RANK,		// 7=�Ѿ��˺����н���
	REWARD_CENTER_REBEL_EXPOIT_RANK,		// 8=�Ѿ���ѫ���н���
	REWARD_CENTER_HELP_BATTLE,				// 9=��ս����
	REWARD_CENTER_PVP_RANK,					// 10=PVP���н���
	REWARD_CENTER_ATTACK_CITY_RANK,			// 11=�������н���
	REWARD_CENTER_ENDLESS_LAND_RANK,		// 12=�޾�֮�����н���
	REWARD_CENTER_SEVEN_DAY_RANK,			// 13=7�����н���
	REWARD_CENTER_CLOSED_TEST_REBATE,		// 14=����ֵ��ʯ����
	REWARD_CENTER_RANK,						// 15=������������
	REWARD_CENTER_STAGE,					// 16=��������λ��
	REWARD_CENTER_TURNTABLE,				// 17=ת�����н���
	REWARD_CENTER_SCORE_MATCH_RANK,			// 18=���������н���
	REWARD_CENTER_RESOURCE_WAR_BASE,		// 19=��Դս���ֽ���(��������)
	REWARD_CENTER_RESOURCE_WAR_EXTRA,		// 20=��Դս���⽱��(���⽱��)

	REWARD_CENTER_GUILD_WAR_BREAK_TOWER,	// 21	����ս��������
	REWARD_CENTER_GUILD_WAR_ATTACK_WINNER,	// 22	����ս�н�����ʤ����
	REWARD_CENTER_GUILD_WAR_ATTACK_LOSER,	// 23	����ս�н���ʧ�ܽ���
	REWARD_CENTER_GUILD_WAR_DEFENSE_WINNER,	// 24	����ս�з��ػ�ʤ����
	REWARD_CENTER_GUILD_WAR_DEFENSE_LOSER,	// 25	����ս�з���ʧ�ܽ���
	REWARD_CENTER_GUILD_WAR_RANK,			// 26	����ս���н���
	REWARD_CENTER_YGAME_RCHARGE,			// 27	YGame��ֵ����
	REWARD_CENTER_YGAME_CASHBACK,			// 28	YGame����֪ͨ����
	REWARD_CENTER_YGAME_BONUS,				// 29	YGame����֪ͨ����

	REWARD_CENTER_RECHARGE_REWARD,			// 30	��ֵ����
	REWARD_CENTER_COST_REWARD,				// 31	���Ѱ���


	// 	REWARD_CENTER_GM_RETURN,				// 9=GM����
	// 	REWARD_CENTER_VIP_WELFARE,				// 10=VIP����
};


CRewardCenterManager::CRewardCenterManager()
{
	m_dwSeriesId = 0;
}

CRewardCenterManager::~CRewardCenterManager()
{}

void CRewardCenterManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pRewardInfo = &pPlayer->GetRoleFullInfo().tRewardInfo;
	m_dwSeriesId = 1;

	// ��������uid
	for (WORD i = 0; i < m_pRewardInfo->wRewardNum; ++i)
	{
		m_dwSeriesId, m_pRewardInfo->arRewardList[i].dwUId = m_dwSeriesId++;
	}

// 	for (BYTE i = 0; i < m_pRewardInfo->byRewardNum; ++i)
// 	{
// 		m_dwSeriesId = max(m_dwSeriesId, m_pRewardInfo->arRewardList[i].dwId);
// 	}
}

// �������б�
void CRewardCenterManager::OnNetRewardList()
{
	SendRewardList();
}

// ���ͽ����б�
void CRewardCenterManager::SendRewardList()
{
	MSG_S2C_REWARD_CENTER_LIST_RESP msg;
	msg.m_byRewardNum = 0;

	for (WORD i = 0; i < m_pRewardInfo->wRewardNum; ++i)
	{
		ClientRewardData *pReward = &msg.m_RewardList[msg.m_byRewardNum++];
		memcpy_s(pReward, sizeof(RewardData), &m_pRewardInfo->arRewardList[i], sizeof(RewardData));
		DWORD dwTitleId;
		GetReward(pReward->wInfoId, pReward->dwValue1, pReward->dwValue2, pReward->vItemList, pReward->vCurrencyList, dwTitleId);
	}

	// todo::��������
	// 	msg.m_byRewardNum = 0;
	// 	RewardData *pReward = &msg.m_RewardList[msg.m_byRewardNum++];
	// 	pReward->byType = 2;
	// 	pReward->dwTime = time(NULL) - 1000;
	// 
	// 	pReward = &msg.m_RewardList[msg.m_byRewardNum++];
	// 	pReward->byType = 4;
	// 	pReward->dwValue = 78;
	// 	pReward->dwTime = time(NULL) - 2000;
	// 
	// 	pReward = &msg.m_RewardList[msg.m_byRewardNum++];
	// 	pReward->byType = 6;
	// 	pReward->dwValue = 1;
	// 	strcat_s(pReward->szText, sizeof(pReward->szText), "Kevin");
	// 	pReward->dwTime = time(NULL) - 3000;
	// 
	// 	pReward = &msg.m_RewardList[msg.m_byRewardNum++];
	// 	pReward->byType = 7;
	// 	pReward->dwValue = 99999;
	// 	strcat_s(pReward->szText, sizeof(pReward->szText), "Fuck");
	// 	pReward->dwTime = time(NULL) - 4000;
	// 
	// 	pReward = &msg.m_RewardList[msg.m_byRewardNum++];
	// 	pReward->byType = 1;
	// 	pReward->dwValue = 78;
	// 	pReward->dwTime = time(NULL) - 4000;

	m_pPlayer->SendPacket(&msg);
}

// ��ȡ����
void CRewardCenterManager::OnNetRewardGet(DWORD *pRewardIdList, BYTE byRewardNum)
{
	WORD wFromMsgId = C2S_REWARD_CENTER_GET_REQ;

	for (BYTE i = 0; i < byRewardNum; ++i)
	{
		DWORD dwId = pRewardIdList[i];

		// ���Ҫ��ȡ�Ľ����Ƿ����
		RewardData *pReward = GetRewardData(dwId);
		if (!pReward)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]û��Ҫ��ȡ�Ľ���[UID:%d]", __FUNCTION__, dwId);
			continue;
		}

		// ȡ����
		vector<Item> vItemList;
		vector<MoneyData> vCurrencyList;
		DWORD dwTitleId = 0;
		if (!GetReward(pReward->wInfoId, pReward->dwValue1, pReward->dwValue2, vItemList, vCurrencyList, dwTitleId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]��ȡ����ʧ��[InfoId:%d,RoleId:%d]", __FUNCTION__, pReward->wInfoId, m_pPlayer->GetRoleId());
			//continue;
		}

		// ������
		m_pPlayer->AddObjectList(vItemList, TRUE, FROM_REWARD_CENTER);
		for (size_t i = 0; i < vCurrencyList.size(); ++i){
			m_pPlayer->ModifyProperty(vCurrencyList[i].byType, vCurrencyList[i].nValue, TRUE, FROM_REWARD_CENTER);
		}

		// �����ȡ�����¿���ֵ����(ygame)
		if (pReward->wInfoId == 100)
		{
			const YGamePayment_Config *pPaymentCfg = g_pCfgMgr->GetYGamePayment(pReward->dwValue1);
			if (pPaymentCfg && pPaymentCfg->nPaymentFromType == 2)
			{
				// ��ӹ����¿�����
				MonthCard tCard;
				tCard.wCardId = pPaymentCfg->dwRechargeId;
				tCard.dwLastTakeRewardTime = 0;
				tCard.dwBuyTime = time(NULL);
				m_pPlayer->GetRechargeManager()->AddMonthCardData(&tCard);
			}
		}
			
		// ɾ������
		RemoveRewardData(dwId);
	}

	MSG_S2C_REWARD_CENTER_GET_RESP msg;
	msg.m_byRewardNum = byRewardNum;
	memcpy_s(msg.m_arRewardIdList, byRewardNum*sizeof(DWORD), pRewardIdList, byRewardNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);
}

// ��ӽ���
void CRewardCenterManager::AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (pPlayer)
	{
		pPlayer->GetRewardCenterManager()->AddReward(tReward);
	}
	else
	{
		PlayerBaseDataManager::Instance()->AddReward(dwServerId, dwRoleId, tReward);
	}
}

// ��ӽ���
void CRewardCenterManager::AddReward(const RewardData &tReward, bool bNotify)
{
	if (m_pRewardInfo->wRewardNum >= MAX_REWARD_CENTER_REWARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����б�����[InfoId:%d]", __FUNCTION__, tReward.wInfoId);
		return;
	}	

	RewardData tmp = tReward;
	tmp.dwUId = GetNewId();

	// ����Ƿ��Ѵ���
	if (GetRewardData(tReward.dwUId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����Id�Ѵ���[uid:%d]", __FUNCTION__, tReward.dwUId);
		return;
	}
				
	m_pRewardInfo->arRewardList[m_pRewardInfo->wRewardNum++] = tmp;

	if (bNotify)
	{
		MSG_S2C_REWARD_NOTICE msg;
		memcpy_s(&msg.m_RewardData, sizeof(RewardData), &tmp, sizeof(RewardData));
		DWORD dwTitleId = 0;
		GetReward(tmp.wInfoId, tmp.dwValue1, tmp.dwValue2, msg.m_RewardData.vItemList, msg.m_RewardData.vCurrencyList, dwTitleId);
		m_pPlayer->SendPacket(&msg);
	}
}

// ȡ����
bool CRewardCenterManager::GetReward(WORD wRewardInfoId, DWORD dwValue1, DWORD dwValue2, vector<Item> &vItemList, vector<MoneyData> &vCurrencyList, DWORD &dwTitleId)
{
	vItemList.clear();
	vCurrencyList.clear();
	dwTitleId = 0;

	const Reward_Config *pRewardCfg = g_pCfgMgr->GetReward(wRewardInfoId);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[RewardInfoId:%d]", __FUNCTION__, wRewardInfoId);
		return false;
	}

	BYTE byRewardType = pRewardCfg->nType;

	// ��������������
	if (REWARD_CENTER_ARENA_RANK == byRewardType)
	{
		const ArenaRank_Config *pRewardCfg = g_pCfgMgr->GetArenaRank(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_GOLD;
		tMoney.nValue = pRewardCfg->nAwardGold;
		vCurrencyList.push_back(tMoney);
		tMoney.byType = ROLE_PRO_ARENA_PRESTIGE;
		tMoney.nValue = pRewardCfg->nAwardPrestige;
		vCurrencyList.push_back(tMoney);
	}
	// �޾�֮����������
	else if (REWARD_CENTER_ENDLESS_LAND_RANK == byRewardType)
	{
		const EndlessLandRankReward_Config *pRewardCfg = g_pCfgMgr->GetEndlessLandRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����޾�֮�ؽ�������[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vItemList;
		dwTitleId = pRewardCfg->dwTitleId;
	}
	// PVP���н���
	else if (REWARD_CENTER_PVP_RANK == byRewardType)
	{
		const PVPRankReward_Config *pRewardCfg = g_pCfgMgr->GetPVPRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���PVP���н�������[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vItemList;
		dwTitleId = pRewardCfg->dwTitleId;
	}
	// ����
	else if (REWARD_CENTER_ATTACK_CITY_RANK == byRewardType)
	{
		const AttackCityRankReward_Config *pRewardCfg = g_pCfgMgr->GetAttackCityRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������н�������[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}
		vItemList = pRewardCfg->vItemList;
	}
	// �Ѿ���ɱ��ʯ����
	else if (REWARD_CENTER_KILL_RABEL == byRewardType)
	{
		const RebelQuality_Config *pQualityCfg = g_pCfgMgr->GetRebelQuality(dwValue2);
		if (!pQualityCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�Ʒ������[Quality:%d]", __FUNCTION__, dwValue2);
			return false;
		}

		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_DIAMOND;
		tMoney.nValue = pQualityCfg->nKillerAwardDiamond;
		vCurrencyList.push_back(tMoney);
	}
	// �Ѿ������ɱ��ʯ����
	else if (REWARD_CENTER_DICOVER == byRewardType)
	{
		const RebelQuality_Config *pQualityCfg = g_pCfgMgr->GetRebelQuality(dwValue2);
		if (!pQualityCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ�Ʒ������[Quality:%d]", __FUNCTION__, dwValue2);
			return false;
		}

		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_DIAMOND;
		tMoney.nValue = pQualityCfg->nDiscoverAwardDiamond;
		vCurrencyList.push_back(tMoney);
	}
	// ����BOSS������������
	else if (REWARD_CENTER_WORLD_BOSS_HONOR_RANK == byRewardType)
	{
		const WorldBossRankReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS�������н���[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vHonorRewardList;
	}
	// ����BOSS�˺���������
	else if (REWARD_CENTER_WORLD_BOSS_DAMAGE_RANK == byRewardType)
	{
		const WorldBossRankReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS�˺����н���[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vDamageRewardList;
	}
	// �Ѿ���ѫ����
	else if (REWARD_CENTER_REBEL_EXPOIT_RANK == byRewardType)
	{
		const RebelRankAward_Config *pRewardCfg = g_pCfgMgr->GetRebelRankAward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ���ѫ���н���[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}
		vCurrencyList.push_back(MoneyData(ROLE_PRO_REBEL_VALUE, pRewardCfg->nExploitAward));
	}
	// �Ѿ��˺�����
	else if (REWARD_CENTER_REBEL_DAMAGE_RANK == byRewardType)
	{
		const RebelRankAward_Config *pRewardCfg = g_pCfgMgr->GetRebelRankAward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����Ѿ��˺����н���[Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}
		vCurrencyList.push_back(MoneyData(ROLE_PRO_REBEL_VALUE, pRewardCfg->nDamageAward));
	}
	// GM����
	else if (REWARD_CENTER_GM == byRewardType)
	{
		vItemList.push_back(Item(dwValue1, dwValue2));
	}
	// ��ս����
	else if (REWARD_CENTER_HELP_BATTLE == byRewardType)
	{
		vCurrencyList.push_back(MoneyData(ROLE_PRO_FRIENDSHIP_PIONT, g_GetConfigValue(125)));
	}
	// 7�����н���
	else if (REWARD_CENTER_SEVEN_DAY_RANK == byRewardType)
	{
		const RoleRankReward_Config *pRewardCfg = g_pCfgMgr->GetSevenDayRankReward(dwValue2, dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7�����н�������[type:%d,rank:%d]", __FUNCTION__, dwValue2, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vItemList;
	}
	else if (REWARD_CENTER_CLOSED_TEST_REBATE == byRewardType)
	{
		const ClosedTestRebate_Config *pRebateCfg = g_pCfgMgr->GetClosedTestRebate(m_pPlayer->GetAccID());
		if (!pRebateCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����˺ŵķ�ⷵ������[accont:%s]", __FUNCTION__, m_pPlayer->GetAccID());
			return false;
		}

		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_DIAMOND;
		tMoney.nValue = pRebateCfg->nDiamond;
		vCurrencyList.push_back(tMoney);
	}
	// ��������������
	else if (REWARD_CENTER_RANK == byRewardType)
	{
		const KingMatchRankReward_Config *pRewardCfg = g_pCfgMgr->GetKingMatchRankReward(dwValue1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������н���[rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pRewardCfg->vRewardList;
	}
	// ��������λ����
	else if (REWARD_CENTER_STAGE == byRewardType)
	{
		const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(dwValue1);
		if (!pStageCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������н���[rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}

		vItemList = pStageCfg->vSeasonReward;
	}
	// ת�����н���
	else if (REWARD_CENTER_TURNTABLE == byRewardType)
	{
		const TurntableRank_Config *pRankCfg = g_pCfgMgr->GetTurntableRank(dwValue2, dwValue1);
		if (!pRankCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ת�����н���[DataId:%d, Rank:%d]", __FUNCTION__, dwValue2, dwValue1);
			return false;
		}
		vItemList = pRankCfg->vRewardItemList;
	}
	// ���������н���
	else if (REWARD_CENTER_SCORE_MATCH_RANK == byRewardType)
	{
		const ScoreMatchRankReward_Config *pRankCfg = g_pCfgMgr->GetScoreMatchRankReward(dwValue1);
		if (!pRankCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������н���[DataId:%d, Rank:%d]", __FUNCTION__, dwValue1);
			return false;
		}
		vItemList = pRankCfg->vRewardItemList;
		dwTitleId = pRankCfg->nTitleId;
	}
	// ��Դս���ֽ���(��������)
	else if (REWARD_CENTER_RESOURCE_WAR_BASE == byRewardType)
	{
		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_RESOURCE_WAR_SCORE;
		tMoney.nValue = dwValue2;
		vCurrencyList.push_back(tMoney);
	}
	// ��Դս���⽱��(���⽱��)
	else if (REWARD_CENTER_RESOURCE_WAR_EXTRA == byRewardType)
	{
		const vector<Item> *pRewardList = CResourceWarManager::Instance()->GetExtraRewardList(dwValue1, dwValue2);
		if (pRewardList)
		{
			vItemList = *pRewardList;
		}
	}

	// ����ս��������
	else if (REWARD_CENTER_GUILD_WAR_BREAK_TOWER == byRewardType)
	{
		const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(dwValue2);
		if (pTowerCfg)
 		{
			vItemList = pTowerCfg->vBreakTowerRewardList;
 		}
	}
	// ����ս�н�����ʤ,����ս�з��ػ�ʤ,����ս�з���ʧ��
	else if (REWARD_CENTER_GUILD_WAR_ATTACK_WINNER == byRewardType ||
		REWARD_CENTER_GUILD_WAR_DEFENSE_WINNER == byRewardType || 
		REWARD_CENTER_GUILD_WAR_DEFENSE_LOSER == byRewardType)
	{
		BYTE byVSType, byVSResult;
		SplitWord(dwValue2, byVSType, byVSResult);
		const GuildWarVSReward_Config *pVSRewardCfg = g_pCfgMgr->GetGuildWarVSReward(byVSType, byVSResult);
		if (pVSRewardCfg)
		{
			vector<vector<Item>>::const_iterator iter = pVSRewardCfg->vRewardList.begin();
			for (; iter != pVSRewardCfg->vRewardList.end(); ++iter)
			{
				vItemList.assign(iter->begin(), iter->end());
			}
		}
	}
	// ����ս�н���ʧ�ܽ���
	else if (REWARD_CENTER_GUILD_WAR_ATTACK_LOSER == byRewardType)
	{
		WORD wHighValue,wLowValue;
		SplitDWord(dwValue2, wHighValue, wLowValue);
		BYTE byVSType, byVSResult;
		SplitWord(wLowValue, byVSType, byVSResult);
		const GuildWarVSReward_Config *pVSRewardCfg = g_pCfgMgr->GetGuildWarVSReward(byVSType, byVSResult);
		if (pVSRewardCfg && pVSRewardCfg->vBreakTowerNumList.size() == pVSRewardCfg->vRewardList.size())
		{
			for (size_t i = 0; i < pVSRewardCfg->vBreakTowerNumList.size(); i++)
			{
				// �ҵ���Ӧ��������������
				if (pVSRewardCfg->vBreakTowerNumList[i] == wHighValue)
				{
					vItemList = pVSRewardCfg->vRewardList[i];
					break;
				}
			}
		}
	}
	// YGame��ֵ����
	else if (REWARD_CENTER_YGAME_RCHARGE == byRewardType)
	{
		const YGamePayment_Config *pPaymentCfg = g_pCfgMgr->GetYGamePayment(dwValue1);
		if (!pPaymentCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ���ygame֧������[assist_id:%d]",dwValue1);
			return false;
		}

		if (pPaymentCfg->nDiamond + pPaymentCfg->nBonus > 0){
			MoneyData tMoney;
			tMoney.byType = ROLE_PRO_DIAMOND;
			tMoney.nValue = pPaymentCfg->nDiamond + pPaymentCfg->nBonus;
			vCurrencyList.push_back(tMoney);
		}

		if (pPaymentCfg->nVipExp > 0){
			MoneyData tMoney;
			tMoney.byType = ROLE_PRO_VIP_EXP;
			tMoney.nValue = pPaymentCfg->nVipExp;
			vCurrencyList.push_back(tMoney);
		}

		if (pPaymentCfg->nYCoin > 0){
			MoneyData tMoney;
			tMoney.byType = ROLE_PRO_YCOIN;
			tMoney.nValue = pPaymentCfg->nYCoin;
			vCurrencyList.push_back(tMoney);
		}

		vItemList = pPaymentCfg->vItemList;
	}
	// YGame����֪ͨ����
	else if (REWARD_CENTER_YGAME_CASHBACK == byRewardType)
	{
		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_DIAMOND;
		tMoney.nValue = dwValue1;
		vCurrencyList.push_back(tMoney);
	}
	// YGame����֪ͨ����
	else if (REWARD_CENTER_YGAME_BONUS == byRewardType)
	{
		MoneyData tMoney;
		tMoney.byType = ROLE_PRO_DIAMOND;
		tMoney.nValue = dwValue1;
		vCurrencyList.push_back(tMoney);
	}
	// ��ֵ����
	else if (REWARD_CENTER_RECHARGE_REWARD == byRewardType)
	{
		const RechargeRankReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeAndCostRankReward(dwValue1, 1);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ�����ֵ��������[Rank:%d]", dwValue1);
			return false;
		}
		vItemList = pRewardCfg->vRewardList;
	}
	// ���Ѱ���
	else if (REWARD_CENTER_COST_REWARD == byRewardType)
	{
		const RechargeRankReward_Config *pRewardCfg = g_pCfgMgr->GetRechargeAndCostRankReward(dwValue1, 2);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ������Ѱ�������[Rank:%d]", dwValue1);
			return false;
		}
		vItemList = pRewardCfg->vRewardList;
	}
	else
	{
		vItemList = pRewardCfg->vItemList;
	}

	return true;
}

// �Ƿ��к��
bool CRewardCenterManager::IsHaveRedPoint(Player *pPlayer)
{
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), pPlayer->GetRoleId(), false);
	if (!pBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[RoleId:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	return pBaseData->vRewardList.size() ? true : false;
}

RewardData* CRewardCenterManager::GetRewardData(DWORD dwRewardUId)
{
	for (WORD i = 0; i < m_pRewardInfo->wRewardNum; ++i)
	{
		if (m_pRewardInfo->arRewardList[i].dwUId == dwRewardUId)
			return &m_pRewardInfo->arRewardList[i];
	}
	return NULL;
}

// ɾ����������
void CRewardCenterManager::RemoveRewardData(DWORD dwRewardUId)
{
	for (WORD i = 0; i < m_pRewardInfo->wRewardNum; ++i)
	{
		if (m_pRewardInfo->arRewardList[i].dwUId == dwRewardUId)
		{
			m_pRewardInfo->arRewardList[i] = m_pRewardInfo->arRewardList[--m_pRewardInfo->wRewardNum];
			break;
		}
	}
}

DWORD CRewardCenterManager::GetNewId()
{
// 	if (!m_dwSeriesId)
// 	{
// 		for (WORD i = 0; i < m_pRewardInfo->wRewardNum; ++i)
// 		{
// 			m_dwSeriesId = max(m_dwSeriesId, m_pRewardInfo->arRewardList[i].dwId);
// 		}
// 	}
	return ++m_dwSeriesId;
}