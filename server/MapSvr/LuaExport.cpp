/*
----------------------------------------------
基本约定：

ID 参数的命名：
idCharacter: 此参数表示的 ID 可以为玩家 ID、NPC ID 等
idPlayer: 此 ID 必须为玩家 ID

导出函数或者变量的编写规范：
// 函数的功能说明
// 函数的原型（变量类型使用 Lua 中的类型）
LUA_FUNC_EXPORT("funcName", LuaFuncName)

// 变量的功能说明
LUA_VAR_EXPORT("var", var)

----------------------------------------------
程序编写说明：
1. Lua 中玩家 ID 使用角色 ID
2. 编写时对函数用途和参数进行注释，此文件将提供给策划作为 API 文档

Lua 中定义由 C++ 方调用的全局函数（例如各个事件回调函数）通常应该加上 idScene（一般为玩家所在场景的 ID）
C++ 中定义由 Lua 方调用的全局函数按需定义参数（无上述约定）
*/

#include "stdafx.h"
#include "../LuaEngine/LuaInterface.h"
#include "LuaFunRegister.h"
//#include "Attributes.h"

LUA_EXPORT_BEGIN
/*--------------------------------------------------------------------------------------------------------------------------*/	
//宏定义导出
/*--------------------------------------------------------------------------------------------------------------------------*/
LUA_VAR_EXPORT("NULL",							NULL)

//对象类型
LUA_VAR_EXPORT("OBJECT_PLAYER",					OBJECT_PLAYER)					//玩家
//LUA_VAR_EXPORT("OBJECT_CHIEF",					OBJECT_CHIEF)					//首领
//LUA_VAR_EXPORT("OBJECT_RETINUE",				OBJECT_RETINUE)					//随从
//LUA_VAR_EXPORT("OBJECT_CARD",					OBJECT_CARD)					//卡牌

//对象种族
// LUA_VAR_EXPORT("CARD_RACE_1",					CARD_RACE_1)					//人族
// LUA_VAR_EXPORT("CARD_RACE_2",					CARD_RACE_2)					//天人族
// LUA_VAR_EXPORT("CARD_RACE_3",					CARD_RACE_3)					//妖族
// LUA_VAR_EXPORT("CARD_RACE_4",					CARD_RACE_4)					//修罗族
// LUA_VAR_EXPORT("CARD_RACE_5",					CARD_RACE_5)					//羽民族
// LUA_VAR_EXPORT("CARD_RACE_6",					CARD_RACE_6)					//巫族
// LUA_VAR_EXPORT("CARD_RACE_7",					CARD_RACE_7)					//鬼族
// LUA_VAR_EXPORT("CARD_RACE_8",					CARD_RACE_8)					//水族
// LUA_VAR_EXPORT("CARD_RACE_9",					CARD_RACE_9)					//天魔族
// LUA_VAR_EXPORT("CARD_RACE_10",					CARD_RACE_10)					//仙兵族
// 
// //对象颜色(稀有度)
// LUA_VAR_EXPORT("CARD_COLOR_NONE",				CARD_COLOR_NONE)				//不使用
// LUA_VAR_EXPORT("CARD_COLOR_WHITE",				CARD_COLOR_WHITE)				//白色(普通)
// LUA_VAR_EXPORT("CARD_COLOR_GREEN",				CARD_COLOR_GREEN)				//绿色(稀有)
// LUA_VAR_EXPORT("CARD_COLOR_BLUE",				CARD_COLOR_BLUE)				//蓝色(珍贵)
// LUA_VAR_EXPORT("CARD_COLOR_VIOLET",				CARD_COLOR_VIOLET)				//紫色(史诗)
// LUA_VAR_EXPORT("CARD_COLOR_ORANGE",				CARD_COLOR_ORANGE)				//橙色(传说)
// LUA_VAR_EXPORT("CARD_COLOR_BLACK",				CARD_COLOR_BLACK)				//黑色(神化)
// LUA_VAR_EXPORT("CARD_COLOR_RED",				CARD_COLOR_RED)					//红色(限量)

