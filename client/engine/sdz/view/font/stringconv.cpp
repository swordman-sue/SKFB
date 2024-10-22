//#include "textureutil.h"
#include "stringconv.h"

//USING_NS_CC;

/* Code from GLIB gutf8.c starts here. */

#define UTF8_COMPUTE(Char, Mask, Len)        \
  if (Char < 128)                \
    {                        \
      Len = 1;                    \
      Mask = 0x7f;                \
    }                        \
  else if ((Char & 0xe0) == 0xc0)        \
    {                        \
      Len = 2;                    \
      Mask = 0x1f;                \
    }                        \
  else if ((Char & 0xf0) == 0xe0)        \
    {                        \
      Len = 3;                    \
      Mask = 0x0f;                \
    }                        \
  else if ((Char & 0xf8) == 0xf0)        \
    {                        \
      Len = 4;                    \
      Mask = 0x07;                \
    }                        \
  else if ((Char & 0xfc) == 0xf8)        \
    {                        \
      Len = 5;                    \
      Mask = 0x03;                \
    }                        \
  else if ((Char & 0xfe) == 0xfc)        \
    {                        \
      Len = 6;                    \
      Mask = 0x01;                \
    }                        \
  else                        \
    Len = -1;

#define UTF8_LENGTH(Char)            \
  ((Char) < 0x80 ? 1 :                \
   ((Char) < 0x800 ? 2 :            \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :            \
      ((Char) < 0x4000000 ? 5 : 6)))))


#define UTF8_GET(Result, Chars, Count, Mask, Len)    \
  (Result) = (Chars)[0] & (Mask);            \
  for ((Count) = 1; (Count) < (Len); ++(Count))        \
    {                            \
      if (((Chars)[(Count)] & 0xc0) != 0x80)        \
    {                        \
      (Result) = -1;                \
      break;                    \
    }                        \
      (Result) <<= 6;                    \
      (Result) |= ((Chars)[(Count)] & 0x3f);        \
    }

#define UNICODE_VALID(Char)            \
  ((Char) < 0x110000 &&                \
   (((Char) & 0xFFFFF800) != 0xD800) &&        \
   ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&    \
   ((Char) & 0xFFFE) != 0xFFFE)


static const char utf8_skip_data[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
  5, 5, 5, 6, 6, 1, 1
};

static const char *const g_utf8_skip = utf8_skip_data;

#define cc_utf8_next_char(p) (char *)((p) + g_utf8_skip[*(unsigned char *)(p)])


// 转换UTF8编码到UCS4编码  
int utf8_to_unicode( const char* pbUTF8, unsigned int& dwUCS4 )  
{  
	int     i, iLen;  
	char    b;  

	if( pbUTF8 == 0 )  
	{   // 参数错误  
		return 0;  
	}  

	b = *pbUTF8++;  
	if( b < 0x80 )  
	{  
		dwUCS4 = b;  
		return 1;  
	}  

	if( b < 0xC0 || b > 0xFD )  
	{   // 非法UTF8  
		return 0;   
	}  

	if( b < 0xE0 )  
	{  
		dwUCS4 = b & 0x1F;  
		iLen = 2;  
	}  
	else if( b < 0xF0 )  
	{  
		dwUCS4 = b & 0x0F;  
		iLen = 3;  
	}  
	else if( b < 0xF8 )  
	{  
		dwUCS4 = b & 7;  
		iLen = 4;  
	}  
	else if( b < 0xFC )  
	{  
		dwUCS4 = b & 3;  
		iLen = 5;  
	}  
	else  
	{  
		dwUCS4 = b & 1;  
		iLen = 6;  
	}  

	for( i = 1; i < iLen; i++ )  
	{  
		b = *pbUTF8++;  
		if( b < 0x80 || b > 0xBF )  
		{   // 非法UTF8  
			break;  
		}  

		dwUCS4 = (dwUCS4 << 6) + (b & 0x3F);  
	}  

	if( i < iLen )  
	{   // 非法UTF8  
		return 0;  
	}  
	else  
	{  
		return iLen;  
	}  
}  

// 转换UCS4编码到UTF8编码  
int unicode_to_utf8( unsigned int dwUCS4, char* pbUTF8 )  
{  
	const unsigned char  abPrefix[] = {0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};  
	const unsigned int adwCodeUp[] = {  
		0x80,           // U+00000000 ～ U+0000007F  
		0x800,          // U+00000080 ～ U+000007FF  
		0x10000,        // U+00000800 ～ U+0000FFFF  
		0x200000,       // U+00010000 ～ U+001FFFFF  
		0x4000000,      // U+00200000 ～ U+03FFFFFF  
		0x80000000      // U+04000000 ～ U+7FFFFFFF  
	};  

	int i, iLen;  

	// 根据UCS4编码范围确定对应的UTF-8编码字节数  
	iLen = sizeof(adwCodeUp) / sizeof(unsigned int);  
	for( i = 0; i < iLen; i++ )  
	{  
		if( dwUCS4 < adwCodeUp[i] )  
		{  
			break;  
		}  
	}  

	if( i == iLen )return 0;    // 无效的UCS4编码  

	iLen = i + 1;   // UTF-8编码字节数  
	if( pbUTF8 != NULL )  
	{   // 转换为UTF-8编码  
		for( ; i > 0; i-- )  
		{  
			pbUTF8[i] = static_cast<char>((dwUCS4 & 0x3F) | 0x80);  
			dwUCS4 >>= 6;  
		}  

		pbUTF8[0] = static_cast<char>(dwUCS4 | abPrefix[iLen - 1]);  
	}  

	return iLen;  
}  

