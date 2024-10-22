#include "stdafx.h"
#include "Chief.h"
#include "Retinue.h"
#include "Card.h"
#include "RobotManager.h"
#include "Robot.h"
#include "BattleFactory.h"
#include "../ToolKit/Rand.h"

#define AI_PREVIEW_TIME		1500		//�ͻ���AI����ʱ��(����)

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

	//���û���ĺ���
	if (!GameObject::Init(pRobot))
		return FALSE;

	return TRUE;
}

VOID Chief::Release()
{
	Card* pCard = NULL;
	Retinue* pRetinue = NULL;
	//�ͷ�����
	for (CardVecIter iter = m_HandCardVec.begin(); iter != m_HandCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		pCard->Release();
		BattleFactory::Instance()->FreeCard(pCard);
	}
	m_HandCardVec.clear();

	//�ͷ�Ĺ��
	for (CardVecIter iter = m_GraveCardVec.begin(); iter != m_GraveCardVec.end(); ++iter)
	{
		pCard = *iter;
		if (!pCard)
			continue;

		pCard->Release();
		BattleFactory::Instance()->FreeCard(pCard);
	}
	m_GraveCardVec.clear();

	//�ͷ����
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

	//�����û���ĺ���
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

//����Key����������
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

//����Key������Ĺ�ؿ���
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

//���뿨�Ƶ�������
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

//���������Ƴ�����
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

//���뿨�Ƶ�Ĺ����
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

//��Ĺ�����Ƴ�����
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

//��ô���ս���������
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

//����һ�����
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

//����Key������ս�����
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

//����Key���Ƴ�ս�����
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

//����Key������ħ�����
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

//����Key���Ƴ�ħ�����
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


//������������¼�
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

//����ս��AI
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

//�������AI
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

	//�������
	BYTE byProcessTimes = 0;
	Card* pCard = NULL;
	//1 ʹ���ٹ�������
	pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_ATTACK_SPELL);
	if (pCard)
	{
		if (PlayCard(pCard, 0, GO_ORIEN_FRONT))
			++byProcessTimes;
	}

	//2 ʹ�����忨��
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

	//3 ʹ�÷����ٻ�ħ�����
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

	//4 ʹ����ӿ���
	//��õз�������������
	Retinue* pRetinue = GetEnemyMaxAttackRetinue();
	if (GetBattleRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		//������Ϊ��һ���غ�
		if (m_bFirstHand && m_shRoundNum == 1)
		{
			//�����߷���������
			//pCard = GetMaxDefenseCard();
			//������ֿ���
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
		//�����غ�
		else
		{
			//�Է���Ӵ���
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
						//��þ���"���"״̬�Ŀ���
						pCard = GetRushCard();
						if (pCard)
						{
							if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_FRONT))
								++byProcessTimes;
						}
						else
						{
							//�����߷���������
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
			//�Է���Ӳ�����
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

	//5 ʹ�÷����ٻ�ս�����
	if (GetBattleRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_BR_SPELL);
		if (pCard)
		{
			if (PlayCard(pCard, GetBattleRetinueNum(), GO_ORIEN_BACK))
				++byProcessTimes;
		}
	}

	//6 ʹ��װ������
	if (GetMagicRetinueNum() < MAX_BATTLE_RETINUE_NUM)
	{
		pCard = GetAppointSubTypeCard(CARD_SUB_TYPE_EQUIPMENT);
		if (pCard)
		{
			if (PlayCard(pCard, GetMagicRetinueNum(), GO_ORIEN_FRONT))
				++byProcessTimes;
		}
	}

	//�������ּ�ʱ��
	if (byProcessTimes)
		m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

	//�л�AI״̬
	m_enAIStat = AI_STAT_ATTACK_1;
}

//������AI
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
	//�������з��������
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pAttacker = *iter;
			if (!pAttacker || !pAttacker->IsEnableAttack())
				continue;

			//ѡ��һ�����ʵĹ���Ŀ��
			Retinue* pTarget = NULL;
			Retinue* pTmp = NULL;
			BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
			for (BYTE i  = 0; i < byTargetNum; ++i)
			{
				pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
				if (!pTmp || pTmp->IsDead() || pTmp->GetOrien() == GO_ORIEN_BACK)
					continue;

				//�ݶ�Ϊ����Ŀ��
				if (pAttacker->GetAttackPower() >= pTmp->GetAttackPower())
				{
					//�����й���Ŀ��Ƚ�
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

			//������Ŀ��
			if (pTarget)
			{
				//�ȷ�ת����
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

				//�������ּ�ʱ��
				if (byProcessTimes)
					m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

				//�����������й�������,ÿ�������һ��,���蹥���������ͬ���Ĵ���ʱ��
				return;
			}
		}
	}

	//�л�AI״̬
	m_enAIStat = AI_STAT_ATTACK_2;
}

