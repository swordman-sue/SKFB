/**\file
�ճ����������
\author Kevin
\date 2016/10/18
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CDailyMissionManager
{
public:
	CDailyMissionManager();
	~CDailyMissionManager();

	void Init(Player *pPlayer);

	// ������������
	void OnNewDay();

public:
	// ���������б�
	void OnNetMissionList();

	// ��ȡ������
	void OnNetMissionAward(WORD wMissionId);

	// ��ȡ���ֽ���
	void OnNetPointAward(WORD wPoint);

public:
	// ���������б�
	void SendMissionList();

	// ������������
	void SendMissionData(const DailyMissionData *pData);

	// ��������
	void OnUpdateMission(WORD wType, int nValue, bool bAdd);

	// �Ƿ��к��
	bool IsHaveRedPoint();

protected:
	// ȡ��������
	DailyMissionData* GetMissionData(WORD wMissionId);

	// �����������
	void AddMissionData(const DailyMissionData &tMission);

	// �����ֽ���(�Ƿ�����ȡ)
	bool CheckPointAward(WORD wPoint);

	// ��ӻ��ֽ�����¼
	void AddPointAwardRecord(WORD wPoint);

	// ��������
	void DoUpdateMission(WORD wMissionId, int nValue, bool bAdd);

private:
	Player *m_pPlayer;
	PlayerDailyMissionData *m_pDailyMissionInfo;
};