/**\file
�ɾ͹�����
\author Kevin
\date 2016/10/17
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CAchievementManager
{
public:
	CAchievementManager();
	~CAchievementManager();

	void Init(Player *pPlayer);

public:

	// ����ɾ��б�
	void OnNetAchievementList();

	// ��ȡ�ɾͽ���
	void OnNetAchievementAward(WORD wAchievementId);

public:

	// ���ͳɾ��б�
	void SendAchievementList();

	// ���ͳɾ�����
	void SendAchievementData(const AchievementData *pData);

	// ���³ɾ�
	void OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd);

	// �Ƿ��к��
	bool IsHaveRedPoint();

protected:
	// ȡ�ɾ�����
	AchievementData* GetAchievementData(WORD wAchievementId);

	// ��ӳɾ�����
	void AddAchievementData(const AchievementData &tAchievement);

private:
	Player *m_pPlayer;
	PlayerAchievementData *m_pAchievementInfo;
};