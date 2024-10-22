#ifndef COMMON_EVENT_H
#define COMMON_EVENT_H

/*
  need -lrt in linux
*/
#if defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
#include <pthread.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class Event
{
public:
	enum
	{
		Fail = -1,
		Succeed = 0,
		Timeout = 1,
	};

	Event();
	~Event();

	bool Wait();
	//return 0 if wait complete, return 1 if timeout, return -1 if error
	int Wait(unsigned long millsecond);
	bool Signal();
private:
	Event(const Event&);
	Event& operator=(const Event&);

#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
	pthread_cond_t m_Cond;
	pthread_mutex_t m_Mutex;
#endif

#ifdef WIN32
	HANDLE m_Event;
#endif
};


#endif /* COMMON_EVENT_H */
