#include "stdafx.h"
#include "LuaFunRegister.h"
#include "GameObject.h"
//#include "Chief.h"
#include "Player.h"

LuaFunRegister::LuaFunRegister(void)
{

}

LuaFunRegister::~LuaFunRegister(void)
{

}

//�������Ƿ�ΪNULL
DWORD LuaFunRegister::IsNull(VOID* pVoid)
{
	return (DWORD)(pVoid == NULL);
}

//�������Ƿ�ΪNULL
DWORD LuaFunRegister::IsNotNull(VOID* pVoid)
{
	return (DWORD)(pVoid != NULL);
}

//���һ�������
DWORD LuaFunRegister::GetRandom()
{
	return (DWORD) ((double)rand() /(double) RAND_MAX * 100.0) + 1;
}

//���һ�������ObjectKey
DWORD LuaFunRegister::GetObjectKey(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetObjectKey();
}

//���һ�����������
DWORD LuaFunRegister::GetObjectType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetObjectType();
}

//����ObjectKey���Ҷ���
VOID* LuaFunRegister::FindObject(VOID* pVoid, DWORD dwObjectKey)
{
	if (!pVoid || !dwObjectKey)
		return NULL;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->FindFromGameObjectMap(dwObjectKey);
}

//��ö��������
DWORD LuaFunRegister::GetRace(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetRace();
	return 0;
}

//��ö�����ɫ(ϡ�ж�)
DWORD LuaFunRegister::GetColor(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetColor();
}

//��ö���ȼ�(�Ǽ�)
DWORD LuaFunRegister::GetLevel(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetLevel();
}

//�����ӵ�ID
DWORD LuaFunRegister::GetRetinueID(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueID();
}

//�����ӵĿ���ID
DWORD LuaFunRegister::GetReitnueCardID(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetReitnueCardID();
}

//�����ӵ�����
DWORD LuaFunRegister::GetRetinueType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueType();
}

//�����ӵ�������
DWORD LuaFunRegister::GetRetinueSubType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueSubType();
}

//��ö��������ֵ
INT LuaFunRegister::GetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetAttr(dwAttrType, dwAttrKind);
}

//���ö��������ֵ
VOID LuaFunRegister::SetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->SetAttr(dwAttrType, dwAttrKind, nValue);
}

//���Ӷ��������ֵ
VOID LuaFunRegister::AddAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->AddAttr(dwAttrType, dwAttrKind, nValue);
}

//������������ֵ
VOID LuaFunRegister::ClearAttr(VOID* pVoid, DWORD dwAttrType)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->ClearAttr(dwAttrType);
}

//�����Ƿ����ĳ��״̬
DWORD LuaFunRegister::HasStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->HasStatus((GameObjectStatus)dwStatus);
}

//���ö����״̬
VOID LuaFunRegister::SetStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->SetStatus((GameObjectStatus)dwStatus);
}

//��������״̬
VOID LuaFunRegister::ClearStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->ClearStatus((GameObjectStatus)dwStatus);
}

//�����Ƿ�Ϊս�����
DWORD LuaFunRegister::IsBattleRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsBattleRetinue();
}

//�����Ƿ�Ϊħ�����
DWORD LuaFunRegister::IsMagicRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsMagicRetinue();
}

//�����Ƿ�Ϊ�������
DWORD LuaFunRegister::IsSpellRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsSpellRetinue();
}

//�����Ƿ�Ϊ�������
DWORD LuaFunRegister::IsTrapRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsTrapRetinue();
}

//�����Ƿ�Ϊװ�����
DWORD LuaFunRegister::IsEquipmentRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsEquipmentRetinue();
}

//�����Ƿ�Ϊ�������
DWORD LuaFunRegister::IsEnvRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsEnvRetinue();
}

//�����Ƿ���
DWORD LuaFunRegister::IsAlive(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsAlive();
	return 0;
}

//�����Ƿ�����
DWORD LuaFunRegister::IsDead(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsDead();
	return 0;
}

//�����Ƿ��ѷ���Ӫ
DWORD LuaFunRegister::IsFriend(VOID* pVoid, VOID* pTarget)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsFriend((GameObject*)pTarget);
}

//���ö�������
VOID LuaFunRegister::SetDead(VOID* pVoid, VOID* pMurderer)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->SetDead((GameObject*)pMurderer);
}

//��ü����������ָ��
VOID* LuaFunRegister::GetChief(VOID* pVoid)
{
	if (!pVoid)
		return NULL;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (VOID*)pGameObject->GetChief();
}

//��õз��������ָ��
VOID* LuaFunRegister::GetEnemyChief(VOID* pVoid)
{
	return NULL;

// 	if (!pVoid)
// 		return NULL;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	return (VOID*)pChief->GetEnemyChief();
}

//���Ӽ�������HP
VOID LuaFunRegister::AddChiefHp(VOID* pVoid, INT nVal)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddChiefHp(nVal);
}

