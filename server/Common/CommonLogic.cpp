#include "stdafx.h"
#include "CommonLogic.h"

#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <atlenc.h>
#include "..\ToolKit\Rand.h"
#include <sys/timeb.h>    
//#include <time.inl>

bool GetLocalIP(char* ip, int nSize)  
{  
	//1.初始化wsa  
	WSADATA wsaData;  
	int ret=WSAStartup(MAKEWORD(2,2),&wsaData);  
	if (ret!=0)  
	{  
		return false;  
	}  
	//2.获取主机名  
	char hostname[64];  
	ret=gethostname(hostname,sizeof(hostname)-1);  
	if (ret==SOCKET_ERROR)  
	{  
		return false;  
	}  
	//3.获取主机ip  
	HOSTENT* host=gethostbyname(hostname);  
	if (host==NULL)  
	{  
		return false;  
	}  
	//4.转化为char*并拷贝返回  
	strncpy(ip,inet_ntoa(*(in_addr*)*host->h_addr_list), nSize);  
	return true;  
}

std::string g_GB2312ToUTF8( const char* str ,char *pszOutBuff)
{
	std::string result;
	// 	WCHAR *strSrc = NULL;
	// 	TCHAR *szRes = NULL;

	wchar_t szSrc[1024] = {0};
	char szRes[1024] = {0};

	int nSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0); 
	//strSrc = new WCHAR[nSize+1]; 
	MultiByteToWideChar(CP_ACP, 0, str, -1, szSrc, nSize); 

	nSize = WideCharToMultiByte(CP_UTF8, 0, szSrc, -1, NULL, 0, NULL, NULL); 
	//szRes = new TCHAR[nSize+1];
	int j=WideCharToMultiByte(CP_UTF8, 0, szSrc, -1, szRes, nSize, NULL, NULL); 

	result = szRes;

	// 安全处理
	if (pszOutBuff /*&& strlen(pszOutBuff) > result.size()*/)
		strcpy(pszOutBuff, szRes);

	return result; 
}

//函数实现 
int HttpPostRequest(char* hostname, int nPort, char* path, char* parameters, char *pszOutBuff, int nOutBuffLen)
 {
	 WSADATA WsaData;
	 WSAStartup(0x0101, &WsaData);

	 //初始化socket
	 struct hostent* host_addr = gethostbyname(hostname);
	 if (host_addr == NULL)
	 {
		 printf("Unable to locate host\n");
		 return false;
	 }

	 sockaddr_in sin;
	 sin.sin_family = AF_INET;
	 sin.sin_port = htons((unsigned short)nPort);
	 sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list);

	 int sock = socket(AF_INET, SOCK_STREAM, 0);
	 if (sock == -1)
	 {
		 printf("Iint failed\n");
		 return false;
	 }

	 //建立连接
	 if (connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1)
	 {
		 printf("connect failed\n");
		 return false;
	 }

	 //初始化发送信息
	 char send_str[2048] = {0};

	 //头信息
	 strcat(send_str, "POST ");
	 strcat(send_str, path);
	 strcat(send_str, " HTTP/1.1\r\n");
 	 strcat(send_str, "Host: ");
 	 strcat(send_str, hostname);
 	 strcat(send_str, "\r\n");
	 strcat(send_str, "Connection: keep-alive\r\n");

	 char content_header[100] = {0};
	 sprintf(content_header,"Content-Length: %d\r\n", strlen(parameters));
	 strcat(send_str, content_header);

	 //内容信息
	 strcat(send_str, "\r\n");
	 strcat(send_str, parameters);

	 if (send(sock, send_str, strlen(send_str),0) == -1)
	 {
		 printf("send failed\n");
		 return false;
	 }

	 //获取返回信息
	 //char recv_str[4096] = {0};
	 //if (recv(sock, recv_str, sizeof(recv_str), 0) == -1)
	 if (recv(sock, pszOutBuff, nOutBuffLen, 0) == -1)
	 {
		 printf("recv failed\n");
		 return false;
	 }

	 WSACleanup();

	 return true;
 }

