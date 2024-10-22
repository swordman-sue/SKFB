#ifndef ZQ_STRUTIL_H
#define ZQ_STRUTIL_H

#include <string>
#include <sstream>
#include "typedef.h"

namespace ZQ
{

class StrUtil
{
public:
	static void ToLowerCase(std::string& str);
	static void ToUpperCase(std::string& str);

	static bool StartsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);
	static bool EndsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);

	static std::string LTrim(const std::string &str);		// 去掉左空格
	static std::string RTrim(const std::string &str);		// 去掉右空格
	static std::string Trim(const std::string &str);		// 去掉前后空格

	static const std::string ReplaceAll(const std::string& str, const std::string& src, const std::string& dest);

	static void SplitFileName(const std::string& qualifiedName, std::string& outBasename, std::string& outPath);
	static std::vector<std::string> Split(const std::string& str, const std::string& c);

	static const std::string Md5(const std::string& str);
	static const std::string Md5(const char *str, GameType::UINT32 size);
	static const std::string UrlEncode(const std::string& str_in);
	static const std::string Base64Encode(const std::string& str_in);

	// 将字符串转成基础数据类型,非基础类型需显示定义
	template<class T>
	static const bool StringToBaseType(const std::string& str, T& val)
	{
		std::istringstream stream;
		stream.str(str);
		if (stream>>val)
		{
			return true;
		}
		return false;
	}

	static const bool StringToBaseType(const std::string& str, std::string& val)
	{	
		val = str;
		return true;
	}

	// 将基础数据类型转成字符串,非基础类型需显示定义
	template<class T>
	static const bool BaseTypeToString(const T& src, std::string& tar)
	{
		std::ostringstream stream;
		if (stream<<src)
		{
			tar = stream.str();
			return true;
		}
		return false;
	}

	static const std::string Utf8ToGBK(const char *utf8_str);

	static const std::string AnsiToUtf8(const char *utf8_str);

	// 是否Unicode英文字母或数字
	inline static bool IsUnicodeEngLetterOrNumber(unsigned int code)
	{
		return (code >= 65 && code <= 90) || (code >= 97 && code <= 122) || (code >= 48 && code <= 57);
	}

	// 是否Unicode空白字符
	inline static bool IsUnicodeSpace(unsigned int code)
	{
		return code == 32 || code == 9;
	}

	// 是否ASCII基本区
	inline static bool IsBaseASCII(unsigned int code)
	{
		return code <= 127;
	}

	// 是否Unicode维文字母
	static bool IsUnicodeUygurLetter(unsigned int code);

#if defined(_WP8)
	// 缓冲区大小 2k
	static Platform::String^ Utf8ToWStringForWindowsPlatform(const char *format, ...);
	static std::string WindowsPlatformToUTF8(Platform::String^ str);
	static Platform::String^ GetJsonValueForKey(const char * json, const char * key);
#endif
};


}

#endif
