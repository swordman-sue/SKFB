//////////////////////////////////////////////////////////////////////////////////////
//	FileName		:	TsIniFile.cpp
//	FileAuthor		:	Kevin
//	FileCreateDate	:	2010/6/13
//	FileDescription	:	Ini文件的读取,调用自己的函数来实现
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "TsFile.h"
#include "TsIniFile.h"
#include "TsMemory.h"

#if defined(_DEBUG) && defined(_TESTMEMORY) && defined(_MSC_VER) 
#	if defined(_MFC_VER) 
#		define new DEBUG_NEW
#	else
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include "crtdbg.h"
#		define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	endif
#endif

//---------------------------------------------------------------------------
//  Function:   Constructor
//---------------------------------------------------------------------------
TMyIniFile::TMyIniFile()
{
	Init();	
}
//----------------------------------------------------------
// FUNCTION	: TMyIniFile::Init
// PURPOSE	: 
// RETURN	: void 
// COMMENTS	:
//----------------------------------------------------------
void TMyIniFile::Init()
{
	m_bBigSmall = FALSE;
	m_lStartSearch = 0;
	m_lReadPos	 = 0;
	m_dwMode	= 0;
	memset(m_FileName, 0, sizeof(m_FileName));
	memset(&m_Header, 0, sizeof(SECNODE));
}
//---------------------------------------------------------------------------
//  Function:   Destructor
//---------------------------------------------------------------------------
TMyIniFile::~TMyIniFile()
{
	ReleaseIniLink();
}
//----------------------------------------------------------
// FUNCTION	: TMyIniFile::Open
// PURPOSE	: 
// RETURN	: BOOL 
// ARGUMENT	: LPTSTR FileName 带有路径
// COMMENTS	: 加载INI文件
//----------------------------------------------------------
BOOL TMyIniFile::Open(LPTSTR FileName)
{	
	if (FileName[0] == 0)
		return FALSE;

	if (m_FileName[0])
		Close();
	
	//g_FuncGetFullPathName(m_FileName, FileName);
	lstrcpy(m_FileName, FileName);

	m_dwMode = 0;

	if (!m_File.Open(m_FileName))
		return FALSE;
	
/*	if (dwSize > 102400)
	{
		m_lReadPos = 102400;
		dwSize = m_lReadPos;
	}
*/
	TMemory		Buffer;
	DWORD		dwSize;

	dwSize = m_File.GetSize();

	if (!Buffer.Alloc(dwSize + 2))
		return FALSE;

	m_File.Read(Buffer.m_lpData, dwSize);
	
	m_lReadPos = CreateIniLink(Buffer.m_lpData, dwSize);	
		
	return TRUE;
}
//----------------------------------------------------------
// FUNCTION	: TMyIniFile::ReadData
// PURPOSE	: 
// RETURN	: BOOL 
// COMMENTS	:
//----------------------------------------------------------
BOOL TMyIniFile::ReadData()
{
	int nSize = 100000;
	TMemory		Buffer;
	
	if (m_dwMode == 1)
		return FALSE;

	if (nSize + m_lReadPos > (int)m_File.GetSize())
	{
		nSize = (int)m_File.GetSize() - m_lReadPos;
	}
	
	if (nSize <= 0)
		return FALSE;

	if (!Buffer.Alloc(nSize + 2))
		return FALSE;

	if (m_File.Read(Buffer.m_lpData, nSize) != (DWORD)nSize)
		return FALSE;
	
	m_lReadPos = CreateIniLink(Buffer.m_lpData, nSize);

	return TRUE;
}
//---------------------------------------------------------------------------
//  Function:   Close a Ini File
//	Comment:	关闭一个INI文件
//---------------------------------------------------------------------------
void TMyIniFile::Close()
{
	ReleaseIniLink();
	m_File.Close();
	Init();
}
//----------------------------------------------------------
// FUNCTION	: TMyIniFile::SetBigSmall
// PURPOSE	: 
// RETURN	: void 
// ARGUMENT	: BOOL bFlag
// COMMENTS	:
//----------------------------------------------------------
void TMyIniFile::SetBigSmall(BOOL bFlag)
{
	m_bBigSmall = bFlag;
}
//---------------------------------------------------------------------------
//  Function:   Erases all data from the INI file in memory
//	Comment:	清除一个INI文件的内容
//---------------------------------------------------------------------------
void TMyIniFile::Clear()
{
	ReleaseIniLink();
}
//---------------------------------------------------------------------------
//  Function:   Read line from buffer
//---------------------------------------------------------------------------
BOOL TMyIniFile::ReadLine(LPSTR Buffer,LONG Size)
{
	if (m_Offset >= Size)
	{
		return FALSE;
	}
	while (Buffer[m_Offset] != 0x0D)
	{
		m_Offset++;
		if (m_Offset >= Size)
			break;
	}
	Buffer[m_Offset] = 0;
	m_Offset += 2;
	return TRUE;
}
//---------------------------------------------------------------------------
//	Function:	Is it a key character
//---------------------------------------------------------------------------
BOOL TMyIniFile::IsKeyChar(CHAR ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return TRUE;
	if ((ch >= 'A') && (ch <= 'Z'))
		return TRUE;
	if ((ch >= 'a') && (ch <= 'z'))
		return TRUE;
	return FALSE;
}
//---------------------------------------------------------------------------
//  Function:   Create ini link
//---------------------------------------------------------------------------
LONG TMyIniFile::CreateIniLink(LPVOID Buffer,LONG Size)
{
	LPSTR lpBuffer = (LPSTR)Buffer;
	LPSTR lpString = NULL;
	LPSTR lpValue  = NULL;
	CHAR  szSection[32] = "[Default]";
	BOOL  bEnd = FALSE;
	LONG  lPos = m_lReadPos;

	m_Offset = 0;
	while (m_Offset < Size)
	{
		bEnd = FALSE;
		lpString = &lpBuffer[m_Offset];
		if (!ReadLine(lpBuffer, Size))
			break;
		if (IsKeyChar(*lpString))
		{
			lpValue = SplitKeyValue(lpString);
			SetKeyValue(szSection, lpString, lpValue);
			bEnd = TRUE;
			lPos += m_Offset;
		}
		if (*lpString == _T('['))
		{
			lstrcpynA(szSection, lpString, sizeof(szSection));
			bEnd = TRUE;
			lPos += m_Offset;
		}
	}
	return lPos;
}
//---------------------------------------------------------------------------
//  Function:   Release ini link
//---------------------------------------------------------------------------
void TMyIniFile::ReleaseIniLink()
{
	SECNODE* pThisSec = &m_Header;
	SECNODE* pNextSec = pThisSec->pNextNode;
	KEYNODE* pThisKey = NULL;
	KEYNODE* pNextKey = NULL;

	m_lStartSearch = 0;

	while (pNextSec != NULL)
	{
		pThisSec = pNextSec->pNextNode;
		pThisKey = &pNextSec->pKeyNode;
		pNextKey = pThisKey->pNextNode;
		while (pNextKey != NULL)
		{
			pThisKey = pNextKey->pNextNode;
			free(pNextKey->pKey);
			free(pNextKey->pValue);
			free(pNextKey);
			pNextKey = pThisKey;
		}
		free(pNextSec->pSection);
		free(pNextSec);
		pNextSec = pThisSec;
	}
	m_Header.pNextNode = NULL;
}
//---------------------------------------------------------------------------
//  Function:   Split Key Value
//---------------------------------------------------------------------------
LPSTR TMyIniFile::SplitKeyValue(LPCSTR pString)
{
	LPSTR pValue = (LPSTR)pString;
	while (*pValue)
	{
		if (*pValue == '=')
			break;
		pValue++;
	}
	*pValue = 0;
	return pValue + 1;
}
//---------------------------------------------------------------------------
//  Function:	Convert String to 32 bit ID
//---------------------------------------------------------------------------
DWORD TMyIniFile::String2Id(LPCSTR pString)
{
	DWORD Id = 0;
	for (int i=0; pString[i]; i++)
	{
		Id = (Id + (i+1) * pString[i]) % 0x8000000b * 0xffffffef;
	}
	return Id ^ 0x12345678;
}
//---------------------------------------------------------------------------
//  Function:   Erase a Section
//---------------------------------------------------------------------------
void TMyIniFile::EraseSection(LPCSTR pSection)
{
	// setup section name
	CHAR szSection[32] = "[";
	if (pSection[0] != '[')
	{
		lstrcatA(szSection, pSection);
		lstrcatA(szSection, "]");
	}
	else
	{
		lstrcpyA(szSection, pSection);
	}

	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	DWORD dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
		{
			break;
		}
		pSecNode = pSecNode->pNextNode;
	}

	// if no such section found
	if (pSecNode == NULL)
	{
		return;
	}

	// erase all key in the section
	KEYNODE* pThisKey = &pSecNode->pKeyNode;
	KEYNODE* pNextKey = pThisKey->pNextNode;
	while (pNextKey != NULL)
	{
		pThisKey = pNextKey->pNextNode;
		free(pNextKey->pKey);
		free(pNextKey->pValue);
		free(pNextKey);
		pNextKey = pThisKey;
	}
	pSecNode->pKeyNode.pNextNode = NULL;
}
//---------------------------------------------------------------------------
//  Function:   Set Value To a Key
//---------------------------------------------------------------------------
BOOL TMyIniFile::SetKeyValue(
	LPCSTR	pSection,
	LPCSTR	pKey,
	LPCSTR	pValue
	)
{
	int		nLen;
	DWORD	dwID;
	// setup section name
	CHAR szSection[32] = "[";
	if (pSection[0] != '[')
	{
		lstrcatA(szSection, pSection);
		lstrcatA(szSection,"]");
	}
	else
	{
		lstrcpyA(szSection, pSection);
	}

	// search for the matched section
	SECNODE* pThisSecNode = &m_Header;
	SECNODE* pNextSecNode = pThisSecNode->pNextNode;
	
	dwID = String2Id(szSection);
	while (pNextSecNode != NULL)
	{
		if (dwID == pNextSecNode->dwID)
		{
			break;
		}
		pThisSecNode = pNextSecNode;
		pNextSecNode = pThisSecNode->pNextNode;
	}

	// if no such section found create a new section
	if (pNextSecNode == NULL)
	{
		nLen = lstrlenA(szSection);
		pNextSecNode = (SECNODE *)calloc(1, sizeof(SECNODE));
		if (!pNextSecNode)
			return FALSE;
		pNextSecNode->pSection = (CHAR *)calloc(nLen + 2, 1);
		if (!pNextSecNode->pSection)
			return FALSE;
		memcpy(pNextSecNode->pSection, szSection, nLen);
		pNextSecNode->dwID = dwID;
		pNextSecNode->pKeyNode.pNextNode = NULL;
		pNextSecNode->pNextNode = NULL;
		pThisSecNode->pNextNode = pNextSecNode;
	}

	// search for the same key
	KEYNODE* pThisKeyNode = &pNextSecNode->pKeyNode;
	KEYNODE* pNextKeyNode = pThisKeyNode->pNextNode;

	if (m_bBigSmall)
	{
		CHAR szTemp[64];
		ZeroMemory(szTemp,sizeof(szTemp));
		lstrcpyA(szTemp,pKey);
		_strlwr_s(szTemp,64);
		dwID = String2Id(szTemp);
	}
	else
		dwID = String2Id(pKey);
	while (pNextKeyNode != NULL)
	{
		if (dwID == pNextKeyNode->dwID)
		{
			break;
		}
		pThisKeyNode = pNextKeyNode;
		pNextKeyNode = pThisKeyNode->pNextNode;
	}

	// if no such key found create a new key
	if (pNextKeyNode == NULL)
	{
		pNextKeyNode = (KEYNODE *)calloc(1, sizeof(KEYNODE));
		if (!pNextKeyNode)
			return FALSE;
		nLen = lstrlenA(pKey);
		pNextKeyNode->pKey = (CHAR *)calloc(nLen + 1, 1);
		if (!pNextKeyNode->pKey)
			return FALSE;
		
		memcpy(pNextKeyNode->pKey, pKey, nLen);		

		nLen = lstrlenA(pValue);
		pNextKeyNode->pValue = (CHAR *)calloc(nLen + 1, 1);
		if (!pNextKeyNode->pValue)
			return FALSE;
		memcpy(pNextKeyNode->pValue, pValue, nLen);

		pNextKeyNode->dwID = dwID;
		pNextKeyNode->pNextNode = NULL;
		pThisKeyNode->pNextNode = pNextKeyNode;
	}
	// replace the old value with new
	else
	{
		free(pNextKeyNode->pValue);
		nLen = lstrlenA(pValue);
		pNextKeyNode->pValue = (CHAR *)calloc(nLen + 1, 1);
		if (!pNextKeyNode->pValue)
			return FALSE;
		memcpy(pNextKeyNode->pValue, pValue, nLen);
	}
	return TRUE;
}
//---------------------------------------------------------------------------
//  Function:   Get Value
//---------------------------------------------------------------------------
BOOL TMyIniFile::GetKeyValue(
	LPCSTR	pSection,
	LPCSTR	pKey,
	LPSTR	pValue,
	DWORD	dwSize)
{
	DWORD	dwID;
	DWORD	dwSearch = m_lStartSearch;
	// setup section name
	CHAR szSection[80] = "[";


	if (pSection[0] != '[')
	{
		lstrcatA(szSection, pSection);
		lstrcatA(szSection, "]");
	}
	else
	{
		lstrcpyA(szSection, pSection);
	}

RETRY_LABEL:
	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
		{
			break;
		}
		pSecNode = pSecNode->pNextNode;
	}

	// if no such section founded
	if (pSecNode == NULL)
	{
		dwSearch ++;
		if (ReadData() == FALSE)
		{
			if (m_dwMode == 1)
				return FALSE;
			if (dwSearch == (DWORD)m_lStartSearch)
				return FALSE;
			else
			{
				dwSearch = 0;
				if (dwSearch == (DWORD)m_lStartSearch)
					return FALSE;
				m_File.Seek(0,FILE_BEGIN);
			}
		}
		goto RETRY_LABEL;
	}

	// search for the same key
	KEYNODE* pKeyNode = pSecNode->pKeyNode.pNextNode;
	dwID = String2Id(pKey);
	while (pKeyNode != NULL)
	{
		if (dwID == pKeyNode->dwID)
		{
			break;
		}
		pKeyNode = pKeyNode->pNextNode;
	}

	// if no such key found
	if (pKeyNode == NULL)
	{

		return FALSE;
	}

	// copy the value of the key
	lstrcpynA(pValue, pKeyNode->pValue, dwSize);
	m_lStartSearch = dwSearch;
	return TRUE;
}
//---------------------------------------------------------------------------
//  Function:   Get string in specify section and key
//	Comment:	从INI文件中读取一个字符串
//---------------------------------------------------------------------------
void TMyIniFile::GetStringW(
	LPCSTR lpSection,		// points to section name
	LPCSTR lpKeyName,		// points to key name
	LPCWSTR lpDefault,		// points to default string
	LPWSTR lpRString,		// points to destination buffer
	DWORD dwSize			// size of string buffer
	)
{
	CHAR *szTemp = new CHAR[dwSize+1];

	if (!GetKeyValue(lpSection, lpKeyName, szTemp, dwSize))
	{
		lstrcpynW(lpRString, lpDefault, dwSize);
	}
	else
	{
		MultiByteToWideChar(CP_ACP,0,szTemp,-1,lpRString,dwSize);
	}

	delete [] szTemp;
}
//---------------------------------------------------------------------------
//  Function:   Get string in specify section and key
//	Comment:	从INI文件中读取一个字符串
//---------------------------------------------------------------------------
void TMyIniFile::GetString(
	LPCSTR lpSection,		// points to section name
	LPCSTR lpKeyName,		// points to key name
	LPCSTR lpDefault,		// points to default string
	LPSTR lpRString,		// points to destination buffer
	DWORD dwSize			// size of string buffer
	)
{
	if (!GetKeyValue(lpSection, lpKeyName, lpRString, dwSize))
	{
		lstrcpynA(lpRString, lpDefault, dwSize);
	}
}
//---------------------------------------------------------------------------
//  Function:   Get integer in specify section and key
//	Comment:	从INI文件中读取一个整数(>0)
//---------------------------------------------------------------------------
void TMyIniFile::GetInteger(
	LPCSTR lpSection,		// points to section name
	LPCSTR lpKeyName,		// points to key name
	int   nDefault,			// default value
	int   *Value            // points to value
	)
{
	CHAR Buffer[80];

	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		*Value = atoi(Buffer);
	}
	else
	{
		*Value = nDefault;
	}
}
//---------------------------------------------------------------------------
//  Function:   Get integer in specify section and key
//	Comment:	从INI文件中读取一个整数(>0)
//---------------------------------------------------------------------------
void TMyIniFile::GetDouble(
	LPCSTR lpSection,		// points to section name
	LPCSTR lpKeyName,		// points to key name
	double   fDefault,			// default value
	double   *Value            // points to value
	)
{
	CHAR Buffer[80];

	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		*Value = atof(Buffer);
	}
	else
	{
		*Value = fDefault;
	}
}
//---------------------------------------------------------------------------
//  Function:   Get integer in specify section and key
//	Comment:	从INI文件中读取一个整数(>0)
//---------------------------------------------------------------------------
void TMyIniFile::GetFloat(
	LPCSTR lpSection,		// points to section name
	LPCSTR lpKeyName,		// points to key name
	float   fDefault,			// default value
	float   *Value            // points to value
	)
{
	CHAR Buffer[80];

	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		*Value = (float)atof(Buffer);
	}
	else
	{
		*Value = fDefault;
	}
}
//---------------------------------------------------------------------------
//  Function:   Get struct from ini file
//	Comment:	从INI文件中读取一个结构
//---------------------------------------------------------------------------
void TMyIniFile::GetStruct(
	LPCSTR	lpSection,		// pointer to section name
	LPCSTR	lpKeyName,		// pointer to key name
	LPVOID	lpStruct,		// pointer to buffer that contains data to add
	DWORD	dwSize			// size, in bytes, of the buffer
	)
{
	CHAR    Buffer[512];
	LPBYTE	lpByte;
	BYTE	ah,al;

	if (!GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
		return;

	lpByte = (LPBYTE)lpStruct;
	int len = lstrlenA(Buffer);
	if (len / 2 != (int)dwSize)
		return;
	for (int i = 0; i < len; i += 2)
	{
		// get byte high
		ah = Buffer[i];
		if ((ah >= 48) && (ah <= 57))
			ah = (BYTE)(ah - 48);
		else
			ah = (BYTE)(ah - 65 + 10);
		// get byte low
		al = Buffer[i+1];
		if ((al >= 48) && (al <= 57))
			al = (BYTE)(al - 48);
		else
			al = (BYTE)(al - 65 + 10);
		// set struct bye
		*lpByte++ = (BYTE)(ah * 16 + al);
	}
}

//---------------------------------------------------------------------------
//  Function:   Write string to ini file
//	Comment:	写一个字符串到INI文件
//---------------------------------------------------------------------------
void TMyIniFile::WriteString(
	LPCSTR	lpSection,		// pointer to section name
	LPCSTR	lpKeyName,		// pointer to key name
	LPCSTR	lpString		// pointer to string to add
	)
{
	SetKeyValue(lpSection, lpKeyName, lpString);
}
//---------------------------------------------------------------------------
//  Function:   Write integer to ini file
//	Comment:	写一个整数到INI文件
//---------------------------------------------------------------------------
void TMyIniFile::WriteInteger(
	LPCSTR	lpSection,		// pointer to section name
	LPCSTR	lpKeyName,		// pointer to key name
	int 	Value			// Integer to write
	)
{
	CHAR Buffer[32];

	wsprintfA(Buffer,"%d",Value);
	SetKeyValue(lpSection, lpKeyName, Buffer);
}
void TMyIniFile::WriteDouble(
	LPCSTR	lpSection,		// pointer to section name
	LPCSTR	lpKeyName,		// pointer to key name
	double 	Value			// Integer to write
	)
{
	CHAR Buffer[32];

	sprintf_s(Buffer,32,"%f",Value);
	SetKeyValue(lpSection, lpKeyName, Buffer);
}
//---------------------------------------------------------------------------
//  Function:   Write struct to ini file
//	Comment:	写一个结构到INI文件
//---------------------------------------------------------------------------
void TMyIniFile::WriteStruct(
	LPCSTR	lpSection,		// pointer to section name
	LPCSTR	lpKeyName,		// pointer to key name
	LPVOID	lpStruct,		// pointer to buffer that contains data to add
	DWORD 	dwSize			// size, in bytes, of the buffer
	)
{
	CHAR    Buffer[512];
	LPSTR	lpBuff = Buffer;
	LPBYTE	lpByte;

	if (dwSize * 2 >= 512)
	{
		return;
	}
	lpByte = (LPBYTE) lpStruct;
	for (DWORD i=0; i<dwSize; i++)
	{
		wsprintfA(lpBuff,"%02x",*lpByte);
		lpBuff++;
		lpBuff++;
		lpByte++;
	}
	_strupr_s(Buffer,512);
	SetKeyValue(lpSection, lpKeyName, Buffer);
}
//----------------------------------------------------------
// FUNCTION	: TMyIniFile::New
// PURPOSE	: 
// RETURN	: BOOL 
// ARGUMENT	: LPTSTR FileName 带有路径
// COMMENTS	: 创建一个新的INI文件
//----------------------------------------------------------
BOOL TMyIniFile::New(LPCTSTR FileName)
{
	if (FileName[0] == 0)
		return FALSE;

	if (m_FileName[0])
		Close();

	lstrcpy(m_FileName,FileName);

	return TRUE;
}
// ---------------------------------------------------------------------------
/// Function name          : TMyIniFile::Save
/// \return Return type    : BOOL 
/// \param Argument        : void
/// Description            : 
// ---------------------------------------------------------------------------
BOOL TMyIniFile::Save(void)
{
// 	CFileOperate File;
// 	char szBuf[256];
// 
// 
// 	m_File.Close();
// 	if (!File.Create(m_FileName))
// 	{
// 		return FALSE;
// 	}
// 	SECNODE* SecNode = m_Header.pNextNode;
// 	KEYNODE* KeyNode = NULL;
// 	while (SecNode != NULL)
// 	{
// 		wsprintf(szBuf,_T("%s%c%c"), SecNode->pSection, 0x0D, 0x0A);
// 		File.Write(szBuf,lstrlen(szBuf));
// 		KeyNode = SecNode->pKeyNode.pNextNode;
// 		while (KeyNode != NULL)
// 		{
// 			wsprintf(szBuf,_T("%s%c%s%c%c"), KeyNode->pKey, '=',
// 					KeyNode->pValue, 0x0D, 0x0A);
// 			File.Write(szBuf,lstrlen(szBuf));
// 			KeyNode = KeyNode->pNextNode;
// 		}
// 		SecNode = SecNode->pNextNode;
// 		wsprintf(szBuf,_T("%c%c"), 0x0D, 0x0A);
// 		File.Write(szBuf,lstrlen(szBuf));			
// 	}
// 	File.Close();
	return TRUE;
}
//-----------------------------------------------------
// Function name   : TMyIniFile::GetData
// Return type     : BOOL 
// Argument        : LPTSTR pSection
// Argument        : LPTSTR pKey
// Argument        : LPTSTR pValue
// Argument        : DWORD dwSize
// Description     : 
//-----------------------------------------------------
BOOL TMyIniFile::GetData(LPCSTR pSection,LPCSTR pKey,LPVOID pValue,DWORD dwSize)
{
	DWORD	dwID;
	DWORD	dwSearch = m_lStartSearch;
	// setup section name
	CHAR szSection[80] = "[";


	if (pSection[0] != '[')
	{
		lstrcatA(szSection, pSection);
		lstrcatA(szSection, "]");
	}
	else
	{
		lstrcpyA(szSection, pSection);
	}

RETRY_LABEL:
	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
		{
			break;
		}
		pSecNode = pSecNode->pNextNode;
	}

	// if no such section founded
	if (pSecNode == NULL)
	{
		dwSearch ++;
		if (ReadData() == FALSE)
		{
			if (m_dwMode == 1)
				return FALSE;
			if (dwSearch == (DWORD)m_lStartSearch)
				return FALSE;
			else
			{
				dwSearch = 0;
				if (dwSearch == (DWORD)m_lStartSearch)
					return FALSE;
				m_File.Seek(0,FILE_BEGIN);
			}
		}
		goto RETRY_LABEL;
	}

	// search for the same key
	KEYNODE* pKeyNode = pSecNode->pKeyNode.pNextNode;
	dwID = String2Id(pKey);
	while (pKeyNode != NULL)
	{
		if (dwID == pKeyNode->dwID)
		{
			break;
		}
		pKeyNode = pKeyNode->pNextNode;
	}

	// if no such key found
	if (pKeyNode == NULL)
	{

		return FALSE;
	}

	// copy the value of the key
	CopyMemory(pValue, pKeyNode->pValue, dwSize);
	m_lStartSearch = dwSearch;
	return TRUE;
}