//���ӵз�����HP
VOID LuaFunRegister::AddEnemyChiefHp(VOID* pVoid, INT nVal)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddEnemyChiefHp(nVal);
}

//����ҷ�����ս���������
DWORD LuaFunRegister::GetBattleRetinueNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetBattleRetinueNum();
}

//����ҷ�����ħ���������
DWORD LuaFunRegister::GetMagicRetinueNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetMagicRetinueNum();
}

//��õз�����ս���������
DWORD LuaFunRegister::GetEnemyBattleRetinueNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return 0;
// 
// 	return pEnemyChief->GetBattleRetinueNum();
}

//��õз�����ħ���������
DWORD LuaFunRegister::GetEnemyMagicRetinueNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return 0;
// 
// 	return pEnemyChief->GetMagicRetinueNum();
}

//����ҷ�ս�����ָ��
VOID* LuaFunRegister::GetBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex)
{
//	if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	return (VOID*)pChief->GetBattleRetinueByIndex((BYTE)dwIndex);
}

//����ҷ�ħ�����ָ��
VOID* LuaFunRegister::GetMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	return (VOID*)pChief->GetMagicRetinueByIndex((BYTE)dwIndex);
}

//��õз�ս�����ָ��
VOID* LuaFunRegister::GetEnemyBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return NULL;
// 
// 	return (VOID*)pEnemyChief->GetBattleRetinueByIndex((BYTE)dwIndex);
}

//��õз�ħ�����ָ��
VOID* LuaFunRegister::GetEnemyMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return NULL;
// 
// 	return (VOID*)pEnemyChief->GetMagicRetinueByIndex((BYTE)dwIndex);
}

//����ҷ�Ĺ�ؿ�������
DWORD LuaFunRegister::GetGraveCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetGraveCardNum();
}

//����ҷ�Ĺ�ؿ���ָ��
VOID* LuaFunRegister::GetGraveCardByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	return (VOID*)pChief->GetGraveCardByIndex((BYTE)dwIndex);
}

//��õз�Ĺ�ؿ�������
DWORD LuaFunRegister::GetEnemyGraveCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return 0;
// 
// 	return pEnemyChief->GetGraveCardNum();
}

//��õз�Ĺ�ؿ���ָ��
VOID* LuaFunRegister::GetEnemyGraveCardByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return NULL;
// 
// 	return (VOID*)pEnemyChief->GetGraveCardByIndex((BYTE)dwIndex);
}

//����ҷ���ǰ��������
DWORD LuaFunRegister::GetHandCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetHandCardNum();
}

//����ҷ�����ָ��
VOID* LuaFunRegister::GetHandCardByIndex(VOID* pVoid, DWORD dwIndex)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return NULL;
// 
// 	return (VOID*)pChief->GetHandCardByIndex((BYTE)dwIndex);
}

//����ҷ���ǰ���鿨������
DWORD LuaFunRegister::GetGroupCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetGroupCardNum();
}

//��õз���ǰ��������
DWORD LuaFunRegister::GetEnemyHandCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return 0;
// 
// 	return pEnemyChief->GetHandCardNum();
}

//��õз���ǰ���鿨������
DWORD LuaFunRegister::GetEnemyGroupCardNum(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return 0;
// 
// 	return pEnemyChief->GetGroupCardNum();
}

//��ð�Ŀ�����
VOID* LuaFunRegister::GetBindObject(VOID* pVoid)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetBindObject();
}

//��һ��Ŀ�����
VOID LuaFunRegister::SetBindObject(VOID* pVoid, VOID* pTarget)
{
	//if (!pVoid || !pTarget)
	//	return;

	//GameObject* pGameObject = (GameObject*)pVoid;
	//pGameObject->SetBindObject((GameObject*)pTarget);
}

//���Ŀ���
VOID LuaFunRegister::ClearBind(VOID* pVoid)
{
	//if (!pVoid)
	//	return;

	//GameObject* pGameObject = (GameObject*)pVoid;
	//pGameObject->ClearBind();
}

//�Ƿ񱻴�����
DWORD LuaFunRegister::IsTrigger(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsTrigger();
}

//���ô�����־
VOID LuaFunRegister::SetTrigger(VOID* pVoid)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetTrigger();
}

//��ýű�������
INT LuaFunRegister::GetScriptData(VOID* pVoid, BYTE byIndex)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetScriptData(byIndex);
}

//���ýű�������
VOID LuaFunRegister::SetScriptData(VOID* pVoid, BYTE byIndex, INT nVal)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetScriptData(byIndex, nVal);
}

//����һ���⻷
VOID LuaFunRegister::AddAura(VOID* pVoid, DWORD dwAuraID)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddAura(dwAuraID);
}

//����׼��������
DWORD LuaFunRegister::GetSacrificeNum(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetSacrificeNum();
}

