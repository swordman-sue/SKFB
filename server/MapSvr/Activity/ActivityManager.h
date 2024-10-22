/**\file
�������
\author Kevin
\date 2017/1/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "LimitActivity.h"
#include "SuperDiscount.h"
#include "FestivalActivity.h"
#include "AccurateRecommend.h"
#include "FacebookActivity.h"
#include "SevenDayLogin.h"
#include "RechargeReward.h"
#include "TotalRechargeReward.h"
#include "HotSaleGoods.h"
#include "CampRecruit.h"
#include "UnlockActivity.h"
#include "RechargeAndCostRankActivity.h"

class Player;
struct TQT_Config;
struct Activity_Config;
struct ActivityBase_Config;


class CActivityManager
{
public:
	CActivityManager();
	~CActivityManager();

	void Init(Player *pPlayer);

	void OnNewDay();

	// �ǳ�
	void OnLogout();

	///// ǩ��ģ��
public:
	// ǩ��
	void OnNetSignIn();

	// ǩ��
	void OnNetSignInForTest();

	// ����ǩ������
	void OnNetSignInDataReq();

	// ����ǩ������
	void SendSignInData();

	// ��ȡ����ǩ������
	void OnNetTakeLuxurySignReward(WORD wSignId);

	// �������ǩ������
	void OnNetLuxurySignData();

	// ���ͺ���ǩ������
	void SendLuxurySignData();

	// ��ʼ������ǩ������
	void InitLuxurySignInData();

	// ����ǩ����ֵ
	void LuxurySignInRecharge(WORD wRechangeId);

	//// ӭ����(���)
public:
	// ��ȡ��������
	void OnNetYCSReward();

	// ����ӭ��������
	void OnNetYCSDataReq();

	// ����ӭ��������
	void SendYCSData();

	//// ͭȸ̨(����)
public:
	// ��ȡ��������
	void OnNetTQTReward();

	// ����ͭȸ̨����
	void OnNetTQTDataReq();

	// ����ͭȸ̨����
	void SendTQTData();

	// �Ƿ�ͭȸ̨�ʱ��
	bool IsTQTActivityTime(int nTime);

	// ͨ��ʱ��ȡͭȸ̨�����
	const TQT_Config * GetTQTCfgByTime(int nTime);

	// VIP���
public:
	// VIP�����������
	void OnNetVipGiftDataReq();

	// ÿ�������ȡ
	void OnNetVipDailyGiftTake(WORD wVipLevel);

	// ÿ���������
	void OnNetVipWeekGiftBuy(WORD wGiftId);

	// ����VIP�������
	void SendVipGiftData();

	// ˢ�������
	void RefreshWeekGift();

	// ��ȡ�����Ƽ���ֵ�Ľ���
	void OnNetTakeTodayRecommendReward();

	// ���ý����Ƽ���ֵ����
	void ResetTodayRecommendRechargeData();

	// �ȼ��̵�
public:
	// �ȼ��̵���������
	void OnNetLevelShopActivityDataReq();
	
	// ��ɫ�ȼ�����
	void RoleLevelUp(WORD wFrom, WORD wTo);

	// ȡ�ȼ��̵�����
	const LevelShopActivityData* GetLevelShopActivityData(WORD wRoleLevel);

// �ȼ����
public:
	// �ȼ������������
	void OnNetLevelGiftDataReq();

	// �ȼ������ȡ
	void OnNetTakeLevelGift(WORD wLevel);

	// �Ƿ�����ȡ�ȼ����
	bool IsTakeLevelGift(WORD wLevel);

	// ��������
public:
	// ����������������
	void OnNetOpenServerFundDataReq();

	// ����������
	void OnNetOpenServerFundBuy();

	// ������������ȡ
	void OnNetOpenServerFundRewardTake(WORD wRewardId);

	// ȫ������ȡ
	void OnNetAllPeopleWelfareTake(WORD wWelfareId);

	// ���Ϳ�����������
	void SendOpenServerFundData();

	// �Ƿ�����ȡ������
	bool IsTakeFundReward(WORD wRewardId);

	// �Ƿ�����ȡȫ����
	bool IsTakeAllPeopleWelfare(WORD wWelfareId);

// ��ʱ�Żݻ
public:
	// ������ʱ�Żݻ����
	void OnNetLimitPreferentialReq();

	// ��ȡ��ʱ�Żݸ���
	void OnNetTakeLimitPreferentialWelfare(WORD wWelfareId);

	// ������ʱ�Żݻ����
	void SendLimitPreferentialData();

// ���ս���
public:
	// ���ս�����������
	void OnNetNextDayRewardData();

	// ��ȡ���ս���
	void OnNetTakeNextDayReward();

// ��¼����
public:
	// ��¼������������
	void OnNetLoginRewardData();

	// ��ȡ��¼��������
	void OnNetTakeLoginReward(WORD wDays);

	// ����Ƿ�����ȡ��¼����
	bool IsTakeLoginReward(WORD wDays);

// ���߽���
public:
	// ���߽�����������
	void OnNetOnlineRewardData();

	// ��ȡ���߽�������
	void OnNetTakeOnlineReward();

	// �������߽�������
	void SendOnlineRewardData();

	// ȡ����ʱ��
	DWORD GetOnlineTime();


public:
	// ȡ�����ID
	WORD GetActivityDataId(int nSystemId);

	// ȡ�����
	WORD GetActivityDay(int nSystemId);

	// �Ƿ��к��
	bool IsHaveRedPoint(WORD wSystemId);

	// ���ʼʱ��(ȡ���ڽ��еĻ��ʼʱ��)(todo::��ʱ��ʹ��,�ӿ�û���)
	//DWORD GetActivityStartTime(int nSystemId);

public:

	// �Ƿ�����
	//static bool IsActivityOpen(int nSystemId, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime);

	// �Ƿ�����
	static bool IsActivityOpen(int nSystemId, Player *pPlayer);

	// �Ƿ�����
	static bool IsActivityOpen(const Activity_Config *pActivityCfg, DWORD dwServerId, WORD wRoleLevel, DWORD dwCreateRoleTime);

	// ���ʱ��
	static bool CheckActivityTime(const ActivityBase_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// ���ʱ��(��ʱ����ʹ��)
	//static bool CheckActivityTimeEx(const ActivityBase_Config *pActivityCfg, DWORD dwServerId);

	// ȡ���������
	static int GetActivityRewardDate(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime);

	// ȡ�����ʱ��
	static int GetActivityRewardTime(WORD wSystemId, DWORD dwServerId, DWORD dwCreateRoleTime);

	// �Ƿ�����
	static bool IsActivityOver(DWORD dwServerId, int nSystemId, WORD wDataId, DWORD dwCreateRoleTime);

	// ȡ��ʱ��������
	static int GetLimitProductMultiple(WORD wMapType);

	// �����Ƽ���ֵ�ص�
	void OnTodayRecommendRechargeCallback();

public:
	// ȡ��ʱ��߼���
	CLimitActivity& GetLimitActivity() { return m_LimitActivity; };
	CSuperDiscount& GetSuperDiscount() { return m_SuperDiscount; };
	CNewSuperDiscount& GetNewSuperDiscount() { return m_NewSuperDiscount; };
	CFestivalActivity& GetFestivalActivity() { return m_FestivalActivity; };
	CAccurateRecommend& GetAccurateRecommend() { return m_AccurateRecommend; };
	CFacebookActivity& GetFacebookActivity() { return m_FacebookActivity; };
	CSevenDayLogin& GetSevenDayLogin() { return m_SevenDayLogin; };
	CRechargeReward& GetRechargeReward() { return m_RechargeReward; };
	CTotalRechargeReward& GetTotalRechargeReward() { return m_TotalRechargeReward; };
	CHotSaleGoods& GetHotSaleGoods() { return m_HotSaleGoods; };
	CCampRecruit& GetCampRecruit() { return m_CampRecruit; };
	CUnlockActivity& GetUnlockActivity() { return m_UnlockActivity; };
	CRechargeAndCostRankActivity& GetRechargeAndCostRankActivity() { return m_RechargeAndCostRankActivity; };

private:
	Player *m_pPlayer;
	SignInData *m_pSignInData;
	LoginRewardData *m_pLoginRewardData;
	PlayerLuxurySignData *m_pLuxurySignInData;
	PlayerActivityData *m_pActivityInfo;

	CLimitActivity m_LimitActivity; // ��ʱ��߼���
	CSuperDiscount m_SuperDiscount;
	CNewSuperDiscount m_NewSuperDiscount;
	CFestivalActivity m_FestivalActivity; // ���ջ
	CAccurateRecommend m_AccurateRecommend;
	CFacebookActivity m_FacebookActivity;
	CSevenDayLogin m_SevenDayLogin;
	CRechargeReward m_RechargeReward;
	CTotalRechargeReward m_TotalRechargeReward;
	CHotSaleGoods m_HotSaleGoods;
	CCampRecruit m_CampRecruit;
	CUnlockActivity m_UnlockActivity;
	CRechargeAndCostRankActivity m_RechargeAndCostRankActivity;
};