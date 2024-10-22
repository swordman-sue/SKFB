////////////////////////////////////////////////////////////////////////////////
//	Filename     :TsIniFile.h
//	Created by   :Kevin
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TsFile.h"

//---------------------------------------------------------------------------------------
typedef struct tagKeyNode
{
	DWORD			dwID;
	LPSTR			pKey;
	LPSTR			pValue;
	tagKeyNode*		pNextNode;
}
KEYNODE;
//---------------------------------------------------------------------------
typedef struct tagSecNode
{
	DWORD			dwID;
	LPSTR			pSection;
	tagKeyNode		pKeyNode;
	tagSecNode*		pNextNode;
}
SECNODE;
//---------------------------------------------------------------------------
class TMyIniFile
{
protected:
	TCHAR				m_FileName[MAX_PATH];
	SECNODE				m_Header;
	LONG				m_Offset;
	LONG				m_lReadPos;				// 读取位置
	LONG				m_lStartSearch;			// 寻找的启示块数
	BOOL				m_bBigSmall;			// 大小写敏感
	CFileOperate		m_File;
	DWORD				m_dwMode;
//---------------------------------------------------
protected:
	virtual	void		Init();
	virtual LONG		CreateIniLink(LPVOID Buffer,LONG Size);
	virtual void		ReleaseIniLink();
	virtual DWORD		String2Id(LPCSTR pString);
	virtual BOOL		ReadLine(LPSTR Buffer,LONG Size);
	virtual BOOL		IsKeyChar(CHAR ch);
	virtual LPSTR		SplitKeyValue(LPCSTR pString);
	virtual BOOL		GetKeyValue(LPCSTR pSection,LPCSTR pKey,LPSTR pValue,DWORD dwSize);
	virtual BOOL		SetKeyValue(LPCSTR pSection,LPCSTR pKey,LPCSTR pValue);	
	virtual	BOOL		ReadData();
//---------------------------------------------------
public:
	TMyIniFile();
	~TMyIniFile();
	virtual BOOL		New(LPCTSTR FileName);
	virtual BOOL		Open(LPTSTR FileName);
	virtual BOOL		Save(void);
	virtual void		Close();
	virtual LPCTSTR		GetFileName(){return m_FileName;}
	virtual void		Clear();
	virtual void		EraseSection(LPCSTR lpSection);
	virtual void		SetBigSmall(BOOL bFlag);
	virtual BOOL		GetData(LPCSTR pSection,LPCSTR pKey,LPVOID pValue,DWORD dwSize);
	virtual void		GetString(
									LPCSTR	lpSection,		// points to section name
									LPCSTR	lpKeyName,		// points to key name
									LPCSTR	lpDefault,		// points to default string
									LPSTR	lpRString,		// points to destination buffer
									DWORD	dwSize			// size of string buffer
									);
	virtual void		GetStringW(
									LPCSTR	lpSection,		// points to section name
									LPCSTR	lpKeyName,		// points to key name
									LPCWSTR	lpDefault,		// points to default string
									LPWSTR	lpRString,		// points to destination buffer
									DWORD	dwSize			// size of string buffer
									);
	virtual void		GetInteger(
									LPCSTR	lpSection,		// points to section name
									LPCSTR	lpKeyName,		// points to key name
									int		nDefault,		// default value
									int		*Value          // return value
									);
	virtual	void		GetDouble(
									LPCSTR lpSection,		// points to section name
									LPCSTR lpKeyName,		// points to key name
									double   fDefault,			// default value
									double   *Value            // points to value
								 );
	virtual	void		GetFloat(
									LPCSTR lpSection,		// points to section name
									LPCSTR lpKeyName,		// points to key name
									float   fDefault,			// default value
									float   *Value            // points to value
								 );
	virtual void		GetStruct(
									LPCSTR	lpSection,		// pointer to section name
									LPCSTR	lpKeyName,		// pointer to key name
									LPVOID	lpStruct,		// pointer to buffer that contains data to add
									DWORD 	dwSize			// size, in bytes, of the buffer
									);
	virtual void		WriteString(
									LPCSTR	lpSection,		// pointer to section name
									LPCSTR	lpKeyName,		// pointer to key name
									LPCSTR	lpString		// pointer to string to add
									);
	virtual void		WriteInteger(
									LPCSTR	lpSection,		// pointer to section name
									LPCSTR	lpKeyName,		// pointer to key name
									int	    Value			// value to write
									);
	virtual void		WriteDouble(
									LPCSTR	lpSection,		// pointer to section name
									LPCSTR	lpKeyName,		// pointer to key name
									double  Value			// value to write
									);
	virtual void		WriteStruct(
									LPCSTR	lpSection,		// pointer to section name
									LPCSTR	lpKeyName,		// pointer to key name
									LPVOID	lpStruct,		// pointer to buffer that contains data to add
									DWORD 	dwSize			// size, in bytes, of the buffer
									);
};