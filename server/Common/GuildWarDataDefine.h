#ifndef _GUILD_WAR_DATA_DEFINE_H_
#define _GUILD_WAR_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// �Կ�����״̬
enum VSStarActivityStatus
{
	VSSTAR_ACTIVITY_CLOSE = 0,		// �ر�
	VSSTAR_ACTIVITY_DECLARE_WAR,	// ��ս
	VSSTAR_ACTIVITY_BATTLE,			// ս��
	VS_STAR_DECIDE_ATTACKER,		// ����������ʱ��
	VSSTAR_ACTIVITY_EXPLOIT = 4,	// ����

	VSSTAR_ACTIVITY_BEGIN = VSSTAR_ACTIVITY_DECLARE_WAR,
	VSSTAR_ACTIVITY_END = VSSTAR_ACTIVITY_BATTLE,
};


// ������
enum
{
	STAR_TOWER_1 = 1, // 1 = ����
	STAR_TOWER_2 = 2, // 2 = ��·
	STAR_TOWER_3 = 3, // 3 = ��·
	STAR_TOWER_4 = 4, // 4 = ��·
};

// ��������
enum
{
	// �ͼ�����
	GUILD_WAR_STAR_GREEN = 1,	// 1 = ��Ʒ
	GUILD_WAR_STAR_BLUE = 2,	// 2 = ��Ʒ

	// �߼�����
	GUILD_WAR_STAR_PURPLE = 3,	// 3 = ��Ʒ 
	GUILD_WAR_STAR_ORANGE = 4,	// 4 = ��Ʒ	
};

// �����������Ϣ
struct StarDefenderInfo
{
	DWORD dwRoleId;			// ��ɫID
	WORD wBeAttackTimes;	// ����������
};

struct StarTowerBaseInfo
{
	WORD wTowerId;				// ����ϢID
	DWORD dwDurability;			// �;ö�
	//WORD wType;					// ����
	WORD wMonsterBeAttackTimes;	// ���ﱻ��������
};

// ��������Ϣ
struct StarTowerInfo : public StarTowerBaseInfo
{
	vector<StarDefenderInfo> vDefenderList; // �������б�(�����ǲ���Ч)
};

struct StarDefenderForClient
{
	DWORD dwRoleId;			// ��ɫID
	WORD wBeAttackTimes;	// ����������
	WORD wHeadIcon;			// ͷ��(Ϊ����Ӣ��InfoId)
	DWORD dwFightingPower;	// ս��
	char szRoleName[MAX_ROLE_LEN];	// ��ɫ����
};

// ����ս����Ϣ(�ͻ���)
struct GuildWarTowerForClient
{
	WORD wTowerId;				// ����ϢID
	DWORD dwDurability;			// ��ǰ�;ö�
	WORD wMonsterBeAttackTimes;	// ���ﱻ��������
	vector<StarDefenderForClient> vDefenderList; // �������б�(�����ǲ���Ч)
};

struct StarBaseInfo
{
	DWORD dwInfoId;			// ��ϢID
	DWORD dwDelongGuildId;	// ��������ID
	DWORD dwAttackGuildId;	// ���������ID
};

// ������Ϣ
struct StarInfo : public StarBaseInfo
{
	map<DWORD, DWORD> mRewardRecordList;	// ������ȡ��¼�б�[role_id,role_id]

	//// ����ģʽר��
	map<DWORD, DWORD> mScoreList;			// �����б�(����ģʽʹ��)[GuildId,Score]

	//// ��սģ��ר��
	map<DWORD, DWORD> mDeclareWarList;		// ��ս�б�[GuildId, GuildId]
	vector<StarTowerInfo> vTowerList;		// ���б�

	// ��һ�������Ĺ���ID,ֻ�Ǳ��λ�ڼ���Ч�������������
	DWORD dwLastDelongGuildId;	// (todo::��Ϊ�Ǻ���ӵĹ���,Ϊ�˲�Ӱ��֮ǰ����,ʹ�õ��߼��е��Ť)
};


// ��ɫ����ս����
struct RoleGuildWarData
{
	WORD wBuyHighStarBattleTimes;		// 1.����߼�������ս����
	WORD wUsedLowStarBattleTimes;		// 2.�ͼ���������ս���� 
	WORD wUsedHighStarBattleTimes;		// 3.�߼���������ս����
};

// ����ս��ս��Ϣ
struct GuildWarDeclareWarData
{
	WORD wStarId; // ��ս������ID
	char szGuildName[MAX_GUILD_NAME_LEN];
	DWORD dwActive;	
	WORD wServerId;
	WORD wIconId;	// ����ͼ��
};

// ռ���������Ϣ(�ͻ���ʹ��)
struct GuildWarOccupyStarDataForClient
{
	WORD wStarId; // ����ID
	BYTE byIsHaveReward;	// �Ƿ��н���
};

// ���������Ϣ(�ͻ���ʹ��)
struct GuildWarStarBaseDataForClient
{
	char szGuildName[MAX_GUILD_NAME_LEN];	// ռ��Ĺ�������
	WORD wStarId;							// ����ID
 	WORD wIconId;							// ����ͼ��
	BYTE byIsInBattle;						// �Ƿ���ս����(���ҹ�����)	
// 	DWORD dwActive;							// �����Ծ��
};

// ��������������Ϣ
struct StarBelongGuildData
{
	DWORD dwGuildId;
	char szGuildName[MAX_GUILD_NAME_LEN];	// ��������
	DWORD dwActive;
	WORD wServerId;
	WORD wIconId;							// ����ͼ��
};

// ���򹥻�������Ϣ
struct StarAttackGuildData : public StarBelongGuildData
{
		
};


// ����ս���������Ϣ
struct GuildWarStarBaseData
{
	WORD wStarId; // ��ս������ID
	char szGuildName[MAX_GUILD_NAME_LEN];
	DWORD dwContext;	// ���±�(��Ծ��,����)
	WORD wServerId;
	WORD wIconId;		// ����ͼ��
};

#pragma pack(pop)
#endif
