

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#if (defined (__linux__)) 
#include <semaphore.h>

#elif (defined (__APPLE__))
#include <sys/semaphore.h>

#elif (defined (WP8))
	#include <windows.h>
	
#elif (defined (WIN32))
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

#endif


class Semaphore
{
public:

	enum
	{
		Fail = -1,
		Succeed = 0,
		Timeout = 1,
	};
	Semaphore(int initcount, const char* name=0);
	~Semaphore();

	// apple下不支持semaphore的sem_timedwait，所以timeout失效，非阻塞返回
	int TryDown(unsigned long timeout_millsec);
	void Down();
	void Up();
private:
	Semaphore(const Semaphore&);
	Semaphore& operator=(const Semaphore&);

	bool m_hasname;
	static int SemIdIncrement();
#ifdef _WIN32
	HANDLE m_Semaphore;

#elif (defined (__linux__)) || (defined (__APPLE__))
	sem_t *m_Semaphore;
#endif
};


#endif
