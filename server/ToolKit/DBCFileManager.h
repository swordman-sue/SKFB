#pragma once

#include "DBCFile.h"
#include <hash_map>
#include <string>
#include "Singleton.h"
#include "LogFile.h"

#define DBC_LOADER_DEF_BEGIN					bool LoadDBCFiles() {\
													const DBCFileDes dbcFiles[] = {
#define DBC_FILE_LOAD(dbcKey, dbcFile, typeFormat)		{dbcKey, dbcFile, typeFormat},
#define DBC_LOADER_DEF_END							};\
													return DBCFileManager::Instance()->LoadAll(dbcFiles, sizeof(dbcFiles) / sizeof(DBCFileDes));\
												}

struct DBCFileDes
{
	INT key;
	std::string fileName;
	const char* typeFormat;
};

class DBCFileManager : public Singleton<DBCFileManager>
{
public:
	DBCFileManager();
	~DBCFileManager();
	// 通过 dbc loader 来载入 dbc 文件
	// dbc loader 通过 DBC_LOADER 等宏来定义
	bool LoadAll();
	bool LoadAll(const DBCFileDes dbcDes[], int count);
	DBCFile* GetDBC(INT dbcKey) const;
	void FreeDBC(INT dbcKey);
	bool ReloadDBC(INT dbcKey);
private:
#ifdef __SGI_STL_PORT
	typedef std::map<INT, DBCFile*> DBC_MAP;
	//typedef std::hash_map<INT, DBCFile*> DBC_MAP;
#else
	typedef stdext::hash_map<INT, DBCFile*> DBC_MAP;
#endif
	DBC_MAP m_dbcFiles;						// 配置文件列表
	LogFile* m_logFile;

private:
	DBCFileManager(const DBCFileManager&);
	void operator=(const DBCFileManager&);
};
