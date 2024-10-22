#include "voicehelper.h"
#include "platform/voice.h"
#if defined(__APPLE__) && !defined(__NO_VOICE_CONVERT__)
	#include "platform/ios/VoiceConvertInterface.h"
#endif
struct MessageMem;

DWORD VoiceHandleFunc(void *data_ptr)
{
	VoiceHelper* fs = (VoiceHelper*)data_ptr;
	fs->ThreadFun();
	return 0;
}

VoiceHelper::VoiceHelper()
	: m_is_stoped(false)
	, m_file_max_size(1024*20)
	, m_sem_for_unhandle_list(0)//TODO:Semaphore
{
	m_thread.Run(&VoiceHandleFunc, this);
}

VoiceHelper::~VoiceHelper()
{

}

bool VoiceHelper::Stop()
{
	m_is_stoped = true;
	m_sem_for_unhandle_list.Up();
	m_thread.Join();
	return true;
}

int VoiceHelper::CCAmrToWav(const std::string &src_file_name, const std::string &dest_file_name)
{

#if defined(__APPLE__) && !defined(__NO_VOICE_CONVERT__)
	if(src_file_name == ""  || dest_file_name == "")
	{
		return VOR_WAV_TO_AMR_ERR;
	}
	int change_ret =  VoiceConvertInterface::AmrToWav(src_file_name.c_str(),dest_file_name.c_str());
	if (change_ret != 0)
	{
		return VOR_WAV_TO_AMR_ERR;
	}
	bool ret_val = FileHelper::IsExsit(src_file_name);
	if(ret_val)
	{
		FileHelper::Remove(src_file_name.c_str());
	}
#endif
	return VOR_SUCC;
}

int VoiceHelper::CCWavToAmr(const std::string &src_file_name, const std::string &dest_file_name)
{
#if defined(__APPLE__) && !defined(__NO_VOICE_CONVERT__)
	if(src_file_name == ""  || dest_file_name == "")
	{
		return VOR_WAV_TO_AMR_ERR;
	}
	int change_ret =  VoiceConvertInterface::WavToAmr(src_file_name.c_str(),dest_file_name.c_str());
	if (change_ret != 0)
	{
		return VOR_WAV_TO_AMR_ERR;
	}
#endif
	return VOR_SUCC;
}

int VoiceHelper::MsgToFile(const std::string &src_file_name, void* data_ptr,unsigned int& data_size)
{
	if(src_file_name == ""  || data_ptr == 0 || data_size == 0)
	{
		return VOR_FROM_MESSAGE;
	}
	FILE* file_ptr = fopen(src_file_name.c_str() ,"wb");
	if(file_ptr == 0)
	{
		return VOR_FROM_MESSAGE;
	}
	fwrite(data_ptr, data_size, 1, file_ptr);
	fclose(file_ptr);
	return VOR_SUCC;
}

int VoiceHelper::FileToMsg(const std::string &dest_file_name,void*& data_ptr,unsigned int& data_size)
{
	if(dest_file_name == "")
	{
		return VOR_TO_MESSAGE;
	}
	struct stat amr_stat;
	int ret = stat(dest_file_name.c_str(), &amr_stat);
	if(0 != ret)
	{
		return VOR_TO_MESSAGE;
	}
	size_t file_size = (size_t)amr_stat.st_size;
	if (file_size <= 0 || file_size >= m_file_max_size)
	{
		return VOR_TO_MESSAGE;
	}
	FILE* file_ptr = fopen(dest_file_name.c_str(), "rb");
	if(!file_ptr)
	{
		return VOR_TO_MESSAGE;
	}
	data_ptr = malloc(file_size);
	fread(data_ptr, file_size, 1, file_ptr);
	data_size = file_size;
	fclose(file_ptr);

	return VOR_SUCC;
}

int VoiceHelper::RenameFile(const std::string &src_file_name,const std::string &dest_file_name, const std::string &rename_file_name)
{
	bool rev_val = false;
	#if defined(_WIN32)
		rev_val = FileHelper::Rename(dest_file_name,rename_file_name);
	#elif defined(__APPLE__)
		rev_val = FileHelper::Rename(src_file_name,rename_file_name);
	#elif defined(__ANDROID__)
		rev_val = FileHelper::Rename(dest_file_name,rename_file_name);
	#endif
	if(!rev_val)
	{
		return VOR_RENAME;
	}
	return VOR_SUCC;
}

int VoiceHelper::DeleteFile(const std::string &src_file_name)
{
	bool ret_val = FileHelper::IsExsit(src_file_name);
	if(ret_val)
	{
		FileHelper::Remove(src_file_name.c_str());
	}
	return VOR_SUCC;
}

void VoiceHelper::ThreadFun()
{
	while (!m_is_stoped)
	{
		if (Semaphore::Succeed != m_sem_for_unhandle_list.TryDown(1000))continue;
		VoiceInfoList tmp_list;
		m_mutex_for_unhandle_list.Lock();
		if(m_unhandle_list.empty())
		{
			m_mutex_for_unhandle_list.Unlock();
			continue;
		}
		m_unhandle_list.swap(tmp_list);
		m_mutex_for_unhandle_list.Unlock();

		for (VoiceInfoList::iterator iter = tmp_list.begin();iter!= tmp_list.end();++iter)
		{
			switch(iter->op_type)
			{
			case VOT_GET_MSG_FROM_FILE:
				{
					iter->op_result = CCWavToAmr(iter->src_file_name, iter->dest_file_name);
					if(iter->op_result == VOR_SUCC)
					{
						iter->op_result = FileToMsg(iter->dest_file_name,iter->data_ptr,iter->data_size);
					}
					if(iter->op_result == VOR_SUCC)
					{
						iter->op_result = RenameFile(iter->src_file_name,iter->dest_file_name, iter->rename_file_name);
						
					}
					break;
				}
			case VOT_SAVE_MSG_AS_FILE:
				{
					iter->op_result = MsgToFile(iter->src_file_name, iter->data_ptr,iter->data_size);
					if(iter->op_result == VOR_SUCC)
					{
						iter->op_result = CCAmrToWav(iter->src_file_name, iter->dest_file_name);
					}
					break;
				}
			case VOT_DELETE_FILE:
				{
					iter->op_result = DeleteFile(iter->src_file_name);
					break;
				}
			default:
				iter->op_result = VOR_TYPE_ERR;
				break;
			}
			m_mutex_for_handled_list.Lock();
			m_handled_list.push_back(*iter);
			m_mutex_for_handled_list.Unlock();
		}

	}
}

void VoiceHelper::Add(VoiceInfo vi)
{
	m_mutex_for_unhandle_list.Lock();
	m_unhandle_list.push_back(vi);
	m_mutex_for_unhandle_list.Unlock();
	m_sem_for_unhandle_list.Up();

}

VoiceHelper::VoiceInfoList VoiceHelper::Get()
{
	VoiceInfoList tmp_list;
	m_mutex_for_handled_list.Lock();
	m_handled_list.swap(tmp_list);
	m_mutex_for_handled_list.Unlock();
	return tmp_list;
}
