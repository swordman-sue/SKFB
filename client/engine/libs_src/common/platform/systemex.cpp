
#include <time.h>

unsigned int GetDayID(int year, int mon, int day)
{
	struct
	{
		unsigned short year;
		unsigned char  mon;
		unsigned char  day;
	} now;

	now.year = (unsigned short)year;
	now.mon = (unsigned char)mon;
	now.day = (unsigned char)day;
	return *(unsigned int*)&now;
}

unsigned int GetDayID()
{
	time_t now_time = time(0);
	struct tm *p_tm = localtime(&now_time);
	return GetDayID((unsigned short)p_tm->tm_year, (unsigned char)p_tm->tm_mon, (unsigned char)p_tm->tm_mday);
}



