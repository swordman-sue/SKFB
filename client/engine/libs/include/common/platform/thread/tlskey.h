
#ifndef TLSKEY_H
#define TLSKEY_H

#ifdef WP8
	#include <Windows.h>

#elif defined _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

#elif defined __linux__
	#include <pthread.h>

#endif


class TLSKey
{
public:
	TLSKey();
	~TLSKey();
	
	void *GetKey();
	void SetKey(const void *value);
private:
	#ifdef _WIN32
		DWORD m_key;

	#elif defined __linux__
		pthread_key_t m_key;

	#endif

};

#endif

