#pragma once

#include "../Common/DataDefine.h"

class Player;

//����˺���Ϣ(������ɫ�б���Ϣ��������ҽ��н�ɫ����)
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
	DWORD						m_dwZoneID;									//����ID
	char						m_szAccID[MAX_ACCOUNT_LEN + 1];				//�˺�ID
	char						m_szPassword[MAX_ACCOUNT_PASSWORD_LEN + 1];	//�˺�����
	DWORD						m_dwGuid;									//GateSvr�е����ΨһID
	USER_STATE					m_UserState;								//���״̬��Ϣ

	BYTE						m_bySelectedCharIndex;						//��ǰѡ�еĽ�ɫ����
	BYTE						m_byCharNum;								//��ɫ����
	BOOL						m_bOccupiedSlot[MAX_CHARACTER_NUM];			//ָ���Ľ�ɫ�����Ƿ�ռ��
	CharListInfo				m_listInfo[MAX_CHARACTER_NUM];				//��ɫ������Ϣ
	Player*						m_pCurPlayer;								//��ǰ���ָ��
	int							m_nDBIndex;									//��ǰ�˺�ʹ�õ�DBThread������
	DWORD						m_dwCurCharID;								//��ǰ��Ϸ�еĽ�ɫID
	bool						m_bRobot;									//�Ƿ�Ϊ������
	DWORD						m_dwChannelUID;								//����UID
	BYTE						m_byTerminalType;							//�ն�����
	char						m_szToken[64];
	char						m_szMobile[MAX_MOBILE_LEN];
};
