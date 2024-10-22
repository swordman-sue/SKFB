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

//检测对象是否为NULL
DWORD LuaFunRegister::IsNull(VOID* pVoid)
{
	return (DWORD)(pVoid == NULL);
}

//检测对象是否不为NULL
DWORD LuaFunRegister::IsNotNull(VOID* pVoid)
{
	return (DWORD)(pVoid != NULL);
}

//获得一个随机数
DWORD LuaFunRegister::GetRandom()
{
	return (DWORD) ((double)rand() /(double) RAND_MAX * 100.0) + 1;
}

//获得一个对象的ObjectKey
DWORD LuaFunRegister::GetObjectKey(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetObjectKey();
}

//获得一个对象的类型
DWORD LuaFunRegister::GetObjectType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetObjectType();
}

//根据ObjectKey查找对象
VOID* LuaFunRegister::FindObject(VOID* pVoid, DWORD dwObjectKey)
{
	if (!pVoid || !dwObjectKey)
		return NULL;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->FindFromGameObjectMap(dwObjectKey);
}

//获得对象的种族
DWORD LuaFunRegister::GetRace(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetRace();
	return 0;
}

//获得对象颜色(稀有度)
DWORD LuaFunRegister::GetColor(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetColor();
}

//获得对象等级(星级)
DWORD LuaFunRegister::GetLevel(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetLevel();
}

//获得随从的ID
DWORD LuaFunRegister::GetRetinueID(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueID();
}

//获得随从的卡牌ID
DWORD LuaFunRegister::GetReitnueCardID(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetReitnueCardID();
}

//获得随从的类型
DWORD LuaFunRegister::GetRetinueType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueType();
}

//获得随从的子类型
DWORD LuaFunRegister::GetRetinueSubType(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetRetinueSubType();
}

//获得对象的属性值
INT LuaFunRegister::GetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetAttr(dwAttrType, dwAttrKind);
}

//设置对象的属性值
VOID LuaFunRegister::SetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->SetAttr(dwAttrType, dwAttrKind, nValue);
}

//增加对象的属性值
VOID LuaFunRegister::AddAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->AddAttr(dwAttrType, dwAttrKind, nValue);
}

//清除对象的属性值
VOID LuaFunRegister::ClearAttr(VOID* pVoid, DWORD dwAttrType)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->ClearAttr(dwAttrType);
}

//对象是否持有某种状态
DWORD LuaFunRegister::HasStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->HasStatus((GameObjectStatus)dwStatus);
}

//设置对象的状态
VOID LuaFunRegister::SetStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->SetStatus((GameObjectStatus)dwStatus);
}

//清除对象的状态
VOID LuaFunRegister::ClearStatus(VOID* pVoid, DWORD dwStatus)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->ClearStatus((GameObjectStatus)dwStatus);
}

//对象是否为战斗随从
DWORD LuaFunRegister::IsBattleRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsBattleRetinue();
}

//对象是否为魔法随从
DWORD LuaFunRegister::IsMagicRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsMagicRetinue();
}

//对象是否为法术随从
DWORD LuaFunRegister::IsSpellRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsSpellRetinue();
}

//对象是否为陷阱随从
DWORD LuaFunRegister::IsTrapRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsTrapRetinue();
}

//对象是否为装备随从
DWORD LuaFunRegister::IsEquipmentRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsEquipmentRetinue();
}

//对象是否为环境随从
DWORD LuaFunRegister::IsEnvRetinue(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsEnvRetinue();
}

//对象是否存活
DWORD LuaFunRegister::IsAlive(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsAlive();
	return 0;
}

//对象是否死亡
DWORD LuaFunRegister::IsDead(VOID* pVoid)
{
	if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsDead();
	return 0;
}

//对象是否友方阵营
DWORD LuaFunRegister::IsFriend(VOID* pVoid, VOID* pTarget)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsFriend((GameObject*)pTarget);
}

//设置对象死亡
VOID LuaFunRegister::SetDead(VOID* pVoid, VOID* pMurderer)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->SetDead((GameObject*)pMurderer);
}

//获得己方首领对象指针
VOID* LuaFunRegister::GetChief(VOID* pVoid)
{
	if (!pVoid)
		return NULL;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (VOID*)pGameObject->GetChief();
}

//获得敌方首领对象指针
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

//增加己方首领HP
VOID LuaFunRegister::AddChiefHp(VOID* pVoid, INT nVal)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddChiefHp(nVal);
}

//增加敌方首领HP
VOID LuaFunRegister::AddEnemyChiefHp(VOID* pVoid, INT nVal)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddEnemyChiefHp(nVal);
}

//获得我方场上战斗随从数量
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

//获得我方场上魔法随从数量
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

//获得敌方场上战斗随从数量
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

//获得敌方场上魔法随从数量
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

//获得我方战斗随从指针
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

