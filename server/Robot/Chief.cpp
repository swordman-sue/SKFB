#include "stdafx.h"
#include "Chief.h"
#include "Retinue.h"
#include "Card.h"
#include "RobotManager.h"
#include "Robot.h"
#include "BattleFactory.h"
#include "../ToolKit/Rand.h"

#define AI_PREVIEW_TIME		1500		//客户端AI表现时长(毫秒)

Chief::Chief()
{
// 	m_pEnemyChief	= NULL;
// 	m_HandCardVec.reserve(MAX_HAND_CARD_NUM*2);
// 	m_BattleRetinueVec.reserve(MAX_BATTLE_RETINUE_NUM*2);
// 	m_MagicRetinueVec.reserve(MAX_BATTLE_RETINUE_NUM*2);
// 	m_GraveCardVec.reserve(MAX_GRAVE_RETINUE_NUM);
// 	m_bFirstHand	= FALSE;
// 	m_shRoundNum	= 0;
// 	m_enAIStat		= AI_STAT_INIT;
// 	m_enBattleEnv	= BATTLE_ENV_COMMON;
// 	m_AITimer.DisableCheckTime();
}

Chief::~Chief()
{

}

BOOL Chief::Init(Robot* pRobot)
{
	m_HandCardVec.clear();
	m_BattleRetinueVec.clear();
	m_MagicRetinueVec.clear();
	m_GraveCardVec.clear();
	m_bFirstHand	= FALSE;
	m_shRoundNum	= 0;
	m_enAIStat		= AI_STAT_INIT;
	m_AITimer.DisableCheckTime();

	//调用基类的函数
	if (!GameObject::Init(pRobot))
		return FALSE;

	return TRUE;
}

VOID Chief::Release()
{
	Card* pCard = NULL;
	Retinue* pRetinue = NULL;
	//释放手牌
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		pCard->Release();
		BattleFactory::Instance()->FreeCard(pCard);
	}
	m_HandCardVec.clear();

	//释放墓地
	for (CardVecIter iter = m_GraveCardVec.begin(); iter != m_GraveCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		pCard->Release();
		BattleFactory::Instance()->FreeCard(pCard);
	}
	m_GraveCardVec.clear();

	//释放随从
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue)
			continue;

		pRetinue->Release();
		BattleFactory::Instance()->FreeRetinue(pRetinue);
	}
	m_BattleRetinueVec.clear();

	for (RetinueVecIter iter = m_MagicRetinueVec.begin(); iter != m_MagicRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue)
			continue;

		pRetinue->Release();
		BattleFactory::Instance()->FreeRetinue(pRetinue);
	}
	m_MagicRetinueVec.clear();

	//最后调用基类的函数
	GameObject::Release();
}

VOID Chief::Update(DWORD dwTick)
{

}

VOID Chief::AddToGameObjectMap(GameObject* pGameObject)
{
	if (!m_pOwner ||!pGameObject)
		return;

	m_pOwner->AddToGameObjectMap(pGameObject);
}

VOID Chief::RemoveFromGameObjectMap(GameObject* pGameObject)
{
	if (!m_pOwner ||!pGameObject)
		return;

	m_pOwner->RemoveFromGameObjectMap(pGameObject);
}

//根据Key来查找手牌
Card* Chief::FindHandCardByKey(DWORD dwKey)
{
	if (!dwKey)
		return NULL;

	Card* pCard = NULL;
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		if (pCard->GetObjectKey() == dwKey)
			return pCard;
	}

	return NULL;
}

//根据Key来查找墓地卡牌
Card* Chief::FindGraveCardByKey(DWORD dwKey)
{
	if (!dwKey)
		return NULL;

	Card* pCard = NULL;
	for (CardVecIter iter = m_GraveCardVec.begin(); iter != m_GraveCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		if (pCard->GetObjectKey() == dwKey)
			return pCard;
	}

	return NULL;
}

//加入卡牌到手牌中
VOID Chief::AddCardToHand(DWORD dwCardKey, DWORD dwCardID)
{
	if (!dwCardKey || !dwCardID)
		return;

	Card* pCard = BattleFactory::Instance()->AllocCard();
	if (!pCard)
		return;

	if (!pCard->Init(m_pOwner, this, dwCardID))
	{
		BattleFactory::Instance()->FreeCard(pCard);
		return;
	}

	pCard->SetObjectKey(dwCardKey);
	m_HandCardVec.push_back(pCard);
	AddToGameObjectMap(pCard);
}

//从手牌中移除卡牌
VOID Chief::RemoveCardFromHand(DWORD dwCardKey)
{
	if (!dwCardKey)
		return;

	Card* pTmp = NULL;
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); /*++iter*/)
	{
		pTmp = *iter;
		if (!pTmp || pTmp->GetObjectKey() != dwCardKey)
		{
			++iter;
			continue;
		}

		RemoveFromGameObjectMap(pTmp);
		pTmp->Release();
		BattleFactory::Instance()->FreeCard(pTmp);
		iter = m_HandCardVec.erase(iter);
		break;
	}
}

