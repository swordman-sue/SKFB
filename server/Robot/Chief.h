#pragma once
#include <vector>
#include "GameObject.h"
#include "../Common/MsgDefine.h"
#include "../ToolKit/Timer.h"

//class Card;
class Retinue;

typedef std::vector<Card*>		CardVec;
typedef CardVec::iterator		CardVecIter;
typedef std::vector<Retinue*>	RetinueVec;
typedef RetinueVec::iterator	RetinueVecIter;

//首领AI状态
enum AIStat
{
	AI_STAT_INIT,				//初始状态
	AI_STAT_PLAY_CARD,			//出牌状态
	AI_STAT_ATTACK_1,			//攻击状态
	AI_STAT_ATTACK_2,			//攻击状态
	AI_STAT_ATTACK_3,			//攻击状态
	AI_STAT_ATTACK_4,			//攻击状态
	AI_STAT_FINISH,				//完成状态
};

class Chief : public GameObject
{
public:
	Chief();
	~Chief();

	BOOL			Init(Robot* pRobot);
	VOID			Release();
	VOID			Update(DWORD dwTick);

	//属性和对象相关
	Chief*			GetChief() { return this; }
	VOID			SetEnemyChief(Chief* pEnemyChief) { m_pEnemyChief = pEnemyChief; }
	Chief*			GetEnemyChief() { return m_pEnemyChief; }
// 	BattleEnv		GetBattleEnv() { return m_enBattleEnv; }
// 	VOID			SetBattleEnv(BattleEnv enBattleEnv) { m_enBattleEnv = enBattleEnv; }
	VOID			SetFirstHand(BOOL bFirstHand) { m_bFirstHand = bFirstHand; }
	VOID			AddToGameObjectMap(GameObject* pGameObject);
	VOID			RemoveFromGameObjectMap(GameObject* pGameObject);

	//卡牌相关
	BYTE			GetHandCardNum() { return (BYTE)m_HandCardVec.size(); }
	BYTE			GetGraveCardNum() { return (BYTE)m_GraveCardVec.size(); }
	Card*			GetHandCardByIndex(BYTE byIndex) { return (byIndex < m_HandCardVec.size() ? m_HandCardVec[byIndex] : NULL); }
	Card*			FindHandCardByKey(DWORD dwKey);
	Card*			GetGraveCardByIndex(BYTE byIndex) { return (byIndex < m_GraveCardVec.size() ? m_GraveCardVec[byIndex] : NULL); }
	Card*			FindGraveCardByKey(DWORD dwKey);
	VOID			AddCardToHand(DWORD dwCardKey, DWORD dwCardID);
	VOID			RemoveCardFromHand(DWORD dwCardKey);
	VOID			AddCardToGrave(DWORD dwCardKey, DWORD dwCardID);
	VOID			RemoveCardFromGrave(DWORD dwCardKey);

	//随从相关
	BYTE			GetBattleRetinueNum() { return (BYTE)m_BattleRetinueVec.size(); }
	BYTE			GetAliveBattleRetinueNum();
	VOID			AddRetinue(DWORD dwRetinueKey, DWORD dwRetinueID, char cSlot, BYTE byOrien);
	Retinue*		GetBattleRetinueByIndex(BYTE byIndex) { return (byIndex < m_BattleRetinueVec.size() ? m_BattleRetinueVec[byIndex] : NULL); }
	Retinue*		FindBattleRetinueByKey(DWORD dwKey);
	VOID			RemoveBattleRetinueByKey(DWORD dwKey);
	BYTE			GetMagicRetinueNum() { return (BYTE)m_MagicRetinueVec.size(); }
	Retinue*		GetMagicRetinueByIndex(BYTE byIndex) { return (byIndex < m_MagicRetinueVec.size() ? m_MagicRetinueVec[byIndex] : NULL); }
	Retinue*		FindMagicRetinueByKey(DWORD dwKey);
	VOID			RemoveMagicRetinueByKey(DWORD dwKey);

