/**\file
��Ʒ������
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CItemManager
{
public:
	CItemManager();
	~CItemManager();

	// ��ʼ��
	void Init(Player *pPlayer);

	// ���粿�����
public:
	// ������Ʒ�б�
	void OnNetItemList();

	// ����ʹ����Ʒ
	void OnNetUseItem(DWORD dwItemId, WORD wUseNum, DWORD dwGiftItemId);

	// ������
	void OnNetFP(BYTE byFBType);

	// ��Ʒ�ϳ�
	void OnNetCompose(WORD wItemId);

	// ��Ʒ�ֽ�
	void OnNetDecompose(WORD wItemId, WORD wItemNum);
public:

	// ȡ��Ʒ����
	DWORD GetItemNum(DWORD dwItemId);

	// ȡ��Ʒ
	const Item* GetItem(DWORD dwItemId);

	// �����Ʒ
	bool AddItem(DWORD dwItemId, DWORD dwNum = 1, BOOL bNotify = TRUE, WORD wFromId = 0);

	// �Ƴ���Ʒ(����ֵΪʵ���Ƴ�������)
	DWORD RemoveItem(DWORD dwItemId, DWORD dwNum = 1, BOOL bNotify = TRUE, WORD wFromId = 0);

	// ʹ����Ʒ
	bool UseItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> *pItemList, DWORD dwGiftItemId = 0, bool bAddToCreateList = false, WORD wFromId = 0);

	// ���÷�����ƷID
	//void SetFPItemId(DWORD dwItemId) { m_dwFPItemId = dwItemId; };

	// ���÷�����Ʒ
	void SetFPItem(Item tItem) { m_tFPItem = tItem; };

	// ʹ�������Ʒ
	static bool UseRandItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> &vItemList);

	// ɾ��������Ʒ
	void DeleteTimeOutItem();

protected:
	// ������Ʒ
	Item* FindItem(DWORD dwItemId);

	// ���ʹ����Ʒ��������Ʒ
	void AddCreateItem(DWORD dwItemId, DWORD dwItemNum = 1);

	// ���ʹ�ò����Ļ���
	void AddCreateMoney(BYTE byType, int nValue);

private:
	Player *m_pPlayer;
	PlayerItemInfo *m_pPlayerItemInfo;

	//DWORD m_dwFPItemId;	// ��ǰ���Ƶ���ƷID
	Item m_tFPItem;	// ��ǰ���Ƶ���Ʒ

	vector<Item> m_vCreateItemList;			// ʹ�ò�������Ʒ�б� 
	vector<MoneyData> m_vCreateMoneyList;	// ʹ�ò����Ļ����б� 
};