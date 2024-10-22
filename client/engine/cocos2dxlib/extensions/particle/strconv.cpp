#include "strconv.h"

NS_CC_BEGIN
	//-----------------------------------------------------------------------
	std::string StrConv::toString(float val, unsigned short precision, 
		unsigned short width, char fill, int flags)
	{
		char tmpbuf[64];
		memset(tmpbuf, 0, 64);
		sprintf( tmpbuf, "%f", val);

		return std::string(tmpbuf);
	}
	//-----------------------------------------------------------------------
	float StrConv::parseReal(const std::string& val, float defaultValue)
	{
		float ret = defaultValue;

		sscanf(val.c_str(), "%f", &ret);
		return ret;
	}

	//-----------------------------------------------------------------------
	std::string StrConv::toString(int val, 
		unsigned short width, char fill, int flags)
	{
		char tmpbuf[64];
		memset(tmpbuf, 0, 64);
		sprintf( tmpbuf, "%d", val);

		return std::string(tmpbuf);
	}
	//-----------------------------------------------------------------------
	int StrConv::parseInt(const std::string& val, int defaultValue)
	{
		int ret = defaultValue;
		sscanf(val.c_str(), "%d", &ret);

		return ret;
	}

	//-----------------------------------------------------------------------
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::string StrConv::toString(size_t val, 
		unsigned short width, char fill, int flags)
	{
		char tmpbuf[64];
		memset(tmpbuf, 0, 64);
		sprintf( tmpbuf, "%u", val);

		return std::string(tmpbuf);
	}
#endif
	//-----------------------------------------------------------------------
	unsigned int StrConv::parseUnsignedInt(const std::string& val, unsigned int defaultValue)
	{
		unsigned int ret = defaultValue;
		sscanf(val.c_str(), "%u", &ret);

		return ret;
	}

	//-----------------------------------------------------------------------
	std::string StrConv::toString(unsigned long val, 
		unsigned short width, char fill, int flags)
	{
		char tmpbuf[64];
		memset(tmpbuf, 0, 64);
		sprintf( tmpbuf, "%lu", val);

		return std::string(tmpbuf);
	}
	//-----------------------------------------------------------------------
	unsigned long StrConv::parseUnsignedLong(const std::string& val, unsigned long defaultValue)
	{
		unsigned long ret = defaultValue;
		sscanf(val.c_str(), "%lu", &ret);

		return ret;
	}

	//-----------------------------------------------------------------------
	std::string StrConv::toString(long val, 
		unsigned short width, char fill, int flags)
	{
		char tmpbuf[64];
		memset(tmpbuf, 0, 64);
		sprintf( tmpbuf, "%ld", val);

		return std::string(tmpbuf);
	}
	//-----------------------------------------------------------------------
	long StrConv::parseLong(const std::string& val, long defaultValue)
	{
		long ret = defaultValue;
		sscanf(val.c_str(), "%ld", &ret);

		return ret;
	}
	//-----------------------------------------------------------------------
	std::string StrConv::toString(bool val, bool yesNo)
	{
		if (val)
		{
			if (yesNo)
			{
				return "yes";
			}
			else
			{
				return "true";
			}
		}
		else
			if (yesNo)
			{
				return "no";
			}
			else
			{
				return "false";
			}
	}
	//-----------------------------------------------------------------------
	bool StrConv::parseBool(const std::string& val, bool defaultValue)
	{
		if ((StrUtil::startsWith(val, "true") || StrUtil::startsWith(val, "yes")
			|| StrUtil::startsWith(val, "1")))
			return true;
		else if ((StrUtil::startsWith(val, "false") || StrUtil::startsWith(val, "no")
			|| StrUtil::startsWith(val, "0")))
			return false;
		else
			return defaultValue;
	}
	//-------------------------------------------------------------------------
	std::string StrConv::toString(const char* val)
	{
		if(val)
		{
			return std::string(val);
		}
		else
		{
			return std::string("");
		}
	}
	//-----------------------------------------------------------------------
	////ColourValue StrConv::parseColourValue(const std::string& val, const ColourValue& defaultValue)
	////{
	////	// Split on space
	////	StrVec vec = StringUtil::split(val);

	////	if (vec.size() == 4)
	////	{
	////		return ColourValue(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), parseReal(vec[3]));
	////	}
	////	else if (vec.size() == 3)
	////	{
	////		return ColourValue(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 1.0f);
	////	}
	////	else
	////	{
	////		return defaultValue;
	////	}
	////}
	////-----------------------------------------------------------------------
	//StrVec StrConv::parseStringVector(const std::string& val)
	//{
	//	return StrUtil::split(val);
	//}
	//-----------------------------------------------------------------------
	bool StrConv::isNumber(const std::string& val)
	{
		float ret;
		if( sscanf(val.c_str(), "%f", &ret) == 0)
		{
			return false;
		}
		return true;
	}
NS_CC_END
