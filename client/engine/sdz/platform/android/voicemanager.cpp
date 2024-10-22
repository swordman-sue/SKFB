#include "platform/voicemanager.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "luaengine/luaengine.h"
#include <string>

#define CPP_INVOKE_JAVA_RECORDMANAGER "com/kuyue/sdklib/VoiceManager"

#define JAVA_INVOKE_CPP_RECORD_START_CALLBACK    	Java_com_kuyue_sdklib_VoiceManager_RecordStartCallBack
#define JAVA_INVOKE_CPP_RECORD_STOP_CALLBACK        Java_com_kuyue_sdklib_VoiceManager_RecordStopCallBack
#define JAVA_INVOKE_CPP_PLAY_START_CALLBACK     	Java_com_kuyue_sdklib_VoiceManager_PlayStartCallBack
#define JAVA_INVOKE_CPP_PLAY_STOP_CALLBACK          Java_com_kuyue_sdklib_VoiceManager_PlayStopCallBack

ZQ::UINT32 VoiceManager::record_start_fun_id = 0;
ZQ::UINT32 VoiceManager::record_stop_fun_id = 0;
ZQ::UINT32 VoiceManager::play_start_fun_id = 0;
ZQ::UINT32 VoiceManager::play_stop_fun_id = 0;
int VoiceManager::current_btn_index = 0;
int VoiceManager::current_module_index = 0;

//录制音频
void VoiceManager::RecordStart(const char* source_file_name,int secord, ZQ::UINT32 record_start_fun)
{

	record_start_fun_id = record_start_fun;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_RECORDMANAGER,"RecordStart","(Ljava/lang/String;I)V");

	if (isHave)
	{
		jstring strfilename = minfo.env->NewStringUTF(source_file_name);
		jint int_secord = secord;
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, strfilename,int_secord);
		minfo.env->DeleteLocalRef(strfilename);
		minfo.env->DeleteLocalRef(minfo.classID);

	}
}
//停止录制音频，ios上包含了音频的转换，将wav转化为amr
void VoiceManager::RecordStop(const char* source_file_name,const char* distince_file_name,ZQ::UINT32 record_stop_fun)
{

		record_stop_fun_id = record_stop_fun;
		cocos2d::JniMethodInfo minfo;
		bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
				CPP_INVOKE_JAVA_RECORDMANAGER,"RecordStop","(Ljava/lang/String;Ljava/lang/String;)V");

		if (isHave)
		{
			jstring strsourcefilename = minfo.env->NewStringUTF(source_file_name);
			jstring strdistincefilename = minfo.env->NewStringUTF(distince_file_name);
			minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, strsourcefilename,strdistincefilename);
			minfo.env->DeleteLocalRef(strsourcefilename);
			minfo.env->DeleteLocalRef(strdistincefilename);
			minfo.env->DeleteLocalRef(minfo.classID);

		}
}
//播放音频，ios上判断音频是否已经转换了，没转换的话，要从arm转化为wav，
//播放完了回调，当前面音频没播放完时，将停止之前音频并会点之前的回调
void VoiceManager::PlayStart(const char* source_file_name,int btn_index,int module_index,ZQ::UINT32 play_start_fun)
{

	if (current_btn_index != 0 && current_module_index != 0 && play_start_fun_id != 0)
		{

			AddToPlayFinishList(0,current_btn_index,current_module_index,play_start_fun_id);
			current_btn_index = 0;
			current_module_index = 0;
			play_start_fun_id = 0;
		}
		current_btn_index = btn_index;
		current_module_index = module_index;
		play_start_fun_id = play_start_fun;
		cocos2d::JniMethodInfo minfo;
		bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
				CPP_INVOKE_JAVA_RECORDMANAGER,"PlayStart","(IILjava/lang/String;)V");

		if (isHave)
		{
			jstring strsourcefilename = minfo.env->NewStringUTF(source_file_name);
			minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,btn_index,module_index, strsourcefilename);
			minfo.env->DeleteLocalRef(strsourcefilename);
			minfo.env->DeleteLocalRef(minfo.classID);
		}
}
//清除播放音频资源
void VoiceManager::PlayStop(ZQ::UINT32 play_stop_fun)
{
	play_stop_fun_id = play_stop_fun;
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_RECORDMANAGER,"PlayStop","()V");

	if (isHave)
	{
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);

	}

}
//文件夹初始化，删除文件夹里面的文件并创建文件夹
void VoiceManager::InitFolder(const char* folder_name,const char* full_folder_name)
{
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_RECORDMANAGER,"InitFolder","(Ljava/lang/String;Ljava/lang/String;)V");

	if (isHave)
	{
		jstring strfoldername = minfo.env->NewStringUTF(folder_name);
		jstring strfullfoldername = minfo.env->NewStringUTF(full_folder_name);
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,strfoldername,strfullfoldername);
		minfo.env->DeleteLocalRef(strfoldername);
		minfo.env->DeleteLocalRef(strfullfoldername);
		minfo.env->DeleteLocalRef(minfo.classID);


	}
}
//释放录制跟播放的资源
bool VoiceManager::ReleaseVoice()
{
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_RECORDMANAGER,"ReleaseVoice","()V");
	if (isHave)
	{
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);

	}
	return true;
}
//编码转换wav to amr
int VoiceManager::WavToAmr(const char* source_file_name,const char* distince_file_name)
{
	return 0;
}
//编码转换amr to wav
int VoiceManager::AmrToWav(const char* source_file_name,const char* distince_file_name)
{
	return 0;
}

