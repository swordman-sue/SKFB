/**\file
物品管理器
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CItemManager
{
public:
	CItemManager();
	~CItemManager();

	// 初始化
	void Init(Player *pPlayer);

	// 网络部分入口
public:
	// 请求物品列表
	void OnNetItemList();

	// 请求使用物品
	void OnNetUseItem(DWORD dwItemId, WORD wUseNum, DWORD dwGiftItemId);

	// 请求翻牌
	void OnNetFP(BYTE byFBType);

	// 物品合成
	void OnNetCompose(WORD wItemId);

	// 物品分解
	void OnNetDecompose(WORD wItemId, WORD wItemNum);
public:

	// 取物品数量
	DWORD GetItemNum(DWORD dwItemId);

	// 取物品
	const Item* GetItem(DWORD dwItemId);

	// 添加物品
	bool AddItem(DWORD dwItemId, DWORD dwNum = 1, BOOL bNotify = TRUE, WORD wFromId = 0);

	// 移除物品(返回值为实际移除的数量)
	DWORD RemoveItem(DWORD dwItemId, DWORD dwNum = 1, BOOL bNotify = TRUE, WORD wFromId = 0);

	// 使用物品
	bool UseItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> *pItemList, DWORD dwGiftItemId = 0, bool bAddToCreateList = false, WORD wFromId = 0);

	// 设置翻牌物品ID
	//void SetFPItemId(DWORD dwItemId) { m_dwFPItemId = dwItemId; };

	// 设置翻牌物品
	void SetFPItem(Item tItem) { m_tFPItem = tItem; };

	// 使用随机物品
	static bool UseRandItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> &vItemList);

	// 删除过期物品
	void DeleteTimeOutItem();

protected:
	// 查找物品
	Item* FindItem(DWORD dwItemId);

	// 添加使用物品产生的物品
	void AddCreateItem(DWORD dwItemId, DWORD dwItemNum = 1);

	// 添加使用产生的货币
	void AddCreateMoney(BYTE byType, int nValue);

private:
	Player *m_pPlayer;
	PlayerItemInfo *m_pPlayerItemInfo;

	//DWORD m_dwFPItemId;	// 当前翻牌的物品ID
	Item m_tFPItem;	// 当前翻牌的物品

	vector<Item> m_vCreateItemList;			// 使用产生的物品列表 
	vector<MoneyData> m_vCreateMoneyList;	// 使用产生的货币列表 
};