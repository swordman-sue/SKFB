#pragma once
#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 7��Ŀ������
struct SevenDayTargetData
{
	WORD wId;				// Ŀ��ID
	DWORD dwValue;			// Ŀ��ֵ
	BYTE byIsGetReward;		// �Ƿ�����ȡ����
};

// ���7������
// struct PlayerSevenDayData
// {
// 	SevenDayTargetData arTargetDataList[MAX_SEVEN_DAY_TARGET_NUM];
// 	BYTE byTargetDataNum;
// };

#pragma pack(pop)
//#endif
