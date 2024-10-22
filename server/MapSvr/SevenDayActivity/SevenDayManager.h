/**\file
7�������
\author Kevin
\date 2016/11/19
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CSevenDayManager
{
public:
	CSevenDayManager();
	~CSevenDayManager();

	void Init(Player *pPlayer);

public:

	// ����Ŀ���б�
	void OnNetTargetList();

	// ��ȡĿ�꽱��
	void OnNetTargetAward(WORD wTargetId);

	// ��ȡ��ɶȽ��� 
	void OnNetCompletionReward(WORD wCompletionNum);

public:

	// ����Ŀ���б�
	void SendTargetList();

	// ����Ŀ������
	void SendTargetData(const SevenDayTargetData *pData);

	// ����Ŀ������
	void OnUpdateTarget(WORD wLogicType, DWORD dwX, bool bAdd);

	// ����Ŀ������
	void OnUpdateTarget(WORD wLogicType);

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// �Ƿ��к��
	bool IsHaveRedPointEx();
protected:
	// ȡ�ɾ�����
	SevenDayTargetData* GetTargetData(WORD wTargetId);

	// ��ӳɾ�����
	void AddTargetData(const SevenDayTargetData &tTargetData);

	// ȡ�����
	WORD GetActivityDay();

	// �Ƿ�ָ��������Ŀ��
	bool IsTheDayTarget(WORD wTargetId,WORD wActivityDays);

	// ����Ŀ������(Ŀ��ֵ���������)
	void DoUpdateTarget(WORD wTargetId, DWORD dwValue, bool bAdd);

	// ����Ŀ������(Ŀ��ֵ���������)
	void DoUpdateTarget(WORD wTargetId);

	// ȡ��ǰ��ɶ�
	WORD GetComplationNum();

	// �Ƿ���ɶȽ�����¼
	bool IsComplationRewardRecord(WORD wComplationValue);

	// �Ƿ�ǰ�Ŀ��
	bool IsCurActivityTarget(WORD wTargetId);
private:
	Player *m_pPlayer;
	//PlayerSevenDayData *m_pSevenDayInfo;
	PlayerActivityData *m_pActivityInfo;

};