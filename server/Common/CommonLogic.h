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

// ȡ�ַ����ļ��������
int GetStringSpacingNum(const char* str, char spacing = ',');

bool IsLegalName(const char* name, int len);

void ChangeLocalChar(const char* pszSrc, char* pszDst, size_t nDstSize);

// ����ֻ��� 13,15,18
bool CheckPhoneNumber(const char *pszPhoneNumber);

// ����Ƿ�������
bool CheckIsNumber(int nValue);

// ����Ƿ����ַ�
bool CheckIsChar(int nValue);

// ����Ƿ��Ǻ���
bool CheckIsChinese(int nValue1, int nValue2);

BOOL MyBase64Encode(const BYTE *pbSrcData,int nSrcLen, LPSTR szDest,int *pnDestLen);

BOOL MyBase64Decode(LPCSTR szSrc, int nSrcLen, BYTE *pbDest, int *pnDestLen);

BYTE GetObejctTypeById(DWORD dwId);

void GetTMTime(tm *ptm, time_t t = -1);

// �Ƿ������ID
bool IsRobotId(DWORD dwId);

// �Ƿ��¿�
bool IsMonthCard(DWORD dwRechargeId);

int g_Rand(int nMin = 1, int nMax = RAND_RANGE_MAX_10000);

// ����ָ�����
int DoRecoveryTimes(DWORD &dwLastTime, DWORD dwCurTime, DWORD dwIntervalTime/*��*/);

// ��ת��
int g_Day2Second(int nDay);

// Сʱת��
int g_Hour2Second(int nHour);

// ����ת�� 
int g_Minute2Second(int nMinute);

// ȡӢ��������������
BYTE GetTrainProIdx(WORD wProType);

// ȡӢ��������������
BYTE GetTrainProType(BYTE byIdx);

// ȡ��ȥ�˶�����
int GetPassDays(DWORD dwLastTime, DWORD dwCurTime);

// ȡ��ȥ�˶���ʱ��
int g_GetPassTime(DWORD dwBeginTime, DWORD dwEndTime);

// �Ƿ�ͬһ��
bool IsTheSameDay(DWORD dwTime1, DWORD dwTime2);

// ȡ����
DWORD g_Time2Date(DWORD dwTime);

// ���� x / y
int g_DivideBy(int x, int y);

// ����ת�ַ���
std::string g_IntToString(DWORD dwValue);

// ��������
WORD g_CorrectWeekDay(WORD wDay);

// ȡ��������
BYTE g_GetWeekDay(DWORD dwTime = 0);

// ȡһ���еĵڼ���(�˽ӿ���BUG)
WORD g_GetWeekOfYear(DWORD dwTime);

// ȡ�������һ��ʼʱ��
DWORD GetLastMondayBeginTime();

// ����������� 
// nValue : Ҫ��ֵ���������
// nValueByteNum : Ҫ��ֵ����������ֽ�����
// nStartIndex : �Ӹ�λ��ʼ
// nGetByteNum : Ҫȡ���ֽ�����
int SplitInteger(int nValue, int nValueByteNum, int nStartIndex, int nGetByteNum);

// ����ʺ�key
std::string MakeAccountKey(DWORD dwChannelType, const char *pszAccount);

// ��װDWORD
DWORD MakeDWord(WORD wHighValue, WORD wLowValue);

// ���DWORD
void SplitDWord(DWORD dwSrcValue, WORD &wHighValue, WORD &wLowValue);

// ��װWORD
WORD MakeWord(BYTE byHighValue, BYTE byLowValue);

// ���WORD
void SplitWord(WORD wSrcValue, BYTE &byHighValue, BYTE &byLowValue);

// ȡ��ǰʱ��(����)
UINT64 g_GetCurrentTime();

// �Ƿ�Android
bool IsAndroid(BYTE byTerminalType);

// ��װ����UID
DWORD MakeChannelUID(DWORD dwCPChannelID, DWORD dwSDKChannelID);

void SplitChannelUID(DWORD dwChannelUID, DWORD &dwCPChannelID, DWORD &dwSDKChannelID);

