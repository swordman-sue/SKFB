#pragma once

#include <windows.h>

#ifdef UNLOCK_CRICUIT_QUEUE
#include "UnLockCircuitQueue.h"
#else
#include "CircuitQueue.h"
#endif

#include "NetBase.h"
#include "Session.h"

//=============================================================================================================================

//=============================================================================================================================
class SendBuffer : public CircuitQueue
{
public:
	inline VOID Init() { m_bComplete = TRUE; Clear(); }
	inline VOID Completion( int nBytesSend ) { 	Dequeue( NULL, nBytesSend ); m_bComplete = TRUE;}

	inline int Write( tagPACKET_HEADER *pHeader, BYTE *pMsg, int nMsgSize)
	{
		if (!pHeader || !pMsg || nMsgSize==0)
		{
			return -1;
		}

		int nTempHead;
		int nTempTail;
		GetCriticalData(&nTempHead,&nTempTail);

		int nTempTail2 = nTempTail;

		if (GetSpace(nTempHead,nTempTail)<(int)(nMsgSize+sizeof(tagPACKET_HEADER)))
		{
			return -2;
		}

		//写Head信息
		BYTE* pTempSrc = (BYTE*)pHeader;
		for (int i = 0; i < sizeof(tagPACKET_HEADER); i++)
		{
			m_pData[nTempTail2] = pTempSrc[i];
			nTempTail2 = (nTempTail2 + 1) % m_nSize;
		}

		//写Msg信息
		if( nTempHead <= nTempTail2 )
		{
			int nBackSpaceCount = m_nSize - nTempTail2;

			if( nBackSpaceCount >= nMsgSize )  
			{
				memcpy(m_pData + nTempTail2,pMsg,nMsgSize);
			}
			else
			{
				memcpy( m_pData + nTempTail2, pMsg, nBackSpaceCount );
				memcpy( m_pData, pMsg + nBackSpaceCount, ( nMsgSize - nBackSpaceCount ) );
			}
		}
		else
		{
			memcpy( m_pData + nTempTail2, pMsg, nMsgSize );
		}

		nTempTail2 = (nTempTail2+nMsgSize)%m_nSize;
		SetCriticalData(-1,nTempTail2);

		return 0;
	}

/*
	inline BOOL	GetSendParam( BYTE **ppSendPtr, int &nLength ) {
		if( !IsReadyToSend() ) { nLength = 0; return FALSE; }
		*ppSendPtr	= GetReadPtr();
		nLength		= GetReadableLen();
		m_bComplete = FALSE;
		return TRUE;
	}
*/

	inline BOOL	GetSendParam( BYTE **ppSendPtr, int &nLength )
	{
		if (m_bComplete==FALSE)
		{
			return FALSE;
		}

		int nTempHead;
		int nTempTail;
		GetCriticalData(&nTempHead,&nTempTail);
		
		if (GetLength(nTempHead,nTempTail) <= 0)
		{
			return FALSE;
		}

		*ppSendPtr	= GetReadPtr(nTempHead);
		nLength		= GetReadableLen(nTempHead,nTempTail);
		m_bComplete = FALSE;
		return TRUE;
	}

/*
private:
	inline BOOL			IsReadyToSend() { return ( m_bComplete && GetLength() > 0 ) ? TRUE : FALSE; }
*/


private:
	volatile BOOL		m_bComplete;
};
