#ifndef COMMON_SEMAPHORE_H
#define COMMON_SEMAPHORE_H

#if defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
#if defined(__APPLE__)
#include <sys/time.h>
#endif
#include <semaphore.h>
#endif

#ifdef WIN32
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
	Semaphore(int initcount = 0);
	~Semaphore();

	void Down();
	void Up();

	int TryDown(unsigned long timeout_millsec);

private:
	Semaphore(const Semaphore&);
	Semaphore& operator=(const Semaphore&);

#ifdef WIN32
	HANDLE m_Semaphore;
#endif

#if defined(__APPLE__)
	sem_t *m_Semaphore;
	char sem_name[50];
#endif
#if defined(__ANDROID__)
	sem_t m_Semaphore;
#endif

};

#endif
