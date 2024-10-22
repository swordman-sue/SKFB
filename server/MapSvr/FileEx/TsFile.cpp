////////////////////////////////////////////////////////////////////////////////
//	Filename     :TsFile.cpp
//	Created by   :Kevin
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TsFile.h"

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
BOOL g_bRealPath = FALSE;

//----------------------------------------------------------
// FUNCTION	: CFileOperate::CFileOperate
// PURPOSE	: 
// RETURN	: 
// COMMENTS	:
//----------------------------------------------------------
CFileOperate::CFileOperate()
{
   m_hFile = INVALID_HANDLE_VALUE;
   m_dwSize = 0;
   m_dwPosition = 0;
}
//----------------------------------------------------------
// FUNCTION	: CFileOperate::~CFileOperate
// PURPOSE	: 
// RETURN	: 
// COMMENTS	:
//----------------------------------------------------------
CFileOperate::~CFileOperate()
{
	Close();
}
//---------------------------------------------------------------------------
//  Function:   Close file
//  Comment:	关闭已经打开的文件
//---------------------------------------------------------------------------
BOOL CFileOperate::Close()
{
	// check file handle
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	// close file handle
	CloseHandle(m_hFile);
	// reset file param
	m_hFile = INVALID_HANDLE_VALUE;
	m_dwSize = 0;
	m_dwPosition = 0;
	// return ture
	return TRUE;
}
//-----------------------------------------
//  Function:   Open exist file
//  Commemt:	打开一个文件，准备读操作
//-----------------------------------------
BOOL CFileOperate::Open(LPCTSTR FileName,
						DWORD dwMode /* = GENERIC_READ */
						)
{

	if (m_hFile != INVALID_HANDLE_VALUE)
		Close();
	
	m_hFile = CreateFile(
					FileName,		// pointer to name of the file with path
					dwMode,	// access (read-write) mode
					FILE_SHARE_READ,// share mode
					NULL,			// pointer to security attributes
					OPEN_EXISTING,	// how to create
					FILE_ATTRIBUTE_NORMAL,// file attributes
					NULL);			// template file
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	m_dwSize = GetFileSize(m_hFile, NULL);
	m_dwPosition = 0;
	return TRUE;
}
//-----------------------------------------
//  Function:   Open exist file
//  Commemt:	打开一个文件，准备读操作
//-----------------------------------------
BOOL CFileOperate::OpenA(LPCSTR FileName,
						DWORD dwMode /* = GENERIC_READ */
						)
{

	if (m_hFile != INVALID_HANDLE_VALUE)
		Close();
	
	m_hFile = CreateFileA(
					FileName,		// pointer to name of the file with path
					dwMode,	// access (read-write) mode
					FILE_SHARE_READ,// share mode
					NULL,			// pointer to security attributes
					OPEN_EXISTING,	// how to create
					FILE_ATTRIBUTE_NORMAL,// file attributes
					NULL);			// template file
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
//#ifdef _DEBUG
//		DebugMsgOutA(CON_ERROR,"CFileOperate::OpenA(%s) failed! err = %u",FileName,GetLastError());
//#endif
		return FALSE;
	}
	m_dwSize = GetFileSize(m_hFile, NULL);
	m_dwPosition = 0;
	return TRUE;
}
//----------------------------------------
//  Function:   Create new file
//  Comment:	创建一个新文件，准备写操作
//----------------------------------------
BOOL CFileOperate::Create(LPCTSTR FileName,DWORD dwMode)

{
	if (m_hFile != INVALID_HANDLE_VALUE)
		Close();

	m_hFile = CreateFile(
					FileName,		// pointer to name of the file with path
					GENERIC_WRITE|GENERIC_READ,	// access (read-write) mode
					FILE_SHARE_READ,// share mode
					NULL,			// pointer to security attributes
					dwMode,			// create or over write
					FILE_ATTRIBUTE_NORMAL, // file attributes
					NULL);			// template file

	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	m_dwSize = GetFileSize(m_hFile, NULL);
	m_dwPosition = 0;
	return TRUE;
}

