/**\file
��ֵ�߼�������
\author Kevin
\date 2017/03/02
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
struct Recharge_Config;


class CRechargeManager
{
public:
	CRechargeManager();
	~CRechargeManager();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// �����ֵ����
	void OnNetRechargeData();

	// ��ֵ����(�˽ӿ��Ѿ���ʹ��,Ŀǰֻ��ΪGMʹ��)
	void OnNetRecharge(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId);

	// ��ֵ����(����)
	void OnNetRechargeEx(WORD wRechargeId, WORD wRechargeType, WORD wGoodsId, DWORD dwContext);

	// �����¿�����
	void OnNetMonthCardData();

	// �¿�����(todo::Ŀǰ����Ч)
	void OnNetMonthCardBuy(WORD wCardId);

	// ��ȡ�¿�����
	void OnNetTakeMonthCardReward(WORD wCardId);

	// ��ȡ��ʱ��ֵ����
	void OnNetTakeLimitRechargeReward(WORD wDataId);

	// ���󶩵���
	void OnNetOrderId(BYTE byRechargeType, WORD wRechargeId, DWORD dwContext);

	// ��ֵ�ɹ��ϱ�
	void OnNetRechargeSuccessReport(WORD wRechargeId, DWORD dwChannel, const char *pszIP, const char *pszOrderId);

	// ���󶩵���
	void OnNetOrderIdFromYD(DWORD dwGuid);


public:
	// ��ֵ����ص�
	static void OnNetRechargeCallbackEx(DWORD dwGuid, const RechargeCallbackData &callbackData, bool bIsNeedResp = true);

	// YGame��ֵ����ص�
	static void OnNetRechargeCallbackForYGame(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nAmount, const char *pszPaymentId);

	// YGame����֪ͨ
	static void OnNetYGameCashbackNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, int nYCoin, DWORD dwDiamond);

	// YGame����֪ͨ
	static void OnNetYGameBonusNotify(DWORD dwGuid, WORD wServerId, DWORD dwRoleId, DWORD dwDiamond);

	// ��ֵ����ص�
	bool OnNetRechargeCallback(const RechargeCallbackData &callbackData);

	// ��ֵ�ۿ۳��λص�(��ͨ���߼�)
	bool OnNetSuperDiscountCallback(const RechargeCallbackData &callbackData);

	// �³�ֵ�ۿ۳��λص�(��ͨ���߼�)
	bool OnNetNewSuperDiscountCallback(const RechargeCallbackData &callbackData);

	// ��ͨ��ֵ�ص�
	bool OnNetCommonRechargeCallback(const RechargeCallbackData &callbackData);

	// �¿�����ص�
	bool OnNetMonthCardBuyCallback(const RechargeCallbackData &callbackData);

	// ֱ���Żݻص�(��ͨ���߼�)
	bool OnNetDirectBuyCallback(const RechargeCallbackData &callbackData);

	// �����Ƽ��ص�(��ͨ���߼�)
	bool OnNetTodayRecommendCallback(const RechargeCallbackData &callbackData);

	// ��׼�Ƽ��ص�(��ͨ���߼�)
	bool OnNetAccurateRecommendCallback(const RechargeCallbackData &callbackData);

	// ������Ʒ(��ͨ���߼�)
	bool OnNetHotSaleGoodsCallback(const RechargeCallbackData &callbackData);

	// �����ֵ�ص�
	bool DoRechargeCallback(const RechargeCallbackData &callbackData, bool bAddDiamond = false);

	// ���ͳ�ֵ����
	void SendRechargeData();

	// �����¿�����
	void SendMonthCardData();

	// ��ӳ�ֵ����
	void AddRechargeData(WORD wRechargeId);

	// �Ƿ��׳�
	//bool IsFirstRecharge(WORD wRechargeId);

	// ����¿�����
	void AddMonthCardData(const MonthCard *pMonthCard);

	// ȡ��ֵ����
	WORD GetRechargeTimes(WORD wRechargeId);

	// �Ƿ��к��
	bool IsHaveRedPoint(WORD wSystemId);

	// ɾ�������¿�
	void DeleteExpiredMonthCard();

	// �Ƿ񶩵���¼
	bool IsOrderRecord(DWORD dwOrderId);

	// ��Ӷ�����¼
	void AddOrderRecord(DWORD dwOrderId);

	// �洢��ֵ��¼
	void SaveRechargeRecord(WORD wRechargeId, WORD wRechargeType, int nAmount, int nRechargeDiamond, const char *pszPayTime, const char *pszSDKOrderId, WORD wFrom);

	// ���AppleIapID
	bool CheckAppleIapId(const RechargeCallbackData &callbackData);

	// ����ֵ���
	bool CheckRechargeResult(int nCfgAmount, const RechargeCallbackData &callbackData);

	// �Ƿ�����׳�
	bool IsTodayFirstRecharge();

	// �Ƿ��׳�
	bool IsFirstRecharge();

	// Խ�ϰ汾ʹ��
protected:
	// �¿�����ص�
	bool DoMonthCard(WORD wCardId);

	// ��ͨ��ֵ
	bool DoCommonRecharge(const Recharge_Config *pRechargeCfg);

	// ��ֵ�ۿ۳��λص�(��ͨ���߼�)
	bool DoNetSuperDiscount(const Recharge_Config *pRechargeCfg);

	// �³�ֵ�ۿ۳��λص�
	bool DoNetNewSuperDiscount(const Recharge_Config *pRechargeCfg);

	// ֱ���Żݻص�
	bool DoDirectBuy(const Recharge_Config *pRechargeCfg, WORD wGoodsId);

	// �����Ƽ��ص�
	bool DoTodayRecommend(const Recharge_Config *pRechargeCfg);

	// ��׼�Ƽ��ص�
	bool DoAccurateRecommend(const Recharge_Config *pRechargeCfg, WORD wGoodsId);

	// ������Ʒ
	bool DoHotSaleGoods(const Recharge_Config *pRechargeCfg, WORD wGoodsId);



	// �����ֵ�ص�
	bool DoRechargeCallbackEx(const Recharge_Config *pRechargeCfg);

protected:

	// ȡ�¿�����
	MonthCard* GetMonthCard(WORD wMonthCardId);

	// ������ͨ��ֵ����
	CommonRecharge* FindCommonRecharge(WORD wRechargeId);

	// ��ȡ��ֵ���
	DWORD GetRechargeAmount(WORD wRechargeId, BYTE byIsMyCard = false);

	// ��ȡ��ֵ���
	DWORD GetRechargeDiamond(WORD wRechargeId, BYTE byIsMyCard = false);
private:

	Player *m_pPlayer;
	RoleRechargeData *m_pRechargeData;
	DWORD m_dwSeriesId; // ϵ��ID
	char m_szOrderId[MAX_ORDER_ID_LEN];

	// ���һ�����󶩵�ʱ��
	DWORD m_dwLastRequestOrderTime;

	DWORD m_dwContext;
};