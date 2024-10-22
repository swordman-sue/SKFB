#pragma once

#include "QueryResult.h"


class QueryForUser : public QueryResult
{
public:
	VOID	SetThreadIndex(int ThreadIndex) { m_dwThreadIndex = ThreadIndex; }
	int		GetThreadIndex() { return m_dwThreadIndex; }

private: 
	int		m_dwThreadIndex;				//数据库线程Index，每个玩家和固定的Index绑定
};