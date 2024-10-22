#ifndef SdkHandle_HANDLE_H
#define SdkHandle_HANDLE_H
#include "zqcommon/typedef.h"
#include <vector>
class SdkHandle
{
public:
	static SdkHandle* GetInstance();
	static void DelInstance();
	~SdkHandle() {};

	typedef std::vector<ZQ::UINT32>	SdkHandleCallbackList;

private:
	SdkHandle():m_inteval(0.0f)
			,m_update_time(1)
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
	

private:
	float m_inteval;
	unsigned int m_update_time;
	SdkHandleCallbackList m_callback_list;
};
#endif
