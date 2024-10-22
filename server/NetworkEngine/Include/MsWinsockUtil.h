#pragma once

class MsWinsockUtil
{
public:
	static void	LoadExtensionFunction( SOCKET ActiveSocket );		

	static LPFN_ACCEPTEX				m_lpfnAccepteEx;					//AcceptEx 
	static LPFN_TRANSMITFILE			m_lpfnTransmitFile;					//TransMitFile 
	static LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockAddrs;			//GetAcceptExSockaddrs
	static LPFN_DISCONNECTEX			m_lpfnDisconnectEx;					//DisconnectEx

private:
	static bool LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc);
};
