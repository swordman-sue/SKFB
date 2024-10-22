#pragma once

#include "../ToolKit/Singleton.h"
#include "../Common/DataDefine.h"
#include "../Common/Const.h"


class StopMessage: public Singleton<StopMessage>
{
public:
	StopMessage();
	~StopMessage();
	void Update(DWORD dwTick);
	void Init(){Close();}
	bool IsOpen(){return true == m_bIsBegin ;}
	void Close(){m_bIsBegin =false;}
	void Open(){m_bIsBegin =true;}
	void Reset(DWORD dwCharID, DWORD dwSumTime, DWORD dwTick );
private:
	bool m_bIsBegin;		// 是否已开始
	DWORD m_dwTick;			// 间隔处理时间
	DWORD m_dwTick2;		// 间隔处理时间
	DWORD m_dwSumTime;		// 总时间
	DWORD m_dwCharID;		// 角色ID

};