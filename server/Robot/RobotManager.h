#pragma once

#include "IOCPServer.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/DBCFile.h"
#include <set>

struct CardEntry;
struct RetinueEntry;
class  Robot;

typedef std::set<Robot*>			RobotSet;
typedef RobotSet::iterator			RobotSetIter;

//�����˹�����
class RobotManager : public Singleton<RobotManager>
{
public:
	RobotManager();
	~RobotManager();

	BOOL				Init();
	VOID				Release();
	VOID				Update(DWORD dwTick);
	VOID				Test(); // ֪ͨ�����в��ԵĽӿ�
	VOID				SetTest() { m_bTest = true; };

	VOID				OnDisconnect(Robot* pRobot);
	BOOL				IsEnableSecurity() { return m_bEnableSecurity; }
	BOOL				IsTestAttackCity() { return m_bIsTestAttackCity; };
	INT					GetOpenChat() { return m_nOpenChat; }
	DWORD				GetZoneID() { return m_dwZoneID; }
	INT					GetRobotNum() { return (INT)m_RobotSet.size(); }
	DWORD				GetBattleMode() { return m_dwBattleMode; }
	DWORD				GetSelectZoneId() { return m_dwSelectZoneId; }

	CardEntry*			GetCardEntry(DWORD dwCardID);
	RetinueEntry*		GetRetinueEntry(DWORD dwRetinueID);
	DWORD               GetSendNum(){return m_dwSendNum;}
	void                AddSendNum(){ ++m_dwSendNum;}

private:
	//DBCFile				m_RobotDBCFile;						//��������Ϣ
	DBCFile				m_CardDBCFile;						//������Ϣ
	DBCFile				m_RetinueDBCFile;					//�����Ϣ
	DBCFile				m_RobotLoginDBCFile;				//�����˵�¼��Ϣ
	char				m_szSvrIP[32];						//���ط�����IP
	WORD				m_wSvrPort;							//���ط������˿�
	IOCPServer			m_IOCPServer;						//��ɶ˿ڷ�����
	RobotSet			m_RobotSet;							//�������б�
	BOOL				m_bEnableSecurity;					//�Ƿ����
	INT					m_nOpenChat;
	DWORD				m_dwZoneID;
	DWORD				m_dwBattleMode;
	DWORD               m_dwSendNum;
	DBCFile             m_RobotDBCFile[5]; 
	bool				m_bTest;
	bool				m_bIsTestAttackCity;
	DWORD				m_dwSelectZoneId;
};