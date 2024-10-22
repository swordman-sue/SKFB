#pragma once

#include <string>

namespace ygth
{
	// ת���ַ���Ϊȫ����д
	void Str2Upper(std::string& str);

	// ת���ַ���Ϊȫ��Сд
	void Str2Lower(std::string& str);

	// �����ַ��������ؿ������ַ����Ĵ�С������ \0��
	int StrCopy(char* dest, int maxLen, const char* src);

	// ��֤���������������¼��� value + addValue
	// value Ϊ�޷�������
	// addValue Ϊ�з�������
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