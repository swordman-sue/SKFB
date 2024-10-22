#ifndef _ACHIEVEMENT_DATA_DEFINE_H_
#define _ACHIEVEMENT_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// �ɾ�����
struct AchievementData
{
	WORD wId;			// �ɾ�ID
	BYTE byLv;			// �ɾ͵ȼ�	
	DWORD dwValue;		// �ɾ�ֵ
	BYTE byCurRewardLevel;	// ��ǰ�����ȼ�(�Ѿ���ȡ��)
};

// ��ҳɾ�����
struct PlayerAchievementData
{
	AchievementData arAchievementList[MAX_ACHIEVEMENT_NUM];
	WORD wAchievementNum;
};

#pragma pack(pop)
#endif
