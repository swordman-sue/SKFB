#include <Windows.h>
#include "LuaInterface.h"
#include <string>

LuaInterface::LuaInterface()
{
	m_luaEngine = LuaEngine::Create();
}

LuaInterface::~LuaInterface()
{
	delete m_luaEngine;
}

bool LuaInterface::RegisterAll()
{
	extern bool LuaFuncExport();
	return LuaFuncExport();
}

/*
void LuaInterface::Reboot()
{
	delete m_luaEngine;
	m_luaEngine = LuaEngine::Create();
}
*/

// ��� Lua ���濪�� package �⣬�˺�����Ҫ�޸�
// Lua �������ʱ���� _CHECK_GLOBAL_FUNCTION_REDEFINE ��
// ���ʹ�� reload ���¼����ļ������¶���ĺ������ܲ��ܱ���鵽
bool LuaInterface::ReloadFile(const char* file)
{
	IF_NOT (file)
		return false;
#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
	m_luaEngine->DisableGlobalFunctionChecker();
#endif
	bool success = m_luaEngine->DoFile(file);
#ifdef _CHECK_GLOBAL_FUNCTION_REDEFINE
	m_luaEngine->EnableGlobalFunctionChecker();
#endif
	return success;
}

bool LuaInterface::LoadDir(const char* dir, bool bFirstLoad)
{
	IF_NOT (dir)
		return false;

	const char DIR_SPERATOR[] = "/";

	HANDLE hFile;
	WIN32_FIND_DATA findData;
	memset(&findData, 0, sizeof(findData));
	char searchName[MAX_PATH];
	strcpy(searchName, dir);
	strcat(searchName, "\\*.*");

	hFile = FindFirstFile(searchName, &findData);
	FindNextFile(hFile, &findData);

	bool res = true;
	while (FindNextFile(hFile, &findData))
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strcpy(searchName, dir);
			strcat(searchName, "\\");
			strcat(searchName, findData.cFileName);
			// ������ . ��ͷ��Ŀ¼������ .svn Ŀ¼��
			if (findData.cFileName[0] != '.')
				res &= this->LoadDir(searchName, bFirstLoad);
		}
		else
		{
			std::string fName = dir;
			fName += DIR_SPERATOR;
			fName += findData.cFileName;

			size_t len = strlen(fName.c_str());
			int i = 0;
			char ext[MAX_PATH];

			while (len > 0)
			{  
				ext[i++] = fName[--len];
				if (fName[len] == '.')
					break;
			}
			ext[i++] = '\0';
			if (!_stricmp(ext, "aul."))
			{
				if (bFirstLoad)
					res &= m_luaEngine->DoFile(fName.c_str());
				else
					res &= this->ReloadFile(fName.c_str());
			}
		}
	}

	FindClose(hFile);

	return res;
}
