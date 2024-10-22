
#ifndef MD5_H
#define MD5_H

#include <string>

std::string MD5String(const char* srcString );
std::string MD5String(const char* srcString, unsigned int len);
bool MD5Check(char *md5string, char* string);

#endif