//������AI
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
	//�������з��������
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
		Retinue* pTarget = NULL;
		for (BYTE i = 0; i < byTargetNum; ++i)
		{
			pTarget = m_pEnemyChief->GetBattleRetinueByIndex(i);
			if (!pTarget || pTarget->IsDead() || pTarget->GetOrien() != GO_ORIEN_BACK)
				continue;

			//ѡ��ǰ�ܹ��ж��ҹ�������ߵ���ӹ���Ŀ��
			Retinue* pAttacker = GetMaxAttackRetinue();
			if (!pAttacker)
				break;

			//�ȷ�ת����
			if (pAttacker->GetOrien() == GO_ORIEN_BACK)
			{
				if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
				{
					m_enAIStat = AI_STAT_ATTACK_3;
					return;
				}
				++byProcessTimes;
			}

			//������Ŀ��
			if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), pTarget->GetObjectKey()))
			{
				m_enAIStat = AI_STAT_ATTACK_3;
				return;
			}
			++byProcessTimes;

			//�������ּ�ʱ��
			if (byProcessTimes)
				m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

			//�����������й�������,ÿ�������һ��,���蹥���������ͬ���Ĵ���ʱ��
			return;
		}
	}

	//�л�AI״̬
	m_enAIStat = AI_STAT_ATTACK_3;
}

//������AI
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
	//�ٴδ������з��������
	if (m_pEnemyChief->GetAliveBattleRetinueNum() > 0)
	{
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pAttacker = *iter;
			if (!pAttacker || !pAttacker->IsEnableAttack())
				continue;

			//ѡ��һ�����ʵĹ���Ŀ��
			Retinue* pTarget = NULL;
			Retinue* pTmp = NULL;
			BYTE byTargetNum = m_pEnemyChief->GetBattleRetinueNum();
			for (BYTE i  = 0; i < byTargetNum; ++i)
			{
				pTmp = m_pEnemyChief->GetBattleRetinueByIndex(i);
				if (!pTmp || pTmp->IsDead() || pTmp->GetOrien() == GO_ORIEN_BACK)
					continue;

				//�ݶ�Ϊ����Ŀ��
				if (pAttacker->GetAttackPower() >= pTmp->GetAttackPower())
				{
					//�����й���Ŀ��Ƚ�
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

			//������Ŀ��
			if (pTarget)
			{
				//�ȷ�ת����
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

				//�������ּ�ʱ��
				if (byProcessTimes)
					m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

				//�����������й�������,ÿ�������һ��,���蹥���������ͬ���Ĵ���ʱ��
				return;
			}
		}
	}

	//�л�AI״̬
	m_enAIStat = AI_STAT_ATTACK_4;
}

//������AI
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
	//4 �����з�����
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		//�з���������
		if (m_pEnemyChief->IsDead())
			break;

		pAttacker = *iter;
		//�����߲����ڻ��߹����߲��ܽ��й���
		if (!pAttacker || !pAttacker->IsEnableAttack())
			continue;

		//�����ߴ���"����"״̬,���ܹ�������
		if (pAttacker->HasStatus(GO_STATUS_RESTRAIN))
			continue;

		//�з����ϴ�������ҹ�����û��"���"״̬ʱ,���ܹ�������
		if (byRetinueNum > 0 && !pAttacker->HasStatus(GO_STATUS_RUSH))
			continue;

		//�ȷ�ת����
		if (pAttacker->GetOrien() == GO_ORIEN_BACK)
		{
			if (!HandleTurnRetinueSyn(pAttacker->GetObjectKey()))
			{
				m_enAIStat = AI_STAT_FINISH;
				return;
			}
			++byProcessTimes;
		}

		//����������
		if (!HandleRetinueAttackSyn(pAttacker->GetObjectKey(), m_pEnemyChief->GetObjectKey()))
		{
			m_enAIStat = AI_STAT_FINISH;
			return;
		}
		++byProcessTimes;

		//�������ּ�ʱ��
		if (byProcessTimes)
			m_AITimer.SetTimer(AI_PREVIEW_TIME * byProcessTimes);

		//�����������й�������,ÿ�������һ��,���蹥���������ͬ���Ĵ���ʱ��
		return;
	}


	//�л�AI״̬
	m_enAIStat = AI_STAT_FINISH;
}

//�������AI
VOID Chief::ProcessFinishAI()
{
	if (m_enAIStat != AI_STAT_FINISH)
		return;

	m_enAIStat = AI_STAT_INIT;

	//��������غ�
	HandleRoundEndOptCmd();
}

//������ֿ���
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

//��������й�������ߵĿ���
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

//��������з�������ߵĿ���
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

//��������о���"���"״̬�Ŀ���
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

//���������һ��ָ�������͵Ŀ���
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

//���һ���������������
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