//��������
VOID LuaFunRegister::DiscardHandCard(VOID* pVoid, BYTE byCardNum)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->DiscardHandCard(byCardNum);
}

//���Ŀ���Ƿ���������
DWORD LuaFunRegister::IsConformCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsConformCond(byCondType, byCondSign, dwCondVal);
}

//���ָ�������Ŀ��鿨������
BYTE LuaFunRegister::GetGroupCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
//	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetGroupCardNumByCond(byCondType, byCondSign, dwCondVal);
}

//���ָ����������������
BYTE LuaFunRegister::GetHandCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetHandCardNumByCond(byCondType, byCondSign, dwCondVal);
}

//���ָ��������Ĺ�ؿ�������
BYTE LuaFunRegister::GetGraveCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return 0;
// 
// 	return pChief->GetGraveCardNumByCond(byCondType, byCondSign, dwCondVal);
}

//��ָ����������
VOID LuaFunRegister::DrawCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->DrawCardByCond(byCardNum, byCondType, byCondSign, dwCondVal);
}

//����ָ������������
VOID LuaFunRegister::DiscardHandCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->DiscardHandCardByCond(byCardNum, byCondType, byCondSign, dwCondVal);
}

//����ָ�������Ŀ����ڿ���
VOID LuaFunRegister::DiscardGroupCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->DiscardGroupCardByCond(byCardNum, byCondType, byCondSign, dwCondVal);
}

//����Ĺ�ؿ���
VOID LuaFunRegister::ReliveCard(VOID* pVoid, DWORD dwCardKey, BYTE byRelivePos, DWORD dwIsFirstDraw)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->ReliveCard(dwCardKey, (RelivePos)byRelivePos, (BOOL)dwIsFirstDraw);
}

//�������ٻ����
VOID LuaFunRegister::SummonRetinueFromHand(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->SummonRetinueFromHand(byNum, byCondType, byCondSign, dwCondVal, byOrien);
}

//�ӿ����ٻ����
VOID LuaFunRegister::SummonRetinueFromGroup(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->SummonRetinueFromGroup(byNum, byCondType, byCondSign, dwCondVal, byOrien);
}

//�������
VOID LuaFunRegister::ControlRetinue(VOID* pVoid, VOID* pTarget)
{
	//if (!pVoid)
		return;

// 	if (!pTarget || ((GameObject*)pTarget)->GetObjectType() != OBJECT_RETINUE)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->ControlRetinue((Retinue*)pTarget);
}

//��ת���
VOID LuaFunRegister::TurnRetinue(VOID* pVoid, DWORD dwIsTriggerScript)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->OnTurn((BOOL)dwIsTriggerScript);
}

//�������
VOID LuaFunRegister::CoverRetinue(VOID* pVoid, DWORD dwIsTriggerScript)
{
	//if (!pVoid)
		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->OnCover((BOOL)dwIsTriggerScript);
}

//�˻����
VOID LuaFunRegister::SendBackRetinue(VOID* pVoid, VOID* pTarget, BYTE bySendBackPos, DWORD dwIsFirstDraw)
{
	//if (!pVoid)
		return;

// 	if (!pTarget || ((GameObject*)pTarget)->GetObjectType() != OBJECT_RETINUE)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	Chief* pChief = pGameObject->GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->SendBackRetinue((Retinue*)pTarget, (SendBackPos)bySendBackPos, (BOOL)dwIsFirstDraw);
}

//�ͷŷ���
VOID LuaFunRegister::CastSpell(VOID* pVoid, DWORD dwSpellID, DWORD dwTargetKey)
{
	//if (!pVoid)
		return;

// 	if (!dwSpellID)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	if (dwTargetKey)
// 		pGameObject->CastSpell(dwSpellID, &dwTargetKey, 1, TRUE);
// 	else
// 		pGameObject->CastSpell(dwSpellID, NULL, 0, TRUE);
}

//���ս������
DWORD LuaFunRegister::GetEnv(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetEnv();
}

//����ս������
VOID LuaFunRegister::SetEnv(VOID* pVoid, DWORD dwEnv)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetEnv(dwEnv);
}

//���ս������
VOID LuaFunRegister::ClearEnv(VOID* pVoid)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->ClearEnv();
}

//����Ƿ��ȡָ��������
bool LuaFunRegister::IsAcceptMission(VOID* pVoid, DWORD dwMissionID)
{
//	if (!pVoid)
		return 0;
// 
// 	Player* pGameObject = (Player*)pVoid;
// 
// 	return pGameObject->IsAcceptMission(dwMissionID) ? true : false;
}

//����һ������
void LuaFunRegister::TriggerMission(VOID* pVoid, DWORD dwMissionID)
{
//	if (!pVoid)
		return ;

// 	Player* pGameObject = (Player*)pVoid;
// 
// 
// 	pGameObject->TriggerMission(dwMissionID);
}