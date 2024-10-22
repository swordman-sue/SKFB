#include "StdAfx.h"
#include "HelperFunction.h"
#include <stdlib.h>
#include <algorithm>

namespace ygth {

void Str2Upper(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void Str2Lower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

int StrCopy(char* dest, int maxLen, const char* src)
{
	if (!dest || !src || maxLen <= 0)
		return 0;
	int n = _snprintf(dest, maxLen, src);
	if (n >= 0)
		return n + 1;
	dest[maxLen - 1] = 0;
	return maxLen;
}

}