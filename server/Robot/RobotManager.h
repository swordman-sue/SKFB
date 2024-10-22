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

//机器人管理器
class RobotManager : public Singleton<RobotManager>
{
public:
	RobotManager();
	~RobotManager();

	BOOL				Init();
	VOID				Release();
	VOID				Update(DWORD dwTick);
	VOID				Test(); // 通知命令行测试的接口
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
	//DBCFile				m_RobotDBCFile;						//机器人信息
	DBCFile				m_CardDBCFile;						//卡牌信息
	DBCFile				m_RetinueDBCFile;					//随从信息
	DBCFile				m_RobotLoginDBCFile;				//机器人登录信息
	char				m_szSvrIP[32];						//网关服务器IP
	WORD				m_wSvrPort;							//网关服务器端口
	IOCPServer			m_IOCPServer;						//完成端口服务器
	RobotSet			m_RobotSet;							//机器人列表
	BOOL				m_bEnableSecurity;					//是否加密
	INT					m_nOpenChat;
	DWORD				m_dwZoneID;
	DWORD				m_dwBattleMode;
	DWORD               m_dwSendNum;
	DBCFile             m_RobotDBCFile[5]; 
	bool				m_bTest;
	bool				m_bIsTestAttackCity;
	DWORD				m_dwSelectZoneId;
};