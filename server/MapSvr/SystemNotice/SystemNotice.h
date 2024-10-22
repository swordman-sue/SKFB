/**\file
充值的逻辑管理器
\author Kevin
\date 2017/03/02
\note
*/

#pragma once
#include "DataDefine.h"
#include "MsgDefine.h"
class Player;

class CSystemNotice
{
public:
	CSystemNotice(WORD wNoticeId, DWORD dwServerId = 0);
	~CSystemNotice();

	// 添加参数
	void AddParam(const char *pszParam);

	// 添加参数
	void AddParam(DWORD dwParam);

	// 设置文本
	void SetText(const char *pszText);

	// 发送消息
	void SendMsg(DWORD dwServerId = 0);
private:
	MSG_S2C_SYSTEM_NOTICE m_msg;
};