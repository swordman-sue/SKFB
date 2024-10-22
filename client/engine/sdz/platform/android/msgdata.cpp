#include "platform/android/msgdata.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

 extern "C"
 {
 	int GetInt(JNIEnv * env,jclass clazz,jobject obj,const char * method_name)
 	{
 		jmethodID int_method = env->GetMethodID(clazz,method_name,"()I");
 		jint int_value = env->CallIntMethod(obj,int_method);
 		return int_value;
 	}

 	std::string GetString(JNIEnv * env,jclass clazz,jobject obj,const char * method_name)
	{
		jmethodID strign_method = env->GetMethodID(clazz,method_name,"()Ljava/lang/String;");
		jstring string_value = (jstring)env->CallObjectMethod(obj,strign_method);
		std::string str = cocos2d::JniHelper::jstring2string(string_value);
		env->DeleteLocalRef(string_value);
		return str;
	}

 }

 MessageData* MessageData::CreateMessage(MSG_CODE msg_code)
 {
	MessageData * msgdata = NULL;

	if(MSG_LOCATION == msg_code)
	{
		msgdata = new MsgLocationData();
	}else if(MSG_PLAY_FINISH == msg_code)
	{
		msgdata = new MsgPlayFinishData();
	}else if(MSG_PUSH == msg_code)
	{
		msgdata = new MsgPushData();
	}else if (MSG_SDK_TOOLS == msg_code)
	{
		msgdata = new MsgSdkData();
	}

	return msgdata;
 }


 MsgLocationData::MsgLocationData(Op_Type _op_type,unsigned int _timeout)
 :op_type(_op_type),code(0),timeout(_timeout)
 {

 }

jobject MsgLocationData::ToJobject(JNIEnv *env)
{
	jclass    m_cls   = env->FindClass("com/kuyue/gps/MsgLocationData");
	jmethodID m_mid   = env->GetMethodID(m_cls,"<init>","()V");
	jfieldID  m_fid_1 = env->GetFieldID(m_cls,"asyncId","I");
	jfieldID  m_fid_2 = env->GetFieldID(m_cls,"timeout","I");
	jfieldID  m_fid_3 = env->GetFieldID(m_cls,"opType","I");
	jobject   m_obj   = env->NewObject(m_cls,m_mid);
	env->SetIntField(m_obj,m_fid_1,this->async_id);
	env->SetIntField(m_obj,m_fid_2,this->timeout);
	env->SetIntField(m_obj,m_fid_3,this->op_type);
	env->DeleteLocalRef(m_cls);
	return m_obj;
};

void MsgLocationData::ToCobject(JNIEnv *env,jobject obj,jclass obj_class)
{
	int code = GetInt(env,obj_class,obj,"getCode");
	this->code = code;
	//获取async_id
	int async_id = GetInt(env,obj_class,obj,"getAsyncId");
	this->async_id = async_id;
	//获取msg
	std::string msg = GetString(env,obj_class,obj,"getMsg");
	this->msg = msg;
}



jobject MsgPlayFinishData::ToJobject(JNIEnv *env)
{
	jclass    m_cls   = env->FindClass("com/kuyue/voice/MsgPlayFinishData");

	jmethodID m_mid   = env->GetMethodID(m_cls,"<init>","()V");
	jfieldID  m_fid_1 = env->GetFieldID(m_cls,"luafunid","I");
	jfieldID  m_fid_2 = env->GetFieldID(m_cls,"code","I");
	jfieldID  m_fid_3 = env->GetFieldID(m_cls,"voiceid","I");
	jobject   m_obj   = env->NewObject(m_cls,m_mid);
	env->SetIntField(m_obj,m_fid_1,this->luafunid);
	env->SetIntField(m_obj,m_fid_2,this->code);
	env->SetIntField(m_obj,m_fid_3,this->voiceid);
	env->DeleteLocalRef(m_cls);
	return m_obj;
};

void MsgPlayFinishData::ToCobject(JNIEnv *env,jobject obj,jclass obj_class)
{
	//获取luafunid
	int luafunid = GetInt(env,obj_class,obj,"getLuafunid");
	this->luafunid = luafunid;
	//获取code
	int code = GetInt(env,obj_class,obj,"getCode");
	this->code = code;

	//获取voiceid
	int voiceid  = GetInt(env,obj_class,obj,"getVoiceid");
	this->voiceid = voiceid;

}

jobject MsgPushData::ToJobject(JNIEnv *env)
{
	jclass    m_cls   = env->FindClass("com/kuyue/pushsdk/MsgPushData");

	jmethodID m_mid   = env->GetMethodID(m_cls,"<init>","()V");
	jfieldID  m_fid_1 = env->GetFieldID(m_cls,"luafunid","I");
	jfieldID  m_fid_2 = env->GetFieldID(m_cls,"code","I");
	jobject   m_obj   = env->NewObject(m_cls,m_mid);
	env->SetIntField(m_obj,m_fid_1,this->luafunid);
	env->SetIntField(m_obj,m_fid_2,this->code);
	env->DeleteLocalRef(m_cls);
	return m_obj;
};

void MsgPushData::ToCobject(JNIEnv *env,jobject obj,jclass obj_class)
{
	//获取luafunid
	int luafunid = GetInt(env,obj_class,obj,"getLuafunid");
	this->luafunid = luafunid;
	//获取code
	int code = GetInt(env,obj_class,obj,"getCode");
	this->code = code;
	//获取data
	std::string data = GetString(env, obj_class, obj, "getData");
	this->data = data;

}

//sdktools消息c++/java转换
jobject MsgSdkData::ToJobject(JNIEnv *env)
{
	return NULL;
};

void MsgSdkData::ToCobject(JNIEnv *env,jobject obj,jclass obj_class)
{
	//获取code
	int code = GetInt(env,obj_class,obj,"getCode");
	this->code = code;
	//获取data
	std::string data = GetString(env, obj_class, obj, "getData");
	this->data = data;
	//消息类型
	int msgType = GetInt(env,obj_class,obj,"getMsgType");
	this->msg_type = MsgSdkData::Msg_Type(msgType);
}
