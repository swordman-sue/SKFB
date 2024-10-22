#ifndef _CC_STRING_CONV_H_
#define _CC_STRING_CONV_H_

#include "cocos2d.h"

int utf8_to_unicode( const char* pbUTF8, unsigned int& dwUCS4 );

int unicode_to_utf8( unsigned int dwUCS4, char* pbUTF8 );

int utf16_strlen(const unsigned short* wstr);

int utf32_strlen(const unsigned int* wstr);

int utf8_find_code(const char* str, char code);
int utf8_find_code(const char* str, int len, char code);
int utf8_find_code(const char* str, const char* str_end, char code);

int utf8_strcmp(const char* str1, const char* str2, int len);
int utf8_strcmp(const char* str1, const char* str2, int len1, int len2);

int utf8_skip(const char* str);

unsigned int utf8_get_char (const char * p);

long utf8_strlen (const char * p, int max);

void utf16_from_utf8(const char* str, unsigned short* dst);

void utf32_from_utf8(const char* str, unsigned int* dst);

class FaceConvHelper
{
public:
	FaceConvHelper();

	~FaceConvHelper();

	static FaceConvHelper* sharedFaceConvHelper();

	void registerFace(unsigned int face_code, const std::string& face_str)
	{
		m_faceStrMap[face_code] = face_str;
	}

	std::string convertToFaceStr(const std::string& text);

private:
	typedef std::map<unsigned int, std::string> FACE_STR_MAP;
	FACE_STR_MAP m_faceStrMap;
};

class StringConvHelper
{
public:
	static std::string unicode_at(const std::string& str, int index);
	static std::string unicode_sub(const std::string& str, int start, int end);
	static int unicode_length(const std::string& str);
};

#endif