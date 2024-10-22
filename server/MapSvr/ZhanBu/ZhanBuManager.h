/**\file
���������
\author Kevin
\date 2016/11/22
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;

// DivinationManager ZhanBu
class CZhanBuManager
{
public:
	CZhanBuManager();
	~CZhanBuManager();

	void Init(Player *pPlayer);

	// ��ʼս��
	bool OnBattleStart();

	// ս������
	bool OnBattleOver(BYTE byIsWin);

public:
	
	// �������ռ������
	void OnNetPlayerZhanBuData();

	// �������б�
	void OnNetCardList();

	// ��������
	void OnNetLevelUp(DWORD dwUID, DWORD *pCostCardList, WORD wCostCardNum);

	// ����ͻ��
	void OnNetBreak(DWORD dwUID);

	// ����ˢ��
	void OnNetCardRefresh(BYTE byRefreshType);

	// ����
	void OnNetOpenCard(BYTE byType, WORD *pIdxList, WORD wIdxListNum);

	// �ͼ�����ˢ�´�������
	void OnNetLowCardGroupRefreshTimesUpdate();

	// �ղ��鼤��
	void OnNetCellectionGroupActivate(WORD wBookId, WORD wGroupId);

	// �ղ�������
	void OnNetCellectionGroupStarUp(WORD wBookId, WORD wGroupId);

	// �ղ�������
	void OnNetCellectionGroupReset(WORD wBookId, WORD wGroupId);

	// �����ղ����б�
	void OnNetCollectionBookList();

	// װ������
	void OnNetEquipCard(WORD wFormationPos, WORD wCardSlot, DWORD dwUID);

	// ж�¿���
	void OnNetUnloadCard(WORD wFormationPos, WORD wCardSlot);

	// ����ɾ�
	void OnNetAcivateAchievement();
public:

	// ȡ����
	const Card* GetCard(DWORD dwUID);

	// ��ӿ���
	void AddCard(DWORD dwInfoId, bool bIsNotify = true, WORD wFromId = 0);

	// ��ӿ���
	void AddCard(const Card *pCard, bool bIsNotify, WORD wFromId = 0);

	// ɾ������
	void RemoveCard(DWORD dwUID, bool bIsNotify = true, WORD wFromId = 0);

	// ����ɾ������
	bool RemoveCard(DWORD *pCardUIDList, WORD wCardNum, WORD wFromId = 0);

	// ȡ��ʼ����
	const Card* GetInitCard(DWORD dwInfoId);

	// ɾ����ʼ����
	void RemoveInitCard(DWORD dwInfoId, bool bIsNotify = true);

	// �Ƿ��ʼ����
	bool IsInitCard(const Card *pCard);

	// ȡ��ʼ��������
	DWORD GetInitCardNum(DWORD dwInfoId);

	// ȡ�ղ���ʷ��¼
	DWORD GetHistoryCollectionValue();

	// ȡָ���ȼ��Ŀ�������
	int GetCardNumByLevel(WORD wLevel);

	// ȡָ���׵Ŀ�������
	int GetCardNumByBreakLevel(WORD wBreakLevel);

protected:

	// ȡ����(�ڲ��ӿ�)
	Card* GetCardInner(DWORD dwUID);

	// ȡһ���µ�ϵ��id
	DWORD GetNewSeriesId();

	// ȡ��������
	const Card* GetCard(DWORD dwFairyInfoId, WORD wMaxLevel);

	// �������ռ������
	void SendPlayerZhanBuData();

	// �����Ƿ��ѷ���
	bool IsCardAlreadyOpen(BYTE byType, WORD wIdx);

	// ���µͼ�����ˢ�´���
	void UpdateLowCardGroupRefreshTimes();

	// ȡ���Ƶ�ǰ�ܾ���
	DWORD GetCardCurTotalExp(DWORD dwUID);

	// ����ͼ�����ˢ��
	bool DoLowCardGroupRefresh(bool bIsNewRole = false);

	// ����߼�����ˢ��
	bool DoHighCardGroupRefresh(bool bIsNewRole = false);

	// ��������֪ͨ
	void CardDataNotify(DWORD dwUID, BYTE byFlag = true);

	// ��������֪ͨ
	void CardDataNotify(const Card *pCard, BYTE byFlag = true);

	// ȡ�ղ�������
	CollectionBook* GetCollectionBook(WORD wBookId);

	// ȡ�ղ�������
	CollectionGroup* GetCollectionGroup(WORD wBookId, WORD wGroupId);

	// ����ղ�������
	void AddCollectGroup(WORD wBookId, WORD wGroupId);

	// ɾ���ղ�������
	void RemoveCollectGroup(WORD wBookId, WORD wGroupId);

	// ����Ƿ��ѳ�ʼ������
	bool IsAlreadyInitData();

	// ��ʼ����
	void InitData();

	// ȡ��ǰ�ղ�ֵ
	DWORD GetCurCollectionValue();

	// ȡ������������
	WORD GetBagEmptySize();

	// ������ʷ�ղ�ֵ
	void UpdateHistoryCollectionValue();
	
public:
	DWORD m_dwSeriesId; // ϵ��ID
	Player *m_pPlayer;
	PlayerZhanBuInfo *m_pZhanBuInfo;
};