
#include "platform/thread/semaphorex.h"
#include "platform/system.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32

static const long INIFINIT_COUNT = 999999;

Semaphore::Semaphore(int initcount)
{
	m_Semaphore = CreateSemaphore(NULL, initcount, INIFINIT_COUNT, NULL); 
}
Semaphore::~Semaphore()
{
	CloseHandle(m_Semaphore);
}

int Semaphore::TryDown(unsigned long timeout_millsec)
{
	DWORD ret = WaitForSingleObject(m_Semaphore, timeout_millsec);
	if ( ret == WAIT_OBJECT_0 )
	{
		return Succeed;
	}
	else if ( ret == WAIT_TIMEOUT )
	{
		return Timeout;
	}
	else
	{
		return Fail;
	}
}

void Semaphore::Down()
{
	WaitForSingleObject(m_Semaphore, INFINITE);
}

void Semaphore::Up()
{
	ReleaseSemaphore(m_Semaphore, 1, NULL);
}

#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)

#include <errno.h>
#if defined(__APPLE__) || defined(__ANDROID__)
#include <fcntl.h>
#endif

Semaphore::Semaphore(int initcount)
{
#if defined(__ANDROID__)
    sem_init(&m_Semaphore, 0, initcount);
#else
	timeval temp_timeval;
	gettimeofday(&temp_timeval, NULL);
	memset(sem_name, 0,50);
	sprintf(sem_name, "Semaphore/%ld/%d", temp_timeval.tv_sec, temp_timeval.tv_usec);
	m_Semaphore = sem_open(sem_name, O_CREAT, 0644, initcount);
#endif
}

Semaphore::~Semaphore()
{
#if defined(__ANDROID__)
	sem_destroy(&m_Semaphore);
#else
	sem_close(m_Semaphore);
	sem_unlink(sem_name);
#endif
}

int Semaphore::TryDown(unsigned long timeout_millsec)
{
#if defined(__APPLE__)
	if (timeout_millsec <= 0)
	{
		if (0 == sem_trywait(m_Semaphore))
		{
			return Succeed;
		}
		if (errno == ETIMEDOUT)
		{
			return Timeout;
		}
	}
	else
	{
		long tmp_time_out = (long)timeout_millsec;
		while (tmp_time_out > 0)
		{
			if (0 == sem_trywait(m_Semaphore))
			{
				return Succeed;
			}
			if (errno == ETIMEDOUT)
			{
				return Timeout;
			}
			else
			{
				PISleep(100);
				tmp_time_out -= 100;
			}
		}
	}
	return Fail;

#else
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	t.tv_sec += timeout_millsec / 1000;
	t.tv_nsec += (timeout_millsec % 1000) * 1000 * 1000;

	if (0 == sem_timedwait(&m_Semaphore, &t))
	{
		return Succeed;
	}
	if (errno == ETIMEDOUT)
	{
		return Timeout;
	}
	else
	{
		return Fail;
	}

#endif
}

void Semaphore::Down()
{
#if defined(__ANDROID__)
    sem_wait(&m_Semaphore);
#else
    sem_wait(m_Semaphore);
#endif
}

void Semaphore::Up()
{
#if defined(__ANDROID__)
    sem_post(&m_Semaphore);
#else
    sem_post(m_Semaphore);
#endif
}

#endif
