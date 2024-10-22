#ifndef _DAILY_MISSION_DATA_DEFINE_H_
#define _DAILY_MISSION_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// �ճ���������
 struct DailyMissionData
 {
 	WORD wMissionId;	// ����ID
 	BYTE byIsGetAward;	// �Ƿ�����ȡ����	
 	DWORD dwValue;		// ����ֵ
 };
 
 // ����ճ���������
 struct PlayerDailyMissionData
 {
	 // ��ǰ����
	 WORD wPoint;

	 // ���ֽ�����¼�б�
	 WORD arPointAwardRecord[MAX_DAILY_MISSION_POINT_AWARD];
	 WORD wPointAwardRecordNum;

	 // �ճ������б�
	 DailyMissionData arDailyMissionList[MAX_DAILY_MISSION_NUM];
	 WORD wDailyMissionNum;	
 };

#pragma pack(pop)
#endif
