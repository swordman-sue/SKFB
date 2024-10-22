/**\file
facebook��߼���
\author Kevin
\date 2017/12/13
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CFacebookActivity
{
public:
	CFacebookActivity();
	~CFacebookActivity();

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	// ��¼
	void OnLogin();

	// �ǳ�
	void OnLogout();

public:
	// ��������
	void OnNetActivityData();

	// ��ȡ�ʺŰ󶨽���
	void OnNetTakeAccountBindingReward();

	// ��ȡ������
	void OnNetTakeShareReward();

	// ��ȡ���۽���(ǿ�м���fb�)
	void OnNetTakeCommendReward();

	// ��������
	void OnNetFriendInvite(const FacebookFriendInfo *pFriendInfo);

	// �����ٻ�
	void OnNetFriendSummon(DWORD *pFriendList, WORD wFriendNum);

	// ��ȡ�������뽱��
	void OnNetTakeFriendInviteReward(WORD wInviteNum);

	// ��ȡ�����ٻ�����
	void OnNetTakeFriendSummonReward(WORD wSummonNum);

	// �����Ѿ�����ĺ��Ѽ�¼
	void OnNetFriendInviteRecord();

public:
	// ����׼������ʱ��
	void SetReadyToCommendTime(DWORD dwTime);

	// ����׼������ʱ��
	void SetReadyToShareTime(DWORD dwTime);

protected:

	// ���ͻ����
	void SendActivityData();

	// �Ƿ�����ȡ���뽱��
	bool IsTakeInviteReward(WORD wInviteNum);

	// �Ƿ�����ȡ�ٻؽ���
	bool IsTakeSummonReward(WORD wSummonNum);

	// ������뽱����¼
	void AddInviteRewardRecord(WORD wInviteNum);

	// ����ٻؽ�����¼
	void AddSummonRewardRecord(WORD wSummonNum);

	// ����ٻؼ�¼
	void AddSummonRecord(DWORD dwFriendId);

	// ɾ���ٻؼ�¼
	void DelSummonRecord(DWORD dwFriendId);

	// ��������¼
	void AddInviteRecord(const char *pszFacebookAccount);

	// ɾ�������¼
	void DelInviteRecord(const char *pszFacebookAccount);

	// �Ƿ��ٻؼ�¼
	bool IsSummonRecord(DWORD dwFriendId);

	// �Ƿ������¼
	bool IsInviteRecord(const char *pszFacebookAccount);
	
	// ��ⱻ��������Ƿ��¼
	void CheckInviteFriendIsLogin();

	// ��ⱻ�ٻغ����Ƿ��¼
	void CheckRecallFriendIsLogin();

protected:
	Player *m_pPlayer;
	FacebookActivityData *m_pActivityData; 
};