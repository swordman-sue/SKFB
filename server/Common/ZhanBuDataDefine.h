#ifndef _ZHAN_BU_DATA_DEFINE_H_
#define _ZHAN_BU_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// ��������
struct Card
{
	DWORD dwId;			// ΨһID
	DWORD dwInfoId;		// ��ϢID
	WORD wLevel;		// �ȼ�
	DWORD dwExp;		// ����
	WORD wBreakLevel;	// ͻ�Ƶȼ�
};

// �ͻ��˿�������
struct ClientCard : public Card
{
// 	DWORD dwId;			// ΨһID
// 	DWORD dwInfoId;		// ��ϢID
// 	WORD wLevel;		// �ȼ�
// 	DWORD dwExp;		// ����
// 	WORD wBreakLevel;	// ͻ�Ƶȼ�
	DWORD dwHeroId;		// ������Ӣ��ID
};

// ����ͼ��
struct FairyPokedex
{
	WORD wId;		// ͼ��Id
	BYTE byStar;	// ����
};

// ��������
struct CardGroupData
{
	DWORD dwCardId; // ����ID
	WORD wIsAlreadyOpen; // �Ƿ��ѷ���
};

// �ղ���
struct CollectionGroup
{
	WORD wId;	// �ղ���ID
	WORD wStar; // ����
};

// �ղ���
struct CollectionBook
{
	WORD wBookId;

	// �ղ����б�
	CollectionGroup arCollectionGroupList[MAX_COLLECTION_GROUP_NUM];
	WORD wCollectionGroupNum;
};

// ���ռ������
struct PlayerZhanBuInfo
{
	Card arCardList[MAX_CARD_NUM];	// �����б�
	WORD wCardNum;	// ��������

	DWORD dwHistoryCollectionValue; // ��ʷ�ղ�ֵ

	WORD wLowCardGroupMapIdx;	// �ͼ�����ؿ�����
	WORD wCurLowCardGroupId;	// ��ǰ�ͼ�����ID

	DWORD dwLowCardGroupLastRecoverTime;	// �ͼ���������ָ�ʱ��
	WORD wCurLowCardGroupHaveRefreshTimes;	// ��ǰ�ͼ�����ӵ�е�ˢ�´���

	WORD wTodayLowCardGroupOpenCardTimes;		// ���յͼ����鷭������
	WORD wTodayLowCardGroupCostRefreshTimes;	// ���յͼ������Ѿ�����ˢ�µĴ���(��Ҫ���ĵ�ˢ�´���)
	WORD wTodayHighCardGroupCostRefreshTimes;	// ���ո߼������Ѿ�����ˢ�µĴ���(��Ҫ���ĵ�ˢ�´���)

	DWORD dwHighCardGroupLastRefreshTime;	// �߼��������ˢ��ʱ��

	// �ͼ�����
	CardGroupData arLowCardGroup[MAX_CARD_GROUP_NUM];
	WORD wLowCardGroupNum;

	// �߼�����
	CardGroupData arHighCardGroup[MAX_CARD_GROUP_NUM];
	WORD wHighCardGroupNum;

	// �ղ����б�
	CollectionBook arCollectionBookList[MAX_COLLECTION_BOOK_NUM];
	WORD wCollectionBookNum;

	WORD wAchievementLevel; // �ɾ͵ȼ�
};

#pragma pack(pop)
#endif
