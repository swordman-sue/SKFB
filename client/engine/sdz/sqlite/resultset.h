#ifndef RESULT_SET_H
#define RESULT_SET_H
#include <string>
#include "sqlite3.h"
#include "dbhelper.h"
#include <stdlib.h>
class ResultSet 
{
public:
	ResultSet():m_Result(NULL)
				,m_Row(0)
				,m_Column(0)
	{}
	~ResultSet();
public:
	friend class DatabaseHelper;
public:
	int GetColumnNum();
	int GetRecordCount(); 
	
public:  
	int GetInt(int row, int column);
	std::string GetString(int row, int column);
private:
    char * GetAt(int row, int column);
private:  
    char **m_Result;
    int m_Row;
    int m_Column;
};
#endif