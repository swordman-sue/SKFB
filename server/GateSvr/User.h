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
	BOOL			m_bDisconnectFromMap;		//断开消息是否来至MapSvr
	BOOL			m_bSendFlag;				//是否有MapSvr发送数据给客户端
	time_t			m_tCreateTime;				//创建时间
	time_t			m_tStamp;					//接受MapSvr发送数据时间
	DWORD			m_dwServerId;				//所在服务器ID

	// 监控使用
	DWORD			m_dwTotalPacketCount;		//累积数量
	DWORD			m_dwMaxPacketCount;			//历史统计最大数量
	DWORD			m_dwCurPacketCount;			//当前统计数量
	DWORD			m_dwCountIntervalTime;		//统计的间隔时间(10秒)
	UINT64			m_ulProcessTime;			//计时时间

	DWORD			m_dwIndex;
};