//加入卡牌到墓地中
VOID Chief::AddCardToGrave(DWORD dwCardKey, DWORD dwCardID)
{
	if (!dwCardKey || !dwCardID)
		return;

	Card* pCard = BattleFactory::Instance()->AllocCard();
	if (!pCard)
		return;

	if (!pCard->Init(m_pOwner, this, dwCardID))
	{
		BattleFactory::Instance()->FreeCard(pCard);
		return;
	}

	pCard->SetObjectKey(dwCardKey);
	m_GraveCardVec.push_back(pCard);
	AddToGameObjectMap(pCard);
}

//从墓地中移除卡牌
VOID Chief::RemoveCardFromGrave(DWORD dwCardKey)
{
	if (!dwCardKey)
		return;

	Card* pTmp = NULL;
	for (CardVecIter iter = m_GraveCardVec.begin(); iter != m_GraveCardVec.end(); /*++iter*/)
	{
		pTmp = *iter;
		if (!pTmp || pTmp->GetObjectKey() != dwCardKey)
		{
			++iter;
			continue;
		}

		RemoveFromGameObjectMap(pTmp);
		pTmp->Release();
		BattleFactory::Instance()->FreeCard(pTmp);
		iter = m_GraveCardVec.erase(iter);
		break;
	}
}

//获得存活的战斗随从数量
BYTE Chief::GetAliveBattleRetinueNum()
{
	BYTE byNum = 0;
	Retinue* pRetinue = NULL;
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (pRetinue && pRetinue->IsAlive())
			++byNum;
	}

	return byNum;
}

//加入一个随从
VOID Chief::AddRetinue(DWORD dwRetinueKey, DWORD dwRetinueID, char cSlot, BYTE byOrien)
{
	if (!dwRetinueKey || !dwRetinueID)
		return;

	RetinueEntry* pRetinueEntry = RobotManager::Instance()->GetRetinueEntry(dwRetinueID);
	if (!pRetinueEntry)
		return;

	if (pRetinueEntry->m_dwRetinueType != RETINUE_TYPE_BATTLE
	 && pRetinueEntry->m_dwRetinueType != RETINUE_TYPE_MAGIC)
		return;

	if (cSlot >= MAX_BATTLE_RETINUE_NUM)
		return;

	if (byOrien != GO_ORIEN_FRONT && byOrien != GO_ORIEN_BACK)
		return;

	Retinue* pRetinue = BattleFactory::Instance()->AllocRetinue();
	if (!pRetinue)
		return;

	if (!pRetinue->Init(m_pOwner, this, pRetinueEntry, byOrien))
	{
		BattleFactory::Instance()->FreeRetinue(pRetinue);
		return;
	}

	pRetinue->SetObjectKey(dwRetinueKey);
	if (pRetinueEntry->m_dwRetinueType == RETINUE_TYPE_BATTLE)
	{
		if (cSlot < 0)
		{
			m_BattleRetinueVec.push_back(pRetinue);
		}
		else
		{
			if (cSlot > (char)m_BattleRetinueVec.size())
				m_BattleRetinueVec.push_back(pRetinue);
			else
				m_BattleRetinueVec.insert(m_BattleRetinueVec.begin() + cSlot, pRetinue);
		}
	}
	else
	{
		if (cSlot < 0)
		{
			m_MagicRetinueVec.push_back(pRetinue);
		}
		else
		{
			if (cSlot > (char)m_MagicRetinueVec.size())
				m_MagicRetinueVec.push_back(pRetinue);
			else
				m_MagicRetinueVec.insert(m_MagicRetinueVec.begin() + cSlot, pRetinue);
		}
	}

	AddToGameObjectMap(pRetinue);
}

//根据Key来查找战斗随从
Retinue* Chief::FindBattleRetinueByKey(DWORD dwKey)
{
	if (!dwKey)
		return NULL;

	Retinue* pRetinue = NULL;
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue)
			continue;

		if (pRetinue->GetObjectKey() == dwKey)
			return pRetinue;
	}

	return NULL;
}

//根据Key来移除战斗随从
VOID Chief::RemoveBattleRetinueByKey(DWORD dwKey)
{
	if (!dwKey)
		return;

	Retinue* pRetinue = NULL;
	RetinueVecIter iter;
	for (iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); /*++iter*/)
	{
		pRetinue = *iter;
		if (!pRetinue || pRetinue->GetObjectKey() != dwKey)
		{
			++iter;
			continue;
		}

		RemoveFromGameObjectMap(pRetinue);
		pRetinue->Release();
		BattleFactory::Instance()->FreeRetinue(pRetinue);
		iter = m_BattleRetinueVec.erase(iter);
		break;
	}
}

//根据Key来查找魔法随从
Retinue* Chief::FindMagicRetinueByKey(DWORD dwKey)
{
	if (!dwKey)
		return NULL;

	Retinue* pRetinue = NULL;
	for (RetinueVecIter iter = m_MagicRetinueVec.begin(); iter != m_MagicRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue)
			continue;

		if (pRetinue->GetObjectKey() == dwKey)
			return pRetinue;
	}

	return NULL;
}

