#pragma once

#include "Const.h"
#include <string>
#include <map>


DWORD GetBitValueByIndex(char cIdx);
bool  IsBitSlotTrueByIndex(DWORD dwData, char cIdx);
bool  IsBitSlotTrueByValue(DWORD dwData, DWORD dwValue);
void  AddBitValueByIndex(DWORD& dwData, char cIdx);
void  DecBitValueByIndex(DWORD& dwData, char cIdx);
void  AddBitValueByValue(DWORD& dwData, DWORD dwValue);
void  DecBitValueByValue(DWORD& dwData, DWORD dwValue);
BYTE  CalcBitValueByValue(DWORD& dwData);

bool GetLocalIP(char* ip,int nSize);

std::string g_GB2312ToUTF8( const char* str ,char *pszOutBuff = NULL);

std::string g_UTF8ToGB2312( const char* str ,char *pszOutBuff);

int HttpPostRequest(char* hostname, int nPort, char* path, char* parameters, char *pszOutBuff, int nOutBuffLen);

BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);

// 取字符串的间隔符数量
int GetStringSpacingNum(const char* str, char spacing = ',');

bool IsLegalName(const char* name, int len);

void ChangeLocalChar(const char* pszSrc, char* pszDst, size_t nDstSize);

// 检测手机号 13,15,18
bool CheckPhoneNumber(const char *pszPhoneNumber);

// 检测是否是数字
bool CheckIsNumber(int nValue);

// 检测是否是字符
bool CheckIsChar(int nValue);

// 检测是否是汉字
bool CheckIsChinese(int nValue1, int nValue2);

BOOL MyBase64Encode(const BYTE *pbSrcData,int nSrcLen, LPSTR szDest,int *pnDestLen);

BOOL MyBase64Decode(LPCSTR szSrc, int nSrcLen, BYTE *pbDest, int *pnDestLen);

BYTE GetObejctTypeById(DWORD dwId);

void GetTMTime(tm *ptm, time_t t = -1);

// 是否机器人ID
bool IsRobotId(DWORD dwId);

// 是否月卡
bool IsMonthCard(DWORD dwRechargeId);

int g_Rand(int nMin = 1, int nMax = RAND_RANGE_MAX_10000);

// 处理恢复次数
int DoRecoveryTimes(DWORD &dwLastTime, DWORD dwCurTime, DWORD dwIntervalTime/*秒*/);

// 天转秒
int g_Day2Second(int nDay);

// 小时转秒
int g_Hour2Second(int nHour);

// 分钟转秒 
int g_Minute2Second(int nMinute);

// 取英雄培养属性索引
BYTE GetTrainProIdx(WORD wProType);

// 取英雄培养属性类型
BYTE GetTrainProType(BYTE byIdx);

// 取过去了多少天
int GetPassDays(DWORD dwLastTime, DWORD dwCurTime);

// 取过去了多少时间
int g_GetPassTime(DWORD dwBeginTime, DWORD dwEndTime);

// 是否同一天
bool IsTheSameDay(DWORD dwTime1, DWORD dwTime2);

// 取日期
DWORD g_Time2Date(DWORD dwTime);

// 除以 x / y
int g_DivideBy(int x, int y);

// 数字转字符串
std::string g_IntToString(DWORD dwValue);

// 纠正天数
WORD g_CorrectWeekDay(WORD wDay);

// 取星期天数
BYTE g_GetWeekDay(DWORD dwTime = 0);

// 取一年中的第几周(此接口有BUG)
WORD g_GetWeekOfYear(DWORD dwTime);

// 取最近的周一开始时间
DWORD GetLastMondayBeginTime();

// 拆分整型数字 
// nValue : 要拆分的整型数字
// nValueByteNum : 要拆分的整型数字字节数量
// nStartIndex : 从高位开始
// nGetByteNum : 要取的字节数量
int SplitInteger(int nValue, int nValueByteNum, int nStartIndex, int nGetByteNum);

// 组合帐号key
std::string MakeAccountKey(DWORD dwChannelType, const char *pszAccount);

// 组装DWORD
DWORD MakeDWord(WORD wHighValue, WORD wLowValue);

// 拆分DWORD
void SplitDWord(DWORD dwSrcValue, WORD &wHighValue, WORD &wLowValue);

// 组装WORD
WORD MakeWord(BYTE byHighValue, BYTE byLowValue);

// 拆分WORD
void SplitWord(WORD wSrcValue, BYTE &byHighValue, BYTE &byLowValue);

// 取当前时间(毫秒)
UINT64 g_GetCurrentTime();

// 是否Android
bool IsAndroid(BYTE byTerminalType);

// 组装渠道UID
DWORD MakeChannelUID(DWORD dwCPChannelID, DWORD dwSDKChannelID);

void SplitChannelUID(DWORD dwChannelUID, DWORD &dwCPChannelID, DWORD &dwSDKChannelID);

