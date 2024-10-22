/**\file
С���ܵ��߼�������
\author Kevin
\date 2016/11/10
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

// ��������Ϣ
// 1.������ID 2.����ID 3.�Ƿ��ѱ���ȡ

// �����뽱����Ϣ
// 1.����ID 2.����(����,����) 3.��Чʱ�� 4.��ƷID 

class CLogicManager
{
public:
	CLogicManager();
	~CLogicManager();

	void Init(Player *pPlayer);

	void OnNewDay();

////// ��ɫ����ģ��
public:
	// �����������
	void OnNetRoleDetails(DWORD dwTargetId, BYTE byType);

	// ��������ֵ�ָ�
	void OnNetPropertyRecover(BYTE byPropertyType);

	// �滻ͷ��
	void OnNetReplaceHeadImage(DWORD dwHeadImageId);

	// ��ɫ�����޸�
	void OnNetRoleNameModify(const char *pszRoleName);

	// ��ʼ��Ϸ
	void OnNetStartGame();

	// ��������
	void OnNetGuideUpdate(int nKey, int nValue);

	// �������
	void OnNetLeaveMessage(const char *pszContact, const char *pszMessage);

	// ������սĿ���б�(todo::�ɵ����,Ŀǰ�Ѳ�ʹ��,���ȱ���)
	void OnNetHelpBattleTargetListEx();

	// ������սĿ���б�(���߼�)
	void OnNetHelpBattleTargetList();

	// ��ȡ����(todo::Ŀǰ��ʱ��ʹ��)
	void OnNetTakeActivityGiftEx(const char *pszCode);

	// ��ȡ����(todo::Ŀǰ��ʱ��ʹ��)
	void OnNetTakeActivityGift(const char *pszCode);

	// ��ȡ�����
	void OnNetTakeActivityGift_New(const char *pszCode);

	// ������ȡ����
	void DoTakeActivityGift(const char *pszCode, bool bIsExist, DWORD dwLimitTime, DWORD dwRewardId, WORD wStatus);

	// ��������
	void OnNetActivityNotice();

	// ���볡��
	void OnNetEnterScene(WORD wSceneType, DWORD dwContext);

	// �뿪����
	void OnNetLeaveScene(WORD wSceneType);

	// ����
	void OnNetGoTo(WORD wSceneType, MapPos tSrcPos, MapPos tDestPos);

	// ��Ʒ����
	void OnNetLockItem(WORD wSceneType, DWORD dwSceneItemUId);

	// ��Ʒʰȡ
	void OnNetPickUpItem(WORD wSceneType, DWORD dwSceneItemUId);

	// ������������
	void OnNetNewRoleGuideUpdate(int nKey, int nValue);


public:
	// ��ȡ�����ص�
	void OnTakeActivityGiftCallback(const char *pszCode, int nResult, int nStatus, DWORD dwRewardId, DWORD dwExpireTime, int nBatch, int nType);


// �ؿ�����
public:
	// ����ؿ������б�
	void OnNetMapCommentList(WORD wChapter);

	// �ؿ�����
	void OnNetMapComment(WORD wChapter, const char *pszText);

	// �ؿ����۵���
	void OnNetMapCommentLike(WORD wChapter, WORD wCommentId);

	// ���������б�
	void SendMapCommentList(WORD wChapter);

// ս��ͷ��(��λ)
public:
	// �����λ
	void OnNetAcivateNobility();

// Ť���齱
public:
	// Ť���齱
	void OnNetNDLottery(WORD wTimes, BYTE byIsCostItem);

	// ����Ť���齱��¼
	void OnNetNDLotteryRecordList();

public:
	void SetGuide(int nKey, int nValue);

private:
	Player *m_pPlayer;

	UINT64 m_lLastLeaveMessageTime;	// �ϴ�����ʱ��

	DWORD m_dwLastTakeActivityGiftTime;	// ��ȡ�����ʱ��
};