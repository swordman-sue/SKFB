/**\file
���ѹ�����
\author Kevin
\date 2016/8/30
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CMainFB;

class CFriendManager
{
	friend class CMainFB;

public:
	CFriendManager();
	~CFriendManager();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// ��������б�
	void OnFriendList();										

	// ����������б�
	void OnBlackList();										

	// ���������б�
	void OnApplyList();										

	// �����Ƽ��б�
	void OnRecommendList();									

	// ��Ӻ���
	void OnFriendAdd(DWORD dwTargetId);							

	// ɾ������(�����߼�)
	void OnFriendDel(DWORD dwTargetId);							

	// ������ѻظ�(�����߼�)
	void OnFriendApplyReply(DWORD dwTargetId, BYTE byIsAccept);	

	// ��Ӻ�����(�����߼�)
	void OnBlackAdd(DWORD dwTargetId);							

	// ɾ��������
	void OnBlackDel(DWORD dwTargetId);							

	// ���;���
	void OnEnergyGive(DWORD dwTargetId);						

	// ��ȡ����
	void OnTakeEnergy(DWORD *pTargetList, BYTE byTargetNum);

	// ����Ŀ��
	void OnNetTargetFind(const char *pszTargetName);

public:
	// ��Ӻ�����������
	bool AddApplyData(DWORD dwTargetId);

	// �Ƿ��������б�
	bool IsInApplyList(DWORD dwRoleId);

	// ������;�������
	void AddGiveEnergyData(DWORD dwRoleId);

	// ���þ�����ȡ״̬
	void SetGetEnergyState(DWORD dwRoleId, BYTE byState);

	// ɾ������
	void RemoveFriend(DWORD dwRoleId);

	// ɾ�������������
	void RemoveApplyData(DWORD dwRoleId);

	// ��Ӻ�������
	void AddFriendData(DWORD dwFriendId);

	// �Ƿ�����б�����
	bool IsFriendListFull();

	// ���º������ݻص�
	void UpdateFriendDataCallback(BYTE byType, const FriendData &tFriendData);

	// �������ݸ���֪ͨ
	void FriendDataUpdateNotify(DWORD dwFriendRoleId, BYTE byType = 1);

	// �������ݸ���֪ͨ
	void FriendDataUpdateNotify(const FriendData &tFriendData, BYTE byType = 1);

	// �Ƿ����
	bool IsFriend(DWORD dwTargetId);

	// �Ƿ������
	bool IsBlack(DWORD dwTargetId);

	// ȡ�����б�
	const FriendData* GetFrindList() { return m_pFriendInfo->arFriendList; };

	// ȡ��������
	WORD GetFriendNum() { return m_pFriendInfo->wFriendNum; };

	// ȡ������������
	BYTE GetFriendNumMax();

	// ȡ������������
	static BYTE GetFriendNumMax(WORD wRoleLevel);

	// ��������
	bool UpdateClientFriendData(ClientFriendData &tFriendData, DWORD dwFriendId);

	// ����������º�������
	void UpateFriendDataInDB(DWORD dwUpateRoleId, const FriendData &tFriendData, BYTE byOperateType);

	// ���ͺ������б�
	void SendBlackList();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ȡ��������(todo::�ӿ������պ����)
	const FriendData* GetFriendDataEx(DWORD dwRoleId);
protected:

	// ȡ��������
	FriendData* GetFriendData(DWORD dwRoleId);

	// ɾ������������
	void RemoveBlackData(DWORD dwRoleId);

	// ��Ӻ�����
	void AddBlackData(DWORD dwTargetId);
private:
	Player *m_pPlayer;
	PlayerFriendData *m_pFriendInfo;
};