	//事件相关
// 	VOID			OnBattleDrawCardCmd(MSG_M2C_DRAW_CARD_CMD* pMsg);
// 	VOID			OnBattleDiscardCardCmd(MSG_M2C_DISCARD_CARD_CMD* pMsg);
// 	VOID			OnBattleDestroyCardCmd(MSG_M2C_DESTROY_CARD_CMD* pMsg);
// 	VOID			OnBattleAddGraveCardCmd(MSG_M2C_ADD_GRAVE_CARD_CMD* pMsg);
// 	VOID			OnBattleReliveGraveCardCmd(MSG_M2C_RELIVE_GRAVE_CARD_CMD* pMsg);
// 	VOID			OnBattleUseRetinueCardCmd(MSG_M2C_USE_RETINUE_CARD_CMD* pMsg);	
// 	VOID			OnBattleUseSpellCardCmd(MSG_M2C_USE_SPELL_CARD_CMD* pMsg);
// 	VOID			OnBattleSummonRetinueCmd(MSG_M2C_SUMMON_RETINUE_CMD* pMsg);
// 	VOID			OnBattleSendBackRetinueCmd(MSG_M2C_SEND_BACK_RETINUE_CMD* pMsg);
// 	VOID			OnBattleControlRetinueCmd(MSG_M2C_CONTROL_RETINUE_CMD* pMsg);
// 	VOID			OnBattleStartCmd(MSG_M2C_BATTLE_START_CMD* pMsg);
// 	VOID			OnBattleChiefInfoCmd(MSG_M2C_BATTLE_CHIEF_INFO_CMD* pMsg);
// 	VOID			OnBattleFirstHandCmd(MSG_M2C_BATTLE_FIRST_HAND_CMD* pMsg);
// 	VOID			OnBattleRoundStartCmd(MSG_M2C_BATTLE_ROUND_START_CMD* pMsg);
// 	VOID			OnBattleResultCmd(MSG_M2C_BATTLE_RESULT_CMD* pMsg);
// 	VOID			OnBattlePrepareFinishCmd(MSG_M2C_BATTLE_PREPARE_FINISH_CMD* pMsg);
// 	VOID			OnBattleEnemyFirstCardCmd(MSG_M2C_BATTLE_ENEMY_FIRST_CARD_CMD* pMsg);
// 	VOID			OnBattleUpdateInfoCmd(MSG_M2C_BATTLE_UPDATE_INFO_CMD* pMsg);

	VOID			OnRetinueDead(Retinue* pRetinue);

	VOID			ProcessAI();
private:
	VOID			ProcessPlayCardAI();
	VOID			ProcessAttackAI_1();
	VOID			ProcessAttackAI_2();
	VOID			ProcessAttackAI_3();
	VOID			ProcessAttackAI_4();
	VOID			ProcessFinishAI();
	Card*			GetFirstCard();
	Card*			GetMaxAttackCard();
	Card*			GetMaxDefenseCard();
	Card*			GetRushCard();
	//Card*			GetAppointSubTypeCard(CardSubType enCardSubType);
	Retinue*		GetMaxAttackRetinue();
	Retinue*		GetEnemyMaxAttackRetinue();
	Retinue*		GetEnemyMinAttackRetinue();
	Retinue*		GetRandomMagicRetinue();
	BOOL			CheckSacrificeForAI(Card* pCard);
	BOOL			SetSacrificeTargetForAI(Card* pCard, DWORD* dwSacrificeTarget, BYTE& bySacrificeNum);
	BOOL			CheckPlayCard(Card* pCard);
	DWORD			GetTargetForPlayCard(Card* pCard);
	BOOL			PlayCard(Card* pCard, BYTE bySummonSlot, GameObjectOrien enOrien);

	BOOL			HandleUseCardSyn(DWORD dwCardKey, DWORD* pTargetArray, BYTE byTargetNum, BYTE bySummonSlot, GameObjectOrien enOrien);
	BOOL			HandleTurnRetinueSyn(DWORD dwRetinueKey);
	BOOL			HandleRetinueAttackSyn(DWORD dwRetinueKey, DWORD dwTargetKey);
	VOID			HandleRoundEndOptCmd();

private:
	Chief*			m_pEnemyChief;		//敌方首领
	CardVec			m_HandCardVec;		//当前手牌
	RetinueVec		m_BattleRetinueVec;	//战斗随从集合
	RetinueVec		m_MagicRetinueVec;	//魔法随从集合
	CardVec			m_GraveCardVec;		//墓地卡牌集合
	BOOL			m_bFirstHand;		//是否为先手
	USHORT			m_shRoundNum;		//回合数
	AIStat			m_enAIStat;			//首领AI状态
	//BattleEnv		m_enBattleEnv;		//战场环境
	Timers			m_AITimer;			//AI计时器(客户端表现用)
};