#ifndef DATABBASE_HELPER_H
#define DATABBASE_HELPER_H
#include "sqlite3.h"
#include "resultset.h"
#include <map>
#include <string>

class ResultSet;

class DatabaseHelper	
{
public:
	~DatabaseHelper(){}
	static DatabaseHelper* GetInstance();

private:
	DatabaseHelper():m_db(0)
					,m_err_msg(NULL)
					,m_result_code(0)
	{}

public:
	//用来创建一个db数据库 db为数据库路径
    // 打开数据库  
     bool Open(const std::string db); 

	 //执行sql语句
	 int Execute(const std::string sql);
      
    //用来判断表格是否存在  
     bool TableIsExist(const std::string tablename);  
      
    //用来创建一个表名为tablename的表格，创建时会先匹配时否有该表的存在如果存在则不创建  
     bool CreateTable(std::string sql,const std::string tablename);
      
    //用来删除一张表名为tablename的表格  
     bool DropTable(const std::string tablename);  
      
    //插入一条数据  
     bool InsertData(std::string sql);  
      
    //删除数据  
     bool DeleteData(std::string sql);  
      
    //修改数据  
     bool UpdateData(std::string sql);

    //返回数据集
	 ResultSet* ExecuteQuery(const std::string sql);

	 //释放返回的数据集
	 void FreeResultSet(ResultSet* rs);
    
	 //开始事务
	 void Begin();

	 //提交事务
	 void Commit();

	 //事务回滚
	 void Rollback();

	 //关闭打开的数据库  
     bool Close(); 

private:
	 sqlite3* m_db ;			//数据库指针 
	 char* m_err_msg;			//错误信息 
	 std::string m_sqlstr;		//SQL指令 
	 int m_result_code;			//sqlite3_exec返回值
//	 sqlite3_stmt  *stmt
};
#endif
