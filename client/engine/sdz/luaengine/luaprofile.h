#ifndef LUAPROFILE_H
#define LUAPROFILE_H

#include <string>
#include <stdio.h>

struct lua_State;

struct CallFuncInfo
{
	CallFuncInfo() : call_num(0), accu_time(0) {}

	//函数所在文件名
	std::string src_filename;	

	//函数类型
	std::string func_type;

	//函数定义开始处的行号
	int func_linedef;

	//开始时间
	unsigned long start_time;		

	//执行耗时
	unsigned long accu_time;

	//调用次数
	int call_num;					
};

class LuaProfile
{
public:
	enum LuaProfileType
	{
		LPT_UNKNOWN = 0,

		//监视某段时间内的LUA函数调用开销
		LPT_PERFORMANCE_MONITOR = 1,

		//监视最近的LUA函数调用情况(用于崩溃时，查看LUA函数调用堆栈)
		LPT_LATEST_CALLFUNC_MONITOR = 2,
	};

public:
	LuaProfile();

	~LuaProfile();

	void Start(LuaProfileType type);
	void Stop();

private:
	void SetHooksFlag(int mask);

	void WriteProfileLog();
	void WriteProfileLog(const char *fmt, ...);

private:
	int m_mask;
	FILE *m_log_file;
};

class LuaProfileAgent
{
public:
	~LuaProfileAgent();

	static LuaProfileAgent* GetInstance(void);
	static void DelInstance();

	void Start(LuaProfile::LuaProfileType type = LuaProfile::LPT_PERFORMANCE_MONITOR);

	void Stop();

private:
	LuaProfileAgent();

private:
	LuaProfile* m_lua_profile;
};

#endif