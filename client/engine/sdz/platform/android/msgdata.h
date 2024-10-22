#ifndef SDK_MESSAGE_DATA_H
#define SDK_MESSAGE_DATA_H
#include "zqcommon/typedef.h"
#include <string>
#include <jni.h>
#include <vector>


struct MessageData
{
	enum MSG_CODE
	{
		MSG_LOCATION = 1,
		MSG_PLAY_FINISH = 2,
		MSG_PUSH = 3,
		MSG_SDK_TOOLS = 4,
	};

	unsigned int async_id;

	MessageData():async_id(0){}
	virtual ~MessageData(){}

	static MessageData* CreateMessage(MSG_CODE msg_code);

	virtual jobject ToJobject(JNIEnv *env)=0;
	virtual void ToCobject(JNIEnv *env,jobject obj,jclass obj_class)=0;
};
struct Message
{
	Message(MessageData *_message):message(_message){};
	MessageData *message;
};
typedef std::vector<Message>	MsgInfoList;




struct MsgLocationData : public MessageData
{
	enum Op_Type
	{
		start = 1,
		stop,
	};

	Op_Type op_type;
	unsigned int code;
	std::string  msg;
	unsigned int  timeout;

	MsgLocationData(Op_Type _op_type,unsigned int _timeout);
	MsgLocationData(Op_Type _op_type):op_type(_op_type),code(0),timeout(0){}
	MsgLocationData():code(0),timeout(0){}

	jobject ToJobject(JNIEnv *env);
	void ToCobject(JNIEnv *env,jobject obj,jclass obj_class);
};

struct MsgPlayFinishData : public MessageData
{
	unsigned int luafunid;
	int  code;
	unsigned int voiceid;

	MsgPlayFinishData():luafunid(0),code(0),voiceid(0){}

	jobject ToJobject(JNIEnv *env);
	void ToCobject(JNIEnv *env,jobject obj,jclass obj_class);
};

struct MsgPushData : public MessageData
{
	unsigned int luafunid;
	int  code;
	std::string  data;

	MsgPushData():luafunid(0),code(0){}
	jobject ToJobject(JNIEnv *env);
	void ToCobject(JNIEnv *env,jobject obj,jclass obj_class);
};
//sdktools回调消息
struct MsgSdkData : public MessageData
{
	enum Msg_Type
	{
		LOGIN = 1,
		TD ,
		COMMON ,
	};
	unsigned int  code;
	std::string  data;
	Msg_Type msg_type;

	MsgSdkData():code(0),msg_type(LOGIN){}
	jobject ToJobject(JNIEnv *env);
	void ToCobject(JNIEnv *env,jobject obj,jclass obj_class);
};

#endif
