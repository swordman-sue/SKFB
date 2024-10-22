/**\file
װ��������
\author Kevin
\date 2016/6/29
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CEquipmentManager
{
public:
	CEquipmentManager();
	~CEquipmentManager();

	void Init(Player *pPlayer);

public:
	// ǿ�� 
	void OnEnhance(BYTE byStrengType, DWORD dwEquipmentUID);

	// һ��ǿ��
	void OnNetOneKeyEnhance(BYTE bySlot);

	// װ������
	void OnRefineEx(DWORD dwEquipmentUID, WORD wItemId, WORD wItemNum);

	// װ������
	void OnRefine(DWORD dwEquipmentUID, EquipmentRefineCostData *pCostList, WORD wCostListNum);

	// װ���ϳ�
	void OnCompose(DWORD dwEquipmentInfoId);

	// ����װ���б�
	void OnEquipmentList();

	// ����װ������
	void OnEquipmentInfo(DWORD dwUID);

	// ����װ��
	void OnEquipmentPutOn(BYTE byFormationPos, BYTE byEquipmentPos, DWORD dwUID);

	// ����
	void OnNetStarUP(DWORD dwUID);

	// ��Ʒ
	void OnNetQualityUP(DWORD dwUID);

public:
	// ���װ��
	bool AddEquipent(DWORD dwEquipmentInfoId, bool bNotify = true, WORD wFromId = 0);

	// ���װ��
	bool AddEquipment(const Equipment &sEquipment, bool bNotify = true, WORD wFromId = 0);

	// ɾ��װ��
	bool RemoveEquipment(DWORD dwEquipmentUID, bool bNotify = true, WORD wFromId = 0);

	// ȡװ��������������
	WORD GetBagEmptySize();

	// ��Ӿ���������Ʒ
	bool AddRefineCostItem(DWORD dwEquipmentUID, DWORD dwItemId, DWORD dwItemNum = 1);

	// ȡװ������
	const Equipment* GetEquipment(DWORD dwUID);

	// װ������֪ͨ
	void EquipmentUpdateNotify(DWORD dwUID, BYTE byFlag = true);

	// �ṩ������ģ��ʹ��
public:
	// ȡ����ָ��ǿ���ȼ���װ������
	WORD GetEquipmentNumByStrengthenLevel(WORD wStrenthenLevel);

	// ȡ����ָ�������ȼ���װ������
	WORD GetEquipmentNumByRefineLevel(WORD wRefineLevel);

	// ȡ����ָ��Ʒ�ʵ�װ������
	WORD GetEquipmentNumByQuality(WORD wQuality);

	// ȡǿ����С�ȼ�(��װ����)
	// ȡ��С��ǿ���ȼ�(��װ����)
	WORD GetEquipmentEnhanceMinLevel();

	// ȡװ�����Ʒ��(��װ����) 
	WORD GetEquipmentLowQuality();

	// ȡ�¸�Ʒ�ʵ�ǿ���ȼ�
	WORD GetNextQualityEnhanceLevel(WORD wCurQuality, WORD wCurEnhanceLevel);

	// ȡ�¸�Ʒ�ʵľ����ȼ�
	WORD GetNextQualityRefineLevel(WORD wCurQuality, WORD wCurRefineLevel, DWORD &dwCurExp);

protected:
	// ����ǿ��
	bool DoEnhance(Equipment *pEquipment, bool &bIsCrit, int &nCostGold);

	// ȡװ��
	Equipment* FindEquipment(DWORD dwUID);

	// ȡһ���µ�ϵ��id
	DWORD GetNewSeriesId();
public:
	DWORD m_dwSeriesId; // ϵ��ID
	Player *m_pPlayer;
	PlayerEquipmentInfo *m_pEquipmentInfo;
};