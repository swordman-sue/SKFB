/**\file
仙灵管理器
\author Kevin
\date 2016/11/22
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;

// DivinationManager ZhanBu
class CZhanBuManager
{
public:
	CZhanBuManager();
	~CZhanBuManager();

	void Init(Player *pPlayer);

	// 开始战斗
	bool OnBattleStart();

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin);

public:
	
	// 请求玩家占卜数据
	void OnNetPlayerZhanBuData();

	// 请求卡牌列表
	void OnNetCardList();

	// 卡牌升级
	void OnNetLevelUp(DWORD dwUID, DWORD *pCostCardList, WORD wCostCardNum);

	// 卡牌突破
	void OnNetBreak(DWORD dwUID);

	// 卡牌刷新
	void OnNetCardRefresh(BYTE byRefreshType);

	// 翻卡
	void OnNetOpenCard(BYTE byType, WORD *pIdxList, WORD wIdxListNum);

	// 低级卡组刷新次数请求
	void OnNetLowCardGroupRefreshTimesUpdate();

	// 收藏组激活
	void OnNetCellectionGroupActivate(WORD wBookId, WORD wGroupId);

	// 收藏组升星
	void OnNetCellectionGroupStarUp(WORD wBookId, WORD wGroupId);

	// 收藏组重置
	void OnNetCellectionGroupReset(WORD wBookId, WORD wGroupId);

	// 请求收藏书列表
	void OnNetCollectionBookList();

	// 装备卡牌
	void OnNetEquipCard(WORD wFormationPos, WORD wCardSlot, DWORD dwUID);

	// 卸下卡牌
	void OnNetUnloadCard(WORD wFormationPos, WORD wCardSlot);

	// 激活成就
	void OnNetAcivateAchievement();
public:

	// 取卡牌
	const Card* GetCard(DWORD dwUID);

	// 添加卡牌
	void AddCard(DWORD dwInfoId, bool bIsNotify = true, WORD wFromId = 0);

	// 添加卡牌
	void AddCard(const Card *pCard, bool bIsNotify, WORD wFromId = 0);

	// 删除卡牌
	void RemoveCard(DWORD dwUID, bool bIsNotify = true, WORD wFromId = 0);

	// 批量删除卡牌
	bool RemoveCard(DWORD *pCardUIDList, WORD wCardNum, WORD wFromId = 0);

	// 取初始卡牌
	const Card* GetInitCard(DWORD dwInfoId);

	// 删除初始卡牌
	void RemoveInitCard(DWORD dwInfoId, bool bIsNotify = true);

	// 是否初始卡牌
	bool IsInitCard(const Card *pCard);

	// 取初始卡牌数量
	DWORD GetInitCardNum(DWORD dwInfoId);

	// 取收藏历史记录
	DWORD GetHistoryCollectionValue();

	// 取指定等级的卡牌数量
	int GetCardNumByLevel(WORD wLevel);

	// 取指定阶的卡牌数量
	int GetCardNumByBreakLevel(WORD wBreakLevel);

protected:

	// 取仙灵(内部接口)
	Card* GetCardInner(DWORD dwUID);

	// 取一个新的系列id
	DWORD GetNewSeriesId();

	// 取仙灵数据
	const Card* GetCard(DWORD dwFairyInfoId, WORD wMaxLevel);

	// 发送玩家占卜数据
	void SendPlayerZhanBuData();

	// 卡牌是否已翻开
	bool IsCardAlreadyOpen(BYTE byType, WORD wIdx);

	// 更新低级卡组刷新次数
	void UpdateLowCardGroupRefreshTimes();

	// 取卡牌当前总经验
	DWORD GetCardCurTotalExp(DWORD dwUID);

	// 处理低级卡组刷新
	bool DoLowCardGroupRefresh(bool bIsNewRole = false);

	// 处理高级卡组刷新
	bool DoHighCardGroupRefresh(bool bIsNewRole = false);

	// 卡牌数据通知
	void CardDataNotify(DWORD dwUID, BYTE byFlag = true);

	// 卡牌数据通知
	void CardDataNotify(const Card *pCard, BYTE byFlag = true);

	// 取收藏书数据
	CollectionBook* GetCollectionBook(WORD wBookId);

	// 取收藏组数据
	CollectionGroup* GetCollectionGroup(WORD wBookId, WORD wGroupId);

	// 添加收藏组数据
	void AddCollectGroup(WORD wBookId, WORD wGroupId);

	// 删除收藏组数据
	void RemoveCollectGroup(WORD wBookId, WORD wGroupId);

	// 检测是否已初始化数据
	bool IsAlreadyInitData();

	// 初始数据
	void InitData();

	// 取当前收藏值
	DWORD GetCurCollectionValue();

	// 取包裹空闲数量
	WORD GetBagEmptySize();

	// 更新历史收藏值
	void UpdateHistoryCollectionValue();
	
public:
	DWORD m_dwSeriesId; // 系列ID
	Player *m_pPlayer;
	PlayerZhanBuInfo *m_pZhanBuInfo;
};