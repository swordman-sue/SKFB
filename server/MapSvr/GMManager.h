#pragma once
#include "../ToolKit/Singleton.h"
#include <map>
#include <string>
#include "Const.h"

class Player;

enum DB_GM_CMD
{
	DB_GM_SYETM_MAIL = 1,					// ϵͳ�ʼ�
	DB_GM_REWARD_CENTER = 2,				// �������ķ�����(ָ��������)
	DB_GM_RELOAD_SERVER_CONFIG = 3,			// ���¼��������ļ�
	DB_GM_RELOAD_SERVER_LIST = 4,			// ���¼��ط������б�
	DB_GM_SYSTEM_NOTICE = 5,				// ϵͳ����
	DB_GM_RANK_VIEW_REFRESH = 6,			// ���������а�鿴�б�ˢ��
	DB_GM_SEND_REWARD_TO_ONE = 7,			// ���Ž���(����)
	DB_GM_SEND_REWARD_TO_ALL = 8,			// ���Ž���(������)
	DB_GM_KICK_OUT = 9,						// ��������
	DB_GM_SILENCE = 10,						// ����
	DB_GM_KICK_OUT_ALL = 11,				// ��������������
	DB_GM_DEL_RANK_DATA = 12,				// ɾ�����а��û�����
	DB_GM_RANK_REFRESH = 13,				// ���а���������
	DB_GM_RESET_GUIDE = 14,					// �������� 
	DB_GM_CLIENT_UPDATE_NOTIFY = 15,		// �㲥�ͻ��˸���
	DB_GM_SEND_ARENA_REWARD = 16,			// ���ž���������
	DB_GM_SEND_WORLD_BOSS_REWARD = 17,		// ��������
	DB_GM_SEND_SEVEN_DAY_RANK_REWARD = 18,	// ����7�����н��� 
	DB_GM_RELOAD_INIT_FILE = 19,			// ���¼���init�ļ�
	DB_GM_SAVE_ALL_DATA = 20,				// �������ݴ洢
	DB_GM_RECHARGE = 21,					// ��ֵ
	DB_GM_SYETM_MAIL_EX = 22,				// ϵͳ�ʼ�
	DB_GM_RESET_CHRISTMAS = 23,				// ����ʥ���ڻ
	DB_GM_SERVER = 6978,					// �Ϸ�����

};


