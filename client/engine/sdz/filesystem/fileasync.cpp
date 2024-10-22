
#include "fileasync.h"
#include "filehelper.h"
#include "filesystemex.h"
#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"
#include "common/platform/system.h"
#include "zip/zip.h"

#include "base/ZipUtils.h"
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformDefine.h"

#include <algorithm>
#include <assert.h>
#include <list>

const std::string ScriptZipFilePassword = "zlgamecspw";

#if defined(__APPLE__) || defined(__ANDROID__)
inline long InterlockedIncrement(volatile long *val)
{
	//__gnu_cxx::__exchange_and_add((volatile int *)val, 1);
	__sync_fetch_and_add((volatile int *)val,1);
	return *val;

}
#endif

unsigned int GetAsyncID()
{
	static long async_id = 0;
	async_id = (unsigned int)(InterlockedIncrement(&async_id));
	return async_id;
}

DWORD FileHandleFunc(void *data_ptr)
{
	FileAsync* fs = (FileAsync*)data_ptr;
	fs->ThreadFun();
	return 0;
}

FileAsync::FileAsync()
	:m_is_stoped(false)
	,m_total_mem_len(1024*1024*5)
	,m_used_mem_len(0)
	,m_sem_for_unhandle_list(0)//TODO:Semaphore
{
	m_thread.Run(&FileHandleFunc, this);
}

FileAsync::~FileAsync()
{
}

void FileAsync::ThreadFun()
{
	while (!m_is_stoped)
	{
		if (m_used_mem_len>m_total_mem_len)
		{
			PISleep(500);
			continue;
		}
		if (Semaphore::Succeed != m_sem_for_unhandle_list.TryDown(1000))continue;

		FileInfoList tmp_list;

		m_mutex_for_unhandle_list.Lock();
		if(m_unhandle_list.empty())
		{
			m_mutex_for_unhandle_list.Unlock();
			continue;
		}
		m_unhandle_list.swap(tmp_list);
		m_mutex_for_unhandle_list.Unlock();
		
		for (FileInfoList::iterator iter = tmp_list.begin();iter!= tmp_list.end();++iter)
		{
			iter->op_result = FOR_SUCC;
			switch(iter->op_type)
			{
				case FOT_READ:
				{
					bool ret = FileHelper::Read(iter->path, &(iter->ptr), &(iter->len));
					if (!ret)
					{
						iter->op_result = FOR_READ_ERR;
					}
					break;
				}
				case FOT_WRITE:
				{
					bool ret = FileHelper::Write(iter->path, iter->ptr, iter->len);
					if (!ret)
					{
						iter->op_result = FOR_WRITE_ERR;
					}
					break;
				}
				case FOT_COPY:
				{
					bool ret = FileHelper::Copy(iter->path, iter->dest_path, 0, &(iter->len));
					if (!ret)
					{
						iter->op_result = FOR_COPY_ERR;
					}
					break;
				}
				case FOT_VERSION_FILE_LOAD:
				{
					bool ret = LoadAllVersionFile(iter->path, iter->dest_path, iter->expansion_path,
						iter->app_version_str, iter->svn_version_str,
						iter->version_file_init, iter->version_file_download, 
						iter->version_file_server,iter->version_file_expansion, iter->op_result_desc);
					if (!ret)
					{
						iter->op_result = FOR_VERSION_FILE_LOAD_ERR;
					}
					break;
				}
				case FOT_VERSION_FILE_WRITE:
				{
					bool ret = WriteVersionFile(iter->path, iter->version_file_download);
					if (!ret)
					{
						iter->op_result = FOR_VERSION_FILE_WRITE_ERR;
					}
					break;
				}
				case FOT_SETTING_FILE_WRITE:
				{
					bool ret = WriteSettingFile(iter->path, iter->setting_file_specail_info);
					if (!ret)
					{
						iter->op_result = FOR_SETTING_FILE_WRITE_ERR;
					}
					break;
				}
				case FOT_UNZIP_RESPACKAGE:
				{
					const std::string& ResVersionXml = "res_version.xml";
					std::vector<std::string>& unzip_file_list = iter->unzip_file_list;
					for (auto it = unzip_file_list.begin(); it != unzip_file_list.end();)
					{
						cocos2d::ZipFile zip_file((*it));
						std::string&& next_file_name = zip_file.getFirstFilename();
						unsigned char* buffer = nullptr;
						ssize_t buffer_size = 0;
						std::list<std::string> unzip_res_version_list;
						while (next_file_name != "")
						{
							if (next_file_name[next_file_name.length()-1] != '/')
							{
								buffer = zip_file.getFileData(next_file_name, &buffer_size);
								if (buffer == nullptr || buffer_size == 0 || !FileHelper::Write(iter->dest_path + next_file_name, (const char*)buffer, buffer_size))
								{
									LogUtil::LogError("file name:%s unzip is error!", next_file_name.c_str());
									iter->op_result = FOR_UNZIP_FILE_INVALID_ERR;
									break;
								}
								else
								{
									unzip_res_version_list.push_back(next_file_name);
								}
								free(buffer);
								buffer = nullptr;
								buffer_size = 0;
							}
							next_file_name = zip_file.getNextFilename();
						}

						const std::string& res_version_path = iter->dest_path + ResVersionXml;
						GameType::StrUIntKVMap res_version_list;
						if (!iter->op_result && FileHelper::IsExsit(res_version_path) && ReadResVersionFromXml(res_version_path, res_version_list))
						{
							FileHelper::Remove(res_version_path);

							auto unzip_it = unzip_res_version_list.begin();
							auto unzip_it_end = unzip_res_version_list.end();
							for (; unzip_it != unzip_it_end; unzip_it++)
							{
								if (*unzip_it == ResVersionXml)
								{
									continue;
								}

								auto tmp_it = res_version_list.find(*unzip_it);
								if (tmp_it != res_version_list.end())
								{
									iter->version_file_download[*unzip_it] = tmp_it->second;
								}
								else
								{
									LogUtil::LogError("file name:%s can't find resource version in %s!", (*unzip_it).c_str(), ResVersionXml.c_str());
								}
							}
						}

						if (!iter->op_result)
						{
							it = unzip_file_list.erase(it);
						}
						else
						{
							break;
						}
					}
					break;
				}
				case FOT_SCRIPT_IOS_UNZIP:
				{
					Zip *zip = new Zip;
					GameType::StrVect_Iter file_iter = iter->unzip_file_list.begin();
					while(file_iter != iter->unzip_file_list.end())
					{
                        std::string& script_file_key = *file_iter;
                        std::string script_file_path = FileSystemEx::GetInstance()->GetResourcePath(script_file_key);
                        cocos2d::Data fileData = cocos2d::FileUtils::getInstance()->getDataFromFile(script_file_path);
                        if (fileData.isNull() || !zip->UnzipBuffer((const char*)fileData.getBytes(), (unsigned int)fileData.getSize(), ScriptZipFilePassword.c_str()))
						{
							delete zip;
                            iter->path = script_file_key;
                            iter->op_result = fileData.isNull() ? FOR_SCRIPT_IOS_LOAD_ERR : FOR_SCRIPT_IOS_UNZIP_ERR;
							break;
						}
						++file_iter;
					}
					if (iter->op_result == FOR_SUCC)
					{
						iter->script_ios_zip = zip;
					}
					break;
				}
				case FOT_GET_DATA_FRM_FILE:
				{
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(iter->path);
					if (!cocos2d::FileUtils::getInstance()->isFileExist(fullPath))
					{
						iter->op_result = FOR_GET_DATA_FRM_FILE_ERR;
					}
					else
					{
						iter->file_data = cocos2d::FileUtils::getInstance()->getDataFromFile(fullPath);
						if (iter->file_data.isNull())
						{
							iter->op_result = FOR_GET_DATA_FRM_FILE_ERR;
						}
					}
					break;
				}
				default:
					iter->op_result = FOR_OP_TYPE_ERR;
					break;
			}

			m_mutex_for_handled_list.Lock();
			m_handled_list.push_back(*iter);
			m_mutex_for_handled_list.Unlock();
		}

	}
}

