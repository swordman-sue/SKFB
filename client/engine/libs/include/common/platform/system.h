
#pragma once

#ifndef SYSTEM_H
#define SYSTEM_H

void PISleep(unsigned long timems);

unsigned long PITime();

// ��ȡÿһ��Ĳ�ͬ��ʾ
unsigned int GetDayID();
		
unsigned int GetDayID(int year, int mon, int day);

#endif
