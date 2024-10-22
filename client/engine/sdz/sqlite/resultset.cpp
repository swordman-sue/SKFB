#include "resultset.h"

ResultSet::~ResultSet()
{
	sqlite3_free_table(m_Result);
}

int ResultSet::GetColumnNum()
{
	return m_Column;
}

int ResultSet::GetRecordCount()
{
	return m_Row;
}

char * ResultSet::GetAt(int row, int column)  
{
    return m_Result[(row + 1) * m_Column + column];  
}

int ResultSet::GetInt(int row, int column)
{
	return atoi(GetAt(row, column));  
}

std::string ResultSet::GetString(int row, int column)
{
	return std::string(GetAt(row, column));  
}