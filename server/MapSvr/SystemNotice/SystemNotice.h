/**\file
��ֵ���߼�������
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

	// ��Ӳ���
	void AddParam(const char *pszParam);

	// ��Ӳ���
	void AddParam(DWORD dwParam);

	// �����ı�
	void SetText(const char *pszText);

	// ������Ϣ
	void SendMsg(DWORD dwServerId = 0);
private:
	MSG_S2C_SYSTEM_NOTICE m_msg;
};