#define SETBIT(x,y) (x) |= (1ull << (y))						//��X�ĵ�Yλ��1
#define CLRBIT(x,y) (x) &= ((1ull << (y))-1)					//��X�ĵ�Yλ��0
#define CLRBIT2(x,y) (x) &= (~(1ull << (y)))					//��X�ĵ�Yλ��0
#define GETBIT(x,y) ((x) & (1ull << (y)))						//��ȡX�ĵ�Yλ�Ƿ�Ϊ1
#define ISBIT(x,y) (((x) & (1ull << (y))) != 0)					//�ж�X�ĵ�Yλ�Ƿ�Ϊ1

#define CHECK_POINTER(p) if((p) == NULL) { MAPLOG(GUI_LEVEL,"ָ��Ϊ��[%s:%u]",__FUNCTION__, __LINE__); return; }
#define CHECK_POINTER_RET(p, ret) if((p) == NULL) { MAPLOG(GUI_LEVEL,"ָ��Ϊ��[%s:%u]",__FUNCTION__, __LINE__); return (ret); }

#define MAKE_GOODS_FIND_ID(type,id) 10000 * type + id

 //ȡ����ֶ�
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

 	// CHAR*����
  	operator const char*() const
  	{
  		return _str.c_str();
  	}
  	// CHAR����
  	operator const char() const
  	{
  		return (char)_str.c_str();
  	}
  	// INT����
  	operator const int() const
  	{
  		return atoi(_str.c_str());
  	}
  	// UINT����
  	operator const unsigned int() const
  	{
  		return (UINT)atoi(_str.c_str());
  	}
  	// INT8����
  	operator const signed char() const
  	{
  		return (signed char)atoi(_str.c_str());
  	}
  	// UINT8����
  	operator const unsigned char() const
  	{
  		return (unsigned char)atoi(_str.c_str());
  	}
  	// DWORD����
   	operator DWORD() const
   	{
   		return (DWORD)atol(_str.c_str());
   	}
  	// WORD����
  	operator WORD() const
  	{
  		return (WORD)atoi(_str.c_str());
  	}
  	// FLOAT����
  	operator float() const
  	{
  		return (float)atof(_str.c_str());
  	}
  	// DOUBLE����
  	operator double() const
  	{
  		return atof(_str.c_str());
  	}
  	// LONG����
  	operator long() const
  	{
  		return atol(_str.c_str());
  	}
  	// SHORT����
  	operator short() const
  	{
  		return (short)atoi(_str.c_str());
  	}

	// CHAR*����
// 	operator const CHAR*() const
// 	{
// 		return _str.c_str();
// 	}
// 	// CHAR����
// 	operator const CHAR() const
// 	{
// 		return (CHAR)_str.c_str();
// 	}
// 	// INT����
// 	operator const INT() const
// 	{
// 		return atoi(_str.c_str());
// 	}
// 	// UINT����
// 	operator const UINT() const
// 	{
// 		return (UINT)atoi(_str.c_str());
// 	}
// 	// INT8����
// 	operator const INT8() const
// 	{
// 		return (INT8)atoi(_str.c_str());
// 	}
// 	// UINT8����
// 	operator const UINT8() const
// 	{
// 		return (UINT8)atoi(_str.c_str());
// 	}
// 	// DWORD����
// 	operator DWORD() const
// 	{
// 		return (DWORD)atol(_str.c_str());
// 	}
// 	// WORD����
// 	operator WORD() const
// 	{
// 		return (WORD)atoi(_str.c_str());
// 	}
// 	// FLOAT����
// 	operator FLOAT() const
// 	{
// 		return (FLOAT)atof(_str.c_str());
// 	}
// 	// DOUBLE����
// 	operator DOUBLE() const
// 	{
// 		return atof(_str.c_str());
// 	}
// 	// LONG����
// 	operator LONG() const
// 	{
// 		return atol(_str.c_str());
// 	}
// 	// SHORT����
// 	operator SHORT() const
// 	{
// 		return (SHORT)atoi(_str.c_str());
// 	}
 };


 // �����ַ��� 
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