// todo::日后调整
BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
{
	if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if(len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	//先转换到UTF-8
	char baseChar = bUpperCase ? 'A' : 'a';
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
	if(pUnicode == NULL)
		return FALSE;
	MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
	LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
	if(pUTF8 == NULL)
	{
		free(pUnicode);
		return FALSE;
	}
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
	pUTF8[cbUTF8] = '\0';

	unsigned char c;
	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)pUTF8;
	unsigned char *pDest = (unsigned char*)pBuf;
	while(*pSrc && cbDest < cbBufLen - 1)
	{
		c = *pSrc;
		if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			*pDest = c;
			++pDest;
			++cbDest;
		}
		else if(c == ' ')
		{
			*pDest = '+';
			++pDest;
			++cbDest;
		}
		else
		{
			//检查缓冲区大小是否够用？
			if(cbDest + 3 > cbBufLen - 1)
				break;
			pDest[0] = '%';
			pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
			pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
			pDest += 3;
			cbDest += 3;
		}
		++pSrc;
	}
	//null-terminator
	*pDest = '\0';
	free(pUnicode);
	free(pUTF8);
	return TRUE;
}


bool IsLegalName(const char* name, int len)
{
	if (!name || *name == '\0')
		return false;

	for (int i=0; i<len; ++i)
	{
		switch (name[i])
		{
		case ' ': case '`': case '~': case '!': case '@':
		case '#': case '$': case '%': case '^': case '&':
		case '*': case '(': case ')': case '-': case '_':
		case '+': case '{': case '}': case '[': case ']':
		case ';': case ':': case '\'': case '\"': case '|':  
		case '\\': case '<': case ',': case '>': case '.':
		case '?': case '/': case '=': case '0xA1': case '\n':
		case '\r': case 26 /* ^Z */:

		// 后面添加
// 		case '！': case '·':case '￥': case '……': case '—':
// 		case '（': case '）': case '——':case '【': case '】':
// 		case '：': case '；': case '、':case '《': case '，': 
// 		case '》': case '。':case '？':
			return false;
		}

		if (name[i] == '\0')
			return true;
	}

	return false;
}


DWORD GetBitValueByIndex( char cIdx)
{
	return 1<<cIdx;
}

bool IsBitSlotTrueByIndex( DWORD dwData, char cIdx)
{
	return (dwData&GetBitValueByIndex(cIdx))!=0;
}
bool  IsBitSlotTrueByValue(DWORD dwData, DWORD dwValue)
{
	return (dwData&dwValue)!=0;
}

void AddBitValueByIndex(DWORD& dwData, char cIdx )
{
	dwData = dwData|GetBitValueByIndex(cIdx);
}
void  DecBitValueByIndex(DWORD& dwData, char cIdx)
{
	dwData = dwData&(~GetBitValueByIndex(cIdx));
}
void  AddBitValueByValue(DWORD& dwData, DWORD dwValue)
{
	dwData = dwData|dwValue;

}
void  DecBitValueByValue(DWORD& dwData, DWORD dwValue)
{
	dwData = dwData&(~dwValue);
}
BYTE  CalcBitValueByValue(DWORD& dwData)
{
	BYTE bNum = 0;
	for (char cIdx = 1;cIdx<32;cIdx++)
	{
		if (IsBitSlotTrueByValue(dwData,GetBitValueByIndex( cIdx)))
		{
			++bNum;
		}
	}
	return bNum;
}

// 取字符串的间隔符数量
int GetStringSpacingNum(const char* str, char spacing)
{
	std::string temp = str ? str : "";
	int nCount = 0;	
	//_str = str ? str : "";
	int nPos = (int)temp.find_first_of(spacing);
	while (nPos != temp.npos /*&& nPos != _str.size()*/)
	{
		nCount++;
		nPos = (int)temp.find_first_of(spacing, nPos+1);
	}
	return nCount;
}