//根据Key来移除魔法随从
VOID Chief::RemoveMagicRetinueByKey(DWORD dwKey)
{
	if (!dwKey)
		return;

	Retinue* pRetinue = NULL;
	RetinueVecIter iter;
	for (iter = m_MagicRetinueVec.begin(); iter != m_MagicRetinueVec.end(); /*++iter*/)
	{
		pRetinue = *iter;
		if (!pRetinue || pRetinue->GetObjectKey() != dwKey)
		{
			++iter;
			continue;
		}

		RemoveFromGameObjectMap(pRetinue);
		pRetinue->Release();
		BattleFactory::Instance()->FreeRetinue(pRetinue);
		m_MagicRetinueVec.erase(iter);
		break;
	}
}


//处理随从死亡事件
VOID Chief::OnRetinueDead(Retinue* pRetinue)
{
	if (!pRetinue)
		return;

	if (pRetinue->IsBattleRetinue())
	{
		RemoveBattleRetinueByKey(pRetinue->GetObjectKey());
	}
	else if (pRetinue->IsMagicRetinue())
	{
		RemoveMagicRetinueByKey(pRetinue->GetObjectKey());
	}
}

//处理战斗AI
VOID Chief::ProcessAI()
{
	if (!m_pOwner)
		return;

	if (m_AITimer.IsEnabled() && m_AITimer.IsExpired(FALSE) == FALSE)
		return;

	switch(m_enAIStat)
	{
	case AI_STAT_PLAY_CARD:
		{
			ProcessPlayCardAI();
		}
		break;

	case AI_STAT_ATTACK_1:
		{
			ProcessAttackAI_1();
		}
		break;

	case AI_STAT_ATTACK_2:
		{
			ProcessAttackAI_2();
		}
		break;

	case AI_STAT_ATTACK_3:
		{
			ProcessAttackAI_3();
		}
		break;

	case AI_STAT_ATTACK_4:
		{
			ProcessAttackAI_4();
		}
		break;

	case AI_STAT_FINISH:
		{
			ProcessFinishAI();
		}
		break;

	default:
		break;
	}
}

//处理出牌AI
VOID Chief::ProcessPlayCardAI()
{
	if (m_enAIStat != AI_STAT_PLAY_CARD)
		return;

	if (!m_pOwner)
		return;

	if (GetBattleRetinueNum() >= MAX_BATTLE_RETINUE_NUM)
	{
		m_enAIStat = AI_STAT_ATTACK_1;
		return;
	}

	//处理出牌
	BYTE byProcessTimes = 0;
	Card* pCard = NULL;
	//1 使用速攻法术牌
	pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_ATTACK_SPELL);
	if (pCard)
	{
		if (PlayCard(pCard, 0, GO_ORIEN_FRONT))
			++byProcessTimes;
	}

	//2 使用陷阱卡牌
	if (GetMagicRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_TRAP);
		if (pCard)
		{
			//HandleUseCardSyn(pCard->GetObjectKey(), NULL, 0, GetBattleRetinueNum(), GO_ORIEN_BACK);
			if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_BACK))
				++byProcessTimes;
		}
	}

	//3 使用法术召唤魔法随从
	if (GetMagicRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_MR_SPELL);
		if (pCard)
		{
			//HandleUseCardSyn(pCard->GetObjectKey(), NULL, 0, GetBattleRetinueNum(), GO_ORIEN_BACK);
			if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_BACK))
				++byProcessTimes;
		}
	}

	//4 使用随从卡牌
	//获得敌方攻击力最高随从
	Retinue* pRetinue = GetEnemyMaxAttackRetinue();
	if (GetBattleRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		//先手且为第一个回合
		if (m_bFirstHand && m_shRoundNum == 1)
		{
			//获得最高防御力卡牌
			//pCard = GetMaxDefenseCard();
			//获得先手卡牌
			pCard = GetFirstCard();
			if (pCard)
			{
				//if (SetSacrificeTargetForAI(pCard, dwSacrificeTarget, bySacrificeNum))
				//	HandleUseCardSyn(pCard->GetObjectKey(), dwSacrificeTarget, bySacrificeNum, GetBattleRetinueNum(), GO_ORIEN_BACK);
				GameObjectOrien enOrien = pCard->GetAttackPower() >= pCard->GetDefensePower() ? GO_ORIEN_FRONT : GO_ORIEN_BACK;
				if (PlayCard(pCard, GetBattleRetinueNum(), enOrien))
					++byProcessTimes;
			}
		}
		//正常回合
		else
		{
			//对方随从存在
			if (pRetinue)
			{
				pCard = GetMaxAttackCard();
				if (pCard && pCard->GetAttackPower() >= pRetinue->GetAttackPower())
				{
					if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_FRONT))
						++byProcessTimes;
				}
				else
				{
					Retinue* pTmp = GetEnemyMinAttackRetinue();
					if (pCard && pTmp && pCard->GetAttackPower() >= pTmp->GetAttackPower())
					{
						if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_FRONT))
							++byProcessTimes;
					}
					else
					{
						//获得具有"冲锋"状态的卡牌
						pCard = GetRushCard();
						if (pCard)
						{
							if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_FRONT))
								++byProcessTimes;
						}
						else
						{
							//获得最高防御力卡牌
							pCard = GetMaxDefenseCard();
							if (pCard)
							{
								if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_BACK))
									++byProcessTimes;
							}
						}
					}
				}
			}
			//对方随从不存在
			else
			{
				pCard = GetMaxAttackCard();
				if (pCard)
				{
					if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_FRONT))
						++byProcessTimes;
				}
			}
		}
	}

	//5 使用法术召唤战斗随从
	if (GetBattleRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_BR_SPELL);
		if (pCard)
		{
			if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_BACK))
				++byProcessTimes;
		}
	}

	//6 使用装备卡牌
	if (GetMagicRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_EQUIPMENT);
		if (pCard)
		{
			if (PlayCard(pCard, GetMagicRetinueNum(), GO_ORIEN_FRONT))
				++byProcessTimes;
		}
	}

	//启动表现计时器
	if (byProcessTimes)
		m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

	//切换AI状态
	m_enAIStat = AI_STAT_ATTACK_1;
}

