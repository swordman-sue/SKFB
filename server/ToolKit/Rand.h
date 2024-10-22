#pragma once
#include "..\ToolKit\MersenneTwister.h"
#include "..\ToolKit\Singleton.h"

class Rand : public Singleton<Rand>
{
public:
	int32 irand (int32 min, int32 max)
	{
		return int32 (m_MTRand.randInt (max - min)) + min;
	}

	uint32 urand (uint32 min, uint32 max)
	{
		return m_MTRand.randInt (max - min) + min;
	}

	uint32 rand32 ()
	{
		return m_MTRand.randInt ();
	}

	double rand_norm(void)
	{
		return m_MTRand.randExc ();
	}

	float rand_norm_f(void)
	{
		return (float)m_MTRand.randExc ();
	}

	double rand_chance (void)
	{
		return m_MTRand.randExc (100.0);
	}

	float rand_chance_f(float fRange = 100.0)
	{
		return (float)m_MTRand.randExc (fRange);
	}

private:
    MTRand  m_MTRand;
};