unsigned int FileAsync::Add(FileInfo fi)
{
	unsigned int async_id = GetAsyncID();
	fi.async_id = async_id;

	m_mutex_for_unhandle_list.Lock();
	m_unhandle_list.push_back(fi);
	m_mutex_for_unhandle_list.Unlock();
	m_sem_for_unhandle_list.Up();

	return async_id;
}

void FileAsync::Cancel(unsigned int async_id)
{
	FileInfo tmp(async_id);
	m_mutex_for_unhandle_list.Lock();
	FileInfoList::iterator iter = find(m_unhandle_list.begin(), m_unhandle_list.end(), tmp);
	if (iter != m_unhandle_list.end())
	{
		if (iter->listener != 0)
		{
			iter->listener->Free();
		}
		m_unhandle_list.erase(iter);
	}
	m_mutex_for_unhandle_list.Unlock();
	m_sem_for_unhandle_list.Up();
}

FileAsync::FileInfoList FileAsync::Get()
{
	FileInfoList tmp_list;
	m_mutex_for_handled_list.Lock();
	m_handled_list.swap(tmp_list);
	m_mutex_for_handled_list.Unlock();
	return tmp_list;
}

void FileAsync::SetMemLimit(unsigned int mem_limit)
{
	m_total_mem_len = mem_limit;
}

void FileAsync::Stop()
{
	if (!m_is_stoped)
	{
		m_is_stoped = true;
		m_sem_for_unhandle_list.Up();
		m_thread.Join();
	}
}