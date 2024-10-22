/****************************************************************************
* Zipπ§æﬂ¿‡
* Created by Swordman Sue 
* Copyright (c) deepseagame All rights reserved.
****************************************************************************/

#include "zip.h"
#include <zlib.h>
#include <assert.h>
#include "unzip/unzip.h"

#define COMMONZIP_MAXPATH 256
#define SAFE_DELETE(p) do { if(p) { delete (p); (p) = 0; } } while(0)

using namespace cocos2d;
using namespace GameType;

struct CommonZipEntryInfo
{
	unz_file_pos pos;
	uLong uncompressed_size;
};

class CommonZipFilePrivate
{
public:
	unzFile zipFile;

	// std::unordered_map is faster if available on the platform
	typedef std::map<std::string, struct CommonZipEntryInfo> FileListContainer;
	FileListContainer fileList;
};

Zip::~Zip()
{
	for (FileInfoMap::iterator it = m_unzip_map.begin(); it != m_unzip_map.end(); ++it)
	{
		delete[] it->second.buff;
		it->second.buff = NULL;
	}
	m_unzip_map.clear();
}

bool Zip::Unzip(const char* zipfilename, const char* password) 
{
	if (nullptr == zipfilename)
	{
		return false;
	}

	CommonZipFilePrivate* pZipFilePrv = new CommonZipFilePrivate();
	pZipFilePrv->zipFile = unzOpen(zipfilename);
    return UnzipInternal(pZipFilePrv, password);
}

bool Zip::UnzipBuffer(const char *buffer, unsigned int bufferLen, const char *password)
{
    if (nullptr == buffer || bufferLen <= 0)
    {
        return false;
    }
    
    CommonZipFilePrivate* pZipFilePrv = new CommonZipFilePrivate();
    pZipFilePrv->zipFile = unzOpenBuffer(buffer, bufferLen);
    return UnzipInternal(pZipFilePrv, password);
}

bool Zip::UnzipInternal(CommonZipFilePrivate* pZipFilePrv, const char* password)
{
    if (nullptr == pZipFilePrv)
    {
        return false;
    }
    
    if (!pZipFilePrv->zipFile)
    {
        SAFE_DELETE(pZipFilePrv);
        return false;
    }
    if (!SetFilter(pZipFilePrv))
    {
        SAFE_DELETE(pZipFilePrv);
        return false;
    }
    
    bool ret = true;
    CommonZipFilePrivate::FileListContainer::iterator it = pZipFilePrv->fileList.begin();
    while (it != pZipFilePrv->fileList.end())
    {
        CommonZipEntryInfo entryInfo = it->second;
        
        int nRet = unzGoToFilePos(pZipFilePrv->zipFile, &entryInfo.pos);
        if (UNZ_OK != nRet)
        {
            ret = false;
            break;
        }
        
        nRet = unzOpenCurrentFilePassword(pZipFilePrv->zipFile, password);
        if (UNZ_OK != nRet)
        {
            ret = false;
            break;
        }
        
        Zip::FileInfo fileInfo;
        fileInfo.buff = new unsigned char[entryInfo.uncompressed_size];
        fileInfo.len = entryInfo.uncompressed_size;
        fileInfo.file_name = it->first;
        
        int nSize = unzReadCurrentFile(pZipFilePrv->zipFile, fileInfo.buff, entryInfo.uncompressed_size);
        if (nSize != 0 && nSize != (int)entryInfo.uncompressed_size)
        {
            ret = false;
            break;
        }
        unzCloseCurrentFile(pZipFilePrv->zipFile);
        
        m_unzip_map.insert(std::make_pair(fileInfo.file_name, fileInfo));
        
        ++it;
    }
    unzClose(pZipFilePrv->zipFile);
    
    SAFE_DELETE(pZipFilePrv);
    
    return ret;
}

bool Zip::GetUzFileData(std::string file_name, void** buff, int* len) const 
{
	FileInfoMap::const_iterator it = m_unzip_map.find(file_name);
	if (it != m_unzip_map.end())
	{
		(*buff) = it->second.buff;
		(*len) = it->second.len;
		return true;
	}
	return false;
}

const Zip::FileInfoMap* Zip::GetAllUzFileData() const 
{
	return &m_unzip_map;
}

void Zip::GetFileList(StrVect& filelist) const
{
	filelist.clear();
	for (FileInfoMap::const_iterator it = m_unzip_map.begin(); it != m_unzip_map.end(); ++it)
	{
		filelist.push_back(it->first);
	}
}

void Zip::SetFilterFileList(GameType::StrVect& file_name_list)
{
	m_filter_file_list = file_name_list;
}

bool Zip::SetFilter(CommonZipFilePrivate* data)
{
	bool ret = false;
	do
	{
		if (!data || !data->zipFile)
		{
			break;
		}

		// clear existing file list
		data->fileList.clear();

		// UNZ_MAXFILENAMEINZIP + 1 - it is done so in unzLocateFile
		char szCurrentFileName[COMMONZIP_MAXPATH + 1];
		unz_file_info64 fileInfo;

		// go through all files and store position information about the required files
		int err = unzGoToFirstFile64(data->zipFile, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName) - 1);
		while (err == UNZ_OK)
		{
			unz_file_pos posInfo;
			int posErr = unzGetFilePos(data->zipFile, &posInfo);
			if (posErr == UNZ_OK)
			{
				std::string currentFileName = szCurrentFileName;
				// cache info about filtered files only (like 'assets/')
				if (m_filter_file_list.empty() || IsFileInFilter(currentFileName))
				{
					CommonZipEntryInfo entry;
					entry.pos = posInfo;
					entry.uncompressed_size = (uLong)fileInfo.uncompressed_size;

					data->fileList[currentFileName] = entry;
				}
			}
			// next file - also get the information about it
			err = unzGoToNextFile64(data->zipFile, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName) - 1);
		}
		ret = true;

	} while(false);

	return ret;
}

bool Zip::IsFileInFilter(std::string& file_name)
{
	for (StrVect::iterator it = m_filter_file_list.begin(); it != m_filter_file_list.end(); ++it)
	{
		if (file_name.substr(0, it->length()) == (*it))
		{
			return true;
		}
	}
	return false;
}