//��õз���������ߵ����
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

//��õз���������͵����
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

//���һ�����ħ�����
Retinue* Chief::GetRandomMagicRetinue()
{
	BYTE byRetinueNum = GetMagicRetinueNum();
	if (byRetinueNum <= 0)
		return NULL;

	UINT nIndex = Rand::Instance()->urand(0, byRetinueNum - 1);
	return m_MagicRetinueVec[nIndex];
}

//����׼�(AI��)
BOOL Chief::CheckSacrificeForAI(Card* pCard)
{
	if (!pCard)
		return FALSE;

	CardEntry* pCardEntry = pCard->GetCardEntry();
	if (!pCardEntry)
		return FALSE;

	//��Ҫ�׼�����ʹ��
	if (pCardEntry->m_dwSacrificeNum)
	{
		//�׼�Ŀ�겻��
		if (GetAliveBattleRetinueNum() < pCardEntry->m_dwSacrificeNum)
			return FALSE;

		//����׼�Ŀ������
		DWORD dwCnt = 0;
		Retinue* pRetinue = NULL;
		for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
		{
			pRetinue = *iter;
			if (!pRetinue || pRetinue->IsDead())
				continue;

			//�����׼��Լ�
			if (pRetinue->GetCardID() == pCard->GetCardID())
				continue;

			//�����׼����������Լ������
			if (pRetinue->GetAttackPower() >= pCard->GetAttackPower())
				continue;

			//���ID����������
			if (pCardEntry->m_dwSacrificeRetinueID && pCardEntry->m_dwSacrificeRetinueID != pRetinue->GetRetinueID())
				continue;

			//�����������������
			if (pCardEntry->m_dwSacrificeRetinueRace && pCardEntry->m_dwSacrificeRetinueRace != pRetinue->GetRace())
				continue;

			++dwCnt;
		}

		if (dwCnt < pCardEntry->m_dwSacrificeNum)
			return FALSE;

	}

	return TRUE;
}

//�����׼�Ŀ��(AI��)
BOOL Chief::SetSacrificeTargetForAI(Card* pCard, DWORD* dwSacrificeTarget, BYTE& bySacrificeNum)
{
	if (!pCard)
		return FALSE;

	CardEntry* pCardEntry = pCard->GetCardEntry();
	if (!pCardEntry)
		return FALSE;

	if (!pCardEntry->m_dwSacrificeNum)
		return TRUE;

	//�׼�Ŀ�겻��
	if (GetAliveBattleRetinueNum() < pCardEntry->m_dwSacrificeNum)
		return FALSE;

	//����׼�Ŀ������
	DWORD    dwCnt = 0;
	Retinue* pRetinue = NULL;
	Retinue* pList[MAX_BATTLE_RETINUE_NUM] = {0};
	for (RetinueVecIter iter = m_BattleRetinueVec.begin(); iter != m_BattleRetinueVec.end(); ++iter)
	{
		pRetinue = *iter;
		if (!pRetinue || pRetinue->IsDead())
			continue;

		//�����׼��Լ�
		if (pRetinue->GetCardID() == pCard->GetCardID())
			continue;

		//�����׼����������Լ������
		if (pRetinue->GetAttackPower() >= pCard->GetAttackPower())
			continue;

		//���ID����������
		if (pCardEntry->m_dwSacrificeRetinueID && pCardEntry->m_dwSacrificeRetinueID != pRetinue->GetRetinueID())
			continue;

		//�����������������
		if (pCardEntry->m_dwSacrificeRetinueRace && pCardEntry->m_dwSacrificeRetinueRace != pRetinue->GetRace())
			continue;

		//�׼�Ŀ�겻��ʱ,ֱ�Ӽ�����ʱ�б�
		if (dwCnt < MAX_BATTLE_RETINUE_NUM && dwCnt < pCardEntry->m_dwSacrificeNum)
		{
			pList[dwCnt++] = pRetinue;
		}
		//�׼�Ŀ���ѹ�ʱ,���չ�����ɸѡ���е�Ŀ��
		else
		{
			Retinue* pTmp = NULL;
			for (DWORD i = 0; i < dwCnt; ++i)
			{
				//��ǰ�׼�Ŀ�깥����С����ʱ�б��ܴ��ڵ�Ŀ��ʱ
				if (pRetinue->GetAttackPower() < pList[i]->GetAttackPower())
				{
					//����Ŀ��
					pTmp = pList[i];
					pList[i] = pRetinue;
					pRetinue = pTmp;
				}
			}
		}
	}

	//�׼�Ŀ����������
	if (dwCnt < pCardEntry->m_dwSacrificeNum)
		return FALSE;


	//�����׼�Ŀ��
	bySacrificeNum = (BYTE)dwCnt;
	for (DWORD i = 0; i < dwCnt; ++i)
	{
		if (!pList[i])
			return FALSE;

		dwSacrificeTarget[i] = pList[i]->GetObjectKey();
	}

	return TRUE;
}

