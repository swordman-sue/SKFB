#include "StdAfx.h"
#include "DBCFileManager.h"
#include "Common.h"

DBCFileManager::DBCFileManager()
{
	m_logFile = new LogFile;
	m_logFile->Init("DBCFileLog", "DBCFile", DEBUG_LEVEL);
}

DBCFileManager::~DBCFileManager()
{
	DBC_MAP::iterator it = m_dbcFiles.begin();
	for (; it != m_dbcFiles.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}

	if (m_logFile)
	{
		m_logFile->ShutDown();
		delete m_logFile;
	}
}

bool DBCFileManager::LoadAll()
{
	extern bool LoadDBCFiles();
	return LoadDBCFiles();
}

bool DBCFileManager::LoadAll(const DBCFileDes dbcDes[], int count)
{
	bool success = true;

	for (int i=0; i<count; ++i)
	{
		INT fileKey = dbcDes[i].key;						// 配置文件编号
		const char* fileName = dbcDes[i].fileName.c_str();	// 配置文件部分路径
		const char* typeFormat = dbcDes[i].typeFormat;
		IF_OK (fileName)
		{
			DBCFile* file = new DBCFile;
			if (file->Load(fileName, typeFormat))
				m_dbcFiles.insert(std::make_pair(fileKey, file));
			else
			{
				m_logFile->InsertLog(ERROR_LEVEL, file->GetLastError());
				success = false;
			}
		}
	}

	return success;
}

DBCFile* DBCFileManager::GetDBC(INT dbcKey) const
{
	DBC_MAP::const_iterator it = m_dbcFiles.find(dbcKey);
	if (it == m_dbcFiles.end())
		return NULL;
	return it->second;
}

void DBCFileManager::FreeDBC(INT dbcKey)
{
	DBC_MAP::iterator it = m_dbcFiles.find(dbcKey);
	if (it == m_dbcFiles.end())
		return;
	SAFE_DELETE(it->second);
	m_dbcFiles.erase(it);
}

bool DBCFileManager::ReloadDBC(INT dbcKey)
{
	DBC_MAP::iterator it = m_dbcFiles.find(dbcKey);
	if (it == m_dbcFiles.end())
		return false;

	DBCFile* file = it->second;
	m_dbcFiles.erase(it);
	if (file == NULL)
		return false;

	DBCFileDes des[1];
	des->fileName = file->GetFileName();
	des->typeFormat = file->GetTypeFormat();
	des->key = dbcKey;
	if (!this->LoadAll(des, 1))
		return false;

	SAFE_DELETE(file);
	return true;
}