//处理攻击AI
VOID Chief::ProcessAttackAI_1()
{
	if (m_enAIStat != AI_STAT_ATTACK_1)
		return;

	if (m_bFirstHand && m_shRoundNum == 1)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (GetBattleRetinueNum() <= 0)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (!m_pEnemyChief)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	Retinue* pAttacker = NULL;
	BYTE byProcessTimes = 0;
	//处理攻击敌方正面随从
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pAttacker = *iter;
			if (!pAttacker || !pAttacker->IsEnableAttack())
				continue;

			//选择一个合适的攻击目标
			Retinue* pTarget = NULL;
			Retinue* pTmp = NULL;
			BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
			for (BYTE i  = 0; i < byTargetNum; ++i)
			{
				pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
				if (!pTmp || pTmp->IsDead() || pTmp->GetOrien() == GO_ORIEN_BACK)
					continue;

				//暂定为攻击目标
				if (pAttacker->GetAttackPower() >= pTmp->GetAttackPower())
				{
					//和已有攻击目标比较
					if (!pTarget)
					{
						pTarget = pTmp;
					}
					else
					{
						if (pTarget->GetAttackPower() < pTmp->GetAttackPower())
							pTarget = pTmp;
					}
				}
			}

			//处理攻击目标
			if (pTarget)
			{
				//先翻转卡牌
				if (pAttacker->GetOrien() == GO_ORIEN_BACK)
				{
					if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
					{
						m_enAIStat = AI_STAT_ATTACK_2;
						return;
					}
					++byProcessTimes;
				}

				if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), pTarget->GetObjectKey()))
				{
					m_enAIStat = AI_STAT_ATTACK_2;
					return;
				}
				++byProcessTimes;

				//启动表现计时器
				if (byProcessTimes)
					m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

				//不能连续进行攻击处理,每桢仅处理一次,给予攻击结果属性同步的处理时间
				return;
			}
		}
	}

	//切换AI状态
	m_enAIStat = AI_STAT_ATTACK_2;
}

//处理攻击AI
VOID Chief::ProcessAttackAI_2()
{
	if (m_enAIStat != AI_STAT_ATTACK_2)
		return;

	if (m_bFirstHand && m_shRoundNum == 1)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (GetBattleRetinueNum() <= 0)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (!m_pEnemyChief)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	Retinue* pAttacker = NULL;
	BYTE byProcessTimes = 0;
	//处理攻击敌方背面随从
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
		Retinue* pTarget = NULL;
		for (BYTE i = 0; i < byTargetNum; ++i)
		{
			pTarget = m_pEnemyChief->GetBattleRetinueByIndex(i);
			if (!pTarget || pTarget->IsDead() || pTarget->GetOrien() != GO_ORIEN_BACK)
				continue;

			//选择当前能够行动且攻击力最高的随从攻击目标
			Retinue* pAttacker = GetMaxAttackRetinue();
			if (!pAttacker)
				break;

			//先翻转卡牌
			if (pAttacker->GetOrien() == GO_ORIEN_BACK)
			{
				if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
				{
					m_enAIStat = AI_STAT_ATTACK_3;
					return;
				}
				++byProcessTimes;
			}

			//处理攻击目标
			if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), pTarget->GetObjectKey()))
			{
				m_enAIStat = AI_STAT_ATTACK_3;
				return;
			}
			++byProcessTimes;

			//启动表现计时器
			if (byProcessTimes)
				m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

			//不能连续进行攻击处理,每桢仅处理一次,给予攻击结果属性同步的处理时间
			return;
		}
	}

	//切换AI状态
	m_enAIStat = AI_STAT_ATTACK_3;
}

