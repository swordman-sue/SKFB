////////////////////////////////////////////////////////////////////////////////
//	Filename     :Singleton.h
//	Created by   :Kevin
//	Created date :2014/12/17
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once

template <typename T>
class Singleton
{
public:
	~Singleton()
	{

	}

	static T * Instance()
	{
		if (0 == ms_pInstance)
		{
			ms_pInstance = new T;
		}

		return ms_pInstance;
	}

	static VOID DestroyInstance()
	{
		if (ms_pInstance) {		
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}

private:

	static T *		ms_pInstance;
};

template< class T > T * ::Singleton<T>::ms_pInstance = 0;
