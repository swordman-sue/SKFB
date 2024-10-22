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
	//��������һ��db���ݿ� dbΪ���ݿ�·��
    // �����ݿ�  
     bool Open(const std::string db); 

	 //ִ��sql���
	 int Execute(const std::string sql);
      
    //�����жϱ���Ƿ����  
     bool TableIsExist(const std::string tablename);  
      
    //��������һ������Ϊtablename�ı�񣬴���ʱ����ƥ��ʱ���иñ�Ĵ�����������򲻴���  
     bool CreateTable(std::string sql,const std::string tablename);
      
    //����ɾ��һ�ű���Ϊtablename�ı��  
     bool DropTable(const std::string tablename);  
      
    //����һ������  
     bool InsertData(std::string sql);  
      
    //ɾ������  
     bool DeleteData(std::string sql);  
      
    //�޸�����  
     bool UpdateData(std::string sql);

    //�������ݼ�
	 ResultSet* ExecuteQuery(const std::string sql);

	 //�ͷŷ��ص����ݼ�
	 void FreeResultSet(ResultSet* rs);
    
	 //��ʼ����
	 void Begin();

	 //�ύ����
	 void Commit();

	 //����ع�
	 void Rollback();

	 //�رմ򿪵����ݿ�  
     bool Close(); 

private:
	 sqlite3* m_db ;			//���ݿ�ָ�� 
	 char* m_err_msg;			//������Ϣ 
	 std::string m_sqlstr;		//SQLָ�� 
	 int m_result_code;			//sqlite3_exec����ֵ
//	 sqlite3_stmt  *stmt
};
#endif
