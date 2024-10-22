#include "strutil.h"
#include <algorithm>
#include "common/md5/md5.h"
#include "json/json.h"

#include "cocos2d.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace ZQ
{

// Î¬ÎÄ×ÖÄ¸UnicodeÂë±í
static const unsigned int s_uygur_unicode_arr[] = 
{
	64490,
	65165,
	64491,
	65166,
	64492,
	64493,
	65257,
	65258,
	65167,
	65169,
	65170,
	65168,
	64342,
	64344,
	64345,
	64343,
	65173,
	65175,
	65176,
	65174,
	65181,
	65183,
	65184,
	65182,
	64378,
	64380,
	64381,
	64379,
	65189,
	65191,
	65192,
	65190,
	65193,
	65194,
	65197,
	65198,
	65199,
	65200,
	64394,
	64395,
	65201,
	65203,
	65204,
	65202,
	65205,
	65207,
	65208,
	65206,
	65229,
	65231,
	65232,
	65230,
	65233,
	65235,
	65236,
	65234,
	65237,
	65239,
	65240,
	65238,
	65241,
	65243,
	65244,
	65242,
	64402,
	64404,
	64405,
	64403,
	64467,
	64469,
	64470,
	64468,
	65245,
	65247,
	65248,
	65246,
	65249,
	65251,
	65252,
	65250,
	65253,
	65255,
	65256,
	65254,
	64426,
	64428,
	64429,
	64427,
	64494,
	64495,
	65261,
	65262,
	64496,
	64497,
	64471,
	64472,
	64498,
	64499,
	64473,
	64474,
	64500,
	64501,
	64475,
	64476,
	64478,
	64479,
	64502,
	64504,
	64465,
	64485,
	64484,
	64486,
	64487,
	64503,
	64505,
	64507,
	64466,
	64506,
	65263,
	64488,
	64489,
	65264,
	65265,
	65267,
	65268,
	65266,
	65163,
	65164,
	65275,
	65276
};

static std::map<unsigned int, bool> s_uygur_unicode_map;

void StrUtil::ToLowerCase(std::string& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

void StrUtil::ToUpperCase(std::string& str) 
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}

bool StrUtil::StartsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(0, patternLen);
	if (lowerCase)
		StrUtil::ToLowerCase(startOfThis);

	return (startOfThis == pattern);
}

bool StrUtil::EndsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
	if (lowerCase)
		StrUtil::ToLowerCase(endOfThis);

	return (endOfThis == pattern);
}

std::string StrUtil::LTrim(const std::string &str)
{
	std::string s = str;
	s.erase(0, s.find_first_not_of(" ")); 
	return s; 
}

std::string StrUtil::RTrim(const std::string &str)
{
	std::string s = str;
	s.erase(s.find_last_not_of(" ")+1);
	return s;
}

std::string StrUtil::Trim(const std::string &str)
{ 
	return LTrim(RTrim(str)); 
} 

const std::string StrUtil::ReplaceAll(const std::string& str, const std::string& src, const std::string& dest)
{
	std::string result = str;
	std::string::size_type pos = 0;
	while(1)
	{
		pos = result.find(src,pos);
		if (pos == std::string::npos) break;
		result.replace(pos,src.size(),dest);
		pos += dest.size();
	}
	return result;
}

std::vector<std::string> StrUtil::Split(const std::string& str, const std::string& c)
{
	std::vector<std::string> res;
	char *cstr = new char[str.size()+1];
	strcpy(cstr,str.c_str());
	cstr[str.size()]=0;

	char *p = strtok(cstr,c.c_str());
	while(p!=NULL)
	{
		res.push_back(p);
		p = strtok(NULL,c.c_str());
	}

	delete[] cstr;
	return res;
}

void StrUtil::SplitFileName(const std::string& qualifiedName, std::string& outBasename, std::string& outPath)
{
	std::string path = qualifiedName;
	// Replace \ with / first
	std::replace( path.begin(), path.end(), '\\', '/' );
	// split based on final /
	size_t i = path.find_last_of('/');

	if (i == std::string::npos)
	{
		outPath.clear();
		outBasename = qualifiedName;
	}
	else
	{
		outBasename = path.substr(i+1, path.size() - i - 1);
		outPath = path.substr(0, i+1);
	}
}

const std::string StrUtil::Md5(const std::string& str)
{
	return MD5String(str.c_str());
}

const std::string StrUtil::Md5(const char *str, GameType::UINT32 size)
{
	return MD5String(str, size);
}

