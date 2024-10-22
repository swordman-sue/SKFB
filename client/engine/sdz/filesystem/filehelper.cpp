#include "filehelper.h"

#include "zqcommon/strutil.h"
#include "common/tinyxml/tinyxml.h"
#include "zip/zip.h"

#include <sys/stat.h>
#include <stdio.h>
#include <vector>

#if (defined(__unix) || defined (__APPLE__))
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#elif defined(_WIN32)
#include <Windows.h>
#include <direct.h>
#endif

// 在android和ios上，只能检测到/xxxx/yyyy目录下的文件是否存在
bool FileHelper::IsExsit(const std::string &full_name)
{
	struct stat tmp_stat;
	int ret = stat(full_name.c_str(), &tmp_stat);
	return (ret == 0);
}

bool FileHelper::Read(const std::string &full_file_name, char** data_ptr, unsigned int *data_len)
{
	struct stat tmp_stat;
	int ret = stat(full_file_name.c_str(), &tmp_stat);
	if(0 != ret)
	{
		return false;
	}
	size_t file_size = (size_t)tmp_stat.st_size;
	*data_len = (unsigned int)file_size;

	if(file_size > 0)
	{
		FILE* file_ptr = fopen(full_file_name.c_str(), "rb");
		if (file_ptr == 0)
		{
			return false;
		}
		(*data_ptr) = (char*)malloc(file_size+1);		// 多加一个字节，避免上层当明文读取越界
		fread(*data_ptr, file_size, 1, file_ptr);
		fclose(file_ptr);
	}

	return true;
}

bool FileHelper::Write(const std::string &full_file_name, const char *data_ptr, unsigned int data_len)
{
	if(IsExsit(full_file_name))
	{
		bool is_del_suc = Remove(full_file_name);
		if(!is_del_suc)
		{
			return false;
		}
	}

	std::string dir = full_file_name;
	if (!Up(&dir))
	{
		return false;
	}

	if (!CreateDir(dir))
	{
		return false;
	}

	FILE* file_ptr = fopen(full_file_name.c_str(),"wb");
	if(file_ptr == 0)
	{
		return false;
	}

#if defined(__APPLE__) || defined(__ANDROID__)
	int fd = fileno(file_ptr);
	fchmod(fd,S_IRWXU|S_IRWXG|S_IRWXO);
#endif

	fwrite(data_ptr, data_len, 1, file_ptr);
	fclose(file_ptr);

	return true;
}

