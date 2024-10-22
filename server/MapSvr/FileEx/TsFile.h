////////////////////////////////////////////////////////////////////////////////
//	Filename     :TsFile.h
//	Created by   :Kevin
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once
//#include <winnt.h>
#include <wtypes.h>


//---------------------------------------------------------------------------
class CFileOperate
{
//---------------------------------------------------
public:
	CFileOperate();
	~CFileOperate();
	virtual BOOL		Open(LPCTSTR FileName,DWORD dwMode = GENERIC_READ|GENERIC_WRITE);
	virtual BOOL		Create(LPCTSTR FileName,DWORD dwMode = CREATE_ALWAYS);
	virtual BOOL		OpenA(LPCSTR FileName,DWORD dwMode = GENERIC_READ|GENERIC_WRITE);
	virtual BOOL		CreateA(LPCSTR FileName,DWORD dwMode = CREATE_ALWAYS);

	virtual BOOL		Eof();
	virtual DWORD		Read(LPVOID lpBuffer,DWORD nReadBytes);
	virtual DWORD		Write(LPCVOID lpBuffer,DWORD nWriteBytes);
	virtual DWORD		Seek(LONG Distance, DWORD MoveMethod);
	virtual DWORD		Tell();
	virtual DWORD		GetSize();
	virtual BOOL		Close();
	virtual BOOL		FlushBuffer();
	virtual BOOL		ReadString(LPTSTR pchData,size_t dwToReadSize);
	virtual BOOL		ReadStringA(LPTSTR pchData,size_t dwToReadSize);// ��ȡ��char���������
	virtual BOOL		SaveString(LPCTSTR pchData,size_t len = 0);
	virtual BOOL		SaveStringA(LPCTSTR pchData,size_t len = 0);	// ��char��ʽ����
	virtual BOOL		ReadDWORD(DWORD* pdwData);
//-------------------------------------------------------------------------
protected:
	HANDLE		m_hFile;		// �ļ����	
	DWORD		m_dwSize;		// �ļ�����
	DWORD		m_dwPosition;	// �ļ�ָ��
	virtual BOOL		TranslateBYTEToDWORD(BYTE *SData,DWORD size,DWORD *DData);
	virtual BOOL		TranslateDWORDToBYTE(DWORD *SData,DWORD size,BYTE *DData);
};
//---------------------------------------------------------------------------
