#pragma once

#include <windows.h>
#include <stdio.h>
#include <assert.h>

//=============================================================================================================================
/// 环形队列
//=============================================================================================================================
class CircuitQueue
{
public:
	CircuitQueue() : m_pData( NULL ),  m_nSize( 0 ), m_nHead( 0 ), m_nTail( 0 )
	{
	}

	virtual ~CircuitQueue()
	{
		if( m_pData ) delete [] m_pData;
	}


	void Create( int nSize, int nExtraSize = 0 )
	{
		if( m_pData ) delete [] m_pData;

		m_pData			= new BYTE[nSize + nExtraSize];
		m_nSize			= nSize;
		m_nExtraSize	= nExtraSize;
	}

	
	inline void Clear()
	{
		m_nHead         = 0;
		m_nTail         = 0;
	}

	inline int GetSize()
	{
		return m_nSize;
	}

	inline int      GetLength()
	{
		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		if( nTempHead<=nTempTail )
		{
			return nTempTail-nTempHead;
		}
		else
		{
			return m_nSize-nTempHead+nTempTail;
		}
	}

	inline BOOL Enqueue( BYTE *pSrc, int nSize )
	{
		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		if( GetSpace(nTempHead,nTempTail) < nSize )
		{
			printf("buffer overflow\n");
			return FALSE;
		}

		if( pSrc )
		{
			if( nTempHead <= nTempTail )
			{
				int nBackSpaceCount = m_nSize - nTempTail;

				if( nBackSpaceCount >= nSize )  
				{
					
					memcpy( m_pData + nTempTail, pSrc, sizeof(BYTE) * nSize );
				}
				else
				{
					memcpy( m_pData + nTempTail, pSrc, sizeof(BYTE) * nBackSpaceCount );
					memcpy( m_pData, pSrc + nBackSpaceCount, sizeof(BYTE) * ( nSize - nBackSpaceCount ) );
				}
			}
			else
			{
				memcpy( m_pData + nTempTail, pSrc, sizeof(BYTE) * nSize );
			}
		}

		nTempTail = (nTempTail+nSize)%m_nSize;
		SetCriticalData(-1,nTempTail);
		return TRUE;
	}

	
	inline BOOL Dequeue( BYTE *pTar, int nSize )
	{
		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		if( GetLength(nTempHead,nTempTail) < nSize )
		{
			return FALSE;
		}

		if( pTar != NULL )
		{
			if( nTempHead < nTempTail )
			{

				memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * nSize );
			}
			else
			{
				if( m_nSize-nTempHead >= nSize )
				{

					memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * nSize );                           
				}
				else
				{
					memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * (m_nSize-nTempHead) );
					memcpy( pTar + m_nSize-nTempHead, m_pData, sizeof(BYTE) * ( nSize - (m_nSize-nTempHead) ) );
				}
			}
		}

		nTempHead = (nTempHead+nSize)%m_nSize;
		SetCriticalData(nTempHead,-1);

		return TRUE;
	}

	
	inline BOOL Peek( BYTE *pTar, int nSize )
	{
		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		if( GetLength(nTempHead,nTempTail) < nSize )
		{
			return FALSE;
		}
		
		if( pTar != NULL )
		{
			if( nTempHead < nTempTail )
			{
				
				memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * nSize );
			}
			else
			{
				if( m_nSize-nTempHead >= nSize )
				{
					
					memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * nSize );                           
				}
				else
				{
					memcpy( pTar, m_pData + nTempHead, sizeof(BYTE) * (m_nSize-nTempHead) );
					memcpy( pTar + m_nSize-nTempHead, m_pData, sizeof(BYTE) * ( nSize - (m_nSize-nTempHead) ) );
				}
			}
		}

		return TRUE;
	}


	inline void CopyHeadDataToExtraBuffer( int nSize )
	{
		assert( nSize <= m_nExtraSize );
		memcpy( m_pData + m_nSize, m_pData, nSize );

	}

	inline BYTE* GetPacketPtr(int wSize)
	{
		int nTempHead;
		int nTempTail;

		GetCriticalData(&nTempHead,&nTempTail);

		if( GetLength(nTempHead,nTempTail) < wSize )	return NULL;

		if( m_nSize - nTempHead < wSize )
			CopyHeadDataToExtraBuffer( wSize - (m_nSize - nTempHead) );

		return  GetReadPtr(nTempHead);
	}

	inline void GetCriticalData(int *piBeginIdx, int *piEndIdx)
	{
		if( piBeginIdx )
		{
			*piBeginIdx = m_nHead;
		}

		if( piEndIdx )
		{
			*piEndIdx = m_nTail;
		}
	}

	inline void SetCriticalData(int iBeginIdx, int iEndIdx)
	{
		if( iBeginIdx >= 0 && iBeginIdx < m_nSize )
		{
			m_nHead = iBeginIdx;
		}

		if( iEndIdx >= 0 && iEndIdx < m_nSize )
		{
			m_nTail = iEndIdx;
		}
	}

protected:

	inline int      GetLength(int nTempHead,int nTempTail)
	{
		if( nTempHead<=nTempTail )
		{
			return nTempTail-nTempHead;
		}
		else
		{
			return m_nSize-nTempHead+nTempTail;
		}
	}

	inline int      GetSpace(int nTempHead,int nTempTail)
	{
		if(nTempHead<=nTempTail)
		{
			return m_nSize-nTempTail+nTempHead-1;
		}
		else
		{
			return nTempHead-nTempTail-1;
		}
	}

	inline BYTE*       GetReadPtr(int nHead)
	{
		return m_pData+nHead;
	}

	inline BYTE*       GetWritePtr(int nTail)
	{
		return m_pData+nTail;
	}


	inline int GetReadableLen(int nTempHead,int nTempTail)
	{
		if(nTempHead==nTempTail)
		{
			return 0;
		}
		else if (nTempHead<nTempTail)
		{
			return nTempTail-nTempHead;
		}
		else
		{
			return m_nSize-nTempHead;
		}
	}


	inline int GetWritableLen(int nTempHead,int nTempTail)
	{
		//缓冲区满
		if((nTempTail+1)%m_nSize==nTempHead)
		{
			return 0;
		}
		else if(nTempHead<=nTempTail)
		{
			if(nTempHead==0)
				return m_nSize-nTempTail-1;
			else
				return m_nSize-nTempTail;
		}
		else
			return nTempHead - nTempTail -1;
	}

protected:
	BYTE				*m_pData;
	int					m_nSize;
	volatile int		m_nHead;
	volatile int		m_nTail;
	int					m_nExtraSize; 
};
