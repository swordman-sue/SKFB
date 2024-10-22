/**\file
��ʱ��߼���
\author Kevin
\date 2017/6/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;
struct LimitActivity_Config;

class CLimitActivity
{
public:
	CLimitActivity(){};
	~CLimitActivity(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// �׳���������
	void OnNetFirstRechargeData();

	// ������������
// 	void OnNetSingleRechargeData();
// 
// 	// �۳���������
// 	void OnNetAccumulateRechargeData();
// 
// 	// ��ʱ��ļ��������
// 	void OnNetRecruitData();

	// ��ȡ�����
	void OnNetTakeActivityReward(WORD wDataId);

public:
	// ������ȡ�׳佱��
	bool DoTakeFirstRechargeReward(const LimitActivity_Config *pActivityCfg);

// 	// ������ȡ���佱��
// 	bool DoTakeSingleRechargeReward(const LimitActivity_Config *pActivityCfg);
// 
// 	// ������ȡ�۳佱��
// 	bool DoTakeAccumulateRechargeReward(const LimitActivity_Config *pActivityCfg);
// 
// 	// ������ʱ��ļ����
// 	bool DoTakeLimitRecruitReward(const LimitActivity_Config *pActivityCfg);

	// ��ӳ�ֵֵ
	void AddRechargeValue(DWORD dwValue);

// 	// ��ӳ�ֵ����
// 	void AddRechargeData(WORD wRechargeId);
// 
// 	// �����ļ����
// 	void AddRecruitTimes(WORD wRecruitTimes = 1);
// 

	// �Ƿ��к��
 	bool IsHaveRedPoint(WORD wSystemId);

	// �Ƿ�����ȡ�׳佱��
	bool IsTakeFirstRechargeReward(WORD wDataId);

	// �Ƿ�����ȡ��ʱ��ļ����
// 	bool IsTakeLimitRecruitReward(WORD wDataId);
 
 	// �Ƿ�����ȡ���۳佱��
// 	bool IsTakeAccumulateRechargeReward(WORD wDataId);
// 
// 	// �Ƿ�����ȡ�˽���
// 	bool IsTakeReward(WORD wDataId);

	// ��ӽ�����¼
	bool AddRewardRecord(WORD wType, WORD wDataId);

	// ���»����
	void UpdateActivityData(int nSystemId);

	// ȡ��ֵ����
	//WORD GetRechargeTimes(WORD wRechargeId);
protected:
	// �����׳�����
	void SendFirstRechargeData();

// 	// ���ݵ�������
// 	void SendSingleRechargeData();
// 
// 	// �����۳�����
// 	void SendAccumulateRechargeData();
// 
// 	// ������ʱ��ļ����
// 	void SendRecruitData();

	// ȡ���佱����¼
	//RecordData* FindSingleRechargeRewardRecord(WORD wDataId);

	// ������ͨ��ֵ����
	//CommonRecharge* FindCommonRecharge(WORD wRechargeId);
private:
	Player *m_pPlayer;
	LimitActivityData *m_pLimitActivityData;

	// ��ʱ�
	CActivityManager *m_pActivityMgr;   
};