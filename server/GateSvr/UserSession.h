#pragma once

#include "NetworkObject.h"

class NetMsgBody;

//��һỰ��
class UserSession : public NetworkObject
{
public:
	UserSession(void);
	virtual ~UserSession(void);

public:
	virtual BOOL		SendPacket(WORD wMsgID, NetMsgBody * pMsg);
	virtual BOOL		SendPacket(WORD wMsgID, unsigned char * pData, WORD wDataSize);
	DWORD				GetSessionIndex() { return m_dwSessionIndex; }
	VOID				SetSessionIndex( DWORD dwIndex ) { m_dwSessionIndex = dwIndex; }
	DWORD				GetGUID() { return m_dwGUID; }
	VOID				SetGUID(DWORD dwGUID) { m_dwGUID = dwGUID; }
	virtual int			GetObjectType(){return OBJECT_TYTE_USERSESSION;}

protected:
	virtual VOID		Init();
	virtual VOID		Release();

	virtual VOID		OnDisconnect();
	virtual VOID		OnAccept(DWORD dwNetworkIndex);
	VOID				OutputDebug(BYTE* pbyMsg, UINT nSize, USHORT shMsgID);

protected:
	DWORD				m_dwSessionIndex;	//�Ự����
	DWORD				m_dwGUID;			//���GUID(GateSvr��Ψһ��ʾһ�����)
};
