#pragma once

#include "../ToolKit/Singleton.h"
#include "../Common/Const.h"
#include "../Common/DataDefine.h"
#include "../Common/DBCDefine.h"


#include "map"


typedef std::map<SystemIDs,BeginEndTime> BETimeMap;
typedef std::map<SystemIDs,BeginEndTime>::iterator BETimeMap_ITR;

typedef std::map<SystemIDs,DWORD> UpdateMap;
typedef std::map<SystemIDs,DWORD>::iterator UpdateMap_ITR;

class SystemTimeManager: public Singleton<SystemTimeManager>
{
public:
	SystemTimeManager();
	~SystemTimeManager();
	bool Init();

	bool IsBegining( SystemTimes* pTime );

	bool IsBegin(SystemIDs dwKey);
	bool IsEnd(SystemIDs dwKey);
	bool IsHave(SystemIDs dwKey);
	void Update(DWORD dwTick);
	bool GetSystemTimes(SystemIDs dwKey,BeginEndTime& Info);
	void StartActivity(SystemIDs dwKey,DWORD dwSecond);
	void DeleteTimes(SystemIDs dwKey);

	void StartActivity(SystemIDs dwKey);
	void EndActivity(SystemIDs dwKey);
	void UpdateActivity(SystemIDs dwKey);

	void SetBeginTimes( SystemTimes* pTime, DWORD &dwTime );
	void SetEndTimes( SystemTimes* pTime, DWORD &dwTime );
	void SetUpdateTimes( SystemTimes* pTime, DWORD &dwTime );
	void CheckInsert();

private:
	BETimeMap m_mapTimes;
	UpdateMap m_mapUpdate;
	

};