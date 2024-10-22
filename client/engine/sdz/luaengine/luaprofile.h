#ifndef LUAPROFILE_H
#define LUAPROFILE_H

#include <string>
#include <stdio.h>

struct lua_State;

struct CallFuncInfo
{
	CallFuncInfo() : call_num(0), accu_time(0) {}

	//���������ļ���
	std::string src_filename;	

	//��������
	std::string func_type;

	//�������忪ʼ�����к�
	int func_linedef;

	//��ʼʱ��
	unsigned long start_time;		

	//ִ�к�ʱ
	unsigned long accu_time;

	//���ô���
	int call_num;					
};

class LuaProfile
{
public:
	enum LuaProfileType
	{
		LPT_UNKNOWN = 0,

		//����ĳ��ʱ���ڵ�LUA�������ÿ���
		LPT_PERFORMANCE_MONITOR = 1,

		//���������LUA�����������(���ڱ���ʱ���鿴LUA�������ö�ջ)
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