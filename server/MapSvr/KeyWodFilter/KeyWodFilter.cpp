#include "stdafx.h"
#include "KeyWodFilter.h"
#include "../FileEx/TsFile.h"

////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG) && defined(_TESTMEMORY) && defined(_MSC_VER) 
#	if defined(_MFC_VER) 
#		define new DEBUG_NEW
#	elif defined(_DLL)
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include "crtdbg.h"
#		define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	else
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include "crtdbg.h"
#	endif
#endif
////////////////////////////////////////////////////////////////////////////////

#define MAX_ONE_ROW     512// 单行最大字符串大小

// add by Kevin
#define MAX_ROLENAMEMAX			32		// 角色名字的最大长度
enum ConsoleOutMsgClass
{
	CON_NONE = 0,         // 不输出
	CON_CMDOUTPUT,        // 程序命令输出，总是要显示，但不记成日志   [任何状况都显示]
	CON_BUG,              // BUG,很严重，必须显示，同时也必须保存     [任何状况都记录日志]
	CON_ERROR,            // 错误                                     [任何状况都记录日志]
	CON_WARNING,          // 警告
	CON_NOTIFY,           // 通知
	CON_DEBUG,            // 调试消息，仅在DEBUG状态下显示
	CON_SPAM              // 详细的日志，暂时不使用
};

void Printf(ConsoleOutMsgClass con,const CHAR* string, ...)
{

}

#define DebugMsgOutA Printf
// add by Kevin

using namespace std;
//-----------------------------------------------------------------
// Function name   : CKeyWordFilter::CKeyWordFilter
// Return type     : 
// Description     : 
//-----------------------------------------------------------------
CKeyWordFilter::CKeyWordFilter()
{
}

//-----------------------------------------------------------------
// Function name   : CKeyWordFilter::~CKeyWordFilter
// Return type     : 
// Description     : 
//-----------------------------------------------------------------
CKeyWordFilter::~CKeyWordFilter()
{
}

//-----------------------------------------------------------------
// Function name   : CKeyWordFilter::Initialize
// Return type     : 
// Description     : 
//-----------------------------------------------------------------
  void CKeyWordFilter::Initialize(const char* szRoleNameFile, 
                                  const char* szSqlCmdFile,
                                  const char* szChatFile)
  {
	  if (szRoleNameFile && !LoadRoleNameFilter(szRoleNameFile))
		  DebugMsgOutA(CON_ERROR, "[%s]  LoadRoleNameFilter failed, path=%s! \n", __FUNCTION__, szRoleNameFile);

	  if (szSqlCmdFile && !LoadSqlCmdFilter(szSqlCmdFile))
		  DebugMsgOutA(CON_ERROR, "[%s]  LoadSqlCmdFilter failed, path=%s! \n", __FUNCTION__, szSqlCmdFile);

	  if (szChatFile && !LoadChatFilter(szChatFile))
		  DebugMsgOutA(CON_ERROR, "[%s]  LoadChatFilter failed, path=%s! \n", __FUNCTION__, szChatFile);
  }

// 
// //-----------------------------------------------------------------
// // Function name   : CKeyWordFilter::LoadRoleNameFilter
// // Return type     : bool 
// // Argument        : const char* szFile
// // Description     : 
// //-----------------------------------------------------------------
 bool CKeyWordFilter::LoadRoleNameFilter(const char* szFile)
 {
 #ifndef __CLIENT__
     if (!szFile)
         return false;
 
      wchar_t temp[MAX_ONE_ROW+1] = {0};
//      m_strRoleNameFilter.clear();
 
     CFileOperate File;
 
 	if (!File.Open(szFile))
         return false;
 
 	while (!File.Eof())
     {
         ZeroMemory(temp, MAX_ONE_ROW+1);
          File.Read(temp, MAX_ONE_ROW);
          m_strRoleNameFilter += temp;
     }
 
     File.Close();
 #endif
     return true;
  }
