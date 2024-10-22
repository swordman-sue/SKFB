#pragma once
#include "usersession.h"

class ServerSession;
struct CS_MSG_BASE;
//class google::protobuf::Message;


class User : public UserSession
{
public:
	User(void);
	virtual ~User(void);

	virtual VOID	Init();
	virtual VOID	Release();

public:

	virtual int		GetObjectType(){ return OBJECT_TYTE_USER; }
	virtual BOOL	SendPacket(WORD dwMsgID, NetMsgBody * pMsg);
	virtual BOOL	SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize);
	virtual	VOID	OnRecv(BYTE *pMsg, WORD wSize);
	virtual VOID	OnDisconnect();
	virtual VOID	OnAccept(DWORD dwNetworkIndex);

	VOID			Process(DWORD dwTime);

	BOOL			SendToMapServer(WORD dwMsgID, DWORD dwSeriesId, unsigned char * pData, WORD wDataSize);
	BOOL			SendToMapServer(NetMsgBody * pMsg);

	VOID			SetDisConnectFromMap(BOOL bDisconnect = TRUE) { m_bDisconnectFromMap = bDisconnect; }

	BOOL			GetSendFlag() { return m_bSendFlag; }
	time_t			GetCreateTime() { return m_tCreateTime; }
	time_t			GetStamp() { return m_tStamp; }

	DWORD			GetIndex() { return m_dwIndex; };
	VOID			SetIndex(DWORD dwIndex) { m_dwIndex = dwIndex; };

	DWORD			GetServerId() { return m_dwServerId; };
	VOID			SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };

private:
	BOOL			m_bDisconnectFromMap;		//�Ͽ���Ϣ�Ƿ�����MapSvr
	BOOL			m_bSendFlag;				//�Ƿ���MapSvr�������ݸ��ͻ���
	time_t			m_tCreateTime;				//����ʱ��
	time_t			m_tStamp;					//����MapSvr��������ʱ��
	DWORD			m_dwServerId;				//���ڷ�����ID

	// ���ʹ��
	DWORD			m_dwTotalPacketCount;		//�ۻ�����
	DWORD			m_dwMaxPacketCount;			//��ʷͳ���������
	DWORD			m_dwCurPacketCount;			//��ǰͳ������
	DWORD			m_dwCountIntervalTime;		//ͳ�Ƶļ��ʱ��(10��)
	UINT64			m_ulProcessTime;			//��ʱʱ��

	DWORD			m_dwIndex;
};