//处理攻击AI
VOID Chief::ProcessAttackAI_3()
{
	if (m_enAIStat != AI_STAT_ATTACK_3)
		return;

	if (m_bFirstHand && m_shRoundNum == 1)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (GetBattleRetinueNum() <= 0)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (!m_pEnemyChief)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	Retinue* pAttacker = NULL;
	BYTE byProcessTimes = 0;
	//再次处理攻击敌方正面随从
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pAttacker = *iter;
			if (!pAttacker || !pAttacker->IsEnableAttack())
				continue;

			//选择一个合适的攻击目标
			Retinue* pTarget = NULL;
			Retinue* pTmp = NULL;
			BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
			for (BYTE i  = 0; i < byTargetNum; ++i)
			{
				pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
				if (!pTmp || pTmp->IsDead() || pTmp->GetOrien() == GO_ORIEN_BACK)
					continue;

				//暂定为攻击目标
				if (pAttacker->GetAttackPower() >= pTmp->GetAttackPower())
				{
					//和已有攻击目标比较
					if (!pTarget)
					{
						pTarget = pTmp;
					}
					else
					{
						if (pTarget->GetAttackPower() < pTmp->GetAttackPower())
							pTarget = pTmp;
					}
				}
			}

			//处理攻击目标
			if (pTarget)
			{
				//先翻转卡牌
				if (pAttacker->GetOrien() == GO_ORIEN_BACK)
				{
					if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
					{
						m_enAIStat = AI_STAT_ATTACK_4;
						return;
					}
					++byProcessTimes;
				}

				if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), pTarget->GetObjectKey()))
				{
					m_enAIStat = AI_STAT_ATTACK_4;
					return;
				}
				++byProcessTimes;

				//启动表现计时器
				if (byProcessTimes)
					m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

				//不能连续进行攻击处理,每桢仅处理一次,给予攻击结果属性同步的处理时间
				return;
			}
		}
	}

	//切换AI状态
	m_enAIStat = AI_STAT_ATTACK_4;
}

//处理攻击AI
VOID Chief::ProcessAttackAI_4()
{
	if (m_enAIStat != AI_STAT_ATTACK_4)
		return;

	if (m_bFirstHand && m_shRoundNum == 1)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (GetBattleRetinueNum() <= 0)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	if (!m_pEnemyChief)
	{
		m_enAIStat = AI_STAT_FINISH;
		return;
	}

	Retinue* pAttacker = NULL;
	BYTE byProcessTimes = 0;
	BYTE byRetinueNum = m_pEnemyChief->GetAliveBattleRetinueNum();
	//4 攻击敌方首领
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		//敌方首领死亡
		if (m_pEnemyChief->IsDead())
			break;

		pAttacker = *iter;
		//攻击者不存在或者攻击者不能进行攻击
		if (!pAttacker || !pAttacker->IsEnableAttack())
			continue;

		//攻击者存在"克制"状态,不能攻击首领
		if (pAttacker->HasStatus(GO_STATUS_RESTRAIN))
			continue;

		//敌方场上存在随从且攻击者没有"冲锋"状态时,不能攻击首领
		if (byRetinueNum > 0 && !pAttacker->HasStatus(GO_STATUS_RUSH))
			continue;

		//先翻转卡牌
		if (pAttacker->GetOrien() == GO_ORIEN_BACK)
		{
			if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
			{
				m_enAIStat = AI_STAT_FINISH;
				return;
			}
			++byProcessTimes;
		}

		//处理攻击首领
		if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), m_pEnemyChief->GetObjectKey()))
		{
			m_enAIStat = AI_STAT_FINISH;
			return;
		}
		++byProcessTimes;

		//启动表现计时器
		if (byProcessTimes)
			m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

		//不能连续进行攻击处理,每桢仅处理一次,给予攻击结果属性同步的处理时间
		return;
	}


	//切换AI状态
	m_enAIStat = AI_STAT_FINISH;
}

//处理结束AI
VOID Chief::ProcessFinishAI()
{
	if (m_enAIStat != AI_STAT_FINISH)
		return;

	m_enAIStat = AI_STAT_INIT;

	//处理结束回合
	HandleRoundEndOptCmd();
}

//获得先手卡牌
Card* Chief::GetFirstCard()
{
	Card* pMaxAttackCard = GetMaxAttackCard();
	Card* pMaxDefenseCard = GetMaxDefenseCard();

	if (pMaxAttackCard && pMaxDefenseCard)
	{
		if (pMaxAttackCard == pMaxDefenseCard)
		{
			return pMaxAttackCard;
		}
		else
		{
			return pMaxAttackCard->GetAttackPower() >= pMaxDefenseCard->GetDefensePower() ? pMaxAttackCard : pMaxDefenseCard;
		}
	}
	else if (!pMaxAttackCard)
	{
		return pMaxDefenseCard;
	}
	else if (!pMaxDefenseCard)
	{
		return pMaxAttackCard;
	}
	else
		return NULL;
}

//获得手牌中攻击力最高的卡牌
Card* Chief::GetMaxAttackCard()
{
	Card* pCard = NULL;
	Card* pTmp = NULL;
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pTmp = *iter;
		if (!pTmp || pTmp->GetCardType() != CARD_TYPE_RETINUE || !CheckSacrificeForAI(pTmp) || !CheckPlayCard(pTmp))
			continue;

		if (!pCard)
		{
			pCard = pTmp;
		}
		else
		{
			if (pCard->GetAttackPower() < pTmp->GetAttackPower())
				pCard = pTmp;
		}
	}

	return pCard;
}

//获得手牌中防御力最高的卡牌
Card* Chief::GetMaxDefenseCard()
{
	Card* pCard = NULL;
	Card* pTmp = NULL;
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pTmp = *iter;
		if (!pTmp || pTmp->GetCardType() != CARD_TYPE_RETINUE || !CheckSacrificeForAI(pTmp) || !CheckPlayCard(pTmp))
			continue;

		if (!pCard)
		{
			pCard = pTmp;
		}
		else
		{
			if (pCard->GetDefensePower() < pTmp->GetDefensePower())
				pCard = pTmp;
		}
	}

	return pCard;
}

