#pragma once
#include "../ToolKit/Singleton.h"
#include <vector>


class CRedisManager : public Singleton<CRedisManager>
{
public:
	CRedisManager(void);
	~CRedisManager(void);

public:
	BOOL RedisQuery(DWORD dwThreadIndex);

private:


private:

};
