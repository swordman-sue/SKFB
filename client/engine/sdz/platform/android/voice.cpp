#include "platform/voice.h"
#include <string>
#include "platform/voice.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <string>
#include "luaengine/luamessageadapter.h"
#define CPP_INVOKE_JAVA_VOICEMANAGER "com/kuyue/voice/VoiceManager"
#define CPP_INVOKE_JAVA_PLAYFINISHMANAGER "com/kuyue/voice/PlayFinishManager"

int Voice::StartRecord(const char* full_file_name,int second)
{
	int code = 0;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_VOICEMANAGER,"StartRecord","(Ljava/lang/String;I)I");
	if (isHave)
	{
		jstring strfullfilename = minfo.env->NewStringUTF(full_file_name);
		code = (int)minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID, strfullfilename,second);
		minfo.env->DeleteLocalRef(strfullfilename);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	return code;
}

int Voice::StopRecord()
{
	int code = 0;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_VOICEMANAGER,"StopRecord","()I");

	if (isHave)
	{
		code = (int)minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	return code;
}

int Voice::StartPlay(const char* full_file_name,unsigned int voice_id, unsigned int lua_fun_id)
{
	int code = 0;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_VOICEMANAGER,"StartPlay","(Ljava/lang/String;II)I");

	if (isHave)
	{
		jstring strfullfilename = minfo.env->NewStringUTF(full_file_name);
		code = (int)minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID,strfullfilename,voice_id,lua_fun_id);
		minfo.env->DeleteLocalRef(strfullfilename);
		minfo.env->DeleteLocalRef(minfo.classID);

	}
	return code;
}


int Voice::StopPlay()
{
	int code = 0;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_VOICEMANAGER,"StopPlay","()I");

	if (isHave)
	{
		code = (int)minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	return code;
}

int Voice::ReleaseVoice()
{
	int code = 0;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_VOICEMANAGER,"ReleaseVoice","()I");

	if (isHave)
	{
		code = (int)minfo.env->CallStaticIntMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	return code;
}

