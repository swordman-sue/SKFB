#ifndef _BASE_DATA_DEFINE_H_
#define _BASE_DATA_DEFINE_H_

#include "Const.h"
#include <time.h>
#include <stdlib.h>
#include <map>
#include <vector>
using namespace std;

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


//ͨ�����ݽṹ����
//Date&Time
struct DATETIME
{
	unsigned short		year;
	unsigned char		month;
	unsigned char		day;
	unsigned char		hour;
	unsigned char		minute;
	unsigned char		second;
	
	DATETIME()
	{
		year		= 0;
		month		= 0;
		day			= 0;
		hour		= 0;
		minute		= 0;
		second		= 0;
	}

	DATETIME( const DATETIME& rhs )
	{
		year		= rhs.year;
		month		= rhs.month;
		day			= rhs.day;
		hour		= rhs.hour;
		minute		= rhs.minute;
		second		= rhs.second;		
	}

	DATETIME( unsigned short y, unsigned char mon, unsigned char d, unsigned char h, unsigned char min, unsigned char s )
	{
		year		= y;
		month		= mon;
		day			= d;
		hour		= h;
		minute		= min;
		second		= s;
	}

	DATETIME(const time_t now)
	{
		tm* gmtm = localtime(&now);
		if (!gmtm)
			return;

		year	= (unsigned short)gmtm->tm_year + 1900;
		month	= (unsigned char)(gmtm->tm_mon + 1);
		day		= (unsigned char)gmtm->tm_mday;
		hour	= (unsigned char)gmtm->tm_hour;
		minute	= (unsigned char)gmtm->tm_min;
		second	= (unsigned char)gmtm->tm_sec;
	}

	DATETIME& operator= ( const DATETIME& rhs )
	{
		if ( this != &rhs )
		{
			year		= rhs.year;
			month		= rhs.month;
			day			= rhs.day;
			hour		= rhs.hour;
			minute		= rhs.minute;
			second		= rhs.second;			
		}
		return *this;
	}

	DATETIME& operator=(const time_t now)
	{
		tm* gmtm = localtime(&now);
		if (!gmtm)
			return *this;

		year	= (unsigned short)gmtm->tm_year + 1900;
		month	= (unsigned char)(gmtm->tm_mon + 1);
		day		= (unsigned char)gmtm->tm_mday;
		hour	= (unsigned char)gmtm->tm_hour;
		minute	= (unsigned char)gmtm->tm_min;
		second	= (unsigned char)gmtm->tm_sec;

		return *this;
	}

	// todo::����ӿڿ��ܻ���ʱ��������
	operator time_t() const
	{
		tm gmtm;
		gmtm.tm_isdst = -1;
		gmtm.tm_sec = second;
		gmtm.tm_min = minute;
		gmtm.tm_hour = hour;
		gmtm.tm_mday = day;
		gmtm.tm_mon = month - 1;
		gmtm.tm_year = year - 1900;
		return mktime(&gmtm);
	}

	//Day of week (0 �C 6; Sunday = 0).
	int GetDayOfWeek() const
	{
		tm gmtm;
		gmtm.tm_isdst = 0;
		gmtm.tm_sec = second;
		gmtm.tm_min = minute;
		gmtm.tm_hour = hour;
		gmtm.tm_mday = day;
		gmtm.tm_mon = month - 1;
		gmtm.tm_year = year - 1900;

		time_t tmptm = mktime(&gmtm);

		tm* plocaltime = localtime(&tmptm);
		
		return plocaltime ? plocaltime->tm_wday : 0 ;
	}

	bool operator==(const DATETIME& rhs) const
	{
		if (this != &rhs)
		{
			return rhs.second == second
				&& rhs.minute == minute
				&& rhs.hour == hour
				&& rhs.day == day
				&& rhs.month == month
				&& rhs.year == year;
		}
		return true;
	}

