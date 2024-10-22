#include "httpstruct.h"
#include "zqcommon/strutil.h"

bool PathTool::m_is_crypt_table_init = false;
ZQ::UINT32 PathTool::m_crypt_table[0x500]={0};

const StrHashInfo HttpInfo::GetHashInfo() const
{
	std::string tmp_str = url;
	for(int i=0;i<post_param_num;++i)
	{
		tmp_str+=post_param_names[i];
		tmp_str+=post_param_values[i];
	}
	for(int i=0;i<get_param_num;++i)
	{
		tmp_str+=get_param_names[i];
		tmp_str+=get_param_values[i];
	}
	if (url_id!=0)
	{
		std::string tmp_id_str;
		ZQ::StrUtil::BaseTypeToString(url_id, tmp_id_str);
		tmp_str+=tmp_id_str;
	}

	StrHashInfo hash_info = PathTool::GetPathHashInfo(tmp_str);
	return hash_info;
}

StrHashInfo PathTool::GetPathHashInfo(const std::string& regular_path)
{
	StrHashInfo tmp_hash_info;
	tmp_hash_info.hash0=HashString(0, regular_path);
	tmp_hash_info.hash1=HashString(1, regular_path);
	tmp_hash_info.hash2=HashString(2, regular_path);
	return tmp_hash_info;
}

void PathTool::InitCryptTable()
{
	if(m_is_crypt_table_init)
	{
		return;
	}

	ZQ::UINT32 seed   = 0x00100001;
	ZQ::UINT32 index1 = 0;
	ZQ::UINT32 index2 = 0;
	ZQ::INT32 i;
	ZQ::UINT32 temp1;
	ZQ::UINT32 temp2;

	memset(m_crypt_table, 0, sizeof(m_crypt_table));

	for (index1 = 0; index1 < 0x100; index1++)
	{
		for(index2 = index1, i = 0 ; i < 5; i++, index2 += 0x100)
		{
			seed  = (seed * 125 + 3) % 0x2AAAAB;
			temp1 = (seed & 0xFFFF) << 0x10;

			seed  = (seed * 125 + 3) % 0x2AAAAB;
			temp2 = (seed & 0xFFFF);

			m_crypt_table[index2] = (temp1 | temp2);
		}
	}

	m_is_crypt_table_init = true;
}

ZQ::UINT32 PathTool::HashString( ZQ::UINT32 type, const std::string& str_in )
{
	if(!m_is_crypt_table_init)
	{
		InitCryptTable();
	}

	ZQ::UINT32 seed1 = 0x7FED7FED;
	ZQ::UINT32 seed2 = 0xEEEEEEEE;
	ZQ::UINT32 ch;			/* One key character */

	for(std::string::const_iterator iStr=str_in.begin();iStr!=str_in.end();iStr++)
	{
		////char val=toupper(*iStr);
		char val = *iStr;
		ch=*((unsigned char*)(&val));

		seed1 = m_crypt_table[(type<<8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}
	return seed1;
}