//获得手牌中具有"冲锋"状态的卡牌
Card* Chief::GetRushCard()
{
	Card* pCard;
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (pCard && pCard->IsRushCard())
			return pCard;
	}

	return NULL;
}

//获得手牌中一张指定子类型的卡牌
// Card* Chief::GetAppointSubTypeCard(CardSubType enCardSubType)
// {
// 	if (enCardSubType <= CARD_SUB_TYPE_NONE || enCardSubType >= CARD_SUB_TYPE_MAX)
// 		return NULL;
// 
// 	Card* pCard = NULL;
// 	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
// 	{
// 		pCard = *iter;
// 		if (pCard && pCard->GetCardSubType() == enCardSubType && CheckPlayCard(pCard))
// 			return pCard;
// 	}
// 
// 	return NULL;
// }

//获得一个攻击力最大的随从
Retinue* Chief::GetMaxAttackRetinue()
{
	Retinue* pRetinue = NULL;
	Retinue* pTmp = NULL;
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pTmp = *iter;
		if (!pTmp || !pTmp->IsEnableAttack())
			continue;

		if (!pRetinue)
		{
			pRetinue = pTmp;
		}
		else
		{
			if (pTmp->GetAttackPower() > pRetinue->GetAttackPower())
				pRetinue = pTmp;
		}
	}

	return pRetinue;
}

//获得敌方攻击力最高的随从
Retinue* Chief::GetEnemyMaxAttackRetinue()
{
	if (!m_pEnemyChief)
		return NULL;

	Retinue* pRetinue = NULL;
	Retinue* pTmp = NULL;
	BYTE byRetinueNum = m_pEnemyChief->GetBattleRetinueNum();
	if (!byRetinueNum)
		return NULL;

	for (BYTE i = 0; i < byRetinueNum; ++i)
	{
		pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
		if (!pTmp || pTmp->GetOrien() != GO_ORIEN_FRONT)
			continue;

		if (!pRetinue)
		{
			pRetinue = pTmp;
		}
		else
		{
			if (pRetinue->GetAttackPower() < pTmp->GetAttackPower())
				pRetinue = pTmp;
		}
	}

	return pRetinue;
}

//获得敌方攻击力最低的随从
Retinue* Chief::GetEnemyMinAttackRetinue()
{
	if (!m_pEnemyChief)
		return NULL;

	Retinue* pRetinue = NULL;
	Retinue* pTmp = NULL;
	BYTE byRetinueNum = m_pEnemyChief->GetBattleRetinueNum();
	if (!byRetinueNum)
		return NULL;

	for (BYTE i = 0; i < byRetinueNum; ++i)
	{
		pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
		if (!pTmp || pTmp->GetOrien() != GO_ORIEN_FRONT)
			continue;

		if (!pRetinue)
		{
			pRetinue = pTmp;
		}
		else
		{
			if (pRetinue->GetAttackPower() > pTmp->GetAttackPower())
				pRetinue = pTmp;
		}
	}

	return pRetinue;
}

//获得一个随机魔法随从
Retinue* Chief::GetRandomMagicRetinue()
{
	BYTE byRetinueNum = GetMagicRetinueNum();
	if (byRetinueNum <= 0)
		return NULL;

	UINT nIndex = Rand::Instance()->urand(0, byRetinueNum - 1);
	return m_MagicRetinueVec[nIndex];
}

//检测献祭(AI用)
BOOL Chief::CheckSacrificeForAI(Card* pCard)
{
	if (!pCard)
		return FALSE;

	CardEntry* pCardEntry = pCard->GetCardEntry();
	if (!pCardEntry)
		return FALSE;

	//需要献祭才能使用
	if (pCardEntry->m_dwSacrificeNum)
	{
		//献祭目标不足
		if (GetAliveBattleRetinueNum() < pCardEntry->m_dwSacrificeNum)
			return FALSE;

		//检测献祭目标条件
		DWORD dwCnt = 0;
		Retinue* pRetinue = NULL;
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pRetinue = *iter;
			if (!pRetinue || pRetinue->IsDead())
				continue;

			//不能献祭自己
			if (pRetinue->GetCardID() == pCard->GetCardID())
				continue;

			//不能献祭攻击大于自己的随从
			if (pRetinue->GetAttackPower() >= pCard->GetAttackPower())
				continue;

			//随从ID条件不满足
			if (pCardEntry->m_dwSacrificeRetinueID && pCardEntry->m_dwSacrificeRetinueID != pRetinue->GetRetinueID())
				continue;

			//随从种族条件不满足
			if (pCardEntry->m_dwSacrificeRetinueRace && pCardEntry->m_dwSacrificeRetinueRace != pRetinue->GetRace())
				continue;

			++dwCnt;
		}

		if (dwCnt < pCardEntry->m_dwSacrificeNum)
			return FALSE;

	}

	return TRUE;
}

