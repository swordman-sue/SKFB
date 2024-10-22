/**\file
红点管理器
\author Kevin
\date 2017/1/6
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CRedPointManager
{
public:
	CRedPointManager();
	~CRedPointManager();

	void Init(Player *pPlayer);

public:
	// 请求红点列表
	void OnNetRedPointList();

public:
	// 红点通知
	void OnRedPointNotify(WORD wSystemId);

	// 是否有红点
	bool IsHaveRedPoint(WORD wSystemId);

	// 打印红点状态 for test
	void PrintRedPointStatus();

private:
	Player *m_pPlayer;
};