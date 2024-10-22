#pragma once

#include "Const.h"
#include "ErrorCode.h"
#include "DataDefine.h"
#include "MessageIdDefine.h"
#include "PacketStruct_Code.h"
#include "DBCDefine.h"
#include <wtypes.h>
#include "MsgDefine.h"

/*--------------------------------------------------------------------------------------------------------------------------*/
//消息注册类
/*--------------------------------------------------------------------------------------------------------------------------*/
class MsgRegister
{
	//typedef std::hash_map<unsigned short,NetMsgBody*> MsgMap;
	typedef std::map<unsigned short, NetMsgBody*> MsgMap;
public:
	static MsgRegister& Instance()
	{
		static MsgRegister s_instance;
		return s_instance;
	}

	NetMsgBody* GetMsgBody(unsigned short sMsgID)
	{
		MsgMap::iterator it = m_MsgMap.find(sMsgID);
		if (it == m_MsgMap.end())
			return NULL;

		return it->second;
	}

private:
	MsgRegister();
	~MsgRegister()
	{
		MsgMap::iterator it;
		for (it = m_MsgMap.begin(); it != m_MsgMap.end(); ++it)
		{
			if (it->second)
				delete it->second;
		}
	}
	void AddMsg(unsigned short sMsgID, NetMsgBody* pMsgBody)
	{
		MsgMap::iterator it = m_MsgMap.find(sMsgID);
		if (it != m_MsgMap.end())
		{
			printf("sMsgID重复添加sMsgID=%d\n", sMsgID);
			if (pMsgBody)
				delete pMsgBody;
			return;
		}
		m_MsgMap.insert(std::make_pair(sMsgID, pMsgBody));
	}
private:
	MsgMap	m_MsgMap;
};
