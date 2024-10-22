#ifndef ASYNC_LOAD_HANDLER_H_
#define ASYNC_LOAD_HANDLER_H_
#include "zqcommon/typedef.h"
#include "msgdata.h"
#include <string>
#include "zqcommon/gamedef.h"
#include "common/platform/thread/mutex.h"

class AsyncLoadHandler
{
public:
	static AsyncLoadHandler * GetInstance();
    bool Add(ZQ::UINT32 func_id);
	std::vector<ZQ::UINT32> Get();
	~AsyncLoadHandler();
private:
	AsyncLoadHandler();
	Mutex m_mutex_data_list;				// 数据增删加锁
	std::vector<ZQ::UINT32>* func_list;
};

#endif