//对象朝向
LUA_VAR_EXPORT("GO_ORIEN_FRONT",				GO_ORIEN_FRONT)					//正面
LUA_VAR_EXPORT("GO_ORIEN_BACK",					GO_ORIEN_BACK)					//背面

//对象状态
LUA_VAR_EXPORT("GO_STATUS_OVERAWE",				GO_STATUS_OVERAWE)				//威慑状态(不能攻击)
LUA_VAR_EXPORT("GO_STATUS_CONTROL",				GO_STATUS_CONTROL)				//反控状态(陷阱卡不生效)
LUA_VAR_EXPORT("GO_STATUS_SLOW",				GO_STATUS_SLOW)					//迟缓状态(被召唤的回合不能进行攻击)
LUA_VAR_EXPORT("GO_STATUS_FALSE_BODY",			GO_STATUS_FALSE_BODY)			//假身状态(不能被献祭)
LUA_VAR_EXPORT("GO_STATUS_OVERTURN",			GO_STATUS_OVERTURN)				//翻身状态(覆盖牌被翻转切不触发翻转效果)
LUA_VAR_EXPORT("GO_STATUS_ASSAULT",				GO_STATUS_ASSAULT)				//突击状态(攻击防御姿态的随从时，仍有溢出伤害)
LUA_VAR_EXPORT("GO_STATUS_IMMUNE",				GO_STATUS_IMMUNE)				//免疫状态(无法被破坏)
LUA_VAR_EXPORT("GO_STATUS_DOUBLE_HIT",			GO_STATUS_DOUBLE_HIT)			//连击状态(可以攻击2次)
LUA_VAR_EXPORT("GO_STATUS_CRITICLE_HIT",		GO_STATUS_CRITICLE_HIT)			//暴击状态(2倍伤害)
LUA_VAR_EXPORT("GO_STATUS_SUBSTITUTE",			GO_STATUS_SUBSTITUTE)			//替身状态(让另外的随从代替自己被攻击)
LUA_VAR_EXPORT("GO_STATUS_AVOIDANCE",			GO_STATUS_AVOIDANCE)			//免伤状态(不会受到伤害)
LUA_VAR_EXPORT("GO_STATUS_BEAR",				GO_STATUS_BEAR)					//隐忍状态(覆盖牌受到攻击不会翻转)
LUA_VAR_EXPORT("GO_STATUS_RUSH",				GO_STATUS_RUSH)					//冲锋状态(可以直接攻击首领)
LUA_VAR_EXPORT("GO_STATUS_RESTRAIN",			GO_STATUS_RESTRAIN)				//克制状态(随从不能攻击首领)
LUA_VAR_EXPORT("GO_STATUS_DRAIN",				GO_STATUS_DRAIN)				//枯竭状态(没有手牌随从不能攻击)
LUA_VAR_EXPORT("GO_STATUS_SILENCE",				GO_STATUS_SILENCE)				//沉默状态(随从脚本，法术脚本，陷阱脚本，法术效果均无法生效)

//卡牌类型
// LUA_VAR_EXPORT("CARD_TYPE_RETINUE",				CARD_TYPE_RETINUE)				//随从卡牌
// LUA_VAR_EXPORT("CARD_TYPE_MAGIC",				CARD_TYPE_MAGIC)				//魔法卡牌
// 
// //魔法卡牌子类型
// LUA_VAR_EXPORT("CARD_SUB_TYPE_ATTACK_SPELL",	CARD_SUB_TYPE_ATTACK_SPELL)		//攻击法术牌
// LUA_VAR_EXPORT("CARD_SUB_TYPE_BR_SPELL",		CARD_SUB_TYPE_BR_SPELL)			//战斗随从法术牌
// LUA_VAR_EXPORT("CARD_SUB_TYPE_MR_SPELL",		CARD_SUB_TYPE_MR_SPELL)			//魔法随从法术牌
// LUA_VAR_EXPORT("CARD_SUB_TYPE_TRAP",			CARD_SUB_TYPE_TRAP)				//陷阱牌
// LUA_VAR_EXPORT("CARD_SUB_TYPE_EQUIPMENT",		CARD_SUB_TYPE_EQUIPMENT)		//装备牌
// 
// //随从类型
// LUA_VAR_EXPORT("RETINUE_TYPE_BATTLE",			RETINUE_TYPE_BATTLE)			//战斗随从
// LUA_VAR_EXPORT("RETINUE_TYPE_MAGIC",			RETINUE_TYPE_MAGIC)				//魔法随从
// 
// //魔法随从类型
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_SPELL",		MAGIC_RETINUE_TYPE_SPELL)		//法术随从
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_TRAP",		MAGIC_RETINUE_TYPE_TRAP)		//陷阱随从
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_EQUIPMENT",	MAGIC_RETINUE_TYPE_EQUIPMENT)	//装备随从
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_ENV",		MAGIC_RETINUE_TYPE_ENV)			//环境随从

