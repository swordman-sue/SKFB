#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// �ؿ���ͼ����
struct MapData
{
	BYTE byStar;			// ��õ�����
	BYTE byBattleTimes;		// ��ǰ��ս����
	BYTE byResetTimes;		// �������ô���
	BYTE byIsGetAward;		// �Ƿ��Ѿ���ȡ����
};

// �½�����
struct ChapterData
{
	MapData arMapList[MAX_CHAPTER_MAP_NUM];			// �ؿ��б�
	BYTE byMapIdx;									// ��ǰ�ؿ�����(��1��ʼ��)
	BYTE arStarAwardList[CHAPTER_STAR_AWARD_MAX];	// ����������ȡ��־�б�(1=����ȡ,0=ľ����ȡ)

	WORD GetStar()
	{
		WORD wStar = 0;
		for (BYTE i = 0; i < byMapIdx; ++i)
		{
			wStar += arMapList[i].byStar;
		}
		return wStar;
	}
};

// ���߸�������
struct MainFBData
{
	ChapterData arChapterList[MAX_FUBEN_CHAPER_NUM];	// �½������б�
	WORD wChapter;	// ��ǰ�½�
	WORD wTakeChapterRewardRecord; // ��ȡ�½ڽ�����¼
};

// ��Ӣ�ؿ��о�����
struct EliteMapEnemyData
{
	WORD wMapId;
	WORD wIsDead;	// �Ƿ�������

	MonsterData arMonsterList[MAX_BATTLE_MONSTER_NUM];	// �����б�(todo::����Ч)
	BYTE byMonsterNum;	// ��������(todo::����Ч)
};

// ��Ӣ�����½�����
struct EliteFBChapter
{
	MapData arMapList[MAX_CHAPTER_MAP_NUM];			// �ؿ��б�
	BYTE byMapIdx;									// ��ǰ�ؿ�����
	BYTE arStarAwardList[CHAPTER_STAR_AWARD_MAX];	// ����������ȡ��־�б�	
	EliteMapEnemyData tEnemyData;					// �������

	WORD GetStar()
	{
		WORD wStar = 0;
		for (BYTE i = 0; i < byMapIdx; ++i)
		{
			wStar += arMapList[i].byStar;
		}
		return wStar;
	}
};

// ��Ӣ��������
struct EliteFBData
{
	EliteFBChapter arChapterList[MAX_FUBEN_CHAPER_NUM];
	WORD wChapter;	// ��ǰ�½�
	DWORD dwLastRefreshEnemyTime;	// �ϴ�ˢ�����ʱ��
};

// Ӣ�۹ؿ�����
struct HeroMapData
{
	BYTE byBattleTimes;		// ��ս����
	BYTE byIsGetAward;		// �Ƿ�����ȡ����
	//BYTE byIsBattle;		// �����Ƿ��Ѿ���ս��
};

// Ӣ�۸����½�����
struct HeroFubenChapterData
{
	HeroMapData arMapList[MAX_CHAPTER_MAP_NUM];// �ؿ��б�
	BYTE byMapIdx;		// ��ǰ�ؿ��ȼ�
	BYTE byIsExtraMapPass;	// ����ؿ��Ƿ�ͨ��
};

// Ӣ�۸�������
struct HeroFBData
{
	HeroFubenChapterData arChapterList[MAX_FUBEN_CHAPER_NUM]; // �½������б� 
	WORD wChapter;		// ��ǰ�½�
	BYTE byBattleTimes;	// ��������ս����
};

// �ճ������½�����
struct DailyChapterData
{
	WORD wChapter;			// �½�(�ؿ�����)
	short nMapIdxRecord;	// �ؿ�ͨ�ؼ�¼
	BYTE byBattleTimes;		// ��������ս����
};

// �ճ���������
struct DailyFBData
{
	DailyChapterData arChapterList[MAX_DAILY_MAP_NUM]; // ��������ս�����½�
	WORD wChapterNum; // ��������ս���½�����
};


// �ճ���������
// struct DailyFBDataEx
// {
// 	WORD arChapterList[MAX_DAILY_MAP_NUM];	// ��������ս�����½�
// 	BYTE byChapterNum;						// ��������ս���½�����
// };

// ���������½�����
struct TowerFubenChapterData
{
	BYTE arMapList[MAX_CHAPTER_MAP_NUM];// �ؿ��б�
	WORD wMapLevel;						// ��ǰ�ؿ��ȼ�
	WORD wPropertyIndex;				// ѡ������Ե�����
};

// ������������
struct TowerFBData
{
	BYTE arMapStarList[MAX_TOWER_MAP_NUM];	// �ؿ������б�
	WORD wCurMapLevel;						// ��ǰ�ؿ��ȼ�(�Ѿ������)
	WORD wMapLevelRecord;					// �ؿ��ȼ���¼
	BYTE byKeepThreeStarPassedRecord;		// ��������ͨ�ؼ�¼(�½�)

	Property arSelectProList[HERO_PRO_MAX];	// ѡ��������б�
	BYTE bySelectProNum;					// ѡ�����������

	BYTE byIsBuyStarGoods;				// �Ƿ��Ѿ�����������Ʒ
	BYTE byStarGoodsIndex;				// ������Ʒ����

	BYTE byIsNeedReset;				// �Ƿ���Ҫ����
	BYTE byTodayResetTimes;			// ���������ô���

	WORD wCurStar;					// ��ǰ����(ѡ�����Կ۵�ʣ�µ�)
	WORD wStarMaxRecord;			// ��ʷ���������¼

	WORD wEliteMapLevel;			// ��Ӣ�ؿ��ȼ�(�Ѿ������)
	BYTE byEliteMapBattleTimes;		// ��Ӣ�ؿ��Ѿ���ս�Ĵ���
	BYTE byBuyEliteMapBattleTimes;	// �Ѿ�����ľ�Ӣ�ؿ���ս����

	BYTE arRandProIdxList[3];		// ������������б�
	BYTE  byRandProNum;				// �����������
};

// �Ѿ�����
// ��ǰ�ؿ��ȼ�

// ���ѷ���Ĺؿ��б�

// ��Ҹ������� 
struct PlayerFubenInfo
{
	MainFBData tMainFubenData;		// ���߸�������
	HeroFBData tHeroFubenData;		// ������������
	DailyFBData tDailyFubenData;	// �ճ���������
	TowerFBData tTowerFubenData;	// ������������
	EliteFBData tEliteFBData;		// ��Ӣ��������
};

#pragma pack(pop)