//设置献祭目标(AI用)
BOOL Chief::SetSacrificeTargetForAI(Card* pCard, DWORD* dwSacrificeTarget, BYTE& bySacrificeNum)
{
	if (!pCard)
		return FALSE;

	CardEntry* pCardEntry = pCard->GetCardEntry();
	if (!pCardEntry)
		return FALSE;

	if (!pCardEntry->m_dwSacrificeNum)
		return TRUE;

	//献祭目标不足
	if (GetAliveBattleRetinueNum() < pCardEntry->m_dwSacrificeNum)
		return FALSE;

	//检测献祭目标条件
	DWORD    dwCnt = 0;
	Retinue* pRetinue = NULL;
	Retinue* pList[MAX_BATTLE_RETINUE_NUM] = {0};
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue || pRetinue->IsDead())
			continue;

		//不能献祭自己
		if (pRetinue->GetCardID() == pCard->GetCardID())
			continue;

		//不能献祭攻击大于自己的随从
		if (pRetinue->GetAttackPower() >= pCard->GetAttackPower())
			continue;

		//随从ID条件不满足
		if (pCardEntry->m_dwSacrificeRetinueID && pCardEntry->m_dwSacrificeRetinueID != pRetinue->GetRetinueID())
			continue;

		//随从种族条件不满足
		if (pCardEntry->m_dwSacrificeRetinueRace && pCardEntry->m_dwSacrificeRetinueRace != pRetinue->GetRace())
			continue;

		//献祭目标不足时,直接加入临时列表
		if (dwCnt < MAX_BATTLE_RETINUE_NUM && dwCnt < pCardEntry->m_dwSacrificeNum)
		{
			pList[dwCnt++] = pRetinue;
		}
		//献祭目标已够时,按照攻击力筛选已有的目标
		else
		{
			Retinue* pTmp = NULL;
			for (DWORD i = 0; i < dwCnt; ++i)
			{
				//当前献祭目标攻击力小于临时列表总存在的目标时
				if (pRetinue->GetAttackPower() < pList[i]->GetAttackPower())
				{
					//交换目标
					pTmp = pList[i];
					pList[i] = pRetinue;
					pRetinue = pTmp;
				}
			}
		}
	}

	//献祭目标数量不足
	if (dwCnt < pCardEntry->m_dwSacrificeNum)
		return FALSE;


	//设置献祭目标
	bySacrificeNum = (BYTE)dwCnt;
	for (DWORD i = 0; i < dwCnt; ++i)
	{
		if (!pList[i])
			return FALSE;

		dwSacrificeTarget[i] = pList[i]->GetObjectKey();
	}

	return TRUE;
}

//检测使用卡牌(AI用)
BOOL Chief::CheckPlayCard(Card* pCard)
{
// 	if (!m_pEnemyChief)
// 		return FALSE;
// 
// 	if (!pCard)
// 		return FALSE;
// 
// 	//没有卡牌使用的目标
// 	if (pCard->GetTargetType() != 0 && GetTargetForPlayCard(pCard) == 0)
// 		return FALSE;
// 
// 	DWORD dwCardID = pCard->GetCardID();
// 	//当手中可以出的随从牌里面和自己场上的牌里面，有一张高于对方场上最高攻击的怪物时，不会使用法术“炫宇千屠”“天地同归”也不会放置陷阱“戮魂阵”
// 	if (dwCardID == 10000432 || dwCardID == 10000468 || dwCardID == 10000585)
// 	{
// 		Retinue* pEnemyRetinue = GetEnemyMaxAttackRetinue();
// 		if (!pEnemyRetinue)
// 			return FALSE;
// 
// 		Retinue* pRetinue = GetMaxAttackRetinue();
// 		if (pRetinue && pRetinue->GetAttackPower() >= pEnemyRetinue->GetAttackPower())
// 			return FALSE;
// 
// 		Card* pCard = GetMaxAttackCard();
// 		if (pCard && pCard->GetAttackPower() >= pEnemyRetinue->GetAttackPower())
// 			return FALSE;
// 	}
// 
// 	//当我方随从中有任意一个攻击高于对方随从的最高攻击时，不会使用“天诛”、“夺舍”、“生死决”、灵叱、
// 	if (dwCardID == 10000429 || dwCardID == 10000427 || dwCardID == 10000509 || dwCardID == 10000534)
// 	{
// 		Retinue* pEnemyRetinue = GetEnemyMaxAttackRetinue();
// 		if (!pEnemyRetinue)
// 			return FALSE;
// 
// 		Retinue* pRetinue = GetMaxAttackRetinue();
// 		if (pRetinue && pRetinue->GetAttackPower() >= pEnemyRetinue->GetAttackPower())
// 			return FALSE;	
// 	}
// 
// 	//当我方魔法随从数量多于对方时，不使用龙咆和天地同归
// 	if (dwCardID == 10000430 || dwCardID == 10000468)
// 	{
// 		if (GetBattleRetinueNum() > m_pEnemyChief->GetBattleRetinueNum())
// 			return FALSE;
// 	}
// 
// 	//色妖/拉尼娜/声鬼/圣洁/尼诺/波旬,必须在"域外天"环境才能生存
// 	if (dwCardID == 10000021
// 	 || dwCardID == 10000162
// 	 || dwCardID == 10000163
// 	 || dwCardID == 10000319
// 	 || dwCardID == 10000368
// 	 || dwCardID == 10000421)
// 	{
// 		if (m_enBattleEnv != BATTLE_ENV_1)
// 			return FALSE;
// 	}
// 
// 	//归墟之灵/龙宿国师,必须在"无尽海"环境才能生存
// 	if (dwCardID == 10000068
// 	 || dwCardID == 10000173)
// 	{
// 		if (m_enBattleEnv != BATTLE_ENV_4)
// 			return FALSE;
// 	}

	return TRUE;
}

