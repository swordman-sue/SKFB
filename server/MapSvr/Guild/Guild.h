/**\file
�����߼���
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
#include "data_define.pb.h"
class Player;
class CGuildManager;

// ���Ȫˮ(���)��Ϣ
struct LastQYInfo
{
	DWORD dwRoleId;	// Ȫˮ���ID
	DWORD dwTime;	// Ȫˮʱ��
	DWORD dwValue;	// Ȫˮֵ
	BYTE byQSType;	// Ȫˮ����
};

class CGuild
{
	friend class CGuildManager;
public:
	CGuild();
	~CGuild();

	bool Init(Player *pCreater, CGuildManager *pGuildMgr, WORD wIconId, const char *pszName);
	void Release();

	void OnNewDay();

	void OnNewWeek();

	// �洢���ݵ����ݿ�
	void SaveDataToDB(bool isDelete = false);

	// �����ݿ��������
	void LoadDataFromDB(CGuildManager *pGuildMgr, PB::GuildInfo *pPBGuildInfo);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer);

	// �Ƿ��к��
	bool IsHaveRedPoint(Player *pPlayer, const GuildMember *pMember, BYTE byType);

	// ��������
	void Copy(CGuild *pGuild);

//////////// �������
public:

	// ս����ʼ
	bool OnBattleStart(Player *pPlayer, BYTE byCamp);

	// ս������
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ��ȡ�����½ڽ���
	void OnNetCopyChapterReward(Player *pPlayer, WORD wChapter);

	// ��ȡ�����ؿ�����
	void OnNetCopyMapReward(Player *pPlayer,WORD wChapter, BYTE byCamp, BYTE byBoxIdx);

	// ����ؿ�������¼
	void OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp);

	// �����Ա����ս��
	void OnNetCopyBattleRecord(Player *pPlayer);

	// ���򸱱���ս����
	void OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

	// �����½�����
	void OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord);

	// �����½ڹؿ�����״̬
	void OnNetMyMapRewardRecord(Player *pPlayer);
public:

	// ������������
	void CreateMonster(bool bResetMemberData = true);

	// �Ƿ����������
	bool IsMonsterDead(BYTE byCamp);

	// �Ƿ�������Ӫ���ﶼ�ҵ�
	bool IsAllCampMonsterDead();

	// ���¹����˺�
	void UpdateMonsterDamage(BYTE byCamp, MonsterData *pMonsterList, BYTE byMonsterNum);

	// ��ǰ����HP
	DWORD GetMonsterCurHP(BYTE byCamp);

	// �Ƿ񸱱�����ʱ��
	bool IsFBOpenTime();

	// ��Ӹ����˺���¼
	void AddFBDamageRecord(DWORD dwRoleId, DWORD dwDamage);

public:

	// ȡ����Id
	DWORD GetGuildId() { return m_tData.dwId; };

	// ȡ�������ڷ�����ID
	DWORD GetServerId() { return m_dwSeverId; };

	// ���ù������еķ�����ID
	void SetServerId(DWORD dwServerId) { m_dwSeverId = dwServerId; };

	// ȡ��������
	const char* GetGuildName() { return m_tData.szName; };

	// ȡ�᳤����
	const char* GetCaptainName();

	// ȡ�᳤ID
	DWORD GetCaptainId();

	// ȡ����ȼ�
	WORD GetGuildLevel() { return m_tData.wLevel; };

	// ���ù���ͼ��
	void SetGuildIconId(WORD wIconId) { m_tData.wIconId = wIconId; };

	// ȡ����ͼ��
	WORD GetGuildIconId() { return  m_tData.wIconId; };

	// ��������
	void SetDeclaration(const char *pszDeclaration);

	// ���ù���
	void SetNotice(const char *pszNotice);

	// ��Ӿ���
	void AddExp(DWORD dwExp);
	
	// ���þ���
	void SetExp(DWORD dwExp) { m_tData.dwExp = dwExp; };

	// ��ӻ�Ծ
	void AddActive(DWORD dwActive) { m_tData.dwActive += dwActive; };

	// ȡ��Ծ
	DWORD GetActive() { return m_tData.dwActive; };

	// ��ӹ���ֵ
	void AddContribution(DWORD dwRoleId, int nContribution);

	// ���Ȫˮֵ
	void AddQSValue(DWORD dwValue) { m_tData.dwQuanShui += dwValue; };

	// ��������
	void Chat(NetMsgBody *pMsg);

	const GuildData& GetGuildData() { return m_tData; };

	// ��乤������
	void FillGuildData(GuildBaseData &tGuildData);

	// ���ó�Աְλ
	void SetMemberPos(DWORD dwRoleId, BYTE byPos);

	// ȡ��Աְλ
	BYTE GetMemberPos(DWORD dwRoleId);

	// ȡĳ��ְλ�ϵ�������
	BYTE GetMemberNumByPos(BYTE byPos);

	// ȡ��ԱȪӿ����
	BYTE GetMemberQYType(DWORD dwRoleId);

	// ��ӳ�ԱȪӿ����
	void SetMemberQYType(DWORD dwRoleId, BYTE byType);

	// ȡ����Ȫӿ����
	BYTE GetGuildQYTimes() { return m_tData.byQYTimes; };

	// ��ӹ���Ȫӿ�ܴ���
	void AddGuildQYTimes(BYTE byTimes = 1) { m_tData.byQYTimes += byTimes; };

	// �Ƿ�����ȡ�˱�Ȫ����
	bool IsGetBQReward(DWORD dwRoleId, WORD wQuanShui);

	// ���Ȫˮ����
	void AddQSRewardRecord(DWORD dwRoleId, WORD wQuanShui);

	// ȡȪˮ
	int GetQuanShui() { return m_tData.dwQuanShui; };

	// ȡ����
	const SkillData* GetSkillData(WORD wSkillId);

	// ��Ӽ���
	void AddSkillData(WORD wSkillId, WORD wSkillLevel);

	// ��������¼
	void AddChatRecord(const ChatRecord &tRecord);

	// ������ս���� todo:: for test
	void ResetBattleTimes(Player *pPlayer);
	void ResetQYTimes(Player *pPlayer);

	// �����г�Ա������
	void SendRewardToAllMember(const RewardData &tRewardData);

public:
	// ���͹�������
	void SendGuildData(Player *pPlayer);

	// ���͹�������
	void SendGuildData(DWORD dwRoleId);

	// ���ͳ�Ա�б�
	void SendMemberList(Player *pPlayer);

	// ����������Ϣ�б� 
	void SendLeaveMsgList(Player *pPlayer);

	// ���������б�
	void SendApplyList(Player *pPlayer);

	// �����¼��б� 
	void SendEventList(Player *pPlaye);

	// ������ʱ��Ʒ�����¼
	//void SendLimitGoodsBuyRecord(Player *pPlayer);

	// ������ʱ��Ʒ�б�
	void SendLimitGoodsList(Player *pPlayer);

	// ���ͼ����б�
	void SendSkillList(Player *pPlayer);

	// ���������¼
	void SendChatRecordList(Player *pPlayer);

	// ���͹��ḱ������
	void SendCopyData(Player *pPlayer);

	// ���͹�������
	void SendMonsterData(Player *pPlayer);

	// ������Ϣ�����г�Ա
	void SendMsgToAllMember(NetMsgBody * pMsg);

	// �㲥���Ἴ������
	void SendSkillDataToAll();
public:
	// ��ӳ�Ա
	bool AddMember(const GuildMember *pMember);
	
	// ɾ����Ա
	bool RemoveMember(DWORD dwRoleId);

	// ȡ��Ա����
	const GuildMember* GetMemberData(DWORD dwRoleId);

	// ȡ�᳤��Ϣ
	const GuildMember* GetCaptainData();

	// ȡ�����Ա����
	BYTE GetMemberNum() { return m_tData.byMemberNum; };

	// ��ɢ
	void Dismiss();

	// �����Ա�Ƿ�����
	bool IsMemberFull();

	// ��������������
	void AddApplyData(Player *pPlayer);

	// ɾ�������������
	void RemoveApplyData(DWORD dwRoleId);

	// ȡ�����������
	GuildApplyData* GetApplyData(DWORD dwRoleId);

	// ����Ƿ��������б�
	bool IsInApplyList(DWORD dwRoleId);

	// ���������Ϣ
	void AddLeaveMessage(const GuildLeaveMessage &sLeaveMsg);

	// ȡ��Ʒ�����¼
	//const ShopGoodsBuyRecord* GetGoodsBuyRecord(WORD wGoodsId);

	// �����Ʒ�����¼
	//bool AddGoodsBuyRecord(WORD wGoodsId, WORD wGoodsNum);

	// ������ʱ��Ʒ 
	void UpdateLimitGoods(vector<Goods> &vGoodsList);

	// ����¼�
	void AddEvent(const GuildEvent &tEvent);

	// ����¼�
	void AddEvent(WORD wEventId, const char *pszTargetName, DWORD dwValue = 0);

	// �������Ȫˮ��Ϣ
	void SetLastQSInfo(DWORD dwRoleId, DWORD dwValue, BYTE byType);

	// ȡ��ʱ��Ʒ
	const Goods* GetLimitGoods(WORD wGoodsId);

	// �����ʱ��Ʒ��������
	void AddLimitGoodsBuyNum(WORD wGoodsId, WORD wBuyNum = 1);

	// ȡ��ʱ��Ʒ�б�
	const vector<Goods>& GetLimitGoodsList() { return m_vLimitGoodsList; };

	// ȡ��Աս�������б�
	void GetMemeberPowerRankList(vector<RolePower> &vPlayerList);

protected:
	// ���ҳ�Ա����
	GuildMember* FindMemberData(DWORD dwRoleId);

	// ������ս����
	void UpdateBattleTimes(GuildMember *pMember);


protected:

	// Ϊ�Ϸ�׼���Ľӿڣ�Ŀǰ��ʱʹ�ò���
	const list<GuildEvent>& GetEventList() { return m_EventList; };
	const list<GuildLeaveMessage>& GetMessageList() { return m_MessageList; };
	const vector<GuildApplyData>& GetApplyData() { return m_ApplyList; };
	const vector<SkillData>& GetSkillDataList() { return m_SkillList; };
	const map<DWORD, DWORD>& GetDamageRecordList() { return m_DamageRecordList; };
	const map<WORD, GuildFBMapReward>& GetMapRewardList() { return m_MapRewardRecordList; };
	const list<ChatRecord>& GetChatRecord() { return m_ChatRecordList; };
	const LastQYInfo& GetLastQYInfo() { return m_tLastQYInfo; };

private:
	CGuildManager *m_pGuildMgr;

	DWORD m_dwSeverId;

	DWORD m_dwCreateMonterTime;	// ��������ʱ��

	// ������Ϣ
	GuildData m_tData;	

	// �����¼��б�
	list<GuildEvent> m_EventList;

	// �����б�
	list<GuildLeaveMessage> m_MessageList; 

	// ��������б�
	vector<GuildApplyData> m_ApplyList;

	// ��ʱ��Ʒ�����¼�б�
	//map<WORD, ShopGoodsBuyRecord> m_GoodsBuyRecordList;

	// ��ʱ��Ʒ�б�
	//map<WORD, Goods> m_LimitGoodsList;
	vector<Goods> m_vLimitGoodsList;

	// �����б�
	vector<SkillData> m_SkillList;

	// ���ḱ�������б�(��ǰ������ս���½ڹ���)
	vector<MonsterData> m_arMonsterList[CAMP_NUM];
	string m_KillerList[CAMP_NUM];

	// �˺���¼�б�[role_id,damage]
	map<DWORD, DWORD> m_DamageRecordList;	

	// �ؿ�������ȡ��¼[chapter,reward]
	map<WORD, GuildFBMapReward> m_MapRewardRecordList;

	// �����¼�б�
	list<ChatRecord> m_ChatRecordList;

	// ���Ȫӿ��Ϣ
	LastQYInfo m_tLastQYInfo;
};