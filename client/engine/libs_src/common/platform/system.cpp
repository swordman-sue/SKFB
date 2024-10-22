
#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
#include <unistd.h>
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

void PISleep(unsigned long timems)
{
	#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
		usleep((timems << 10) - (timems << 4) - (timems << 3));
	#elif defined(_WIN32)
		Sleep(timems);
	#endif
}

unsigned long PITime()
{
	#if defined(__unix) || defined(__APPLE__) || defined(__ANDROID__)
		struct timezone tz={0, 0};
		timeval time;
		gettimeofday(&time,&tz);
		return (time.tv_sec*1000+time.tv_usec/1000);
	#elif defined(_WIN32)
		return timeGetTime();
	#endif
}

