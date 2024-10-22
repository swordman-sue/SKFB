#ifndef PACKETSTRUCT_CODE_H_
#define PACKETSTRUCT_CODE_H_

#include "DataDefine.h"

#define ntohll(x) (((unsigned long long)ntohl((unsigned long)x)) << 32) + ntohl((unsigned long)(x >> 32)) 
#define htonll(x) (((unsigned long long)htonl((unsigned long)x)) << 32) + htonl((unsigned long)(x >> 32))

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeChar( unsigned char** pstrEncode, unsigned char ucSrc )
{
	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	**pstrEncode = ( unsigned char )ucSrc;
	(*pstrEncode)++;

	return ( int )sizeof( unsigned char );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeChar( unsigned char** pstrDecode, unsigned char *pucDest )
{
	if( pstrDecode == 0 || *pstrDecode == 0 || pucDest == 0 )
	{
		return 0;
	}

	*pucDest = **pstrDecode;
	(*pstrDecode)++;

	return ( int )sizeof( unsigned char );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeShort( unsigned char** pstrEncode, unsigned short usSrc )
{
	unsigned short usTemp;

	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	// modify by Kevin
	usTemp = usSrc;
	//usTemp = htons( usSrc );

	memcpy( ( void* )( *pstrEncode ), ( const void* )&usTemp, sizeof( unsigned short ) );
	*pstrEncode += sizeof( unsigned short );

	return ( int )sizeof( unsigned short );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeShort( unsigned char** pstrDecode, unsigned short *pusDest )
{
	unsigned short usTemp;

	if( pstrDecode == 0 || *pstrDecode == 0 || pusDest == 0 )
	{
		return 0;
	}

	memcpy( ( void* )&usTemp, ( const void* )( *pstrDecode ), sizeof( unsigned short ) );
	*pstrDecode += sizeof(unsigned short);

	// modify by Kevin
	*pusDest = usTemp;
	//*pusDest = ntohs( usTemp );

	return ( int )sizeof( unsigned short );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeLong( unsigned char** pstrEncode, unsigned long ulSrc )
{
	unsigned long ulTemp;

	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	// modify by Kevin
	ulTemp = ulSrc;
	//ulTemp = htonl( ulSrc );

	memcpy( ( void* )( *pstrEncode ), ( const void* )&ulTemp, sizeof( unsigned long ) );
	*pstrEncode += sizeof( unsigned long );

	return ( int )sizeof( unsigned long );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeLong( unsigned char** pstrDecode, unsigned long *pulDest )
{
	unsigned long ulTemp;

	if( pstrDecode == NULL || *pstrDecode == NULL || pulDest == NULL)
	{
		return 0;
	}

	memcpy( ( void* )&ulTemp, ( const void* )( *pstrDecode ), sizeof( unsigned long ) );
	*pstrDecode += sizeof( unsigned long );

	// modify by Kevin
	*pulDest = ulTemp;
	//*pulDest = ntohl( ulTemp );

	return ( int )sizeof( unsigned long );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeLongLong( unsigned char** pstrEncode, unsigned long long ulSrc )
{
	unsigned long long ulTemp;

	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	// modify by Kevin
	ulTemp = ulSrc;
	//ulTemp = htonll( ulSrc );

	memcpy( ( void* )( *pstrEncode ), ( const void* )&ulTemp, sizeof( unsigned long long ) );
	*pstrEncode += sizeof( unsigned long long );

	return ( int )sizeof( unsigned long long );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeLongLong( unsigned char** pstrDecode, unsigned long long *pulDest )
{
	unsigned long long ulTemp;

	if( pstrDecode == NULL || *pstrDecode == NULL || pulDest == NULL)
	{
		return 0;
	}

	memcpy( ( void* )&ulTemp, ( const void* )( *pstrDecode ), sizeof( unsigned long long ) );
	*pstrDecode += sizeof( unsigned long long );

	// modify by Kevin
	*pulDest = ulTemp;
	//*pulDest = ntohll( ulTemp );

	return ( int )sizeof( unsigned long long );
}


/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeInt( unsigned char** pstrEncode, unsigned int uiSrc )
{
	unsigned int uiTemp;

	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	// modify by Kevin
	uiTemp = uiSrc;
	//uiTemp = htonl( uiSrc );

	memcpy( ( void* )( *pstrEncode ), ( const void* )&uiTemp, sizeof( unsigned int ) );
	*pstrEncode += sizeof( unsigned int );

	return ( int )sizeof( unsigned int );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeInt( unsigned char** pstrDecode, unsigned int *puiDest )
{
	unsigned int uiTemp;

	if( pstrDecode == 0 || *pstrDecode == 0 || puiDest == 0 )
	{
		return 0;
	}

	memcpy( ( void* )&uiTemp, ( const void* )( *pstrDecode ), sizeof( unsigned int ) );
	*pstrDecode += sizeof( unsigned int );

	// modify by Kevin
	*puiDest = uiTemp;
	//*puiDest = ntohl( uiTemp );

	return ( int )sizeof( unsigned int );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeFloat( unsigned char** pstrEncode, float fSrc )
{
	if( pstrEncode == 0 || *pstrEncode == 0 )
	{
		return 0;
	}

	unsigned int *puiTemp = (unsigned int *)&fSrc;

	// modify by Kevin
	unsigned int uiTemp =  *puiTemp;
	//unsigned int uiTemp =  htonl(*puiTemp);

	memcpy( ( void* )( *pstrEncode ), ( const void* )&uiTemp, sizeof( float ) );
	*pstrEncode += sizeof( float );

	return ( int )sizeof( float );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeDouble(unsigned char** pstrEncode, double fSrc)
{
	if (pstrEncode == 0 || *pstrEncode == 0)
	{
		return 0;
	}

	//unsigned int *puiTemp = (unsigned int *)&fSrc;

	// modify by Kevin
	//unsigned int uiTemp = *puiTemp;
	//unsigned int uiTemp =  htonl(*puiTemp);

	memcpy((void*)(*pstrEncode), (const void*)&fSrc, sizeof(double));
	*pstrEncode += sizeof(double);

	return (int)sizeof(double);
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeFloat( unsigned char** pstrDecode, float *pfDest )
{
	if( pstrDecode == 0 || *pstrDecode == 0 || pfDest == 0 )
	{
		return 0;
	}
	
	memcpy( ( void* )pfDest, ( const void* )( *pstrDecode ), sizeof( float ) );
	*pstrDecode += sizeof( float );

	// modify by Kevin
	unsigned int uiTemp = *((unsigned int*)pfDest);
	//unsigned int uiTemp = ntohl(*((unsigned int*)pfDest));
	memcpy((void*)pfDest,(const void*)&uiTemp,sizeof(float));

	return ( int )sizeof( float );
}

inline int DecodeDouble(unsigned char** pstrDecode, double *pfDest)
{
	if (pstrDecode == 0 || *pstrDecode == 0 || pfDest == 0)
	{
		return 0;
	}

	memcpy((void*)pfDest, (const void*)(*pstrDecode), sizeof(double));
	*pstrDecode += sizeof(double);

	// modify by Kevin
	//unsigned int uiTemp = *((unsigned int*)pfDest);
	//unsigned int uiTemp = ntohl(*((unsigned int*)pfDest));
	//memcpy((void*)pfDest, (const void*)&uiTemp, sizeof(double));

	return (int)sizeof(double);
}
/*---------------------------------------------------------------------------------------------------------*/

inline int EncodeString( unsigned char** pstrEncode, const char* strSrc, short sMaxStrLength)
{
	unsigned short usTemp;
	unsigned short usTempLength;

	if( pstrEncode == 0 || *pstrEncode == 0 || strSrc == 0 || sMaxStrLength <= 0 )
	{
		return 0;
	}

	// 防止实际的字符串长度超过指定的长度
	usTempLength = ( unsigned short )strlen( strSrc );
	if( usTempLength > sMaxStrLength )
	{
		usTempLength = sMaxStrLength;
	}

	// modify by Kevin
	usTemp = usTempLength;
	//usTemp = htons( usTempLength );

	memcpy( ( void* )( *pstrEncode ), ( const void* )&usTemp, sizeof( unsigned short ) );
	*pstrEncode += sizeof( unsigned short );

	memcpy( ( void* )( *pstrEncode ), ( const void* )strSrc, usTempLength );
	*pstrEncode += usTempLength;

	return ( int )( usTempLength + sizeof( unsigned short ) );
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeString( unsigned char** pstrDecode, char* strDest, short sMaxStrLength, int buffer_length )
{
	// 变量说明:
	// sMaxStrLength	要接收内容的字符串长度上限
	// buffer_length	消息内容当前还剩下的长度
	// usTempLength     协议里指定本字段内容的长度


	unsigned short usTemp;
	unsigned short usTempLength;
	unsigned short usRealLength;

	if(pstrDecode == 0 || *pstrDecode == 0 || strDest == 0 || sMaxStrLength <= 0 )
	{
		return 0;
	}

	// add by Kevin 
	memset(strDest, 0, sMaxStrLength);

	memcpy( ( void* )&usTemp, ( const void* )( *pstrDecode ), sizeof( unsigned short ) );

	// modify by Kevin
	usTempLength = usTemp;
	//usTempLength = ntohs( usTemp );

	if (int(usTempLength + sizeof( unsigned short )) <= buffer_length)
	{
		if( usTempLength > sMaxStrLength )
		{
			usRealLength = sMaxStrLength;
		}
		else
		{
			usRealLength = usTempLength;
		}

		*pstrDecode += sizeof( unsigned short );
		memcpy( ( void* )strDest, ( const void* )( *pstrDecode ), usRealLength );
		*pstrDecode += usTempLength;

		// 如果整个字符串被写满时, 则强行末尾加结束符
		if (usRealLength == sMaxStrLength)
			strDest[usRealLength-1] = '\0';

		return ( int )( usTempLength + sizeof( unsigned short ) );
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
inline int EncodeMemory( unsigned char** pstrEncode, const char *pcSrc, short sMemSize)
{
	if(pstrEncode == 0 || *pstrEncode == 0 || pcSrc == 0 || sMemSize <= 0 )
	{
		return 0;
	}
	memcpy( ( void* )( *pstrEncode ), ( const void* )pcSrc, sMemSize );
	*pstrEncode += sMemSize;
	return sMemSize;
}

/*---------------------------------------------------------------------------------------------------------*/
inline int DecodeMemory( unsigned char** pstrDecode, char *pcDest, short sMemSize, int buffer_length)
{
	if(pstrDecode == 0 || *pstrDecode == 0 || pcDest == 0 || sMemSize <= 0 )
	{
		return 0;
	}

	if (sMemSize <= buffer_length)
	{
		memcpy( ( void* )pcDest, ( const void* )( *pstrDecode ), sMemSize );
		*pstrDecode += sMemSize;
		return sMemSize;
	}
	return 0;
}

inline int EncodeDateTime( unsigned char ** pstrEncode, const DATETIME& dateTime )
{
	if (pstrEncode != NULL && *pstrEncode != NULL)
	{
		int bytes = EncodeShort( pstrEncode, dateTime.year );
		bytes += EncodeChar( pstrEncode, dateTime.month );
		bytes += EncodeChar( pstrEncode, dateTime.day );
		bytes += EncodeChar( pstrEncode, dateTime.hour );
		bytes += EncodeChar( pstrEncode, dateTime.minute );
		bytes += EncodeChar( pstrEncode, dateTime.second );
		return bytes;
	}
	else
	{
		return -1;
	}
}

inline int DecodeDateTime( unsigned char ** pstrDecode, DATETIME& dateTime, int buffer_length)
{
	if (pstrDecode != NULL && *pstrDecode != NULL)
	{
		int bytes = DecodeShort( pstrDecode, (unsigned short*)&dateTime.year );
		bytes += DecodeChar( pstrDecode, (unsigned char*)&dateTime.month );
		bytes += DecodeChar( pstrDecode, (unsigned char*)&dateTime.day );
		bytes += DecodeChar( pstrDecode, (unsigned char*)&dateTime.hour );
		bytes += DecodeChar( pstrDecode, (unsigned char*)&dateTime.minute );
		bytes += DecodeChar( pstrDecode, (unsigned char*)&dateTime.second );
		return bytes;
	}
	else
	{
		return -1;
	}
}

/*---------------------------------------------------------------------------------------------------------*/

const int NetMsgEncodeMsgError = -300;
const int NetMsgDecodeMsgError = -400;

class NetMsgBody
{
public:
	NetMsgBody(){}
	//virtual ~NetMsgBody(){};
	virtual int EncodeMsg( unsigned char* pOutBuffer, short& nBufLength ) = 0;
	virtual int DecodeMsg( const unsigned char* pInBuffer, short nBufLength ) = 0;
	virtual int GetType() = 0;
	virtual const char* GetClassName() = 0;
};

class NetMsgHeadHandler
{
public:
	static int EncodeServerHead(DWORD dwGUID, DWORD dwSeriesId, WORD wMsgID, unsigned char* pOutBuffer, short& nBufLength)
	{
		if ( !pOutBuffer )
		{
			return -1;
		}

		unsigned char* pMsg = pOutBuffer;
		short nMsgLength = 0;

		nBufLength = 0;

		nMsgLength = short(EncodeInt( &pMsg, ( unsigned int )dwGUID ));
		nBufLength +=nMsgLength;

		nMsgLength = short(EncodeInt(&pMsg, (unsigned int)dwSeriesId));
		nBufLength += nMsgLength;

		nMsgLength = short(EncodeShort(&pMsg, ( unsigned short )wMsgID ));
		nBufLength +=nMsgLength;

		return 0;
	}

	static int DecodeServerHead(const unsigned char* pInBuffer, short nBufLength, DWORD& dwGUID, DWORD &dwSeriesId, WORD& wMsgID)
	{
		if ( !pInBuffer || nBufLength <= 0 )
		{
			return -1;
		}

		short nMsgLength = nBufLength;
		unsigned char* pMsg = const_cast<unsigned char*>(pInBuffer);

		nMsgLength -= short(DecodeInt( &pMsg, (unsigned int *)&dwGUID ));

		if( nMsgLength < 0 )
		{
			return -1;
		}

		nMsgLength -= short(DecodeInt(&pMsg, (unsigned int *)&dwSeriesId));

		if (nMsgLength < 0)
		{
			return -1;
		}

		nMsgLength -= short(DecodeShort( &pMsg, (unsigned short *)&wMsgID ));

		if( nMsgLength < 0 )
		{
			return -1;
		}

		return 0;
	}

	static int EncodeClientHead(WORD dwMsgID, DWORD dwSeriesId, unsigned char* pOutBuffer, short& nBufLength)
	{
		if ( !pOutBuffer )
		{
			return -1;
		}

		unsigned char* pMsg = pOutBuffer;
		short nMsgLength = 0;

		nBufLength = 0;

		nMsgLength = short(EncodeInt(&pMsg, (unsigned short)dwSeriesId));
		nBufLength +=nMsgLength;

		nMsgLength = short(EncodeShort(&pMsg, (unsigned short)dwMsgID));
		nBufLength += nMsgLength;

		return 0;
	}

	static int DecodeClientHead(const unsigned char* pInBuffer, short nBufLength, DWORD& dwSeriesId, WORD& dwMsgID)
	{
		if ( !pInBuffer || nBufLength <= 0 )
		{
			return -1;
		}

		short nMsgLength = nBufLength;
		unsigned char* pMsg = const_cast<unsigned char*>(pInBuffer);

		nMsgLength -= short(DecodeInt(&pMsg, (unsigned int *)&dwSeriesId));
		if (nMsgLength < 0)
		{
			return -1;
		}

		nMsgLength -= short(DecodeShort( &pMsg, (unsigned short *)&dwMsgID ));
		if( nMsgLength < 0 )
		{
			return -1;
		}

		return 0;
	}
};

/*---------------------------------------------------------------------------------------------------------*/

#define CLASS_NAME(X) #X

#define Declare_Msg_Begin( _MsgId, _MsgClassName )						\
class _MsgClassName : public NetMsgBody									\
{																		\
public:																	\
	static const char* GetClassName1()									\
{																		\
	static string strClassName = CLASS_NAME(_MsgClassName);				\
	return strClassName.data();											\
}																		\
	virtual const char* GetClassName()									\
{																		\
	return GetClassName1();												\
}																		\
	virtual int GetType()												\
{																		\
	return _MsgId;														\
}												


//	_MsgClassName() {}														
//	~_MsgClassName() {}

//////////////////////// 解码相关接口 ///////////////////////////

#define	Declare_DecodeMsg_Function_Begin												\
	virtual int DecodeMsg( const unsigned char* pInBuffer, short nBufLen )		\
{																			\
	if ( !pInBuffer || nBufLen <= 0 )										\
{																		\
	return -1;															\
}																		\
	short nMsgLength = nBufLen;												\
	unsigned char* pMsg = ( unsigned char* )pInBuffer;						\
	short sTempLength = 0;														

#define	Declare_DecodeMsg_Empty_Function_Begin												\
	virtual int DecodeMsg( const unsigned char* pInBuffer, short nBufLen )		\
{

#define Declare_DecodeMsg_Memory( _pMem, _nMemLen )										\
	sTempLength = ( short )DecodeMemory( &pMsg, ( char* )&_pMem, _nMemLen, nMsgLength);	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Short( _sShort )												\
	sTempLength = ( short )DecodeShort( &pMsg, ( unsigned short* )&_sShort );\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_UnsignedShort( _usShort )										\
	sTempLength = ( short )DecodeShort( &pMsg, ( unsigned short* )&_usShort );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_String( _szString, _nStrLen )									\
	sTempLength = ( short )DecodeString( &pMsg, _szString, _nStrLen, nMsgLength );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Int( nVal )													\
	sTempLength = ( short )DecodeInt( &pMsg, ( unsigned int* )&nVal );		\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_UnsignedInt( unVal )											\
	sTempLength = ( short )DecodeInt( &pMsg, ( unsigned int* )&unVal );		\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Char( cVal )													\
	sTempLength = ( short )DecodeChar( &pMsg, ( unsigned char* )&cVal );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_UnsignedChar( ucVal )											\
	sTempLength = ( short )DecodeChar( &pMsg, ( unsigned char* )&ucVal );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Long( nlVal )													\
	sTempLength = ( short )DecodeLong( &pMsg, ( unsigned long* )&nlVal );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_UnsignedLongLong( unVal )												\
	sTempLength = ( short )DecodeLongLong( &pMsg, ( unsigned long long* )&unVal );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Float( fVal )										\
	sTempLength = ( short )DecodeFloat( &pMsg, ( float* )&fVal );	\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Double( fVal )										\
	sTempLength = (short)DecodeDouble(&pMsg, (double*)&fVal);	\
	nMsgLength -= sTempLength;


#define Declare_DecodeMsg_DateTime( dtVal )												\
	sTempLength = ( short )DecodeDateTime( &pMsg, dtVal, nMsgLength );		\
	nMsgLength -= sTempLength;

#define Declare_DecodeMsg_Function_End													\
	if ( nMsgLength < 0 )													\
{																		\
	return -1;															\
}																		\
	return 0;																\
}

#define Declare_DecodeMsg_Empty_Function_End													\
	return 0;}


/////////////////////////// 编码相关接口 //////////////////////////

#define Declare_EncodeMsg_Function_Begin													\
	virtual int EncodeMsg( unsigned char* pOutBuffer, short& nBufLength )		\
{																			\
	if ( !pOutBuffer )														\
{																		\
	return -1;															\
}																		\
	unsigned char* pMsg = pOutBuffer;										\
	nBufLength = 0;															\
	short nMsgLength;

#define Declare_EncodeMsg_Empty_Function_Begin													\
	virtual int EncodeMsg( unsigned char* pOutBuffer, short& nBufLength )		\
{

#define Declare_EncodeMsg_Memory( _pMem, _nMemLen )										\
	nMsgLength = ( short )EncodeMemory( &pMsg, ( char* )&_pMem, _nMemLen );	\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_Short( _sShort )												\
	nMsgLength = ( short )EncodeShort( &pMsg, ( unsigned short )_sShort );	\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_UnsignedShort( _usShort )										\
	nMsgLength = ( short )EncodeShort( &pMsg, ( unsigned short )_usShort );	\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_String( _szString, _nStrLen )									\
	nMsgLength = ( short )EncodeString( &pMsg, _szString, _nStrLen - 1 );	\
	nBufLength += nMsgLength;

#define Declare_EncodeMsg_Int( nVal )													\
	nMsgLength = ( short )EncodeInt( &pMsg, ( unsigned int )nVal );			\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_UnsignedInt( unVal )											\
	nMsgLength = ( short )EncodeInt( &pMsg, ( unsigned int )unVal );		\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_Char( cVal )													\
	nMsgLength = ( short )EncodeChar( &pMsg, ( unsigned char )cVal );		\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_UnsignedChar( ucVal )											\
	nMsgLength = ( short )EncodeChar( &pMsg, ( unsigned char )ucVal );		\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_Long( nlVal )													\
	nMsgLength = ( short )EncodeLong( &pMsg, ( unsigned long )nlVal );		\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_UnsignedLongLong( unVal )											\
	nMsgLength = ( short )EncodeLongLong( &pMsg, ( unsigned long long )unVal );	\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_Float( fVal )													\
	nMsgLength = ( short )EncodeFloat( &pMsg, ( float )fVal );			\
	nBufLength += nMsgLength;

#define Declare_EncodeMsg_Double( fVal )													\
	nMsgLength = (short)EncodeDouble(&pMsg, (double)fVal);			\
	nBufLength += nMsgLength;

#define Declare_EncodeMsg_DateTime( dtVal )												\
	nMsgLength = ( short )EncodeDateTime( &pMsg, dtVal );					\
	nBufLength += nMsgLength; 

#define Declare_EncodeMsg_Function_End													\
	return 0; }

#define Declare_EncodeMsg_Empty_Function_End													\
	return 0; }

#define Declare_Msg_Member( _sType, _sValue )											\
	_sType		_sValue;

#define Declare_Msg_Member_Array( _sType, _sValue, _sCount )							\
	_sType		_sValue[ _sCount ];

#define Declare_Msg_Member_Array_2( _sType, _sValue, _sCount,  _sCount2)				\
	_sType		_sValue[ _sCount ][ _sCount2 ];

#define Declare_Msg_Member_Array_3( _sType, _sValue, _sCount,  _sCount2,  _sCount3)		\
	_sType		_sValue[ _sCount ][ _sCount2 ][ _sCount3 ];

#define Declare_Msg_End };

#endif
