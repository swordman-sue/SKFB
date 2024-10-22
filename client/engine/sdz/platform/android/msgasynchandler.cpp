#include "platform/android/msgasynchandler.h"
#include "platform/android/msgdata.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"


#define MESSAGE_ASYNC_CLASS  "com/kuyue/queue/MessageMagager"


using namespace std;


MsgInfoList MessageAsyncHandler::Get(MessageData::MSG_CODE msg_code)
{
	MsgInfoList msgInfos;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, MESSAGE_ASYNC_CLASS,
			"Get", "(I)Ljava/util/Vector;");

	if (is_get)
	{
		jobject vector_obj = method_info.env->CallStaticObjectMethod(method_info.classID, method_info.methodID,msg_code);
		if(vector_obj)
		{
			jclass vector_class =  method_info.env->GetObjectClass(vector_obj);
			//获取第几个数据
			jmethodID vector_list_get_method =  method_info.env->GetMethodID(vector_class,"get","(I)Ljava/lang/Object;");
			//获取size
			jmethodID vector_list_size_method =  method_info.env->GetMethodID(vector_class,"size","()I");

			jint len = method_info.env->CallIntMethod(vector_obj,vector_list_size_method);

			for(int i = 0;i < len;++i)
			{
				jobject msgInfo_obj = method_info.env->CallObjectMethod(vector_obj,vector_list_get_method,i);
				MessageData* msgdata = MessageData::CreateMessage(msg_code);
				if(msgdata != NULL)
				{
					jclass obj_class = method_info.env->GetObjectClass(msgInfo_obj);
					msgdata->ToCobject(method_info.env,msgInfo_obj,obj_class);
					Message msg(msgdata);
					msgInfos.push_back(msg);
					method_info.env->DeleteLocalRef(obj_class);
				}
				method_info.env->DeleteLocalRef(msgInfo_obj);
			}
			method_info.env->DeleteLocalRef(vector_class);
		}
		method_info.env->DeleteLocalRef(method_info.classID);
	}
   return msgInfos;
}

bool MessageAsyncHandler::Add(MessageData::MSG_CODE msg_code,MessageData* msgInfo)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, MESSAGE_ASYNC_CLASS,
			"Add", "(ILcom/kuyue/queue/MessageData;)Z");

	if (is_get)
	{
		jobject jobj = msgInfo->ToJobject(method_info.env);

		ret_val  = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID,msg_code,jobj);

		method_info.env->DeleteLocalRef(jobj);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
	return ret_val;
}