	bool operator<(const DATETIME& rhs) const
	{
		if (this != &rhs)
		{
			return year == rhs.year ?
				month == rhs.month ?
				day == rhs.day ?
				hour == rhs.hour ?
				minute == rhs.minute ?
				second < rhs.second:
			minute < rhs.minute:
			hour < rhs.hour:
			day < rhs.day:
			month < rhs.month:
			year < rhs.year;
		}
		return false;
	}

	int	toString( char* output ) const
	{
		if ( output == NULL )
		{
			return -1;
		}

		sprintf( output, "%04d-%02d-%02d %02d:%02d:%02d", year, (int)month, (int)day, (int)hour, (int)minute, (int)second );

		return 0;
	}

	int ParseFromString( const char* input )
	{
		if ( input == NULL )
		{
			return -1;
		}

		if ( strlen( input ) != strlen("0000-00-00 00:00:00") )
		{
			return -2;
		}

		char temp[5];

		memcpy( temp, input, 4 );
		temp[4] = '\0';
		year	= ( unsigned short )atoi( temp );

		memcpy( temp, input + 5, 2 );
		temp[2] = '\0';
		month	= ( unsigned char )atoi( temp );

		memcpy( temp, input + 8, 2 );
		temp[2] = '\0';
		day		= ( unsigned char )atoi( temp );

		memcpy( temp, input + 11, 2 );
		temp[2] = '\0';
		hour	= ( unsigned char )atoi( temp );

		memcpy( temp, input + 14, 2 );
		temp[2] = '\0';
		minute  = ( unsigned char )atoi( temp );

		memcpy( temp, input + 17, 2 );
		temp[2] = '\0';
		second	= ( unsigned char )atoi( temp );

		return 0;
	}
};

//��ɫ�ʺ�Key
struct AccKey
{
	//DWORD			m_dwZoneID;						//����ID
	DWORD			m_dwPlatform;					//����ƽ̨
	char			m_szAccID[MAX_ACCOUNT_LEN];		//�˺�ID

