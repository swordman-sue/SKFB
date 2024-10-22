#include "voicemanager.h"
#include "platform/voice.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include <sys/stat.h>

#if defined(__ANDROID__)
#include "platform/android/msgasynchandler.h"
#include "platform/android/msgdata.h"
#include <typeinfo>
#endif

static VoiceManager* s_instance = nullptr;

VoiceManager* VoiceManager::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new VoiceManager;
	}
	return s_instance;
}

void VoiceManager::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool VoiceManager::Init()
{
	m_update_interval = 0.2f;
	
	m_voice_path = FileSystemEx::GetInstance()->GetNonVersionPath() + "voice/";
	m_tmp_amr_full_name = m_voice_path + "record.amr";
	m_tmp_wav_full_name = m_voice_path + "record.wav";

	if (!FileHelper::CreateDir(m_voice_path.c_str()))
	{
		LogUtil::LogError("[VoiceManager::Init CreateDirError][dir:%s]", m_voice_path.c_str());
		return false;
	}
	return true;
}

bool VoiceManager::Start()
{
	return true;
}

bool VoiceManager::Update(float interval)
{
	VoiceHelper::VoiceInfoList tmp_list = m_voice_helper.Get();
	for (VoiceHelper::VoiceInfoList::iterator iter = tmp_list.begin();iter!=tmp_list.end();++iter)
	{
		switch(iter->op_type)
		{
		case VoiceHelper::VOT_GET_MSG_FROM_FILE:
			{
				// 修改直接调用CallFunction并向回调函数传递语音数据块和语音数据大小参数 edit by JiaMiao 2016-2-18
				LuaEngine::GetInstance()->CallFunction(iter->callback_id, "IiIis#i", iter->callback_id, iter->op_result, iter->voice_id, iter->voice_length, 
					iter->data_ptr, iter->data_size, iter->data_size);

				free(iter->data_ptr);
				iter->data_ptr = 0;
				iter->data_size = 0;

				break;
			}
		case VoiceHelper::VOT_SAVE_MSG_AS_FILE:
			{
				LuaEngine::GetInstance()->CallFunction(iter->callback_id, "IiIi",iter->callback_id,iter->op_result,iter->voice_id,iter->voice_length);
				break;
			}
		case VoiceHelper::VOT_DELETE_FILE:
			{
				LuaEngine::GetInstance()->CallFunction(iter->callback_id, "IiI",iter->callback_id,iter->op_result,iter->voice_id);
				break;
			}
		default:
			iter->op_result = VoiceHelper::VOR_TYPE_ERR;
			break;
		}
	}
#if defined(__ANDROID__)
	m_update_interval = m_update_interval + interval;
	if(m_update_interval > 0.2f)
	{
		m_update_interval = 0.0f;
		MsgInfoList msginfos = MessageAsyncHandler::Get(MessageData::MSG_PLAY_FINISH);
		for (MsgInfoList::iterator iter = msginfos.begin();iter!= msginfos.end();++iter)
		{
			MessageData *msgdata = iter->message;
			if(typeid(*msgdata) == typeid(MsgPlayFinishData))
			{
				MsgPlayFinishData *paly_finish = dynamic_cast<MsgPlayFinishData *>(msgdata);
				LuaEngine::GetInstance()->CallFunction(paly_finish->luafunid,"IiI",paly_finish->luafunid,paly_finish->code,paly_finish->voiceid);
			}
			delete msgdata;
		}
	}
#endif
	return true;
}

bool VoiceManager::Pause()
{
	return true;
}

bool VoiceManager::Resume()
{
	return true;
}

bool VoiceManager::Stop()
{
	m_voice_helper.Stop();
	return true;
}

bool VoiceManager::Release()
{
	Voice::ReleaseVoice();
	return true;
}

// 修改直接向SaveMsgAsFile函数传入语音数据块，不用旧的传递MessageMem的方式 edit by JiaMiao 2016-2-18
int VoiceManager::SaveMsgAsFile(char *voice_msg, unsigned int voice_msg_len, unsigned int voice_id,int voice_length,unsigned int lua_fun_id)
{
	if (voice_id == 0 || lua_fun_id == 0 || voice_msg_len == 0)
	{
		return VOICE_PARAMETER_ERROR;
	}
	std::string src_file_name = GetFileNameByVoiceID(voice_id,AMR);
	std::string dest_file_name = GetFileNameByVoiceID(voice_id,WAV);
	VoiceHelper::VoiceInfo vi;
	vi.op_type = VoiceHelper::VOT_SAVE_MSG_AS_FILE;
	vi.callback_id = lua_fun_id;
	vi.data_ptr = (void *)voice_msg;
	vi.data_size = voice_msg_len;
	vi.src_file_name = src_file_name;
	vi.dest_file_name = dest_file_name;
	vi.voice_id = voice_id;
	vi.voice_length = voice_length;
	m_voice_helper.Add(vi);
	return VOICE_SUCCESS;
}

