/*
----------------------------------------------
����Լ����

ID ������������
idCharacter: �˲�����ʾ�� ID ����Ϊ��� ID��NPC ID ��
idPlayer: �� ID ����Ϊ��� ID

�����������߱����ı�д�淶��
// �����Ĺ���˵��
// ������ԭ�ͣ���������ʹ�� Lua �е����ͣ�
LUA_FUNC_EXPORT("funcName", LuaFuncName)

// �����Ĺ���˵��
LUA_VAR_EXPORT("var", var)

----------------------------------------------
�����д˵����
1. Lua ����� ID ʹ�ý�ɫ ID
2. ��дʱ�Ժ�����;�Ͳ�������ע�ͣ����ļ����ṩ���߻���Ϊ API �ĵ�

Lua �ж����� C++ �����õ�ȫ�ֺ�������������¼��ص�������ͨ��Ӧ�ü��� idScene��һ��Ϊ������ڳ����� ID��
C++ �ж����� Lua �����õ�ȫ�ֺ������趨�������������Լ����
*/

#include "stdafx.h"
#include "../LuaEngine/LuaInterface.h"
#include "LuaFunRegister.h"
//#include "Attributes.h"

LUA_EXPORT_BEGIN
/*--------------------------------------------------------------------------------------------------------------------------*/	
//�궨�嵼��
/*--------------------------------------------------------------------------------------------------------------------------*/
LUA_VAR_EXPORT("NULL",							NULL)

//��������
LUA_VAR_EXPORT("OBJECT_PLAYER",					OBJECT_PLAYER)					//���
//LUA_VAR_EXPORT("OBJECT_CHIEF",					OBJECT_CHIEF)					//����
//LUA_VAR_EXPORT("OBJECT_RETINUE",				OBJECT_RETINUE)					//���
//LUA_VAR_EXPORT("OBJECT_CARD",					OBJECT_CARD)					//����

//��������
// LUA_VAR_EXPORT("CARD_RACE_1",					CARD_RACE_1)					//����
// LUA_VAR_EXPORT("CARD_RACE_2",					CARD_RACE_2)					//������
// LUA_VAR_EXPORT("CARD_RACE_3",					CARD_RACE_3)					//����
// LUA_VAR_EXPORT("CARD_RACE_4",					CARD_RACE_4)					//������
// LUA_VAR_EXPORT("CARD_RACE_5",					CARD_RACE_5)					//������
// LUA_VAR_EXPORT("CARD_RACE_6",					CARD_RACE_6)					//����
// LUA_VAR_EXPORT("CARD_RACE_7",					CARD_RACE_7)					//����
// LUA_VAR_EXPORT("CARD_RACE_8",					CARD_RACE_8)					//ˮ��
// LUA_VAR_EXPORT("CARD_RACE_9",					CARD_RACE_9)					//��ħ��
// LUA_VAR_EXPORT("CARD_RACE_10",					CARD_RACE_10)					//�ɱ���
// 
// //������ɫ(ϡ�ж�)
// LUA_VAR_EXPORT("CARD_COLOR_NONE",				CARD_COLOR_NONE)				//��ʹ��
// LUA_VAR_EXPORT("CARD_COLOR_WHITE",				CARD_COLOR_WHITE)				//��ɫ(��ͨ)
// LUA_VAR_EXPORT("CARD_COLOR_GREEN",				CARD_COLOR_GREEN)				//��ɫ(ϡ��)
// LUA_VAR_EXPORT("CARD_COLOR_BLUE",				CARD_COLOR_BLUE)				//��ɫ(���)
// LUA_VAR_EXPORT("CARD_COLOR_VIOLET",				CARD_COLOR_VIOLET)				//��ɫ(ʷʫ)
// LUA_VAR_EXPORT("CARD_COLOR_ORANGE",				CARD_COLOR_ORANGE)				//��ɫ(��˵)
// LUA_VAR_EXPORT("CARD_COLOR_BLACK",				CARD_COLOR_BLACK)				//��ɫ(��)
// LUA_VAR_EXPORT("CARD_COLOR_RED",				CARD_COLOR_RED)					//��ɫ(����)

