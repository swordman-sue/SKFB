#ifndef SDK_FILE__TOOLS_H_
#define SDK_FILE__TOOLS_H_
#include "zqcommon/typedef.h"
#include <string>
#include "filesystem/fileasync.h"
#include "filesystem/filesystemex.h"
#include <map>
#include "cocos2d.h"

typedef std::map<ZQ::UINT32, FileListener*> ListenersMap;

class FileAsyncHandler
{
public:
    static bool Add(FileAsync::FileInfo _fileInfo);
	static FileAsync::FileInfoList	Get();
	static void SetMemLimit(unsigned int mem_limit);
	static bool ReadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len);
	static bool Copy(const char *src_full_path, const char *dest_full_path);
	static void Cancel(unsigned int async_id);

private:
	static void ReleasedMemSize(unsigned int mem_size);
	static  ListenersMap map_listener;

};
#endif