// ---------------------------------------------------------------------------
/// Function name          : CFileOperate::FlushBuffer
/// \return Return type    : BOOL 
/// Description            : 
// ---------------------------------------------------------------------------
BOOL CFileOperate::FlushBuffer()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	return FlushFileBuffers(m_hFile);
}
//----------------------------------------
//  Function:   Create new file
//  Comment:	创建一个新文件，准备写操作
//----------------------------------------
BOOL CFileOperate::CreateA(LPCSTR FileName,DWORD dwMode)

{
	if (m_hFile != INVALID_HANDLE_VALUE)
		Close();

	m_hFile = CreateFileA(
					FileName,		// pointer to name of the file with path
					GENERIC_WRITE|GENERIC_READ,	// access (read-write) mode
					FILE_SHARE_READ,// share mode
					NULL,			// pointer to security attributes
					dwMode,			// create or over write
					FILE_ATTRIBUTE_NORMAL, // file attributes
					NULL);			// template file

	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	m_dwPosition = 0;
	return TRUE;
}
//---------------------------------------------------------------------------
//  Function:   Read data from file
//  Return:		success : number of bytes read from file
//              fail    : 0
//  Comment:	读取文件数据
//---------------------------------------------------------------------------
DWORD CFileOperate::Read(LPVOID lpBuffer,DWORD nReadBytes)
{
	DWORD BytesRead;
	
	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0;
	ReadFile(m_hFile, lpBuffer, nReadBytes, &BytesRead, NULL);
	m_dwPosition += BytesRead;
	return BytesRead;
}
//---------------------------------------------------------------------------
//  Function:   Write data to file
//  Return:		success : number of bytes write to file
//              fail    : 0
//  Comment:	写入文件数据
//---------------------------------------------------------------------------
DWORD CFileOperate::Write(LPCVOID lpBuffer,DWORD nWriteBytes)
{
	DWORD BytesWrite;
	
	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0;
	WriteFile(m_hFile, lpBuffer, nWriteBytes, &BytesWrite, NULL);
	m_dwPosition += BytesWrite;

	m_dwSize += BytesWrite;

	return BytesWrite;
}
//---------------------------------------------------------------------------
//  Function:   Move file pointer
//  Params:		Distance = file offset
//              MoveMethod = FILE_BEGIN
//                           FILE_CURRENT
//                           FILE_END
//	Return:		success : pointer of file
//				fail    : 0xffffffff
//  Comment:	移动文件指针
//---------------------------------------------------------------------------
DWORD CFileOperate::Seek(LONG Distance, DWORD MoveMethod)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0xffffffff;
	m_dwPosition = SetFilePointer(m_hFile, Distance, NULL, MoveMethod);
	return m_dwPosition;
}
//---------------------------------------------------------------------------
//  Function:   Get file pointer pos
//	Return:		success : low dword of file size
//				fail    : 0xffffffff
//  Comment:	取得文件指针位置
//---------------------------------------------------------------------------
DWORD CFileOperate::Tell()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0xffffffff;
	return m_dwPosition;
}
//---------------------------------------------------------------------------
//  Function:   Get file size (number of bytes)
//	Return:		success : low dword of file size
//				fail    : 0xffffffff
//  Comment:	取得文件的大小
//---------------------------------------------------------------------------
DWORD CFileOperate::GetSize()
{	
	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0xffffffff;
	return m_dwSize;
}
//**************************************************************************
//  Function:   Check file end 
//	Return:		success : is End
//				fail    : 
//  Comment:	检测文件指针是否到文件结束
//**************************************************************************
BOOL CFileOperate::Eof()
{
	if (m_dwPosition >= m_dwSize)
		return TRUE;
	return FALSE;
}
//**************************************************************************
//  Function:   ReadDWORD
//	Return:		success : is End
//				fail    : 
//  Comment:	从当前文件位置读取一个32长度的数据
//**************************************************************************
BOOL CFileOperate::ReadDWORD(DWORD* pdwData)
{
	BYTE	byData[4];	
	if (!pdwData)
		return FALSE;

	Read(byData,4);

	TranslateBYTEToDWORD(byData,1,pdwData);
	return TRUE;
}
//**************************************************************************
//  Function:   TranslateBYTEToDWORD
//	Return:		success : is End
//				fail    : 
//  Comment:	将从文件读出的BYTE转换为DWORD
//**************************************************************************
BOOL CFileOperate::TranslateBYTEToDWORD(BYTE *SData,DWORD size,DWORD *DData)
{
	DWORD temp=0,temp1=0,temp2=0,temp3=0;
	int in=0,index=0;
	
	if (!SData||!DData)
		return FALSE;
	
	for(DWORD i=0;i<size;i++)
	{
		temp=*(SData+in);in++;
		temp1=*(SData+in);in++;
		temp2=*(SData+in);in++;
		temp3=*(SData+in);in++;
		temp=temp&0x000000ff;
 		temp1=(DWORD)(temp1<<8) &0x0000ff00;
		temp2=(DWORD)(temp2<<16)&0x00ff0000;
		temp3=(DWORD)(temp3<<24)&0xff000000;
		*(DData+index)=(DWORD)(temp|temp1|temp2|temp3);
		index++;
	}
	return TRUE;
}
//**************************************************************************
//  Function:   TranslateDWORDToBYTE
//	Return:		success : is End
//				fail    : 
//  Comment:	将DWORD转换为BYTE
//**************************************************************************
BOOL CFileOperate::TranslateDWORDToBYTE(DWORD *SData,DWORD size,BYTE *DData)
{
	if (!SData||!DData)
		return FALSE;
	for(DWORD i=0;i<size;i++)
	{
		*(DData++)=(BYTE)(*(SData+i)&0x00000088);
		*(DData++)=(BYTE)(*(SData+i)&0x00008800);
		*(DData++)=(BYTE)(*(SData+i)&0x00880000);
		*(DData++)=(BYTE)(*(SData+i)&0x88000000);
	}
	return TRUE;
}
//**************************************************************************
//  Function:   ReadString
//	Return:		success : is End
//				fail    : 
//  Comment:	读取一个字符串，dwSize为字符串长度
//**************************************************************************
BOOL CFileOperate::ReadString(LPTSTR pchData,size_t dwToReadSize)
{
  if (!pchData)
	  return FALSE;

  Read(pchData,(DWORD)dwToReadSize*sizeof(TCHAR));

  return TRUE;
}