void ChangeLocalChar(const char* pszSrc, char* pszDst, size_t nDstSize)
{
	if (!pszSrc || !pszDst || nDstSize <= 0)	
		return;

	wchar_t wszTmp[1024];
	MultiByteToWideChar(CP_UTF8,0, (LPCSTR)pszSrc, -1, wszTmp, 1024);
	WideCharToMultiByte(CP_ACP, 0, wszTmp, -1 , (LPSTR)pszDst, (int)nDstSize, 0, 0);
}

// 检测手机号 13,15,18
bool CheckPhoneNumber(const char *pszPhoneNumber)
{
	if (!pszPhoneNumber)
		return 0;

	int nLen = (int)strlen(pszPhoneNumber);	
	if (0 == nLen)
		return 0;

	// 手机必须为11位
	if (11 != nLen)
		return 0;

	// 手机规则:
	// 第一位为1;第二位为3,5,8; 其他位为0-9;
	// 1   2  3  4  5  6  7  8  9
	// 49  50 51 52 53 54 55 56 57
	for (int i = 0; i < nLen; ++i)
	{
		int nValue = pszPhoneNumber[i];
		if (0 == i)
		{
			if (nValue != 49)
			{
				return false;
			}
		}
		else if (1 == i)
		{
			if (nValue != 51 && nValue != 53 && nValue != 56)
			{
				return false;
			}
		}
		else
		{
			if (nValue < 48 || nValue > 57)
			{
				return false;
			}
		}
	}

	return true;
}

// 检测是否是数字
bool CheckIsNumber(int nValue)
{
	return nValue >= 48 && nValue <= 57 ? true : false;
};

// 检测是否是字符
bool CheckIsChar(int nValue)
{
	return (nValue >= 65 && nValue <= 90) || (nValue >= 97 && nValue <= 122) ? true : false;
};

// 检测是否是汉字
bool CheckIsChinese(int nValue1, int nValue2)
{
	return (nValue1 >= -127 && nValue1 <= -1) && (nValue2 <= -1 || nValue2 >= 64)  ? true : false;
};


