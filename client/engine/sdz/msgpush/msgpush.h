#ifndef SDK_MSGPUSH_H
#define SDK_MSGPUSH_H
#include "zqcommon/typedef.h"
#include <string>

class MsgPush
	{
public:
	static MsgPush* GetInstance();
	static void DelInstance();
	~MsgPush() {};

private:
	MsgPush():m_inteval(0.0f)
		,m_update_time(1)
	{};

public:
	enum MethodType
	{
		MSG_CODE_SETTAG = 101,
		MSG_CODE_SETALIAS = 102,
		MSG_CODE_STARTPUSH = 103,
	};

public:
	bool Init();
	bool Start();
	bool Update(float time);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();

public:
	void StartMsgPush(unsigned int lua_fun_id);
	void SetTag(const char* tag, unsigned int lua_fun_id);
	void SetAlias(const char* alias, unsigned int lua_fun_id);
	std::string ResumeMsgPush();
	std::string StopMsgPush();
	std::string GetApiKey();
	std::string GetAppInfo();
	std::string GetActivityStatus();
	std::string SetTimerTask(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer);
	std::string CancelTimerTask(const char* taskTime);
	void DelegateSocket(unsigned int socket_id);
private:
	float m_inteval;
	unsigned int m_update_time;
};
#endif

