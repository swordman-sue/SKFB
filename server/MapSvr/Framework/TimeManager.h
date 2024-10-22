#pragma once
#include "DataDefine.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/Timer.h"
#include <vector>


typedef void (*fnTimeEventCallback)();

class CTimeManager : public Singleton<CTimeManager>
{
public:
	CTimeManager(void);
	~CTimeManager(void);

	void Init();

	void Update();

	// �µ�һ��
	void OnNewDay();

	// ע��ʱ���¼�
	void RegisterTimeEvent(const TimeEventData &tEventData);

	// ע��ص�����
	void RegisterFunc(WORD wEventId, fnTimeEventCallback fnCallbackFunc);
public:

	// ���鸱�����ˢ��
	static void OnEliteFBEnemyRefresh();

protected:


private:
	time_t							m_CurrentTime;
	struct tm*						m_ptm;
	int								m_nDay;
	int								m_nHour;

	std::vector<TimeEventData> m_vTimeDataList;	// ʱ�������б�

	std::map<DWORD, fnTimeEventCallback> m_CallbackFuncList;	// �ص������б�
};
