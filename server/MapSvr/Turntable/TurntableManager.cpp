/**\file
转盘管理器
\author Kevin
\date 2018/3/20
\note
*/

#include "stdafx.h"
#include "TurntableManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Hero\HeroManager.h"
#include "DBCDefine.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "..\Fuben\FubenManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Activity\ActivityManager.h"
#include "..\Rank\RankManager.h"
#include "..\RewardCenter\RewardCenterManager.h"


CTurntableManager::CTurntableManager()
{
	m_bIsUpdate = false;
	m_bLoadDataFromDB = false;
	m_dwLastProccessTimer = time(NULL);
};

CTurntableManager::~CTurntableManager()
{

};

void CTurntableManager::Init()
{
	//m_TurntableDataList.clear();
	LoadDataRequest();
}

 void CTurntableManager::Update()
 {
	 if (!m_bLoadDataFromDB) return;

	 vector<DWORD> vDeleteList;
 	map<DWORD, TurntableData>::iterator iter = m_TurntableDataList.begin();
 	for (; iter != m_TurntableDataList.end(); ++iter)
 	{
 		TurntableData *pTurntableData = &iter->second;
 
 		// 检测活动是否已结束
 		if (pTurntableData->wCurActivityDataId && 
			CActivityManager::IsActivityOver(pTurntableData->dwServerId, SYSTEM_TURNTABLE, pTurntableData->wCurActivityDataId, 0))
 		{
 			// 发放排行榜奖励
 			DoRankReward(pTurntableData->dwServerId, pTurntableData->wCurActivityDataId);
 
 			// 活动结束数据重置
//  			pTurntableData->dwDiamond = 0;
//  			pTurntableData->wCurActivityDataId = 0;
//  			pTurntableData->mLotteryRecordList.clear();

			// 重置排行榜
			CRankManager::Instance()->ResetRoleRank(pTurntableData->dwServerId, RANK_TURNTABLE_LOTTERY);

			m_bIsUpdate = true;

			vDeleteList.push_back(pTurntableData->dwServerId);
 		}
 	}

	// 删除已经结束的服务器数据
	for (vector<DWORD>::iterator iter = vDeleteList.begin(); iter != vDeleteList.end(); ++iter)
	{
		m_TurntableDataList.erase(*iter);
	}

	// 检测数据存储时间
	DWORD dwTime = time(NULL);
	if (m_bIsUpdate && dwTime >= m_dwLastProccessTimer + 60)
	{
		SendDataToDB();
		m_dwLastProccessTimer = dwTime;
		m_bIsUpdate = false;
	}
 }

 // 加载数据
 void CTurntableManager::SendDataToDB()
 {
	 // 还木有处理,不处理
// 	 if (!m_TurntableDataList.size())
// 		 return;

	 PB::AllServerTurntableData PBAllServerTurntableDataList;
	 map<DWORD, TurntableData>::iterator iter = m_TurntableDataList.begin();
	 for (; iter != m_TurntableDataList.end(); ++iter)
	 {
		 ::PB::TurntableData *pPBTurntableData =  PBAllServerTurntableDataList.add_turntable_data_list();
		 pPBTurntableData->set_server_id(iter->second.dwServerId);
		 pPBTurntableData->set_cur_activity_data_id(iter->second.wCurActivityDataId);
		 pPBTurntableData->set_diamond(iter->second.dwDiamond);
		
		 deque<TurntableLotteryRecord>::iterator lottery_record_iter = iter->second.mLotteryRecordList.begin();
		 for (; lottery_record_iter != iter->second.mLotteryRecordList.end(); ++lottery_record_iter)
		 {
			 ::PB::TurntableLotteryRecord *pPBLotteryRecord = pPBTurntableData->add_lottery_record_list();
			 pPBLotteryRecord->set_role_id(lottery_record_iter->dwRoleId);
			 pPBLotteryRecord->set_time(lottery_record_iter->dwTime);
			 pPBLotteryRecord->set_index(lottery_record_iter->wIndex);
			 pPBLotteryRecord->set_context(lottery_record_iter->dwContext);
		 }
	 }

	 string strSaveData("");
	 PBAllServerTurntableDataList.AppendToString(&strSaveData);
	 if (strSaveData.size() > MAX_TURNTABLE_LIST_DATA_SIZE)
	 {
		 MAPLOG(ERROR_LEVEL, "[%s]转盘存储数据过大[size:%d]", __FUNCTION__, strSaveData.size());
		 return;
	 }

	 //if (strSaveData.size())
	 {
		 TurntableData_Save* pQuery = TurntableData_Save::ALLOC();
		 pQuery->SetIndex(DB_TURNTABLE_DATA_SAVE);
		 pQuery->SetThreadIndex(DB_THREAD_COMMON);
		 pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
		 g_MapServer.MapDBQuery(pQuery);
	 }
 }

 // 存储数据
 void CTurntableManager::LoadDataFromDB(const char *pData, int size)
 {
	 m_bLoadDataFromDB = true;

	 if (size <= 0)
		 return;

	 m_TurntableDataList.clear();

	 PB::AllServerTurntableData PBAllServerTurntableDataList;
	 if (!PBAllServerTurntableDataList.ParseFromArray(pData, size))
	 {
		 MAPLOG(ERROR_LEVEL, "解析转盘数据出错(size:%d)", size);
		 return;
	 }

	 TurntableData tTurntableData;
	 TurntableLotteryRecord tLotteryRecord;
	 for (int i = 0; i < PBAllServerTurntableDataList.turntable_data_list_size(); ++i)
	 {
		 tTurntableData.mLotteryRecordList.clear();
		 const ::PB::TurntableData &tPBTurntableData = PBAllServerTurntableDataList.turntable_data_list(i);
		 tTurntableData.dwServerId = tPBTurntableData.server_id();
		 tTurntableData.wCurActivityDataId = tPBTurntableData.cur_activity_data_id();
		 tTurntableData.dwDiamond = tPBTurntableData.diamond();
		 for (int k = 0; k < tPBTurntableData.lottery_record_list_size(); ++k)
		 {
			 const ::PB::TurntableLotteryRecord &tPBLotteryRecord = tPBTurntableData.lottery_record_list(k);
			 tLotteryRecord.dwRoleId = tPBLotteryRecord.role_id();
			 tLotteryRecord.dwTime = tPBLotteryRecord.time();
			 tLotteryRecord.wIndex = tPBLotteryRecord.index();
			 tLotteryRecord.dwContext = tPBLotteryRecord.context();
			 tTurntableData.mLotteryRecordList.push_back(tLotteryRecord);
		 }
		 m_TurntableDataList.insert(make_pair(tTurntableData.dwServerId, tTurntableData));
	 }
 }

 // 加载数据请求
 void CTurntableManager::LoadDataRequest()
 {
	 TurntableData_Load* pQuery = TurntableData_Load::ALLOC();
	 pQuery->SetIndex(DB_TURNTABLE_DATA_LOAD);
	 pQuery->SetThreadIndex(DB_THREAD_COMMON);
	 g_MapServer.MapDBQuery(pQuery);
 }
 
 // 请求转盘数据
 void CTurntableManager::OnNetTurntableData(Player *pPlayer)
 {
 	WORD wDataId = pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_TURNTABLE);
 	if (!wDataId)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]活动还没开始或已结束[ServerId:%d]", __FUNCTION__, pPlayer->GetServerId());
 		return;
 	}
 
 	// 如果还没初始化
 	map<DWORD, TurntableData>::iterator iter = m_TurntableDataList.find(pPlayer->GetServerId());
 	if (iter == m_TurntableDataList.end())
 	{
 		const Turntable_Config *pTurntableCfg = g_pCfgMgr->GetTurntableData(wDataId);
 		if (!pTurntableCfg)
 		{
 			MAPLOG(ERROR_LEVEL, "[%s]找不到转盘数据配置[DataId:%d]", __FUNCTION__, wDataId);
 			return;
 		}
 
 		TurntableData tData;
 		tData.dwServerId = pPlayer->GetServerId();
 		tData.dwDiamond = pTurntableCfg->nInitDiamond;
 		tData.wCurActivityDataId = wDataId;
 		tData.mLotteryRecordList.clear();
 		m_TurntableDataList.insert(make_pair(pPlayer->GetServerId(), tData));
 	}
	else
	{
		// todo::补救代码,防止配置出问题导致活动数据ID出错
		if (iter->second.wCurActivityDataId != wDataId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]转盘活动数据缓存的数据ID与实际不一致,被纠正[MemDataId:%d,CfgDataId:%d]", 
				__FUNCTION__, iter->second.wCurActivityDataId, wDataId);

			iter->second.wCurActivityDataId = wDataId;
		}
	}

	// 如果是活动重新开启,则重置数据
	if (pPlayer->GetRoleFullInfo().tRoleBaseInfo.wTurntableActivityDataId != wDataId)
	{
		pPlayer->GetRoleFullInfo().tRoleBaseInfo.wTurntableActivityDataId = wDataId;
		pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwTurntableScore = 0;
	}
 
 	SendTurntableData(pPlayer);
 }
 
 // 转盘抽奖请求
 void CTurntableManager::OnNetTurntableLottery(Player *pPlayer, WORD wLotteryType, BYTE byIsCostItem)
 {
 	DWORD dwServerId = pPlayer->GetServerId();
 
 	// 检测是否在活动中
 	WORD wDataId = pPlayer->GetActivityManager()->GetActivityDataId(SYSTEM_TURNTABLE);
 	if (!wDataId)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]活动还没开始或已结束[ServerId:%d]", __FUNCTION__, dwServerId);
 		return;
 	}
 
 	// 取转盘数据
 	TurntableData *pTurntableData = GetTurntableData(dwServerId);
 	if (!pTurntableData)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找到转盘数据[ServerId:%d]", __FUNCTION__, dwServerId);
 		return;
 	}
 
 	// 取转盘配置
 	const Turntable_Config *pTurntableCfg = g_pCfgMgr->GetTurntableData(wDataId);
 	if (!pTurntableCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找到转盘数据[ServerId:%d,DataId:%d]", __FUNCTION__, dwServerId, wDataId);
 		return;
 	}
 
 	// 取抽奖配置
 	const vector<TurntableLottery_Config> *pLotteryCfg = g_pCfgMgr->GetTurntableLottery(wDataId);
 	if (!pLotteryCfg && !pLotteryCfg->size())
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找到转盘抽奖配置数据[ServerId:%d,DataId:%d]", __FUNCTION__, dwServerId, wDataId);
 		return;
 	}
 
 	int nLotteryTimes = 1;
 	int nLotteryDiamond = 0;
 	int mLotteryScore = 0;
	int nCostDiamond = 0;
 	
 	// 普通抽奖
 	if (0 == wLotteryType)
 	{
 		mLotteryScore = pTurntableCfg->nNormalLotteryScore;
 		nLotteryTimes = 1;
 		nLotteryDiamond = pTurntableCfg->nNormalLotteryDiamond;
		nCostDiamond = pTurntableCfg->nNormalLotteryCostDiamond;
 	}
 	// 一键抽奖
 	else
 	{
 		mLotteryScore = pTurntableCfg->nOneKeyLotteryScore;
 		nLotteryTimes = pTurntableCfg->nOneKeyLotteryTimes;
 		nLotteryDiamond = pTurntableCfg->nOneKeyLotteryDiamond;
		nCostDiamond = pTurntableCfg->nOneKeyLotteryCostDiamond;
 	}
 
 	DWORD dwCostItemNum = pTurntableCfg->dwCostItemNum * nLotteryTimes;
 
 	// 检测消耗
	if (byIsCostItem)
	{
		// 消耗道具
		if (pPlayer->GetItemNum(pTurntableCfg->dwCostItemId) < dwCostItemNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]转盘抽奖消耗道具不足[DataId:%d,CostItemId:%d,CostItemNum:%d]", __FUNCTION__,
				wDataId, pTurntableCfg->dwCostItemId, dwCostItemNum);
			return;
		}

		// 扣除物品
		pPlayer->RemoveItem(pTurntableCfg->dwCostItemId, dwCostItemNum, FROM_TURNTABLE_LOTTERY);
	}
	else
	{
		// 消耗钻石
		if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			MAPLOG(ERROR_LEVEL, "[%s]转盘抽奖消耗钻石不足[DataId:%d,CostDiamond:%d]", __FUNCTION__,wDataId, nCostDiamond);
			return;
		}

		// 扣除钻石
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TURNTABLE_LOTTERY);
	}

 	// 奖池添加钻石
 	//pTurntableData->dwDiamond += mLotteryScore;
  
 	int nTotalWeight = 0;
 	vector<TurntableLottery_Config>::const_iterator iter = pLotteryCfg->begin();
 	for (; iter != pLotteryCfg->end(); ++iter)
 	{
 		nTotalWeight += iter->nWeight;
 	}
 
 	if (!nTotalWeight)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]转盘抽奖总权值为0[ServerId:%d,DataId:%d]", __FUNCTION__, dwServerId, wDataId);
 		return;
 	}
 
	// 奖池添加钻石
 	pTurntableData->dwDiamond += nLotteryDiamond;

	pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwTurntableScore += mLotteryScore;
	DWORD dwCurTurntableScore = pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwTurntableScore;
 
 	// 处理随机逻辑
 	vector<WORD> vIndexList;
 	vector<DWORD> vItemNumList;
 	for (int i = 0; i < nLotteryTimes; ++i)
 	{
 		int nRandValue = g_Rand(0, nTotalWeight);
 		int nCurValue = 0;
 		for (iter = pLotteryCfg->begin(); iter != pLotteryCfg->end(); ++iter)
 		{
 			nCurValue += iter->nWeight;
 			if (nRandValue <= nCurValue)
 			{
 				DWORD dwValue = 0;
 
 				if (iter->nRewardDiamondRate)
 				{
 					// 按比例获得钻石
 					dwValue = pTurntableData->dwDiamond * float(iter->nRewardDiamondRate * 0.0001);
 
 					// 奖池更新
 					pTurntableData->dwDiamond -= dwValue;
 
 					// 发奖励(钻石)
					pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, dwValue, TRUE, FROM_TURNTABLE_LOTTERY);
 				}
 				else
 				{
 					dwValue = g_Rand(iter->dwRewardItemNumMin, iter->dwRewardItemNumMax);
 
 					// 发奖励(物品)
					pPlayer->AddObject(iter->dwRewardItemId, dwValue, TRUE, FROM_TURNTABLE_LOTTERY);
 				}
 
 				vIndexList.push_back(iter->wIndex);
 				vItemNumList.push_back(dwValue);
 
				// MAPLOG(ERROR_LEVEL, "[%s]转盘抽中[Index:%d,Value:%d]", __FUNCTION__, iter->wIndex, dwValue);

 				// 添加抽奖记录
 				if (iter->bIsNeedRecord)
 				{
 					TurntableLotteryRecord tLotteryRecord;
 					tLotteryRecord.dwRoleId = pPlayer->GetRoleId();
 					tLotteryRecord.dwTime = time(NULL);
 					tLotteryRecord.wIndex = iter->wIndex;
 					tLotteryRecord.dwContext = dwValue;
 					AddLotteryRecord(dwServerId, tLotteryRecord);
 				}
 				break;
 			}
 		}
 	}
 
 	// 响应
 	MSG_S2C_TURNTABLE_LOTTERY_RESP msg;
 	msg.m_wLotteryType = wLotteryType;
 	for (int i = 0; i < vIndexList.size() && i < vItemNumList.size(); ++i)
 	{
 		TurntableLotteryResultData tLotteryReuslt;
 		tLotteryReuslt.wIndex = vIndexList[i];
 		tLotteryReuslt.dwContext = vItemNumList[i];
 		msg.m_LotteryResultList.push_back(tLotteryReuslt);
 	}
 	pPlayer->SendPacket(&msg);

	// 处理排行榜
	if (dwCurTurntableScore >= pTurntableCfg->nRankNeedScore)
	{
		CRankManager::Instance()->AddRoleRankData(RANK_TURNTABLE_LOTTERY, pPlayer, dwCurTurntableScore, 0, 0, 200);
	}

	m_bIsUpdate = true;
 }
 
 // 发送转盘数据
 void CTurntableManager::SendTurntableData(Player *pPlayer)
 {
	 DWORD dwServerId = pPlayer->GetServerId();

	 TurntableData *pTurntableData = GetTurntableData(dwServerId);
 	if (!pTurntableData)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到转盘数据[ServerId:%d]", __FUNCTION__, dwServerId);
 		return;
 	}
 
 	ClientTurntableLotteryRecord tClientLotteryRecrod;
 
 	MSG_S2C_TURNTABLE_DATA_RESP msg;
	msg.m_dwCurRankScore = pPlayer->GetRoleFullInfo().tRoleBaseInfo.dwTurntableScore;
 	msg.m_dwDiamond = pTurntableData->dwDiamond;
 	msg.m_wCurActivityDataId = pTurntableData->wCurActivityDataId;
 	deque<TurntableLotteryRecord>::iterator iter = pTurntableData->mLotteryRecordList.begin();
 	for (; iter != pTurntableData->mLotteryRecordList.end(); ++iter)
 	{
 		memcpy_s(&tClientLotteryRecrod, sizeof(TurntableLotteryRecord), &*iter, sizeof(TurntableLotteryRecord));
 		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter->dwRoleId,false);
 		if (pBaseData)
 		{
 			strcpy_s(tClientLotteryRecrod.szRoleName, sizeof(tClientLotteryRecrod.szRoleName), pBaseData->szRoleName);
 		}
 		msg.m_LotteryRecordList.push_back(tClientLotteryRecrod);
 	}
 	pPlayer->SendPacket(&msg);
 }
 
 // 添加抽奖记录
 void CTurntableManager::AddLotteryRecord(DWORD dwServerId, const TurntableLotteryRecord &tLotteryRecord)
 {
 	TurntableData *pTurntableData =  GetTurntableData(dwServerId);
 	if (!pTurntableData)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到转盘数据[ServerId:%d]", __FUNCTION__,dwServerId);
 		return;
 	}
 
 	pTurntableData->mLotteryRecordList.push_back(tLotteryRecord);
 
 	// 只取最近4条记录 todo::需要调整为配置
	if (pTurntableData->mLotteryRecordList.size() > MAX_TURNTALE_LOTTERY_RECORD_NUM)
 	{
 		pTurntableData->mLotteryRecordList.pop_front();
 	}
 }
 
 // 发奖励排行奖励
 void CTurntableManager::DoRankReward(DWORD dwServerId, WORD wDataId)
 {
 	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(dwServerId, RANK_TURNTABLE_LOTTERY);
 	if (!pRankList)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家排行榜数据[ServerId:%d,RankType:%d]", __FUNCTION__, dwServerId, RANK_TURNTABLE_LOTTERY);
 		return;
 	}
 
 	RoleRankList::const_iterator iter = pRankList->begin(); 
 	for (; iter != pRankList->end(); ++iter)
 	{
 		const TurntableRank_Config *pRankCfg = g_pCfgMgr->GetTurntableRank(wDataId, iter->wRank);
 		if (!pRankCfg)
 		{
 			MAPLOG(ERROR_LEVEL, "[%s]找不到转盘排行奖励配置[DataId:%d,Rank:%d]", __FUNCTION__, wDataId, iter->wRank);
 			continue;
 		}
 

 		RewardData tRewardData;
 		tRewardData.wInfoId = 25;
 		tRewardData.dwTime = time(NULL);
 		tRewardData.dwValue1 = iter->wRank;
		tRewardData.dwValue2 = wDataId;
 		CRewardCenterManager::AddReward(dwServerId, iter->dwRoleId, tRewardData);
 
 		MAPLOG(GUI_LEVEL, "[%s]转盘排行榜发放奖励[ServerId:%d,DataId:%d,RoleId:%d]", __FUNCTION__, dwServerId, wDataId, iter->dwRoleId);
 	}
 
	MAPLOG(IMPORTANT_LEVEL, "[%s]转盘排行榜发放奖励[ServerId:%d,DataId:%d,RankSize:%d]", __FUNCTION__, dwServerId, wDataId, pRankList->size());
 }
 
 // 取转盘数据
 TurntableData* CTurntableManager::GetTurntableData(DWORD dwServerId)
 {
 	map<DWORD, TurntableData>::iterator iter = m_TurntableDataList.find(dwServerId);
 	if (iter == m_TurntableDataList.end())
 	{
 		TurntableData tData;
 		tData.dwServerId = dwServerId;
 		tData.dwDiamond = 0;
 		tData.wCurActivityDataId = 0;
 		tData.mLotteryRecordList.clear();
 		m_TurntableDataList.insert(make_pair(dwServerId, tData));
 		iter = m_TurntableDataList.find(dwServerId);
 	}
 	return &iter->second;
 }