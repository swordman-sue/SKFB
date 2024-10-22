#ifndef COMMON_MUTEX_H
#define COMMON_MUTEX_H

#if defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
#include <pthread.h>
#endif 

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400 
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class Mutex
{
 public:
	Mutex();
	~Mutex();

	bool Lock();
	bool TryLock();
	bool Unlock();
 private:
	 Mutex(const Mutex&);
	 Mutex& operator=(const Mutex&);

#if defined(__linux__) || defined(__APPLE__)
	pthread_mutex_t m_Lock;
#endif

#ifdef WIN32
	CRITICAL_SECTION m_CriticalSection;
#endif
};

#endif /* COMMON_MUTEX_H */