// GMָ��
enum GMCmd
{
	GM_MY_TEST = 9999,
	GM_SET_LEVEL = 1,						//1.���õȼ�
	GM_ADD_EXP,								//2.��Ӿ���
	GM_SET_VIP,								//3.����VIP�ȼ�
	GM_SET_GOLD,							//4.���ý��
	GM_SET_STAMINA,							//5 ��������
	GM_SET_DIAMOND,							//6 ������ʯ
	GM_SET_ENERGY,							//7 ���þ��� 
	GM_ADD_HERO,							//8 ���Ӣ��
	GM_ADD_EQUIPMENT,						//9 ���װ��
	GM_ADD_ITEM,							//10 �����Ʒ
	GM_ADD_TREASURE,						//11 ��ӱ���
	GM_ADD_REBEL,							//12 ����Ѿ�
	GM_ADD_REBEL_VALUE,						//13 ����Ѿ�ֵ(ս��)
	GM_ADD_REBEL_DAMAGE,					//14 ����Ѿ��˺�ֵ
	GM_ADD_REBEL_EXPLOIT,					//15 ����Ѿ���ѫֵ
	GM_ADD_TOWER_PRESTIGE,					//16 �����������(����)
	GM_ADD_TOWER_STAR,						//17 �����������
	GM_ADD_LAND_PATROL_TIME,				//18 ������Ѳ��ʱ��
	GM_ADD_ARENA_PRESTIGE,					//19 ��Ӿ���������
	GM_ADD_GUILD_QS,						//20 ��ӹ���Ȫˮ	
	GM_ADD_GUILD_CONTRIBUTION,				//21 ��ӹ��ṱ��ֵ	
	GM_ADD_VIP_EXP,							//22 ���VIP����	
	GM_ADD_DAILY_MISSION_POINT,				//23 ����ճ��������
	GM_ADD_ACHIEVEMENT_VALUE,				//24 ��ӳɾ͸���ֵ
	GM_ADD_HERO_SOUL,						//25 ����ۻ�
	GM_WORLD_BOSS_START,					//26 ������ʼ
	GM_WORLD_BOSS_OVER,						//27 ��������
	GM_ADD_GUILD_EXP,						//28 ��ӹ��ᾭ��
	GM_SET_BOSS_LEVEL,						//29 ����BOSS�ȼ�
	GM_ADD_BOSS_HONOR,						//30 ���BOSS����
	GM_ADD_BOSS_DAMAGE,						//31 ���BOSS�˺�
	GM_PASS_ALL_MAIN_MAP,					//32 ͨ���������߸����ؿ�
	GM_SET_PASS_TOWER_NORMAL_MAP_IDX,		//33 ����������ͨ�ؿ�ͨ����
	GM_SET_PASS_TOWER_ELITE_MAP_IDX,		//34 ����������Ӣ�ؿ�ͨ����
	GM_ADD_TITLE,							//35 ��ӳƺ�
	GM_USE_TITLE,							//36 ʹ�óƺ�
	GM_ADD_GOD_SOUL,						//37 ������
	GM_SET_TOWER_RESET_TIMES,				//38 �����������ô���
	GM_SET_HERO_FB_CHAPTER,					//39 ����Ӣ�۸����½�
	GM_RESET_HERO_FB_BATTLE_TIMES,			//40 ����Ӣ�۸�����ս����
	GM_SET_DAILY_FB_BATTLE_TIMES,			//41 �����ճ�������ս����
	GM_PRINT_RED_PIONT_STATUS,				//42 ��ӡ���״̬
	GM_ELITE_FB_ENEMY_REFRESH,				//43 ��Ӣ�������ˢ��
	GM_ADD_SPIRIT,							//44 ��Ӿ���
	GM_SET_FRIENDSHIP_POINT,				//45 ���������
	GM_SET_PASS_ELITE_MAP,					//46 ����ͨ�ؾ�Ӣ�ؿ�
	GM_RESET_GUILD_FB_BATTLE_TIMES,			//47 ���ù��ḱ����ս����
	GM_RESET_GUILD_QY_TIMES,				//48 ����Ȫӿ����
	GM_RESET_KICK_OUT,						//49 ���Լ�
	GM_REFRESH_WEEK_GIFT,					//50 ˢ�������
	GM_MONTH_CARD_BUY,						//51 �¿�����
	GM_RECHARGE,							//52 ��ֵ
	GM_RESET_ELITE_FB_BATTLE_TIMES,			//53 ���þ�Ӣ������ս����
	GM_SET_ENDLESS_STONE,					//54 �����޾�ʯ
	GM_RESET_ENDLESS_DATA,					//55 �����޾�֮������
	GM_SUPER_DISCOUNT_RECHARGE,				//56 ��ֵ�ۿ۳�ֵ
	GM_SIGNIN,								//57 ǩ��
	GM_REMOVE_ROLE_RANK,					//58 ɾ����ɫ���а�
	GM_UPDATE_ROLE_RANK,					//59 ���������ɫ���а�
	GM_ADD_GEM,								//60 ��ӱ�ʯ
	GM_KILL_ATTACK_CITY_MONSTER,			//61 ��ɱ���﹥�ǵ�ǰ���й���
	GM_SET_KING_MATCH_SEASON,				//62 ������������ǰ����
	GM_SET_KING_MATCH_STAGE,				//63 ������������λ
	GM_SET_GEM_CION,						//64 ���ñ�ʯ��
	GM_PRINT_LAND_REWARD,					//65 ��ӡ��ؽ���
	GM_ADD_CARD,							//66 ��ӿ���
	GM_ADD_DESTINY_COIN,					//67 ����Ӳ��
	GM_ADD_STAR_SOUL,						//68 �ǻ�
	GM_ADD_COLLETION_VALUE,					//69 ����ղ�ֵ
	GM_ADD_CHRISTMAS_SCORE,					//70 ���ʥ������
	GM_SET_HEARTOFWORLD,					//71 ��������֮��
	GM_NEW_SUPER_DISCOUNT_RECHARGE,			//72 ��ֵ�ۿ۳�ֵ
	GM_ADD_OCCUPY_TIME,						//73 ���ռ��ʱ��(GMʹ��)
	GM_RESET_TOWER,							//74 ��������
	GM_BUY_RECHARGE,						//75 �����ֵ
	GM_ADD_YCOIN,							//76 ���ycoin
	GM_NEW_DAY,								//77 �������ֱ������һ��
	GM_PRINT_HERO_PRO = 78,					//78 ���Ӣ������
	GM_DELETE_ITEM,							//79 ɾ����ҵ���
	GM_SET_PVP_SCORE,						//80 �޸����pvp����
	GM_NEW_WEEK,							//81 �������ֱ������һ��
	GM_ACCUPY_STAR,							//82 ռ������
	GM_ADD_RECHARGE_AMOUNT,					//83 ��ӳ�ֵ���

	DBGM_START = 0,

};

// ϵͳ����
struct SystemReward
{
	DWORD dwServerId;	// ������ID
	WORD wMinLevel;		// �ȼ�����
	WORD wMaxLevel;		// �ȼ�����
	BYTE byRewardType;	// ��������

	RewardData tRewardData;	// ��������
};

// ϵͳ�㲥��Ϣ
struct SystemNoticeInfo
{
	WORD wServerId;	// �㲥�ķ�����ID(0=��ʾ���з�����)
	WORD wNoticeId;	// ����ID
	char szText[MAX_SYSTEM_NOTICE_LEN+1]; // ��������

	int nNoticeTimes;
};

// GM���ʼ���Ϣ
struct GMSendEmailInfo
{
	DWORD dwServerId;
	char szTitle[MAX_EMAIL_TITLE_LEN]; // ����
	char szText[MAX_EMAIL_TEXT]; // ����	
};

//�����й�����
class GMManager : public Singleton<GMManager>
{
public:
	GMManager();
	~GMManager();

public:
	BOOL		Init();
	VOID		Release();
	void		Update();

protected:
	// �����ʼ�
	void ProcessSendEmail();

	// ����ϵͳ����
	void ProcessSystemReward();


public:
	void		OnNetGMMsg(Player *pPlayer, WORD wCmdId, DWORD dwValue = 0, DWORD dwValue1 = 0, DWORD dwValue2 = 0);

public:
	// �������ݿ������
	void		OnCmdFromDB(WORD wCmdId, const DWORD *pParamList, int nParamNum, const char *pszText = NULL, const char *pszText1 = NULL);

private:
	DWORD m_dwProceesTime;
	list<SystemReward> m_vSystemRewardList;	// ϵͳ�����б�

	DWORD m_dwProcessNoticeTime;
	DWORD m_dwSendNoticeInterval;	// ������ļ��ʱ��(��)
	list<SystemNoticeInfo> m_vSystemNoticeList;	// ϵͳ�㲥�б�

	DWORD m_dwProcessEmailTime;
	list<GMSendEmailInfo> m_vGMSendEmailList;	// GM���ʼ��б�

	DWORD m_dwCPOrderId;

	DWORD m_dwActivePlayerLimitTime;	// ��Ծ�������ʱ��
};