#pragma once

#include "../Common/Const.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/LogFile.h"
#include <hash_map>
#include <map>
#include <deque>
#include "PlayerManager.h"
#include "KeyWodFilter/KeyWodFilter.h"
#include "ToolKit/IniFile.h"

class CGameLogic : public Singleton<CGameLogic>
{
public:
	CGameLogic();
	~CGameLogic();

	void Init(IniFile *pInifile);

	void ReloadInitFile(IniFile *pInifile);

	void Update();

	void OnNewDay();

	// 热云统计
public:
	// 取热云AppId
	const char* GetReyunAppId() { return m_szReyunAppId; };

	// 取热云统计开关
	bool IsOpenReyunStatistics() { return m_bIsOpenReyunStatistics; };

	// 热云统计
	void OnReyunStatistics(WORD wStatisticsType, DWORD dwChannelType, const char *pszAccount, WORD wServerId);

	// 热云统计
	void OnReyunStatistics(const char *pszPath, const char *pszContext);

	// 充值统计
	void OnRechargeStatistics(const RechargeCallbackData &callbackData,Player *pPlayer);

	// 关卡开始统计
	void OnStageStartStatistics(Player *pPlayer, int nX, int nY);

	// 关卡结束统计
	void OnStageOverStatistics(Player *pPlayer, int nX, int nY, bool bIsPass);

	// 货币交易统计
	void OnCurrencyStatistics(WORD wCurrencyType, int nCurrencyValue, const char *pszAccount, DWORD dwChannelType, WORD wServerId);

	// 登录统计
	void OnLoginStatistics(Player *pPlayer);

	// 帐号注册
	void OnRegisterStatistics(DWORD dwChannelType, const char *pszAccount, WORD wServerId);

	// 商品购买统计
	void OnGoodsBuyStatistics(Player *pPlayer, WORD wShopType, WORD wGoodsId, WORD wBuyNum);


	// 游戏统计
public:
	// 创建角色统计
	void OnGameStatistics_CreateRole(DWORD dwRoleId, WORD wServerId);
 
	// 登录统计
	void OnGameStatistics_Login(Player *pPlayer, bool bIsNewDay);

	// 充值统计
	void OnGameStatistics_Recharge(Player *pPlayer, int nAmount, bool bIsTodayFirstPayment, bool bIsFristPayment);

	// 新的一天
	void OnGameStatistics_NewDay();

	// 在线数据统计
	void OnGameStatistics_Online(WORD wServerId, WORD wOnlineNum);

	// MyCard统计相关
public:
	// 充值成功上服
	void OnMyCardRechargeSuccessReport(Player *pPlayer, int nAmount, const char *pszOrderNo);


	// 客户端信息相关
public:
	// 添加客户端信息
	void AddClientInfo(DWORD dwChannelType, const char *pszAccount, ClientInfo &tClientInfo);

	// 取客户端信息
	const ClientInfo* GetClientInfo(DWORD dwChannelType, const char *pszAccount);

	// 添加阵营招募记录数据
	void AddCampRecruitRecord(DWORD dwServerId,WORD wCamp, const CampRecruitRecord &tRecord);

	// 取阵营招募记录列表
	const deque<CampRecruitRecord>* GetCampRecruitRecordList(DWORD dwServerId, WORD wCamp);

	// 热云统计相关数据
private:
	char m_szReyunAppId[64];		// 热云appid
	bool m_bIsOpenReyunStatistics;	// 是否开启热云统计

	map<string, ClientInfo> m_ClientInfoList;		// 客户端信息列表 channelid + account, data	

	// 阵营招募记录列表
	map<DWORD, deque<CampRecruitRecord>> m_CampRecruitRecordList;
};


