#pragma once

#include "NetworkObject.h"

class Robot;

//机器人会话
class RobotSession : public NetworkObject
{

public:
	RobotSession();
	~RobotSession();

public:
	//受信后处理
	virtual	VOID	OnRecv( BYTE *pData, WORD wSize );
	//连接后处理
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
	//断开连接后处理
	virtual VOID	OnDisconnect();
	//设定会话所有者
	void			SetOwner( Robot* pRobot ) { m_pOwner = pRobot; }
	Robot*			GetOwner() { return m_pOwner; }
	//是否已经连接
	BOOL			IsConnected() { return m_bIsConnected; }

private:
	Robot*			m_pOwner;
	BOOL			m_bIsConnected;
};
