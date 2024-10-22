#pragma once

#define QUERY_RESULT_STRING_LEN		256
#define QUERY_RESULT_TABLE_LEN		64

class QueryResult
{
public:
	QueryResult(void);
	virtual ~QueryResult(void);
	virtual inline BOOL				Init() = 0;
	virtual	VOID					Release() =0;

	virtual inline BOOL				InitEx();

	inline DWORD					GetIndex() { return m_dwQueryIndex; }
	inline VOID						SetIndex(DWORD dwIdx) { m_dwQueryIndex = dwIdx; }

	inline DWORD					GetQueryIndex() { return m_dwQueryIndex; }
	inline VOID						SetQueryIndex(DWORD dwQueryIndex) { m_dwQueryIndex = dwQueryIndex; }

	inline ULONG					GetResultRowNum() { return m_ulResultRowNum; }
	inline VOID						SetResultRowNum( ULONG ulVal ) { m_ulResultRowNum = ulVal; }	

	inline unsigned int				GetQueryResult() { return m_QueryResult; }
	inline VOID						SetQueryResult(unsigned int uires){ m_QueryResult = uires; }

	inline const char*				GetQueryResultString() { return m_QueryResultString; }
	inline VOID						SetQueryResultString(const char* pResultString)
	{ strncpy(m_QueryResultString, pResultString, sizeof(m_QueryResultString)); m_QueryResultString[QUERY_RESULT_STRING_LEN] = '\0'; }

	inline const char*				GetQueryResultTable() { return m_QueryResultTable; }
	inline VOID						SetQueryResultTable(const char* pResultTable)
	{ strncpy(m_QueryResultTable, pResultTable, sizeof(m_QueryResultTable)); m_QueryResultTable[QUERY_RESULT_TABLE_LEN] = '\0'; }

	inline DWORD					GetKey() { return m_dwKey; }
	inline VOID						SetKey(DWORD dwKey) { m_dwKey = dwKey; }

	inline UINT64					GetInitTime() { return m_ulInitTime; }

	//===Test
//public:
//	SaveCostMonitor					m_SaveCostMonitor;

private:
	DWORD							m_dwQueryIndex;	// 查询索引		
	ULONG							m_ulResultRowNum;	// 结果集记录
	unsigned int					m_QueryResult;		// 结果代码
	char							m_QueryResultString[QUERY_RESULT_STRING_LEN+1];
	char							m_QueryResultTable[QUERY_RESULT_TABLE_LEN+1];
	DWORD							m_dwKey;
	ULONG							m_ulInitTime;			// 初始化的时间戳
};