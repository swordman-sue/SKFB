#pragma once
#include "../LuaEngine/LuaString.h"

class GameObject;

class LuaFunRegister
{
public:
	LuaFunRegister(void);
	~LuaFunRegister(void);

public:

	//函数注册
	//检测对象是否为NULL
	static DWORD IsNull(VOID* pVoid);
	static DWORD IsNotNull(VOID* pVoid);

	//获得一个随机数(1-100)
	static DWORD GetRandom();

	//获得对象的ObjectKey
	static DWORD GetObjectKey(VOID* pVoid);

	//获得对象的类型
	static DWORD GetObjectType(VOID* pVoid);

	//根据ObjectKey查找对象
	static VOID* FindObject(VOID* pVoid, DWORD dwObjectKey);

	//获得对象的种族
	static DWORD GetRace(VOID* pVoid);

	//获得对象颜色(稀有度)
	static DWORD GetColor(VOID* pVoid);

	//获得对象等级(星级)
	static DWORD GetLevel(VOID* pVoid);

	//获得随从的ID
	static DWORD GetRetinueID(VOID* pVoid);

	//获得随从的卡牌ID
	static DWORD GetReitnueCardID(VOID* pVoid);

	//获得随从的类型
	static DWORD GetRetinueType(VOID* pVoid);

	//获得随从的子类型
	static DWORD GetRetinueSubType(VOID* pVoid);

