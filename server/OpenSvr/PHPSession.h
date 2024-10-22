/**\file
   处理第三方过来的消息逻辑(第三方属于客户端)
   \author Kevin
   \date 2014/12/24
   \note 
*/

#pragma once
#include "serversession.h"

class CHttpParser;

//PHP会话类 
class PHPSession : public ServerSession
{
public:
	PHPSession(void);
	virtual ~PHPSession(void);

public:
	virtual	VOID	OnRecv( BYTE *pMsg, WORD wSize );
	virtual VOID	OnDisconnect();
	virtual VOID	OnAccept( DWORD dwNetworkIndex );

public:
	// quicksdk_android 充值回调
	void OnQuickSDKRechargeCallback_Android(CHttpParser *pHttpParser);

	// quicksdk_ios 充值回调
	void OnQuickSDKRechargeCallback_Ios(CHttpParser *pHttpParser);

	// 云顶_ios 充值回调
	void OnYDRechargeCallback_Ios(CHttpParser *pHttpParser);

	// 通用充值回调
	void OnCommonRechargeCallback(CHttpParser *pHttpParser);

	// YGame充值回调
	void OnYGameRechargeCallback(CHttpParser *pHttpParser);

	// YGame cashback coin to user
	void OnYGameCashbackNotify(CHttpParser *pHttpParser);

	// YGame bonus in game
	void OnYGameBonusNotify(CHttpParser *pHttpParser);

public:

	DWORD GetCreateTime() { return m_tCreateTime; };

protected:
	// 处理quick_sdk充值回调
	void DoQuickRechargeCallback(CHttpParser *pHttpParser, string strMd5Key, string strCallbackKey);

	// 处理通用充值回调
	void DoCommonRechargeCallback(CHttpParser *pHttpParser);

private:
	time_t m_tCreateTime; // 创建时间

};
