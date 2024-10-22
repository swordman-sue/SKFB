/**\file
����������
\author Kevin
\date 2016/7/12
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CFubenManager;
struct Map_Config;

// ����
class CFBBase
{
public:
	CFBBase();
	~CFBBase();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

protected:
	CFubenManager *m_pFBManager;
	Player *m_pPlayer;
};

// ���߸���
class CMainFB : public CFBBase
{
public:
	CMainFB();
	~CMainFB();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ʼս��
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx, DWORD dwHelpBattleTargetId);

	// ս������
	bool OnBattleOver(BYTE byIsWin, BYTE byStar);

	// ���ø�����ս����
	bool OnResetBattleTimes(WORD wChapter, BYTE byMapIdx);

	// ��ȡ��������
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// ���󸱱�����
	void OnNetFBData();

	// ���͸�������
	void SendFBData();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ɨ��
	void OnNetClean(WORD wChapter, BYTE byMapIdx);

	// һ����ȡ����(�ؿ�+����)
	bool OnNetOneKeyTakeReward(WORD wChapter);

	// ��ȡ�½ڽ���
	void OnNetTakeChapterReward(WORD wRewardLevel);

public:
	// ȡ������
	WORD GetTotalStar();

protected:
	// ȡ�ؿ�����
	MapData* GetMapData(WORD wChapter, BYTE byMapIdx);

	// ȡ�½�����
	ChapterData* GetChapterData(WORD wChapter);

	// ȡ��һ����ս���½�
	WORD GetNextChapter();

	// ȡ��һ����ս�Ĺؿ�����
	BYTE GetNextMapIndex();



private:
	MainFBData *m_pFBData;	// ���߸�������
};


// ��Ӣ����
class CEliteFB : public CFBBase
{
public:
	CEliteFB();
	~CEliteFB();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ʼս��
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// ս������
	bool OnBattleOver(BYTE byIsWin, BYTE byStar, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ���ø�����ս����
	bool OnResetBattleTimes(WORD wChapter, BYTE byMapIdx);

	// ��ȡ��������
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// ���󸱱�����
	void OnNetFBData();

	// �������
	WORD CreateEnemy();

	// ���͸�������
	void SendFBData();

	// ��¼
	void Login();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// һ����ȡ����(�ؿ�+����)
	bool OnNetOneKeyTakeReward(WORD wChapter);

protected:

	// �������
	void CreateEnemy(WORD wChapter);

	// ȡ��ǰ�������
	BYTE GetEnemyNum();

	// ȡ�ؿ�����
	MapData* GetMapData(WORD wChapter, BYTE byMapIdx);

	// ȡ�½�����
	EliteFBChapter* GetChapterData(WORD wChapter);

	// ȡ��һ����ս���½�
	WORD GetNextChapter();

	// ȡ��һ����ս�Ĺؿ�����
	BYTE GetNextMapIndex();

	// ȡ������
	WORD GetTotalStar();

	// �Ƿ���йҵ�(todo::��ʱ�Ѳ�ʹ��)
	bool IsEnemyDead(WORD wChapter);

	// ���¹����˺�(todo::��ʱ�Ѳ�ʹ��)
	void UpateMonsterDamage(WORD wChapter, MonsterData *pMonsterList, BYTE byMonsterNum);
private:
	EliteFBData *m_pFBData;	// ��Ӣ��������

	WORD m_wNewEnemyNum;
};

// Ӣ�۸���
class CHeroFB : public CFBBase
{
public:
	CHeroFB();
	~CHeroFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ʼս��
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// ս������
	bool OnBattleOver(BYTE byIsWin, BYTE byStar);

	// ��ȡ��������
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// ����Ӣ�۸�������
	void OnHeroFBData();

	// ȡ��һ����ս���½�
	WORD GetNextChapter();

	// ȡ��һ����ս�Ĺؿ�����
	BYTE GetNextMapIndex();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ���͸�������
	void SendFBData();

protected:

	// ȡ�½�����
	HeroFubenChapterData* GetChapterData(WORD wChapter);

private:
	HeroFBData *m_pFBData;	// Ӣ�۸�������
};

// �ճ�����
class CDailyFB : public CFBBase
{
public:
	CDailyFB();
	~CDailyFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ʼս��
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// ս������
	bool OnBattleOver(BYTE byIsWin, WORD wContext, BYTE byIsPass);

	// �����ճ���������
	void OnDailyDataReq();


public:

	// �����ճ���������
	void SendDailyData();

protected:

	// ȡ�½�����
	DailyChapterData* GetChapterData(WORD wChapter);

	// ����½�����
	void AddChapterData(const DailyChapterData &tChapter);

private:
	DailyFBData *m_pFBData;
};

// ��������
class CTowerFB : public CFBBase
{
public:
	CTowerFB();
	~CTowerFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ͨս����ʼ
	bool OnNormalBattleStart(BYTE byType);

	// ��ͨս������
	bool OnNormalBattleOver(BYTE byIsWin, bool bIsClean = false);

	// ��Ӣս����ʼ
	bool OnEliteBattleStart(WORD wMapLevel);

	// ��Ӣս����ʼ
	bool OnEliteBattleOver(BYTE byIsWin);

	// ������ս����
	void OnNetBattleTimesBuy();

	// ���ø���
	bool OnNetFBReset(bool bIsGM);

	// ѡ������
	void OnNetSelectPro(BYTE byStar, BYTE byProIndex);

	// ����������Ʒ
	void OnNetBuyStarGoods();

	// ����������ͨ��������
	void OnTowerFBData();

	// һ��ɨ��
	void OnNetOneKeyClean(bool bIsToHistoryMap);

public:

	// ������������
	void SendTowerData();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ȡ��ʷ����
	WORD GetMaxStarRecord();

protected:
	// ȡ����������
	WORD GetTotalStar();

	// �������
	BYTE RandProIdx(BYTE byStar);
private:
	TowerFBData *m_pFBData;
};

// �Ѿ�����
class CRebelFB : public CFBBase
{
public:
	CRebelFB(){};
	~CRebelFB(){};
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// ��ʼս��
	bool OnBattleStart(DWORD dwRebelId, WORD wType);

	// ս������
	bool OnBattleOver(BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

protected:

private:
	PlayerRebelData *m_pFBData;
};

class CFubenManager
{
public:
	CFubenManager();
	~CFubenManager();

	void Init(Player *pPlayer);

	void OnNewDay();
public:
	// ��ʼս��
	void OnBattleStart(BYTE byFBType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, DWORD dwPower = 0, const char *pSign = NULL);

	// ս������
	void OnBattleOver(BYTE byFBType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1, MonsterData *pMonsterList, BYTE byMonsterNum,
		DWORD dwDamage, DWORD dwPower = 0,  const char *pSign = NULL);

	// ��ȡ��������
	void OnNetTakeReward(BYTE byAwardType, BYTE byFBType, WORD wChapter, BYTE byIndex);

	// ����ɨ��
	///void OnNetFBClean(BYTE byFBType, BYTE byTimes, DWORD dwParam1, DWORD dwParam2);

	// ɨ��
	void OnNetSweep(BYTE byFBType, WORD wChapter, BYTE byMapIdx);

	// ��������
	void OnNetFBReset(BYTE byFBType, DWORD dwParam1, DWORD dwParam2);

	// һ����ȡ
	void OnNetOneKeyTakeReward(BYTE byFBType, WORD wChapter);
public:

	// ��ӵ������
	bool AddDropObject(DWORD dwMonsterId, DWORD dwDropObjectId, WORD wDropObjectNum);

	// ��ӵ����������
	bool AddDropOjectToPlayer(WORD wMapType);

	// ������������Ʒ
	bool MonsterDropItem(WORD wMapId, WORD wMapType);

	// ȡ������Ʒ�б�
	vector<MonsterDropData>& GetDropItemList() { return m_DropItemList; };

	// ������ʱ����
	void DoLimitDrop(DWORD dwFBType, vector<Item> &vRewardItemList);

	// ��ӹ�������
	void AddMonsterData(const MonsterData &tMonster);

	// ������սĿ������ 
	void SetBattleTargetData(const vector<ToBattleHeroData> &vTargetHeroList, const vector<SpiritData> &vTargetSpriteList);

	// ����Ƿ���ս����
	bool CheckIsInBattle();

	// ����ս����Ϣ
	void SetBattleInfo(WORD wChapter, BYTE byMapLv, DWORD wMapId = 0, DWORD dwHelpBattleTargetId = 0, DWORD dwHelpBattleHeroInfoId = 0);

	// ����ս������
	void SetBattleType(BYTE byType) { m_byFBType = byType; };
	BYTE GetBattleType() { return m_byFBType; };

	// ȡ��ʼս��ʱ��
	UINT64 GetStartBattleTime() { return m_lStartBattleTime; };

	// ����ս����Ϣ
	void ResetBattleInfo();

	// ȡ��ǰ��ս���½�
	WORD GetCurChapter(){ return m_wCurChapter; };

	// ȡ��ǰ��ս�Ĺؿ�����
	BYTE GetMapIdx() { return m_byCurMapIdx; };

	// ȡ��ǰ��ս�Ĺؿ�ID
	DWORD GetMapId() { return m_dwMapId; };

	void SetMapId(DWORD dwMapId) { m_dwMapId = dwMapId; };

	// ȡ��սĿ��ID
	DWORD GetHelpBattleTargetId() { return m_dwHelpBattleTargetId; };
	DWORD GetHelpBattleHeroInfoId() { return m_dwHelpBattleHeroInfoId; };

	void SetTargetId(DWORD dwTargetId) { m_dwTargetId = dwTargetId; };
	DWORD GetTargetId() { return m_dwTargetId; };

	void SetTargetServerId(DWORD dwTargetServerId) { m_dwTargetServerId = dwTargetServerId; };
	DWORD GetTargetServerId() { return m_dwTargetServerId; };

	void SetTargetFightingPower(DWORD dwFightingPower) { m_dwTargetFightingPower = dwFightingPower; };
	DWORD GetTargetFightingPower() { return m_dwTargetFightingPower; };

	void SetResult(BYTE byResult) { m_byResult = byResult; };

	// �����Ƿ��״λ�ʤ
	void SetIsFirstWin(BYTE byIsFirstWin){ m_byIsFirstWin = byIsFirstWin; };

	// �����Ƿ���ͨ��
	void SetIsPassed(BYTE byIsPassed) { m_byIsPassed = byIsPassed; };

	// �����Ƿ��һ�ɱ
	void SetIsMeKill(BYTE byIsMeKill) { m_byIsMeKill = byIsMeKill; };

	void SetParam(BYTE byIndex, DWORD dwParam);
	DWORD GetParam(BYTE byIndex);

	// �����ʤ����
	void DoWinReward(int nMapType, const Map_Config *pMapCfg);

	// �������Ӣ������
	void AddHelpBattleHeroPro(const Property &tPro);
public:
	CTowerFB& GetTower() { return m_TowerFB; }; // ȡ���������߼���
	CDailyFB& GetDaily() { return m_DailyFB; }; // ȡ�ճ������߼���
	CHeroFB& GetHeroFB() { return m_HeroFB; }; // Ӣ�۸����߼���
	CEliteFB& GetEliteFB() { return m_EliteFB; }; // ��Ӣ�����߼���
	CMainFB& GetMainFB() { return m_MainFB; };

private:
	Player *m_pPlayer;
	PlayerFubenInfo *m_pFubenInfo;

	UINT64 m_lStartBattleTime;	// ��ʼս��ʱ��
	BYTE m_byFBType;			// ��ǰ��ս�ĸ�������
	WORD m_wCurChapter;			// ��ǰ��ս���½�
	BYTE m_byCurMapIdx;			// ��ǰ��ս�Ĺؿ�����
	DWORD m_dwMapId;			// ��ǰ�ؿ�ID
	DWORD m_dwTargetId;			// Ŀ��ID
	WORD m_dwTargetServerId;	// Ŀ�����ڵķ�����ID
	DWORD m_arParamList[MAX_BATTLE_PARAM_NUM];	// �����б�
	BYTE m_byResult;			// ���
	BYTE m_byIsFirstWin;		// �Ƿ��һ�λ�ʤ(�ؿ�)
	BYTE m_byIsPassed;			// �Ƿ���ͨ��(Ŀǰ����Ч)
	BYTE m_byIsMeKill;			// �Ƿ��һ�ɱ
	DWORD m_dwTargetFightingPower;	// Ŀ��ս��
	DWORD m_dwHelpBattleTargetId;	// ��սĿ��ID
	DWORD m_dwHelpBattleHeroInfoId;	// ��սӢ����ϢID

	vector<MonsterDropData> m_DropItemList; // �����б�
	vector<MonsterData> m_MonsterList;		// �����б�
	vector<ToBattleHeroData> m_TargetHeroList;	// Ŀ������Ӣ���б�
	vector<SpiritData> m_vTargetSpriteList;		// Ŀ�꾫���б�
	vector<Property> m_vHelpBattleHeroProList; // ����Ӣ�������б�

	// ��ʤ���������б� m_vFirstWinMoneyList
	// ��ʤ������Ʒ�б� m_vFirstWinItemList;
	// ��ͨ��������Ǯ�� m_vNormalMoneyList;
	// ��ͨ������Ʒ�б� m_vNormalItemList;
	// ���佱����Ʒ�б� m_vDropItemList;
	// ���佱����Ʒ�б� m_vBoxItemList;

	CMainFB		m_MainFB;
	CHeroFB		m_HeroFB;
	CDailyFB	m_DailyFB;
	CTowerFB	m_TowerFB;
	CRebelFB	m_RebelFB;
	CEliteFB	m_EliteFB;
};