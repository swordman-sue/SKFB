/**\file
   ����Map��������Ϣ�߼�
   \author Kevin
   \date 2014/12/24
   \note 
*/

#pragma once

#include "ServerSession.h"


//��ͼ�������Ự��
class MapServerSession : public ServerSession
{
public:
	MapServerSession(void);
	virtual ~MapServerSession(void);

public:
	virtual	VOID		OnRecv( BYTE *pMsg, WORD wSize );
	virtual SERVER_TYPE	GetServerType() { return MAP_SERVER;}
	virtual VOID		OnDisconnect();
	virtual VOID		OnAccept( DWORD dwNetworkIndex );

	DWORD				GetServerID() { return m_dwServerID; }
	VOID				SetServerID(DWORD dwServerID) { m_dwServerID = dwServerID; }

	bool				IsValid() { return m_bValid; }
	VOID				SetValid(bool bValid) { m_bValid = bValid; }

	virtual VOID		Init();
	virtual VOID		Release();

private:
	DWORD				m_dwServerID;
	bool				m_bValid;

};