//������
LUA_VAR_EXPORT("GO_ORIEN_FRONT",				GO_ORIEN_FRONT)					//����
LUA_VAR_EXPORT("GO_ORIEN_BACK",					GO_ORIEN_BACK)					//����

//����״̬
LUA_VAR_EXPORT("GO_STATUS_OVERAWE",				GO_STATUS_OVERAWE)				//����״̬(���ܹ���)
LUA_VAR_EXPORT("GO_STATUS_CONTROL",				GO_STATUS_CONTROL)				//����״̬(���忨����Ч)
LUA_VAR_EXPORT("GO_STATUS_SLOW",				GO_STATUS_SLOW)					//�ٻ�״̬(���ٻ��Ļغϲ��ܽ��й���)
LUA_VAR_EXPORT("GO_STATUS_FALSE_BODY",			GO_STATUS_FALSE_BODY)			//����״̬(���ܱ��׼�)
LUA_VAR_EXPORT("GO_STATUS_OVERTURN",			GO_STATUS_OVERTURN)				//����״̬(�����Ʊ���ת�в�������תЧ��)
LUA_VAR_EXPORT("GO_STATUS_ASSAULT",				GO_STATUS_ASSAULT)				//ͻ��״̬(����������̬�����ʱ����������˺�)
LUA_VAR_EXPORT("GO_STATUS_IMMUNE",				GO_STATUS_IMMUNE)				//����״̬(�޷����ƻ�)
LUA_VAR_EXPORT("GO_STATUS_DOUBLE_HIT",			GO_STATUS_DOUBLE_HIT)			//����״̬(���Թ���2��)
LUA_VAR_EXPORT("GO_STATUS_CRITICLE_HIT",		GO_STATUS_CRITICLE_HIT)			//����״̬(2���˺�)
LUA_VAR_EXPORT("GO_STATUS_SUBSTITUTE",			GO_STATUS_SUBSTITUTE)			//����״̬(���������Ӵ����Լ�������)
LUA_VAR_EXPORT("GO_STATUS_AVOIDANCE",			GO_STATUS_AVOIDANCE)			//����״̬(�����ܵ��˺�)
LUA_VAR_EXPORT("GO_STATUS_BEAR",				GO_STATUS_BEAR)					//����״̬(�������ܵ��������ᷭת)
LUA_VAR_EXPORT("GO_STATUS_RUSH",				GO_STATUS_RUSH)					//���״̬(����ֱ�ӹ�������)
LUA_VAR_EXPORT("GO_STATUS_RESTRAIN",			GO_STATUS_RESTRAIN)				//����״̬(��Ӳ��ܹ�������)
LUA_VAR_EXPORT("GO_STATUS_DRAIN",				GO_STATUS_DRAIN)				//�ݽ�״̬(û��������Ӳ��ܹ���)
LUA_VAR_EXPORT("GO_STATUS_SILENCE",				GO_STATUS_SILENCE)				//��Ĭ״̬(��ӽű��������ű�������ű�������Ч�����޷���Ч)