//**************************************************************************
//  Function:   ReadString
//	Return:		success : is End
//				fail    : 
//  Comment:	读取一个字符串，dwSize为字符串长度
//**************************************************************************
BOOL CFileOperate::ReadStringA(LPTSTR pchData,size_t dwToReadSize)
{
	if (!pchData)
		return FALSE;

#ifdef _UNICODE
	char* pString = new char [dwToReadSize+1];
	
	Read(pString,(DWORD)dwToReadSize);
	
	pString[dwToReadSize] = 0;

	MultiByteToWideChar(CP_ACP,0,pString,-1,pchData,(DWORD)dwToReadSize);

	delete [] pString;

#else
	Read(pchData,dwToReadSize*sizeof(TCHAR));
#endif

	return TRUE;
}
//**************************************************************************
//  Function:   SaveString
//	Return:		success : is End
//				fail    : 
//  Comment:	读取一个字符串，dwSize为字符串长度
//**************************************************************************
BOOL CFileOperate::SaveStringA(LPCTSTR pchData,size_t len)
{
	if (!pchData)
		return FALSE;

#ifdef _UNICODE
	DWORD n = lstrlen(pchData)+1;
	LPSTR pString = new char [n*2];

	int nLenRet = WideCharToMultiByte(CP_THREAD_ACP,0,pchData,-1,pString,n*2,NULL,NULL);
	if (0 == nLenRet)
	{
		return FALSE;
	}

	pString[nLenRet] = 0;

	Write(pString,nLenRet);

	delete [] pString;
#else
	if (len == 0)
		len = lstrlen(pchData)+1;
	Write((LPVOID)pchData,len);
#endif
	return TRUE;
}

// ---------------------------------------- 
// Function name   : CFileOperate::SaveString
// Return type     : BOOL 
// Argument        : LPTSTR pchData
// Description     : 
// ---------------------------------------- 
BOOL CFileOperate::SaveString(LPCTSTR pchData,size_t len)
{
  if (!pchData)
	  return FALSE;

  if (len == 0)
	  len = (lstrlen(pchData)+1)*sizeof(TCHAR);
  Write((LPVOID)pchData,(DWORD)len*sizeof(TCHAR));

  return TRUE;
}
