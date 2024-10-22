#include "platform/wp8/asyncloadhandler.h"


using namespace std;
using namespace PhoneDirect3DXamlAppComponent;
using namespace PhoneDirect3DXamlAppComponent::OpenXLiveHelper;

static MsgInfoList msgInfoList;

AsyncLoadHandler * AsyncLoadHandler::GetInstance()
{
	static AsyncLoadHandler instance;
	return &instance;
}

AsyncLoadHandler::AsyncLoadHandler()
{
	func_list = new std::vector<ZQ::UINT32>;
}
AsyncLoadHandler::~AsyncLoadHandler()
{
	delete func_list;
}

std::vector<ZQ::UINT32> AsyncLoadHandler::Get()
{
	std::vector<ZQ::UINT32> datalist;
	if (!func_list->empty())
	{
		m_mutex_data_list.Lock();
		for (vector<ZQ::UINT32>::const_iterator iter = func_list->begin(); iter != func_list->end(); iter++)
		{
			datalist.push_back(*iter);
		}
		func_list->clear();
		m_mutex_data_list.Unlock();
	}
	return datalist;
}

bool AsyncLoadHandler::Add(ZQ::UINT32 func_id)
{
	bool ret_val = true;
	m_mutex_data_list.Lock();
	func_list->push_back(func_id);
	m_mutex_data_list.Unlock();
	return ret_val;
}



