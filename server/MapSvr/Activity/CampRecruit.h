/**\file
��Ӫ��ļ
\author Kevin
\date 2018/3/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CCampRecruit
{
public:
	CCampRecruit(){};
	~CCampRecruit(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// ������ļ����
	void OnNetRecruitData();

	// ��ļ����
	void OnNetRecruit(WORD wRecuitType, WORD wCamp);

	// ��������������
	void OnNetScoreTaskReward(WORD wCamp, WORD wRewardIdx);

	// ��ļ��¼����
	void OnNetRecruitRecord(WORD wCamp);

public:
	// ������ļ����
	void SendRecruitData();

	// ������ļ����
	void UpdateRecruitData();

	// ȡ��ǰ�����ID
	WORD GetCurDataId();

	// �����Ӫ��ļ����
	void AddCampRecruitScore(WORD wCamp, DWORD dwScore);

	// ȡ��Ӫ��ļ����
	DWORD GetCampRecruitScore(WORD wCamp);

	// �Ƿ������������¼
	bool IsScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx);

	// ��ӻ�����������¼
	void AddScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx);

	// �����ļ��¼
	void AddRecruitRecord(WORD wCamp, const Item &tItem);

private:
	Player *m_pPlayer;
	PlayerCampRecruitData *m_pCampRecruitData;
	CActivityManager *m_pActivityMgr;
};