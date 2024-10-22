#include "sdkhandle.h"
#if defined(__ANDROID__)
	#include "platform/android/msgasynchandler.h"
	#include "platform/android/msgdata.h"
#elif defined(_WP8)
	#include "platform/wp8/msgasynchandler.h"
	#include "platform/wp8/msgdata.h"
	#include "view/widget/edittext.h"
	#include "platform/wp8/asyncloadhandler.h"
#endif
#include "luaengine/luaengine.h"
#include "platform/sdktools.h"
#include <typeinfo>
#include "zqcommon/logutil.h"

static SdkHandle* s_instance = nullptr;

SdkHandle* SdkHandle::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new SdkHandle;
	}
	return s_instance;
}

void SdkHandle::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool SdkHandle::Init()
{
	return true;
}

bool SdkHandle::Pause()
{
	return true;
}

bool SdkHandle::Resume()
{
	return true;
}
bool SdkHandle::Start()
{
	return true;
}

bool SdkHandle::Stop()
{
	return true;
}

bool SdkHandle::Release()
{
	return true;
}

bool SdkHandle::Update(float time)
{
	if (time < 0)
	{
		time = 0.1f;
	}
	m_inteval = m_inteval + time;
	if(m_inteval < m_update_time)
	{
		return true;
	}
	m_inteval = 0;

#if defined(__ANDROID__)
		MsgInfoList msginfos = MessageAsyncHandler::Get(MessageData::MSG_SDK_TOOLS);
		for (MsgInfoList::iterator iter = msginfos.begin();iter!= msginfos.end();++iter)
		{
			MessageData *msgdata = iter->message;
			if(typeid(*msgdata) == typeid(MsgSdkData))
			{
				MsgSdkData *sdkData = dynamic_cast<MsgSdkData *>(msgdata);
				switch(sdkData->msg_type)
				{
				case MsgSdkData::LOGIN:
					{
						LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_login_panel_fun_id,"is",sdkData->code,(sdkData->data).c_str());
						break;
					}
				case MsgSdkData::TD:
					{
						LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_td_finish_fun_id,"is",sdkData->code,(sdkData->data).c_str());
						break;
					}
				case MsgSdkData::COMMON:
					{
						LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_common_fun_id,"is",sdkData->code,(sdkData->data).c_str());
						break;
					}
				}
				if (sdkData)
				{
					delete sdkData;
					sdkData = NULL;
				}
			}
		}
#elif defined(_WP8)
	MsgInfoList msgsdkinfos = MessageAsyncHandler::Get(MessageData::MSG_SDK_TOOLS);
	for (MsgInfoList::iterator iter = msgsdkinfos.begin(); iter != msgsdkinfos.end(); ++iter)
	{
		MessageData *msgdata = iter->message;
		if (typeid(*msgdata) == typeid(MsgSdkData))
		{
			MsgSdkData *sdkData = dynamic_cast<MsgSdkData *>(msgdata);
			switch (sdkData->msg_type)
			{
			case MsgSdkData::LOGIN:
			{
				LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_login_panel_fun_id, "is", sdkData->code, (sdkData->data).c_str());
				break;
			}
			case MsgSdkData::TD:
			{
				LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_td_finish_fun_id, "is", sdkData->code, (sdkData->data).c_str());
				break;
			}
			case MsgSdkData::COMMON:
			{
				LuaEngine::GetInstance()->CallFunction(SdkTools::sdk_common_fun_id, "is", sdkData->code, (sdkData->data).c_str());
				break;
			}
			}
			if (sdkData)
			{
				delete sdkData;
				sdkData = NULL;
			}
		}
	}
	MsgInfoList msgWpInputInfos = MessageAsyncHandler::Get(MessageData::MSG_WP_INPUT_TOOLS);
	for (MsgInfoList::iterator iter = msgWpInputInfos.begin(); iter != msgWpInputInfos.end(); ++iter)
	{
		MessageData *msgdata = iter->message;
		if (typeid(*msgdata) == typeid(MsgSdkData))
		{
			MsgSdkData *sdkData = dynamic_cast<MsgSdkData *>(msgdata);
			switch (sdkData->msg_type)
			{
			case MsgSdkData::WP_INPUT:
			{
				LuaEngine::GetInstance()->CallFunction(EditText::current_edittext_fun_id, "s", (sdkData->data).c_str());
				break;
			}
			}
			if (sdkData)
			{
				delete sdkData;
				sdkData = NULL;
			}
		}
	}

	std::vector<ZQ::UINT32> datalist = AsyncLoadHandler::GetInstance()->Get();
	for (std::vector<ZQ::UINT32>::const_iterator iter = datalist.begin(); iter != datalist.end(); ++iter)
	{
		ZQ::UINT32 func_id = *iter;
		if (func_id > 0)
		{
			LuaEngine::GetInstance()->CallFunction(func_id, "");
			LuaEngine::GetInstance()->DelLuaFuncID(func_id);
		}
	}
#endif
	return true;
}

