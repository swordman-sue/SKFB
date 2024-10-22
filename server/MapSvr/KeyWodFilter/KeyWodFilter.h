#pragma once
#include "deelx.h"
#include <string>


class CKeyWordFilter
{
public:
    CKeyWordFilter();
    ~CKeyWordFilter();

    //!  初始化
    //! \return void  
    void    Initialize(const char* szRoleNameFile, 
                       const char* szSqlCmdFile,
                       const char* szChatFile);

public:

	//!  匹配帐号取名规则
	//! \return bool  
	//! \param const char* szRoleName
	bool   MatchAccountName(const char* szAccountName);

    //!  匹配玩家取名规则
    //! \return bool  
    //! \param const char* szRoleName
    bool   MatchRoleName(const char* szRoleName);

	//!  匹配玩家取名规则
	//! \return bool  
	//! \param const char* szRoleName
	bool   MatchRoleNameA(const char* szRoleName);


    //!  匹配Sql语法屏蔽规则
    //! \return bool  
    //! \param const char* szSqlCmd
    bool   MatchSqlCmd(const char* szSqlCmd);

    //!  匹配聊天词过滤规则
    //! \return bool  
    //! \param const char* szChat
    bool   MatchChat(const char* szChat);

    //!  替换聊天词过滤规则
    //! \return bool  
    //! \param char* szChat
    void   ReplaceChat(const char* szChat, char* szReplaced, int nSize);

	//!  替换数据库关键字过滤规则
	//! \return bool  
	//! \param char* szSql
	void ReplaceSql(const char* szSql, char* szReplaced, int nSize);
private:
    
    //!  加载玩家取名规则
    //! \return bool  
    //! \param const char* szFile
    bool    LoadRoleNameFilter(const char* szFile);

    //!  加载Sql屏蔽字规则
    //! \return bool  
    //! \param const char * szFile
    bool    LoadSqlCmdFilter(const char * szFile);

    //!  加载聊天词过滤规则
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

