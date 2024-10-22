////////////////////////////////////////////////////////////////////////////////
//	Filename     :Pool.h
//	Created by   :dcc
//	Created date :2010/6/13
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////

#pragma once

#if (!defined(_MT))
#   error Please define _MT
#endif

#include "PoolSim.h"

template<class T>
class Pool : public PoolSim<T>
{
public:
	Pool(){}

	virtual void OnDestroy(LPPOOLITEM lpItem)
	{
		lpItem->_Data.OnDestroy();
	}
	virtual void OnCreate(LPPOOLITEM lpItem)
	{
		lpItem->_Data.OnCreate();
	}
	virtual void OnRelease(LPPOOLITEM lpItem)
	{
		lpItem->_Data.OnRelease();
	}
	virtual void OnSelect(LPPOOLITEM lpItem)
	{
		lpItem->_Data.OnSelect();
	}
};

