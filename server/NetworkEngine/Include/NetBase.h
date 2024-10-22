#ifndef _NETBASE_H_
#define _NETBASE_H_

//=============================================================================================================================

//=============================================================================================================================
typedef struct tagPACKET_HEADER
{
 	DWORD	dwPackLen;		//整个协议包的总长度(msg_id + msg_text),不包括总长度自己.
// 	DWORD	dwHeaderLen;	//头部的长度
} PACKET_HEADER;

struct ProtoBasic
{
	ProtoBasic()
	{
		this->dwPackLen = 0;
		this->dwHeaderLen = 0;
		this->dwGUID = 0;
	}

	ProtoBasic(DWORD dwPackLen, DWORD dwHeaderLen, DWORD dwGUID)
	{
		this->dwPackLen = dwPackLen;
		this->dwHeaderLen = dwHeaderLen;
		this->dwGUID = dwGUID;
	}
	int Decode(const char* pData, DWORD uSize)
	{
		if ( uSize < sizeof(ProtoBasic) )
		{
			//LOG_DEBUG("DecodeBasic error, size : " << uSize);
			return -1;
		}

		dwPackLen = *((DWORD*)pData);
		dwHeaderLen = *((DWORD*)(pData+sizeof(DWORD)));
		dwGUID = *((DWORD*)(pData+sizeof(DWORD)+sizeof(DWORD)));
		//LOG_DEBUG("uPackLen : " << uPackLen << ", uHeaderLen : " << uHeaderLen);	
		return 0;	
	}

	DWORD dwPackLen;	//整个协议包的总长度
	DWORD dwHeaderLen;	//头部的长度
	DWORD dwGUID;		//玩家GUID(服务器使用)
};


#endif