/**\file
装备管理器
\author Kevin
\date 2016/6/29
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CEquipmentManager
{
public:
	CEquipmentManager();
	~CEquipmentManager();

	void Init(Player *pPlayer);

public:
	// 强化 
	void OnEnhance(BYTE byStrengType, DWORD dwEquipmentUID);

	// 一键强化
	void OnNetOneKeyEnhance(BYTE bySlot);

	// 装备精炼
	void OnRefineEx(DWORD dwEquipmentUID, WORD wItemId, WORD wItemNum);

	// 装备精炼
	void OnRefine(DWORD dwEquipmentUID, EquipmentRefineCostData *pCostList, WORD wCostListNum);

	// 装备合成
	void OnCompose(DWORD dwEquipmentInfoId);

	// 请求装备列表
	void OnEquipmentList();

	// 请求装备详情
	void OnEquipmentInfo(DWORD dwUID);

	// 穿戴装备
	void OnEquipmentPutOn(BYTE byFormationPos, BYTE byEquipmentPos, DWORD dwUID);

	// 升星
	void OnNetStarUP(DWORD dwUID);

	// 升品
	void OnNetQualityUP(DWORD dwUID);

public:
	// 添加装备
	bool AddEquipent(DWORD dwEquipmentInfoId, bool bNotify = true, WORD wFromId = 0);

	// 添加装备
	bool AddEquipment(const Equipment &sEquipment, bool bNotify = true, WORD wFromId = 0);

	// 删除装备
	bool RemoveEquipment(DWORD dwEquipmentUID, bool bNotify = true, WORD wFromId = 0);

	// 取装备背包空闲数量
	WORD GetBagEmptySize();

	// 添加精炼消耗物品
	bool AddRefineCostItem(DWORD dwEquipmentUID, DWORD dwItemId, DWORD dwItemNum = 1);

	// 取装备数据
	const Equipment* GetEquipment(DWORD dwUID);

	// 装备更新通知
	void EquipmentUpdateNotify(DWORD dwUID, BYTE byFlag = true);

	// 提供给其他模块使用
public:
	// 取到达指定强化等级的装备数量
	WORD GetEquipmentNumByStrengthenLevel(WORD wStrenthenLevel);

	// 取到达指定精炼等级的装备数量
	WORD GetEquipmentNumByRefineLevel(WORD wRefineLevel);

	// 取到达指定品质的装备数量
	WORD GetEquipmentNumByQuality(WORD wQuality);

	// 取强化最小等级(已装备的)
	// 取最小的强化等级(已装备的)
	WORD GetEquipmentEnhanceMinLevel();

	// 取装备最低品质(已装备的) 
	WORD GetEquipmentLowQuality();

	// 取下个品质的强化等级
	WORD GetNextQualityEnhanceLevel(WORD wCurQuality, WORD wCurEnhanceLevel);

	// 取下个品质的精炼等级
	WORD GetNextQualityRefineLevel(WORD wCurQuality, WORD wCurRefineLevel, DWORD &dwCurExp);

protected:
	// 处理强化
	bool DoEnhance(Equipment *pEquipment, bool &bIsCrit, int &nCostGold);

	// 取装备
	Equipment* FindEquipment(DWORD dwUID);

	// 取一个新的系列id
	DWORD GetNewSeriesId();
public:
	DWORD m_dwSeriesId; // 系列ID
	Player *m_pPlayer;
	PlayerEquipmentInfo *m_pEquipmentInfo;
};