//获得我方魔法随从指针
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

//获得敌方战斗随从指针
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

//获得敌方魔法随从指针
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

//获得我方墓地卡牌数量
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

//获得我方墓地卡牌指针
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

//获得敌方墓地卡牌数量
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

//获得敌方墓地卡牌指针
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

//获得我方当前手牌数量
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

//获得我方手牌指针
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

//获得我方当前牌组卡牌数量
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

//获得敌方当前手牌数量
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

//获得敌方当前牌组卡牌数量
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

//获得绑定目标对象
VOID* LuaFunRegister::GetBindObject(VOID* pVoid)
{
	//if (!pVoid)
		return NULL;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetBindObject();
}

//绑定一个目标对象
VOID LuaFunRegister::SetBindObject(VOID* pVoid, VOID* pTarget)
{
	//if (!pVoid || !pTarget)
	//	return;

	//GameObject* pGameObject = (GameObject*)pVoid;
	//pGameObject->SetBindObject((GameObject*)pTarget);
}

//解除目标绑定
VOID LuaFunRegister::ClearBind(VOID* pVoid)
{
	//if (!pVoid)
	//	return;

	//GameObject* pGameObject = (GameObject*)pVoid;
	//pGameObject->ClearBind();
}

//是否被触发了
DWORD LuaFunRegister::IsTrigger(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return (DWORD)pGameObject->IsTrigger();
}

//设置触发标志
VOID LuaFunRegister::SetTrigger(VOID* pVoid)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetTrigger();
}

//获得脚本用数据
INT LuaFunRegister::GetScriptData(VOID* pVoid, BYTE byIndex)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetScriptData(byIndex);
}

//设置脚本用数据
VOID LuaFunRegister::SetScriptData(VOID* pVoid, BYTE byIndex, INT nVal)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetScriptData(byIndex, nVal);
}

//附加一个光环
VOID LuaFunRegister::AddAura(VOID* pVoid, DWORD dwAuraID)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->AddAura(dwAuraID);
}

//获得献祭随从数量
DWORD LuaFunRegister::GetSacrificeNum(VOID* pVoid)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return pGameObject->GetSacrificeNum();
}

//丢弃手牌
VOID LuaFunRegister::DiscardHandCard(VOID* pVoid, BYTE byCardNum)
{
	if (!pVoid)
		return;

	GameObject* pGameObject = (GameObject*)pVoid;
	pGameObject->DiscardHandCard(byCardNum);
}

//检测目标是否满足条件
DWORD LuaFunRegister::IsConformCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	if (!pVoid)
		return 0;

	GameObject* pGameObject = (GameObject*)pVoid;
	return (DWORD)pGameObject->IsConformCond(byCondType, byCondSign, dwCondVal);
}

//获得指定条件的卡组卡牌数量
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

//获得指定条件的手牌数量
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

//获得指定条件的墓地卡牌数量
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

//按指定条件抽牌
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

//舍弃指定条件的手牌
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

//舍弃指定条件的卡组内卡牌
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

//复活墓地卡牌
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

//从手牌召唤随从
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

//从卡组召唤随从
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

//控制随从
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

//翻转随从
VOID LuaFunRegister::TurnRetinue(VOID* pVoid, DWORD dwIsTriggerScript)
{
	//if (!pVoid)
		return;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->OnTurn((BOOL)dwIsTriggerScript);
}

//覆盖随从
VOID LuaFunRegister::CoverRetinue(VOID* pVoid, DWORD dwIsTriggerScript)
{
	//if (!pVoid)
		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->OnCover((BOOL)dwIsTriggerScript);
}

//退还随从
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

//释放法术
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

//获得战场环境
DWORD LuaFunRegister::GetEnv(VOID* pVoid)
{
	//if (!pVoid)
		return 0;

// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	return pGameObject->GetEnv();
}

//设置战场环境
VOID LuaFunRegister::SetEnv(VOID* pVoid, DWORD dwEnv)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->SetEnv(dwEnv);
}

//清除战场环境
VOID LuaFunRegister::ClearEnv(VOID* pVoid)
{
// 	if (!pVoid)
// 		return;
// 
// 	GameObject* pGameObject = (GameObject*)pVoid;
// 	pGameObject->ClearEnv();
}

//检测是否接取指定的任务
bool LuaFunRegister::IsAcceptMission(VOID* pVoid, DWORD dwMissionID)
{
//	if (!pVoid)
		return 0;
// 
// 	Player* pGameObject = (Player*)pVoid;
// 
// 	return pGameObject->IsAcceptMission(dwMissionID) ? true : false;
}

//触发一个任务
void LuaFunRegister::TriggerMission(VOID* pVoid, DWORD dwMissionID)
{
//	if (!pVoid)
		return ;

// 	Player* pGameObject = (Player*)pVoid;
// 
// 
// 	pGameObject->TriggerMission(dwMissionID);
}