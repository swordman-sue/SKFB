/**\file
   �����������������Ϣ�߼�(���������ڿͻ���)
   \author Kevin
   \date 2014/12/24
   \note 
*/

#pragma once
#include "serversession.h"

class CHttpParser;

//PHP�Ự�� 
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
	// quicksdk_android ��ֵ�ص�
	void OnQuickSDKRechargeCallback_Android(CHttpParser *pHttpParser);

	// quicksdk_ios ��ֵ�ص�
	void OnQuickSDKRechargeCallback_Ios(CHttpParser *pHttpParser);

	// �ƶ�_ios ��ֵ�ص�
	void OnYDRechargeCallback_Ios(CHttpParser *pHttpParser);

	// ͨ�ó�ֵ�ص�
	void OnCommonRechargeCallback(CHttpParser *pHttpParser);

	// YGame��ֵ�ص�
	void OnYGameRechargeCallback(CHttpParser *pHttpParser);

	// YGame cashback coin to user
	void OnYGameCashbackNotify(CHttpParser *pHttpParser);

	// YGame bonus in game
	void OnYGameBonusNotify(CHttpParser *pHttpParser);

public:

	DWORD GetCreateTime() { return m_tCreateTime; };

protected:
	// ����quick_sdk��ֵ�ص�
	void DoQuickRechargeCallback(CHttpParser *pHttpParser, string strMd5Key, string strCallbackKey);

	// ����ͨ�ó�ֵ�ص�
	void DoCommonRechargeCallback(CHttpParser *pHttpParser);

private:
	time_t m_tCreateTime; // ����ʱ��

};