//对象属性
// LUA_VAR_EXPORT("GOAT_RACE",						GOAT_RACE)						//对象种族
// LUA_VAR_EXPORT("GOAT_MAX_HP",					GOAT_MAX_HP)					//对象最大生命值
// LUA_VAR_EXPORT("GOAT_HP",						GOAT_HP)						//对象生命值
// LUA_VAR_EXPORT("GOAT_ATTACK_POWER",				GOAT_ATTACK_POWER)				//对象攻击力
// LUA_VAR_EXPORT("GOAT_DEFENSE_POWER",			GOAT_DEFENSE_POWER)				//对象防御力
// LUA_VAR_EXPORT("GOAT_ACTION",					GOAT_ACTION)					//对象行动力
// LUA_VAR_EXPORT("GOAT_STATUS",					GOAT_STATUS)					//对象状态
// LUA_VAR_EXPORT("GOAT_ORIEN",					GOAT_ORIEN)						//对象朝向
// 
// //对象属性种类
// LUA_VAR_EXPORT("eATTR_KIND_CALC",				eATTR_KIND_CALC)	            //结果属性
// LUA_VAR_EXPORT("eATTR_KIND_BASE",				eATTR_KIND_BASE)	            //基本属性
// LUA_VAR_EXPORT("eATTR_KIND_ITEM",				eATTR_KIND_ITEM)	            //物品引起的属性修改
// LUA_VAR_EXPORT("eATTR_KIND_SKILL",				eATTR_KIND_SKILL)	            //天赋，技能，buff引起的属性修改
// LUA_VAR_EXPORT("eATTR_KIND_SKILL_PERC",			eATTR_KIND_SKILL_PERC)          //天赋，技能，buff引起的属性修改(百分比)

//复活位置
// LUA_VAR_EXPORT("RELIVE_POS_GROUP_CARD",			RELIVE_POS_GROUP_CARD)	        //复活到牌组
// LUA_VAR_EXPORT("RELIVE_POS_HAND_CARD",			RELIVE_POS_HAND_CARD)	        //复活到手牌
// LUA_VAR_EXPORT("RELIVE_POS_BATTLEFILED",		RELIVE_POS_BATTLEFILED)	        //复活到战场
// 
// //退还位置
// LUA_VAR_EXPORT("SEND_BACK_POS_GROUP_CARD",		SEND_BACK_POS_GROUP_CARD)	    //退还到卡组
// LUA_VAR_EXPORT("SEND_BACK_POS_HAND_CARD",		SEND_BACK_POS_HAND_CARD)	    //退还到手牌

