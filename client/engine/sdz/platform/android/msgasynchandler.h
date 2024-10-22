#ifndef SDK_MESSAGE_ASYCN_HANDLER_H_
#define SDK_MESSAGE_ASYCN_HANDLER_H_
#include "zqcommon/typedef.h"
#include "msgdata.h"
#include <string>

class MessageAsyncHandler
{
public:
    static bool Add(MessageData::MSG_CODE msg_code,MessageData *msgInfo);
	static MsgInfoList	Get(MessageData::MSG_CODE msg_code);
};
#endif
