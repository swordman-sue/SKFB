#include "location.h"
#if defined(__ANDROID__)
	#include "platform/android/msgasynchandler.h"
	#include "platform/android/msgdata.h"
#endif
#include "luaengine/luaengine.h"
#include "platform/sdktools.h"
#include <typeinfo>
#include "platform/locationtools.h"
#include "zqcommon/logutil.h"

static Location* s_instance = nullptr;

Location* Location::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new Location;
	}
	return s_instance;
}

void Location::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool Location::Init()
{
	return true;
}

bool Location::Pause()
{
	return true;
}

bool Location::Resume()
{
	return true;
}
bool Location::Start()
{
	return true;
}

bool Location::Stop()
{
	return true;
}

bool Location::Release()
{
	return true;
}

void Location::SetLocationTimeout(const int timeout)
{
	if(timeout <= 0  || timeout > 180)
	{
		m_timeout = 10;
	}else
	{
		m_timeout = timeout;
	}
}

void Location::StartLocation(ZQ::UINT32 fun_id)
{
	if(fun_id > 0)
	{
		m_callback_list.push_back(fun_id);
		LocationTools::StartLocation(m_timeout);
	}
}

void Location::StopLocation()
{
	LocationTools::StopLocation();
	m_callback_list.clear();
}

void Location::CallbackLocation(int code,const char* location_data)
{
	LocationCallbackList temp_list;
	m_callback_list.swap(temp_list);

	for(LocationCallbackList::iterator iter = temp_list.begin();iter!= temp_list.end();++iter)
	{
		LuaEngine::GetInstance()->CallFunction(*iter,"is",code,location_data);
	}
}


bool Location::Update(float time)
{
	if (time < 0)
	{
		time = 0.1;
	}
	m_inteval = m_inteval + time;
	if(m_inteval < m_update_time)
	{
		return true;
	}
	m_inteval = 0;

#if defined(__ANDROID__)
		MsgInfoList msginfos = MessageAsyncHandler::Get(MessageData::MSG_LOCATION);
		for (MsgInfoList::iterator iter = msginfos.begin();iter!= msginfos.end();++iter)
		{
			MessageData *msgdata = iter->message;
			if(typeid(*msgdata) == typeid(MsgLocationData))
			{
				MsgLocationData *locationData = dynamic_cast<MsgLocationData *>(msgdata);
				CallbackLocation(locationData->code,(locationData->msg).c_str());
				delete locationData;
			}
		}
#elif  defined(_WIN32)
	if (m_callback_list.size() != 0)
	{
		CallbackLocation(0,"{\"latitude\":23.1269542,\"longitude\":113.3344062}");
	}
#endif
	return true;
}