//目标条件
// LUA_VAR_EXPORT("STC_NONE",						STC_NONE)						//无条件
// LUA_VAR_EXPORT("STC_RACE",						STC_RACE)						//目标种族条件
// LUA_VAR_EXPORT("STC_LV",						STC_LV)							//目标星级条件
// LUA_VAR_EXPORT("STC_AP",						STC_AP)							//目标攻击力
// LUA_VAR_EXPORT("STC_DP",						STC_DP)							//目标防御力
// LUA_VAR_EXPORT("STC_RETINUE_ID",				STC_RETINUE_ID)					//目标随从ID
// LUA_VAR_EXPORT("STC_RETINUE_TYPE",				STC_RETINUE_TYPE)				//目标随从类型
// LUA_VAR_EXPORT("STC_RETINUE_SUB_TYPE",			STC_RETINUE_SUB_TYPE)	        //目标随从子类型
// LUA_VAR_EXPORT("STC_CARD_ID",					STC_CARD_ID)					//目标卡牌ID
// LUA_VAR_EXPORT("STC_CARD_TYPE",					STC_CARD_TYPE)					//目标卡牌类型
// LUA_VAR_EXPORT("STC_CARD_SUB_TYPE",				STC_CARD_SUB_TYPE)				//目标子卡牌类型
// LUA_VAR_EXPORT("STC_COLOR",						STC_COLOR)						//目标颜色条件
// LUA_VAR_EXPORT("STC_SACRIFICE_NUM",				STC_SACRIFICE_NUM)				//目标献祭数量

//目标条件符号
// LUA_VAR_EXPORT("STCS_NONE",						STCS_NONE)						//无符号
// LUA_VAR_EXPORT("STCS_EQUAL",					STCS_EQUAL)						//'='符号
// LUA_VAR_EXPORT("STCS_UNEQUAL",					STCS_UNEQUAL)					//'!='符号
// LUA_VAR_EXPORT("STCS_GREAT_EQUAL",				STCS_GREAT_EQUAL)				//'>='符号
// LUA_VAR_EXPORT("STCS_GREAT",					STCS_GREAT)						//'>'符号
// LUA_VAR_EXPORT("STCS_LESS_EQUAL",				STCS_LESS_EQUAL)				//'<='符号
// LUA_VAR_EXPORT("STCS_LESS",						STCS_LESS)						//'<'符号

//战场环境
// LUA_VAR_EXPORT("BATTLE_ENV_COMMON",				BATTLE_ENV_COMMON)				//普通战场环境
// LUA_VAR_EXPORT("BATTLE_ENV_1",					BATTLE_ENV_1)					//域外天环境
// LUA_VAR_EXPORT("BATTLE_ENV_2",					BATTLE_ENV_2)					//地府环境
// LUA_VAR_EXPORT("BATTLE_ENV_3",					BATTLE_ENV_3)					//云界关环境
// LUA_VAR_EXPORT("BATTLE_ENV_4",					BATTLE_ENV_4)					//海洋环境
// LUA_VAR_EXPORT("BATTLE_ENV_5",					BATTLE_ENV_5)					//云阳城环境
// LUA_VAR_EXPORT("BATTLE_ENV_6",					BATTLE_ENV_6)					//锯齿林环境
// LUA_VAR_EXPORT("BATTLE_ENV_7",					BATTLE_ENV_7)					//巫沉峡环境
// LUA_VAR_EXPORT("BATTLE_ENV_8",					BATTLE_ENV_8)					//血海环境
// LUA_VAR_EXPORT("BATTLE_ENV_9",					BATTLE_ENV_9)					//云海环境
// LUA_VAR_EXPORT("BATTLE_ENV_10",					BATTLE_ENV_10)					//仙兵峡环境
// LUA_VAR_EXPORT("BATTLE_ENV_11",					BATTLE_ENV_11)					//玄机谷环境

