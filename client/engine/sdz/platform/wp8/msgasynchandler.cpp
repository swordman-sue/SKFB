#include "platform/wp8/msgasynchandler.h"
#include "platform/wp8/msgdata.h"
//#include <jni.h>
//#include "platform/wp8xaml/jni/JniHelper.h"
#include "platform/wp8/winrtcallback.h"


#define MESSAGE_ASYNC_CLASS  "com/kuyue/queue/MessageMagager"


using namespace std;
using namespace PhoneDirect3DXamlAppComponent;
using namespace PhoneDirect3DXamlAppComponent::OpenXLiveHelper;

static MsgInfoList msgInfoList;

MsgInfoList MessageAsyncHandler::Get(MessageData::MSG_CODE msg_code)
{
	MsgInfoList msgInfos;


	static XLiveDelegate^ aDelegate = ref new XLiveDelegate();
	Platform::Array<BaseMsgData^>^ msgs = aDelegate->GlobalCallback->GetAyncMessage(msg_code);
	for (unsigned int i = 0; i < msgs->Length; ++i)
	{
		BaseMsgData^ tempmsg = msgs[i];
		MsgSdkData* msgdata = MsgSdkData::CreateMessage(msg_code);
		msgdata->ToCobject(tempmsg);
		Message msg(msgdata);
		msgInfos.push_back(msg);
	}
   return msgInfos;
}

bool MessageAsyncHandler::Add(MessageData::MSG_CODE msg_code,MessageData* msgInfo)
{
	bool ret_val = false;

	return ret_val;
}



