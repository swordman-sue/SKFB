/**\file
英雄管理器
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
struct HeroVipSummon_Config;

class CHeroManager
{
public:
	CHeroManager();
	~CHeroManager();

	void Init(Player *pPlayer);

public:
	// 英雄上阵
	void OnHeroBattle(BYTE byPos, DWORD dwHeroUID);

	// 援军上阵
	void OnReinforcementBattle(BYTE byPos, DWORD dwHeroUID);

	// 合成
	void OnCompose(WORD wHeroInfoId);

	// 召唤
	void OnSummon(BYTE byType);

	// VIP召唤
	void OnNetVIPSummon(WORD wCamp);

	// 升级(todo::旧接口目前暂时不使用)
	void OnLevelUp(DWORD dwHeroUID, DWORD *pCostHeroList, BYTE byCostHeroListNum);

	// 升级
	void OnLevelUpEx(DWORD dwHeroUID, DWORD *pCostHeroList, WORD wCostHeroListNum);

	// 突破(进阶)
	void OnBreakEx(DWORD dwHeroUID);

	// 突破(进阶)
	void OnBreak(DWORD dwHeroUID, DWORD *pCostSameNameHeroList, WORD wCostSameNameHeroNum, 
		DWORD *pCostSameQualityHeroList, WORD wCostSameQualityHeroNum,
		DWORD *pCostSameProfessionHeroList, WORD wCostSameProfessionHeroNum);

	// 进化(品质)
	void OnNetEvolution(DWORD dwHeroUID);

	// 培养(todo::最终版本,上帝求我都不改)
	void OnNetTrain(DWORD dwHeroUID, BYTE byCostType, BYTE byTrainTimes);

	// 培养替换(todo::最终版本,上帝求我都不改)
	void OnNetTrainReplace(DWORD dwHeroUID);

	// 领取培养奖励(todo::最终版本,上帝求我都不改)
	void OnNetGetTrainReward(DWORD dwHeroUID, BYTE byProType);

	// 天命(潜能)
	void OnNetDestiny(DWORD dwHeroUID);

	// 天命(潜能)
	void OnNetDestinyEx(DWORD dwHeroUID, WORD wDoTimes);

	// 装备觉醒道具
	void OnNetEquipAwakeningItem(DWORD dwHeroUID, WORD wItemId);

	// 觉醒
	void OnNetAwaken(DWORD dwHeroUID, const DWORD *pCostHeroUIDList, WORD wCostHeroNum);

	// 请求英雄基本数据列表
	void OnNetHeroBaseList();

	// 请求英雄详细信息
	void OnNetHeroInfo(DWORD dwUID);

	// 请求收集英雄列表
	void OnNetCollectionList();

	// 请求召唤数据
	void OnNetSummonData();
public:
	// 添加英雄
	DWORD AddHero(DWORD dwHeroInfoId, bool bNotify = true, WORD wFromId = 0);

	// 添加英雄
	bool AddHero(const HeroInfo &rInfo, bool bNotify = true);

	// 移除英雄
	bool RemoveHero(DWORD dwHeroUID, bool bNotify = true, WORD wFromId = 0);

	// 移除英雄
	bool RemoveHero(DWORD *pHeroUIDList, WORD wHeroNum, WORD wFromId = 0);

	// 取英雄
	const HeroInfo* GetHero(DWORD dwHeroUID);

	// 取包裹空闲数量
	WORD GetBagEmptySize();

	// 取英雄数量
	WORD GetHeroNumByInfoId(DWORD dwHeroInfoId);

	// 是否上阵(使用uid检测)
	bool IsToBattle(DWORD dwUID);

	// 是否上阵(使用infoid检测)
	bool IsToBattleEx(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// 是否可以上阵
	bool IsCanToBattle(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// 是否可以上阵
	bool IsCanToBattleEx(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// 发送英雄列表
	void SendHeroBaseList();

	// 发送阵型列表
	void SendFormationList();

	// 发送召唤数据
	void SendSummonData();

	// 填充阵型列表
	void FillFormationList(ClientFormation *pFormationList, BYTE &byFormationNum);

	// 是否已收集此英雄
	bool IsAlreadyCollection(DWORD dwHeroInfoId);

	// 更新培养等级
	void UpdateTrainLevel();

	// 取被祝福的英雄数量
	WORD GetBlessHeroNum();

	// 是否祝福精灵
	bool IsBlessSprite(DWORD dwSpriteId);

	// 设置精灵祝福
	void SetSpriteBless(WORD wFormactionPos, DWORD dwSpriteId);

	// 取阵位上的祝福精灵
	DWORD GetBlessSpriteId(WORD wFormactionPos);

	// 取祝福精灵所在的阵位
	short GetBlessSpriteFormactionPos(DWORD dwSpriteId);

public:

	// 取所有英雄的战力
	//DWORD GetAllHeroBattleValue();

	// 添加收集的英雄
	void AddCollectionHero(DWORD dwHeroInfoId);

	// 取装备被穿戴的英雄ID
	WORD GetEquipmentWearHeroId(DWORD dwEquipemntUID);

	// 取宝物被穿戴的英雄ID
	WORD GetTreasureWearHeroId(DWORD dwTreasureUID);

	// 取卡牌被穿戴的英雄ID
	DWORD GetCardWearHeroId(DWORD dwCardUID);

	// 英雄基本信息通知
	void HeroBaseDataNotify(DWORD dwUID, BYTE byFlag = true, bool bNotice = true);

	// 通过索引取阵型数据
	const Formation* GetFormationByIndex(BYTE bySlot);

	// 是否装备被穿戴
	bool IsEquipmentWear(BYTE bySlot, DWORD dwEquipmentInfoId);

	// 是否宝物被穿戴
	bool IsTreasureWear(BYTE bySlot, DWORD dwTreasureInfoId);

	// 是否装备的卡牌类型
	bool IsEquipCardType(BYTE bySlot, WORD wCardType);

	// 提供给其他模块使用
public:
	// 取到达指定突破等级的英雄数量
	WORD GetHeroNumByBreakLevel(WORD wBreakLevel);

	// 取到达指定突破等级的英雄数量
	WORD GetHeroNumByDestinyLevel(WORD wDestinyLevel);

	// 取到达指定培养等级的英雄数量
	WORD GetHeroNumByTrainLevel(WORD wTrainLevel);

	// 取阵型上英雄的最低等级
	WORD GetFormationHeroMinLevel();

	// 取阵型上英雄的最低进阶等级（突破等级）
	WORD GetFormationHeroMinBreakLevel();

	// 取阵型上英雄的最低培养等级
	WORD GetFormationHeroMinTrainLevel();

	// 取阵型上英雄的最低天命等级
	WORD GetFormationHeroMinDestinyLevel();

	// 取阵型上所有英雄的星数
	WORD GetFormationAllHeroAwakenStar();

	// 取上阵英雄数量
	BYTE GetToBattleHeroNum();

	// 是否有红点
	bool IsHaveRedPoint();

	// 设置宝石孔位
	void SetGemHole(WORD wFormationSlot, WORD wHole, DWORD dwGemId);

	// 取援军的最小等级
	WORD GetReinforcementMinLevel();

protected:
	// 查找英雄
	HeroInfo* FindHero(DWORD dwHeroUID);

	// 处理普通召唤
	bool DoNormalSummon(vector<DWORD> &OutHeroList, DWORD dwCostItemId = 0, int nFriendshipPoint = 0);

	// 处理高级召唤
	bool DoAdvanceSummon(vector<DWORD> &vOutHeroList, DWORD dwCostItemId = 0, int nCostDiamond = 0, int nSummonTimes = 1);

	// 随机一个普通召唤英雄
	DWORD RandNormalSummonHero();

	// 随机一个高级召唤英雄
	DWORD RandAdvancedSummonHero();

	// 随机一个VIP召唤英雄
	const HeroVipSummon_Config * RandVipSummonHero(WORD wCamp, bool isSpecial);

	// 取一个新的系列id
	DWORD GetNewSeriesId();

	// 取英雄培养属性上限
	int GetTrainProMax(BYTE byProType, WORD wHeroLevel);

	// 取英雄培养属性上限
	int GetTrainProMaxEx(BYTE byProType, WORD wProLevel);

	// 取培养等级上限
	BYTE GetTrainLevelMax(BYTE byProType);

	// 检测突破消耗的英雄
	bool CheckBreakCostHero(HeroInfo *pBreakHero, DWORD *pForCostHeroList, WORD wForCostHeroNum, 
		WORD wNeedToCostHeroNum, vector<DWORD> *pForCostHeroInfoIdList, WORD wCostType);

	// 记录英雄突破消耗的英雄
	void RecordBreakCostHero(HeroInfo *pBreakHero, DWORD dwCostHeroInfoId);

private:
	DWORD m_dwSeriesId; // 系列ID
	Player *m_pPlayer;
	PlayerHeroInfo *m_pPlayerHeroInfo;

	// 英雄突破消耗类型
	enum
	{
		SAME_NAME = 1,						// 同名英雄 
		SAME_QUALITY = 2,					// 同品质英雄 
		SAME_QUALITY_AND_PROFESSION = 3,	// 同品质同职业英雄 
	};
};