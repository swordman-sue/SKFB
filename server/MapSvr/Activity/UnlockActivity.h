/**\file
unlock活动逻辑类
\author Kevin
\date 2018/04/28
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CUnlockActivity
{
public:
	CUnlockActivity();
	~CUnlockActivity();

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);


public:
	// 请求活动数据
	void OnNetActivityData();

	// 商城评分完成
	void OnNetShopScoreFinish();

	// 观看视频通知
	void OnNetWatchingVideoNotify();

protected:

	// 发送活动数据
	void SendActivityData();

protected:
	Player *m_pPlayer;
	CActivityManager *m_pActivityMgr;
	UnlockActivityData *m_pActivityData;
};