const std::string StrUtil::UrlEncode(const std::string& str_in)
{
	if (str_in == "")
	{
		return "";
	}

	std::string str_out = "";
	static const char hex_map[] = "0123456789ABCDEF";

	const char *p = str_in.c_str();
	char c;
	while ((c = *p++) != '\0')
	{
		if (c == ' ')
		{
			str_out += '+';
		}
		else if (c < 0 || ( !isalpha(c) && !isdigit(c)))
		{
			str_out += '%';
			str_out += hex_map[(int)((unsigned char)c / 16)];
			str_out += hex_map[(int)((unsigned char)c % 16)];
		}
		else
		{
			str_out += c;
		}
	}

	return str_out;
}

const std::string StrUtil::Base64Encode(const std::string& strIn)
{
	std::string strOutStr;
	const char* strInCStr = strIn.c_str();
	char* strOutCStr = 0;
	cocos2d::base64Encode((const unsigned char*)strInCStr, strlen(strInCStr), &strOutCStr);
	if (strOutCStr != 0)
	{
		strOutStr = strOutCStr;
		delete strOutCStr;
	}
	return strOutStr;
}

const std::string StrUtil::Utf8ToGBK(const char *utf8_str)
{
#ifdef _WIN32
	if(utf8_str == 0)
	{
		return "";
	}

	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_str, -1, NULL,0); 
	unsigned short * wszGBK = new unsigned short[len+1];       
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_str, -1, (LPWSTR)wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
	std::string gbkString = szGBK;
	delete[] wszGBK;
	delete[] szGBK;
	return gbkString;
#endif

	return utf8_str;
}

const std::string StrUtil::AnsiToUtf8(const char *ansi_str)
{
#ifdef _WIN32
	if(!ansi_str)
	{
		return "";
	}
	//AnsiToUnicode
	int len ;
	wchar_t * unicode_result;
	len = MultiByteToWideChar(CP_ACP, 0, ansi_str,-1, NULL,0 ); 
	unicode_result = (wchar_t *)malloc((len+1)*sizeof(wchar_t));
	memset(unicode_result,0,(len+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0,ansi_str,-1,(LPWSTR)unicode_result,len);

	//UnicodeToUtf8
	len = WideCharToMultiByte( CP_UTF8, 0, unicode_result, -1, NULL, 0, NULL, NULL );
	char *utf8_result =(char *)malloc((len+1)*sizeof(char));
	memset(utf8_result, 0, sizeof(char) * (len + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, unicode_result, -1, utf8_result, len, NULL, NULL );
	std::string utf8_str = utf8_result;
	free(unicode_result);
	free(utf8_result);
	return utf8_str;
#endif
	return ansi_str;
}

bool StrUtil::IsUnicodeUygurLetter(unsigned int code)
{
	if (s_uygur_unicode_map.empty())
	{
		int arr_len = sizeof(s_uygur_unicode_arr) / sizeof(s_uygur_unicode_arr[0]);
		for (int i = 0; i < arr_len; ++i)
		{
			s_uygur_unicode_map.insert(std::make_pair(s_uygur_unicode_arr[i], true));
		}
	}

	return s_uygur_unicode_map.find(code) != s_uygur_unicode_map.end();
}

#if defined(_WP8)
Platform::String^ StrUtil::Utf8ToWStringForWindowsPlatform(const char *format, ...)
{
	Platform::String ^ ret;
	if (!format)
	{
		wchar_t wch[1] = { 0 };
		return ref new Platform::String(wch, __Platform_wcslen(wch));
	}
#if defined(_WP8) || defined(WIN32)
	const int max_len = 2 * 1024;
	char buffer[max_len] = { 0 };
	va_list argp;
	va_start(argp, format);
	int msg_length = ::vsprintf(buffer, format, argp);
	va_end(argp);

	wchar_t wsz_buf[max_len] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wsz_buf, sizeof(wsz_buf));
	return ref new Platform::String(wsz_buf, __Platform_wcslen(wsz_buf));
#endif
	printf("%s\n", "[StrUtil::Utf8ToWStringForWindowsPlatform] Error: not support this plat but wp8");
	return ret;
}

std::string StrUtil::WindowsPlatformToUTF8(Platform::String^ str)
{
	const int max_len = 2 * 1024;
	int iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_UTF8,
		0,
		str->Data(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	char * buffer = new char[iTextLen + 1];
	WideCharToMultiByte(CP_UTF8,
		0,
		str->Data(),
		-1,
		buffer,
		iTextLen,
		NULL,
		NULL);
	buffer[iTextLen] = '\0';
	std::string ret = std::string(buffer);
	delete buffer;
	return ret;
}


Platform::String^ StrUtil::GetJsonValueForKey(const char * jsonstr, const char * keystr)
{
	Json::Reader json_reader;
	Json::Value root;
	std::string json_str(jsonstr, strlen(jsonstr));

	if (!json_reader.parse(json_str, root))
	{
		return "";
	}
	const char * ret = root[keystr].asCString();
	return ZQ::StrUtil::Utf8ToWStringForWindowsPlatform(ret);
}

#endif
}
