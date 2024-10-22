#pragma once
#include "deelx.h"
#include <string>


class CKeyWordFilter
{
public:
    CKeyWordFilter();
    ~CKeyWordFilter();

    //!  ��ʼ��
    //! \return void  
    void    Initialize(const char* szRoleNameFile, 
                       const char* szSqlCmdFile,
                       const char* szChatFile);

public:

	//!  ƥ���ʺ�ȡ������
	//! \return bool  
	//! \param const char* szRoleName
	bool   MatchAccountName(const char* szAccountName);

    //!  ƥ�����ȡ������
    //! \return bool  
    //! \param const char* szRoleName
    bool   MatchRoleName(const char* szRoleName);

	//!  ƥ�����ȡ������
	//! \return bool  
	//! \param const char* szRoleName
	bool   MatchRoleNameA(const char* szRoleName);


    //!  ƥ��Sql�﷨���ι���
    //! \return bool  
    //! \param const char* szSqlCmd
    bool   MatchSqlCmd(const char* szSqlCmd);

    //!  ƥ������ʹ��˹���
    //! \return bool  
    //! \param const char* szChat
    bool   MatchChat(const char* szChat);

    //!  �滻����ʹ��˹���
    //! \return bool  
    //! \param char* szChat
    void   ReplaceChat(const char* szChat, char* szReplaced, int nSize);

	//!  �滻���ݿ�ؼ��ֹ��˹���
	//! \return bool  
	//! \param char* szSql
	void ReplaceSql(const char* szSql, char* szReplaced, int nSize);
private:
    
    //!  �������ȡ������
    //! \return bool  
    //! \param const char* szFile
    bool    LoadRoleNameFilter(const char* szFile);

    //!  ����Sql�����ֹ���
    //! \return bool  
    //! \param const char * szFile
    bool    LoadSqlCmdFilter(const char * szFile);

    //!  ��������ʹ��˹���
    //! \return bool  
    //! \param const char * szFile
    bool    LoadChatFilter(const char * szFile);

protected:
     CRegexpT <char>     m_Regexp;
     CRegexpT <wchar_t>  m_wRegexp;

     std::wstring m_strChatFilter;
     std::wstring m_strRoleNameFilter;
	 std::string m_strSqlCmdFilter;
};

