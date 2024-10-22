#pragma once

#include <limits>
#include "Common.h"

template <typename NumType>
class KeyGenerator
{
public:
	KeyGenerator() { m_max = m_value = m_step = 0; }
	~KeyGenerator() { }

	void Create(NumType keyMin, NumType keyMax, int step = 1)
	{
		ASSERT(keyMin < keyMax);
		ASSERT(step > 0);
		m_max = keyMax;
		m_value = keyMin;
		m_step = step;
	}

	NumType Generate()
	{
		IF_NOT (m_value <= m_max)
			return -1;
		NumType res = m_value;
		m_value += m_step;
		return res;
	}

private:
	NumType m_max;
	NumType m_value;
	int m_step;
};