//���ʹ�ÿ���(AI��)
BOOL Chief::CheckPlayCard(Card* pCard)
{
// 	if (!m_pEnemyChief)
// 		return FALSE;
// 
// 	if (!pCard)
// 		return FALSE;
// 
// 	//û�п���ʹ�õ�Ŀ��
// 	if (pCard->GetTargetType() != 0 && GetTargetForPlayCard(pCard) == 0)
// 		return FALSE;
// 
// 	DWORD dwCardID = pCard->GetCardID();
// 	//�����п��Գ��������������Լ����ϵ������棬��һ�Ÿ��ڶԷ�������߹����Ĺ���ʱ������ʹ�÷���������ǧ���������ͬ�顱Ҳ����������塰¾����
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
// 	//���ҷ������������һ���������ڶԷ���ӵ���߹���ʱ������ʹ�á�����������ᡱ����������������߳��
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
// 	//���ҷ�ħ������������ڶԷ�ʱ����ʹ�����غ����ͬ��
// 	if (dwCardID == 10000430 || dwCardID == 10000468)
// 	{
// 		if (GetBattleRetinueNum() > m_pEnemyChief->GetBattleRetinueNum())
// 			return FALSE;
// 	}
// 
// 	//ɫ��/������/����/ʥ��/��ŵ/��Ѯ,������"������"������������
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
// 	//����֮��/���޹�ʦ,������"�޾���"������������
// 	if (dwCardID == 10000068
// 	 || dwCardID == 10000173)
// 	{
// 		if (m_enBattleEnv != BATTLE_ENV_4)
// 			return FALSE;
// 	}

	return TRUE;
}

//��ó��Ƶ�Ŀ��
DWORD Chief::GetTargetForPlayCard(Card* pCard)
{
	if (!pCard)
		return 0;

	Retinue* pRetinue = NULL;
	//����ʹ��Ŀ������
	switch((CardTargetType)pCard->GetTargetType())
	{
		//�ҷ�ս�����
	case CTT_OWNER_BR:
		{
			//ѡȡ��������ߵ����
			pRetinue = GetMaxAttackRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//�ҷ�ħ�����
	case CTT_OWNER_MR:
		{
			//���ѡȡħ�����
			pRetinue = GetRandomMagicRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//�з�ս�����
	case CTT_ENEMY_BR:
		{
			//ѡȡ��������ߵ����
			pRetinue = GetEnemyMaxAttackRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//�з�ħ�����
	case CTT_ENEMY_MR:
		{
			if (!m_pEnemyChief || m_pEnemyChief->IsDead())
				return 0;

			//���ѡȡħ�����
			pRetinue = m_pEnemyChief->GetRandomMagicRetinue();
			if (!pRetinue)
				return 0;

			return pRetinue->GetObjectKey();
		}
		break;

		//�ҷ�����ֵ
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

//�������
BOOL Chief::PlayCard(Card* pCard, BYTE bySummonSlot, GameObjectOrien enOrien)
{
	if (!pCard)
		return FALSE;

	DWORD dwTargetKey = 0;

	//��Ҫ�׼�Ŀ��������
	if (pCard->GetCardType() == CARD_TYPE_RETINUE && pCard->GetSacrificeNum() > 0)
	{
		//����׼�Ŀ��
		DWORD dwSacrificeTarget[MAX_BATTLE_RETINUE_NUM] = {0};
		BYTE  bySacrificeNum = 0;
		if (!SetSacrificeTargetForAI(pCard, dwSacrificeTarget, bySacrificeNum))
			return FALSE;

		HandleUseCardSyn(pCard->GetObjectKey(), dwSacrificeTarget, bySacrificeNum, bySummonSlot, enOrien);
	}
	//��������
	else
	{
		//��Ҫʹ��Ŀ��
		if (pCard->GetTargetType() != 0)
		{
			dwTargetKey = GetTargetForPlayCard(pCard);
			if (dwTargetKey == 0)
				return FALSE;

			HandleUseCardSyn(pCard->GetObjectKey(), &dwTargetKey, 1, bySummonSlot, enOrien);
		}
		//����Ҫʹ��Ŀ��
		else
		{
			HandleUseCardSyn(pCard->GetObjectKey(), NULL, 0, bySummonSlot, enOrien);
		}
	}

	return TRUE;
}

//����ʹ�ÿ���
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

//������ӷ�ת
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

//������ӹ���
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

//����غϽ���
VOID Chief::HandleRoundEndOptCmd()
{
	if (!m_pOwner)
		return;

	MSG_C2M_BATTLE_ROUND_END_OPT_CMD msg;
	m_pOwner->SendPacket(&msg);
}