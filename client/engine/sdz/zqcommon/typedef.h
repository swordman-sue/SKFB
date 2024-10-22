
#include <string>
#include <map>
#include <vector>

namespace GameType
{
	// 基础类型
	typedef unsigned char	BOOL;

	typedef char			INT8;
	typedef unsigned char	UINT8;

	typedef short			INT16;
	typedef unsigned short	UINT16;

	typedef int				INT32;
	typedef unsigned int	UINT32;

	// 只考虑32bit和64bit
#if defined (_WIN32) || defined (__APPLE__) 
	//typedef long long				INT64;
	//typedef unsigned long long	UINT64;
#else
	//typedef long					INT64;
	//typedef unsigned long			UINT64;
#endif

	// 通用结构
	typedef std::map<std::string, std::string>					StrKVMap;
	typedef std::map<std::string, std::string>::const_iterator	StrKVMap_CIter;
	typedef std::map<std::string, std::string>::iterator		StrKVMap_Iter;

	typedef std::vector<std::string>					StrVect;
	typedef std::vector<std::string>::const_iterator	StrVect_CIter;
	typedef std::vector<std::string>::iterator			StrVect_Iter;

	typedef std::map<std::string, unsigned int>						StrUIntKVMap;
	typedef std::map<std::string, unsigned int>::const_iterator		StrUIntKVMap_CIter;
	typedef std::map<std::string, unsigned int>::iterator			StrUIntKVMap_Iter;
}

namespace ZQ
{
	typedef unsigned char	BOOL;

	typedef char			INT8;
	typedef unsigned char	UINT8;

	typedef short			INT16;
	typedef unsigned short	UINT16;

	typedef int				INT32;
	typedef unsigned int	UINT32;

// 只考虑32bit和64bit
#if defined (_WIN32) || defined (__APPLE__) 
	//typedef long long				INT64;
	//typedef unsigned long long	UINT64;
#else
	//typedef long					INT64;
	//typedef unsigned long			UINT64;
#endif
}

