/**\file
宝物管理器
\author Kevin
\date 2016/6/29
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;


class CTreasureManager
{
public:
	CTreasureManager();
	~CTreasureManager();

	void Init(Player *pPlayer);

public:
	// 宝物强化 
	void OnEnhance(DWORD dwTreasureUID, DWORD *pCostTreasureList, WORD wCostTreasureNum);

	// 宝物精炼
	void OnRefine(DWORD dwTreasureUID);

	// 宝物合成
	void OnComposeEx(DWORD dwTreasureInfoId);

	// 宝物合成
	void OnCompose(DWORD dwTreasureInfoId, WORD wTreasureNum);

	// 宝物穿戴
	void OnPutOn(BYTE byFormationPos, BYTE byTreasurePos, DWORD dwTreasureUID);

	// 熔炼
	void OnNetSmelt(DWORD dwTreasureInfoId);

	// 升品
	void OnNetQualityUP(DWORD dwTreasureUID);

	// 请求宝物列表
	void OnTreasureList();
public:
	// 添加宝物
	bool AddTreasure(DWORD dwTreasureInfoId, BOOL bNotice = TRUE, WORD wFromId = 0);

	// 添加宝物
	bool AddTreasure(const Treasure &sTreasure, BOOL bNotice = TRUE);

	// 删除宝物
	bool RemoveTreasure(DWORD dwUID, WORD wFromId = 0);

	// 删除宝物(只删除没有强化及精练的)
	bool RemoveTreasureByInfoId(DWORD dwInfoId, WORD wFromId);

	// 取宝物背包空闲数量
	WORD GetBagEmptySize();

	// 取宝物强化等级对应的总经验
	DWORD GetStrengthLevelTotalExp(WORD wQuality, WORD wLevel);

	// 取宝物数据
	const Treasure* GetTreasure(DWORD dwTreasureUID);

	// 宝物更新通知
	void TreasureUpdateNotify(DWORD dwTreasureUID, BYTE byFlag = true);

	// 宝物更新通知
	void TreasureUpdateNotify(const Treasure &sTreasure, BYTE byFlag = true);

	// 处理强化
	bool DoStrengthen(Treasure *pTreasure, const Treasure_Config *pTreasureCfg, DWORD dwCostTreasureUId, int &nCostGold);

	// 提供给其他模块使用
public:
	// 取到达指定品质的宝物数量
	WORD GetTreasureNumByQuality(WORD wQuality);

	// 取已经穿戴中最小强化等级的定物
	WORD GetMinEnhanceLevelInWear();
	
	// 取已经穿戴中最低品质的宝物
	WORD GetLowQualityInWear();

protected:
	// 查找宝物
	Treasure* FindTreasure(DWORD dwUID);

	// 取一个新的系列id
	DWORD GetNewSeriesId();

public:
	DWORD m_dwSeriesId; // 系列ID
	Player *m_pPlayer;
	PlayerTreasureInfo *m_pTreasureInfo;
};