BOOL MyBase64Encode(const BYTE *pbSrcData,int nSrcLen,LPSTR szDest,int *pnDestLen)
{
	DWORD dwFlags = ATL_BASE64_FLAG_NONE;

	static const char s_chBase64EncodingTable[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',	'h',
		'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
		'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_' };

		if (!pbSrcData || !szDest || !pnDestLen)
		{
			return FALSE;
		}

		if(*pnDestLen < Base64EncodeGetRequiredLength(nSrcLen, dwFlags))
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		int nWritten( 0 );
		int nLen1( (nSrcLen/3)*4 );
		int nLen2( nLen1/76 );
		int nLen3( 19 );

		for (int i=0; i<=nLen2; i++)
		{
			if (i==nLen2)
				nLen3 = (nLen1%76)/4;

			for (int j=0; j<nLen3; j++)
			{
				DWORD dwCurr(0);
				for (int n=0; n<3; n++)
				{
					dwCurr |= *pbSrcData++;
					dwCurr <<= 8;
				}
				for (int k=0; k<4; k++)
				{
					BYTE b = (BYTE)(dwCurr>>26);
					*szDest++ = s_chBase64EncodingTable[b];
					dwCurr <<= 6;
				}
			}
			nWritten+= nLen3*4;

			if ((dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
			{
				*szDest++ = '\r';
				*szDest++ = '\n';
				nWritten+= 2;
			}
		}

		if (nWritten && (dwFlags & ATL_BASE64_FLAG_NOCRLF)==0)
		{
			szDest-= 2;
			nWritten -= 2;
		}

		nLen2 = (nSrcLen%3) ? (nSrcLen%3 + 1) : 0;
		if (nLen2)
		{
			DWORD dwCurr(0);
			for (int n=0; n<3; n++)
			{
				if (n<(nSrcLen%3))
					dwCurr |= *pbSrcData++;
				dwCurr <<= 8;
			}
			for (int k=0; k<nLen2; k++)
			{
				BYTE b = (BYTE)(dwCurr>>26);
				*szDest++ = s_chBase64EncodingTable[b];
				dwCurr <<= 6;
			}
			nWritten+= nLen2;
			if ((dwFlags & ATL_BASE64_FLAG_NOPAD)==0)
			{
				nLen3 = nLen2 ? 4-nLen2 : 0;
				for (int j=0; j<nLen3; j++)
				{
					*szDest++ = '=';
				}
				nWritten+= nLen3;
			}
		}

		*pnDestLen = nWritten;
		return TRUE;
};

int MyDecodeBase64Char(unsigned int ch)
{
	// returns -1 if the character is invalid
	// or should be skipped
	// otherwise, returns the 6-bit code for the character
	// from the encoding table
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A' + 0;	// 0 range starts at 'A'
	if (ch >= 'a' && ch <= 'z')
		return ch - 'a' + 26;	// 26 range starts at 'a'
	if (ch >= '0' && ch <= '9')
		return ch - '0' + 52;	// 52 range starts at '0'
	if (ch == '-')
		return 62;
	if (ch == '_')
		return 63;
	return -1;
};

BOOL MyBase64Decode(LPCSTR szSrc, int nSrcLen, BYTE *pbDest, int *pnDestLen)
{
	// walk the source buffer
	// each four character sequence is converted to 3 bytes
	// CRLFs and =, and any characters not in the encoding table
	// are skiped

	if (szSrc == NULL || pnDestLen == NULL)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	LPCSTR szSrcEnd = szSrc + nSrcLen;
	int nWritten = 0;

	BOOL bOverflow = (pbDest == NULL) ? TRUE : FALSE;

	while (szSrc < szSrcEnd &&(*szSrc) != 0)
	{
		DWORD dwCurr = 0;
		int i;
		int nBits = 0;
		for (i=0; i<4; i++)
		{
			if (szSrc >= szSrcEnd)
				break;
			int nCh = MyDecodeBase64Char(*szSrc);
			szSrc++;
			if (nCh == -1)
			{
				// skip this char
				i--;
				continue;
			}
			dwCurr <<= 6;
			dwCurr |= nCh;
			nBits += 6;
		}

		if(!bOverflow && nWritten + (nBits/8) > (*pnDestLen))
			bOverflow = TRUE;

		// dwCurr has the 3 bytes to write to the output buffer
		// left to right
		dwCurr <<= 24-nBits;
		for (i=0; i<nBits/8; i++)
		{
			if(!bOverflow)
			{
				*pbDest = (BYTE) ((dwCurr & 0x00ff0000) >> 16);
				pbDest++;
			}
			dwCurr <<= 8;
			nWritten++;
		}

	}

	*pnDestLen = nWritten;

	if(bOverflow)
	{
		if(pbDest != NULL)
		{
			ATLASSERT(FALSE);
		}

		return FALSE;
	}

	return TRUE;
};

BYTE GetObejctTypeById(DWORD dwId)
{
	return BYTE(dwId * 0.0001);
}

void GetTMTime(tm *ptm, time_t t)
 {
	if (-1 == t)
		t = time(NULL);

	localtime_s(ptm, &t);
 }

// 是否机器人ID
bool IsRobotId(DWORD dwId)
{
	return dwId >= ID_ROBOT_MIN && dwId <= ID_ROBOT_MAX ? true : false;
}

// 是否月卡
bool IsMonthCard(DWORD dwRechargeId)
{
	// ID规则(4位数) type * 1000 + id
	dwRechargeId = dwRechargeId * 0.001;
	return dwRechargeId == 1 ? true : false;
}

int g_Rand(int nMin, int nMax)
{
	if (nMin > nMax)
		return 0;

	return Rand::Instance()->irand(nMin, nMax);
}

// 处理恢复次数
int DoRecoveryTimes(DWORD &dwLastTime, DWORD dwCurTime, DWORD dwIntervalTime/*秒*/)
{
	if (dwCurTime <= dwLastTime)
		return 0;

	if (!dwIntervalTime)
		return 0;

	DWORD dwPassTime = dwCurTime - dwLastTime;

	int nTimes = int(dwPassTime / dwIntervalTime);

	dwLastTime += (nTimes * dwIntervalTime);
	//dwRemainTime = dwPassTime % dwIntervalTime;

	return nTimes;
}

// 天转秒
int g_Day2Second(int nDay)
{
	return g_Hour2Second(nDay * 24);
}

// 小时转秒
int g_Hour2Second(int nHour)
{
	return nHour * 60 * 60; 
}

// 分钟转秒 
int g_Minute2Second(int nMinute)
{
	return nMinute * 60;
}

// 取英雄培养属性索引
BYTE GetTrainProIdx(WORD wProType)
{
	return wProType ? wProType - 1 : 0;
}

// 取英雄培养属性类型
BYTE GetTrainProType(BYTE byIdx)
{
	return byIdx + 1;
}

// 取过去了多少天(不满一天当天处理)(todo::暂时不使用)
int GetPassDaysEx(DWORD dwLastTime, DWORD dwCurTime)
{
	// 数据合法性检测
	if (dwLastTime > dwCurTime)
	{
		//printf("出现dwLastTime[%d] > dwCurTime[%d]的情况\n", dwLastTime, dwCurTime);
		return 0;
	}
		
	// 有可能会出现这种情况
	if (dwLastTime == dwCurTime)
	{
		//printf("出现dwLastTime[%d] == dwCurTime[%d]的情况\n", dwLastTime, dwCurTime);
		return 1;
	}

	// 计算第一天的总时间
	tm tmLastTime;
	time_t t = dwLastTime;
	localtime_s(&tmLastTime, &t);
	int nRemainHour = 23 - tmLastTime.tm_hour;	// 剩余小时(还剩多少时间满一天)
	int nRemainMin = 59 - tmLastTime.tm_min;	// 剩余分钟(还剩多少时间满一天)
	int nRemainSec = 60 - tmLastTime.tm_sec;	// 剩余秒数(还剩多少时间满一天)

	// 剩余总的时间(秒)
	int nRemainTotalSec = nRemainHour * 60 * 60 + nRemainMin * 60 + nRemainSec;

	// 计算过去了多少时间
	int nPassTime = dwCurTime - dwLastTime;

	// 还在当天
	if (nPassTime < nRemainTotalSec)
		return 1;

	// 减掉第一天的时间(因为第一天肯定不是按24小时算)
	nPassTime -= nRemainTotalSec;

	return int(nPassTime / (60 * 60 * 24)) + 1 + 1;
}

// 取过去了多少天(不满一天当天处理)
int GetPassDays(DWORD dwLastTime, DWORD dwCurTime)
{
	// 数据合法性检测
	if (dwLastTime > dwCurTime)
	{
		//printf("出现dwLastTime[%d] > dwCurTime[%d]的情况\n", dwLastTime, dwCurTime);
		return 0;
	}

	// 有可能会出现这种情况
	if (dwLastTime == dwCurTime)
	{
		//printf("出现dwLastTime[%d] == dwCurTime[%d]的情况\n", dwLastTime, dwCurTime);
		return 1;
	}

	// 计算第一天的总时间
	tm tmLastTime;
	time_t t = dwLastTime;
	localtime_s(&tmLastTime, &t);

	// 一天有多少秒
	int nTotalSecInOneDay = g_Day2Second(1);

	// 当天过去了多少秒
	int nPassSecInThatDay = g_Hour2Second(tmLastTime.tm_hour) + g_Minute2Second(tmLastTime.tm_min) + tmLastTime.tm_sec;

	// 当天还差多少秒足够一天
	int nNeedSec = nTotalSecInOneDay - nPassSecInThatDay;

	// 计算过去了多少时间
	int nPassTime = dwCurTime - dwLastTime;

	// 还在当天
	if (nPassTime < nNeedSec)
		return 1;

	// 减掉第一天的时间(因为第一天有可能没满24小时,但都是按一天算,所以特殊处理)
	nPassTime -= nNeedSec;

	// 过去了多少天
	int nPassDay = nPassTime / (60 * 60 * 24);

	// 加上第一天
	nPassDay += 1;

	// 加上最后一天(todo::最后一天肯定没满24小时,所以，也特殊处理当一天算)
	nPassDay += 1;

	return nPassDay;
}

// 取过去了多少时间
int g_GetPassTime(DWORD dwBeginTime, DWORD dwEndTime)
{
	if (dwEndTime <= dwBeginTime)
		return 0;

	return dwEndTime - dwBeginTime;
}

// 是否同一天
bool IsTheSameDay(DWORD dwTime1, DWORD dwTime2)
{
	if (0 == dwTime1 || 0 == dwTime2)
		return false;

	tm tmTime1;
	time_t t1 = dwTime1;

	tm tmTime2;
	time_t t2 = dwTime2;

	localtime_s(&tmTime1, &t1);

	localtime_s(&tmTime2, &t2);

	// 不同的年份
	if (tmTime1.tm_year != tmTime2.tm_year)
		return false;
	
	return tmTime1.tm_yday == tmTime2.tm_yday ? true : false;
}

// 取日期
DWORD g_Time2Date(DWORD dwTime)
{
	tm tmTime;
	time_t t = dwTime;

	localtime_s(&tmTime, &t);

	return (tmTime.tm_year + 1900) * 10000 + (tmTime.tm_mon + 1) * 100 + tmTime.tm_mday;
}

// 除 Divide
int g_DivideBy(int x, int y)
{
	if (0 == x || 0 == y)
		return 0;

	return x / y;
}

// 数字转字符串
std::string g_IntToString(DWORD dwValue)
{
	char szStrValue[16] = { 0 };
	sprintf_s(szStrValue, sizeof(szStrValue), "%d", dwValue);
	return szStrValue;
}

// 纠正天数
WORD g_CorrectWeekDay(WORD wDay)
{
	return wDay ? wDay : 7;  // 0代表周日
}

// 取星期天数
BYTE g_GetWeekDay(DWORD dwTime)
{
	tm tmTime;
	time_t t = dwTime ? dwTime : time(NULL);
	localtime_s(&tmTime, &t);
	return tmTime.tm_wday ? tmTime.tm_wday : 7;
}

// 取一年中的第几周
WORD g_GetWeekOfYear(DWORD dwTime)
{
	if (!dwTime)
		return 0;

	time_t tmp = dwTime;
	tm tmDetailTime;
	localtime_s(&tmDetailTime, &tmp);

	int nYDay = tmDetailTime.tm_yday + 1;

	// 	if (!tmDetailTime.tm_yday)
	// 		return 0;

	return nYDay / 7 + (nYDay % 7 ? 1 : 0);

// 	if (!tmDetailTime.tm_yday)
// 		return 0;
// 
// 	return tmDetailTime.tm_yday / 7 + (tmDetailTime.tm_yday % 7 ? 1 : 0);
}

// 取最近的周一开始时间
DWORD GetLastMondayBeginTime()
{
	// 当前时间
	time_t tCurTime = time(NULL);
	tm tmDetail;
	localtime_s(&tmDetail, &tCurTime);

	int nCurWeekDay = tmDetail.tm_wday;

	// 周天当作星期7
	if (0 == nCurWeekDay)
		nCurWeekDay = 7;

	// 组装最近的周一开始时间
	int nPassedDay = nCurWeekDay - 1;
	time_t dwLastMondayTime = tCurTime - (24 * 60 * 60) * nPassedDay;
	localtime_s(&tmDetail, &dwLastMondayTime);
	tmDetail.tm_sec = 0;
	tmDetail.tm_min = 0;
	tmDetail.tm_hour = 0;

	return mktime(&tmDetail);
}

// 拆分整整 nValue:  
int SplitInteger(int nValue, int nValueByteNum, int nStartIndex, int nGetByteNum)
{
	char szFormat[16] = { 0 };
	sprintf_s(szFormat, sizeof(szFormat), "%s0%dd", "%", nValueByteNum);

	char szSrcBuff[32] = { 0 };
	sprintf_s(szSrcBuff, sizeof(szSrcBuff), szFormat, nValue);

	char szDestBuff[32] = { 0 };
	int nDestBuffNum = 0;

	for (int i = nStartIndex; i < nStartIndex + nGetByteNum; ++i)
	{
		szDestBuff[nDestBuffNum++] = szSrcBuff[i];
	}

	return atoi(szDestBuff);
}

// 组合帐号key
std::string MakeAccountKey(DWORD dwChannelType, const char *pszAccount)
{
	char szAccountKey[128] = { 0 };

	sprintf_s(szAccountKey, sizeof(szAccountKey)-1, "%s_%d", pszAccount, dwChannelType);

	return szAccountKey;
}

// 组装DWORD
DWORD MakeDWord(WORD wHighValue, WORD wLowValue)
{
	// 65535
	return wHighValue * 100000 + wLowValue;
}

// 拆分DWORD
void SplitDWord(DWORD dwSrcValue, WORD &wHighValue, WORD &wLowValue)
{
	if (!dwSrcValue)
	{
		wHighValue = 0;
		wLowValue = 0;
	}
	else
	{
		wHighValue = dwSrcValue * 0.00001;
		wLowValue = dwSrcValue % 100000;
	}
}

// 组装WORD
WORD MakeWord(BYTE byHighValue, BYTE byLowValue)
{
	// 65535 = 256
	return WORD(byHighValue * 1000 + byLowValue);
}

// 拆分WORD
void SplitWord(WORD wSrcValue, BYTE &byHighValue, BYTE &byLowValue)
{
	if (!wSrcValue)
	{
		byHighValue = 0;
		byLowValue = 0;
	}
	else
	{
		byHighValue = wSrcValue * 0.001;
		byLowValue = wSrcValue % 1000;
	}
}


// 取当前时间(毫秒)
UINT64 g_GetCurrentTime()
{
	__timeb64 curTime;
	_ftime64_s(&curTime);
	return curTime.time * 1000 + curTime.millitm;
}

// 是否Android
bool IsAndroid(BYTE byTerminalType)
{
	return TERMINAL_ANDROID == byTerminalType ? true : false;
}

// 组装渠道UID
DWORD MakeChannelUID(DWORD dwCPChannelID, DWORD dwSDKChannelID)
{
	return dwCPChannelID * 100000 + dwSDKChannelID;
}

void SplitChannelUID(DWORD dwChannelUID, DWORD &dwCPChannelID, DWORD &dwSDKChannelID)
{
	if (!dwChannelUID)
		return;

	dwCPChannelID = dwChannelUID * 0.00001;
	dwSDKChannelID = dwChannelUID % 100000;
}


void LinkString::AddParam(const char *pszKey, const char *pszValue)
{
	m_ParamList.insert(std::make_pair(pszKey, pszValue));
}

void LinkString::AddParam(const char *pszKey, DWORD dwValue)
{
	char szBuff[16] = {0};
	sprintf_s(szBuff, sizeof(szBuff), "%d", dwValue);
	AddParam(pszKey, szBuff);
}

const std::string& LinkString::CreateLinkString()
{
	m_strLinkString = "";
	size_t nCount = 0;

	std::map<std::string, std::string>::iterator iter = m_ParamList.begin();
	for (; iter != m_ParamList.end(); ++iter)
	{
		++nCount;

		m_strLinkString.append(iter->first); //  key
		m_strLinkString.append("=");		 // =
		m_strLinkString.append(iter->second);// value

		//拼接时，不包括最后一个&字符
		if (nCount != m_ParamList.size())
			m_strLinkString.append("&");
	}

	return m_strLinkString;
}