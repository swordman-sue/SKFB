/**\file
���������
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "Guild.h"
#include "ToolKit\Timer.h"
class Player;

typedef std::map<DWORD, CGuild*> GUILD_MAP; // GuildId, Guild
typedef std::map<DWORD, GUILD_MAP> ALL_GUILD_MAP; // ServerId, GuildList
typedef std::map<DWORD, DWORD> GUILD_FIND_MAP;
typedef std::map<DWORD, vector<DWORD>> GUILD_APPLY_MAP;
typedef std::map<string,DWORD> GUILD_NAME_MAP;
typedef std::map<DWORD, DWORD> GUILDID_2_SERVERID_MAP;

//#define GUILD_MAX_SERIES_ID 1000000

class CGuildManager : public Singleton<CGuildManager>
{
public:
	CGuildManager();
	~CGuildManager();

	void Init();
	void Release();

	void Update();

	void OnNewDay();

	void OnNewWeek();

	// �����������
	void LoadDataRequest();

	// ��������
	void LoadDataFromDB(const char *pszData, int nSize);

	// ȡ��������
	int GetGuildNum();

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

public:
	// ���󹤻�����
	void OnGuildData(Player *pPlayer);

	// ���󹤻��б�
	void OnGuildList(Player *pPlayer);

	// ���󹤻��Ա�б�
	void OnGuildMemberList(Player *pPlayer);

	// ���������б�
	void OnLeaveMsgList(Player *pPlayer);

	// ���������б�
	void OnGuildApplyList(Player *pPlayer);

	// ���󹤻��¼��б�
	void OnGuildEventList(Player *pPlayer);

	// ��������
	void OnCreate(Player *pPlayer, WORD wIconId, const char *pszName);

	// ��ɢ����
	void OnDismiss(Player *pPlayer);

	// ������� 
	void OnApply(Player *pPlayer, DWORD dwGuildId);

	// �������
	void OnApplyCheck(Player *pPlayer, DWORD dwTargetId, BYTE byIsAccept);

	// ȡ������
	void OnApplyCancel(Player *pPlayer, DWORD dwGuildId);

	// �߳�����
	void OnKickOut(Player *pPlayer, DWORD dwTargetId);

	// �˳�����
	void OnLeave(Player *pPlayer);

	// ����
	void OnAppoint(Player *pPlayer, DWORD dwTargetId, BYTE byPos);

	// �޸Ĺ�����Ϣ(�޸Ĺ���,�޸�����,�޸�ͼ��)
	void OnModifyGuildInfo(Player *pPlayer, BYTE byType, const char *pszText, WORD wIconId);

	// ����
	void OnLeaveMessage(Player *pPlayer, const char *pszMessage);

	// ��������
	void OnSearch(Player *pPlayer, const char *pszGuildName, DWORD dwGuildId);

	// Ȫӿ(����)
	void OnNetQY(Player *pPlayer, BYTE byType);

	// ��ȡȪˮ����
	void OnNetQSAward(Player *pPlayer, WORD wQuanShui);

	// ��������
	void OnNetSkillLevelUp(Player *pPlayer, BYTE byType, WORD wSkillId, WORD wSkillLevel);

	// ���󹤻Ἴ���б�
	void OnNetSkillList(Player *pPlayer);

	// ����
	void OnNetImpeach(Player *pPlayer);

	// ���󹤻��������
	void OnNetMonsterData(Player *pPlayer);

	// �����½�����
	void OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord);

	// ս����ʼ
	bool OnBattleStart(Player *pPlayer, BYTE byCamp);

	// ս������
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum);

	// �����½ڹؿ�����״̬
	void OnNetMyMapRewardRecord(Player *pPlayer);

	// ����
public:
	// ���󹤻ḱ������
	void OnNetGuildCopyData(Player *pPlayer);

	// ��ȡ�����½ڽ���
	void OnNetCopyChapterReward(Player *pPlayer, WORD wChapter);

	// ��ȡ�����ؿ�����
	void OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx);

	// ����ؿ�������¼
	void OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp);

	// �����Ա����ս��
	void OnNetCopyBattleRecord(Player *pPlayer);

	// ���򸱱���ս����
	void OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

public:
	// ȡ��������
	CGuild* GetGuildById(DWORD dwGuildId);

	// ͨ����������ȡ��������
	CGuild* GetGuildByName(const char *pszGuildName);
	
	// ���������Ƿ��Ѵ���
	bool IsGuildNameExist(const char *pszGuildName);

	// ͨ����ɫIDȡ��������
	CGuild* GetGuildByRoleId(DWORD dwRoleId);

	// ȡ������ڹ����ID(0=��ʾľ�й���)
	DWORD GetGuildIdByRoleId(DWORD dwRoleId);

	// ɾ����Ա
	void RemoveMember(DWORD dwRoleId);

	// ɾ������
	void RemoveGuild(DWORD dwGuildId, const char *pszGuildName);

	// �����������
	void AddApplyData(DWORD dwRoleId, DWORD dwGuildId);

	// ��Ӳ�������
	void AddFindData(DWORD dwRoleId, DWORD dwGuildId);

	// ɾ����������
	void RemoveFindData(DWORD dwRoleId);

	// ɾ��������Ĺ���ID
	void RemoveApplyGuildId(DWORD dwRoleId, DWORD dwGuildId);

	// ɾ������������Ĺ�����Ϣ
	void RemoveAllApplyData(DWORD dwRoleId);

	// ��ӹ���
	void AddGuild(DWORD dwServerId, CGuild *pGuild);

	// ȡһ�����õĹ���ID
	DWORD GetFreeGuildId(DWORD dwServerId);

	DWORD GetSeriesId() { return m_dwSeriesId; };

	void SetLimitGoodsRefreshTime(DWORD dwRefreshTime) { m_dwLimitGoodsRefreshTime = dwRefreshTime; };
	DWORD GetLimitGoodsRefreshTime() { return m_dwLimitGoodsRefreshTime; };

	// ˢ����ʱ��Ʒ(�ɰ汾��ˢ���߼�)
	void RefreshLimitGoodsEx();

	// ˢ����ʱ��Ʒ 
	void RefreshLimitGoods();

	const vector<Goods>& GetLimitGoodsList() { return m_vLimitGoodsList; };

	// �Ϸ�����
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

protected:
	// ȡ�������б�
	GUILD_MAP* GetGuildList(DWORD dwServerId);

private:
	ALL_GUILD_MAP		m_AllGuildList;				// ���й����б� ServerId, GuildList
	GUILD_FIND_MAP		m_FindList;					// ��������б� RoleId, GuildId
	GUILD_APPLY_MAP		m_ApplyList;				// ���������빤���б� RoleId, GuildIdList
	GUILD_NAME_MAP		m_NameList;					// ���������б� GuildName,GuildId
	//GUILDID_2_SERVERID_MAP m_GuildId2ServerIdList; 

	DWORD m_dwSeriesId;	// ϵ��ID todo::����ID������߼���Щ���ӣ�С��ʹ��

	// ��ʱ��Ʒ�б�
	vector<Goods> m_vLimitGoodsList;
	DWORD m_dwLimitGoodsRefreshTime;	// ��ʱ��Ʒˢ��ʱ��
	CDateTimers m_LimitShopRefreshTimer; // ��ʱ�̵�ˢ�¼�ʱ��
};