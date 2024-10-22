#include "msgpush.h"
#if defined(__ANDROID__)
#include "platform/android/msgasynchandler.h"
#include "platform/android/msgdata.h"
#endif
#include "luaengine/luaengine.h"
#include <typeinfo>
#include "platform/pushtools.h"
#include "zqcommon/logutil.h"
#include "cocos2d.h"

static MsgPush* s_instance = nullptr;

MsgPush* MsgPush::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new MsgPush;
	}
	return s_instance;
}

void MsgPush::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool MsgPush::Init()
{
	return true;
}

bool MsgPush::Pause()
{
	return true;
}

bool MsgPush::Resume()
{
	return true;
}
bool MsgPush::Start()
{
	return true;
}

bool MsgPush::Stop()
{
	return true;
}

bool MsgPush::Release()
{
	return true;
}

void MsgPush::StartMsgPush(unsigned int lua_fun_id)
{
	if(lua_fun_id > 0)
	{
		PushTools::StartMsgPush(lua_fun_id);
	}
}

void MsgPush::SetTag(const char* tag, unsigned int lua_fun_id)
{
	if(lua_fun_id > 0)
	{
		PushTools::SetTag(tag, lua_fun_id);
	}
}

void MsgPush::SetAlias(const char* alias, unsigned int lua_fun_id)
{
	if(lua_fun_id > 0)
	{
		PushTools::SetAlias(alias, lua_fun_id);
	}
}

std::string MsgPush::ResumeMsgPush()
{
	return PushTools::ResumeMsgPush();
}

std::string MsgPush::StopMsgPush()
{
	return PushTools::StopMsgPush();
}

std::string MsgPush::GetApiKey()
{
	return PushTools::GetApiKey();
}

std::string MsgPush::GetAppInfo()
{
	std::string appResult = PushTools::GetAppInfo();
	return appResult;
}

std::string MsgPush::GetActivityStatus()
{
	return PushTools::GetActivityStatus();
}

std::string MsgPush::SetTimerTask(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
	return PushTools::SetTimerTask(taskID, taskTime, taskName, taskLast, preTime, timer);
}

std::string MsgPush::CancelTimerTask(const char* taskTime)
{
	return PushTools::CancelTimerTask(taskTime);
}

void MsgPush::DelegateSocket(unsigned int socket_id)
{
	PushTools::DelegateSocket(socket_id);
}

bool MsgPush::Update(float time)
{
	m_inteval = m_inteval + time;
	if(m_inteval < m_update_time)
	{
		return true;
	}
	m_inteval = 0;

#if defined(__ANDROID__)
	MsgInfoList msginfos = MessageAsyncHandler::Get(MessageData::MSG_PUSH);
	for (MsgInfoList::iterator iter = msginfos.begin();iter!= msginfos.end();++iter)
	{
		MessageData *msgdata = iter->message;
		if(typeid(*msgdata) == typeid(MsgPushData))
		{
			MsgPushData *msgPushData = dynamic_cast<MsgPushData *>(msgdata);
			switch(msgPushData->code)
			{
			case MsgPush::MSG_CODE_STARTPUSH :
				{
					LuaEngine::GetInstance()->CallFunction(msgPushData->luafunid, "s", (msgPushData->data.c_str()));
					break;
				}
			case MsgPush::MSG_CODE_SETTAG :
				{
					LuaEngine::GetInstance()->CallFunction(msgPushData->luafunid, "s", (msgPushData->data.c_str()));
					break;
				}
			case MsgPush::MSG_CODE_SETALIAS :
				{
					LuaEngine::GetInstance()->CallFunction(msgPushData->luafunid, "s", (msgPushData->data.c_str()));
					break;
				}
			default:
				break;
			}
			delete msgPushData;
		}
	}
#endif
	return true;
}