	//获得对象的属性值
	static INT	 GetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind);

	//设置对象的属性值
	static VOID	 SetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue);

	//增加对象的属性值
	static VOID	 AddAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue);

	//清除对象的属性值
	static VOID	 ClearAttr(VOID* pVoid, DWORD dwAttrType);

	//对象是否持有某种状态
	static DWORD HasStatus(VOID* pVoid, DWORD dwStatus);

	//设置对象的状态
	static VOID	 SetStatus(VOID* pVoid, DWORD dwStatus);

	//清除对象的状态
	static VOID	 ClearStatus(VOID* pVoid, DWORD dwStatus);

	//对象是否为战斗随从
	static DWORD IsBattleRetinue(VOID* pVoid);

	//对象是否为魔法随从
	static DWORD IsMagicRetinue(VOID* pVoid);

	//对象是否为法术随从
	static DWORD IsSpellRetinue(VOID* pVoid);

	//对象是否为陷阱随从
	static DWORD IsTrapRetinue(VOID* pVoid);

	//对象是否为装备随从
	static DWORD IsEquipmentRetinue(VOID* pVoid);

	//对象是否为环境随从
	static DWORD IsEnvRetinue(VOID* pVoid);

	//对象是否存活
	static DWORD IsAlive(VOID* pVoid);

	//对象是否死亡
	static DWORD IsDead(VOID* pVoid);

	//对象是否友方阵营
	static DWORD IsFriend(VOID* pVoid, VOID* pTarget);

	//设置对象死亡
	static VOID  SetDead(VOID* pVoid, VOID* pMurderer);

	//获得己方首领对象指针
	static VOID* GetChief(VOID* pVoid);

	//获得敌方首领对象指针
	static VOID* GetEnemyChief(VOID* pVoid);

	//增加己方首领HP
	static VOID	 AddChiefHp(VOID* pVoid, INT nVal);

	//增加敌方首领HP
	static VOID	 AddEnemyChiefHp(VOID* pVoid, INT nVal);

	//获得我方场上战斗随从数量
	static DWORD GetBattleRetinueNum(VOID* pVoid);

	//获得我方场上魔法随从数量
	static DWORD GetMagicRetinueNum(VOID* pVoid);

	//获得敌方场上战斗随从数量
	static DWORD GetEnemyBattleRetinueNum(VOID* pVoid);

	//获得敌方场上魔法随从数量
	static DWORD GetEnemyMagicRetinueNum(VOID* pVoid);

	//获得我方战斗随从指针
	static VOID* GetBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//获得我方魔法随从指针
	static VOID* GetMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//获得敌方战斗随从指针
	static VOID* GetEnemyBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//获得敌方魔法随从指针
	static VOID* GetEnemyMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//获得我方墓地卡牌数量
	static DWORD GetGraveCardNum(VOID* pVoid);

	//获得我方墓地卡牌指针
	static VOID* GetGraveCardByIndex(VOID* pVoid, DWORD dwIndex);

	//获得敌方墓地卡牌数量
	static DWORD GetEnemyGraveCardNum(VOID* pVoid);

	//获得敌方墓地卡牌指针
	static VOID* GetEnemyGraveCardByIndex(VOID* pVoid, DWORD dwIndex);

	//获得我方当前手牌数量
	static DWORD GetHandCardNum(VOID* pVoid);

	//获得我方手牌指针
	static VOID* GetHandCardByIndex(VOID* pVoid, DWORD dwIndex);

	//获得我方当前牌组卡牌数量
	static DWORD GetGroupCardNum(VOID* pVoid);

	//获得敌方当前手牌数量
	static DWORD GetEnemyHandCardNum(VOID* pVoid);

	//获得敌方当前牌组卡牌数量
	static DWORD GetEnemyGroupCardNum(VOID* pVoid);

	//获得绑定目标对象
	static VOID* GetBindObject(VOID* pVoid);

	//绑定一个目标对象
	static VOID  SetBindObject(VOID* pVoid, VOID* pTarget);

	//解除目标绑定
	static VOID  ClearBind(VOID* pVoid);

	//是否被触发了
	static DWORD IsTrigger(VOID* pVoid);

	//设置触发标志
	static VOID	 SetTrigger(VOID* pVoid);

	//获得脚本用数据
	static INT   GetScriptData(VOID* pVoid, BYTE byIndex);

	//设置脚本用数据
	static VOID  SetScriptData(VOID* pVoid, BYTE byIndex, INT nVal);

	//附加一个光环
	static VOID  AddAura(VOID* pVoid, DWORD dwAuraID);

	//获得献祭随从数量
	static DWORD GetSacrificeNum(VOID* pVoid);

	//丢弃手牌
	static VOID	 DiscardHandCard(VOID* pVoid, BYTE byCardNum);

	//检测目标是否满足条件
	static DWORD IsConformCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//获得指定条件的卡组卡牌数量
	static BYTE	 GetGroupCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//获得指定条件的手牌数量
	static BYTE	 GetHandCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//获得指定条件的墓地卡牌数量
	static BYTE	 GetGraveCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//按指定条件抽牌
	static VOID	 DrawCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//舍弃指定条件的手牌
	static VOID	 DiscardHandCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//舍弃指定条件的卡组内卡牌
	static VOID	 DiscardGroupCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//复活墓地卡牌
	static VOID	 ReliveCard(VOID* pVoid, DWORD dwCardKey, BYTE byRelivePos, DWORD dwIsFirstDraw);

	//从手牌召唤随从
	static VOID	 SummonRetinueFromHand(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien);

	//从卡组召唤随从
	static VOID	 SummonRetinueFromGroup(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien);

	//控制随从
	static VOID	 ControlRetinue(VOID* pVoid, VOID* pTarget);

	//翻转随从
	static VOID	 TurnRetinue(VOID* pVoid, DWORD dwIsTriggerScript);

	//覆盖随从
	static VOID	 CoverRetinue(VOID* pVoid, DWORD dwIsTriggerScript);

	//退还随从
	static VOID	 SendBackRetinue(VOID* pVoid, VOID* pTarget, BYTE bySendBackPos, DWORD dwIsFirstDraw);

	//释放法术
	static VOID	 CastSpell(VOID* pVoid, DWORD dwSpellID, DWORD dwTargetKey);

	//获得战场环境
	static DWORD GetEnv(VOID* pVoid);

	//设置战场环境
	static VOID	 SetEnv(VOID* pVoid, DWORD dwEnv);

	//清除战场环境
	static VOID	 ClearEnv(VOID* pVoid);

	static bool  IsAcceptMission(VOID* pVoid, DWORD dwMissionID );

	static VOID  TriggerMission(VOID* pVoid, DWORD dwMissionID );

};
