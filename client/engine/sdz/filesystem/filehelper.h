
#ifndef FILEHELPER_H
#define FILEHELPER_H

#include "zqcommon/typedef.h"

// 文件路径的最大长度
#define GAMERES_PATHLEN_MAX 256

class Zip;

class FileHelper
{
public:
	static bool IsExsit(const std::string& full_name);

	static bool Read(const std::string &full_file_name, char** data_ptr, unsigned int *data_len);
	static bool Write(const std::string &full_file_name, const char* data_ptr, unsigned int data_len);
	static bool WriteAdd(const std::string &full_file_name, const char* data_ptr, unsigned int data_len);

	static bool Copy(const std::string &src_full_path, const std::string &dest_full_path, char **data_ptr, unsigned int *data_len, bool is_cover=true);
	static bool Remove(const std::string &full_file_name);

	static bool Rename(const std::string &old_file_name, const std::string &new_file_name);
	static bool CreateDir(const std::string &dir);
	static bool CopyDir(const std::string &src_path, const std::string &dest_path, bool is_cover);

	static bool ChmodFile(const std::string &full_path);

	static bool GetFileListByDir(const std::string &dir, GameType::StrVect &file_list, GameType::StrVect &dir_list);

	static bool UnzipAndroidFile(const std::string &zip_file_path, const std::string &full_file_name, void** data_ptr, unsigned int *data_len);
	static bool UnzipAndroidDirTo(const std::string &apk_file_path, const std::string &src_path, const std::string &dest_path, bool is_cover);

	static bool UnzipFile(const std::string &zip_file_path, const std::string &full_file_name, void** data_ptr, unsigned int *data_len);
	static bool UnzipFile(Zip* zip, const std::string& src_file_name, const std::string& dst_file_path);

	static bool ReadVersionRootPropertyFromIos(const std::string& config_file_path, std::map<std::string, std::string>& property_name);

private:
	inline static bool IsPathSpliter(char c)
	{
		return c == '/' || c == '\\';
	}
	inline static bool Up(std::string *path)
	{
		for(int i = (int)path->length()-1;i>=0;--i)
		{
			if (IsPathSpliter( (*path)[i]))
			{
				path->resize(i);
				return true;
			}
		}
		return false;
	}
};

#endif
