#pragma once

#include "QueryResult.h"


class QueryForUser : public QueryResult
{
public:
	VOID	SetThreadIndex(int ThreadIndex) { m_dwThreadIndex = ThreadIndex; }
	int		GetThreadIndex() { return m_dwThreadIndex; }

private: 
	int		m_dwThreadIndex;				//���ݿ��߳�Index��ÿ����Һ͹̶���Index��
};