#define SETBIT(x,y) (x) |= (1ull << (y))						//将X的第Y位置1
#define CLRBIT(x,y) (x) &= ((1ull << (y))-1)					//将X的第Y位清0
#define CLRBIT2(x,y) (x) &= (~(1ull << (y)))					//将X的第Y位清0
#define GETBIT(x,y) ((x) & (1ull << (y)))						//获取X的第Y位是否为1
#define ISBIT(x,y) (((x) & (1ull << (y))) != 0)					//判断X的第Y位是否为1

#define CHECK_POINTER(p) if((p) == NULL) { MAPLOG(GUI_LEVEL,"指针为空[%s:%u]",__FUNCTION__, __LINE__); return; }
#define CHECK_POINTER_RET(p, ret) if((p) == NULL) { MAPLOG(GUI_LEVEL,"指针为空[%s:%u]",__FUNCTION__, __LINE__); return (ret); }

#define MAKE_GOODS_FIND_ID(type,id) 10000 * type + id

 //取间隔字段
 class CPXValue
 {
 private:
	std::string _str;
 public:
 	CPXValue(const char* str, int n = 0, char spacing = ',')
 	{
 		_str = str ? str : "";
 		int _start = 0;
 		int _end = (int)_str.find_first_of(spacing);
 
  		for (int i = 0; i < n; ++ i)
  		{
  			_start = (_end >= 0 && _end != _str.size()) ? (_end + 1) : _end;
  			_end = (int)_str.find_first_of(spacing, _start);
  		}
  
  		if (_start >= 0 && _start != _str.size())
  		{
   			if (_end > 0 && _end != _str.size())
   			{
   				_str = _str.substr(_start, _end - _start);
   			}
   			else
   			{
   				_str = _str.substr(_start);
   			}
  		}
  		else
  		{
			//_str.erase(_str.begin(),_str.end());
  			//_str.clear();
			_str = "";
  		}
 	}

 	// CHAR*返回
  	operator const char*() const
  	{
  		return _str.c_str();
  	}
  	// CHAR返回
  	operator const char() const
  	{
  		return (char)_str.c_str();
  	}
  	// INT返回
  	operator const int() const
  	{
  		return atoi(_str.c_str());
  	}
  	// UINT返回
  	operator const unsigned int() const
  	{
  		return (UINT)atoi(_str.c_str());
  	}
  	// INT8返回
  	operator const signed char() const
  	{
  		return (signed char)atoi(_str.c_str());
  	}
  	// UINT8返回
  	operator const unsigned char() const
  	{
  		return (unsigned char)atoi(_str.c_str());
  	}
  	// DWORD返回
   	operator DWORD() const
   	{
   		return (DWORD)atol(_str.c_str());
   	}
  	// WORD返回
  	operator WORD() const
  	{
  		return (WORD)atoi(_str.c_str());
  	}
  	// FLOAT返回
  	operator float() const
  	{
  		return (float)atof(_str.c_str());
  	}
  	// DOUBLE返回
  	operator double() const
  	{
  		return atof(_str.c_str());
  	}
  	// LONG返回
  	operator long() const
  	{
  		return atol(_str.c_str());
  	}
  	// SHORT返回
  	operator short() const
  	{
  		return (short)atoi(_str.c_str());
  	}

	// CHAR*返回
// 	operator const CHAR*() const
// 	{
// 		return _str.c_str();
// 	}
// 	// CHAR返回
// 	operator const CHAR() const
// 	{
// 		return (CHAR)_str.c_str();
// 	}
// 	// INT返回
// 	operator const INT() const
// 	{
// 		return atoi(_str.c_str());
// 	}
// 	// UINT返回
// 	operator const UINT() const
// 	{
// 		return (UINT)atoi(_str.c_str());
// 	}
// 	// INT8返回
// 	operator const INT8() const
// 	{
// 		return (INT8)atoi(_str.c_str());
// 	}
// 	// UINT8返回
// 	operator const UINT8() const
// 	{
// 		return (UINT8)atoi(_str.c_str());
// 	}
// 	// DWORD返回
// 	operator DWORD() const
// 	{
// 		return (DWORD)atol(_str.c_str());
// 	}
// 	// WORD返回
// 	operator WORD() const
// 	{
// 		return (WORD)atoi(_str.c_str());
// 	}
// 	// FLOAT返回
// 	operator FLOAT() const
// 	{
// 		return (FLOAT)atof(_str.c_str());
// 	}
// 	// DOUBLE返回
// 	operator DOUBLE() const
// 	{
// 		return atof(_str.c_str());
// 	}
// 	// LONG返回
// 	operator LONG() const
// 	{
// 		return atol(_str.c_str());
// 	}
// 	// SHORT返回
// 	operator SHORT() const
// 	{
// 		return (SHORT)atoi(_str.c_str());
// 	}
 };


 // 链接字符串 
 class LinkString
 {
 public:
	 LinkString() {};
	 ~LinkString(){};

 public:
	 void AddParam(const char *pszKey, const char *pszValue);

	 void AddParam(const char *pszKey, DWORD dwValue);

	 const std::string& CreateLinkString();

 public:
	 std::map<std::string, std::string> m_ParamList;
	 std::string m_strLinkString;
 };