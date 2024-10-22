#include "platform/pushtools.h"
#include "luaengine/luaengine.h"
#include "platform/CCCommon.h"
#include "cocos2d.h"
#include <string>
#import "PushManager.h"

void PushTools::StartMsgPush(unsigned int lua_fun_id)
{
    [PushManager StartMsgPush:lua_fun_id];
}

std::string PushTools::StopMsgPush()
{
    NSString *nsResult = [PushManager StopMsgPush];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

std::string PushTools::ResumeMsgPush()
{
    NSString *nsResult = [PushManager ResumeMsgPush];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

std::string PushTools::GetApiKey()
{
    NSString *nsResult = [PushManager GetApiKey];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

void PushTools::SetTag(const char* tag, unsigned int lua_fun_id)
{
    if (lua_fun_id != 0) {
        NSString *temp = [NSString stringWithUTF8String:tag];
        [PushManager SetTag:temp:lua_fun_id];
    }
}

void PushTools::SetAlias(const char* alias, unsigned int lua_fun_id)
{
    if (lua_fun_id != 0) {
        NSString *temp = [NSString stringWithUTF8String:alias];
        [PushManager SetAlias:temp:lua_fun_id];
    }
}

std::string PushTools::GetAppInfo()
{
    NSString *nsResult = [PushManager GetAppInfo];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

std::string PushTools::SetTimerTask(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
    NSString *task_id = [NSString stringWithUTF8String:taskID];
    NSString *task_time = [NSString stringWithUTF8String:taskTime];
    NSString *task_name = [NSString stringWithUTF8String:taskName];
    NSString *task_last = [NSString stringWithUTF8String:taskLast];
    
    NSString *task_pre = [NSString stringWithUTF8String:preTime];
    NSString *task_timer = [NSString stringWithUTF8String:timer];
    NSString *nsResult = [PushManager SetTimerTask:task_id :task_time :task_name :task_last :task_pre :task_timer];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

std::string PushTools::GetActivityStatus()
{
    NSString *nsResult = [PushManager GetActivityStatus];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

std::string PushTools::CancelTimerTask(const char* taskID)
{
    NSString *nsTaskID = [NSString stringWithUTF8String:taskID];
    NSString *nsResult = [PushManager CancelTimerTask:nsTaskID];
    std::string sResult = [nsResult UTF8String];
    return sResult;
}

void PushTools::InvokeCallBackPushMethod(const char* method_type, unsigned int lua_fun_id, const char* extra_info)
{
}
std::string PushTools::InvokePushTaskMethod(const char* taskID, const char* taskTime, const char* taskName, const char* taskLast, const char* preTime, const char* timer)
{
    return "ios";
}
std::string PushTools::InvokeVoidPushMethod(const char* method_type, const char* extra_info)
{
    return "ios";
}

void PushTools::DelegateSocket(unsigned int socket_id)
{
#ifdef __ZL_APPSTORE__
    NSLog(@"app store app do not DelegateSocket");
    return;
#endif
    NSLog(@"begin DelegateSocket");
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocket(NULL, socket_id,  &readStream, &writeStream);
    CFReadStreamSetProperty(readStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
    CFWriteStreamSetProperty(writeStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
    //CFReadStreamScheduleWithRunLoop(readStream, [NSRunLoop currentRunLoop], NSDefaultRunLoopMode);
    //CFWriteStreamScheduleWithRunLoop(writeStream, [NSRunLoop currentRunLoop], NSDefaultRunLoopMode);
    CFReadStreamOpen(readStream);
    CFWriteStreamOpen(writeStream);
}


