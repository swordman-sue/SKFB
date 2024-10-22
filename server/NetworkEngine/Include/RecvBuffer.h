#pragma once

#include <windows.h>

#ifdef UNLOCK_CRICUIT_QUEUE
#include "UnLockCircuitQueue.h"
#else
#include "CircuitQueue.h"
#endif

#include "NetBase.h"

//=============================================================================================================================

//=============================================================================================================================
class RecvBuffer : public CircuitQueue
{
public:
	inline VOID Init() { Clear(); }
	inline VOID	Completion( int nBytesRecvd ) { Enqueue( NULL, nBytesRecvd ); }
	inline VOID	GetRecvParam( BYTE **ppRecvPtr, int &nLength ) {

		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		*ppRecvPtr	= GetWritePtr(nTempTail);
		nLength		= GetWritableLen(nTempHead,nTempTail);
	}

	inline BOOL ReadPacketHeader(PACKET_HEADER* pHeader)
	{
		if(!pHeader)
			return FALSE;

		return Dequeue( (BYTE*)pHeader, sizeof(PACKET_HEADER) );
	}

	inline VOID	RemovePacket( WORD wSize ) { Dequeue( NULL, wSize ); }

};