// 
// //-----------------------------------------------------------------
// // Function name   : CKeyWordFilter::LoadSqlCmdFilter
// // Return type     : bool 
// // Argument        : const char * szFile
// // Description     : 
// //-----------------------------------------------------------------
 bool CKeyWordFilter::LoadSqlCmdFilter(const char * szFile)
 {
 #ifndef __CLIENT__
     if (!szFile)
         return false;
 
     char temp[MAX_ONE_ROW+1] = {0};
     //m_strSqlCmdFilter.clear();
 
 	CFileOperate File;
     
 	if (!File.Open(szFile))
         return false;
 
 	while (!File.Eof())
     {
          ZeroMemory(temp, MAX_ONE_ROW+1);
		  File.Read(temp, MAX_ONE_ROW);
          m_strSqlCmdFilter += temp;
     }
 
     File.Close();
 #endif
     return true;
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::LoadChatFilter
 // Return type     : bool 
 // Argument        : const char * szFile
 // Description     : 
 //-----------------------------------------------------------------
 bool CKeyWordFilter::LoadChatFilter(const char * szFile)
 {
 #ifndef __CLIENT__
     if (!szFile)
         return false;
 
     wchar_t temp[MAX_ONE_ROW+1] = {0};
     //m_strChatFilter.clear();
 
     CFileOperate File;
     
 	if (!File.OpenA(szFile))
         return false;
 
 	while (!File.Eof())
     {
         ZeroMemory(temp, MAX_ONE_ROW+1);
         File.Read(temp, MAX_ONE_ROW);
         m_strChatFilter += temp;
     }
 
      File.Close();
 #endif
     return true;
 }
 
// 
// //-----------------------------------------------------------------
// // Function name   : CKeyWordFilter::MatchAccountName
// // Return type     : bool 
// // Argument        : const char* szAccountName
// // Description     : 
// //-----------------------------------------------------------------
 bool CKeyWordFilter::MatchAccountName( const char* szAccountName )
 {
  	static CRegexpT <char> regexp("^(?!_)(?!.*?_$)[a-zA-Z0-9_]+$");
  	MatchResult PreResult = regexp.MatchExact(szAccountName);
  
  	if (PreResult.IsMatched())
  		return false;
 
 	return true;
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::MatchRoleName
 // Return type     : bool 
 // Argument        : const char* szRoleName
 // Description     : 
 //-----------------------------------------------------------------
bool CKeyWordFilter::MatchRoleName(const char* szRoleName)
{
	static CRegexpT <wchar_t> regexp(L"^(?!_)(?!.*?_$)[a-zA-Z0-9_\\[\\]\\u4E00-\\u9FA5]+$");

	wchar_t wCharName[MAX_ROLENAMEMAX + 1] = { 0 };
	MultiByteToWideChar(CP_THREAD_ACP, 0, szRoleName, -1, wCharName, MAX_ROLENAMEMAX);

	MatchResult PreResult = regexp.MatchExact(wCharName);

	if (PreResult.IsMatched())
	{
		m_wRegexp.Compile(m_strRoleNameFilter.c_str());
		MatchResult result = m_wRegexp.Match(wCharName);

		return result.IsMatched();
	}
	return true;
}
 
 //!  匹配玩家取名规则
 //! \return bool  
 //! \param const char* szRoleName
 bool   CKeyWordFilter::MatchRoleNameA(const char* szRoleName)
 {
//   	static CRegexpT <char> regexp("^(?!_)(?!.*?_$)[a-zA-Z0-9_\\[\\]\\u4E00-\\u9FA5]+$");
//   	MatchResult PreResult = regexp.MatchExact(szRoleName);
//   
//   	if (PreResult.IsMatched())
//   	{
//   		m_Regexp.Compile(m_strRoleNameFilter.c_str());
//   		MatchResult result = m_Regexp.Match(szRoleName);
//   
//   		return result.IsMatched();
//   	}
  	return true;
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::MatchSqlCmd
 // Return type     : bool 
 // Argument        : const char* szSqlCmd
 // Description     : 
 //-----------------------------------------------------------------
 bool CKeyWordFilter::MatchSqlCmd(const char* szSqlCmd)
 {
      m_Regexp.Compile(m_strSqlCmdFilter.c_str());
      MatchResult result = m_Regexp.Match(szSqlCmd);
  
      return result.IsMatched();
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::MatchChat
 // Return type     : bool 
 // Argument        : const char* szChat
 // Description     : 
 //-----------------------------------------------------------------
 bool CKeyWordFilter::MatchChat(const char* szChat)
 {
  	wchar_t wChat[1024] = {0};	// MAX_MAILMESSAGELEGTH
  	MultiByteToWideChar(CP_THREAD_ACP, 0, szChat, -1, wChat, 1024);
  
      m_wRegexp.Compile(m_strChatFilter.c_str());
      MatchResult result = m_wRegexp.Match(wChat);
  
      return result.IsMatched();
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::ReplaceChat
 // Return type     : bool 
 // Argument        : const char* szChat
 // Description     : 
 void CKeyWordFilter::ReplaceChat(const char* szChat, char* szReplaced, int nSize)
 {
  	static const wchar_t sRule[16] = L"*";
  	m_wRegexp.Compile(m_strChatFilter.c_str());
  
  	wchar_t wChat[1024] = {0};
  	MultiByteToWideChar(CP_THREAD_ACP, 0, szChat, -1, wChat, 1024);
  
  	wchar_t * sTemp =  m_wRegexp.Replace(wChat, sRule);
  	WideCharToMultiByte(CP_THREAD_ACP,0,sTemp,-1,szReplaced,MAX_PATH,NULL,NULL);
  
      m_wRegexp.ReleaseString(sTemp);
 }
 
 //-----------------------------------------------------------------
 // Function name   : CKeyWordFilter::ReplaceSql
 // Return type     : bool 
 // Argument        : const char* szChat
 // Description     : 
 void CKeyWordFilter::ReplaceSql(const char* szSql, char* szReplaced, int nSize)
 {
  	const char sRule[16] = "*";
  	m_Regexp.Compile(m_strSqlCmdFilter.c_str());
  
  	char * sTemp =  m_Regexp.Replace(szSql, sRule);
  	int size = strlen(sTemp);
  	if (size < nSize)
  		nSize = size;
  
  	memcpy(szReplaced, sTemp, nSize);
  	szReplaced[nSize] = 0;
  	m_Regexp.ReleaseString(sTemp);
 }