void VoiceManager::VoiceRecordRelease()
{
	cocos2d::JniMethodInfo minfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo,
			CPP_INVOKE_JAVA_RECORDMANAGER,"VoiceRecordRelease","()V");
	if (isHave)
	{
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);

	}
}
extern
"C"
	{
		void JAVA_INVOKE_CPP_RECORD_START_CALLBACK(JNIEnv* env,jobject thiz, jint code)
		{
			if (VoiceManager::record_start_fun_id != 0)
			{
				LuaEngine::GetInstance()->CallFunction(VoiceManager::record_start_fun_id , "i",code);
			}
		}

		void JAVA_INVOKE_CPP_RECORD_STOP_CALLBACK(JNIEnv* env,jobject thiz, jint code, jint second,jstring sourcefilename,jstring distincefilename)
		{
			std::string strsourcefilename = "";
			std::string strdistincefilename = "";
			if(sourcefilename)
			{
				strsourcefilename = cocos2d::JniHelper::jstring2string(sourcefilename);
			}
			if(distincefilename)
			{
				strdistincefilename = cocos2d::JniHelper::jstring2string(distincefilename);
			}
			if(VoiceManager::record_stop_fun_id != 0)
			{
				LuaEngine::GetInstance()->CallFunction(VoiceManager::record_stop_fun_id , "iiss",code,second,strsourcefilename.c_str(),strdistincefilename.c_str());
			}
		}

		void JAVA_INVOKE_CPP_PLAY_START_CALLBACK(JNIEnv* env,jobject thiz, jint code,jint btn_index,jint module_index)
		{

			if (VoiceManager::play_start_fun_id != 0 && btn_index != 0 && module_index != 0)
			{

				VoiceManager::AddToPlayFinishList(code,btn_index,module_index,VoiceManager::play_start_fun_id);
				VoiceManager::current_btn_index = 0;
				VoiceManager::current_module_index = 0;
				VoiceManager::play_start_fun_id = 0;

			}
		}

		void JAVA_INVOKE_CPP_PLAY_STOP_CALLBACK(JNIEnv* env,jobject thiz, jint code)
		{
			if (VoiceManager::play_stop_fun_id != 0)
			{
				LuaEngine::GetInstance()->CallFunction(VoiceManager::play_stop_fun_id , "i",code);
			}
		}

}
