#ifndef SDK_LOCATION_H
#define SDK_LOCATION_H
#include "zqcommon/typedef.h"
#include <vector>

class Location
{
public:
	static Location* GetInstance();
	static void DelInstance();
	~Location() {};

	typedef std::vector<ZQ::UINT32>	LocationCallbackList;

private:
	Location():m_inteval(0.0f)
			,m_update_time(2)
	 	 	,m_timeout(10)
			{};

public:
	bool Init();
	bool Start();
	bool Update(float time);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();

public:
	void SetLocationTimeout(const int timeout);
	void StartLocation(ZQ::UINT32 fun_id);
	void StopLocation();
	void CallbackLocation(int code,const char* location_data);

private:
	float m_inteval;
	unsigned int m_update_time;
	int m_timeout;
	LocationCallbackList m_callback_list;
};
#endif
