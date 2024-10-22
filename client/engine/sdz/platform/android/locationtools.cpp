#include "platform/locationtools.h"
#include "platform/android/msgasynchandler.h"
#include "platform/android/msgdata.h"
#include "cocos2d.h"
void LocationTools::StartLocation(int timeout)
{
	MsgLocationData msg(MsgLocationData::start,timeout);
	MessageAsyncHandler::Add(MessageData::MSG_LOCATION,&msg);
};

void LocationTools::StopLocation()
{
	MsgLocationData msg(MsgLocationData::stop);
	MessageAsyncHandler::Add(MessageData::MSG_LOCATION,&msg);
}
