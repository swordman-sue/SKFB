/**\file
精灵管理器
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CSpriteManager
{
public:
	CSpriteManager();
	~CSpriteManager();

	void Init(Player *pPlayer);

public:
	// 请求精灵数据
	void OnNetSpiritDataReq();

	// 精灵强化
	void OnNetSpiritEnhance(DWORD dwSpiritId);

	// 精灵合成
	void OnNetCompose(DWORD dwFragmentId);

	// 精灵培养(todo::目前已不使用)
	void OnNetTrainEx(DWORD dwSpriteId);

	// 精灵培养
	void OnNetTrain(DWORD dwSpriteId, WORD wTrainTimes);

	// 精灵升星
	void OnNetStarUp(DWORD dwSpriteId);

	// 精灵祝福
	void OnNetBless(WORD wBlessType, WORD wFormationPos, DWORD dwSpiritId);

	// 精灵使用
	void OnNetSpiritUse(DWORD dwSpiritId);

	// 世界之心数据请求
	void OnNetHeartOfWorldData();

	// 世界之心升级
	void OnNetHeartOfWorldLevelUp();


public:

	// 是否有此精灵
	bool HaveSpirit(DWORD dwSpiritId);

	// 添加精灵
	bool AddSpirit(DWORD dwSpiritId, WORD wFromId = 0);

	// 添加精灵
	bool AddSpirit(const SpiritData &tSpriteData);

	// 发送精灵数据
	void SendSpiritData();

	// 取图鉴等级
	WORD GetPokedexLevel(WORD wPokedexId);

	// 取图鉴强化等级
	WORD GetEnhanceLevel(WORD wPokedexId);

	// 是否图鉴已激活
	bool IsPokedexActivate(WORD wPokedexId);

	// 添加图鉴
	void AddPokedex(WORD wPokedexId);

	// 发送世界之心数据
	void SendHeartOfWorldData();

	// 取出战精灵ID
	DWORD GetToBattleSpiritId();

	// 取精灵强化等级
	WORD GetSpiritEnhanceLevel(DWORD dwSpiritId);

	// 取精灵培养次数
	WORD GetSpiritTrainTimes(DWORD dwSpiritId);

	// 取精灵数据
	const SpiritData* GetSpiritEx(DWORD dwSpiritId);

	// 取祝福精灵数量
	WORD GetBlessSpriteNum();

	// 取当前祝福精灵数量上限
	WORD GetCurMaxBlessSpriteNum();

	// 取祝福英雄列表
	void GetBlessSpriteList(vector<SpiritData> &vSpriteList);

	// 取祝福类型
	WORD GetBlessType(DWORD dwOldSpriteId, DWORD dwSpiritId);

	// 取精灵列表
	const SpiritData* GetSpriteList() { return m_pSpriteInfo->arSpiritList; };

	// 取精灵列表数量
	WORD GetSpriteListNum() { return m_pSpriteInfo->wSpiritNum; };

	// 取额外奖励的掉落物品信息
	void GetExtraRewardDropItemInfo(WORD wFBType, map<DWORD, int> &mItemInfoList);

	// 取额外奖励的货币信息
	void GetExtraRewardCurrencyInfo(WORD wFBType, map<int, int> &mCurrencyInfoList);

	// 取额外奖励的货币值
	int GetExtraRewardCurrencyValue(const map<int, int> &mCurrencyInfoList, int nCurrencyType, int nCurrencyValue);

protected:

	// 取精灵数据
	SpiritData* GetSpirit(DWORD dwSpiritId);

	// 取精灵图鉴
	SpiritPokedex* GetPokedex(WORD wPokedexId);

private:
	Player *m_pPlayer;
	PlayerSpriteInfo *m_pSpriteInfo;
};