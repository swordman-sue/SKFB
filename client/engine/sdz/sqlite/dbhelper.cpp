#include "dbhelper.h"
#include "zqcommon/logutil.h"

int isExisted(void * para, int n_column, char ** column_value, char ** column_name )  
{  
    bool *isExisted_=(bool*)para;  
    *isExisted_=(**column_value)!='0';  
    return 0;  
}

DatabaseHelper* DatabaseHelper::GetInstance()
{
	static DatabaseHelper db;
	return &db;
}

bool DatabaseHelper::Open(const std::string db)
{
	if (db.empty())
	{
		LogUtil::LogError("open database fail,dbname is empty");  
		return false;
	}
	int nRet = sqlite3_open(db.c_str(), &m_db);
	if (nRet != SQLITE_OK)
	{
		this->Close();
		LogUtil::LogError("open database fail dbname = [%s] errcode = [%d]",db.c_str(),nRet);  
	}
	return nRet == SQLITE_OK;
}

int DatabaseHelper::Execute(const std::string sql)
{
	if (!m_db)
	{
		LogUtil::LogError("database is not connect");
		return -1;
	}
	if (sql.empty())
	{
		LogUtil::LogError("excute sql is null");
		return -1;
	}
	int nRet = sqlite3_exec(m_db, sql.c_str(), NULL, NULL, (char **) &m_err_msg);  
    if (nRet != SQLITE_OK)  
    {
		LogUtil::LogError("excute sql fail sql = [%s], errmsg = [%s]",sql.c_str(),m_err_msg);
        return 1;  
    }
    return nRet;
}

bool DatabaseHelper::TableIsExist(std::string tablename)
{
	if (m_db && !tablename.empty())  
    {  
        bool tableIsExisted = false;  
        std::string sqlstr = "select count(type) from sqlite_master where type='table' and name ='"+tablename+"'";  
        int nRet = sqlite3_exec(m_db,sqlstr.c_str(),isExisted,&tableIsExisted,&m_err_msg);
        return tableIsExisted;  
    }else
	{
		LogUtil::LogError("tablename  is  empty or database pointer is null");
	}
    return false;  
}

bool DatabaseHelper::CreateTable(std::string sql,const std::string name)
{
	if (!TableIsExist(name))  
    {  
        int nRet = this->Execute(sql);
        return nRet == SQLITE_OK;
    } 
	return true;
}

bool DatabaseHelper::DropTable(const std::string tablename)
{
	if (tablename.empty())
	{
		LogUtil::LogError("table name is empty when drop table");
		return false;
	}
    int nRet = this->Execute(std::string("DROP TABLE IF EXISTS ")+tablename);
	return nRet == SQLITE_OK;
}

bool DatabaseHelper::InsertData(std::string sql)
{
    int nRet = this->Execute(sql);
    return nRet == SQLITE_OK;
}

bool DatabaseHelper::DeleteData(std::string sql)
{
	int nRet = this->Execute(sql);
    return nRet == SQLITE_OK;
}
bool DatabaseHelper::UpdateData(std::string sql)
{
	int nRet = this->Execute(sql);
    return nRet == SQLITE_OK;
}

ResultSet* DatabaseHelper::ExecuteQuery(const std::string sql)
{ 
	if (sql.empty())
	{
		LogUtil::LogError("sql is empty when excute query");
		return NULL;
	}
    ResultSet *pRs = new ResultSet();  
	int nRet = sqlite3_get_table(m_db, sql.c_str(), &pRs->m_Result, &pRs->m_Row, &pRs->m_Column, (char **) &m_err_msg);   
    if (nRet != SQLITE_OK)  
    {
		return NULL; 
    }     
    return pRs;
}

void DatabaseHelper::FreeResultSet(ResultSet* rs)
{
	if (rs != NULL)  
	{  
		delete rs;  
		rs = NULL;  
	}
}

void DatabaseHelper::Begin()
{
	this->Execute("BEGIN TRANSACTION;"); 
}

void DatabaseHelper::Commit()
{
	this->Execute("COMMIT TRANSACTION;"); 
}

void DatabaseHelper::Rollback()
{
	this->Execute("ROLLBACK TRANSACTION;"); 
}

bool DatabaseHelper::Close()
{
	if (m_db)
	{
		int nRet = sqlite3_close(m_db);
		m_db = NULL;
		return nRet == SQLITE_OK;
	}
	return true;
}