bool FileHelper::WriteAdd(const std::string &full_file_name, const char* data_ptr, unsigned int data_len)
{
	std::string dir = full_file_name;
	if (!Up(&dir))
	{
		return false;
	}

	if (!CreateDir(dir))
	{
		return false;
	}

	FILE* file_ptr = fopen(full_file_name.c_str(), "ab+");
	if (file_ptr == 0)
	{
		return false;
	}

#if defined(__APPLE__) || defined(__ANDROID__)
	int fd = fileno(file_ptr);
	fchmod(fd, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

	fwrite(data_ptr, data_len, 1, file_ptr);
	fclose(file_ptr);

	return true;
}

bool FileHelper::Copy(const std::string &src_full_path, const std::string &dest_full_path, char **data_ptr, unsigned int *data_len, bool is_cover)
{
	char *tmp_data_ptr = 0;
	unsigned int tmp_data_len = 0;

	if (!Read(src_full_path, &tmp_data_ptr, &tmp_data_len))
	{
		return false;
	}

	if (is_cover || !IsExsit(dest_full_path))
	{
		if (!Write(dest_full_path, tmp_data_ptr, tmp_data_len))
		{
			free(tmp_data_ptr);
			tmp_data_ptr = 0;
			tmp_data_len = 0;
			return false;
		}
	}

	if (0==data_ptr)
	{
		free(tmp_data_ptr);
		tmp_data_ptr = 0;
	}
	else
	{
		*data_ptr = tmp_data_ptr;
	}

	if (0!=tmp_data_len && NULL != data_len)
	{
		*data_len = tmp_data_len;
	}

	return true;
}

bool FileHelper::Remove(const std::string &full_file_nam)
{
	int ret_val = remove(full_file_nam.c_str());
	return (ret_val != -1);
}

bool FileHelper::Rename(const std::string &old_file_name, const std::string &new_file_name)
{
	if(!IsExsit(old_file_name))
	{
		return false;
	}

	if(IsExsit(new_file_name))
	{
		if(!Remove(new_file_name))
		{
			return false;
		}
	}

	int ret_val = rename(old_file_name.c_str(), new_file_name.c_str());
	return (ret_val == 0);
}

bool FileHelper::CopyDir(const std::string &src_path, const std::string &dest_path, bool is_cover)
{
	GameType::StrVect file_list;
	GameType::StrVect dir_list;
	if (!GetFileListByDir(src_path, file_list, dir_list))
	{
		return false;
	}

	for (GameType::StrVect_CIter iter = file_list.begin();iter!=file_list.end();++iter)
	{
		if (!Copy(src_path +"/"+ (*iter), dest_path +"/"+ (*iter), 0, 0, is_cover))
		{
			return false;
		}
	}

	for (GameType::StrVect_CIter iter = dir_list.begin();iter!=dir_list.end();++iter)
	{
		if (!CopyDir(src_path+"/"+(*iter),dest_path+"/"+(*iter),is_cover))
		{
			return false;
		}
	}

	return true;
}

bool FileHelper::ChmodFile(const std::string &full_path)
{
	if (full_path == "" || !IsExsit(full_path))
	{
		return false;
	}

#if (defined(__unix) || defined (__APPLE__) || defined(__ANDROID__))
	chmod(full_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
	
	return true;
}

bool FileHelper::CreateDir(const std::string &dir)
{
	char now_path[1024] = {0};
	char path_copy[1024] = {0};
	strcpy(path_copy, dir.c_str());

#if (defined(__unix) || defined (__APPLE__) || defined(__ANDROID__))
	if (path_copy[0] == '/')
	{
		strcat(now_path, "/");
	}
#endif

	char *this_level = strtok(path_copy, "\\/");
	while (this_level != 0)
	{
		strcat(now_path, this_level);

#if (defined(__unix) || defined (__APPLE__) || defined(__ANDROID__))
		if (mkdir(now_path, S_IRWXU|S_IRWXG|S_IRWXO) != 0)
		{
			struct stat file_stat;
			if ((stat(now_path, &file_stat) == 0) &&!S_ISDIR(file_stat.st_mode))
			{
				return false;
			}
		}
		else
		{
			chmod(now_path, S_IRWXU | S_IRWXG | S_IRWXO);
		}
#elif defined(WP8)
		if (_mkdir(now_path) != 0)
		{
			std::wstringstream w_now_path;
			w_now_path << now_path;
			WIN32_FILE_ATTRIBUTE_DATA lpinf;
			GetFileAttributesEx(w_now_path.str().c_str(), GetFileExInfoStandard, &lpinf);
			if ((lpinf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				return false;
			}
		}
#elif defined(_WIN32)
		if (mkdir(now_path) != 0)
		{
			DWORD attributes = GetFileAttributesA(now_path);
			if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				return false;
			}
		}
#endif
		strcat(now_path, "/");
		this_level = strtok(NULL, "\\/");
	}

	return true;
}



#if (defined(__unix) || defined (__APPLE__))

bool FileHelper::GetFileListByDir(const std::string &dir, GameType::StrVect &file_list, GameType::StrVect &dir_list)
{
	DIR *dp;
	struct dirent *dirp;

	if((dp = opendir(dir.c_str())) == NULL)
	{
		return false;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		char currfile[1024] = {0};
		sprintf(currfile, "%s/%s", dir.c_str(), dirp->d_name);
		struct stat file_stat;
		stat(currfile, &file_stat);
		if (S_ISDIR(file_stat.st_mode))
		{
			std::string tmp_name = dirp->d_name;
			if (tmp_name == "." || tmp_name == "..")
			{
				continue;
			}
			dir_list.push_back(tmp_name);
		}
		else
		{
			file_list.push_back(dirp->d_name);
		}
	}
	closedir(dp);

	return true;
}

#elif defined(WP8)

bool FileHelper::GetFileListByDir(const std::string &dir, GameType::StrVect &file_list, GameType::StrVect &dir_list)
{
	WIN32_FIND_DATA FindFileData;
	char DirSpec[MAX_PATH + 8] = { 0 };
	strncpy(DirSpec, dir.c_str(), dir.length() + 1);
	strncat(DirSpec, "\\*", 3);

	std::wstringstream wss;
	wss << DirSpec;

	HANDLE hFind = FindFirstFileEx(wss.str().c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do
	{
		std::wstring w_file_name = FindFileData.cFileName;
		std::string file_name = std::string(w_file_name.begin(), w_file_name.end());
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (file_name == "." || file_name == "..")
			{
				continue;
			}
			dir_list.push_back(file_name);
		}
		else
		{
			file_list.push_back(file_name);
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	DWORD dwError = GetLastError();
	FindClose(hFind);
	if (dwError != ERROR_NO_MORE_FILES)
	{
		return false;
	}

	return true;
}

#elif defined(_WIN32)

bool FileHelper::GetFileListByDir(const std::string &dir, GameType::StrVect &file_list, GameType::StrVect &dir_list)
{
	WIN32_FIND_DATAA FindFileData;
	char DirSpec[MAX_PATH + 8] = { 0 };
	strncpy(DirSpec, dir.c_str(), dir.length() + 1);
	strncat(DirSpec, "\\*", 3);

	HANDLE hFind = FindFirstFileExA(DirSpec, FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::string tmp_name = FindFileData.cFileName;
			if (tmp_name == "." || tmp_name == "..")
			{
				continue;
			}
			dir_list.push_back(tmp_name);
		}
		else
		{
			file_list.push_back(FindFileData.cFileName);
		}
	} while (FindNextFileA(hFind, &FindFileData) != 0);

	DWORD dwError = GetLastError();
	FindClose(hFind);
	if (dwError != ERROR_NO_MORE_FILES)
	{
		return false;
	}

	return true;
}
#endif

bool FileHelper::UnzipAndroidFile(const std::string &apk_file_path, const std::string &full_file_name, void** data_ptr, unsigned int *data_len)
{
	const static std::string android_assets_path_header = "assets/";
	std::vector<std::string> filter_file_list;
	filter_file_list.push_back(android_assets_path_header + full_file_name);

	Zip tmp_zip;
	tmp_zip.SetFilterFileList(filter_file_list);
	if (!tmp_zip.Unzip(apk_file_path.c_str(), 0))
	{
		return false;
	}

	void *buff;
	int buff_len;
	if (!tmp_zip.GetUzFileData((android_assets_path_header + full_file_name).c_str(), &buff, &buff_len))
	{
		return false;
	}

	*data_ptr = malloc(buff_len);
	memcpy(*data_ptr,buff,buff_len);
	*data_len = buff_len;

	return true;
}

bool FileHelper::UnzipAndroidDirTo(const std::string &apk_file_path, const std::string &src_path, const std::string &dest_path, bool is_cover)
{
	const static std::string android_assets_path_header = "assets/";
	std::string zip_src_path = android_assets_path_header + src_path;
	if(!ZQ::StrUtil::EndsWith(src_path, "/"))
	{
		zip_src_path = zip_src_path + "/";
	}
	std::vector<std::string> filter_file_list;
	filter_file_list.push_back(zip_src_path);

	Zip tmp_zip;
	tmp_zip.SetFilterFileList(filter_file_list);
	if (!tmp_zip.Unzip(apk_file_path.c_str(), 0))
	{
		return false;
	}

	const Zip::FileInfoMap* file_info_map = tmp_zip.GetAllUzFileData();
	for (Zip::FileInfoMap::const_iterator iter = file_info_map->begin();iter!=file_info_map->end();++iter)
	{
		std::string tmp_dest_path = iter->second.file_name;
		unsigned int tmp_index = tmp_dest_path.find_first_of(android_assets_path_header + src_path);
		if(tmp_index!=0)
		{
			return false;
		}
		tmp_dest_path.erase(0, zip_src_path.length()-1);

		if (is_cover || !IsExsit(dest_path+tmp_dest_path))
		{
			if (!Write(dest_path+tmp_dest_path, (const char*)(iter->second.buff), iter->second.len))
			{
				return false;
			}
		}
	}

	return true;
}

bool FileHelper::UnzipFile(const std::string &apk_file_path, const std::string &full_file_name, void** data_ptr, unsigned int *data_len)
{
	std::vector<std::string> filter_file_list;
	filter_file_list.push_back(full_file_name);

	Zip tmp_zip;
	tmp_zip.SetFilterFileList(filter_file_list);
	if (!tmp_zip.Unzip(apk_file_path.c_str(), 0))
	{
		return false;
	}

	void *buff;
	int buff_len;
	if (!tmp_zip.GetUzFileData((full_file_name).c_str(), &buff, &buff_len))
	{
		return false;
	}

	*data_ptr = malloc(buff_len);
	memcpy(*data_ptr,buff,buff_len);
	*data_len = buff_len;

	return true;
}

bool FileHelper::UnzipFile(Zip* zip, const std::string& src_file_name, const std::string& dst_file_path)
{
	void *buff;
	int buff_len;
	if (!zip->GetUzFileData((src_file_name).c_str(), &buff, &buff_len))
	{
		return false;
	}

	Write(dst_file_path, (const char*)buff, buff_len);

	return true;
}

bool FileHelper::ReadVersionRootPropertyFromIos(const std::string& config_file_path, std::map<std::string, std::string>& property_name)
{
	Zip temp_zip;
	void *buffer = 0;
	int unzip_buf_size;

	if (!temp_zip.Unzip(config_file_path.c_str(), 0) || !temp_zip.GetUzFileData("version.xml", &buffer, &unzip_buf_size))
	{
		return false;
	}

	TiXmlDocument document;
	if (!document.Parse((const char *)buffer))
	{
		return false;
	}

	const TiXmlElement* root_elem = document.RootElement();
	if (nullptr == root_elem)
	{
		return false;
	}

	for (auto& it : property_name)
	{
		const char* value = root_elem->Attribute(it.first.c_str());
		if (value)
		{
			it.second = value;
		}
	}

	return true;
}