//��������
// LUA_VAR_EXPORT("CARD_TYPE_RETINUE",				CARD_TYPE_RETINUE)				//��ӿ���
// LUA_VAR_EXPORT("CARD_TYPE_MAGIC",				CARD_TYPE_MAGIC)				//ħ������
// 
// //ħ������������
// LUA_VAR_EXPORT("CARD_SUB_TYPE_ATTACK_SPELL",	CARD_SUB_TYPE_ATTACK_SPELL)		//����������
// LUA_VAR_EXPORT("CARD_SUB_TYPE_BR_SPELL",		CARD_SUB_TYPE_BR_SPELL)			//ս����ӷ�����
// LUA_VAR_EXPORT("CARD_SUB_TYPE_MR_SPELL",		CARD_SUB_TYPE_MR_SPELL)			//ħ����ӷ�����
// LUA_VAR_EXPORT("CARD_SUB_TYPE_TRAP",			CARD_SUB_TYPE_TRAP)				//������
// LUA_VAR_EXPORT("CARD_SUB_TYPE_EQUIPMENT",		CARD_SUB_TYPE_EQUIPMENT)		//װ����
// 
// //�������
// LUA_VAR_EXPORT("RETINUE_TYPE_BATTLE",			RETINUE_TYPE_BATTLE)			//ս�����
// LUA_VAR_EXPORT("RETINUE_TYPE_MAGIC",			RETINUE_TYPE_MAGIC)				//ħ�����
// 
// //ħ���������
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_SPELL",		MAGIC_RETINUE_TYPE_SPELL)		//�������
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_TRAP",		MAGIC_RETINUE_TYPE_TRAP)		//�������
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_EQUIPMENT",	MAGIC_RETINUE_TYPE_EQUIPMENT)	//װ�����
// LUA_VAR_EXPORT("MAGIC_RETINUE_TYPE_ENV",		MAGIC_RETINUE_TYPE_ENV)			//�������

//��������
// LUA_VAR_EXPORT("GOAT_RACE",						GOAT_RACE)						//��������
// LUA_VAR_EXPORT("GOAT_MAX_HP",					GOAT_MAX_HP)					//�����������ֵ
// LUA_VAR_EXPORT("GOAT_HP",						GOAT_HP)						//��������ֵ
// LUA_VAR_EXPORT("GOAT_ATTACK_POWER",				GOAT_ATTACK_POWER)				//���󹥻���
// LUA_VAR_EXPORT("GOAT_DEFENSE_POWER",			GOAT_DEFENSE_POWER)				//���������
// LUA_VAR_EXPORT("GOAT_ACTION",					GOAT_ACTION)					//�����ж���
// LUA_VAR_EXPORT("GOAT_STATUS",					GOAT_STATUS)					//����״̬
// LUA_VAR_EXPORT("GOAT_ORIEN",					GOAT_ORIEN)						//������
// 
// //������������
// LUA_VAR_EXPORT("eATTR_KIND_CALC",				eATTR_KIND_CALC)	            //�������
// LUA_VAR_EXPORT("eATTR_KIND_BASE",				eATTR_KIND_BASE)	            //��������
// LUA_VAR_EXPORT("eATTR_KIND_ITEM",				eATTR_KIND_ITEM)	            //��Ʒ����������޸�
// LUA_VAR_EXPORT("eATTR_KIND_SKILL",				eATTR_KIND_SKILL)	            //�츳�����ܣ�buff����������޸�
// LUA_VAR_EXPORT("eATTR_KIND_SKILL_PERC",			eATTR_KIND_SKILL_PERC)          //�츳�����ܣ�buff����������޸�(�ٷֱ�)

//����λ��
// LUA_VAR_EXPORT("RELIVE_POS_GROUP_CARD",			RELIVE_POS_GROUP_CARD)	        //�������
// LUA_VAR_EXPORT("RELIVE_POS_HAND_CARD",			RELIVE_POS_HAND_CARD)	        //�������
// LUA_VAR_EXPORT("RELIVE_POS_BATTLEFILED",		RELIVE_POS_BATTLEFILED)	        //���ս��
// 
// //�˻�λ��
// LUA_VAR_EXPORT("SEND_BACK_POS_GROUP_CARD",		SEND_BACK_POS_GROUP_CARD)	    //�˻�������
// LUA_VAR_EXPORT("SEND_BACK_POS_HAND_CARD",		SEND_BACK_POS_HAND_CARD)	    //�˻�������

