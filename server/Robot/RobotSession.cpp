#include "stdafx.h"
#include "RobotSession.h"
#include "Robot.h"
#include "PacketHandler.h"

RobotSession::RobotSession()
{
	m_pOwner = NULL;
	m_bIsConnected = FALSE;
}

RobotSession::~RobotSession()
{

}

VOID RobotSession::OnRecv( BYTE *pData, WORD wSize )
{
	PacketHandler::Instance()->ParsePacket(this, (unsigned char*)pData, wSize);
}

//���Ӻ���
VOID RobotSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	if (!m_pOwner)
		return;

	m_bIsConnected = bSuccess;
	m_pOwner->OnConnect(bSuccess, dwNetworkIndex);
}

//�Ͽ����Ӻ���
VOID RobotSession::OnDisconnect()
{
	if (!m_pOwner)
		return;

	m_bIsConnected = FALSE;
	m_pOwner->OnDisconnect();
}
