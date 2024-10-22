/**\file
����ս������
\author Kevin
\date 2018/9/4
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "Guild.h"
#include "ToolKit\Timer.h"
#include "DBCDefine.h"
class Player;



class CGuildWarMgr : public Singleton<CGuildWarMgr>
{
public:
	CGuildWarMgr();
	~CGuildWarMgr();

	void Init();
	void Release();

	void Update();

	void OnNewDay();

	// �����������
	void LoadDataRequest();

	// �洢���ݵ����ݿ�
	void SaveDataToDB();

	// ��������
	void LoadDataFromDB(const char *pszData, int nSize);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// ռ������
	void OnOccupyStarForGM(Player *pPlayer, WORD wStarId);

public:

	// ս����ʼ(�ͼ�����)
	bool OnNetLowStarBattleStart(Player *pPlayer, DWORD dwStarId);

	// ս������(�ͼ�����) 
	bool OnNetLowStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate);

	// ս����ʼ(�߼�����) 
	bool OnNetHighStarBattleStart(Player *pPlayer, DWORD dwStarId, WORD wTowerId, DWORD dwTargetId);

	// ս������(�߼�����)
	bool OnNetHighStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate);

public:

	// ���󹤻�ս����
	void OnNetGuildWarData(Player *pPlayer);

	// ���󱾾�����ռ�������
	void OnNetGuildOccupyStarList(Player *pPlayer);

	// ���󱾾�������ս�������б�
	void OnNetGuildDeclareWarStarList(Player *pPlayer);

	// ���������б�
	void OnNetStarList(Player *pPlayer);

	// ��ս
	void OnNetDeclareWar(Player *pPlayer, DWORD dwStarId);

	// ����
	void OnNetOrganizeDefence(Player *pPlayer, WORD wStarId, WORD wTowerType, DWORD dwMemberId, BYTE byIsToBattle);

	// ����
	void OnNetEvacuate(Player *pPlayer, WORD wStarId);

	// ����Ը߼�������ս����
	void OnNetBuyHighStarBattleTimes(Player *pPlayer);

	// ��ȡ������(����Exploit)
	void OnNetTakeStarReward(Player *pPlayer, WORD wTakeType, WORD *pStarList, WORD wStarNum);

	// ����������ϸ
	void OnNetStarDetail(Player *pPlayer, WORD wStarId);

	// �������б�
	void OnNetTowerList(Player *pPlayer, DWORD dwStarId);

public:
	// ��ɢ�����¼�
	void OnDismissGuild(WORD wServerId, DWORD dwGuildId);

	// �˳������¼�
	void OnLeave(CGuild *pGuild, DWORD dwRoleId);

public:

	// ȡ�����Ѿ���ս������
	WORD GetAlreadyDeclareWarNum(WORD wRegionId, DWORD dwGuildId);

	// ȡռ��߼����������
	WORD GetOccupyHighStarNum(WORD wServerId, DWORD dwGuildId);

	// ȡ����ID
	WORD GetRegionId(WORD wServerId);

	// ����Ƿ񹤻������
	bool IsGuildStar(DWORD dwGuildId, DWORD dwStarId);

	// �Ƿ�ͼ�������սʱ��
	bool IsLowStarBattleTime();

	// �Ƿ�ͼ�������ʱ��
	bool IsLowStarRewardTime();

	// ���߼�����״̬
	bool CheckHighStarActivityStatus(int nActivityStatus);

	// ������Ƿ�����
	bool IsTowerBreak(DWORD dwBelongGuildId, const StarTowerInfo *pTowerInfo);

	// ȡ��������
	WORD GetBreakTowerNum(const StarInfo *pStarInfo);

	// �Ƿ�ɹ�������
	bool IsCanAttackBoss(const StarInfo *pStarInfo);

	// �Ƿ�ͼ�����
	bool IsLowStar(WORD wStarType);

	// �Ƿ�߼�����
	bool IsHighStar(WORD wStarType);

	// �Ƿ����ڷ�����
	bool IsInDefense(const StarInfo *pStarInfo, DWORD dwRoleId);

	// ȡ������
	WORD GetTowerType(WORD wTowerId);

	// ȡ��������ǰ��������
	WORD GetCurTowerDefenseNum(const StarInfo *pStarInfo, WORD wTowerType);

protected:

	// ��ӹ������
	void AddGuildScore(StarInfo &tStarInfo, DWORD dwGuildId, DWORD dwScore);

	// ȡ������Ϣ(���Ŀǰû����ᴴ��)
	StarInfo& GetStarInfo(WORD wServerId, DWORD dwStarId);

	// ȡ������Ϣ(���Ŀǰû����ᴴ��)
	StarInfo& GetStarInfoByRegionId(WORD wRegionId, DWORD dwStarId);

	// ����������Ϣ
	StarInfo* GetStarInfoEx(WORD wServerId, DWORD dwStarId);

	// ȡ����Ϣ
	StarTowerInfo* GetTowerInfo(StarInfo *pStarInfo, WORD wTowerId);

	// ͨ������ȡ����Ϣ
	StarTowerInfo* GetTowerInfoByType(StarInfo *pStarInfo, WORD wTowerType);

	// ȡ��������Ϣ
	StarDefenderInfo* GetDefenderInfo(StarTowerInfo *pTowerInfo, DWORD dwRoleId);

	// ȡ�����������б�
	map<DWORD, StarInfo>* GetStarList(WORD wAreaId);

	// ȡ����ռ��ĵ������б�
	const map<DWORD, DWORD>* GetGuildStarList(DWORD dwGuildId);

	// ����������Ϣ
	void ResetStarInfo(StarInfo &tStarInfo, DWORD dwBelongGuildId, bool bIsNew);

	// ȡ��ҹ���ս����
	RoleGuildWarData& GetRoleGuildWarData(DWORD dwRoleId);

	// ���ռ�������
	void AddOccupyStar(DWORD dwGuildId, WORD wStarId);

	// ɾ��ռ�������
	void RemoveOccupyStar(DWORD dwGuildId, WORD wStarId);

	// ɾ����ս�Ĺ���
	void RemoveDeclareWarGuild(WORD wRegionId, DWORD dwGuildId);

	// �����������
	void AddStarInfo(WORD wRegionId, const StarInfo &tStarInfo);

	// �Ƴ�������
	void RemoveDefender(StarInfo &tStarInfo, DWORD dwRoleId);

	// �������ݸ���֪ͨ
	//void StarUpdateNotify(DWORD dwGuildId, WORD wStarId, WORD wTowerId);

protected:
	// ��������
	void BreakTowerReward(WORD wStarId, StarTowerInfo *pTowerInfo, DWORD dwAttackGuildId);

	// �߼�������������
	void HighStarActivtiyOverReward(const StarInfo *pStarInfo, BYTE byIsAttackerWin, WORD wBreakTowerNum = 0);

	// �߼�������սʱ�����(����˭��ý���Ȩ)
	void HighStarDeclareWarTimeOver();

	// �߼���������(����˭�������Ȩ)
	void HighStarActivityOver();

	// �ͼ�������ʼ(������������)
	void LowStarActivityStart();

	// �ͼ���������(����˭�������Ȩ)
	void LowStarActivityOver();


protected:
	// ���͹������ɫ����
	void SendGuildWarRoleData(Player *pPlayer);

	// ���������б�
	void SendStarList(Player *pPlayer);

	// ���͹���ռ��������б�
	void SendOccupyStarList(Player *pPlayer);

	// ���͹�����ս�������б�
	void SendDeclareWarStarList(Player *pPlayer);

	// ����
public:
	// ȡͨ������
	const Server_Config* GetCommonCfg(DWORD dwKey, bool bOutLog = false);

	DWORD GetConfigValue(DWORD dwKey, DWORD dwDefault = 0);

private:

	bool m_bIsLoadDataFromDB;
	DWORD m_dwSaveTimer;

	// �߼�������ս����ʱ���ʱ��(����������ʱ��)
	vector<CDateTimers> m_vHighStarDeclareWarOverTimer;

	// �߼���������ʱ���ʱ��(��սʱ�����ʱ��,������ʤ��ʱ��)
	vector<CDateTimers> m_vHighStarActivityOverTimer;

	// �ͼ���������ʱ���ʱ��(������ʤ��ʱ��)
	CDateTimers m_LowStarActivityOverTimer;

	///// ��Ҫ�洢���ݿ�
	
	// ������Ϣ�б�
	map<DWORD, map<DWORD, StarInfo>> m_StarInfoList; // [area_id, star_id, data]

	// ��ҹ���ս�����б�
	map<DWORD, RoleGuildWarData> m_RoleGuildWarDataList; //[RoleId, Data]

	// ������а�


	///// ����Ҫ�洢���ݿ�

	// ���������б�
	map<DWORD, map<DWORD,DWORD>> m_GuildStarList;// [guild_id, star_list[star_id, star_id]]
};