//Ŀ������
// LUA_VAR_EXPORT("STC_NONE",						STC_NONE)						//������
// LUA_VAR_EXPORT("STC_RACE",						STC_RACE)						//Ŀ����������
// LUA_VAR_EXPORT("STC_LV",						STC_LV)							//Ŀ���Ǽ�����
// LUA_VAR_EXPORT("STC_AP",						STC_AP)							//Ŀ�깥����
// LUA_VAR_EXPORT("STC_DP",						STC_DP)							//Ŀ�������
// LUA_VAR_EXPORT("STC_RETINUE_ID",				STC_RETINUE_ID)					//Ŀ�����ID
// LUA_VAR_EXPORT("STC_RETINUE_TYPE",				STC_RETINUE_TYPE)				//Ŀ���������
// LUA_VAR_EXPORT("STC_RETINUE_SUB_TYPE",			STC_RETINUE_SUB_TYPE)	        //Ŀ�����������
// LUA_VAR_EXPORT("STC_CARD_ID",					STC_CARD_ID)					//Ŀ�꿨��ID
// LUA_VAR_EXPORT("STC_CARD_TYPE",					STC_CARD_TYPE)					//Ŀ�꿨������
// LUA_VAR_EXPORT("STC_CARD_SUB_TYPE",				STC_CARD_SUB_TYPE)				//Ŀ���ӿ�������
// LUA_VAR_EXPORT("STC_COLOR",						STC_COLOR)						//Ŀ����ɫ����
// LUA_VAR_EXPORT("STC_SACRIFICE_NUM",				STC_SACRIFICE_NUM)				//Ŀ���׼�����

//Ŀ����������
// LUA_VAR_EXPORT("STCS_NONE",						STCS_NONE)						//�޷���
// LUA_VAR_EXPORT("STCS_EQUAL",					STCS_EQUAL)						//'='����
// LUA_VAR_EXPORT("STCS_UNEQUAL",					STCS_UNEQUAL)					//'!='����
// LUA_VAR_EXPORT("STCS_GREAT_EQUAL",				STCS_GREAT_EQUAL)				//'>='����
// LUA_VAR_EXPORT("STCS_GREAT",					STCS_GREAT)						//'>'����
// LUA_VAR_EXPORT("STCS_LESS_EQUAL",				STCS_LESS_EQUAL)				//'<='����
// LUA_VAR_EXPORT("STCS_LESS",						STCS_LESS)						//'<'����

//ս������
// LUA_VAR_EXPORT("BATTLE_ENV_COMMON",				BATTLE_ENV_COMMON)				//��ͨս������
// LUA_VAR_EXPORT("BATTLE_ENV_1",					BATTLE_ENV_1)					//�����컷��
// LUA_VAR_EXPORT("BATTLE_ENV_2",					BATTLE_ENV_2)					//�ظ�����
// LUA_VAR_EXPORT("BATTLE_ENV_3",					BATTLE_ENV_3)					//�ƽ�ػ���
// LUA_VAR_EXPORT("BATTLE_ENV_4",					BATTLE_ENV_4)					//���󻷾�
// LUA_VAR_EXPORT("BATTLE_ENV_5",					BATTLE_ENV_5)					//�����ǻ���
// LUA_VAR_EXPORT("BATTLE_ENV_6",					BATTLE_ENV_6)					//����ֻ���
// LUA_VAR_EXPORT("BATTLE_ENV_7",					BATTLE_ENV_7)					//�׳�Ͽ����
// LUA_VAR_EXPORT("BATTLE_ENV_8",					BATTLE_ENV_8)					//Ѫ������
// LUA_VAR_EXPORT("BATTLE_ENV_9",					BATTLE_ENV_9)					//�ƺ�����
// LUA_VAR_EXPORT("BATTLE_ENV_10",					BATTLE_ENV_10)					//�ɱ�Ͽ����
// LUA_VAR_EXPORT("BATTLE_ENV_11",					BATTLE_ENV_11)					//�����Ȼ���

/*--------------------------------------------------------------------------------------------------------------------------*/	
//��������
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

//����ϵͳ ע�ắ��
LUA_FUNC_EXPORT("IsAcceptMission",				LuaFunRegister::IsAcceptMission)
LUA_FUNC_EXPORT("TriggerMission",				LuaFunRegister::TriggerMission)
LUA_EXPORT_END