	bool operator < (const AccKey& other) const
	{
		if (m_dwPlatform < other.m_dwPlatform)
		{
			return true;
		}
		else if (m_dwPlatform > other.m_dwPlatform)
		{
			return false;
		}
		else
		{
			if (strcmp(m_szAccID, other.m_szAccID) < 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
};

// ��¼�ʺ���Ϣ
struct LoginAccountInfo
{
	DWORD dwGuid;
	char szAccountName[MAX_ACCOUNT_LEN + 1];			//�ʺ�����		
	char szPassword[MAX_ACCOUNT_PASSWORD_LEN + 1];		//�ʺ�����
	//char szToken[MAX_TOKEN_LEN+1];
	DWORD dwTime;									//�����¼ʱ��
	DWORD dwChannelUID;								//����UID
	BYTE byLoginType;								//��¼����
	BYTE byTerminalType;							//�ն�����
	DWORD dwServerId;								//��¼�ķ�����ID
	DWORD dwRetryTimes;								//���������¼����
	BYTE bIsSDK;									//�Ƿ�SDK
};


//��ɫ�б���Ϣ
struct CharListInfo
{
	char			m_szAccID[MAX_ACCOUNT_LEN];			//��ɫ�ʺ�ID
	DWORD			m_dwCharID;							//��ɫID
	char			m_szCharName[MAX_ROLE_LEN+1];		//��ɫ��
	BYTE			m_bySex;							//��ɫ�Ա�
	BYTE			m_byAge;							//��ɫ����
	//USHORT			m_shLv;								//��ɫ�ȼ�
};

//��������
struct Property
{
	WORD wType;		// ��������
	int nValue;		// ����ֵ
};

// �ɶ�����
struct PairData
{
	int nKey;
	int nValue;
};

//��������
struct ObjectData
{
	//BYTE byType;
	DWORD dwObjId;
	WORD wObjNum;

	ObjectData(/*BYTE byType = 0, */DWORD dwObjId = 0, WORD wObjNum = 0)
	{
		//this->byType = byType;
		this->dwObjId = dwObjId;
		this->wObjNum = wObjNum;
	}
};

// ��������
struct SkillData
{
	WORD wSkillId;	// ����ID
	WORD wLevel;	// ���ܵȼ�
};

// �̵���Ʒ�����¼
struct ShopGoodsBuyRecord
{
	WORD wGoodsId;	// ��ƷID
	WORD wBuyNum;	// ��������
};

// �̵���Ʒ����
struct ShopGoodsData
{
	WORD wGoodsId;	// ��ƷID
	WORD wBuyNum;	// ��Ʒ����	
};

// ��Ʒ�����¼
struct GoodsBuyRecord
{
	ShopGoodsData arGoodsList[MAX_GOOGS_BUY_RECORD_NUM];	// ��Ʒ�б�
	BYTE byGoodsNum;	// ��Ʒ����
	//vector<ShopGoodsData> vGoodsList;	// ��Ʒ�б�
};

typedef map<WORD, ShopGoodsBuyRecord> GoodsBuyRecordMap;

// ��ҳ�������
struct PlayerCommonData
{
	DWORD dwRoleId;							// ��ɫID
	WORD wLevel;							// ��ɫ�ȼ�
	WORD wHeadIcon;							// ͷ��(Ϊ����Ӣ��InfoId)
	DWORD dwPlatform;						// ƽ̨ID
	DWORD dwFightingPower;					// ս��
	char szAccount[MAX_ACCOUNT_LEN];		// �ʺ�
	char szRoleName[MAX_ROLE_LEN];			// �����ɫ����
	char szGuildName[MAX_GUILD_NAME_LEN];	// ��������	
	DWORD dwOfflineTime;					// ����ʱ��(0=��ʾ����)
	BYTE byVipLevel;						// VIP�ȼ�
	DWORD dwSpriteId;						// ����ID(0=��ʾľ��)
	DWORD dwTitleId;						// ��ǰʹ�õĳƺ�(0=��ʾľ��)
	DWORD dwNicknameId;						// ��ǰʹ�õ��ǳ�(0=��ʾľ��)
	WORD wServerId;
};


// ������Ϣ
struct MonsterData
{
	BYTE byIndex;		// վλ����
	DWORD dwDamage;		// ��ǰ���ۻ��˺�
	DWORD dwTotalHP;	// �ܵ�HP
};

// ��������
struct MoneyData
{
	BYTE byType;	// ��������
	int nValue;		// ����ֵ

	MoneyData()
	{
		byType = 0;
		nValue = 0;
	}

	MoneyData(BYTE byType, int nValue)
	{
		this->byType = byType;
		this->nValue = nValue;
	}
};

// ��������Ϣ
struct ServerInfo
{
	DWORD dwServerId;
	DWORD dwOpenTime;
};

// ��¼����
struct RecordData
{
	WORD wRecordId;		// ��¼ID
	WORD wRecordNum;	// ��¼����
};

// ��ͼ����
struct MapPos
{
	WORD wMapX;
	WORD wMapY;
};

// ������Ʒ
struct SceneItem
{
	DWORD dwUID;			// ��ƷΨһID
	DWORD dwInfoId;			// ��Ʒ��ϢID
	DWORD dwCreateTime;		// ����ʱ��
	DWORD dwLockPlayerId;	// ��ס��������Id
	DWORD dwLockTime;		// ��סʱ��
	MapPos tMapPos;			// ��ͼ����
};

// ��Ʒ(����)
struct Item
{
	DWORD dwId;			// ��ƷID
	DWORD dwNum;		// ����

	Item()
	{
		memset(this, 0, sizeof(Item));
	}

	Item(DWORD dwId, DWORD dwNum)
	{
		this->dwId = dwId;
		this->dwNum = dwNum;
	}
};

// ף����������
struct BlessSpriteData
{
	DWORD dwSpriteId;		// ����id
	WORD wStar;				// ����
	WORD wEnhanceLevel;		// ǿ���ȼ�
	WORD wTrainStage;		// ������
};


#pragma pack(pop)
#endif