int VoiceManager::StartRecord(int secord)
{
	if(secord <= 0)
	{
		return VOICE_PARAMETER_ERROR;
	}
	#if defined(_WIN32)
		return Voice::StartRecord(m_tmp_amr_full_name.c_str(),secord);
	#elif defined(__APPLE__)
		return Voice::StartRecord(m_tmp_wav_full_name.c_str(),secord);
	#elif defined(__ANDROID__)
		return Voice::StartRecord(m_tmp_amr_full_name.c_str(),secord);
	#endif
}

int VoiceManager::StopRecord(unsigned int voice_id,unsigned int lua_func_id)
{
	if(voice_id == 0 || lua_func_id == 0)
	{
		return VOICE_PARAMETER_ERROR;
	}
	int ret_val =  Voice::StopRecord();
	if(ret_val <= 0)
	{
		return VOICE_TOO_SMALL;
	}
	std::string rename_file_name = "";
	std::string src_file_name = m_tmp_wav_full_name;
	std::string dest_file_name = m_tmp_amr_full_name;
	bool ret_bool = false;
	struct stat tmp_stat;
	#if defined(_WIN32)
		rename_file_name = GetFileNameByVoiceID(voice_id,AMR);
		ret_bool = FileHelper::IsExsit(dest_file_name);
		stat(dest_file_name.c_str(), &tmp_stat);
	#elif defined(__APPLE__)
		rename_file_name = GetFileNameByVoiceID(voice_id,WAV);
		ret_bool = FileHelper::IsExsit(src_file_name);
		stat(src_file_name.c_str(), &tmp_stat);
	#elif defined(__ANDROID__)
		rename_file_name = GetFileNameByVoiceID(voice_id,AMR);
		ret_bool = FileHelper::IsExsit(dest_file_name);
		stat(dest_file_name.c_str(), &tmp_stat);
	#endif
	size_t file_size = (size_t)tmp_stat.st_size;
	if(!ret_bool || file_size == 0)
	{
		return VOICE_FILE_ERROR;
	}
	VoiceHelper::VoiceInfo vi;
	vi.op_type = VoiceHelper::VOT_GET_MSG_FROM_FILE;
	vi.callback_id = lua_func_id;
	vi.voice_id = voice_id;
	vi.src_file_name = src_file_name;
	vi.dest_file_name = dest_file_name;
	vi.rename_file_name = rename_file_name;
	vi.voice_length = ret_val;
	m_voice_helper.Add(vi);
	return VOICE_SUCCESS;
}

int VoiceManager::StartPlay(unsigned int voice_id, unsigned int lua_fun_id)
{
	if(voice_id == 0 || lua_fun_id == 0)
	{
		return VOICE_PARAMETER_ERROR;
	}
	std::string play_name = "";
	#if defined(_WIN32)
		play_name = GetFileNameByVoiceID(voice_id,AMR);
	#elif defined(__APPLE__)
		play_name = GetFileNameByVoiceID(voice_id,WAV);
	#elif defined(__ANDROID__)
		play_name = GetFileNameByVoiceID(voice_id,AMR);
	#endif
	bool ret_bool = FileHelper::IsExsit(play_name);
	if(!ret_bool)
	{
		return VOICE_FILE_ERROR;
	}
	return Voice::StartPlay(play_name.c_str(),voice_id,lua_fun_id);
}

int VoiceManager::StopPlay(unsigned int voice_id)
{
	return Voice::StopPlay();
}

int VoiceManager::ReleaseVoice()
{
	return Voice::ReleaseVoice();
}

int VoiceManager::DeleteFileByVoiceId(unsigned int voice_id,unsigned int lua_func_id)
{
	std::string delete_name = "";
	#if defined(_WIN32)
		delete_name = GetFileNameByVoiceID(voice_id,AMR);
	#elif defined(__APPLE__)
		delete_name = GetFileNameByVoiceID(voice_id,WAV);
	#elif defined(__ANDROID__)
		delete_name = GetFileNameByVoiceID(voice_id,AMR);
	#endif
	VoiceHelper::VoiceInfo vi;
	vi.op_type = VoiceHelper::VOT_DELETE_FILE;
	vi.callback_id = lua_func_id;
	vi.voice_id = voice_id;
	vi.src_file_name = delete_name;
	m_voice_helper.Add(vi);
	return VOICE_SUCCESS;
}

std::string VoiceManager::GetFileNameByVoiceID(unsigned int voice_id,int file_type)
{
	std::string voice_id_str;
	ZQ::StrUtil::BaseTypeToString(voice_id, voice_id_str);

	if(file_type == AMR)
	{
		return m_voice_path + voice_id_str + "a";
	}
	else
	{
		return m_voice_path + voice_id_str + "w";
	}
}


