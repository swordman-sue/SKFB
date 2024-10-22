#ifndef ZIP_H
#define ZIP_H

#include <string>
#include <map>
#include <vector>
#include "zqcommon/typedef.h"

class CommonZipFilePrivate;

class Zip
{
public:
	Zip() {};
	~Zip();

	struct FileInfo
	{
		FileInfo() : buff(0), len(0)
		{
		}

		~FileInfo()
		{
		}

		std::string file_name;
		void* buff;
		unsigned int len;
	};
	typedef std::map<std::string, FileInfo> FileInfoMap;

	bool Unzip(const char* zipfilename, const char* password);
    bool UnzipBuffer(const char* buffer, unsigned int bufferLen, const char* password);
	bool GetUzFileData(std::string file_name, void** buff, int* len) const;
	const FileInfoMap* GetAllUzFileData() const;
	void GetFileList(GameType::StrVect& filelist) const;
	void SetFilterFileList(GameType::StrVect& file_name_list);

private:
	bool SetFilter(CommonZipFilePrivate* data);
	bool IsFileInFilter(std::string& file_name);
    bool UnzipInternal(CommonZipFilePrivate* pZipFilePrv, const char* password);

private:
	FileInfoMap m_unzip_map;
	GameType::StrVect m_filter_file_list;
};

#endif