/*--------------------------------------------------------------------------------------------------------------------------*/	
//函数导出
/*--------------------------------------------------------------------------------------------------------------------------*/
LUA_FUNC_EXPORT("IsNull",						LuaFunRegister::IsNull)
LUA_FUNC_EXPORT("IsNotNull",					LuaFunRegister::IsNotNull)
LUA_FUNC_EXPORT("GetRandom",					LuaFunRegister::GetRandom)
LUA_FUNC_EXPORT("GetObjectKey",					LuaFunRegister::GetObjectKey)
LUA_FUNC_EXPORT("GetObjectType",				LuaFunRegister::GetObjectType)
LUA_FUNC_EXPORT("FindObject",					LuaFunRegister::FindObject)
LUA_FUNC_EXPORT("GetRace",						LuaFunRegister::GetRace)
LUA_FUNC_EXPORT("GetColor",						LuaFunRegister::GetColor)
//LUA_FUNC_EXPORT("GetLv",						LuaFunRegister::GetLv)
LUA_FUNC_EXPORT("GetRetinueID",					LuaFunRegister::GetRetinueID)
LUA_FUNC_EXPORT("GetRetinueType",				LuaFunRegister::GetRetinueType)
LUA_FUNC_EXPORT("GetRetinueSubType",			LuaFunRegister::GetRetinueSubType)
LUA_FUNC_EXPORT("GetAttr",						LuaFunRegister::GetAttr)
LUA_FUNC_EXPORT("SetAttr",						LuaFunRegister::SetAttr)
LUA_FUNC_EXPORT("AddAttr",						LuaFunRegister::AddAttr)
LUA_FUNC_EXPORT("ClearAttr",					LuaFunRegister::ClearAttr)
LUA_FUNC_EXPORT("HasStatus",					LuaFunRegister::HasStatus)
LUA_FUNC_EXPORT("SetStatus",					LuaFunRegister::SetStatus)
LUA_FUNC_EXPORT("ClearStatus",					LuaFunRegister::ClearStatus)
LUA_FUNC_EXPORT("IsBattleRetinue",				LuaFunRegister::IsBattleRetinue)
LUA_FUNC_EXPORT("IsMagicRetinue",				LuaFunRegister::IsMagicRetinue)
LUA_FUNC_EXPORT("IsSpellRetinue",				LuaFunRegister::IsSpellRetinue)
LUA_FUNC_EXPORT("IsTrapRetinue",				LuaFunRegister::IsTrapRetinue)
LUA_FUNC_EXPORT("IsEquipmentRetinue",			LuaFunRegister::IsEquipmentRetinue)
LUA_FUNC_EXPORT("IsEnvRetinue",					LuaFunRegister::IsEnvRetinue)
LUA_FUNC_EXPORT("IsAlive",						LuaFunRegister::IsAlive)
LUA_FUNC_EXPORT("IsDead",						LuaFunRegister::IsDead)
LUA_FUNC_EXPORT("IsFriend",						LuaFunRegister::IsFriend)
LUA_FUNC_EXPORT("SetDead",						LuaFunRegister::SetDead)
LUA_FUNC_EXPORT("GetChief",						LuaFunRegister::GetChief)
LUA_FUNC_EXPORT("GetEnemyChief",				LuaFunRegister::GetEnemyChief)
LUA_FUNC_EXPORT("AddChiefHp",					LuaFunRegister::AddChiefHp)
LUA_FUNC_EXPORT("AddEnemyChiefHp",				LuaFunRegister::AddEnemyChiefHp)
LUA_FUNC_EXPORT("GetBattleRetinueNum",			LuaFunRegister::GetBattleRetinueNum)
LUA_FUNC_EXPORT("GetMagicRetinueNum",			LuaFunRegister::GetMagicRetinueNum)
LUA_FUNC_EXPORT("GetEnemyBattleRetinueNum",		LuaFunRegister::GetEnemyBattleRetinueNum)
LUA_FUNC_EXPORT("GetEnemyMagicRetinueNum",		LuaFunRegister::GetEnemyMagicRetinueNum)
LUA_FUNC_EXPORT("GetBattleRetinueByIndex",		LuaFunRegister::GetBattleRetinueByIndex)
LUA_FUNC_EXPORT("GetMagicRetinueByIndex",		LuaFunRegister::GetMagicRetinueByIndex)
LUA_FUNC_EXPORT("GetEnemyBattleRetinueByIndex",	LuaFunRegister::GetEnemyBattleRetinueByIndex)
LUA_FUNC_EXPORT("GetEnemyMagicRetinueByIndex",	LuaFunRegister::GetEnemyMagicRetinueByIndex)
LUA_FUNC_EXPORT("GetGraveCardNum",				LuaFunRegister::GetGraveCardNum)
LUA_FUNC_EXPORT("GetGraveCardByIndex",			LuaFunRegister::GetGraveCardByIndex)
LUA_FUNC_EXPORT("GetEnemyGraveCardNum",			LuaFunRegister::GetEnemyGraveCardNum)
LUA_FUNC_EXPORT("GetEnemyGraveCardByIndex",		LuaFunRegister::GetEnemyGraveCardByIndex)
LUA_FUNC_EXPORT("GetHandCardNum",				LuaFunRegister::GetHandCardNum) 
LUA_FUNC_EXPORT("GetHandCardByIndex",			LuaFunRegister::GetHandCardByIndex)
LUA_FUNC_EXPORT("GetGroupCardNum",				LuaFunRegister::GetGroupCardNum)
LUA_FUNC_EXPORT("GetEnemyHandCardNum",			LuaFunRegister::GetEnemyHandCardNum)
LUA_FUNC_EXPORT("GetEnemyGroupCardNum",			LuaFunRegister::GetEnemyGroupCardNum)
LUA_FUNC_EXPORT("GetBindObject",				LuaFunRegister::GetBindObject)
LUA_FUNC_EXPORT("SetBindObject",				LuaFunRegister::SetBindObject)
LUA_FUNC_EXPORT("ClearBind",					LuaFunRegister::ClearBind)
LUA_FUNC_EXPORT("IsTrigger",					LuaFunRegister::IsTrigger)
LUA_FUNC_EXPORT("SetTrigger",					LuaFunRegister::SetTrigger)
LUA_FUNC_EXPORT("GetScriptData",				LuaFunRegister::GetScriptData)
LUA_FUNC_EXPORT("SetScriptData",				LuaFunRegister::SetScriptData)
LUA_FUNC_EXPORT("AddAura",						LuaFunRegister::AddAura)
LUA_FUNC_EXPORT("GetSacrificeNum",				LuaFunRegister::GetSacrificeNum)
LUA_FUNC_EXPORT("DiscardHandCard",				LuaFunRegister::DiscardHandCard)
LUA_FUNC_EXPORT("IsConformCond",				LuaFunRegister::IsConformCond)
LUA_FUNC_EXPORT("GetGroupCardNumByCond",		LuaFunRegister::GetGroupCardNumByCond)
LUA_FUNC_EXPORT("GetHandCardNumByCond",			LuaFunRegister::GetHandCardNumByCond)
LUA_FUNC_EXPORT("GetGraveCardNumByCond",		LuaFunRegister::GetGraveCardNumByCond)
LUA_FUNC_EXPORT("DrawCardByCond",				LuaFunRegister::DrawCardByCond)
LUA_FUNC_EXPORT("DiscardHandCardByCond",		LuaFunRegister::DiscardHandCardByCond)
LUA_FUNC_EXPORT("DiscardGroupCardByCond",		LuaFunRegister::DiscardGroupCardByCond)
LUA_FUNC_EXPORT("ReliveCard",					LuaFunRegister::ReliveCard)
LUA_FUNC_EXPORT("SummonRetinueFromHand",		LuaFunRegister::SummonRetinueFromHand)
LUA_FUNC_EXPORT("SummonRetinueFromGroup",		LuaFunRegister::SummonRetinueFromGroup)
LUA_FUNC_EXPORT("ControlRetinue",				LuaFunRegister::ControlRetinue)
LUA_FUNC_EXPORT("TurnRetinue",					LuaFunRegister::TurnRetinue)
LUA_FUNC_EXPORT("CoverRetinue",					LuaFunRegister::CoverRetinue)
LUA_FUNC_EXPORT("SendBackRetinue",				LuaFunRegister::SendBackRetinue)
LUA_FUNC_EXPORT("CastSpell",					LuaFunRegister::CastSpell)
LUA_FUNC_EXPORT("GetEnv",						LuaFunRegister::GetEnv)
LUA_FUNC_EXPORT("SetEnv",						LuaFunRegister::SetEnv)
LUA_FUNC_EXPORT("ClearEnv",						LuaFunRegister::ClearEnv)

//任务系统 注册函数
LUA_FUNC_EXPORT("IsAcceptMission",				LuaFunRegister::IsAcceptMission)
LUA_FUNC_EXPORT("TriggerMission",				LuaFunRegister::TriggerMission)
LUA_EXPORT_END
