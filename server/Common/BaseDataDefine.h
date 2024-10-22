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


//通用数据结构定义
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

	// todo::这个接口可能会有时区的问题
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

	//Day of week (0 – 6; Sunday = 0).
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

//角色帐号Key
struct AccKey
{
	//DWORD			m_dwZoneID;						//分区ID
	DWORD			m_dwPlatform;					//渠道平台
	char			m_szAccID[MAX_ACCOUNT_LEN];		//账号ID

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

// 登录帐号信息
struct LoginAccountInfo
{
	DWORD dwGuid;
	char szAccountName[MAX_ACCOUNT_LEN + 1];			//帐号名称		
	char szPassword[MAX_ACCOUNT_PASSWORD_LEN + 1];		//帐号密码
	//char szToken[MAX_TOKEN_LEN+1];
	DWORD dwTime;									//请求登录时间
	DWORD dwChannelUID;								//渠道UID
	BYTE byLoginType;								//登录类型
	BYTE byTerminalType;							//终端类型
	DWORD dwServerId;								//登录的服务器ID
	DWORD dwRetryTimes;								//重新请求登录次数
	BYTE bIsSDK;									//是否SDK
};


//角色列表信息
struct CharListInfo
{
	char			m_szAccID[MAX_ACCOUNT_LEN];			//角色帐号ID
	DWORD			m_dwCharID;							//角色ID
	char			m_szCharName[MAX_ROLE_LEN+1];		//角色名
	BYTE			m_bySex;							//角色性别
	BYTE			m_byAge;							//角色年龄
	//USHORT			m_shLv;								//角色等级
};

//属性数据
struct Property
{
	WORD wType;		// 属性类型
	int nValue;		// 属性值
};

// 成对数据
struct PairData
{
	int nKey;
	int nValue;
};

//对象数据
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

// 技能数据
struct SkillData
{
	WORD wSkillId;	// 技能ID
	WORD wLevel;	// 技能等级
};

// 商店商品购买记录
struct ShopGoodsBuyRecord
{
	WORD wGoodsId;	// 商品ID
	WORD wBuyNum;	// 购买数量
};

// 商店商品数据
struct ShopGoodsData
{
	WORD wGoodsId;	// 商品ID
	WORD wBuyNum;	// 商品数量	
};

// 商品购买记录
struct GoodsBuyRecord
{
	ShopGoodsData arGoodsList[MAX_GOOGS_BUY_RECORD_NUM];	// 商品列表
	BYTE byGoodsNum;	// 商品数量
	//vector<ShopGoodsData> vGoodsList;	// 商品列表
};

typedef map<WORD, ShopGoodsBuyRecord> GoodsBuyRecordMap;

// 玩家常用数据
struct PlayerCommonData
{
	DWORD dwRoleId;							// 角色ID
	WORD wLevel;							// 角色等级
	WORD wHeadIcon;							// 头像(为形象英雄InfoId)
	DWORD dwPlatform;						// 平台ID
	DWORD dwFightingPower;					// 战力
	char szAccount[MAX_ACCOUNT_LEN];		// 帐号
	char szRoleName[MAX_ROLE_LEN];			// 申请角色名称
	char szGuildName[MAX_GUILD_NAME_LEN];	// 工会名称	
	DWORD dwOfflineTime;					// 离线时间(0=表示在线)
	BYTE byVipLevel;						// VIP等级
	DWORD dwSpriteId;						// 精灵ID(0=表示木有)
	DWORD dwTitleId;						// 当前使用的称号(0=表示木有)
	DWORD dwNicknameId;						// 当前使用的昵称(0=表示木有)
	WORD wServerId;
};


// 怪物信息
struct MonsterData
{
	BYTE byIndex;		// 站位索引
	DWORD dwDamage;		// 当前的累积伤害
	DWORD dwTotalHP;	// 总的HP
};

// 货币数据
struct MoneyData
{
	BYTE byType;	// 货币类型
	int nValue;		// 货币值

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

// 服务器信息
struct ServerInfo
{
	DWORD dwServerId;
	DWORD dwOpenTime;
};

// 记录数据
struct RecordData
{
	WORD wRecordId;		// 记录ID
	WORD wRecordNum;	// 记录数量
};

// 地图坐标
struct MapPos
{
	WORD wMapX;
	WORD wMapY;
};

// 场景物品
struct SceneItem
{
	DWORD dwUID;			// 物品唯一ID
	DWORD dwInfoId;			// 物品信息ID
	DWORD dwCreateTime;		// 创建时间
	DWORD dwLockPlayerId;	// 锁住宝箱的玩家Id
	DWORD dwLockTime;		// 锁住时间
	MapPos tMapPos;			// 地图坐标
};

// 物品(道具)
struct Item
{
	DWORD dwId;			// 物品ID
	DWORD dwNum;		// 数量

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

// 祝福精灵数据
struct BlessSpriteData
{
	DWORD dwSpriteId;		// 精灵id
	WORD wStar;				// 星数
	WORD wEnhanceLevel;		// 强化等级
	WORD wTrainStage;		// 培养阶
};


#pragma pack(pop)
#endif