int utf16_strlen(const unsigned short* wstr)
{
	int i = 0;
	while(wstr[i++]){}
	return i - 1;
}

int utf32_strlen(const unsigned int* wstr)
{
	int i = 0;
	while(wstr[i++]){}
	return i - 1;
}

int utf8_find_code(const char* str, char code)
{
	int i = 0;
	do 
	{
		if(str[i] == code)
		{
			return i;
		}
		++i;
	} while (str[i] != 0);
	return -1;
}

int utf8_find_code(const char* str, int len, char code)
{
	int i = 0;
	do 
	{
		if(str[i] == code)
		{
			return i;
		}
		++i;
	} while (str[i] != 0 && i < len);
	return -1;
}

int utf8_find_code(const char* str, const char* str_end, char code)
{
	int i = 0;
	do 
	{
		if(str[i] == code)
		{
			return i;
		}
		++i;
	} while (str[i] != 0 && str + i < str_end);
	return -1;
}

int utf8_strcmp(const char* str1, const char* str2, int len)
{
	return memcmp((void*)str1, (void*)str2, sizeof(char) * len);
}

int utf8_strcmp(const char* str1, const char* str2, int len1, int len2)
{
	if(len1 != len2)
		return -1;

	return memcmp((void*)str1, (void*)str2, sizeof(char) * len1);
}

int utf8_skip(const char* str)
{
	return g_utf8_skip[*(unsigned char *)(str)];
}


unsigned int utf8_get_char (const char * p)
{
  int i, mask = 0, len;
  unsigned int result;
  unsigned char c = (unsigned char) *p;

  UTF8_COMPUTE (c, mask, len);
  if (len == -1)
    return (unsigned int) - 1;
  UTF8_GET (result, p, i, mask, len);

  return result;
}

long utf8_strlen (const char * p, int max)
{
  long len = 0;
  const char *start = p;

  if (!(p != NULL || max == 0))
  {
      return 0;
  }

  if (max < 0)
    {
      while (*p)
    {
      p = cc_utf8_next_char (p);
      ++len;
    }
    }
  else
    {
      if (max == 0 || !*p)
    return 0;

      p = cc_utf8_next_char (p);

      while (p - start < max && *p)
    {
      ++len;
      p = cc_utf8_next_char (p);
    }

      /* only do the last len increment if we got a complete
       * char (don't count partial chars)
       */
      if (p - start == max)
    ++len;
    }

  return len;
}

void utf16_from_utf8(const char* str, unsigned short* dst)
{
	int len = utf8_strlen(str, -1);
	dst[len] = 0;

	for (int i = 0; i < len; ++i)
	{
		dst[i] = utf8_get_char(str);
		str = cc_utf8_next_char(str);
	}
}

void utf32_from_utf8(const char* str, unsigned int* dst)
{
	int len = utf8_strlen(str, -1);
	dst[len] = 0;

	for (int i = 0; i < len; ++i)
	{
		dst[i] = utf8_get_char(str);
		str = cc_utf8_next_char(str);
	}
}

FaceConvHelper::FaceConvHelper()
{

}

FaceConvHelper::~FaceConvHelper()
{
	m_faceStrMap.clear();
}

static FaceConvHelper s_faceConvHelper;

FaceConvHelper* FaceConvHelper::sharedFaceConvHelper()
{
	return &s_faceConvHelper;
}

#define UNICODE_STR_MAX_LEN	250
std::string FaceConvHelper::convertToFaceStr(const std::string& text)
{
	std::string face_str;
	face_str.reserve(500);

	unsigned int code, wstr[UNICODE_STR_MAX_LEN];
	int len = utf8_strlen(text.c_str(), -1);
	if(len >= UNICODE_STR_MAX_LEN)
		return "";

	utf32_from_utf8(text.c_str(), wstr);

	char str_code[10];
	int l;

	FACE_STR_MAP::iterator it;

	for(int i = 0; i < len; ++i)
	{
		code = wstr[i];

		it = m_faceStrMap.find(code);
		if(it != m_faceStrMap.end())
		{
			face_str.append(m_faceStrMap[code]);
		}
		else
		{
			l = unicode_to_utf8(code, str_code);
			str_code[l] = 0;

			face_str.append(str_code);
		}
	}

	return face_str;
}

std::string StringConvHelper::unicode_at(const std::string& str, int index)
{
	char str_code[10];
	unsigned int code, wstr[UNICODE_STR_MAX_LEN];

	//错误防御
	int len = utf8_strlen(str.c_str(), -1);
	if(len >= UNICODE_STR_MAX_LEN)
		return "";
	if(index < 0 || index > len)
		return "";

	utf32_from_utf8(str.c_str(), wstr);
	code = wstr[index];
	int l = unicode_to_utf8(code, str_code);
	str_code[l] = 0;
	return str_code;
}

std::string StringConvHelper::unicode_sub(const std::string& str, int start, int end)
{
	std::string sub_str;
	sub_str.reserve(500);
	char str_code[10];
	unsigned int wstr[UNICODE_STR_MAX_LEN];
	int l = 0;

	//错误防御
	int len = utf8_strlen(str.c_str(), -1);
	if(len >= UNICODE_STR_MAX_LEN)
		return "";
	if(start < 0 || end > len)
		return "";

	utf32_from_utf8(str.c_str(), wstr);

	for(int i = start; i < end; ++i)
	{
		l = unicode_to_utf8(wstr[i], str_code);
		str_code[l] = 0;

		sub_str.append(str_code);
	}

	return sub_str;
}

int StringConvHelper::unicode_length(const std::string& str)
{
	return utf8_strlen(str.c_str(), -1);
}