#pragma once

#include "NetworkObject.h"

class Robot;

//�����˻Ự
class RobotSession : public NetworkObject
{

public:
	RobotSession();
	~RobotSession();

public:
	//���ź���
	virtual	VOID	OnRecv( BYTE *pData, WORD wSize );
	//���Ӻ���
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
	//�Ͽ����Ӻ���
	virtual VOID	OnDisconnect();
	//�趨�Ự������
	void			SetOwner( Robot* pRobot ) { m_pOwner = pRobot; }
	Robot*			GetOwner() { return m_pOwner; }
	//�Ƿ��Ѿ�����
	BOOL			IsConnected() { return m_bIsConnected; }

private:
	Robot*			m_pOwner;
	BOOL			m_bIsConnected;
};