//获得出牌的目标
DWORD Chief::GetTargetForPlayCard(Card* pCard)
{
	if (!pCard)
		return 0;

	Retinue* pRetinue = NULL;
	//卡牌使用目标类型
	switch((CardTargetType)pCard->GetTargetType())
	{
		//我方战斗随从
	case CTT_OWNER_BR:
		{
			//选取攻击力最高的随从
			pRetinue = GetMaxAttackRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//我方魔法随从
	case CTT_OWNER_MR:
		{
			//随机选取魔法随从
			pRetinue = GetRandomMagicRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//敌方战斗随从
	case CTT_ENEMY_BR:
		{
			//选取攻击力最高的随从
			pRetinue = GetEnemyMaxAttackRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//敌方魔法随从
	case CTT_ENEMY_MR:
		{
			if (!m_pEnemyChief || m_pEnemyChief->IsDead())
				return 0;

			//随机选取魔法随从
			pRetinue = m_pEnemyChief->GetRandomMagicRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//我方生命值
	case CTT_OWNER_HP:
		{
			if (GetHp() > GetMaxHp() / 2)
				return 0;

			return GetObjectKey();
		}
		break;

	default:
		return 0;
	}
}

//处理出牌
BOOL Chief::PlayCard(Card* pCard, BYTE bySummonSlot, GameObjectOrien enOrien)
{
	if (!pCard)
		return FALSE;

	DWORD dwTargetKey = 0;

	//需要献祭目标的随从牌
	if (pCard->GetCardType() == CARD_TYPE_RETINUE && pCard->GetSacrificeNum() > 0)
	{
		//获得献祭目标
		DWORD dwSacrificeTarget[MAX_BATTLE_RETINUE_NUM] = {0};
		BYTE  bySacrificeNum = 0;
		if (!SetSacrificeTargetForAI(pCard, dwSacrificeTarget, bySacrificeNum))
			return FALSE;

		HandleUseCardSyn(pCard->GetObjectKey(), dwSacrificeTarget, bySacrificeNum, bySummonSlot, enOrien);
	}
	//其他卡牌
	else
	{
		//需要使用目标
		if (pCard->GetTargetType() != 0)
		{
			dwTargetKey = GetTargetForPlayCard(pCard);
			if (dwTargetKey == 0)
				return FALSE;

			HandleUseCardSyn(pCard->GetObjectKey(), &dwTargetKey, 1, bySummonSlot, enOrien);
		}
		//不需要使用目标
		else
		{
			HandleUseCardSyn(pCard->GetObjectKey(), NULL, 0, bySummonSlot, enOrien);
		}
	}

	return TRUE;
}

//处理使用卡牌
BOOL Chief::HandleUseCardSyn(DWORD dwCardKey, DWORD* pTargetArray, BYTE byTargetNum, BYTE bySummonSlot, GameObjectOrien enOrien)
{
	if (!m_pOwner)
		return FALSE;

	if (!dwCardKey)
		return FALSE;

	if (byTargetNum > 0 && !pTargetArray)
		return FALSE;

	MSG_C2M_BATTLE_USE_CARD_SYN msg;
	msg.m_dwCardKey		= dwCardKey;
	msg.m_byTargetNum	= byTargetNum;
	for (BYTE i = 0; i < byTargetNum && i < MAX_BATTLE_RETINUE_NUM ; ++i)
		msg.m_dwTargetKey[i] = pTargetArray[i];
	msg.m_bySlot		= bySummonSlot;
	msg.m_byOrien		= enOrien;

	m_pOwner->SendPacket(&msg);

	return TRUE;
}

//处理随从翻转
BOOL Chief::HandleTurnRetinueSyn(DWORD dwRetinueKey)
{
	if (!m_pOwner)
		return FALSE;

	if (!dwRetinueKey)
		return FALSE;

	MSG_C2M_BATTLE_TURN_RETINUE_SYN msg;
	msg.m_dwRetinueKey = dwRetinueKey;
	m_pOwner->SendPacket(&msg);

	return TRUE;
}

//处理随从攻击
BOOL Chief::HandleRetinueAttackSyn(DWORD dwRetinueKey, DWORD dwTargetKey)
{
	if (!m_pOwner)
		return FALSE;

	if (!dwRetinueKey || !dwTargetKey)
		return FALSE;

	MSG_C2M_BATTLE_RETINUE_ATTACK_SYN msg;
	msg.m_dwRetinueKey = dwRetinueKey;
	msg.m_dwTargetKey  = dwTargetKey;
	m_pOwner->SendPacket(&msg);

	return TRUE;
}

//处理回合结束
VOID Chief::HandleRoundEndOptCmd()
{
	if (!m_pOwner)
		return;

	MSG_C2M_BATTLE_ROUND_END_OPT_CMD msg;
	m_pOwner->SendPacket(&msg);
}