#ifndef _NETBASE_H_
#define _NETBASE_H_

//=============================================================================================================================

//=============================================================================================================================
typedef struct tagPACKET_HEADER
{
 	DWORD	dwPackLen;		//����Э������ܳ���(msg_id + msg_text),�������ܳ����Լ�.
// 	DWORD	dwHeaderLen;	//ͷ���ĳ���
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

	DWORD dwPackLen;	//����Э������ܳ���
	DWORD dwHeaderLen;	//ͷ���ĳ���
	DWORD dwGUID;		//���GUID(������ʹ��)
};


#endif