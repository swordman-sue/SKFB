#pragma once

#include "../Common/DataDefine.h"

class Player;

//玩家账号信息(包括角色列表信息，方便玩家进行角色管理)
class AccountInfo
{
public:
	AccountInfo(void);
	~AccountInfo(void);

public:
	VOID						Init();
	VOID						Create(DWORD dwZoneID, const char* pszAccID, const char* pszPassword,DWORD dwGuid, 
		DWORD dwChannelUID, BYTE byTerminalType, const char *pszMobile);

public:
	DWORD						GetZoneID() { return m_dwZoneID; }
	char*						GetAccID() { return m_szAccID; }
	char*						GetPassword() { return m_szPassword; }
	DWORD						GetGuid() { return m_dwGuid; }

	VOID						SetZoneId(DWORD dwZoneId) { m_dwZoneID = dwZoneId; };

	USER_STATE					GetUserState() { return m_UserState; }
	VOID						SetUserState(USER_STATE UserState) { m_UserState = UserState; }
	BYTE						GetSelectedCharIndex() { return m_bySelectedCharIndex; }
	VOID						SetSelectedCharIndex(BYTE bySelectedCharIndex);
	DWORD						GetSelectedCharID();
	const char*					GetSelectedCharName();
	BYTE						GetCharNum() { return m_byCharNum; }
	VOID						SetCharNum(BYTE byCharNum) { m_byCharNum = byCharNum; }

	VOID						SetOccupiedSolt(DWORD dwSoltIndex,BOOL val);
	BOOL						FindEmptySlot(BYTE& dwSoltIndex) const;

	VOID						SetPlayerListInfo(const CharListInfo& charinfo, BYTE byIndex);
	CharListInfo*				GetPlayerListInfoByIndex(BYTE byIndex);
	CharListInfo*				GetPlayerListInfoByCharID(DWORD dwCharID);

	Player*						GetPlayer() { return m_pCurPlayer; }
	VOID						SetPlayer(Player* pPlayer) { m_pCurPlayer = pPlayer; }

	int							GetDBIndex() { return m_nDBIndex; }
	VOID						SetDBIndex(int nDBIndex) { m_nDBIndex = nDBIndex; }

	DWORD						GetCurCharID() { return m_dwCurCharID; }
	VOID						SetCurCharID(DWORD dwCurCharID) { m_dwCurCharID = dwCurCharID; }

	bool						IsRobot() { return m_bRobot; }
	VOID						SetRobot(bool bRobot) { m_bRobot = bRobot; }

	void						SetChannelUID(DWORD dwChannelUID) { m_dwChannelUID = dwChannelUID; };
	DWORD						GetChannelUID() { return m_dwChannelUID; };

	void						SetTerminalType(BYTE byTerminalType) { m_byTerminalType = byTerminalType; };
	BYTE						GetTerminalType() { return m_byTerminalType; };

	const char*					GetToken() { return m_szToken; };

	const char*					GetMobile() { return m_szMobile; };
	void						SetMobile(const char *pszMobile)
	{
		strcpy_s(m_szMobile, sizeof(m_szMobile)-1, pszMobile);
	}

private:
	DWORD						m_dwZoneID;									//分区ID
	char						m_szAccID[MAX_ACCOUNT_LEN + 1];				//账号ID
	char						m_szPassword[MAX_ACCOUNT_PASSWORD_LEN + 1];	//账号密码
	DWORD						m_dwGuid;									//GateSvr中的玩家唯一ID
	USER_STATE					m_UserState;								//玩家状态信息

	BYTE						m_bySelectedCharIndex;						//当前选中的角色索引
	BYTE						m_byCharNum;								//角色数量
	BOOL						m_bOccupiedSlot[MAX_CHARACTER_NUM];			//指定的角色索引是否被占用
	CharListInfo				m_listInfo[MAX_CHARACTER_NUM];				//角色基础信息
	Player*						m_pCurPlayer;								//当前玩家指针
	int							m_nDBIndex;									//当前账号使用的DBThread索引号
	DWORD						m_dwCurCharID;								//当前游戏中的角色ID
	bool						m_bRobot;									//是否为机器人
	DWORD						m_dwChannelUID;								//渠道UID
	BYTE						m_byTerminalType;							//终端类型
	char						m_szToken[64];
	char						m_szMobile[MAX_MOBILE_LEN];
};
