/**\file
���ջ�߼���
\author Kevin
\date 2017/6/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;
struct FestivalActivity_Config;

class CFestivalActivity
{
public:
	CFestivalActivity(){};
	~CFestivalActivity(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	void OnNewDay();

public:

	// ������
	void OnNetActivityData(WORD wActivityType);

	// ��ȡ�����
	void OnNetTakeActivityReward(WORD wActivityType, WORD wTargetId);

	// ��Ʒ�һ�
	void OnNetGoodsExchange(WORD wActivityType, WORD wTargetId, WORD wExchangeNum);

	// ������
	void OnNetRedPoint();

public:
	// ����ص�
	void OnOnceRechargeCallback(WORD wRechargeId);

	// ѭ���۳�ص�
	void OnAccumulateRechargeCallback(DWORD dwAmount);

	// ��ʱ��ļ
	void OnLimitRecruit(WORD wTimes);

	// ���۶һ�����ʯ����Ʒ��
	void OnDiamondExchange(WORD wTargetId, DWORD dwExchangeNum);

	// ��ԾĿ�� 
	void OnActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd);

	// ��ԾĿ�� 
	void OnActiveTarget(WORD wActivityType, WORD wTargetId, DWORD dwContext);

	// ���϶һ�����Ʒ����Ʒ��
	void OnItemExchange(WORD wTargetId, DWORD dwExchangeNum);

	// ���յ�½
	void OnNetFestivalLogin();

public:
	// �������¼�
	void DoOnceRechargeEvent(WORD wActivityType,WORD wRechargeId);

	// �����۳��¼�
	void DoAccumulateRechargeEvent(WORD wActivityType, DWORD dwAmount);

	// �����¼�¼�
	void DoLoginEvent(WORD wActivityType);

	// �����¼�¼�
	void DoLoginEventEx(WORD wActivityType);

	// �����ԾĿ���¼�
	void DoActiveTargetEvent(WORD wActivityType, WORD wTargetId, DWORD dwContext);

	// ����һ��¼�
	void DoExchangeEvent(WORD wActivityType, WORD wTargetId, DWORD dwExchangeNum);

public:
	// �Ƿ���ջ����
	static bool IsFestivalActivityOpen(int nActivityType, DWORD dwServerId, DWORD dwCreateRoleTime);

	// �Ƿ���ջ����
	static bool IsFestivalActivityOpen(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// ȡ�����������ʱ��(ȡ���ڻ�е�)
	static DWORD GetActivityDataResetTime(WORD wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime);

	// ȡ�����������ʱ��(ȡ���ڻ�е�)
	static DWORD GetActivityDataResetTime(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// �Ƿ��к��
	bool IsHaveRedPoint(WORD wActivityType);
protected:
	// ���Ŀ������
	void AddActivityTarget(WORD wActivityId, WORD wTargetId, DWORD dwValue = 1);

	// ȡ���ջ����
	FestivalActivity* GetFestivalActivity(WORD wActivityId);

	// ���»����
	void UpdateActivityData(WORD wActivityType);

	// ���ͻ����
	void SendActivityData(WORD wActivityType);



private:
	Player *m_pPlayer;
	PlayerFestivalActivityInfo *m_pFestivalActivityInfo;

	// ��ʱ�
	CActivityManager *m_pActivityMgr;
};