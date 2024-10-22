#ifndef _FRIEND_DATA_DEFINE_H_
#define _FRIEND_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// ��������
struct FriendData
{
	DWORD dwFriendId;			// ����ID
	WORD wIsGiveEnergy;			// �Ƿ������;���(�����ͽ������Է��ı�־)
	WORD wTakeEnergyState;		// ��ȡ����״̬(0=������ȡ,1=����ȡ,2=����ȡ) (����ȡ�Է�������״̬)
	WORD wHelpBattleTimes;		// ������ս����		
};


// ���ѿͻ�������
struct ClientFriendData : public PlayerCommonData
{
	BYTE byIsGiveEnergy;					// �Ƿ������;���
	BYTE byTakeEnergyState;					// ��ȡ����״̬(0=������ȡ,1=����ȡ,2=����ȡ)
	//char szAccount[MAX_ACCOUNT_LEN];		// �ʺ�
};

// ��Һ�������
struct PlayerFriendData
{
	FriendData arFriendList[MAX_FRIEND_NUM];	// �����б�
	WORD wFriendNum;							// ��������

	DWORD arBlackList[MAX_BLACK_NUM];			// �������б�
	WORD wBlackNum;								// ����������

	DWORD arApplyList[MAX_FRIEND_APPLY_NUM];	// �����б�
	WORD wApplyNum;							// �����б�����

	WORD wGetEnergyTimes;						// ��ȡ��������
};

#pragma pack(pop)
#endif
