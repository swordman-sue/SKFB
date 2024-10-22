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
	bool m_bIsBegin;		// �Ƿ��ѿ�ʼ
	DWORD m_dwTick;			// �������ʱ��
	DWORD m_dwTick2;		// �������ʱ��
	DWORD m_dwSumTime;		// ��ʱ��
	DWORD m_dwCharID;		// ��ɫID

};