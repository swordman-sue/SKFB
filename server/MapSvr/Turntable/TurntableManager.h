/**\file
转盘管理器
\author Kevin
\date 2018/3/20
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;

// 转盘数据
struct TurntableData : public TurntableBaseData
{
	deque<TurntableLotteryRecord> mLotteryRecordList; // 抽奖记录列表
};

class CTurntableManager : public Singleton<CTurntableManager>
{
public:
	CTurntableManager();
	~CTurntableManager();

	void Init();

	void Update();

	// 加载数据
	void SendDataToDB();

	// 存储数据
	void LoadDataFromDB(const char *pData, int size);

	// 加载数据请求
	void LoadDataRequest();

public:

	// 请求转盘数据
	void OnNetTurntableData(Player *pPlayer);

	// 转盘抽奖请求
	void OnNetTurntableLottery(Player *pPlayer, WORD wLotteryType, BYTE byIsCostItem);


protected:
	// 发送转盘数据
	void SendTurntableData(Player *pPlayer);

	// 添加抽奖记录
	void AddLotteryRecord(DWORD dwServerId, const TurntableLotteryRecord &tLotteryRecrod);

	// 发奖励排行奖励
	void DoRankReward(DWORD dwServerId, WORD wDataId);

	// 取转盘数据
	TurntableData* GetTurntableData(DWORD dwServerId);

private:

	bool m_bIsUpdate;
	bool m_bLoadDataFromDB;
	DWORD m_dwLastProccessTimer;

	// 转盘数据列表[server_id, data_list]
	map<DWORD, TurntableData> m_TurntableDataList; 
};
