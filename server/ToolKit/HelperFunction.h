#pragma once

#include <string>

namespace ygth
{
	// 转换字符串为全部大写
	void Str2Upper(std::string& str);

	// 转换字符串为全部小写
	void Str2Lower(std::string& str);

	// 拷贝字符串，返回拷贝的字符串的大小（包括 \0）
	int StrCopy(char* dest, int maxLen, const char* src);

	// 保证不出现溢出的情况下计算 value + addValue
	// value 为无符号类型
	// addValue 为有符号类型
	template <typename UnsignedType, typename SingedType>
	UnsignedType AddValue(UnsignedType value, SingedType addValue)
	{
		if (addValue <= 0)
		{
			if ((UnsignedType)(-addValue) >= value)
				return 0;
			else
				return addValue + value;
		}
		else
		{
			const UnsignedType MAX_SIGNED_VALUE = (std::numeric_limits<SingedType>::max)();
			if (value <= MAX_SIGNED_VALUE + 1)
				return value + addValue;
			else
			{
				UnsignedType tmpMoney = value - MAX_SIGNED_VALUE;
				tmpMoney += addValue;
				if (tmpMoney >= MAX_SIGNED_VALUE + 1)
					return MAX_SIGNED_VALUE * 2 + 1;
				else
					return tmpMoney + MAX_SIGNED_VALUE;
			}
		}
	}
}