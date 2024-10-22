/**\file
���������
\author Kevin
\date 2016/6/29
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;


class CTreasureManager
{
public:
	CTreasureManager();
	~CTreasureManager();

	void Init(Player *pPlayer);

public:
	// ����ǿ�� 
	void OnEnhance(DWORD dwTreasureUID, DWORD *pCostTreasureList, WORD wCostTreasureNum);

	// ���ﾫ��
	void OnRefine(DWORD dwTreasureUID);

	// ����ϳ�
	void OnComposeEx(DWORD dwTreasureInfoId);

	// ����ϳ�
	void OnCompose(DWORD dwTreasureInfoId, WORD wTreasureNum);

	// ���ﴩ��
	void OnPutOn(BYTE byFormationPos, BYTE byTreasurePos, DWORD dwTreasureUID);

	// ����
	void OnNetSmelt(DWORD dwTreasureInfoId);

	// ��Ʒ
	void OnNetQualityUP(DWORD dwTreasureUID);

	// �������б�
	void OnTreasureList();
public:
	// ��ӱ���
	bool AddTreasure(DWORD dwTreasureInfoId, BOOL bNotice = TRUE, WORD wFromId = 0);

	// ��ӱ���
	bool AddTreasure(const Treasure &sTreasure, BOOL bNotice = TRUE);

	// ɾ������
	bool RemoveTreasure(DWORD dwUID, WORD wFromId = 0);

	// ɾ������(ֻɾ��û��ǿ����������)
	bool RemoveTreasureByInfoId(DWORD dwInfoId, WORD wFromId);

	// ȡ���ﱳ����������
	WORD GetBagEmptySize();

	// ȡ����ǿ���ȼ���Ӧ���ܾ���
	DWORD GetStrengthLevelTotalExp(WORD wQuality, WORD wLevel);

	// ȡ��������
	const Treasure* GetTreasure(DWORD dwTreasureUID);

	// �������֪ͨ
	void TreasureUpdateNotify(DWORD dwTreasureUID, BYTE byFlag = true);

	// �������֪ͨ
	void TreasureUpdateNotify(const Treasure &sTreasure, BYTE byFlag = true);

	// ����ǿ��
	bool DoStrengthen(Treasure *pTreasure, const Treasure_Config *pTreasureCfg, DWORD dwCostTreasureUId, int &nCostGold);

	// �ṩ������ģ��ʹ��
public:
	// ȡ����ָ��Ʒ�ʵı�������
	WORD GetTreasureNumByQuality(WORD wQuality);

	// ȡ�Ѿ���������Сǿ���ȼ��Ķ���
	WORD GetMinEnhanceLevelInWear();
	
	// ȡ�Ѿ����������Ʒ�ʵı���
	WORD GetLowQualityInWear();

protected:
	// ���ұ���
	Treasure* FindTreasure(DWORD dwUID);

	// ȡһ���µ�ϵ��id
	DWORD GetNewSeriesId();

public:
	DWORD m_dwSeriesId; // ϵ��ID
	Player *m_pPlayer;
	PlayerTreasureInfo *m_pTreasureInfo;
};