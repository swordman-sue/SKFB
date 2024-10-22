#pragma once
#include <Windows.h>
#include <mysql.h>

class SqlDatabase
{
public:
	SqlDatabase(void);
	~SqlDatabase(void);

public:
	bool Connect(const char* dbname, const char* servername, const char* username, const char* password, unsigned int port = 3306);

	bool IsConnected() const;

	void Disconnect();

	void ThreadStart();

	void ThreadEnd();

	//用于select查询
	int OpenRecordset();
	void CloseRecordset();

	//用于insert update delete
	int Execute();

	unsigned int affected_rows();

	const char* error();
	unsigned int errorno();
	unsigned int escape_string(char* to, const char* from, unsigned int length);

	unsigned int GetInsertId();

	bool Fetch();

	bool GetInt(unsigned int nColIndex, int& iVal);
	bool GetUInt(unsigned int nColIndex, unsigned int& uiVal);
	bool GetFloat(unsigned int nColIndex, float& fVal);
	bool GetUShort(unsigned int nColIndex, unsigned short& usVal);
	bool GetUChar(unsigned int nColIndex, unsigned char& ucVal);
	bool GetBYTE(unsigned int nColIndex, BYTE& byVal);
	bool GetWORD(unsigned int nColIndex, WORD& wVal);
	bool GetDWORD(unsigned int nColIndex, DWORD& dwVal);
	bool GetShort(unsigned int nColIndex, short& sVal);
	bool GetUInt64(unsigned int nColIndex, unsigned __int64& i64Val);
	bool GetString(unsigned int nColIndex, char*buf, unsigned int BufLen);
	unsigned int GetBlob(unsigned int nColIndex, unsigned char*buf,unsigned int BufLen = 0);


public:
	char*						GetQuery() { return m_szQuery; }

private:
	char						m_szHostAddress[50];			// host ip or name
	char						m_szUserName[50];				// db user name
	char						m_szPassword[50];				// db password
	char						m_szDBName[50];					// database name
	unsigned int				m_nPort;

	MYSQL						m_Mysql;						// mysql connect

	MYSQL_RES*					m_pRes;						// current operator recordset
	MYSQL_ROW					m_Row;						// current operator row
	unsigned long*				m_lFieldsLength;			// current row field length

	unsigned int				m_nResNum;					// recoredset number
	unsigned int				m_nFieldsNum;				// Field number

	char						m_szQuery[1024 * 1000];		// current operator sql
	bool						m_bConnected;				// if connected
};
