#pragma once
#include <string>
#include <regex>
#include <sstream>

using namespace std;

class QuickEncrypt
{
public:
	QuickEncrypt(void);
	~QuickEncrypt(void);
	static string QuickEncrypt::decode(string src,string key);
	static string QuickEncrypt::encode(string src,string key);
};

