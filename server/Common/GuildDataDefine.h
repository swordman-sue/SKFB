#ifndef _GUILD_DATA_DEFINE_H_
#define _GUILD_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ������빤������
struct GuildApplyData
{
	DWORD dwRoleId;					// �����ɫID
};

// �����Ա����
struct GuildMember
{
	DWORD dwRoleId;						// ��ԱID
	BYTE byPosition;					// ְλ
	DWORD dwTotalContribution;			// �ۻ�����ֵ 
	//DWORD dwCurContribution;			// ��ǰ����ֵ(todo::�˴��ֶ��Ѳ�ʹ��) 
	DWORD dwTodayContribution;			// ���չ���ֵ
	DWORD dwLastLogoutTime;				// ���������ʱ��
	BYTE byQYType;						// Ȫӿ����(0��ʾ��ľ��Ȫӿ)
	BYTE byBattleTimes;					// ��ս��������
	BYTE byTodayBattleTimes;			// ������ս����
	BYTE byBuyBattleTimes;				// �Ѿ�����ĸ�����ս����
	DWORD dwLastBattleTimesRecoverTime;	// ��ս��������ָ�ʱ��

	// ��ȡȪˮ������¼
	WORD arQSRewardRecord[MAX_GUILD_QS_REWARD_RECORD];
	BYTE byQSRewardRecordNum;

	// ��ȡ�����½ڽ�����¼(��Ǩ�Ƶ���������)
// 	WORD arTakeFBChapterRewardRecord[MAX_GUILD_FB_CHAPTER_NUM];
// 	WORD wTakeFBChapterRewardRecordNum;

	// ����Ҫ�洢���ֶ�
	bool bHaveNewLeaveMessage;  // �Ƿ����µ�����
	bool bIsLimitShopRefresh;	// �Ƿ���ʱ�̵�ˢ��


	void Init()
	{
		memset(this, 0, sizeof(GuildMember));
	}
};

// �ͻ��˹����Ա����
struct ClientGuildMember : public GuildMember
{
	char szRoleName[MAX_ROLE_LEN];	// ��Ա����
	WORD wLevel;					// ��Ա�ȼ�
	DWORD dwFightingPower;			// ս��
	WORD wHeadIconId;				// ͷ��ID
	void Init()
	{
		memset(this, 0, sizeof(GuildMember));
	}
};

// ��������
struct GuildData
{
	DWORD dwId;										// ����ID
	char szName[MAX_GUILD_NAME_LEN];				// ��������
	WORD wIconId;									// ����ͼ��
	DWORD dwCreateTime;								// ����ʱ��
	WORD wLevel;									// ����ȼ�
	DWORD dwExp;									// ���ᾭ��
	DWORD dwQuanShui;								// Ȫˮ
	BYTE byQYTimes;									// Ȫӿ����
	char szNotice[MAX_GUILD_NOTICE_LEN];			// ����
	char szDeclaration[MAX_GUILD_DECLARATION_LEN];	// ����
	WORD wTodayStartChapter;						// ���տ�ʼ�½�
	WORD wCurPassChapter;							// ��ǰͨ�ص��½�
	WORD wHistoryChapterRecord;						// ��ʷ�½ڼ�¼
	WORD wIsFromHistoryChapter;						// �Ƿ����ʷ�½ڿ�ʼ
	DWORD dwActive;									// �����Ծ��

	GuildMember arMemberList[MAX_GUILD_MEMBER_NUM];		// ��Ա�б�
	BYTE byMemberNum;									// ��Ա����

	void Init()
	{
		memset(this, 0, sizeof(GuildData));
	}
};

// ������������
struct GuildLeaveMessage
{
	DWORD dwTime;								// ����ʱ��
	char szName[MAX_ROLE_LEN];					// ���Խ�ɫ����
	BYTE byPos;									// ���Խ�ɫְλ
	char szText[MAX_GUILD_LEAVEMESSAGE_LEN];	// ��������
};

// �����������
struct GuildBaseData
{
	DWORD dwId;										// ����ID
	char szGuildName[MAX_GUILD_NAME_LEN];			// ��������
	WORD wIconId;									// ����ͼ��
	WORD wLevel;									// ����ȼ�
	BYTE byMemberNum;								// ��Ա����
	char szCaptainName[MAX_ROLE_LEN];				// �᳤����
	BYTE byIsFull;									// �Ƿ�����Ա
	BYTE byIsApply;									// �Ƿ�������
	char szDeclaration[MAX_GUILD_DECLARATION_LEN];	// ����
};

// �����¼�
struct GuildEvent
{
	WORD wEventId;						// �¼�ID
	char szTargetName[MAX_ROLE_LEN];	// �¼�Ŀ������
	DWORD dwValue;						// ������ֵ
	DWORD dwTime;						// ����ʱ��
};

// ���Ÿ�������
struct GuildFBData
{
	WORD wCurPassChapter;	// ��ǰͨ�ص��½�(0=��ʾ��û��ͨ��)
	BYTE byBattleTimes;		// ����ս�������
	BYTE byBuyTimes;		// �ѹ������
	DWORD dwLastBattleTimesRecoverTime;	// ��ս��������ָ�ʱ��
	WORD wTodayStartChapter;						// ���տ�ʼ�½�
	//WORD wHistoryChapterRecord;						// ��ʷ�½ڼ�¼
	WORD wIsFromHistoryChapter;						// �Ƿ����ʷ�½ڿ�ʼ

	vector<WORD> vTakeChapterRewardRecord;	// ��ȡ�½ڽ�����¼
};

// ���Ÿ�������
struct GuildFBBox
{
	WORD wBoxIdx;		// ��������
	WORD wRewardIdx;	// ���佱������
	char szRoleName[MAX_ROLE_LEN]; // ��ȡ�ߵ�����
};

// ���Ÿ����ؿ���������
struct GuildFBMapReward
{
	vector<GuildFBBox> vTakeBoxList[CAMP_NUM];		// ����ȡ�ı����¼�б�
	set<DWORD> arAlreadyGetPlayerList[CAMP_NUM];	// �Ѿ���ȡ������б�
};

// ���Ÿ����˺�����
struct GuildFBDamageData : public PlayerCommonData
{
	DWORD dwDamage;		// �˺�
	BYTE byBattleTimes;	// ս������
};

// ���ḱ����������
// struct GuildMonsterData : public MonsterData
// {
// 	char szKillerName[MAX_ROLE_LEN];	// ��ɱ������
// };

// �ҵĹ����½ڹؿ�������¼
struct MyGuildMapRewardRecord
{
	WORD wChapter;	// �½�
	BYTE arRecordList[CAMP_NUM]; // ��¼�б� 
};


#pragma pack(pop)
#endif
