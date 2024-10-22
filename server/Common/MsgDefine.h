#pragma once

#include "Const.h"
#include "ErrorCode.h"
#include "DataDefine.h"
#include "MessageIdDefine.h"
#include "PacketStruct_Code.h"
#include "DBCDefine.h"
#include <wtypes.h>

/*--------------------------------------------------------------------------------------------------------------------------*/	
// 1) �ͻ��˺�GateSvr֮����Ϣ����
/*--------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------*/	
// 2) �ͻ��˺�MapSvr֮����Ϣ����
/*--------------------------------------------------------------------------------------------------------------------------*/


// ��Ϸ������ʾ
Declare_Msg_Begin(S2C_GAME_ERROR_NOTIFY, MSG_S2C_GAME_ERROR_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wErrorCode)
Declare_DecodeMsg_UnsignedShort(m_wFromMsgId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wErrorCode)
Declare_EncodeMsg_UnsignedShort(m_wFromMsgId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wErrorCode)		// ������
Declare_Msg_Member(WORD, m_wFromMsgId)		// ����������ϢID
Declare_Msg_End

// �ʺŵ�¼
Declare_Msg_Begin(C2S_LOGIN_ACCOUNT_REQ, MSG_C2S_LOGIN_ACCOUNT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byTerminalType)
Declare_DecodeMsg_UnsignedInt(m_dwCPChannelID)
Declare_DecodeMsg_UnsignedInt(m_dwSDKChannelID)
Declare_DecodeMsg_UnsignedChar(m_byLoginType)
Declare_DecodeMsg_UnsignedShort(m_wCode)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szPassword, sizeof(m_szPassword)-1)
Declare_DecodeMsg_String(m_szIP, sizeof(m_szIP)-1)
Declare_DecodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byTerminalType)
Declare_EncodeMsg_UnsignedInt(m_dwCPChannelID)
Declare_EncodeMsg_UnsignedInt(m_dwSDKChannelID)
Declare_EncodeMsg_UnsignedChar(m_byLoginType)
Declare_EncodeMsg_UnsignedShort(m_wCode)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szPassword, sizeof(m_szPassword))
Declare_EncodeMsg_String(m_szIP, sizeof(m_szIP))
Declare_EncodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byTerminalType)						// �ն�����(1=android 2=ios 3=����)
Declare_Msg_Member(DWORD, m_dwCPChannelID)	// CP����ID
Declare_Msg_Member(DWORD, m_dwSDKChannelID)	// SDK����ID
Declare_Msg_Member(BYTE, m_byLoginType)		// ��¼����(0=����,1=�û���ע��,2=�û�����¼,3=�ֻ��ŵ�¼,4=�ֻ���ע��)
Declare_Msg_Member(WORD, m_wCode)	// ��֤��
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// �ʺ�
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN)	// ����
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// �豸ID deviceid
Declare_Msg_End


// �ʺŵ�¼��Ӧ
Declare_Msg_Begin(S2C_LOGIN_ACCOUNT_RESP, MSG_S2C_LOGIN_ACCOUNT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLastLoginServerNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedInt(m_dwVersion)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_EncodeMsg_UnsignedChar(m_byIsBindMobile)
Declare_EncodeMsg_UnsignedShort(m_wLastLoginServerNum)
for (WORD i = 0; i < m_wLastLoginServerNum; ++i)
{
	LastLoginServerData &tSeverData = m_LastLoginServerList[i];
	Declare_EncodeMsg_UnsignedInt(tSeverData.dwServerId);
	Declare_EncodeMsg_UnsignedInt(tSeverData.dwHeadIcon);
	Declare_EncodeMsg_UnsignedShort(tSeverData.wLevel);
	Declare_EncodeMsg_String(tSeverData.szRoleName, sizeof(tSeverData.szRoleName));
}
Declare_EncodeMsg_Memory(m_LastLoginServerList, MAX_LOGIN_SERVER_LIST*sizeof(LastLoginServerData));
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(DWORD, m_dwVersion)	// �汾
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// �ʺ�
Declare_Msg_Member(BYTE, m_byIsBindMobile)	// �Ƿ��Ѱ��ֻ���
Declare_Msg_Member(WORD, m_wLastLoginServerNum)
Declare_Msg_Member_Array(LastLoginServerData, m_LastLoginServerList, MAX_LOGIN_SERVER_LIST)	// �����¼�������б�
Declare_Msg_End


// �ʺ�ע������
Declare_Msg_Begin(C2S_ACCOUNT_REGISTER_REQ, MSG_C2S_ACCOUNT_REGISTER_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_UnsignedChar(m_byType);
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szPassword, sizeof(m_szPassword)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_EncodeMsg_UnsignedChar(m_byType);
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szPassword, sizeof(m_szPassword))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member_Array(char, m_szCode, 32) // ��֤��
Declare_Msg_Member(BYTE, m_byType)	// ����(1�û���ע��,2=�ֻ���ע��)
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN ) // �ʺ�
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // ����
Declare_Msg_End


// �ʺ�ע����Ӧ
Declare_Msg_Begin(S2C_ACCOUNT_REGISTER_RESP, MSG_S2C_ACCOUNT_REGISTER_RESP)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szPassword, sizeof(m_szPassword)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szPassword, sizeof(m_szPassword))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member_Array(char, m_szCode, 32) // ��֤��
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // �ʺ�
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // ����
Declare_Msg_End


// �ʺ������޸�����
Declare_Msg_Begin(C2S_ACCOUNT_PASSWORD_CHANGE_REQ, MSG_C2S_ACCOUNT_PASSWORD_CHANGE_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szPassword, sizeof(m_szPassword)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szPassword, sizeof(m_szPassword))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member_Array(char, m_szCode, 32) // ��֤��
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // �ʺ�
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // ����
Declare_Msg_End


// �ʺ������޸���Ӧ
Declare_Msg_Begin(S2C_ACCOUNT_PASSWORD_CHANGE_RESP, MSG_S2C_ACCOUNT_PASSWORD_CHANGE_RESP)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szPassword, sizeof(m_szPassword)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szPassword, sizeof(m_szPassword))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member_Array(char, m_szCode, 32) // ��֤��
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // �ʺ�
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // ����
Declare_Msg_End


// ��ȡ����
Declare_Msg_Begin(C2S_TAKE_ACTIVITY_GIFT_REQ, MSG_C2S_TAKE_ACTIVITY_GIFT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szCode, MAX_ACTIVATE_CODE_LEN)	// ������
Declare_Msg_End

// ��ȡ������Ӧ
Declare_Msg_Begin(S2C_TAKE_ACTIVITY_GIFT_RESP, MSG_S2C_TAKE_ACTIVITY_GIFT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGiftId)		// ��õ����ID
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_ACTIVITY_NOTICE_REQ, MSG_C2S_ACTIVITY_NOTICE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_ACTIVITY_NOTICE_RESP, MSG_S2C_ACTIVITY_NOTICE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNoticeId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNoticeId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNoticeId)		// ����ID
Declare_Msg_End


// ��֤������
Declare_Msg_Begin(C2S_AUTHENTICATION_CODE_REQ, MSG_C2S_AUTHENTICATION_CODE_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_UnsignedChar(m_byType);
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedChar(m_byType);
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(BYTE, m_byType)		// 1=ע��,2=�޸�����
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// �ֻ���
Declare_Msg_End

// ��֤����Ӧ
Declare_Msg_Begin(S2C_AUTHENTICATION_CODE_RESP, MSG_S2C_AUTHENTICATION_CODE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ֻ��Ű�����
Declare_Msg_Begin(C2S_MOBILE_BIND_REQ, MSG_C2S_MOBILE_BIND_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCode);
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCode);
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCode)		// ��֤��
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // �ʺ�
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// �ֻ���
Declare_Msg_End

// �ֻ��Ű���Ӧ
Declare_Msg_Begin(S2C_MOBILE_BIND_RESP, MSG_S2C_MOBILE_BIND_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


 // SDK��¼����
Declare_Msg_Begin(C2S_SDK_LOGIN_REQ, MSG_C2S_SDK_LOGIN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byTerminalType)
Declare_DecodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_DecodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szToken, sizeof(m_szToken)-1)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_String(m_szIP, sizeof(m_szIP)-1)
Declare_DecodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byTerminalType)
Declare_EncodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_EncodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_String(m_szIP, sizeof(m_szIP))
Declare_EncodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(DWORD, m_dwVersion)							// �汾
Declare_Msg_Member(BYTE, m_byTerminalType)						// �ն�����(1=android 2=ios 3=����)
Declare_Msg_Member(DWORD, m_dwCPChannelType)					// CP��������
Declare_Msg_Member(DWORD, m_dwSDKChannelType)					// SDK��������
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// �ʺ�
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)							// ������
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)					// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// �豸ID deviceid
Declare_Msg_End


// SDK�ʺŵ�¼��Ӧ
Declare_Msg_Begin(S2C_SDK_LOGIN_RESP, MSG_S2C_SDK_LOGIN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLastLoginServerNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_EncodeMsg_UnsignedChar(m_byIsBindMobile)
Declare_EncodeMsg_UnsignedShort(m_wLastLoginServerNum)
for (WORD i = 0; i < m_wLastLoginServerNum; ++i)
{
	LastLoginServerData &tSeverData = m_LastLoginServerList[i];
	Declare_EncodeMsg_UnsignedInt(tSeverData.dwServerId);
	Declare_EncodeMsg_UnsignedInt(tSeverData.dwHeadIcon);
	Declare_EncodeMsg_UnsignedShort(tSeverData.wLevel);
	Declare_EncodeMsg_String(tSeverData.szRoleName, sizeof(tSeverData.szRoleName));
}
Declare_EncodeMsg_Memory(m_LastLoginServerList, MAX_LOGIN_SERVER_LIST*sizeof(LastLoginServerData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// �ʺ�
Declare_Msg_Member(BYTE, m_byIsBindMobile)	// �Ƿ��Ѱ��ֻ���
Declare_Msg_Member(WORD, m_wLastLoginServerNum)
Declare_Msg_Member_Array(LastLoginServerData, m_LastLoginServerList, MAX_LOGIN_SERVER_LIST)	// �����¼�������б�
Declare_Msg_End

// ����û���Ϣ����
Declare_Msg_Begin(M2O_CHECK_USER_INFO_REQ, MSG_M2O_CHECK_USER_INFO_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_UnsignedShort(m_wLoginType)
Declare_DecodeMsg_UnsignedChar(m_byTerminalType)
Declare_DecodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_DecodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_DecodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_DecodeMsg_String(m_szIP, sizeof(m_szIP)-1)
Declare_DecodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedShort(m_wLoginType)
Declare_EncodeMsg_UnsignedChar(m_byTerminalType)
Declare_EncodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_EncodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_EncodeMsg_String(m_szIP, sizeof(m_szIP))
Declare_EncodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLoginType)	// ��¼����(1=quick_sdk, 2=�ƶ�ios, 3=��ͨ)
Declare_Msg_Member(BYTE, m_byTerminalType)	// �ն�����(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwCPChannelType)	// CP��������
Declare_Msg_Member(DWORD, m_dwSDKChannelType)	// SDK��������
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// �ʺ�
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// �豸ID deviceid
Declare_Msg_End

// SDK��¼��Ӧ
Declare_Msg_Begin(O2M_CHECK_USER_INFO_RESP, MSG_O2M_CHECK_USER_INFO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Int(m_nResult)
//Declare_DecodeMsg_UnsignedShort(m_wLoginType)
Declare_DecodeMsg_UnsignedChar(m_byTerminalType)
Declare_DecodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_DecodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_DecodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_DecodeMsg_String(m_szIP, sizeof(m_szIP)-1)
Declare_DecodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Int(m_nResult)
//Declare_EncodeMsg_UnsignedShort(m_wLoginType)
Declare_EncodeMsg_UnsignedChar(m_byTerminalType)
Declare_EncodeMsg_UnsignedInt(m_dwCPChannelType)
Declare_EncodeMsg_UnsignedInt(m_dwSDKChannelType)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_EncodeMsg_String(m_szIP, sizeof(m_szIP))
Declare_EncodeMsg_String(m_szDeviceId, sizeof(m_szDeviceId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nResult)	// ���
Declare_Msg_Member(WORD, m_wLoginType)	// ��¼����(1=quick_sdk, 2=�ƶ�ios, 3=��ͨ)
Declare_Msg_Member(BYTE, m_byTerminalType)	// �ն�����(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwCPChannelType)	// CP��������
Declare_Msg_Member(DWORD, m_dwSDKChannelType)	// SDK��������
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// �ʺ�
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// �豸ID deviceid
Declare_Msg_End


// SDK����������
Declare_Msg_Begin(C2S_SDK_ORDER_ID_REQ, MSG_C2S_SDK_ORDER_ID_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRechargeType)
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRechargeType);
Declare_EncodeMsg_UnsignedShort(m_wRechargeId);
Declare_EncodeMsg_UnsignedInt(m_dwContext);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRechargeType)	// ��ֵ����(1=��ͨ��ֵ,2=�¿�)
Declare_Msg_Member(WORD, m_wRechargeId)		// ��ֵid(�¿�ID,��ֵId)
Declare_Msg_Member(DWORD, m_dwContext)		// ������
Declare_Msg_End


// SDK��������Ӧ
Declare_Msg_Begin(S2C_SDK_ORDER_ID_RESP, MSG_S2C_SDK_ORDER_ID_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szOrderId, sizeof(m_szOrderId)-1)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)
Declare_DecodeMsg_UnsignedChar(m_byRechargeType)
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szOrderId, sizeof(m_szOrderId))
Declare_EncodeMsg_String(m_szSign, sizeof(m_szSign))
Declare_EncodeMsg_UnsignedChar(m_byRechargeType);
Declare_EncodeMsg_UnsignedShort(m_wRechargeId);
Declare_EncodeMsg_UnsignedInt(m_dwContext);
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szOrderId, MAX_ORDER_ID_LEN)		// ������
Declare_Msg_Member_Array(char, m_szSign, MAX_ORDER_SIGN_LEN)		// ǩ��
Declare_Msg_Member(BYTE, m_byRechargeType)	// ��ֵ����(1=��ͨ��ֵ,2=�¿�)
Declare_Msg_Member(WORD, m_wRechargeId)		// ��ֵid(�¿�ID,��ֵId)
Declare_Msg_Member(DWORD, m_dwContext)		// ������
Declare_Msg_End


// SDK����������
Declare_Msg_Begin(M2O_SDK_ORDER_ID_REQ, MSG_M2O_SDK_ORDER_ID_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nAmount)
Declare_DecodeMsg_String(m_szTitle, sizeof(m_szTitle)-1)
Declare_DecodeMsg_String(m_szDesc, sizeof(m_szDesc)-1)
Declare_DecodeMsg_String(m_szExtInfo, sizeof(m_szExtInfo)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nAmount)
Declare_EncodeMsg_String(m_szTitle, sizeof(m_szTitle))
Declare_EncodeMsg_String(m_szDesc, sizeof(m_szDesc))
Declare_EncodeMsg_String(m_szExtInfo, sizeof(m_szExtInfo))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nAmount)							// ���׽��	��λ���֣����֣�����ң�����������
Declare_Msg_Member_Array(char, m_szTitle, BUFF_LEN_32)		// ��Ʒ�ı���	������ë����  �45λ
Declare_Msg_Member_Array(char, m_szDesc, BUFF_LEN_128)		// ��������	������Ʒ���׿Ƽ�̨��� �100λ
Declare_Msg_Member_Array(char, m_szExtInfo, BUFF_LEN_64)	// CP͸������	CP��չ�������첽֪ͨʱ��͸����CP�����������64λ, ����Ϊ��
Declare_Msg_End


// SDK��������Ӧ
Declare_Msg_Begin(O2M_SDK_ORDER_ID_RESP, MSG_O2M_SDK_ORDER_ID_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szAccessKey, sizeof(m_szAccessKey)-1)
Declare_DecodeMsg_String(m_szOrderNumber, sizeof(m_szOrderNumber)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szAccessKey, sizeof(m_szAccessKey))
Declare_EncodeMsg_String(m_szOrderNumber, sizeof(m_szOrderNumber))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szAccessKey, BUFF_LEN_64)		// vivoSDK��Ҫ�Ĳ��� vivoSDKʹ��
Declare_Msg_Member_Array(char, m_szOrderNumber, BUFF_LEN_64)	// ������ˮ�� vivo������
Declare_Msg_End


// CP����������
Declare_Msg_Begin(O2M_CP_ORDER_ID_REQ, MSG_O2M_CP_ORDER_ID_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// �����ŵĽ�ɫId
Declare_Msg_End


// CP��������Ӧ
Declare_Msg_Begin(M2O_CP_ORDER_ID_RESP, MSG_M2O_CP_ORDER_ID_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedInt(m_dwOrderId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedInt(m_dwOrderId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// �����ŵĽ�ɫId
Declare_Msg_Member(DWORD, m_dwOrderId)	// ������ID
Declare_Msg_End



// ��¼����
Declare_Msg_Begin(C2S_LOGIN_REQ, MSG_C2S_LOGIN_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_UnsignedShort(m_wLoginType)
Declare_DecodeMsg_UnsignedChar(m_byPlatformType)
Declare_DecodeMsg_UnsignedInt(m_dwChannelType)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szToken, sizeof(m_szToken)-1)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedShort(m_wLoginType)
Declare_EncodeMsg_UnsignedChar(m_byPlatformType)
Declare_EncodeMsg_UnsignedInt(m_dwChannelType)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(WORD, m_wLoginType)		// ��¼����(1=quick_sdk, 2=�ƶ�ios, 3=��ͨ)
Declare_Msg_Member(BYTE, m_byPlatformType)	// ƽ̨����(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwChannelType)	// ��������
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// �ʺ�
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)	// ������
Declare_Msg_End

// ��¼��Ӧ
Declare_Msg_Begin(S2C_LOGIN_RESP, MSG_S2C_LOGIN_RESP)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_UnsignedShort(m_wLoginType)
Declare_DecodeMsg_UnsignedChar(m_byPlatformType)
Declare_DecodeMsg_UnsignedInt(m_dwChannelType)
Declare_DecodeMsg_String(m_szAccount, sizeof(m_szAccount)-1)
Declare_DecodeMsg_String(m_szToken, sizeof(m_szToken)-1)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedShort(m_wLoginType)
Declare_EncodeMsg_UnsignedChar(m_byPlatformType)
Declare_EncodeMsg_UnsignedInt(m_dwChannelType)
Declare_EncodeMsg_String(m_szAccount, sizeof(m_szAccount))
Declare_EncodeMsg_String(m_szToken, sizeof(m_szToken))
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(WORD, m_wLoginType)	// ��¼����
Declare_Msg_Member(BYTE, m_byPlatformType)	// ƽ̨����(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwChannelType)	// ��������
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// �ʺ�
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)	// ������
Declare_Msg_End



// ���������������
Declare_Msg_Begin(O2M_ONLINE_PLAYER_NUM_REQ, MSG_O2M_ONLINE_PLAYER_NUM_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �������������Ӧ
Declare_Msg_Begin(M2O_ONLINE_PLAYER_NUM_RESP, MSG_M2O_ONLINE_PLAYER_NUM_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerNum)
for (WORD i = 0; i < m_wServerNum; ++i)
{
	Declare_DecodeMsg_UnsignedShort(m_ServerIdList[i]);
	Declare_DecodeMsg_UnsignedShort(m_PlayerNumList[i]);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerNum)
for (WORD i = 0; i < m_wServerNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_ServerIdList[i]);
	Declare_EncodeMsg_UnsignedShort(m_PlayerNumList[i]);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerNum)	// ����������
Declare_Msg_Member_Array(WORD, m_ServerIdList, MAX_SERVER_NUM)	// ������ID�б�
Declare_Msg_Member_Array(WORD, m_PlayerNumList, MAX_SERVER_NUM)	// ��������б�
Declare_Msg_End


// ��ֵ�ɹ��ϱ�����
Declare_Msg_Begin(C2S_RECHARGE_SUCCESS_REPORT_REQ, MSG_C2S_RECHARGE_SUCCESS_REPORT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szIP, sizeof(m_szIP)-1)
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_String(m_szOrderId, sizeof(m_szOrderId)-1)
Declare_DecodeMsg_UnsignedInt(m_dwChannel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szIP, sizeof(m_szIP))
Declare_EncodeMsg_UnsignedShort(m_wRechargeId)
Declare_EncodeMsg_String(m_szOrderId, sizeof(m_szOrderId))
Declare_EncodeMsg_UnsignedInt(m_dwChannel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szIP, BUFF_LEN_32)	// ip  ��  string  ��ҳ�ֵʱ�� IP ��ַ
Declare_Msg_Member(WORD, m_wRechargeId)	// iap_id  ��  string  ��Ʒ����
Declare_Msg_Member_Array(char, m_szOrderId, BUFF_LEN_64)// order_id  ��  string  �����ţ�֧��ƽ̨�ṩ�Ķ����ţ�
Declare_Msg_Member(DWORD, m_dwChannel)	// channel  ��  int  ��������(Ҫ��ͻ�����YundingKeyData.getChannel()��ȡ���յ����������������)
Declare_Msg_End


// ��ֵ�ɹ��ϱ���Ӧ
Declare_Msg_Begin(S2C_RECHARGE_SUCCESS_REPORT_RESP, MSG_S2C_RECHARGE_SUCCESS_REPORT_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ֵ�ɹ��ϱ�(����)
Declare_Msg_Begin(M2O_RECHARGE_SUCCESS_REPORT_NOTIFY, MSG_M2O_RECHARGE_SUCCESS_REPORT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RechargeSuccessReportData, m_ReportData)
Declare_Msg_End

// mycar��ֵ�ɹ��ϱ�
Declare_Msg_Begin(M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY, MSG_M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(MyCardRechargeSuccessReportData, m_ReportData)
Declare_Msg_End



// ����ͳ��֪ͨ
Declare_Msg_Begin(M2O_STATISTICS_NOTIFY, MSG_M2O_STATISTICS_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wType);
Declare_DecodeMsg_String(m_szContext, sizeof(m_szContext))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wType);
Declare_EncodeMsg_String(m_szContext, sizeof(m_szContext))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wType)
Declare_Msg_Member_Array(char, m_szContext, BUFF_LEN_512)		// ֪ͨ����
Declare_Msg_End


// ��ֵ��������
Declare_Msg_Begin(O2M_RECHARGE_SEND_GOODS_REQ, MSG_O2M_RECHARGE_SEND_GOODS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_CallbackData.byIsTest)
Declare_DecodeMsg_UnsignedInt(m_CallbackData.dwChannel)
//Declare_DecodeMsg_UnsignedInt(m_CallbackData.dwChannelUid)
Declare_DecodeMsg_UnsignedInt(m_CallbackData.dwCPOrderId)
Declare_DecodeMsg_String(m_CallbackData.szSDKOrderId, sizeof(m_CallbackData.szSDKOrderId))
Declare_DecodeMsg_String(m_CallbackData.szPayTime, sizeof(m_CallbackData.szPayTime))
Declare_DecodeMsg_String(m_CallbackData.szChannelUid, sizeof(m_CallbackData.szChannelUid))
Declare_DecodeMsg_UnsignedInt(m_CallbackData.nAmount)
Declare_DecodeMsg_UnsignedChar(m_CallbackData.byStatus)
Declare_DecodeMsg_UnsignedInt(m_CallbackData.dwRoleId)
Declare_DecodeMsg_UnsignedShort(m_CallbackData.wRechargeId)
Declare_DecodeMsg_UnsignedShort(m_CallbackData.wRechargeType)
Declare_DecodeMsg_UnsignedShort(m_CallbackData.wGoodsId)
Declare_DecodeMsg_String(m_CallbackData.szIP, sizeof(m_CallbackData.szIP))
Declare_DecodeMsg_UnsignedInt(m_CallbackData.dwYDChannelId)
Declare_DecodeMsg_UnsignedShort(m_CallbackData.wCurrencyType)
Declare_DecodeMsg_UnsignedChar(m_CallbackData.byIsMyCard)
Declare_DecodeMsg_UnsignedChar(m_CallbackData.byIsCheckRechargeStrId)
Declare_DecodeMsg_String(m_CallbackData.szRechargeStrId, sizeof(m_CallbackData.szRechargeStrId))
Declare_DecodeMsg_UnsignedChar(m_CallbackData.byIsNotCheck)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_CallbackData.byIsTest)
Declare_EncodeMsg_UnsignedInt(m_CallbackData.dwChannel)
//Declare_EncodeMsg_UnsignedInt(m_CallbackData.dwChannelUid)
Declare_EncodeMsg_UnsignedInt(m_CallbackData.dwCPOrderId)
Declare_EncodeMsg_String(m_CallbackData.szSDKOrderId, sizeof(m_CallbackData.szSDKOrderId))
Declare_EncodeMsg_String(m_CallbackData.szPayTime, sizeof(m_CallbackData.szPayTime))
Declare_EncodeMsg_String(m_CallbackData.szChannelUid, sizeof(m_CallbackData.szChannelUid))
Declare_EncodeMsg_UnsignedInt(m_CallbackData.nAmount)
Declare_EncodeMsg_UnsignedChar(m_CallbackData.byStatus)
Declare_EncodeMsg_UnsignedInt(m_CallbackData.dwRoleId)
Declare_EncodeMsg_UnsignedShort(m_CallbackData.wRechargeId)
Declare_EncodeMsg_UnsignedShort(m_CallbackData.wRechargeType)
Declare_EncodeMsg_UnsignedShort(m_CallbackData.wGoodsId)
Declare_EncodeMsg_String(m_CallbackData.szIP, sizeof(m_CallbackData.szIP))
Declare_EncodeMsg_UnsignedInt(m_CallbackData.dwYDChannelId)
Declare_EncodeMsg_UnsignedShort(m_CallbackData.wCurrencyType)
Declare_EncodeMsg_UnsignedChar(m_CallbackData.byIsMyCard)
Declare_EncodeMsg_UnsignedChar(m_CallbackData.byIsCheckRechargeStrId)
Declare_EncodeMsg_String(m_CallbackData.szRechargeStrId, sizeof(m_CallbackData.szRechargeStrId))
Declare_EncodeMsg_UnsignedChar(m_CallbackData.byIsNotCheck)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RechargeCallbackData, m_CallbackData)		// ��ֵ�ص�����
Declare_Msg_End


// ��ֵ������Ӧ
Declare_Msg_Begin(M2O_RECHARGE_SEND_GOODS_RESP, MSG_M2O_RECHARGE_SEND_GOODS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szOrderId, sizeof(m_szOrderId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szOrderId, sizeof(m_szOrderId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szOrderId, MAX_ORDER_ID_LEN)		// ������
Declare_Msg_End



// YGame��ֵ֪ͨ����
Declare_Msg_Begin(O2M_YGAME_RECHARGE_SEND_GOODS_REQ, MSG_O2M_YGAME_RECHARGE_SEND_GOODS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedInt(m_nAmount)
Declare_DecodeMsg_UnsignedInt(m_nDiamond)
Declare_DecodeMsg_String(m_szPaymentId, sizeof(m_szPaymentId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedInt(m_nAmount)
Declare_EncodeMsg_UnsignedInt(m_nDiamond)
Declare_EncodeMsg_String(m_szPaymentId, sizeof(m_szPaymentId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)		// ������ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// ��ɫId
Declare_Msg_Member(int, m_nAmount)			// ���
Declare_Msg_Member(int, m_nDiamond)			// ��ʯ
Declare_Msg_Member_Array(char, m_szPaymentId, MAX_YGAME_PAYMENT_ID_LEN)	// ֧��ID
Declare_Msg_End

// YGame��ֵ֪ͨ��Ӧ
Declare_Msg_Begin(M2O_YGAME_RECHARGE_SEND_GOODS_RESP, MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// YGame����֪ͨ����
Declare_Msg_Begin(O2M_YGAME_CASHBACK_NOTIFY_REQ, MSG_O2M_YGAME_CASHBACK_NOTIFY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedInt(m_nYcoin)
Declare_DecodeMsg_UnsignedInt(m_nDiamond)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedInt(m_nYcoin)
Declare_EncodeMsg_UnsignedInt(m_nDiamond)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)		// ������ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// ��ɫId
Declare_Msg_Member(int, m_nYcoin)			// ���
Declare_Msg_Member(int, m_nDiamond)			// ��ʯ
Declare_Msg_End

// YGame����֪ͨ��Ӧ
Declare_Msg_Begin(M2O_YGAME_CASHBACK_NOTIFY_RESP, MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// YGame����֪ͨ����
Declare_Msg_Begin(O2M_YGAME_BONUS_NOTIFY_REQ, MSG_O2M_YGAME_BONUS_NOTIFY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedInt(m_dwDiamond)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedInt(m_dwDiamond)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)		// ������ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// ��ɫId
Declare_Msg_Member(DWORD, m_dwDiamond)		// ��������ʯ
Declare_Msg_End

// YGame����֪ͨ��Ӧ
Declare_Msg_Begin(M2O_YGAME_BONUS_NOTIFY_RESP, MSG_M2O_YGAME_BONUS_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �µ�һ��
Declare_Msg_Begin(S2C_NEW_DAY_NOTIFY, MSG_S2C_NEW_DAY_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ѡ�����������
Declare_Msg_Begin(C2S_LOGIN_SELECT_SERVER_REQ, MSG_C2S_LOGIN_SELECT_SERVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSelectZoneId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSelectZoneId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSelectZoneId)		// ѡ��ķ���ID
Declare_Msg_End

// ѡ���������Ӧ
Declare_Msg_Begin(S2C_LOGIN_SELECT_SERVER_RESP, MSG_S2C_LOGIN_SELECT_SERVER_RESP)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedChar(m_byHaveRole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedChar(m_byHaveRole)
Declare_EncodeMsg_Function_End
	Declare_Msg_Member(BYTE, m_byHaveRole)		// �Ƿ��н�ɫ
Declare_Msg_End

// ������ɫ����
Declare_Msg_Begin(C2S_LOGIN_CREATE_ROLE_REQ, MSG_C2S_LOGIN_CREATE_ROLE_REQ)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_String(m_szRoleName, sizeof(m_szRoleName)-1)
	Declare_DecodeMsg_UnsignedChar(m_bySex)
	Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName))
	Declare_EncodeMsg_UnsignedChar(m_bySex)
	Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
	Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// ��ɫ����
	Declare_Msg_Member(BYTE, m_bySex)		// ��ɫ�Ա�
	Declare_Msg_Member(BYTE, m_byType)		// ��������(0=Ĭ��,1=��������)
Declare_Msg_End



// ��ʼ��Ϸ����
Declare_Msg_Begin(C2S_START_GAME_REQ, MSG_C2S_START_GAME_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ʼ��Ϸ��Ӧ
Declare_Msg_Begin(S2C_START_GAME_RESP, MSG_S2C_START_GAME_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwClientVersion)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwClientVersion)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwClientVersion)	// �ͻ��˰汾
Declare_Msg_End


// ������ɫ��Ӧ
Declare_Msg_Begin(S2C_LOGIN_CREATE_ROLE_RESP, MSG_S2C_LOGIN_CREATE_ROLE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ɫ�����޸�����
Declare_Msg_Begin(C2S_ROLE_NAME_MODIFY_REQ, MSG_C2S_ROLE_NAME_MODIFY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szRoleName, sizeof(m_szRoleName)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// ��ɫ����
Declare_Msg_End

// ��ɫ�����޸���Ӧ
Declare_Msg_Begin(S2C_ROLE_NAME_MODIFY_RESP, MSG_S2C_ROLE_NAME_MODIFY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szRoleName, sizeof(m_szRoleName)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byResult)
Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byResult)	// ���
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// ��ɫ����
Declare_Msg_End

// ϵͳ����
Declare_Msg_Begin(S2C_SYSTEM_NOTICE, MSG_S2C_SYSTEM_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedShort(m_wNoticeId)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1)
Declare_DecodeMsg_UnsignedShort(m_wParamListNum)
for (WORD i = 0; i < m_wParamListNum; ++i)
{
	Declare_DecodeMsg_String(m_ParamList[i].szText, sizeof(m_ParamList[i].szText)-1)
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedShort(m_wNoticeId)
Declare_EncodeMsg_String(m_szText, sizeof(m_szText))
Declare_EncodeMsg_UnsignedShort(m_wParamListNum)
for (WORD i = 0; i < m_wParamListNum; ++i)
{
	Declare_EncodeMsg_String(m_ParamList[i].szText, sizeof(m_ParamList[i].szText))
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)	// �㲥�ķ�����ID(0=��ʾ���з�����)
Declare_Msg_Member(WORD, m_wNoticeId)	// ����ID
Declare_Msg_Member_Array(char, m_szText, MAX_SYSTEM_NOTICE_LEN + 1)	// ��������
Declare_Msg_Member(WORD, m_wParamListNum)	// �����б�����
Declare_Msg_Member_Array(SystemNoticeParam, m_ParamList, MAX_SYSTEM_NOTICE_PARAM_NUM)	// ��������
Declare_Msg_End



// ������Ϸ֪ͨ
Declare_Msg_Begin(S2C_LOGIN_ENTER_GAME_NOTIFY, MSG_S2C_LOGIN_ENTER_GAME_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ɫ��Ϣͬ��
Declare_Msg_Begin(S2C_ROLE_INFO_NOTIFY, MSG_S2C_ROLE_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwRoleId)									// ��ɫID
Declare_DecodeMsg_String(m_RoleInfo.szRoleName, sizeof(m_RoleInfo.szRoleName) - 1)	// ��ɫ����
Declare_DecodeMsg_String(m_RoleInfo.szAccount, sizeof(m_RoleInfo.szAccount) - 1)		// ��ɫ�ʺ�
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwStamina)									// ����
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wLevel)									// �ȼ�
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwExp)										// ����
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.wSex)									// �Ա�
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwGold)									// ���
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwHeadImage)								// ͷ��ͼ��
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwDiamond)									// ��ʯ	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwEnergy)									// ����
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nArenaPrestige)							// ����������			
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nHeroSoul)									// �꾧
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nGodSoul)									// ��� 
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nWanted)									// ͨ����
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwFightingPower)							// ս��	
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.sVipData.wLevel)							// Vip�ȼ�	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.sVipData.dwExp)							// Vip����	
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.wClickLikeTimes)							// ���޴���	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwRebelValue)								// �Ѿ�ֵ(ս��)	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nTowerPrestige)							// ��������(����)	
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)											// ����ID	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwCurContribution)							// ���ṱ��ֵ	
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wDailyInitLevel)							// ÿ�ճ�ʼ�ȼ�
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastStaminaRecoverTime)					// �����ϴλָ�ʱ��
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastEnergyRecoverTime)					// �����ϴλָ�ʱ��
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastWantedRecoverTime)					// ͨ����(������)�ϴλָ�ʱ��
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wGuideNum)								// �����б�
Declare_DecodeMsg_Memory(m_RoleInfo.arGuideList, m_RoleInfo.wGuideNum*sizeof(GuideData));
Declare_DecodeMsg_UnsignedInt(m_dwOpenServerTime)									// ����ʱ��
Declare_DecodeMsg_UnsignedInt(m_dwServerId)											// ������ID
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nHonor)									// ����
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wFriendshipPoint)					// �ܵ������
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wTodayFriendshipPoint)					// ���������
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRoleId)									// ��ɫID
Declare_EncodeMsg_String(m_RoleInfo.szRoleName, sizeof(m_RoleInfo.szRoleName) - 1)	// ��ɫ����
Declare_EncodeMsg_String(m_RoleInfo.szAccount, sizeof(m_RoleInfo.szAccount) - 1)		// ��ɫ�ʺ�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwStamina)									// ����
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wLevel)									// �ȼ�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwExp)										// ����
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.wSex)									// �Ա�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwGold)									// ���
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwHeadImage)								// ͷ��ͼ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwDiamond)									// ��ʯ	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwEnergy)									// ����
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nArenaPrestige)							// ����������			
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nHeroSoul)									// �꾧
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nGodSoul)									// ��� 
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nWanted)									// ͨ����
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwFightingPower)							// ս��	
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.sVipData.wLevel)							// Vip�ȼ�	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.sVipData.dwExp)							// Vip����	
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.wClickLikeTimes)							// ���޴���	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRebelValue)								// �Ѿ�ֵ(ս��)	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nTowerPrestige)							// ��������(����)	
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)											// ����ID	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwCurContribution)							// ���ṱ��ֵ	
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wDailyInitLevel)							// ÿ�ճ�ʼ�ȼ�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastStaminaRecoverTime)					// �����ϴλָ�ʱ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastEnergyRecoverTime)					// �����ϴλָ�ʱ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastWantedRecoverTime)					// ͨ����(������)�ϴλָ�ʱ��
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wGuideNum)								// �����б�
Declare_EncodeMsg_Memory(m_RoleInfo.arGuideList, m_RoleInfo.wGuideNum*sizeof(GuideData));
Declare_EncodeMsg_UnsignedInt(m_dwOpenServerTime)									// ����ʱ��
Declare_EncodeMsg_UnsignedInt(m_dwServerId)											// ������ID
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nHonor)									// ����
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wFriendshipPoint)						// �ܵ������
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wTodayFriendshipPoint)					// ���������
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nEndlessStore)								// �޾�ʯ
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRoleCreateTime)							// ��ɫ����ʱ��
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wNobilityLevel)							// ��λ�ȼ�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nGemCoin)									// ��ʯ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nDestinyCoin)								// ����Ӳ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nStarSoul)									// �ǻ�
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nCrystal)									// ˮ��
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nScoreMatchHonor)							// ����������
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nYCoin)									// YCoin
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nResouceWarScore)							// ��Դս����
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwTotalRechargeAmount)						// �ܵĳ�ֵ���
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleBaseInfo, m_RoleInfo)	// ��ɫ������Ϣ
Declare_Msg_Member(DWORD, m_dwGuildId)	// ����ID
Declare_Msg_Member(DWORD, m_dwOpenServerTime)	// ����ʱ��
Declare_Msg_Member(DWORD, m_dwServerId)	// ������ID
Declare_Msg_End


// ������Ըı�
Declare_Msg_Begin(S2C_ROLE_PROPERTY_NOTIFY, MSG_S2C_ROLE_PROPERTY_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_tProperty.wType)
Declare_DecodeMsg_UnsignedInt(m_tProperty.nValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_tProperty.wType)
Declare_EncodeMsg_UnsignedInt(m_tProperty.nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(Property, m_tProperty)
Declare_Msg_End

// ��ɫ��ϸ����
Declare_Msg_Begin(C2S_ROLE_DETAILS_REQ, MSG_C2S_ROLE_DETAILS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_Member(DWORD, m_dwTargetId)	// ���ҵ�Ŀ��ID
Declare_Msg_End

// ��ɫ��ϸ��Ӧ
Declare_Msg_Begin(S2C_ROLE_DETAILS_RESP, MSG_S2C_ROLE_DETAILS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_Details.dwRoleId);
Declare_DecodeMsg_UnsignedChar(m_Details.wLevel);
Declare_DecodeMsg_UnsignedShort(m_Details.wHeadIcon);
Declare_DecodeMsg_UnsignedInt(m_Details.dwFightingPower);
Declare_DecodeMsg_String(m_Details.szRoleName, sizeof(m_Details.szRoleName) - 1);
Declare_DecodeMsg_String(m_Details.szGuildName, sizeof(m_Details.szGuildName) - 1);
Declare_DecodeMsg_UnsignedChar(m_Details.byIsFriend);
Declare_DecodeMsg_UnsignedChar(m_Details.byIsBlack);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_Details.dwRoleId);
Declare_EncodeMsg_UnsignedChar(m_Details.wLevel);
Declare_EncodeMsg_UnsignedChar(m_Details.byVipLevel);
Declare_EncodeMsg_UnsignedShort(m_Details.wHeadIcon);
Declare_EncodeMsg_UnsignedInt(m_Details.dwFightingPower);
Declare_EncodeMsg_String(m_Details.szRoleName, sizeof(m_Details.szRoleName));
Declare_EncodeMsg_String(m_Details.szGuildName, sizeof(m_Details.szGuildName));
Declare_EncodeMsg_UnsignedChar(m_Details.byIsFriend);
Declare_EncodeMsg_UnsignedChar(m_Details.byIsBlack);
Declare_EncodeMsg_UnsignedShort(m_Details.byFormationNum);
for (BYTE i = 0; i < m_Details.byFormationNum; ++i)
{
	ClientFormation *pFormaion = &m_Details.arFormationList[i];
	Declare_EncodeMsg_UnsignedChar(pFormaion->bySlot);
	Declare_EncodeMsg_UnsignedShort(pFormaion->wHeroInfoId);
	Declare_EncodeMsg_UnsignedChar(pFormaion->byAwakenStar);
	Declare_EncodeMsg_UnsignedShort(pFormaion->wBreakLevel);
	Declare_EncodeMsg_UnsignedChar(pFormaion->byQaulity);
	Declare_EncodeMsg_UnsignedShort(pFormaion->byEquipmentNum);
	for (BYTE j = 0; j < pFormaion->byEquipmentNum; ++j)
	{
		FormationEquipment *pEquip = &pFormaion->arEquipmentList[j];
		//Declare_EncodeMsg_UnsignedChar(pEquip->byType);
		Declare_EncodeMsg_UnsignedShort(pEquip->wInfoId);
		Declare_EncodeMsg_UnsignedShort(pEquip->wStrengthenLevel);
		Declare_EncodeMsg_UnsignedChar(pEquip->byRefineLevel);
	}
	Declare_EncodeMsg_UnsignedShort(pFormaion->byTreasureNum);
	for (BYTE j = 0; j < pFormaion->byTreasureNum; ++j)
	{
		FormationTreasure *pTreasure = &pFormaion->arTreasureList[j];
		//Declare_EncodeMsg_UnsignedChar(pTreasure->byType);
		Declare_EncodeMsg_UnsignedShort(pTreasure->wInfoId);
		Declare_EncodeMsg_UnsignedShort(pTreasure->wStrengthenLevel);
		Declare_EncodeMsg_UnsignedChar(pTreasure->byRefineLevel);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_Member(ClientRoleDetails, m_Details)
Declare_Msg_End

// ��ɫ���Իָ�����
Declare_Msg_Begin(C2S_ROLE_PRO_RECOVER_REQ, MSG_C2S_ROLE_PRO_RECOVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// ��������
Declare_Msg_End

// ��ɫ���Իָ���Ӧ
Declare_Msg_Begin(S2C_ROLE_PRO_RECOVER_RESP, MSG_S2C_ROLE_PRO_RECOVER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwRecoverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwRecoverTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)				// ��������
Declare_Msg_Member(DWORD, m_dwRecoverTime)		// �ָ�ʱ��
Declare_Msg_End


// ͷ���滻����
Declare_Msg_Begin(C2S_HEAD_IMAGE_REPLACE_REQ, MSG_C2S_HEAD_IMAGE_REPLACE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeadImageId)		// ͷ��ID
Declare_Msg_End

// ͷ���滻��Ӧ
Declare_Msg_Begin(S2C_HEAD_IMAGE_REPLACE_RESP, MSG_S2C_HEAD_IMAGE_REPLACE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeadImageId)		// ͷ��ID
Declare_Msg_End


// ս����ʼ(����)
Declare_Msg_Begin(C2S_BATTLE_START_REQ, MSG_C2S_BATTLE_START_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwParam1)
Declare_DecodeMsg_UnsignedInt(m_dwParam2)
Declare_DecodeMsg_UnsignedInt(m_dwParam3)
Declare_DecodeMsg_UnsignedInt(m_dwParam4)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// ǩ��
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwParam1)
Declare_EncodeMsg_UnsignedInt(m_dwParam2)
Declare_EncodeMsg_UnsignedInt(m_dwParam3)
Declare_EncodeMsg_UnsignedInt(m_dwParam4)
Declare_EncodeMsg_UnsignedInt(m_dwPower)
Declare_EncodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// ǩ��
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// ս������
Declare_Msg_Member(DWORD, m_dwParam1)		// ����1
Declare_Msg_Member(DWORD, m_dwParam2)		// ����2
Declare_Msg_Member(DWORD, m_dwParam3)		// ����3
Declare_Msg_Member(DWORD, m_dwParam4)		// ����4
Declare_Msg_Member(DWORD, m_dwPower)		// ս��
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // �ʺ�
Declare_Msg_End

// ս����ʼ(��Ӧ)
Declare_Msg_Begin(S2C_BATTLE_START_RESP, MSG_S2C_BATTLE_START_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetFightingPower)
Declare_EncodeMsg_UnsignedInt(m_dwHelpBattleHeroInfoId)
//Declare_EncodeMsg_UnsignedChar(m_byIsPassed)
Declare_EncodeMsg_UnsignedInt(m_dwParam1)
Declare_EncodeMsg_UnsignedInt(m_dwParam2)
Declare_EncodeMsg_UnsignedInt(m_dwParam3)
Declare_EncodeMsg_UnsignedInt(m_dwParam4)
Declare_EncodeMsg_UnsignedShort(m_byMonsterDropNum)
Declare_EncodeMsg_Memory(m_DropItemList, m_byMonsterDropNum * sizeof(MonsterDropData))
Declare_EncodeMsg_UnsignedShort(m_byMonsterNum)
for (BYTE i = 0; i < m_byMonsterNum; ++i)
{
	Declare_EncodeMsg_UnsignedChar(m_MonsterList[i].byIndex)
	Declare_EncodeMsg_UnsignedInt(m_MonsterList[i].dwDamage)
	Declare_EncodeMsg_UnsignedInt(m_MonsterList[i].dwTotalHP)
}
Declare_EncodeMsg_UnsignedShort(m_wTargetToBattleHeroNum)
for (BYTE i = 0; i < m_wTargetToBattleHeroNum; ++i)
{
	ToBattleHeroData &tHeroData = m_TargetToBattleHeroList[i];
	Declare_EncodeMsg_UnsignedShort(tHeroData.wHeroInfoId)
	Declare_EncodeMsg_UnsignedChar(tHeroData.byPos)
	Declare_EncodeMsg_UnsignedShort(tHeroData.wLevel)
	Declare_EncodeMsg_UnsignedChar(tHeroData.byQuality)
	Declare_EncodeMsg_UnsignedShort(tHeroData.byProNum)
	Declare_EncodeMsg_Memory(tHeroData.arPorList, tHeroData.byProNum * sizeof(Property))
 	Declare_EncodeMsg_UnsignedInt(tHeroData.tBlessSpriteData.dwSpriteId)
 	Declare_EncodeMsg_UnsignedShort(tHeroData.tBlessSpriteData.wStar)
 	Declare_EncodeMsg_UnsignedShort(tHeroData.tBlessSpriteData.wEnhanceLevel)
 	Declare_EncodeMsg_UnsignedShort(tHeroData.tBlessSpriteData.wTrainStage)
}
Declare_EncodeMsg_UnsignedShort(m_TargetSpriteList.size())
for (size_t i = 0; i < m_TargetSpriteList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_TargetSpriteList[i].dwId);
	Declare_EncodeMsg_UnsignedShort(m_TargetSpriteList[i].wTrainStage);
}
Declare_EncodeMsg_UnsignedShort(m_wHelpBattleHeroProNum)
for (WORD i = 0; i < m_wHelpBattleHeroProNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_vHelpBattleHeroProList[i].wType);
	Declare_EncodeMsg_UnsignedInt(m_vHelpBattleHeroProList[i].nValue);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)					// ս������
Declare_Msg_Member(DWORD, m_dwTargetFightingPower)	// Ŀ��ս��
Declare_Msg_Member(DWORD, m_dwHelpBattleHeroInfoId)	// ��սӢ����ϢID
//Declare_Msg_Member(BYTE, m_byIsPassed)				// �Ƿ���ͨ��
Declare_Msg_Member(DWORD, m_dwParam1)				// ����1
Declare_Msg_Member(DWORD, m_dwParam2)				// ����2
Declare_Msg_Member(DWORD, m_dwParam3)				// ����3
Declare_Msg_Member(DWORD, m_dwParam4)				// ����3
Declare_Msg_Member(BYTE, m_byMonsterDropNum)									// ������Ʒ����
Declare_Msg_Member_Array(MonsterDropData, m_DropItemList, MAX_MAP_DROP_NUM);	// ������Ʒ�б�
Declare_Msg_Member(BYTE, m_byMonsterNum)										// ��������
Declare_Msg_Member_Array(MonsterData, m_MonsterList, MAX_BATTLE_MONSTER_NUM);	// �����б�
Declare_Msg_Member(WORD, m_wTargetToBattleHeroNum)									// ����Ӣ������
Declare_Msg_Member_Array(ToBattleHeroData, m_TargetToBattleHeroList, MAX_BATTLE_HERO_NUM);// Ŀ������Ӣ���б�
Declare_Msg_Member(WORD, m_wHelpBattleHeroProNum) // ��սӢ����������
Declare_Msg_Member_Array(Property, m_vHelpBattleHeroProList, HERO_PRO_MAX);// ��սӢ�������б�
Declare_Msg_Member(vector<SpiritData>, m_TargetSpriteList) // Ŀ�꾫���б�
Declare_Msg_End

// ս������(����)
Declare_Msg_Begin(C2S_BATTLE_END_REQ, MSG_C2S_BATTLE_END_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_nWinValue)
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_UnsignedInt(m_dwContext1)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
Declare_DecodeMsg_UnsignedInt(m_dwTotalDamge)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// ǩ��
Declare_DecodeMsg_UnsignedShort(m_byMonsterNum)
m_byMonsterNum = min(m_byMonsterNum, MAX_BATTLE_MONSTER_NUM);
for (BYTE i = 0; i < m_byMonsterNum; i++)
{
	Declare_DecodeMsg_UnsignedChar(m_MonsterList[i].byIndex)
	Declare_DecodeMsg_UnsignedInt(m_MonsterList[i].dwDamage)
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// ս������
Declare_Msg_Member(int, m_nWinValue)		// �Ƿ��ʤֵ
Declare_Msg_Member(BYTE, m_byStar)		// �������	
Declare_Msg_Member(DWORD, m_dwContext)		// ������
Declare_Msg_Member(DWORD, m_dwContext1)		// ������1
Declare_Msg_Member(DWORD, m_dwPower)		// ս��
Declare_Msg_Member(DWORD, m_dwTotalDamge)		// ���˺�
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // ǩ��
Declare_Msg_Member(BYTE, m_byMonsterNum)		// ��������
Declare_Msg_Member_Array(MonsterData, m_MonsterList, MAX_BATTLE_MONSTER_NUM);	// �����б�
Declare_Msg_End

// ս������(��Ӧ)
Declare_Msg_Begin(S2C_BATTLE_END_RESP, MSG_S2C_BATTLE_END_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsWin)
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsWin)
Declare_EncodeMsg_UnsignedChar(m_byStar)
Declare_EncodeMsg_UnsignedChar(m_byIsFirstWin)
Declare_EncodeMsg_UnsignedChar(m_byIsMeKill)
Declare_EncodeMsg_UnsignedShort(m_byParamNum)
Declare_EncodeMsg_Memory(m_arParamList, m_byParamNum*sizeof(DWORD));
Declare_EncodeMsg_UnsignedShort(m_wRewardItemNum)
Declare_EncodeMsg_Memory(m_arRewardItemList, m_wRewardItemNum*sizeof(Item));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsWin)			// �Ƿ��ʤ
Declare_Msg_Member(BYTE, m_byStar)			// �������	
Declare_Msg_Member(BYTE, m_byIsFirstWin)	// �Ƿ��һ�λ��
Declare_Msg_Member(BYTE, m_byIsMeKill)		// �Ƿ����һ�ɱ
Declare_Msg_Member(BYTE, m_byParamNum)		// ��������
Declare_Msg_Member_Array(DWORD, m_arParamList, MAX_BATTLE_PARAM_NUM);	// �����б�
Declare_Msg_Member(WORD, m_wRewardItemNum)
Declare_Msg_Member_Array(Item, m_arRewardItemList, MAX_BATTLE_REWARD_ITEM_NUM);	// ��������Ʒ�б�
Declare_Msg_End

// ������սĿ��
Declare_Msg_Begin(C2S_HELP_BATTLE_TARGET_REQ, MSG_C2S_HELP_BATTLE_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������սĿ����Ӧ
Declare_Msg_Begin(S2C_HELP_BATTLE_TARGET_RESP, MSG_S2C_HELP_BATTLE_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTargetNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTargetNum)
for (WORD i = 0; i < m_wTargetNum; ++i)
{
	HelpBattleTargetData &tTargetData = m_arTargetList[i];
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwRoleId);
	Declare_EncodeMsg_String(tTargetData.szRoleName, sizeof(tTargetData.szRoleName));
	Declare_EncodeMsg_UnsignedShort(tTargetData.wLevel);
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwFightingPower);
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwHeroInfoId);
	Declare_EncodeMsg_UnsignedChar(tTargetData.byTargetType);
	Declare_EncodeMsg_UnsignedChar(tTargetData.byHelpBattleTimes);
	Declare_EncodeMsg_UnsignedChar(tTargetData.byVipLevel);
	//Declare_EncodeMsg_UnsignedInt(tTargetData.dwTitleId);
}
Declare_EncodeMsg_UnsignedShort(m_wTodayUsedHelpBattleTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTargetNum)	// Ŀ������
Declare_Msg_Member_Array(HelpBattleTargetData, m_arTargetList, MAX_HELP_BATTLE_TARGET_NUM);	// Ŀ���б�
Declare_Msg_Member(WORD, m_wTodayUsedHelpBattleTimes)	// ������ʹ����ս�Ĵ���
Declare_Msg_End

// ����
Declare_Msg_Begin(C2S_RANK_LIKE_REQ, MSG_C2S_RANK_LIKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// ����
Declare_Msg_Member(DWORD, m_dwTargetId)		// Ŀ��ID
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_RANK_LIKE_RESP, MSG_S2C_RANK_LIKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// ����
Declare_Msg_Member(DWORD, m_dwTargetId)		// Ŀ��ID
Declare_Msg_End

// ����
Declare_Msg_Begin(C2S_RANK_LEAVE_MESSAGE_REQ, MSG_C2S_RANK_LEAVE_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szText, sizeof(m_szText))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_64)	//
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_RANK_LEAVE_MESSAGE_RESP, MSG_S2C_RANK_LEAVE_MESSAGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szText, sizeof(m_szText))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_64)	//
Declare_Msg_End

// �����б�����
Declare_Msg_Begin(C2S_RANK_LEAVE_MESSAGE_LIST_REQ, MSG_C2S_RANK_LEAVE_MESSAGE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б���Ӧ
Declare_Msg_Begin(S2C_RANK_LEAVE_MESSAGE_LIST_RESP, MSG_S2C_RANK_LEAVE_MESSAGE_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLeaveMessageNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLeaveMessageNum)
for (WORD i = 0; i < m_wLeaveMessageNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_LeaveMessageList[i].dwRoleId)
	Declare_EncodeMsg_String(m_LeaveMessageList[i].szMessage, MAX_RANK_LEAVE_MESSAGE_LEN)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_wLeaveMessageNum)		// �����б�����
Declare_Msg_Member_Array(LeaveMessageData, m_LeaveMessageList, MAX_RANK_LEAVE_MESSAGE_NUM);	// �����б�
Declare_Msg_End

// �������а��б�
Declare_Msg_Begin(C2S_RANK_LIST_REQ, MSG_C2S_RANK_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedChar(m_bySubType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedChar(m_bySubType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// ��������
Declare_Msg_Member(BYTE, m_bySubType)		// ����������
Declare_Msg_End

// �������а��б���Ӧ
Declare_Msg_Begin(S2C_RANK_LIST_RESP, MSG_S2C_RANK_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedChar(m_bySubType)
Declare_DecodeMsg_UnsignedShort(m_wListNum)
for (WORD i = 0; i < m_wListNum && i < MAX_RANK_MUM; ++i)
{
	ClientRoleRankData &sData = m_arRankList[i];
	Declare_DecodeMsg_UnsignedInt(sData.dwRoleId)
	Declare_DecodeMsg_UnsignedInt(sData.dwServerId)
	Declare_DecodeMsg_String(sData.szRoleName, sizeof(sData.szRoleName) - 1)
	Declare_DecodeMsg_UnsignedShort(sData.wHeadIcon)
	Declare_DecodeMsg_UnsignedChar(sData.wLevel)
	Declare_DecodeMsg_UnsignedInt(sData.dwFightingPower)
	Declare_DecodeMsg_UnsignedInt(sData.dwValue)
	Declare_DecodeMsg_UnsignedInt(sData.dwParam1)
	Declare_DecodeMsg_UnsignedInt(sData.dwParam2)
	Declare_DecodeMsg_String(sData.szGuildName, sizeof(sData.szGuildName) - 1)
	Declare_DecodeMsg_UnsignedChar(sData.byVipLevel);
	Declare_DecodeMsg_UnsignedInt(sData.dwTitleId)
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedChar(m_bySubType)
Declare_EncodeMsg_UnsignedShort(m_wRank)
Declare_EncodeMsg_UnsignedInt(m_dwRankValue)
Declare_EncodeMsg_UnsignedShort(m_wListNum)
for (WORD i = 0; i < m_wListNum && i < MAX_RANK_MUM; ++i)
{
	ClientRoleRankData &sData = m_arRankList[i];
	Declare_EncodeMsg_UnsignedInt(sData.dwRoleId)
	Declare_EncodeMsg_UnsignedInt(sData.dwServerId)
	Declare_EncodeMsg_String(sData.szRoleName, sizeof(sData.szRoleName))
	Declare_EncodeMsg_UnsignedShort(sData.wHeadIcon)
	Declare_EncodeMsg_UnsignedChar(sData.wLevel)
	Declare_EncodeMsg_UnsignedInt(sData.dwFightingPower)
	Declare_EncodeMsg_UnsignedInt(sData.dwValue)
	Declare_EncodeMsg_UnsignedInt(sData.dwParam1)
	Declare_EncodeMsg_UnsignedInt(sData.dwParam2)
	Declare_EncodeMsg_String(sData.szGuildName, sizeof(sData.szGuildName))
	Declare_EncodeMsg_UnsignedChar(sData.byVipLevel);
	Declare_EncodeMsg_UnsignedInt(sData.dwTitleId)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)									// ��������
Declare_Msg_Member(BYTE, m_bySubType)		// ����������
Declare_Msg_Member(WORD, m_wRank)									// ����
Declare_Msg_Member(DWORD, m_dwRankValue)							// ����ֵ
Declare_Msg_Member(WORD, m_wListNum)								// �б�����
Declare_Msg_Member_Array(ClientRoleRankData, m_arRankList, MAX_RANK_MUM);	// �б�����
Declare_Msg_End



// ���󹤻����а��б�
Declare_Msg_Begin(C2S_GUILD_RANK_LIST_REQ, MSG_C2S_GUILD_RANK_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRankType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRankType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRankType)		// ������������
Declare_Msg_End

// �������а�������Ӧ
Declare_Msg_Begin(S2C_GUILD_RANK_LIST_RESP, MSG_S2C_GUILD_RANK_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRankType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRankType)
Declare_EncodeMsg_UnsignedShort(m_wRank)
Declare_EncodeMsg_UnsignedInt(m_dwRankValue)
Declare_EncodeMsg_UnsignedShort(m_wListNum)
for (WORD i = 0; i < m_wListNum && i < MAX_GUILD_RANK_MUM; ++i)
{
	ClientGuildRankData &sData = m_arRankList[i];
	Declare_EncodeMsg_UnsignedInt(sData.dwGuildId)
	Declare_EncodeMsg_String(sData.szGuildName, sizeof(sData.szGuildName))
	Declare_EncodeMsg_String(sData.szCaptainName, sizeof(sData.szCaptainName))
	Declare_EncodeMsg_UnsignedShort(sData.wIconId)
	Declare_EncodeMsg_UnsignedChar(sData.wLevel)
	Declare_EncodeMsg_UnsignedInt(sData.dwValue)
	Declare_EncodeMsg_UnsignedChar(sData.byMemberNum)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRankType)								// ��������
Declare_Msg_Member(WORD, m_wRank)									// ����
Declare_Msg_Member(DWORD, m_dwRankValue)							// ����ֵ
Declare_Msg_Member(WORD, m_wListNum)								// �б�����
Declare_Msg_Member_Array(ClientGuildRankData, m_arRankList, MAX_GUILD_RANK_MUM);	// �б�����
Declare_Msg_End


// �����ҵ�����
Declare_Msg_Begin(C2S_MY_RANK_REQ, MSG_C2S_MY_RANK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRankType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRankType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRankType)
Declare_Msg_End

// �����ҵ�������Ӧ
Declare_Msg_Begin(S2C_MY_RANK_RESP, MSG_S2C_MY_RANK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRankType)
Declare_DecodeMsg_UnsignedShort(m_wMyRank)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRankType)
Declare_EncodeMsg_UnsignedShort(m_wMyRank)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRankType)	// ��������
Declare_Msg_Member(WORD, m_wMyRank)		// �ҵ����� 0=��ʾľ�н�������
Declare_Msg_End

//���ؽ�ɫ���֪ͨ
// Declare_Msg_Begin(M2C_LOAD_CHAR_FINISH_CMD, MSG_M2C_LOAD_CHAR_FINISH_CMD)
// Declare_DecodeMsg_Empty_Function_Begin
// Declare_DecodeMsg_Empty_Function_End
// 
// Declare_EncodeMsg_Empty_Function_Begin
// Declare_EncodeMsg_Empty_Function_End
// 
// Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_GUIDE_UPDATE_REQ, MSG_C2S_GUIDE_UPDATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nKey)
Declare_DecodeMsg_UnsignedInt(m_nValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nKey)
Declare_EncodeMsg_UnsignedInt(m_nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nKey)
Declare_Msg_Member(int, m_nValue)
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_GUIDE_UPDATE_RESP, MSG_S2C_GUIDE_UPDATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nKey)
Declare_DecodeMsg_UnsignedInt(m_nValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nKey)
Declare_EncodeMsg_UnsignedInt(m_nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nKey)
Declare_Msg_Member(int, m_nValue)
Declare_Msg_End


//������
Declare_Msg_Begin(C2S_HEART_BEAT_REQ, MSG_C2S_HEART_BEAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwValue)
Declare_Msg_End

//������
Declare_Msg_Begin(S2C_HEART_BEAT_RESP, MSG_S2C_HEART_BEAT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwValue)
Declare_Msg_End


//������ʱ��
Declare_Msg_Begin(C2S_SERVER_TIME_REQ, MSG_C2S_SERVER_TIME_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//ͬ��������ʱ��
Declare_Msg_Begin(S2C_SERVER_TIME_RESP, MSG_S2C_SERVER_TIME_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwServerTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwServerTime)
Declare_EncodeMsg_UnsignedShort(m_wYear)
Declare_EncodeMsg_UnsignedChar(m_byMonth)
Declare_EncodeMsg_UnsignedChar(m_byDay)
Declare_EncodeMsg_UnsignedChar(m_byHour)
Declare_EncodeMsg_UnsignedChar(m_byMinute)
Declare_EncodeMsg_UnsignedChar(m_bySecond)
Declare_EncodeMsg_UnsignedInt(m_dwMakeTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwServerTime)
Declare_Msg_Member(WORD, m_wYear)
Declare_Msg_Member(BYTE, m_byMonth)
Declare_Msg_Member(BYTE, m_byDay)
Declare_Msg_Member(BYTE, m_byHour)
Declare_Msg_Member(BYTE, m_byMinute)
Declare_Msg_Member(BYTE, m_bySecond)
Declare_Msg_Member(DWORD, m_dwMakeTime)
Declare_Msg_End


// �ʼ��б�����
Declare_Msg_Begin(C2S_EMAIL_LIST_REQ, MSG_C2S_EMAIL_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ʼ��б���Ӧ
Declare_Msg_Begin(S2C_EMAIL_LIST_RESP, MSG_S2C_EMAIL_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byEmailNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byEmailNum = min(m_byEmailNum, MAX_EMAIL_COUNT);
Declare_EncodeMsg_UnsignedShort(m_byEmailNum)
for (BYTE i=0; i<m_byEmailNum; i++)
{
	EmailInfo &sEmail = m_EmailList[i];
	//Declare_EncodeMsg_UnsignedInt(sEmail.dwID)			// ΨһID
	Declare_EncodeMsg_UnsignedShort(sEmail.wInfoId)			// ��ϢID
	Declare_EncodeMsg_UnsignedInt(sEmail.dwReceiveTime)		// ����ʱ��
	Declare_EncodeMsg_UnsignedShort(sEmail.wContextNum);
	for (BYTE j = 0; j < sEmail.wContextNum; ++j)
	{
		EmailContext &sContext = sEmail.arContextList[j];
		Declare_EncodeMsg_UnsignedChar(sContext.wType);						// ����������
		Declare_EncodeMsg_String(sContext.szText, sizeof(sContext.szText));	// �����Ļ���
	}
	Declare_EncodeMsg_String(sEmail.szTitle, sizeof(sEmail.szTitle));	// ����
	Declare_EncodeMsg_String(sEmail.szText, sizeof(sEmail.szText));	// �ʼ�����
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE,  m_byEmailNum)								// �ʼ�����
Declare_Msg_Member_Array(EmailInfo, m_EmailList, MAX_EMAIL_COUNT)   // �ʼ��б�
Declare_Msg_End


//���ʼ�
Declare_Msg_Begin(S2C_EMAIL_NEW_NOTIFY, MSG_S2C_EMAIL_NEW_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tEmailInfo.wInfoId) // ��ϢID
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedInt(m_tEmailInfo.dwID)			// ΨһID
Declare_EncodeMsg_UnsignedShort(m_tEmailInfo.wInfoId)		// ��ϢID
Declare_EncodeMsg_UnsignedInt(m_tEmailInfo.dwReceiveTime)	// ����ʱ��
Declare_EncodeMsg_UnsignedShort(m_tEmailInfo.wContextNum);
for (WORD i = 0; i < m_tEmailInfo.wContextNum; ++i)
{
	EmailContext &sContext = m_tEmailInfo.arContextList[i];
	Declare_EncodeMsg_UnsignedChar(sContext.wType);	// ����������
	Declare_EncodeMsg_String(sContext.szText, sizeof(sContext.szText));	// �����Ļ���
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(EmailInfo, m_tEmailInfo)
Declare_Msg_End


//C2M ��������
Declare_Msg_Begin(C2S_CHAT_REQ, MSG_C2S_CHAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_tData.byChatType)						// ��������
	Declare_DecodeMsg_UnsignedChar(m_tData.byIsVoice)					// �Ƿ�����
	Declare_DecodeMsg_UnsignedInt(m_tData.dwRoleId)						// Ŀ���ɫID
	Declare_DecodeMsg_String(m_tData.szTargetName, sizeof(m_tData.szTargetName)-1)		// Ŀ������
	Declare_DecodeMsg_String(m_tData.szChatText, sizeof(m_tData.szChatText)-1)	// ��������
	Declare_DecodeMsg_String(m_tData.szVoice, sizeof(m_tData.szVoice)-1)		// ��������
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_tData.byChatType)						// ��������
	Declare_EncodeMsg_UnsignedChar(m_tData.byIsVoice)					// �Ƿ�����
	Declare_EncodeMsg_UnsignedInt(m_tData.dwRoleId)						// Ŀ���ɫID
	Declare_EncodeMsg_String(m_tData.szTargetName, sizeof(m_tData.szTargetName))		// Ŀ������
	Declare_EncodeMsg_String(m_tData.szChatText, sizeof(m_tData.szChatText))		// ��������
	Declare_EncodeMsg_String(m_tData.szVoice, sizeof(m_tData.szVoice))		// ��������
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ChatReqData, m_tData)
Declare_Msg_End

//����Ӧ��
Declare_Msg_Begin(S2C_CHAT_RESP, MSG_S2C_CHAT_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//����֪ͨ
Declare_Msg_Begin(S2C_CHAT_NOTIFY, MSG_S2C_CHAT_NOTIFY)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_Char(m_tNotifyData.byChatType)
 	Declare_DecodeMsg_UnsignedInt( m_tNotifyData.tSenderData.dwRoleID )
//	Declare_DecodeMsg_UnsignedShort(m_tNotifyData.tSenderData.wLevel)
	Declare_DecodeMsg_UnsignedInt(m_tNotifyData.tSenderData.dwHeadIcon)
	Declare_DecodeMsg_UnsignedShort(m_tNotifyData.tSenderData.wServerId)
	Declare_DecodeMsg_UnsignedChar(m_tNotifyData.tSenderData.byVipLevel)
	Declare_DecodeMsg_String(m_tNotifyData.tSenderData.szRoleName, MAX_ROLE_LEN-1)
	Declare_DecodeMsg_UnsignedInt(m_tNotifyData.tSenderData.dwCurUseNicknameId)
	Declare_DecodeMsg_UnsignedInt(m_tNotifyData.dwVoiceId)
	Declare_DecodeMsg_String(m_tNotifyData.szChatText, MAX_CHAT_TEXT_LEN-1)
	Declare_DecodeMsg_UnsignedChar(m_byGMLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Char(m_tNotifyData.byChatType)
Declare_EncodeMsg_UnsignedInt(m_tNotifyData.tSenderData.dwRoleID)
//Declare_EncodeMsg_UnsignedShort(m_tChatCmd.tSenderData.wLevel)
Declare_EncodeMsg_UnsignedInt(m_tNotifyData.tSenderData.dwHeadIcon)
Declare_EncodeMsg_UnsignedShort(m_tNotifyData.tSenderData.wServerId)
Declare_EncodeMsg_UnsignedChar(m_tNotifyData.tSenderData.byVipLevel)
Declare_EncodeMsg_String(m_tNotifyData.tSenderData.szRoleName, MAX_ROLE_LEN)
Declare_EncodeMsg_UnsignedInt(m_tNotifyData.tSenderData.dwCurUseNicknameId)
Declare_EncodeMsg_UnsignedInt(m_tNotifyData.dwVoiceId)
Declare_EncodeMsg_String(m_tNotifyData.szChatText, MAX_CHAT_TEXT_LEN)
Declare_EncodeMsg_UnsignedChar(m_byGMLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ChatNotifyData, m_tNotifyData)
Declare_Msg_Member(BYTE, m_byGMLevel)	// ����Ƶ��
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_CHAT_VOICE_REQ, MSG_C2S_CHAT_VOICE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwVoiceId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwVoiceId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwVoiceId)	// ����ID
Declare_Msg_End


// ����������Ӧ
Declare_Msg_Begin(S2C_CHAT_VOICE_RESP, MSG_S2C_CHAT_VOICE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szVoice, MAX_CHAT_VOICE_LEN - 1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szVoice, MAX_CHAT_VOICE_LEN)
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szVoice, MAX_CHAT_VOICE_LEN)	//��������
Declare_Msg_End


// �����¼�б�����
Declare_Msg_Begin(C2S_CHAT_RECORD_LIST_REQ, MSG_C2S_CHAT_RECORD_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byChatType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byChatType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byChatType)	// ����Ƶ��
Declare_Msg_End

// �����¼�б���Ӧ
Declare_Msg_Begin(S2C_CHAT_RECORD_LIST_RESP, MSG_S2C_CHAT_RECORD_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byChatType)
Declare_EncodeMsg_UnsignedShort(m_wRecordNum)
for (WORD i = 0; i < m_wRecordNum; ++i)
{
	ChatRecord &tRecord = m_arRecordList[i];
	Declare_EncodeMsg_UnsignedInt(tRecord.tSenderData.dwRoleID)
	Declare_EncodeMsg_UnsignedInt(tRecord.tSenderData.dwHeadIcon)
	Declare_EncodeMsg_UnsignedShort(tRecord.tSenderData.wServerId)
	Declare_EncodeMsg_UnsignedChar(tRecord.tSenderData.byVipLevel)
	Declare_EncodeMsg_String(tRecord.tSenderData.szRoleName, MAX_ROLE_LEN - 1)
	Declare_EncodeMsg_UnsignedInt(tRecord.tSenderData.dwCurUseNicknameId)
	//Declare_EncodeMsg_UnsignedInt(tRecord.dwVoiceId)
	Declare_EncodeMsg_String(tRecord.szChatText, MAX_CHAT_TEXT_LEN)
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTime)
	Declare_EncodeMsg_UnsignedChar(tRecord.byGMLevel)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byChatType)	// ����Ƶ��
Declare_Msg_Member(WORD, m_wRecordNum)							// ��¼����
Declare_Msg_Member_Array(ChatRecord, m_arRecordList, MAX_CHAT_RECORD_COUNT)	// �����¼�б�
Declare_Msg_End


// ϵͳ����
Declare_Msg_Begin(S2C_SYS_BROADCAST_NOTIFY, MSG_S2C_SYS_BROADCAST_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwBroadCastId)
Declare_DecodeMsg_String(m_szText, MAX_BROADCAST_TEXT_LEN)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBroadCastId)
Declare_EncodeMsg_String(m_szText, MAX_BROADCAST_TEXT_LEN)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBroadCastId)							// ����ID
Declare_Msg_Member_Array(char, m_szText, MAX_BROADCAST_TEXT_LEN + 1)	//�ַ���1
Declare_Msg_End


// ��Ʒ����
Declare_Msg_Begin(C2S_SHOP_GOODS_BUY_REQ, MSG_C2S_SHOP_GOODS_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wParam1)
Declare_DecodeMsg_UnsignedShort(m_wParam2)
Declare_DecodeMsg_UnsignedShort(m_wParam3)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wParam1)
Declare_EncodeMsg_UnsignedShort(m_wParam2)
Declare_EncodeMsg_UnsignedShort(m_wParam3)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �̵���Ʒ����
Declare_Msg_Member(WORD, m_wParam1)		// ����1
Declare_Msg_Member(WORD, m_wParam2)		// ����2
Declare_Msg_Member(WORD, m_wParam3)		// ����2
Declare_Msg_End

// ��Ʒ������Ӧ
Declare_Msg_Begin(S2C_SHOP_GOODS_BUY_RESP, MSG_S2C_SHOP_GOODS_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wParam1)
Declare_DecodeMsg_UnsignedShort(m_wParam2)
Declare_DecodeMsg_UnsignedShort(m_wParam2)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wParam1)
Declare_EncodeMsg_UnsignedShort(m_wParam2)
Declare_EncodeMsg_UnsignedShort(m_wParam3)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �̵���Ʒ����
Declare_Msg_Member(WORD, m_wParam1)		// ����1
Declare_Msg_Member(WORD, m_wParam2)		// ����2
Declare_Msg_Member(WORD, m_wParam3)		// ����2
Declare_Msg_End

// ��Ʒ�����¼����
Declare_Msg_Begin(C2S_SHOP_GOODS_BUY_RECORD_REQ, MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �̵���Ʒ����
Declare_Msg_End

// ��Ʒ�����¼��Ӧ
Declare_Msg_Begin(S2C_SHOP_GOODS_BUY_RECORD_RESP, MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
m_byBuyRecordNum = min(m_byBuyRecordNum, MAX_GOOGS_BUY_RECORD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byBuyRecordNum)
Declare_EncodeMsg_Memory(m_arBuyRecord, m_byBuyRecordNum * sizeof(ShopGoodsBuyRecord))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// �̵���Ʒ����
Declare_Msg_Member(BYTE, m_byBuyRecordNum)	// �����¼����
Declare_Msg_Member_Array(ShopGoodsBuyRecord, m_arBuyRecord, MAX_GOOGS_BUY_RECORD_NUM);	// �����¼
Declare_Msg_End

// ��Ʒ����
Declare_Msg_Begin(C2S_SHOP_GOODS_SELL_REQ, MSG_C2S_SHOP_GOODS_SELL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedShort(m_byGoodsNum)
m_byGoodsNum = min(m_byGoodsNum, MAX_SELL_GOODS_NUM);
Declare_DecodeMsg_Memory(m_GoodsList, m_byGoodsNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
m_byGoodsNum = min(m_byGoodsNum, MAX_SELL_GOODS_NUM);
Declare_EncodeMsg_UnsignedShort(m_byGoodsNum)
Declare_EncodeMsg_Memory(m_GoodsList, m_byGoodsNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)	// ��������
Declare_Msg_Member(BYTE, m_byGoodsNum)	// ��Ʒ����
Declare_Msg_Member_Array(DWORD, m_GoodsList, MAX_SELL_GOODS_NUM);	// ��Ʒ�б�
Declare_Msg_End

// ��Ʒ������Ӧ
Declare_Msg_Begin(S2C_SHOP_GOODS_SELL_RESP, MSG_S2C_SHOP_GOODS_SELL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedShort(m_byGoodsNum)
m_byGoodsNum = min(m_byGoodsNum, MAX_SELL_GOODS_NUM);
Declare_DecodeMsg_Memory(m_GoodsList, m_byGoodsNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
m_byGoodsNum = min(m_byGoodsNum, MAX_SELL_GOODS_NUM);
Declare_EncodeMsg_UnsignedShort(m_byGoodsNum)
Declare_EncodeMsg_Memory(m_GoodsList, m_byGoodsNum * sizeof(DWORD));
Declare_EncodeMsg_UnsignedShort(m_byCurrencyNum)
Declare_EncodeMsg_Memory(m_CurrencyList, m_byCurrencyNum * sizeof(MoneyData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)	// ��������
Declare_Msg_Member(BYTE, m_byGoodsNum)	// ��Ʒ����
Declare_Msg_Member_Array(DWORD, m_GoodsList, MAX_SELL_GOODS_NUM);	// ��Ʒ�б�
Declare_Msg_Member(BYTE, m_byCurrencyNum)	// ��������
Declare_Msg_Member_Array(MoneyData, m_CurrencyList, MAX_MONEY_TYPE_NUM);	// ��õĻ����б�
Declare_Msg_End


// ��ʱ��Ʒ�б�����
Declare_Msg_Begin(C2S_SHOP_LIMIT_GOODS_LIST_REQ, MSG_C2S_SHOP_LIMIT_GOODS_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �̵���Ʒ����
Declare_Msg_End

// ��ʱ��Ʒ�б���Ӧ
Declare_Msg_Begin(S2C_SHOP_LIMIT_GOODS_LIST_RESP, MSG_S2C_SHOP_LIMIT_GOODS_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwRefreshTime)
Declare_EncodeMsg_UnsignedShort(m_byGoodsNum)
Declare_EncodeMsg_Memory(m_arGoodsList, m_byGoodsNum * sizeof(Goods))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// �̵���Ʒ����
Declare_Msg_Member(DWORD, m_dwRefreshTime)	// ˢ��ʱ��
Declare_Msg_Member(BYTE, m_byGoodsNum)	// ��Ʒ����
Declare_Msg_Member_Array(Goods, m_arGoodsList, MAX_LIMIT_GOODS_NUM);	// ��Ʒ�б�
Declare_Msg_End

// �̵�ˢ������
Declare_Msg_Begin(C2S_SHOP_REFRESH_REQ, MSG_C2S_SHOP_REFRESH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byShopType)
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byShopType)
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byShopType)			// �̵���Ʒ����
Declare_Msg_Member(BYTE, m_byRefreshType)		// ˢ������
Declare_Msg_End

// �̵�ˢ����Ӧ
Declare_Msg_Begin(S2C_SHOP_REFRESH_RESP, MSG_S2C_SHOP_REFRESH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byShopType)
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byShopType)
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byShopType)			// �̵���Ʒ����
Declare_Msg_Member(BYTE, m_byRefreshType)		// ˢ������
Declare_Msg_End


// Ӣ���̵���������
Declare_Msg_Begin(C2S_HERO_SHOP_DATA_REQ, MSG_C2S_HERO_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// Ӣ���̵�������Ӧ
Declare_Msg_Begin(S2C_HERO_SHOP_DATA_RESP, MSG_S2C_HERO_SHOP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_HeroShop.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_HeroShop.wGoodsNum)
Declare_EncodeMsg_Memory(m_HeroShop.arGoodsList, m_HeroShop.wGoodsNum*sizeof(ShopGoodsBuyRecord));
Declare_EncodeMsg_UnsignedInt(m_HeroShop.dwLastRecoverTime)
Declare_EncodeMsg_UnsignedChar(m_HeroShop.wFreeRefreshTimes)
Declare_EncodeMsg_UnsignedChar(m_HeroShop.wCostRefreshTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RefreshShopData, m_HeroShop)		// �̵���Ʒ����
Declare_Msg_End

// �����̵���������
Declare_Msg_Begin(C2S_AWAKEN_SHOP_DATA_REQ, MSG_C2S_AWAKEN_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����̵�������Ӧ
Declare_Msg_Begin(S2C_AWAKEN_SHOP_DATA_RESP, MSG_S2C_AWAKEN_SHOP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_AwakenShop.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_AwakenShop.wGoodsNum)
Declare_EncodeMsg_Memory(m_AwakenShop.arGoodsList, m_AwakenShop.wGoodsNum*sizeof(ShopGoodsBuyRecord));
Declare_EncodeMsg_UnsignedInt(m_AwakenShop.dwLastRecoverTime)
Declare_EncodeMsg_UnsignedChar(m_AwakenShop.wFreeRefreshTimes)
Declare_EncodeMsg_UnsignedChar(m_AwakenShop.wCostRefreshTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RefreshShopData, m_AwakenShop)		// �̵���Ʒ����
Declare_Msg_End


// �ǻ��̵���������
Declare_Msg_Begin(C2S_STAR_SOUL_SHOP_DATA_REQ, MSG_C2S_STAR_SOUL_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ǻ��̵�������Ӧ
Declare_Msg_Begin(S2C_STAR_SOUL_SHOP_DATA_RESP, MSG_S2C_STAR_SOUL_SHOP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_ShopData.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ShopData.wGoodsNum)
Declare_EncodeMsg_Memory(m_ShopData.arGoodsList, m_ShopData.wGoodsNum*sizeof(ShopGoodsBuyRecord));
Declare_EncodeMsg_UnsignedInt(m_ShopData.dwLastAutoRefreshTime)
Declare_EncodeMsg_UnsignedChar(m_ShopData.wUsedRefreshTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(StarSoulShopData, m_ShopData)		// �̵���Ʒ����
Declare_Msg_End

// ֱ���̵���������
Declare_Msg_Begin(C2S_DIRECT_SHOP_DATA_REQ, MSG_C2S_DIRECT_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ֱ���̵�������Ӧ
Declare_Msg_Begin(S2C_DIRECT_SHOP_DATA_RESP, MSG_S2C_DIRECT_SHOP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_ShopData.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ShopData.wGoodsNum)
Declare_EncodeMsg_Memory(m_ShopData.arGoodsList, m_ShopData.wGoodsNum*sizeof(ShopGoodsBuyRecord));
Declare_EncodeMsg_UnsignedShort(m_ShopData.wCanTakeGoodsNum)
Declare_EncodeMsg_Memory(m_ShopData.arCanTakeGoodsList, m_ShopData.wCanTakeGoodsNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DirectBuyShopData, m_ShopData)		// �̵���Ʒ����
Declare_Msg_End

// ��ȡֱ����Ʒ��������
Declare_Msg_Begin(C2S_TAKE_DIRECT_BUY_GOODS_REQ, MSG_C2S_TAKE_DIRECT_BUY_GOODS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)		// ��ȡ����ƷID
Declare_Msg_End

// ��ȡֱ����Ʒ��������
Declare_Msg_Begin(S2C_TAKE_DIRECT_BUY_GOODS_RESP, MSG_S2C_TAKE_DIRECT_BUY_GOODS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)		// ��ȡ����ƷID
Declare_Msg_End

// ˮ���̵���������
Declare_Msg_Begin(C2S_CRYSTAL_SHOP_DATA_REQ, MSG_C2S_CRYSTAL_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ˮ���̵�������Ӧ
Declare_Msg_Begin(S2C_CRYSTAL_SHOP_DATA_RESP, MSG_S2C_CRYSTAL_SHOP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_ShopData.wTodayRefreshTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ShopData.wTodayRefreshTimes)
Declare_EncodeMsg_UnsignedShort(m_ShopData.wGoodsNum)
for (WORD i = 0; i < m_ShopData.wGoodsNum; ++i)
{
	ShopGoodsBuyRecord &tBuyRecord = m_ShopData.arGoodsList[i];
	Declare_EncodeMsg_UnsignedShort(tBuyRecord.wGoodsId);
	Declare_EncodeMsg_UnsignedShort(tBuyRecord.wBuyNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(CrystalShopData, m_ShopData)		// �̵�����
Declare_Msg_End

// ����ɨ������
Declare_Msg_Begin(C2S_FB_CLEAN_REQ, MSG_C2S_FB_CLEAN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFBType)
//Declare_DecodeMsg_UnsignedChar(m_byCleanTimes)
Declare_DecodeMsg_UnsignedInt(m_dwParam1)
Declare_DecodeMsg_UnsignedInt(m_dwParam2)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFBType)
//Declare_EncodeMsg_UnsignedChar(m_byCleanTimes)
Declare_EncodeMsg_UnsignedInt(m_dwParam1)
Declare_EncodeMsg_UnsignedInt(m_dwParam2)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFBType)		// ��������
//Declare_Msg_Member(BYTE, m_byCleanTimes)	// ɨ������
Declare_Msg_Member(DWORD, m_dwParam1)		// ����1
Declare_Msg_Member(DWORD, m_dwParam2)		// ����2
Declare_Msg_End

// ����ɨ����Ӧ
Declare_Msg_Begin(S2C_FB_CLEAN_RESP, MSG_S2C_FB_CLEAN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFBType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFBType)
Declare_EncodeMsg_UnsignedInt(m_dwParam1)
Declare_EncodeMsg_UnsignedInt(m_dwParam2)
Declare_EncodeMsg_UnsignedChar(m_byResult)
Declare_EncodeMsg_UnsignedShort(m_vDropItemList.size());
for (size_t i = 0; i < m_vDropItemList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vDropItemList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_vDropItemList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFBType)		// ��������
Declare_Msg_Member(DWORD, m_dwParam1)		// ����1
Declare_Msg_Member(DWORD, m_dwParam2)		// ����2
Declare_Msg_Member(BYTE, m_byResult)		// ���(0=����, 1=�����Ѿ�, 2=Ӣ�۱�������, 3=װ����������)
Declare_Msg_Member(vector<Item>, m_vDropItemList)	// ������Ʒ�б�
Declare_Msg_End


// ���߸�����������
Declare_Msg_Begin(C2S_FB_MAIN_DATA_REQ, MSG_C2S_FB_MAIN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���߸���������Ӧ
Declare_Msg_Begin(S2C_FB_MAIN_DATA_RESP, MSG_S2C_FB_MAIN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_Data.wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wTakeChapterRewardRecord)
Declare_EncodeMsg_UnsignedShort(m_Data.wChapter)
for (WORD i = 0; i < m_Data.wChapter; ++i)
{
	ChapterData &sChapter = m_Data.arChapterList[i];
	Declare_EncodeMsg_UnsignedShort(sChapter.byMapIdx);
	Declare_EncodeMsg_Memory(sChapter.arMapList, sChapter.byMapIdx * sizeof(MapData));
	Declare_EncodeMsg_UnsignedShort(CHAPTER_STAR_AWARD_MAX);
	Declare_EncodeMsg_Memory(sChapter.arStarAwardList, CHAPTER_STAR_AWARD_MAX * sizeof(BYTE));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(MainFBData, m_Data)	// ��������
Declare_Msg_End



// �ճ�������������
Declare_Msg_Begin(C2S_FB_DAILY_DATA_REQ, MSG_C2S_FB_DAILY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// Ӣ�۸�����������
Declare_Msg_Begin(C2S_FB_HERO_DATA_REQ, MSG_C2S_FB_HERO_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// Ӣ�۸���������Ӧ
Declare_Msg_Begin(S2C_FB_HERO_DATA_RESP, MSG_S2C_FB_HERO_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_tFBData.byBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_tFBData.byBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_tFBData.wChapter)
for (WORD i = 0; i < m_tFBData.wChapter; ++i)
{
	HeroFubenChapterData &tChapterData = m_tFBData.arChapterList[i];
	Declare_EncodeMsg_UnsignedChar(tChapterData.byIsExtraMapPass);
	Declare_EncodeMsg_UnsignedShort(tChapterData.byMapIdx);
	Declare_EncodeMsg_Memory(tChapterData.arMapList, tChapterData.byMapIdx*sizeof(HeroMapData));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(HeroFBData, m_tFBData)	// ��������
Declare_Msg_End


// ��Ӣ������������
Declare_Msg_Begin(C2S_FB_ELITE_DATA_REQ, MSG_C2S_FB_ELITE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ӣ����������Ӧ
Declare_Msg_Begin(S2C_FB_ELITE_DATA_RESP, MSG_S2C_FB_ELITE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_Data.wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wChapter)
for (WORD i = 0; i < m_Data.wChapter; ++i)
{
	EliteFBChapter &sChapter = m_Data.arChapterList[i];
	Declare_EncodeMsg_UnsignedShort(sChapter.byMapIdx);
	Declare_EncodeMsg_Memory(sChapter.arMapList, sChapter.byMapIdx * sizeof(MapData));
	Declare_EncodeMsg_UnsignedShort(CHAPTER_STAR_AWARD_MAX);
	Declare_EncodeMsg_Memory(sChapter.arStarAwardList, CHAPTER_STAR_AWARD_MAX * sizeof(BYTE));
	Declare_EncodeMsg_UnsignedShort(sChapter.tEnemyData.wMapId);
	Declare_EncodeMsg_UnsignedChar(sChapter.tEnemyData.wIsDead);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(EliteFBData, m_Data)	// ��������
Declare_Msg_End

// �ճ�������������
Declare_Msg_Begin(C2S_TOWER_DATA_REQ, MSG_C2S_TOWER_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ճ�����������Ӧ
Declare_Msg_Begin(S2C_FB_DAILY_DATA_RESP, MSG_S2C_FB_DAILY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_tFBData.wChapterNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tFBData.wChapterNum)
for (WORD i = 0; i < m_tFBData.wChapterNum; ++i)
{
	DailyChapterData &tChapter = m_tFBData.arChapterList[i];
	Declare_EncodeMsg_UnsignedChar(tChapter.wChapter);
	Declare_EncodeMsg_Char(tChapter.nMapIdxRecord);
	Declare_EncodeMsg_UnsignedChar(tChapter.byBattleTimes);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DailyFBData, m_tFBData)	// �ճ���������
Declare_Msg_End

// ������������
Declare_Msg_Begin(S2C_TOWER_DATA_RESP, MSG_S2C_TOWER_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_FBData.wCurMapLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_FBData.wCurMapLevel)
Declare_EncodeMsg_Memory(m_FBData.arMapStarList, m_FBData.wCurMapLevel*sizeof(BYTE))
Declare_EncodeMsg_UnsignedChar(m_FBData.wCurMapLevel)
Declare_EncodeMsg_UnsignedChar(m_FBData.wMapLevelRecord)
Declare_EncodeMsg_UnsignedChar(m_FBData.byKeepThreeStarPassedRecord)
Declare_EncodeMsg_UnsignedShort(m_FBData.bySelectProNum)
Declare_EncodeMsg_Memory(m_FBData.arSelectProList, m_FBData.bySelectProNum*sizeof(Property))
Declare_EncodeMsg_UnsignedChar(m_FBData.byIsBuyStarGoods)
Declare_EncodeMsg_UnsignedChar(m_FBData.byStarGoodsIndex)
Declare_EncodeMsg_UnsignedChar(m_FBData.byIsNeedReset)
Declare_EncodeMsg_UnsignedChar(m_FBData.byTodayResetTimes)
Declare_EncodeMsg_UnsignedShort(m_FBData.wCurStar)
Declare_EncodeMsg_UnsignedShort(m_FBData.wStarMaxRecord)
Declare_EncodeMsg_UnsignedShort(m_FBData.wEliteMapLevel)
Declare_EncodeMsg_UnsignedChar(m_FBData.byEliteMapBattleTimes)
Declare_EncodeMsg_UnsignedChar(m_FBData.byBuyEliteMapBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_FBData.byRandProNum)
Declare_EncodeMsg_Memory(m_FBData.arRandProIdxList, m_FBData.byRandProNum*sizeof(BYTE))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(TowerFBData, m_FBData)		// ��������
Declare_Msg_Member(BYTE, m_byRandProNum)		// �����������
Declare_Msg_End

// ������ս��������
Declare_Msg_Begin(C2S_TOWER_BUY_TIMES_REQ, MSG_C2S_TOWER_BUY_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������ս������Ӧ
Declare_Msg_Begin(S2C_TOWER_BUY_TIMES_RESP, MSG_S2C_TOWER_BUY_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���ùؿ�
Declare_Msg_Begin(C2S_TOWER_RESET_REQ, MSG_C2S_TOWER_RESET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���ùؿ���Ӧ
Declare_Msg_Begin(S2C_TOWER_RESET_RESP, MSG_S2C_TOWER_RESET_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ѡ������
Declare_Msg_Begin(C2S_TOWER_SELECT_PRO_REQ, MSG_C2S_TOWER_SELECT_PRO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedChar(m_byProIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStar)
Declare_EncodeMsg_UnsignedChar(m_byProIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStar)			// ����
Declare_Msg_Member(BYTE, m_byProIndex)		// ��������
Declare_Msg_End


// ѡ��������Ӧ
Declare_Msg_Begin(S2C_TOWER_SELECT_PRO_RESP, MSG_S2C_TOWER_SELECT_PRO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedChar(m_byProIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStar)
Declare_EncodeMsg_UnsignedChar(m_byProIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStar)			// ����
Declare_Msg_Member(BYTE, m_byProIndex)		// ��������
Declare_Msg_End

// ����������Ʒ
Declare_Msg_Begin(C2S_TOWER_BUY_STAR_GOODS_REQ, MSG_C2S_TOWER_BUY_STAR_GOODS_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������Ʒ��Ӧ
Declare_Msg_Begin(S2C_TOWER_BUY_STAR_GOODS_RESP, MSG_S2C_TOWER_BUY_STAR_GOODS_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


Declare_Msg_Begin(C2S_TOWER_ONE_KEY_CLEAN_REQ, MSG_C2S_TOWER_ONE_KEY_CLEAN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsToHistoryMap)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsToHistoryMap)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsToHistoryMap)		// �Ƿ�ɨ������ʷ�ؿ�
Declare_Msg_End

Declare_Msg_Begin(S2C_TOWER_ONE_KEY_CLEAN_RESP, MSG_S2C_TOWER_ONE_KEY_CLEAN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsToHistoryMap)
Declare_DecodeMsg_UnsignedChar(m_byBegin)
Declare_DecodeMsg_UnsignedChar(m_byEnd)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsToHistoryMap)
Declare_EncodeMsg_UnsignedChar(m_byBegin)
Declare_EncodeMsg_UnsignedChar(m_byEnd)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsToHistoryMap)		// �Ƿ�ɨ������ʷ�ؿ�
Declare_Msg_Member(BYTE, m_byBegin)		// ��ʼ
Declare_Msg_Member(BYTE, m_byEnd)		// ����
Declare_Msg_End

// ����������ȡ
Declare_Msg_Begin(C2S_FB_AWARD_GET_REQ, MSG_C2S_FB_AWARD_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byAwardType)
Declare_DecodeMsg_UnsignedChar(m_byFBType)
Declare_DecodeMsg_UnsignedShort(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byAwardType)
Declare_EncodeMsg_UnsignedChar(m_byFBType)
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byAwardType)		// ��������
Declare_Msg_Member(BYTE, m_byFBType)		// ��������
Declare_Msg_Member(WORD, m_wChapter)		// �½�
Declare_Msg_Member(BYTE, m_byIndex)			// ����
Declare_Msg_End

// ����������ȡ��Ӧ
Declare_Msg_Begin(S2C_FB_AWARD_GET_RESP, MSG_S2C_FB_AWARD_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byAwardType)
Declare_DecodeMsg_UnsignedChar(m_byFBType)
Declare_DecodeMsg_UnsignedShort(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byAwardType)
Declare_EncodeMsg_UnsignedChar(m_byFBType)
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byAwardType)		// ��������
Declare_Msg_Member(BYTE, m_byFBType)		// ��������
Declare_Msg_Member(WORD, m_wChapter)		// �½�
Declare_Msg_Member(BYTE, m_byIndex)			// ����
Declare_Msg_End


// ������������
 Declare_Msg_Begin(C2S_FB_RESET_REQ, MSG_C2S_FB_RESET_REQ)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byMapType)
 Declare_DecodeMsg_UnsignedInt(m_dwParam1)
 Declare_DecodeMsg_UnsignedInt(m_dwParam2)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byMapType)
 Declare_EncodeMsg_UnsignedInt(m_dwParam1)
 Declare_EncodeMsg_UnsignedInt(m_dwParam2)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byMapType)		// �ؿ�����
 Declare_Msg_Member(DWORD, m_dwParam1)		// ����1
 Declare_Msg_Member(DWORD, m_dwParam2)		// ����2
 Declare_Msg_End

 // ����������Ӧ
 Declare_Msg_Begin(S2C_FB_RESET_RESP, MSG_S2C_FB_RESET_RESP)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byMapType)
 Declare_DecodeMsg_UnsignedInt(m_dwParam1)
 Declare_DecodeMsg_UnsignedInt(m_dwParam2)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byMapType)
 Declare_EncodeMsg_UnsignedInt(m_dwParam1)
 Declare_EncodeMsg_UnsignedInt(m_dwParam2)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byMapType)		// �ؿ�����
 Declare_Msg_Member(DWORD, m_dwParam1)		// ����1
 Declare_Msg_Member(DWORD, m_dwParam2)		// ����2
 Declare_Msg_End

 // һ����ȡ��������(�ؿ�+����)
 Declare_Msg_Begin(C2S_FB_ONE_KEY_TAKE_REWARD_REQ, MSG_C2S_FB_ONE_KEY_TAKE_REWARD_REQ)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byFBType)
 Declare_DecodeMsg_UnsignedShort(m_wChapter)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byFBType)
 Declare_EncodeMsg_UnsignedShort(m_wChapter)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byFBType)		// ��������
 Declare_Msg_Member(WORD, m_wChapter)		// �ؿ�����
 Declare_Msg_End

 // һ����ȡ��������(�ؿ�+����)
 Declare_Msg_Begin(S2C_FB_ONE_KEY_TAKE_REWARD_RESP, MSG_S2C_FB_ONE_KEY_TAKE_REWARD_RESP)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byFBType)
 Declare_DecodeMsg_UnsignedShort(m_wChapter)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byFBType)
 Declare_EncodeMsg_UnsignedShort(m_wChapter)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byFBType)		// ��������
 Declare_Msg_Member(WORD, m_wChapter)		// �ؿ�����
 Declare_Msg_End

// ��ȡ���߸����½ڽ�������
 Declare_Msg_Begin(C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ, MSG_C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(WORD, m_wRewardLevel)		// �����ȼ�
 Declare_Msg_End

 // ��ȡ���߸����½ڽ�����Ӧ
 Declare_Msg_Begin(S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP, MSG_S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_EncodeMsg_UnsignedShort(m_vItemList.size())
 for (size_t i = 0; i < m_vItemList.size(); ++i)
 {
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwId)
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwNum)
 }
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(WORD, m_wRewardLevel)		// �����ȼ�
 Declare_Msg_Member(vector<Item>, m_vItemList)	// ��Ʒ�б�
 Declare_Msg_End

// ��Ʒ�б�����
Declare_Msg_Begin(C2S_ITEM_LIST_REQ, MSG_C2S_ITEM_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ʒ�б���Ӧ
Declare_Msg_Begin(S2C_ITEM_LIST_RESP, MSG_S2C_ITEM_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
m_wItemNum = min(m_wItemNum, MAX_ITEM_NUM);
 Declare_DecodeMsg_Memory(m_ItemList, m_wItemNum * sizeof(Item))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wItemNum = min(m_wItemNum, MAX_ITEM_NUM);
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
Declare_EncodeMsg_Memory(m_ItemList, m_wItemNum * sizeof(Item))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemNum)	// ��Ʒ����
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ITEM_NUM);	// ��Ʒ�б�
Declare_Msg_End

// ��Ʒʹ������
Declare_Msg_Begin(C2S_ITEM_USE_REQ, MSG_C2S_ITEM_USE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwItemId)
Declare_DecodeMsg_UnsignedShort(m_wUseNum)
Declare_DecodeMsg_UnsignedInt(m_dwGiftItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwItemId)
Declare_EncodeMsg_UnsignedShort(m_wUseNum)
Declare_EncodeMsg_UnsignedInt(m_dwGiftItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwItemId)		// ʹ�õ���ƷID
Declare_Msg_Member(WORD, m_wUseNum)			// ʹ�õ�����
Declare_Msg_Member(DWORD, m_dwGiftItemId)	// ѡ��ʹ�õ������ƷID
Declare_Msg_End

// ��Ʒʹ����Ӧ
Declare_Msg_Begin(S2C_ITEM_USE_RESP, MSG_S2C_ITEM_USE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwItemId)
Declare_DecodeMsg_UnsignedShort(m_wUseNum)
Declare_DecodeMsg_UnsignedShort(m_byItemNum)
Declare_DecodeMsg_Memory(m_ItemList, m_byItemNum*sizeof(Item));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwItemId)
Declare_EncodeMsg_UnsignedShort(m_wUseNum)
Declare_EncodeMsg_UnsignedShort(m_byItemNum)
Declare_EncodeMsg_Memory(m_ItemList, m_byItemNum*sizeof(Item));
Declare_EncodeMsg_UnsignedShort(m_byMoneyNum)
Declare_EncodeMsg_Memory(m_MoneyList, m_byMoneyNum*sizeof(MoneyData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwItemId)	// ʹ�õ���ƷID
Declare_Msg_Member(WORD, m_wUseNum)		// ʹ�õ�����
Declare_Msg_Member(BYTE, m_byItemNum)	// ��Ʒ����
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ITEM_NUM);	// ��Ʒ�б�
Declare_Msg_Member(BYTE, m_byMoneyNum)	// ��������
Declare_Msg_Member_Array(MoneyData, m_MoneyList, MAX_MONEY_TYPE_NUM);	// �����б�
Declare_Msg_End

// ��Ʒ��������
Declare_Msg_Begin(C2S_ITEM_FP_REQ, MSG_C2S_ITEM_FP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// ��������
Declare_Msg_End

// ��Ʒ������Ӧ
Declare_Msg_Begin(S2C_ITEM_FP_RESP, MSG_S2C_ITEM_FP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwFPItemId)
Declare_DecodeMsg_UnsignedInt(m_tCreateItem.dwId)
Declare_DecodeMsg_UnsignedInt(m_tCreateItem.dwNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwFPItemId)
Declare_EncodeMsg_UnsignedInt(m_tCreateItem.dwId)
Declare_EncodeMsg_UnsignedInt(m_tCreateItem.dwNum)
Declare_EncodeMsg_UnsignedChar(m_tMoneyData.byType)
Declare_EncodeMsg_UnsignedInt(m_tMoneyData.nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// ��������
Declare_Msg_Member(DWORD, m_dwFPItemId)		// ���Ƶ���ƷID
Declare_Msg_Member(Item, m_tCreateItem)		// �����������Ʒ
Declare_Msg_Member(MoneyData, m_tMoneyData)	// �����Ļ���
Declare_Msg_End


// ��Ʒ�ϳ�����
Declare_Msg_Begin(C2S_ITEM_COMPOSE_REQ, MSG_C2S_ITEM_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// �ϳɵ���ƷID
Declare_Msg_End

// ��Ʒ�ϳ���Ӧ
Declare_Msg_Begin(S2C_ITEM_COMPOSE_RESP, MSG_S2C_ITEM_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// �ϳɵ���ƷID
Declare_Msg_End

// ��Ʒ�ֽ�����
Declare_Msg_Begin(C2S_ITEM_DECOMPOSE_REQ, MSG_C2S_ITEM_DECOMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// �ֽ����ƷID
Declare_Msg_Member(WORD, m_wItemNum)	// �ֽ����Ʒ����
Declare_Msg_End

// ��Ʒ�ֽ���Ӧ
Declare_Msg_Begin(S2C_ITEM_DECOMPOSE_RESP, MSG_S2C_ITEM_DECOMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// �ֽ����ƷID
Declare_Msg_Member(WORD, m_wItemNum)	// �ֽ����Ʒ����
Declare_Msg_End

// ��Ʒ����
Declare_Msg_Begin(S2C_ITEM_DATA_NOTIFY, MSG_S2C_ITEM_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwItemId)
Declare_DecodeMsg_UnsignedInt(m_dwUseNum)
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwItemId)
Declare_EncodeMsg_UnsignedInt(m_dwUseNum)
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwItemId)	// ��ƷID
Declare_Msg_Member(DWORD, m_dwUseNum)		// ��Ʒ����
Declare_Msg_Member(BYTE, m_byFlag)		// ������־(0=����,1=���)
Declare_Msg_End


// ����Ӣ�ۻ�����Ϣ�б�
Declare_Msg_Begin(C2S_HERO_BASE_LIST_REQ, MSG_C2S_HERO_BASE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����Ӣ�ۻ�����Ϣ�б���Ӧ
Declare_Msg_Begin(S2C_HERO_BASE_LIST_RESP, MSG_S2C_HERO_BASE_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wHeroBaseNum)
m_wHeroBaseNum = min(m_wHeroBaseNum, MAX_HERO_BAG_NUM);
Declare_DecodeMsg_Memory(m_HeroBaseList, m_wHeroBaseNum * sizeof(ClientHeroBaseData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wHeroBaseNum = min(m_wHeroBaseNum, MAX_HERO_BAG_NUM);
Declare_EncodeMsg_UnsignedShort(m_wHeroBaseNum)
for (WORD i = 0; i < m_wHeroBaseNum; ++i)
{
	ClientHeroBaseData &tBaseData = m_HeroBaseList[i];
	Declare_EncodeMsg_UnsignedInt(tBaseData.dwUID)
	Declare_EncodeMsg_UnsignedShort(tBaseData.wInfoId)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byQuality)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byQualityStage)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byDestinyLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byAwakenStar)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byAwakenLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byBreakLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byIsToBattle)
	Declare_EncodeMsg_UnsignedInt(tBaseData.dwExp)
	Declare_EncodeMsg_UnsignedShort(HERO_TRAIN_PRO_NUM)
	for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
	{
		Declare_EncodeMsg_UnsignedChar(tBaseData.arTrainProList[k].wType);
		Declare_EncodeMsg_UnsignedChar(tBaseData.arTrainProLvList[k]);
		Declare_EncodeMsg_UnsignedInt(tBaseData.arTrainProList[k].nValue);
	}
	Declare_EncodeMsg_UnsignedShort(tBaseData.wAwakenItemNum);
	Declare_EncodeMsg_Memory(tBaseData.arAwakenItemList, tBaseData.wAwakenItemNum * sizeof(WORD));
}
//Declare_EncodeMsg_Memory(m_HeroBaseList, m_wHeroBaseNum * sizeof(ClientHeroBaseData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wHeroBaseNum)	// Ӣ������
Declare_Msg_Member_Array(ClientHeroBaseData, m_HeroBaseList, MAX_HERO_BAG_NUM);	// Ӣ���б�
Declare_Msg_End

// ����Ӣ����ϸ��Ϣ
Declare_Msg_Begin(C2S_HERO_INFO_REQ, MSG_C2S_HERO_INFO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ��ϸID
Declare_Msg_End

// ����Ӣ����ϸ��Ϣ��Ӧ
Declare_Msg_Begin(S2C_HERO_INFO_RESP, MSG_S2C_HERO_INFO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_Info.dwUID)
Declare_DecodeMsg_UnsignedShort(m_Info.dwIntoId)
Declare_DecodeMsg_UnsignedChar(m_Info.wLevel)
Declare_DecodeMsg_UnsignedChar(m_Info.wQuality)
Declare_DecodeMsg_UnsignedChar(m_Info.wQualityStage)
Declare_DecodeMsg_UnsignedChar(m_Info.wBreakLevel)
Declare_DecodeMsg_UnsignedChar(m_Info.wDestinyLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_Info.dwUID)
Declare_EncodeMsg_UnsignedShort(m_Info.dwIntoId)
Declare_EncodeMsg_UnsignedChar(m_Info.wLevel)
Declare_EncodeMsg_UnsignedChar(m_Info.wQuality)
Declare_EncodeMsg_UnsignedChar(m_Info.wQualityStage)
Declare_EncodeMsg_UnsignedChar(m_Info.wBreakLevel)
Declare_EncodeMsg_UnsignedChar(m_Info.wDestinyLevel)
Declare_EncodeMsg_UnsignedInt(m_Info.dwDestinyExp)
Declare_EncodeMsg_UnsignedChar(m_Info.wAwakenStar)
Declare_EncodeMsg_UnsignedChar(m_Info.wAwakenLevel)
Declare_EncodeMsg_UnsignedShort(m_Info.wAwakenItemNum)
Declare_EncodeMsg_Memory(m_Info.arAwakenItemList, m_Info.wAwakenItemNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(HERO_TRAIN_PRO_NUM)
for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
{
	Declare_EncodeMsg_UnsignedChar(i + 1);							// ��������
	Declare_EncodeMsg_UnsignedChar(m_Info.arTrainProLvList[i]);		// ���Եȼ�
	Declare_EncodeMsg_UnsignedInt(m_Info.arTrainProValueList[i]);		// ����ֵ
	Declare_EncodeMsg_UnsignedInt(m_Info.arLastTrainTmpProList[i]);	// ������ʱֵ
	Declare_EncodeMsg_UnsignedChar(m_Info.arTrainRewardFlag[i]);		// ������־
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ClientHero, m_Info)	// ��ϸ��Ϣ
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_HERO_AWAKEN_REQ, MSG_C2S_HERO_AWAKEN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedShort(m_wCostHeroNum)
m_wCostHeroNum = min(m_wCostHeroNum, MAX_HERO_AWAKEN_COST_HERO_NUM);
Declare_DecodeMsg_Memory(m_CollectionList, m_wCostHeroNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ���ѵ�Ӣ��
Declare_Msg_Member(WORD, m_wCostHeroNum)	// ���ĵ�Ӣ������
Declare_Msg_Member_Array(DWORD, m_CollectionList, MAX_HERO_AWAKEN_COST_HERO_NUM);	// ��Ϊ���ĵ�Ӣ��
Declare_Msg_End


// ������Ӧ
Declare_Msg_Begin(S2C_HERO_AWAKEN_RESP, MSG_S2C_HERO_AWAKEN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byAwakenStar)
Declare_EncodeMsg_UnsignedChar(m_byAwakenLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// ���ѵ�Ӣ��
Declare_Msg_Member(BYTE, m_byAwakenStar)	// �����Ǽ�
Declare_Msg_Member(BYTE, m_byAwakenLevel)	// ���ѵȼ�
Declare_Msg_End

// װ�����ѵ�������
Declare_Msg_Begin(C2S_HERO_EQUIP_AWAKEN_ITEM_REQ, MSG_C2S_HERO_EQUIP_AWAKEN_ITEM_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// ���ѵ�Ӣ��
Declare_Msg_Member(WORD, m_wAwakenItemId)	// װ���ľ��ѵ���
Declare_Msg_End

// װ�����ѵ�����Ӧ
Declare_Msg_Begin(S2C_HERO_EQUIP_AWAKEN_ITEM_RESP, MSG_S2C_HERO_EQUIP_AWAKEN_ITEM_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// ���ѵ�Ӣ��
Declare_Msg_Member(WORD, m_wAwakenItemId)	// װ���ľ��ѵ���
Declare_Msg_End

// ����Ӣ������
Declare_Msg_Begin(C2S_HERO_TO_BATTLE_REQ, MSG_C2S_HERO_TO_BATTLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ��ϸID
Declare_Msg_Member(BYTE, m_byPos)	// ����λ��
Declare_Msg_End

// ����Ӣ��������Ӧ
Declare_Msg_Begin(S2C_HERO_TO_BATTLE_RESP, MSG_S2C_HERO_TO_BATTLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ��ϸID
Declare_Msg_Member(BYTE, m_byPos)	// ����λ��
Declare_Msg_End

// ����Ӣ���ռ��б�
Declare_Msg_Begin(C2S_HERO_COLLECTION_LIST_REQ, MSG_C2S_HERO_COLLECTION_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����Ӣ���б���Ӧ
Declare_Msg_Begin(S2C_HERO_COLLECTION_LIST_RESP, MSG_S2C_HERO_COLLECTION_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byCollectionNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byCollectionNum)
Declare_EncodeMsg_Memory(m_CollectionList, m_byCollectionNum * sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCollectionNum)							// Ӣ���ռ�����
Declare_Msg_Member_Array(WORD, m_CollectionList, MAX_HERO_NUM);	// Ӣ���ռ��б�
Declare_Msg_End


// ͻ������
Declare_Msg_Begin(C2S_HERO_BREAK_REQ, MSG_C2S_HERO_BREAK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
// ͬ��Ӣ��
Declare_DecodeMsg_UnsignedShort(m_wCostSameNameHeroNum)
m_wCostSameNameHeroNum = min(m_wCostSameNameHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameNameHeroList, m_wCostSameNameHeroNum * sizeof(DWORD));
// ͬƷ��Ӣ��
Declare_DecodeMsg_UnsignedShort(m_wCostSameQualityHeroNum)
m_wCostSameQualityHeroNum = min(m_wCostSameQualityHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameQualityHeroList, m_wCostSameQualityHeroNum * sizeof(DWORD));
// ְͬҵӢ��
Declare_DecodeMsg_UnsignedShort(m_wCostSameProfessionHeroNum)
m_wCostSameProfessionHeroNum = min(m_wCostSameProfessionHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameProfessionHeroList, m_wCostSameProfessionHeroNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
// ͬ��Ӣ��
Declare_EncodeMsg_UnsignedShort(m_wCostSameNameHeroNum)
m_wCostSameNameHeroNum = min(m_wCostSameNameHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameNameHeroList, m_wCostSameNameHeroNum * sizeof(DWORD));
// ͬƷ��Ӣ��
Declare_EncodeMsg_UnsignedShort(m_wCostSameQualityHeroNum)
m_wCostSameQualityHeroNum = min(m_wCostSameQualityHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameQualityHeroList, m_wCostSameQualityHeroNum * sizeof(DWORD));
// ְͬҵӢ��
Declare_EncodeMsg_UnsignedShort(m_wCostSameProfessionHeroNum)
m_wCostSameProfessionHeroNum = min(m_wCostSameProfessionHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameProfessionHeroList, m_wCostSameProfessionHeroNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)	// UID
Declare_Msg_Member_Array(DWORD, m_arCostSameNameHeroList, MAX_HERO_NUM);		// ͬ��Ӣ���б�
Declare_Msg_Member(WORD, m_wCostSameNameHeroNum)								// ͬ��Ӣ������
Declare_Msg_Member_Array(DWORD, m_arCostSameQualityHeroList, MAX_HERO_NUM);		// ͬƷ��Ӣ���б� 
Declare_Msg_Member(WORD, m_wCostSameQualityHeroNum)								// ͬƷ��Ӣ������
Declare_Msg_Member_Array(DWORD, m_arCostSameProfessionHeroList, MAX_HERO_NUM);	// ְͬҵӢ���б�
Declare_Msg_Member(WORD, m_wCostSameProfessionHeroNum)							// ְͬҵӢ������
Declare_Msg_End


// ͻ������
Declare_Msg_Begin(S2C_HERO_BREAK_RESP, MSG_S2C_HERO_BREAK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)	// UID
Declare_Msg_End

// Ӣ�ۺϳ�����
Declare_Msg_Begin(C2S_HERO_COMPOSE_REQ, MSG_C2S_HERO_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wHeroInfoId)	// Ӣ������ID
Declare_Msg_End

// Ӣ�ۺϳ���Ӧ
Declare_Msg_Begin(S2C_HERO_COMPOSE_RESP, MSG_S2C_HERO_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wHeroInfoId)	// Ӣ������ID
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_HERO_TRAIN_REQ, MSG_C2S_HERO_TRAIN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_UnsignedChar(m_byCostType)
Declare_DecodeMsg_UnsignedChar(m_byTrainTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_UnsignedChar(m_byCostType)
Declare_EncodeMsg_UnsignedChar(m_byTrainTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_Member(BYTE, m_byCostType)		// �������� (1=����(��Ʒ), 2=����(��Ʒ+��ʯ))
Declare_Msg_Member(BYTE, m_byTrainTimes)	// ��������
Declare_Msg_End

//////////////////////////////////////////////// todo::���˴���.

// ������Ӧ
Declare_Msg_Begin(S2C_HERO_TRAIN_RESP, MSG_S2C_HERO_TRAIN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_UnsignedShort(m_byProNum)
Declare_EncodeMsg_Memory(m_ProList, m_byProNum*sizeof(Property));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_Member_Array(Property, m_ProList, MAX_HERO_TRAIN_PROP_MUM);	// Ӣ�����������б�
Declare_Msg_Member(BYTE, m_byProNum)	// ��������
Declare_Msg_End

// Ӣ�������滻����
Declare_Msg_Begin(C2S_HERO_TRAIN_REPLACE_REQ, MSG_C2S_HERO_TRAIN_REPLACE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// Ӣ�������滻��Ӧ
Declare_Msg_Begin(S2C_HERO_TRAIN_REPLACE_RESP, MSG_S2C_HERO_TRAIN_REPLACE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// Ӣ������������ȡ����
Declare_Msg_Begin(C2S_HERO_TRAIN_REWARD_GET_REQ, MSG_C2S_HERO_TRAIN_REWARD_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_UnsignedChar(m_byProType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_UnsignedChar(m_byProType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_Member(BYTE, m_byProType)		// ��������
Declare_Msg_End

// Ӣ������������ȡ��Ӧ
Declare_Msg_Begin(S2C_HERO_TRAIN_REWARD_GET_RESP, MSG_S2C_HERO_TRAIN_REWARD_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_UnsignedChar(m_byProType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_UnsignedChar(m_byProType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_Member(BYTE, m_byProType)		// ��������
Declare_Msg_End


// Ӣ�۽�������
Declare_Msg_Begin(C2S_HERO_EVOLUTION_REQ, MSG_C2S_HERO_EVOLUTION_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// Ӣ�۽�����Ӧ
Declare_Msg_Begin(S2C_HERO_EVOLUTION_RESP, MSG_S2C_HERO_EVOLUTION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_UnsignedShort(m_wQuality)
Declare_DecodeMsg_UnsignedShort(m_wStage)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_UnsignedShort(m_wQuality)
Declare_EncodeMsg_UnsignedShort(m_wStage)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_Member(WORD, m_wQuality)		// Ʒ��
Declare_Msg_Member(WORD, m_wStage)		// Ʒ��
Declare_Msg_End


// VIP�ٻ�����
Declare_Msg_Begin(C2S_HERO_VIP_SUMMON_REQ, MSG_C2S_HERO_VIP_SUMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_dwCamp)		// ��Ӫ
Declare_Msg_End


// VIP�ٻ���Ӧ
Declare_Msg_Begin(S2C_HERO_VIP_SUMMON_RESP, MSG_S2C_HERO_VIP_SUMMON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwCamp)
Declare_EncodeMsg_UnsignedShort(m_vHeroList.size())
for (int i = 0; i < m_vHeroList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vHeroList[i].dwId);
	Declare_EncodeMsg_UnsignedShort(m_vHeroList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwCamp)	// ��Ӫ
Declare_Msg_Member(vector<Item>, m_vHeroList)	// ��õ�Ӣ���б�
Declare_Msg_End


// Ӣ�ۻ�����Ϣ֪ͨ
// Declare_Msg_Begin(S2C_HERO_BASE_INFO_NOTIFY, MSG_S2C_HERO_BASE_INFO_NOTIFY)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedChar(m_byFlag)
// Declare_DecodeMsg_Memory(m_BaseData, sizeof(ClientHeroBaseData))
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedChar(m_byFlag)
// Declare_EncodeMsg_UnsignedInt(m_BaseData.dwUID)
// Declare_EncodeMsg_UnsignedShort(m_BaseData.wInfoId)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byLevel)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byQuality)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byDestinyLevel)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byAwakenStar)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byAwakenLevel)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byBreakLevel)
// Declare_EncodeMsg_UnsignedChar(m_BaseData.byIsToBattle)
// Declare_EncodeMsg_UnsignedInt(m_BaseData.dwExp)
// Declare_EncodeMsg_UnsignedShort(HERO_TRAIN_PRO_NUM)
// for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
// {
// 	Declare_EncodeMsg_UnsignedChar(m_BaseData.arTrainProList[i].wType);
// 	Declare_EncodeMsg_UnsignedChar(m_BaseData.arTrainProLvList[i]);
// 	Declare_EncodeMsg_UnsignedInt(m_BaseData.arTrainProList[i].nValue);
// }
// //Declare_EncodeMsg_Memory(m_BaseData, sizeof(ClientHeroBaseData))
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(BYTE, m_byFlag)						// ���ݱ�־(1=��ӣ����£�0=ɾ��)
// Declare_Msg_Member(ClientHeroBaseData, m_BaseData)		// Ӣ�ۻ�������
// Declare_Msg_End

// Ӣ�ۻ�����Ϣ֪ͨ
Declare_Msg_Begin(S2C_HERO_BASE_INFO_NOTIFY, MSG_S2C_HERO_BASE_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_UnsignedShort(m_wHeroNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_UnsignedShort(m_wHeroNum)
for (WORD i = 0; i < m_wHeroNum; ++i)
{
	ClientHeroBaseData &tBaseData = m_HeroBaseDataList[i];
	Declare_EncodeMsg_UnsignedInt(tBaseData.dwUID)
	Declare_EncodeMsg_UnsignedShort(tBaseData.wInfoId)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byQuality)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byQualityStage)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byDestinyLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byAwakenStar)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byAwakenLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byBreakLevel)
	Declare_EncodeMsg_UnsignedChar(tBaseData.byIsToBattle)
	Declare_EncodeMsg_UnsignedInt(tBaseData.dwExp)
	Declare_EncodeMsg_UnsignedShort(HERO_TRAIN_PRO_NUM)
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		Declare_EncodeMsg_UnsignedChar(tBaseData.arTrainProList[i].wType);
		Declare_EncodeMsg_UnsignedChar(tBaseData.arTrainProLvList[i]);
		Declare_EncodeMsg_UnsignedInt(tBaseData.arTrainProList[i].nValue);
	}
	Declare_EncodeMsg_UnsignedShort(tBaseData.wAwakenItemNum);
	Declare_EncodeMsg_Memory(tBaseData.arAwakenItemList, tBaseData.wAwakenItemNum*sizeof(WORD));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag)	// ���ݱ�־(1=��ӣ����£�0=ɾ��)
Declare_Msg_Member(WORD, m_wHeroNum)// Ӣ������
Declare_Msg_Member_Array(ClientHeroBaseData, m_HeroBaseDataList, MAX_HERO_BAG_NUM) // Ӣ�ۻ��������б�
Declare_Msg_End


// Ӣ����������
Declare_Msg_Begin(C2S_HERO_LEVEL_UP_REQ, MSG_C2S_HERO_LEVEL_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_UnsignedShort(m_wCostHeroNum)
m_wCostHeroNum = min(m_wCostHeroNum, MAX_HERO_BAG_NUM);
Declare_DecodeMsg_Memory(m_arCostHeroList, m_wCostHeroNum*sizeof(DWORD))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)					// UID
Declare_Msg_Member(WORD, m_wCostHeroNum)				// ���ĵ�Ӣ������
Declare_Msg_Member_Array(DWORD, m_arCostHeroList, MAX_HERO_BAG_NUM)	// ���ĵ�Ӣ���б�
Declare_Msg_End

// Ӣ��������Ӧ
Declare_Msg_Begin(S2C_HERO_LEVEL_UP_RESP, MSG_S2C_HERO_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End


// �ٻ�����
Declare_Msg_Begin(C2S_HERO_SUMMON_REQ, MSG_C2S_HERO_SUMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �ٻ�����
Declare_Msg_End


// �ٻ���Ӧ
Declare_Msg_Begin(S2C_HERO_SUMMON_RESP, MSG_S2C_HERO_SUMMON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
// Declare_DecodeMsg_UnsignedChar(m_byHeroNum)
// Declare_DecodeMsg_Memory(m_HeroList, m_byHeroNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_byHeroNum)
Declare_EncodeMsg_Memory(m_HeroList, m_byHeroNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// �ٻ�����
Declare_Msg_Member(BYTE, m_byHeroNum)		// �ٻ���õ�Ӣ������
Declare_Msg_Member_Array(WORD, m_HeroList, HERO_ONE_KEY_SUMMON_TIMES);	// �ٻ���õ�Ӣ���б�
Declare_Msg_End

// ����(Ǳ��)����
Declare_Msg_Begin(C2S_HERO_DESTINY_REQ, MSG_C2S_HERO_DESTINY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// Ӣ��UID
Declare_Msg_End

// ����(Ǳ��)��Ӧ
Declare_Msg_Begin(S2C_HERO_DESTINY_RESP, MSG_S2C_HERO_DESTINY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_UnsignedChar(m_byDestinyLevel)
Declare_EncodeMsg_UnsignedInt(m_dwDestinyExp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// Ӣ��UID
Declare_Msg_Member(BYTE, m_byDestinyLevel)	// �����ȼ�
Declare_Msg_Member(DWORD, m_dwDestinyExp)	// ��������
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_HERO_SELL_REQ, MSG_C2S_HERO_SELL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// Ӣ��UID
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_HERO_SELL_RESP, MSG_S2C_HERO_SELL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// Ӣ��UID
Declare_Msg_End

// Ԯ������
Declare_Msg_Begin(C2S_REINFORCEMENT_TO_BATTLE_REQ, MSG_C2S_REINFORCEMENT_TO_BATTLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ��ϸID
Declare_Msg_Member(BYTE, m_byPos)	// ����λ��
Declare_Msg_End

// Ԯ��������Ӧ
Declare_Msg_Begin(S2C_REINFORCEMENT_TO_BATTLE_RESP, MSG_S2C_REINFORCEMENT_TO_BATTLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// ��ϸID
Declare_Msg_Member(BYTE, m_byPos)	// ����λ��
Declare_Msg_End

// �ٻ���������
Declare_Msg_Begin(C2S_HERO_SUMMON_DATA_REQ, MSG_C2S_HERO_SUMMON_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ٻ���������
Declare_Msg_Begin(S2C_HERO_SUMMON_DATA_RESP, MSG_S2C_HERO_SUMMON_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byNormalFreeTimes)
Declare_DecodeMsg_UnsignedInt(m_dwNormalFreeTime)
Declare_DecodeMsg_UnsignedChar(m_byAdvancedTimes)
Declare_DecodeMsg_UnsignedInt(m_dwAdvancedFreeTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byNormalFreeTimes)
Declare_EncodeMsg_UnsignedInt(m_dwNormalFreeTime)
Declare_EncodeMsg_UnsignedChar(m_byAdvancedTimes)
Declare_EncodeMsg_UnsignedInt(m_dwAdvancedFreeTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byNormalFreeTimes)			// ��ͨ����ٻ�����
Declare_Msg_Member(DWORD, m_dwNormalFreeTime)			// ��ͨ����ٻ�ʱ��
Declare_Msg_Member(DWORD, m_dwAdvancedFreeTime)			// �߼�����ٻ�ʱ��
Declare_Msg_Member(BYTE, m_byAdvancedTimes)				// �߼��ٻ�����
Declare_Msg_End


// �������б�
Declare_Msg_Begin(C2S_TREASURE_LIST_REQ, MSG_C2S_TREASURE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �������б���Ӧ
Declare_Msg_Begin(S2C_TREASURE_LIST_RESP, MSG_S2C_TREASURE_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wTreasureNum = min(m_wTreasureNum, MAX_TREASURE_NUM);
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Memory(m_TreasureList, m_wTreasureNum * sizeof(TreasureBaseData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureNum)	// ��������
Declare_Msg_Member_Array(TreasureBaseData, m_TreasureList, MAX_TREASURE_NUM);	// �����б�
Declare_Msg_End


// ���������Ϣ֪ͨ
Declare_Msg_Begin(S2C_TREASURE_INFO_NOTIFY, MSG_S2C_TREASURE_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Memory(m_sBaseData, sizeof(TreasureBaseData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Memory(m_sBaseData, sizeof(TreasureBaseData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag) // 1=���£���ӣ� 0=ɾ��
Declare_Msg_Member(TreasureBaseData, m_sBaseData)	// ���������Ϣ
Declare_Msg_End

// ����ϳ�����
Declare_Msg_Begin(C2S_TREASURE_COMPOSE_REQ, MSG_C2S_TREASURE_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)	// ������ϢID
Declare_Msg_Member(WORD, m_wTreasureNum)	// ��������
Declare_Msg_End

// ����ϳ���Ӧ
Declare_Msg_Begin(S2C_TREASURE_COMPOSE_RESP, MSG_S2C_TREASURE_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)	// ������ϢID
Declare_Msg_Member(WORD, m_wTreasureNum)	// ��������
Declare_Msg_End

// ���ﾫ��
Declare_Msg_Begin(C2S_TREASURE_REFINE_REQ, MSG_C2S_TREASURE_REFINE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// ����UID
Declare_Msg_End

// ���ﾫ����Ӧ
Declare_Msg_Begin(S2C_TREASURE_REFINE_RESP, MSG_S2C_TREASURE_REFINE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_DecodeMsg_UnsignedChar(m_byRefineLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_EncodeMsg_UnsignedChar(m_byRefineLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// ����UID
Declare_Msg_Member(BYTE, m_byRefineLevel)	// �����ȼ�
Declare_Msg_End

// ����ǿ��
Declare_Msg_Begin(C2S_TREASURE_STRENGTHEN_REQ, MSG_C2S_TREASURE_STRENGTHEN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_DecodeMsg_UnsignedShort(m_wCostItemNum)
m_wCostItemNum = min(m_wCostItemNum, MAX_TREASURE_ENHANCE_COST_ITEM);
Declare_DecodeMsg_Memory(m_CostItemList, m_wCostItemNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
// Declare_EncodeMsg_UnsignedChar(m_byCostItemNum)
// Declare_EncodeMsg_Memory(m_CostItemList, m_byCostItemNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// ����UID
Declare_Msg_Member(WORD, m_wCostItemNum)	// ������Ʒ����
Declare_Msg_Member_Array(DWORD, m_CostItemList, MAX_TREASURE_ENHANCE_COST_ITEM);
Declare_Msg_End

// ����ǿ����Ӧ
Declare_Msg_Begin(S2C_TREASURE_STRENGTHEN_RESP, MSG_S2C_TREASURE_STRENGTHEN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_EncodeMsg_UnsignedShort(m_wOldLevel)
Declare_EncodeMsg_UnsignedShort(m_wNewLevel)
Declare_EncodeMsg_UnsignedInt(m_dwExp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// ����UID
Declare_Msg_Member(WORD, m_wOldLevel)		// �ɵĵȼ�
Declare_Msg_Member(WORD, m_wNewLevel)		// �ɵĵȼ�
Declare_Msg_Member(DWORD, m_dwExp)			// ��õľ���
Declare_Msg_End

// ���ﴩ������
Declare_Msg_Begin(C2S_TREASURE_PUT_ON_REQ, MSG_C2S_TREASURE_PUT_ON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFormationPos)
Declare_DecodeMsg_UnsignedChar(m_byTreasurePos)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFormationPos)
Declare_EncodeMsg_UnsignedChar(m_byTreasurePos)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFormationPos)	// ����λ��
Declare_Msg_Member(BYTE, m_byTreasurePos)	// ����λ��
Declare_Msg_Member(DWORD, m_dwUID)			// ����uid
Declare_Msg_End

// ���ﴩ������
Declare_Msg_Begin(S2C_TREASURE_PUT_ON_RESP, MSG_S2C_TREASURE_PUT_ON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFormationPos)
Declare_DecodeMsg_UnsignedChar(m_byTreasurePos)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFormationPos)
Declare_EncodeMsg_UnsignedChar(m_byTreasurePos)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFormationPos)	// ����λ��
Declare_Msg_Member(BYTE, m_byTreasurePos)	// ����λ��
Declare_Msg_Member(DWORD, m_dwUID)			// ����uid
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_TREASURE_SMELT_REQ, MSG_C2S_TREASURE_SMELT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)			// ����uid
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_TREASURE_SMELT_RESP, MSG_S2C_TREASURE_SMELT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)			// ����uid
Declare_Msg_End

// ������Ʒ����
Declare_Msg_Begin(C2S_TREASURE_QUALITY_UP_REQ, MSG_C2S_TREASURE_QUALITY_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUId)			// ����uid
Declare_Msg_End

// ������Ʒ��Ӧ
Declare_Msg_Begin(S2C_TREASURE_QUALITY_UP_RESP, MSG_S2C_TREASURE_QUALITY_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_tTreasure.dwId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_tTreasure.dwId)
Declare_EncodeMsg_UnsignedInt(m_tTreasure.dwInfoId)
Declare_EncodeMsg_UnsignedShort(m_tTreasure.wEnhanceLevel)
Declare_EncodeMsg_UnsignedInt(m_tTreasure.dwEnhanceExp)
Declare_EncodeMsg_UnsignedShort(m_tTreasure.wRefineLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(Treasure, m_tTreasure)			// ����uid
Declare_Msg_End


// �Ѿ���������
Declare_Msg_Begin(C2S_REBEL_DATA_REQ, MSG_C2S_REBEL_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �Ѿ�����������Ӧ
Declare_Msg_Begin(S2C_REBEL_DATA_RESP, MSG_S2C_REBEL_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_RebelData.dwTodayExploit)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_RebelData.dwTodayExploit)
Declare_EncodeMsg_UnsignedInt(m_RebelData.dwTodayDamage)
Declare_EncodeMsg_UnsignedShort(m_RebelData.wExploitRank)
Declare_EncodeMsg_UnsignedShort(m_RebelData.wDamageRank)
Declare_EncodeMsg_UnsignedChar(m_RebelData.byIsAlreadyShare)
Declare_EncodeMsg_UnsignedShort(m_RebelData.wInitRoleLevel)
Declare_EncodeMsg_UnsignedShort(m_RebelData.byRebelListNum)
for (BYTE i = 0; i < m_RebelData.byRebelListNum; ++i)
{
	ClientRebelData &sRebelData = m_RebelData.arRebelList[i];
	Declare_EncodeMsg_UnsignedInt(sRebelData.dwRebelId);
	Declare_EncodeMsg_UnsignedInt(sRebelData.dwDiscovererRoleId);
	Declare_EncodeMsg_String(sRebelData.szDiscovererName, sizeof(sRebelData.szDiscovererName) - 1);
	Declare_EncodeMsg_UnsignedInt(sRebelData.dwTotalDamage);
	Declare_EncodeMsg_UnsignedShort(sRebelData.wRebelLevel);
	Declare_EncodeMsg_UnsignedShort(sRebelData.wRebelMapId);
	Declare_EncodeMsg_UnsignedInt(sRebelData.dwCreateTime);
	Declare_EncodeMsg_UnsignedChar(sRebelData.byQuality);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ClientPlayerRebelData, m_RebelData)	 // �Ѿ�����
Declare_Msg_End


// ��ȡ�Ѿ���ѫ��������
Declare_Msg_Begin(C2S_REBEL_EXPLOIT_AWARD_REQ, MSG_C2S_REBEL_EXPLOIT_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAwardNum)
m_wAwardNum = min(m_wAwardNum, MAX_EXPLOIT_AWARD_NUM);
Declare_DecodeMsg_Memory(m_AwardList, m_wAwardNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wAwardNum = min(m_wAwardNum, MAX_EXPLOIT_AWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_wAwardNum)
Declare_EncodeMsg_Memory(m_AwardList, m_wAwardNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAwardNum)								// ��������
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// �����б�
Declare_Msg_End

// ��ȡ�Ѿ���ѫ������Ӧ
Declare_Msg_Begin(S2C_REBEL_EXPLOIT_AWARD_RESP, MSG_S2C_REBEL_EXPLOIT_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAwardNum)
// m_byAwardNum = min(m_byAwardNum, MAX_EXPLOIT_AWARD_NUM);
// Declare_DecodeMsg_Memory(m_AwardList, m_byAwardNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wAwardNum = min(m_wAwardNum, MAX_EXPLOIT_AWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_wAwardNum)
Declare_EncodeMsg_Memory(m_AwardList, m_wAwardNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAwardNum)								// ��������
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// �����б�
Declare_Msg_End

// ��ѫ������¼����
Declare_Msg_Begin(C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ, MSG_C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ѫ������¼����
Declare_Msg_Begin(S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP, MSG_S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byAwardNum)
m_byAwardNum = min(m_byAwardNum, MAX_EXPLOIT_AWARD_NUM);
Declare_DecodeMsg_Memory(m_AwardList, m_byAwardNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byAwardNum = min(m_byAwardNum, MAX_EXPLOIT_AWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byAwardNum)
Declare_EncodeMsg_Memory(m_AwardList, m_byAwardNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_byAwardNum)								// ��������
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// �����б�
Declare_Msg_End


// �Ѿ�����
Declare_Msg_Begin(C2S_REBEL_SHARE_REQ, MSG_C2S_REBEL_SHARE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �Ѿ�������Ӧ
Declare_Msg_Begin(S2C_REBEL_SHARE_RESP, MSG_S2C_REBEL_SHARE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����Ѿ�֪ͨ
Declare_Msg_Begin(S2C_REBEL_DISCOVER_NOTIFY, MSG_S2C_REBEL_DISCOVER_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byQaulity)
Declare_DecodeMsg_UnsignedShort(m_wLevel);
Declare_DecodeMsg_UnsignedShort(m_wMapId);
Declare_DecodeMsg_UnsignedInt(m_dwRebelId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byQaulity)
Declare_EncodeMsg_UnsignedShort(m_wLevel);
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedInt(m_dwRebelId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byQaulity)		// �Ѿ�Ʒ��
Declare_Msg_Member(WORD, m_wLevel)			// �Ѿ��ȼ�
Declare_Msg_Member(WORD, m_wMapId)			// �Ѿ��ȼ�
Declare_Msg_Member(DWORD, m_dwRebelId)		// �Ѿ�Id
Declare_Msg_End

// �ֽⷵ������
Declare_Msg_Begin(C2S_RECYCLE_DECOMPOSE_RETURN_REQ, MSG_C2S_RECYCLE_DECOMPOSE_RETURN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
m_byTargetNum = min(m_byTargetNum, MAX_RECYCLE_DECOMPOSE_NUM);
Declare_DecodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byObjType)
m_byTargetNum = min(m_byTargetNum, MAX_RECYCLE_DECOMPOSE_NUM);
Declare_EncodeMsg_UnsignedChar(m_byTargetNum)
Declare_EncodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)										// �ֽ�����
Declare_Msg_Member(WORD, m_byTargetNum)										// �ֽ�Ŀ������
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// �ֽ�Ŀ���б�
Declare_Msg_End


// �ֽⷵ����Ӧ
Declare_Msg_Begin(S2C_RECYCLE_DECOMPOSE_RETURN_RESP, MSG_S2C_RECYCLE_DECOMPOSE_RETURN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
m_byObjectNum = min(m_byObjectNum, MAX_RECYCLE_RETURN_OBJ_NUM);
Declare_EncodeMsg_UnsignedShort(m_byObjectNum)
Declare_EncodeMsg_Memory(m_ReturnObjectList, m_byObjectNum*sizeof(Item));
Declare_EncodeMsg_UnsignedShort(m_byMoneyNum)
Declare_EncodeMsg_Memory(m_arMoneyList, m_byMoneyNum*sizeof(MoneyData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)											// �ֽ�����
Declare_Msg_Member_Array(Item, m_ReturnObjectList, MAX_RECYCLE_RETURN_OBJ_NUM);	// ���������б�
Declare_Msg_Member(BYTE, m_byObjectNum)											// ���������б�����
Declare_Msg_Member_Array(MoneyData, m_arMoneyList, MAX_MONEY_TYPE_NUM);			// ���������б�
Declare_Msg_Member(BYTE, m_byMoneyNum)											// ������������
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_RECYCLE_RESET_RETURN_REQ, MSG_C2S_RECYCLE_RESET_RETURN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// ��������
Declare_Msg_Member(DWORD, m_dwTargetUId)	// ����Ŀ��
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_RECYCLE_RESET_RETURN_RESP, MSG_S2C_RECYCLE_RESET_RETURN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_Memory(m_tMoney, sizeof(MoneyData))
m_byObjectNum = min(m_byObjectNum, MAX_RECYCLE_RETURN_OBJ_NUM);
Declare_EncodeMsg_UnsignedShort(m_byObjectNum)
Declare_EncodeMsg_Memory(m_ReturnObjectList, m_byObjectNum*sizeof(Item));
Declare_EncodeMsg_UnsignedShort(m_wMoneyNum)
Declare_EncodeMsg_Memory(m_arMoneyList, m_wMoneyNum*sizeof(MoneyData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)											// ��������
Declare_Msg_Member(MoneyData, m_tMoney)											// ��������(todo::�Ѿ�����, ����Ϊ�˷�ֹ�ɰ汾����,��ʱ����)
Declare_Msg_Member_Array(Item, m_ReturnObjectList, MAX_RECYCLE_RETURN_OBJ_NUM);	// ���������б�
Declare_Msg_Member(BYTE, m_byObjectNum)											// ���������б�����
Declare_Msg_Member_Array(MoneyData, m_arMoneyList, MAX_MONEY_TYPE_NUM);			// ���������б�
Declare_Msg_Member(WORD, m_wMoneyNum)											// ������������
Declare_Msg_End


// �ֽ��������
Declare_Msg_Begin(C2S_RECYCLE_DECOMPOSE_REQ, MSG_C2S_RECYCLE_DECOMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
m_byTargetNum = min(m_byTargetNum, MAX_RECYCLE_DECOMPOSE_NUM);
Declare_DecodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)										// �ֽ�����
Declare_Msg_Member(WORD, m_byTargetNum)										// �ֽ�Ŀ������
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// �ֽ�Ŀ���б�
Declare_Msg_End

// �ֽ������Ӧ
Declare_Msg_Begin(S2C_RECYCLE_DECOMPOSE_RESP, MSG_S2C_RECYCLE_DECOMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
m_byTargetNum = min(m_byTargetNum, MAX_RECYCLE_DECOMPOSE_NUM);
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
Declare_EncodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)										// �ֽ�����
Declare_Msg_Member(BYTE, m_byTargetNum)										// �ֽ�Ŀ������
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// �ֽ�Ŀ���б�
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_RECYCLE_RESET_REQ, MSG_C2S_RECYCLE_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// ��������
Declare_Msg_Member(DWORD, m_dwTargetUId)	// ����Ŀ��
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_RECYCLE_RESET_RESP, MSG_S2C_RECYCLE_RESET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// ��������
Declare_Msg_Member(DWORD, m_dwTargetUId)	// ����Ŀ��
Declare_Msg_End



// ����װ���б�
Declare_Msg_Begin(C2S_EQUIPMENT_LIST_REQ, MSG_C2S_EQUIPMENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����װ���б���Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_LIST_RESP, MSG_S2C_EQUIPMENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_wEquipmentNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wEquipmentNum = min(m_wEquipmentNum, MAX_EQUIPMENT_NUM);
Declare_EncodeMsg_UnsignedShort(m_wEquipmentNum)
Declare_EncodeMsg_Memory(m_EquipmentList, m_wEquipmentNum * sizeof(EquipmentClientData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wEquipmentNum)	// װ������
Declare_Msg_Member_Array(EquipmentClientData, m_EquipmentList, MAX_EQUIPMENT_NUM);	// װ���б�
Declare_Msg_End


// ����װ������
Declare_Msg_Begin(C2S_EQUIPMENT_INFO_REQ, MSG_C2S_EQUIPMENT_INFO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)
Declare_Msg_End

// ����װ��������Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_INFO_RESP, MSG_S2C_EQUIPMENT_INFO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_Equipment.dwId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_Equipment.dwId)
Declare_EncodeMsg_UnsignedShort(m_Equipment.dwInfoId)
Declare_EncodeMsg_UnsignedShort(m_Equipment.wEnhanceLevel)
Declare_EncodeMsg_UnsignedShort(m_Equipment.wRefineLevel)
Declare_EncodeMsg_UnsignedChar(m_Equipment.wStar)
Declare_EncodeMsg_UnsignedChar(m_Equipment.wStarLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(Equipment, m_Equipment)
Declare_Msg_End

// װ����Ϣ֪ͨ(��ʱ��ʹ��)
Declare_Msg_Begin(S2C_EQUIPMENT_INFO_NOTIFY, MSG_S2C_EQUIPMENT_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Memory(m_sEquipment, sizeof(EquipmentClientData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Memory(m_sEquipment, sizeof(EquipmentClientData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag)
Declare_Msg_Member(EquipmentClientData, m_sEquipment)	// װ����Ϣ
Declare_Msg_End


// װ���ϳ�����
Declare_Msg_Begin(C2S_EQUIPMENT_COMPOSE_REQ, MSG_C2S_EQUIPMENT_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwInfoId)
Declare_Msg_End

// װ���ϳ���Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_COMPOSE_RESP, MSG_S2C_EQUIPMENT_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwInfoId)
Declare_Msg_End

// ����װ������
Declare_Msg_Begin(C2S_EQUIPMENT_PUT_ON_REQ, MSG_C2S_EQUIPMENT_PUT_ON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFormationPos)
Declare_DecodeMsg_UnsignedChar(m_byEquipmentPos)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFormationPos)
Declare_EncodeMsg_UnsignedChar(m_byEquipmentPos)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFormationPos)	// ����λ��
Declare_Msg_Member(BYTE, m_byEquipmentPos)	// װ��λ��
Declare_Msg_Member(DWORD, m_dwUID)			// װ��uid
Declare_Msg_End

// ����װ������
Declare_Msg_Begin(S2C_EQUIPMENT_PUT_ON_RESP, MSG_S2C_EQUIPMENT_PUT_ON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFormationPos)
Declare_DecodeMsg_UnsignedChar(m_byEquipmentPos)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFormationPos)
Declare_EncodeMsg_UnsignedChar(m_byEquipmentPos)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFormationPos)	// ����λ��
Declare_Msg_Member(BYTE, m_byEquipmentPos)	// װ��λ��
Declare_Msg_Member(DWORD, m_dwUID)			// װ��uid
Declare_Msg_End



// װ��ǿ������
Declare_Msg_Begin(C2S_EQUIPMENT_STRENG_REQ, MSG_C2S_EQUIPMENT_STRENG_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ǿ������
Declare_Msg_Member(DWORD, m_dwUID)	// װ��UID	
Declare_Msg_End

// װ��ǿ����Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_STRENG_RESP, MSG_S2C_EQUIPMENT_STRENG_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedChar(m_byCritTimes)
Declare_DecodeMsg_UnsignedShort(m_wStrengthenLevel)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedChar(m_byCritTimes)
Declare_EncodeMsg_UnsignedShort(m_wStrengthenLevel)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)				// ǿ������
Declare_Msg_Member(BYTE, m_byCritTimes)			// ��������
Declare_Msg_Member(WORD, m_wStrengthenLevel)	// ǿ���ȼ�
Declare_Msg_Member(DWORD, m_dwUID)				// װ��UID	
Declare_Msg_End

// һ��ǿ������
Declare_Msg_Begin(C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ, MSG_C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_bySlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_bySlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_bySlot)	// ���Ͳ�λ
Declare_Msg_End

// һ��ǿ����Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP, MSG_S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_bySlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_bySlot)
Declare_EncodeMsg_UnsignedShort(m_wEquipmentNum)
for (WORD i = 0; i < m_wEquipmentNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_arOldEnhanceLevel[i]);
	Declare_EncodeMsg_UnsignedShort(m_arNewEnhanceLevel[i]);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_bySlot)			// ǿ������
Declare_Msg_Member(WORD, m_wEquipmentNum)	// ǿ��װ��������
Declare_Msg_Member_Array(WORD, m_arOldEnhanceLevel, EQUIPMENT_TYPE_MAX); // ǿ��֮ǰ�ȼ��б�
Declare_Msg_Member_Array(WORD, m_arNewEnhanceLevel, EQUIPMENT_TYPE_MAX); // ǿ��֮��ȼ��б�
Declare_Msg_End

// װ������
Declare_Msg_Begin(C2S_EQUIPMENT_REFINE_REQ, MSG_C2S_EQUIPMENT_REFINE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwEquipmentUID)
Declare_DecodeMsg_UnsignedShort(m_wCostListNum)
m_wCostListNum = min(m_wCostListNum, MAX_EQUIPMENT_REFINE_COST_NUM);
for (WORD i = 0; i < m_wCostListNum; ++i)
{
	Declare_DecodeMsg_UnsignedInt(m_arCostList[i].dwItemId);
	Declare_DecodeMsg_UnsignedShort(m_arCostList[i].wItemNum);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwEquipmentUID)
Declare_EncodeMsg_UnsignedShort(m_wCostListNum)
for (WORD i = 0; i < m_wCostListNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_arCostList[i].dwItemId);
	Declare_EncodeMsg_UnsignedShort(m_arCostList[i].wItemNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwEquipmentUID)	// װ��UID
Declare_Msg_Member(WORD, m_wCostListNum)		// �����б�����
Declare_Msg_Member_Array(EquipmentRefineCostData, m_arCostList, MAX_EQUIPMENT_REFINE_COST_NUM); // �����б�
Declare_Msg_End

// װ��������Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_REFINE_RESP, MSG_S2C_EQUIPMENT_REFINE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwEquipmentUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwEquipmentUID)
Declare_EncodeMsg_UnsignedChar(m_byOldRefineLevel)
Declare_EncodeMsg_UnsignedChar(m_byNewRefineLevel)
Declare_EncodeMsg_UnsignedInt(m_dwRefineExp)
Declare_EncodeMsg_UnsignedInt(m_dwAddRefineExp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwEquipmentUID)		// װ��UID
Declare_Msg_Member(BYTE, m_byOldRefineLevel)	// �ɵľ����ȼ�
Declare_Msg_Member(BYTE, m_byNewRefineLevel)	// �µľ����ȼ�
Declare_Msg_Member(DWORD, m_dwRefineExp)		// ��������
Declare_Msg_Member(DWORD, m_dwAddRefineExp)		// ������ӵľ�������
Declare_Msg_End


// װ����������
Declare_Msg_Begin(C2S_EQUIPMENT_STAR_UP_REQ, MSG_C2S_EQUIPMENT_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// װ��UID	
Declare_Msg_End


// װ��������Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_STAR_UP_RESP, MSG_S2C_EQUIPMENT_STAR_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCurStar)
Declare_DecodeMsg_UnsignedChar(m_byCurStarLevel)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCurStar)
Declare_EncodeMsg_UnsignedChar(m_byCurStarLevel)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCurStar)		// ��ǰ��
Declare_Msg_Member(BYTE, m_byCurStarLevel)	// ��ǰ�Ǽ�
Declare_Msg_Member(DWORD, m_dwUID)	// װ��UID	
Declare_Msg_End


// װ����Ʒ����
Declare_Msg_Begin(C2S_EQUIPMENT_QUALITY_UP_REQ, MSG_C2S_EQUIPMENT_QUALITY_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// װ��UID	
Declare_Msg_End

// װ����Ʒ��Ӧ
Declare_Msg_Begin(S2C_EQUIPMENT_QUALITY_UP_RESP, MSG_S2C_EQUIPMENT_QUALITY_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// װ��UID	
Declare_Msg_End

//���������
//�����˵�¼����
Declare_Msg_Begin(C2M_ROBOT_LOGIN_SYN, MSG_C2M_ROBOT_LOGIN_SYN)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szAccID, MAX_ACCOUNT_LEN)
	Declare_DecodeMsg_UnsignedInt(m_dwZoneID)
Declare_DecodeMsg_Function_End

Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szAccID, MAX_ACCOUNT_LEN)
	Declare_EncodeMsg_UnsignedInt(m_dwZoneID)
Declare_EncodeMsg_Function_End

Declare_Msg_Member_Array(char, m_szAccID, MAX_ACCOUNT_LEN + 1)				//�������ʺ�ID		
Declare_Msg_Member(DWORD, m_dwZoneID)										//����ID
Declare_Msg_End

//�����˵�¼Ӧ��
Declare_Msg_Begin( M2C_ROBOT_LOGIN_ACK, MSG_M2C_ROBOT_LOGIN_ACK )
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedInt( m_dwErrorCode )
Declare_DecodeMsg_Function_End

Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedInt( m_dwErrorCode )
Declare_EncodeMsg_Function_End

Declare_Msg_Member( DWORD, m_dwErrorCode )
Declare_Msg_End


/*--------------------------------------------------------------------------------------------------------------------------*/	
// 3) GateSvr��MapSvr֮����Ϣע��
/*--------------------------------------------------------------------------------------------------------------------------*/
//��ɫ�Ͽ�����֪ͨ
Declare_Msg_Begin( G2M_CLIENT_DISCONNECT_CMD, MSG_G2M_CLIENT_DISCONNECT_CMD )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End

Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//�߳����֪ͨ
Declare_Msg_Begin( M2G_CLIENT_KICKOUT_CMD, MSG_M2G_CLIENT_KICKOUT_CMD )
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Char(m_dwErrorCode)
Declare_DecodeMsg_Function_End

Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Char(m_dwErrorCode)
Declare_EncodeMsg_Function_End

Declare_Msg_Member(DWORD, m_dwErrorCode)
Declare_Msg_End

/*--------------------------------------------------------------------------------------------------------------------------*/	
// 4) MapSvr��OpenSvr֮����Ϣע��
/*--------------------------------------------------------------------------------------------------------------------------*/

//M2O ��������Ϣ
Declare_Msg_Begin( M2O_SERVERID_CMD, MSG_M2O_SERVERID_CMD )
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwServerID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwServerID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwServerID)
Declare_Msg_End


//������(open <--> map)
Declare_Msg_Begin( O2M_HEART_BEAT, MSG_O2M_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//������(open <--> map)
Declare_Msg_Begin( M2O_HEART_BEAT, MSG_M2O_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//������(gate <--> map)
Declare_Msg_Begin( G2M_HEART_BEAT, MSG_G2M_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//������(map <--> gate)
Declare_Msg_Begin( M2G_HEART_BEAT, MSG_M2G_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//�ͻ�����Ϣ
Declare_Msg_Begin(M2G_CLIENT_INFO, MSG_M2G_CLIENT_INFO)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)	//������ID
Declare_Msg_Member(DWORD, m_dwRoleId)	//��ɫID
Declare_Msg_End


// ��ɫ����
Declare_Msg_Begin(S2C_ROLE_LEVEL_UP_NOTIFY, MSG_S2C_ROLE_LEVEL_UP_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//��ɫ�ȼ�
Declare_Msg_End


// �߼�ͨ����Ϣ map --> open
Declare_Msg_Begin(M2O_LOGIC_COMMON_REQ, MSG_M2O_LOGIC_COMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//��ɫ�ȼ�
Declare_Msg_End

// �߼�ͨ����Ϣ map --> open
Declare_Msg_Begin(O2M_LOGIC_COMMON_RESP, MSG_O2M_LOGIC_COMMON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//��ɫ�ȼ�
Declare_Msg_End

// ��֤������
Declare_Msg_Begin(M2O_AUTHENTICATION_CODE_REQ, MSG_M2O_AUTHENTICATION_CODE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_UnsignedInt(m_dwTemplId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_UnsignedInt(m_dwTemplId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// �ֻ���
Declare_Msg_Member(DWORD, m_dwTemplId)	// ����ģ��ID
Declare_Msg_End

// ��֤����Ӧ
Declare_Msg_Begin(O2M_AUTHENTICATION_CODE_RESP, MSG_O2M_AUTHENTICATION_CODE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_UnsignedShort(m_wCode);
Declare_DecodeMsg_UnsignedShort(m_nErrorCode);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_UnsignedShort(m_wCode);
Declare_EncodeMsg_UnsignedShort(m_nErrorCode);
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// �ֻ���
Declare_Msg_Member(WORD, m_wCode)	// ��֤��
Declare_Msg_Member(short, m_nErrorCode)	// ������(0=��ʾ�ɹ�)
Declare_Msg_End

// ����ͳ��֪ͨ
Declare_Msg_Begin(M2O_REYUN_STATISTIC_NOTIFY, MSG_M2O_REYUN_STATISTIC_NOTIFY)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_String(m_szURL, sizeof(m_szURL))
Declare_DecodeMsg_String(m_szPath, sizeof(m_szPath))
Declare_DecodeMsg_String(m_szContext, sizeof(m_szContext))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_String(m_szURL, sizeof(m_szURL))
Declare_EncodeMsg_String(m_szPath, sizeof(m_szPath))
Declare_EncodeMsg_String(m_szContext, sizeof(m_szContext))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member_Array(char, m_szURL, MAX_REYUN_HTTP_URL_LEN)	// url
Declare_Msg_Member_Array(char, m_szPath, MAX_REYUN_HTTP_PATH_LEN)
Declare_Msg_Member_Array(char, m_szContext, MAX_REYUN_HTTP_CONTEXT_LEN)	// context
Declare_Msg_End

// �����б�֪ͨ
Declare_Msg_Begin(S2C_FORMATION_LIST_NOTIFY, MSG_S2C_FORMATION_LIST_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFormationNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byFormationNum)
for (int i = 0; i < m_byFormationNum; ++i)
{
	Formation &tmp = m_FormactionList[i];
	Declare_EncodeMsg_UnsignedInt(tmp.dwHeroUID);
	Declare_EncodeMsg_UnsignedInt(tmp.dwBlessSpriteId);
	Declare_EncodeMsg_UnsignedInt(tmp.arTreasureList[0]);
	Declare_EncodeMsg_UnsignedInt(tmp.arTreasureList[1]);
	Declare_EncodeMsg_Memory(tmp.arEquipmentList, sizeof(tmp.arEquipmentList));
	Declare_EncodeMsg_UnsignedShort(WORD(MAX_GEM_HOLE_NUM));
	Declare_EncodeMsg_Memory(tmp.arGemList, sizeof(tmp.arGemList));
	Declare_EncodeMsg_UnsignedShort(WORD(MAX_CARD_SLOT_NUM));
	Declare_EncodeMsg_Memory(tmp.arCardList, sizeof(tmp.arCardList));
}
Declare_EncodeMsg_UnsignedShort(m_byReinforcementNum);
Declare_EncodeMsg_Memory(m_arReinforcementList, m_byReinforcementNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFormationNum)	// �����б�����
Declare_Msg_Member_Array(Formation, m_FormactionList, MAX_BATTLE_HERO_NUM);	// �����б�
Declare_Msg_Member(BYTE, m_byReinforcementNum)	// Ԯ���б�����
Declare_Msg_Member_Array(DWORD, m_arReinforcementList, MAX_REINFORCEMENT_NUM); // Ԯ���б�
Declare_Msg_End

// GM
Declare_Msg_Begin(C2M_GM_REQ, MSG_C2M_GM_REQ)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedShort(m_wCmd)				// ����
	//Declare_DecodeMsg_UnsignedInt(m_dwTargetAccountId)	// ����
	Declare_DecodeMsg_UnsignedInt(m_dwValue1)			// ����
	Declare_DecodeMsg_UnsignedInt(m_dwValue2)			// ����
	Declare_DecodeMsg_UnsignedInt(m_dwValue3)			// ����
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedShort(m_wCmd)		
	//Declare_EncodeMsg_UnsignedInt(m_dwTargetAccountId)		
	Declare_EncodeMsg_UnsignedInt(m_dwValue1)		
	Declare_EncodeMsg_UnsignedInt(m_dwValue2)	
	Declare_EncodeMsg_UnsignedInt(m_dwValue3)
Declare_EncodeMsg_Function_End				
Declare_Msg_Member(WORD, m_wCmd)				// GMָ��
//Declare_Msg_Member(DWORD, m_dwTargetAccountId)	// ������Ŀ���ʺ�ID
Declare_Msg_Member(DWORD, m_dwValue1)			// ����ֵ1
Declare_Msg_Member(DWORD, m_dwValue2)			// ����ֵ2
Declare_Msg_Member(DWORD, m_dwValue3)			// ����ֵ3
Declare_Msg_End

////////////////////////////////////////////////////////////////////////////////////////////////
// ����ģ��

// ��������б�
Declare_Msg_Begin(C2S_FRIEND_LIST_REQ, MSG_C2S_FRIEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������б���Ӧ
Declare_Msg_Begin(S2C_FRIEND_LIST_RESP, MSG_S2C_FRIEND_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFriendNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byGetEnergyTimes)
Declare_EncodeMsg_UnsignedShort(m_byFriendNum)
for (BYTE i = 0; i < m_byFriendNum; ++i)
{
	ClientFriendData &tmp = m_FriendList[i];
	Declare_EncodeMsg_UnsignedInt(tmp.dwRoleId);
	Declare_EncodeMsg_UnsignedChar(tmp.wLevel);
	Declare_EncodeMsg_UnsignedShort(tmp.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tmp.dwFightingPower);
	Declare_EncodeMsg_String(tmp.szRoleName, sizeof(tmp.szRoleName)-1);
	Declare_EncodeMsg_String(tmp.szGuildName, sizeof(tmp.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedChar(tmp.byIsGiveEnergy);
	Declare_EncodeMsg_UnsignedChar(tmp.byTakeEnergyState);
	Declare_EncodeMsg_UnsignedInt(tmp.dwOfflineTime);
	Declare_EncodeMsg_UnsignedChar(tmp.byVipLevel);
	//Declare_EncodeMsg_String(tmp.szAccount, sizeof(tmp.szAccount) - 1);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byGetEnergyTimes)	// ��ȡ��������
Declare_Msg_Member(BYTE, m_byFriendNum)			// ��������
Declare_Msg_Member_Array(ClientFriendData, m_FriendList, MAX_FRIEND_NUM);	// �����б�
Declare_Msg_End

// ����������б�
Declare_Msg_Begin(C2S_BLACK_LIST_REQ, MSG_C2S_BLACK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������б���Ӧ
Declare_Msg_Begin(S2C_BLACK_LIST_RESP, MSG_S2C_BLACK_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBlackNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byBlackNum)
for (BYTE i = 0; i < m_byBlackNum; ++i)
{
	ClientFriendData &tmp = m_BlackList[i];
	Declare_EncodeMsg_UnsignedInt(tmp.dwRoleId);
	Declare_EncodeMsg_UnsignedChar(tmp.wLevel);
	Declare_EncodeMsg_UnsignedShort(tmp.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tmp.dwFightingPower);
	Declare_EncodeMsg_String(tmp.szRoleName, sizeof(tmp.szRoleName) - 1);
	Declare_EncodeMsg_String(tmp.szGuildName, sizeof(tmp.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedInt(tmp.dwOfflineTime);
	Declare_EncodeMsg_UnsignedChar(tmp.byVipLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBlackNum)	// ����������
Declare_Msg_Member_Array(ClientFriendData, m_BlackList, MAX_BLACK_NUM);	// �������б�
Declare_Msg_End

// ���������б�
Declare_Msg_Begin(C2S_FRIEND_APPLY_LIST_REQ, MSG_C2S_FRIEND_APPLY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���������б���Ӧ
Declare_Msg_Begin(S2C_FRIEND_APPLY_LIST_RESP, MSG_S2C_FRIEND_APPLY_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byApplyNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byApplyNum)
for (BYTE i = 0; i < m_byApplyNum; ++i)
{
	ClientFriendData &tmp = m_ApplyList[i];
	Declare_EncodeMsg_UnsignedInt(tmp.dwRoleId);
	Declare_EncodeMsg_UnsignedChar(tmp.wLevel);
	Declare_EncodeMsg_UnsignedShort(tmp.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tmp.dwFightingPower);
	Declare_EncodeMsg_String(tmp.szRoleName, sizeof(tmp.szRoleName) - 1);
	Declare_EncodeMsg_String(tmp.szGuildName, sizeof(tmp.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedInt(tmp.dwOfflineTime);
	Declare_EncodeMsg_UnsignedChar(tmp.byVipLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byApplyNum)	// ��������
Declare_Msg_Member_Array(ClientFriendData, m_ApplyList, MAX_FRIEND_APPLY_NUM);	// �����б�
Declare_Msg_End

// �����Ƽ��б�
Declare_Msg_Begin(C2S_FRIEND_RECOMMEND_LIST_REQ, MSG_C2S_FRIEND_RECOMMEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �Ƽ��б���Ӧ
Declare_Msg_Begin(S2C_FRIEND_RECOMMEND_LIST_RESP, MSG_S2C_FRIEND_RECOMMEND_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecommendNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRecommendNum)
for (BYTE i = 0; i < m_byRecommendNum; ++i)
{
	ClientFriendData &tmp = m_RecommendList[i];
	Declare_EncodeMsg_UnsignedInt(tmp.dwRoleId);
	Declare_EncodeMsg_UnsignedChar(tmp.wLevel);
	Declare_EncodeMsg_UnsignedShort(tmp.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tmp.dwFightingPower);
	Declare_EncodeMsg_String(tmp.szRoleName, sizeof(tmp.szRoleName) - 1);
	Declare_EncodeMsg_String(tmp.szGuildName, sizeof(tmp.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedInt(tmp.dwOfflineTime);
	Declare_EncodeMsg_UnsignedChar(tmp.byVipLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecommendNum)	// �Ƽ�����
Declare_Msg_Member_Array(ClientFriendData, m_RecommendList, MAX_FRIEND_RECOMMEND_NUM);	// �Ƽ��б�
Declare_Msg_End

// ��Ӻ���
Declare_Msg_Begin(C2S_FRIEND_ADD_REQ, MSG_C2S_FRIEND_ADD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ��Ӻ�����Ӧ
Declare_Msg_Begin(S2C_FRIEND_ADD_RESP, MSG_S2C_FRIEND_ADD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_UnsignedShort(m_wResult)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedShort(m_wResult)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_Member(WORD, m_wResult)
Declare_Msg_End


// ɾ������
Declare_Msg_Begin(C2S_FRIEND_DEL_REQ, MSG_C2S_FRIEND_DEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ɾ��������Ӧ
Declare_Msg_Begin(S2C_FRIEND_DEL_RESP, MSG_S2C_FRIEND_DEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ������ѻظ�
Declare_Msg_Begin(C2S_FRIEND_APPLY_REPLY_REQ, MSG_C2S_FRIEND_APPLY_REPLY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byAccept)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byAccept)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byAccept)
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ������ѻظ���Ӧ
Declare_Msg_Begin(S2C_FRIEND_APPLY_REPLY_RESP, MSG_S2C_FRIEND_APPLY_REPLY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byAccept)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byAccept)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byAccept)
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End


// ��Ӻ�����
Declare_Msg_Begin(C2S_BLACK_ADD_REQ, MSG_C2S_BLACK_ADD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ��Ӻ�������Ӧ
Declare_Msg_Begin(S2C_BLACK_ADD_RESP, MSG_S2C_BLACK_ADD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ɾ��������
Declare_Msg_Begin(C2S_BLACK_DEL_REQ, MSG_C2S_BLACK_DEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ɾ����������Ӧ
Declare_Msg_Begin(S2C_BLACK_DEL_RESP, MSG_S2C_BLACK_DEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ���;���
Declare_Msg_Begin(C2S_FRIEND_ENERGY_GIVE_REQ, MSG_C2S_FRIEND_ENERGY_GIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ���;�����Ӧ
Declare_Msg_Begin(S2C_FRIEND_ENERGY_GIVE_RESP, MSG_S2C_FRIEND_ENERGY_GIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// ��ȡ����
Declare_Msg_Begin(C2S_FRIEND_ENERGY_GET_REQ, MSG_C2S_FRIEND_ENERGY_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
m_byTargetNum = min(m_byTargetNum, MAX_FRIEND_NUM);
Declare_DecodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byTargetNum = min(m_byTargetNum, MAX_FRIEND_NUM);
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
Declare_EncodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_byTargetNum)			// Ŀ������
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_FRIEND_NUM);	// Ŀ���б�
Declare_Msg_End

// ��ȡ������Ӧ
Declare_Msg_Begin(S2C_FRIEND_ENERGY_GET_RESP, MSG_S2C_FRIEND_ENERGY_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
m_byTargetNum = min(m_byTargetNum, MAX_FRIEND_NUM);
Declare_DecodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byTargetNum = min(m_byTargetNum, MAX_FRIEND_NUM);
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
Declare_EncodeMsg_Memory(m_TargetList, m_byTargetNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_byTargetNum)			// Ŀ������
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_FRIEND_NUM);	// Ŀ���б�
Declare_Msg_End


// �������ݸ���֪ͨ
Declare_Msg_Begin(S2C_FRIEND_DATA_UPDATE_NOTIFY, MSG_S2C_FRIEND_DATA_UPDATE_NOTIFY)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedChar(m_byFlag);
	Declare_DecodeMsg_UnsignedInt(m_FriendData.dwRoleId);
	Declare_DecodeMsg_UnsignedChar(m_FriendData.wLevel);
	Declare_DecodeMsg_UnsignedShort(m_FriendData.wHeadIcon);
	Declare_DecodeMsg_UnsignedInt(m_FriendData.dwFightingPower);
	Declare_DecodeMsg_String(m_FriendData.szRoleName, sizeof(m_FriendData.szRoleName) - 1);
	Declare_DecodeMsg_String(m_FriendData.szGuildName, sizeof(m_FriendData.szGuildName) - 1);
	Declare_DecodeMsg_UnsignedChar(m_FriendData.byIsGiveEnergy);
	Declare_DecodeMsg_UnsignedChar(m_FriendData.byTakeEnergyState);
	Declare_DecodeMsg_UnsignedInt(m_FriendData.dwOfflineTime);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedChar(m_byFlag);
	Declare_EncodeMsg_UnsignedChar(m_byType);
	Declare_EncodeMsg_UnsignedInt(m_FriendData.dwRoleId);
	Declare_EncodeMsg_UnsignedChar(m_FriendData.wLevel);
	Declare_EncodeMsg_UnsignedShort(m_FriendData.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(m_FriendData.dwFightingPower);
	Declare_EncodeMsg_String(m_FriendData.szRoleName, sizeof(m_FriendData.szRoleName) - 1);
	Declare_EncodeMsg_String(m_FriendData.szGuildName, sizeof(m_FriendData.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedChar(m_FriendData.byIsGiveEnergy);
	Declare_EncodeMsg_UnsignedChar(m_FriendData.byTakeEnergyState);
	Declare_EncodeMsg_UnsignedInt(m_FriendData.dwOfflineTime);
	Declare_EncodeMsg_UnsignedChar(m_FriendData.byVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag)
Declare_Msg_Member(BYTE, m_byType)	// ����(1=����,2=����)
Declare_Msg_Member(ClientFriendData, m_FriendData)
Declare_Msg_End


// ����Ŀ��
Declare_Msg_Begin(C2S_FRIEND_TARGET_FIND_REQ, MSG_C2S_FRIEND_TARGET_FIND_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szTargetName, sizeof(m_szTargetName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szTargetName, sizeof(m_szTargetName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szTargetName, MAX_ROLE_LEN);
Declare_Msg_End


// ����Ŀ��
Declare_Msg_Begin(S2C_FRIEND_TARGET_FIND_RESP, MSG_S2C_FRIEND_TARGET_FIND_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_TargetData.dwRoleId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwRoleId);
Declare_EncodeMsg_UnsignedChar(m_TargetData.wLevel);
Declare_EncodeMsg_UnsignedShort(m_TargetData.wHeadIcon);
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwFightingPower);
Declare_EncodeMsg_String(m_TargetData.szRoleName, sizeof(m_TargetData.szRoleName) - 1);
Declare_EncodeMsg_String(m_TargetData.szGuildName, sizeof(m_TargetData.szGuildName) - 1);
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwOfflineTime);
Declare_EncodeMsg_UnsignedChar(m_TargetData.byVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerCommonData, m_TargetData)	// Ŀ������
Declare_Msg_End


//////////////////////////////////////////////////////////////////////////////
// �ᱦģ��

// �ᱦ��������
Declare_Msg_Begin(C2S_DUOBAO_DATA_REQ, MSG_C2S_DUOBAO_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �ᱦ������Ӧ
Declare_Msg_Begin(S2C_DUOBAO_DATA_RESP, MSG_S2C_DUOBAO_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_DuobaoInfo.dwPeaceOverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_DuobaoInfo.dwPeaceOverTime)
Declare_EncodeMsg_UnsignedShort(m_DuobaoInfo.wDailyDoubaoTimes)
Declare_EncodeMsg_UnsignedShort(m_DuobaoInfo.wPregrossRewardRecordNum)
Declare_EncodeMsg_Memory(m_DuobaoInfo.arPregrossRewardRecord, m_DuobaoInfo.wPregrossRewardRecordNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerDuobaoInfo, m_DuobaoInfo)	// �ᱦ����
Declare_Msg_End


// ����ᱦ��սĿ���б�
Declare_Msg_Begin(C2S_DUOBAO_TARGET_LIST_REQ, MSG_C2S_DUOBAO_TARGET_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureInfoId)		// ������ϢID
Declare_Msg_Member(WORD, m_wTreasureItemId)		// ������ƬID
Declare_Msg_End

// ����ᱦ��սĿ���б���Ӧ
Declare_Msg_Begin(S2C_DUOBAO_TARGET_LIST_RESP, MSG_S2C_DUOBAO_TARGET_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_EncodeMsg_UnsignedShort(m_byPlayerNum)
for (BYTE i = 0; i < m_byPlayerNum; ++i)
{
	DuobaoTargetPlayerData &tPlayer = m_PlayerList[i];
	Declare_EncodeMsg_UnsignedInt(tPlayer.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tPlayer.wLevel);
	Declare_EncodeMsg_UnsignedShort(tPlayer.dwHeadImage);
	Declare_EncodeMsg_UnsignedInt(tPlayer.dwFightingPower);
	Declare_EncodeMsg_String(tPlayer.szRoleName, sizeof(tPlayer.szRoleName)-1);
	Declare_EncodeMsg_UnsignedShort(tPlayer.byToBattleHeroNum);
	Declare_EncodeMsg_Memory(tPlayer.arToBattleHeroList, tPlayer.byToBattleHeroNum*sizeof(WORD));
}
Declare_EncodeMsg_UnsignedShort(m_byMonsterNum)
Declare_EncodeMsg_Memory(m_MonsterList, m_byMonsterNum*sizeof(DuobaoTargetMonsterData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureInfoId)		// ������ϢID
Declare_Msg_Member(WORD, m_wTreasureItemId)		// ������ƬID
Declare_Msg_Member(BYTE, m_byPlayerNum)			// �������
Declare_Msg_Member_Array(DuobaoTargetPlayerData, m_PlayerList, DUOBAO_TARGET_NUM);
Declare_Msg_Member(BYTE, m_byMonsterNum)			// ��������
Declare_Msg_Member_Array(DuobaoTargetMonsterData, m_MonsterList, DUOBAO_TARGET_NUM);
Declare_Msg_End

// �ᱦ5������
Declare_Msg_Begin(C2S_DUOBAO_FIVE_TIMES_REQ, MSG_C2S_DUOBAO_FIVE_TIMES_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_DecodeMsg_UnsignedShort(m_wDBTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_EncodeMsg_UnsignedShort(m_wDBTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureInfoId)	// ������ϢID
Declare_Msg_Member(WORD, m_wTreasureItemId)	// ������ƬID(��Ƭ��ʵ����Ʒ)
Declare_Msg_Member(WORD, m_wDBTimes)	// �ᱦ����
Declare_Msg_End

// �ᱦ5����Ӧ
Declare_Msg_Begin(S2C_DUOBAO_FIVE_TIMES_RESP, MSG_S2C_DUOBAO_FIVE_TIMES_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ItemList.size())
if (m_ItemList.size())
	Declare_EncodeMsg_Memory(m_ItemList[0], m_ItemList.size()*sizeof(Item));
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<Item>, m_ItemList);	// ��õ���Ʒ�б�
Declare_Msg_Member(BYTE, m_byFlag)				// ��־(1=��ã�0=û�л��)
Declare_Msg_End


// ��ȡ�ᱦ���Ƚ�������
Declare_Msg_Begin(C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ, MSG_C2S_TAKE_DUOBAO_PROGRESS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPreprossIdNum)
m_wPreprossIdNum = min(m_wPreprossIdNum, MAX_DOUBAO_PREGROSS_NUM);
Declare_DecodeMsg_Memory(m_PregrossIdList, m_wPreprossIdNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPreprossIdNum)
m_wPreprossIdNum = min(m_wPreprossIdNum, MAX_DOUBAO_PREGROSS_NUM);
Declare_EncodeMsg_Memory(m_PregrossIdList, m_wPreprossIdNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPreprossIdNum)
Declare_Msg_Member_Array(WORD, m_PregrossIdList, MAX_DOUBAO_PREGROSS_NUM);			// ����ID�б�
Declare_Msg_End


// ��ȡ�ᱦ���Ƚ�����Ӧ
Declare_Msg_Begin(S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP, MSG_S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPreprossIdNum)
Declare_EncodeMsg_Memory(m_PregrossIdList, m_wPreprossIdNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPreprossIdNum)
Declare_Msg_Member_Array(WORD, m_PregrossIdList, MAX_DOUBAO_PREGROSS_NUM);			// ����ID�б�
Declare_Msg_End


// ��������������
Declare_Msg_Begin(C2S_PLAYER_LAND_DATA_REQ, MSG_C2S_PLAYER_LAND_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)			// �����Ŀ�����ID
Declare_Msg_End

// ����������������Ӧ
Declare_Msg_Begin(S2C_PLAYER_LAND_DATA_RESP, MSG_S2C_PLAYER_LAND_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRiotSuppressTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedChar(m_byRiotSuppressTimes)
Declare_EncodeMsg_UnsignedInt(m_dwPatrolTotalTime)
Declare_EncodeMsg_UnsignedShort(m_byCityNum)
Declare_EncodeMsg_Memory(m_CityList, m_byCityNum*sizeof(LandCityBaseData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)				// �����Ŀ�����ID
Declare_Msg_Member(BYTE, m_byRiotSuppressTimes)		// ������ѹ����
Declare_Msg_Member(DWORD, m_dwPatrolTotalTime)		// Ѳ����ʱ��
Declare_Msg_Member(BYTE, m_byCityNum)			// �ǳ�����
Declare_Msg_Member_Array(LandCityBaseData, m_CityList, MAX_FRIEND_NUM);			// �ǳ��б�
Declare_Msg_End

// Ѳ������
Declare_Msg_Begin(C2S_LAND_PATROL_REQ, MSG_C2S_LAND_PATROL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwPatrolHeroUID)
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedChar(m_byPatrolType)
Declare_DecodeMsg_UnsignedChar(m_byTimeType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedChar(m_byPatrolType)
Declare_EncodeMsg_UnsignedChar(m_byTimeType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwPatrolHeroUID)	// Ѳ��Ӣ��uid
Declare_Msg_Member(WORD, m_wCityId)			// ���ID
Declare_Msg_Member(BYTE, m_byPatrolType)	// Ѳ������
Declare_Msg_Member(BYTE, m_byTimeType)		// ʱ������(1=4Сʱ,2=8Сʱ,3=12Сʱ)
Declare_Msg_End

// Ѳ����Ӧ
Declare_Msg_Begin(S2C_LAND_PATROL_RESP, MSG_S2C_LAND_PATROL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedChar(m_byPatrolType)
Declare_DecodeMsg_UnsignedChar(m_byTimeType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwPatrolHeroUID)
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedChar(m_byPatrolType)
Declare_EncodeMsg_UnsignedChar(m_byTimeType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwPatrolHeroUID)	// Ѳ��Ӣ��uid
Declare_Msg_Member(WORD, m_wCityId)			// ���ID
Declare_Msg_Member(BYTE, m_byPatrolType)	// Ѳ������
Declare_Msg_Member(BYTE, m_byTimeType)		// ʱ������(1=4Сʱ,2=8Сʱ,3=12Сʱ)
Declare_Msg_End

// �鿴Ѳ�߽���
Declare_Msg_Begin(C2S_LAND_VIEW_PATROL_AWARD_REQ, MSG_C2S_LAND_VIEW_PATROL_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ���ID
Declare_Msg_End

// �鿴Ѳ�߽�����Ӧ
Declare_Msg_Begin(S2C_LAND_VIEW_PATROL_AWARD_RESP, MSG_S2C_LAND_VIEW_PATROL_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedInt(m_dwGold)
Declare_DecodeMsg_UnsignedInt(m_dwDiamond)
Declare_DecodeMsg_UnsignedInt(m_dwEnergy)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedInt(m_dwGold)
Declare_EncodeMsg_UnsignedInt(m_dwDiamond)
Declare_EncodeMsg_UnsignedInt(m_dwEnergy)
Declare_EncodeMsg_UnsignedInt(m_dwHeroSoul)
Declare_EncodeMsg_UnsignedInt(m_dwGodSoul)
Declare_EncodeMsg_UnsignedShort(m_byAwardNum)
Declare_EncodeMsg_Memory(m_AwardList, m_byAwardNum*sizeof(Item));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ���ID(0=��ʾ�鿴����)
Declare_Msg_Member(DWORD, m_dwGold)			// �������
Declare_Msg_Member(DWORD, m_dwDiamond)		// ������ʯ
Declare_Msg_Member(DWORD, m_dwEnergy)		// ��������
Declare_Msg_Member(DWORD, m_dwHeroSoul)		// �����ۻ�
Declare_Msg_Member(DWORD, m_dwGodSoul)		// �������
Declare_Msg_Member(BYTE, m_byAwardNum)		// ��������
Declare_Msg_Member_Array(Item, m_AwardList, DUOBAO_TARGET_NUM);	// �����б�
Declare_Msg_End

// ��ȡѲ�߽���
Declare_Msg_Begin(C2S_LAND_GET_PATROL_AWARD_REQ, MSG_C2S_LAND_GET_PATROL_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ���ID(0=��ʾ����)
Declare_Msg_End

// ��ȡѲ�߽�����Ӧ
Declare_Msg_Begin(S2C_LAND_GET_PATROL_AWARD_RESP, MSG_S2C_LAND_GET_PATROL_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ��سǳ�ID(0=��ʾ����)
Declare_Msg_End

// ��ؼ�������(����,����)
Declare_Msg_Begin(C2S_LAND_SKILL_LEVELUP_REQ, MSG_C2S_LAND_SKILL_LEVELUP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ���ID
Declare_Msg_End

// ��ؼ���������Ӧ(����,����)
Declare_Msg_Begin(S2C_LAND_SKILL_LEVELUP_RESP, MSG_S2C_LAND_SKILL_LEVELUP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedChar(m_bySkillLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedChar(m_bySkillLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// ���ID
Declare_Msg_Member(BYTE, m_bySkillLevel)	// ��ǰ���ܵȼ�
Declare_Msg_End

// �����ѹ����
Declare_Msg_Begin(C2S_LAND_SUPPRESS_RIOT_REQ, MSG_C2S_LAND_SUPPRESS_RIOT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedInt(m_dwFriendId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedInt(m_dwFriendId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)				// ���ID
Declare_Msg_Member(DWORD, m_dwFriendId)			// ����ID
Declare_Msg_End

// �����ѹ������Ӧ
Declare_Msg_Begin(S2C_LAND_SUPPRESS_RIOT_RESP, MSG_S2C_LAND_SUPPRESS_RIOT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedInt(m_dwFriendId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedInt(m_dwFriendId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)				// ���ID
Declare_Msg_Member(DWORD, m_dwFriendId)			// ����ID
Declare_Msg_End

// �����������б�
Declare_Msg_Begin(C2S_FRIEND_LAND_LIST_REQ, MSG_C2S_FRIEND_LAND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����������б���Ӧ
Declare_Msg_Begin(S2C_FRIEND_LAND_LIST_RESP, MSG_S2C_FRIEND_LAND_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byLandNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byLandNum)
for (BYTE i = 0; i < m_byLandNum; ++i)
{
	FriendLandData &tLandData = m_LandList[i];
	Declare_EncodeMsg_UnsignedInt(tLandData.dwRoleId);
	Declare_EncodeMsg_String(tLandData.szName, sizeof(tLandData.szName)-1);
	Declare_EncodeMsg_UnsignedShort(tLandData.wLevel);
	Declare_EncodeMsg_UnsignedShort(tLandData.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tLandData.dwLastLogoutTime);
	Declare_EncodeMsg_UnsignedChar(tLandData.byCityNum);
	Declare_EncodeMsg_UnsignedChar(tLandData.byPatrolCityNum);
	Declare_EncodeMsg_UnsignedChar(tLandData.byRiotCityNum);
}
Declare_EncodeMsg_Memory(m_LandList, m_byLandNum*sizeof(FriendLandData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byLandNum)		// �������
Declare_Msg_Member_Array(FriendLandData, m_LandList, MAX_FRIEND_NUM);	// ����б�
Declare_Msg_End

// ��سǳ���������
Declare_Msg_Begin(C2S_LAND_CITY_DETAILS_REQ, MSG_C2S_LAND_CITY_DETAILS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwPlayerId)
Declare_DecodeMsg_UnsignedShort(m_dwCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwPlayerId)
Declare_EncodeMsg_UnsignedShort(m_dwCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwPlayerId)			// ���ID
Declare_Msg_Member(WORD, m_dwCityId)			// �ǳ�ID
Declare_Msg_End

// ��سǳ�����������Ӧ
Declare_Msg_Begin(S2C_LAND_CITY_DETAILS_RESP, MSG_S2C_LAND_CITY_DETAILS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwPlayerId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwPlayerId)
Declare_EncodeMsg_UnsignedShort(m_tCityData.wId)
Declare_EncodeMsg_UnsignedChar(m_tCityData.bySkillLevel)
Declare_EncodeMsg_UnsignedShort(m_tCityData.wPatrolHeroInfoId)
Declare_EncodeMsg_UnsignedInt(m_tCityData.dwStartPatrolTime)
Declare_EncodeMsg_UnsignedChar(m_tCityData.byPatrolType)
Declare_EncodeMsg_UnsignedChar(m_tCityData.byTimeType)
Declare_EncodeMsg_UnsignedChar(m_tCityData.byIsRiot)
Declare_EncodeMsg_UnsignedChar(m_tCityData.wAwardItemNum)
Declare_EncodeMsg_UnsignedShort(m_tCityData.byPatrolEventNum)
Declare_EncodeMsg_Memory(m_tCityData.arPatrolEventList, m_tCityData.byPatrolEventNum*sizeof(LandPatrolEvent));
Declare_EncodeMsg_UnsignedShort(m_tCityData.byRiotEventNum)
for (BYTE i = 0; i < m_tCityData.byRiotEventNum; ++i)
{
	LandRiotEvent &tRiotEvent = m_tCityData.arRiotEventList[i];
	Declare_EncodeMsg_UnsignedInt(tRiotEvent.dwHappenTime)
	Declare_EncodeMsg_UnsignedInt(tRiotEvent.dwSuppressTime)
	Declare_EncodeMsg_String(tRiotEvent.szFriendName, sizeof(tRiotEvent.szFriendName) - 1)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwPlayerId)			// ���ID
Declare_Msg_Member(LandCityData, m_tCityData)	// �ǳ�����
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_SPIRIT_DATA_REQ, MSG_C2S_SPIRIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_SPIRIT_DATA_RESP, MSG_S2C_SPIRIT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tSpriteInfo.wSpiritNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_tSpriteInfo.dwCurUseSpiritId);
Declare_EncodeMsg_UnsignedShort(m_tSpriteInfo.wSpiritNum)
for (WORD i = 0; i < m_tSpriteInfo.wSpiritNum; ++i)
{
	SpiritData &tSpiritData = m_tSpriteInfo.arSpiritList[i];
	Declare_EncodeMsg_UnsignedInt(tSpiritData.dwId);
	Declare_EncodeMsg_UnsignedShort(tSpiritData.wEnhanceLevel);
	Declare_EncodeMsg_UnsignedShort(tSpiritData.wTrainStage);
	Declare_EncodeMsg_UnsignedShort(tSpiritData.wTrainTimes);
	Declare_EncodeMsg_UnsignedShort(tSpiritData.wStarLevel);
}
Declare_EncodeMsg_UnsignedShort(m_tSpriteInfo.wPokedexNum)
for (WORD i = 0; i < m_tSpriteInfo.wPokedexNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_tSpriteInfo.arPokedexList[i].wPokedexId);
	Declare_EncodeMsg_UnsignedShort(m_tSpriteInfo.arPokedexList[i].wLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerSpriteInfo, m_tSpriteInfo)	 // ������Ϣ
Declare_Msg_End


// ����֮����������
Declare_Msg_Begin(C2S_HEART_OF_WORLD_DATA_REQ, MSG_C2S_HEART_OF_WORLD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����֮��������Ӧ
Declare_Msg_Begin(S2C_HEART_OF_WORLD_DATA_RESP, MSG_S2C_HEART_OF_WORLD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStage)
Declare_DecodeMsg_UnsignedChar(m_byLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStage)
Declare_EncodeMsg_UnsignedChar(m_byLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStage)			// �׶�
Declare_Msg_Member(BYTE, m_byLevel)			// �ȼ�
Declare_Msg_End

// ����֮����������
Declare_Msg_Begin(C2S_HEART_OF_WORLD_LEVEL_UP_REQ, MSG_C2S_HEART_OF_WORLD_LEVEL_UP_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����֮��������Ӧ
Declare_Msg_Begin(S2C_HEART_OF_WORLD_LEVEL_UP_RESP, MSG_S2C_HEART_OF_WORLD_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStage)
Declare_DecodeMsg_UnsignedChar(m_byLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStage)
Declare_EncodeMsg_UnsignedChar(m_byLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStage)			// �׶�
Declare_Msg_Member(BYTE, m_byLevel)			// �ȼ�
Declare_Msg_End


// ����ǿ������
Declare_Msg_Begin(C2S_SPIRIT_ENHANCE_REQ, MSG_C2S_SPIRIT_ENHANCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// ����ID
Declare_Msg_End


// ����ǿ����Ӧ
Declare_Msg_Begin(S2C_SPIRIT_ENHANCE_RESP, MSG_S2C_SPIRIT_ENHANCE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// ����ID
Declare_Msg_End


// ����ʹ������
Declare_Msg_Begin(C2S_SPIRIT_USE_REQ, MSG_C2S_SPIRIT_USE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// ����ID
Declare_Msg_End

// ����ʹ����Ӧ
Declare_Msg_Begin(S2C_SPIRIT_USE_RESP, MSG_S2C_SPIRIT_USE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// ����ID
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_SPIRIT_TRAIN_REQ, MSG_C2S_SPIRIT_TRAIN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_UnsignedShort(m_wTrainTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_UnsignedShort(m_wTrainTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_Member(WORD, m_wTrainTimes)			// ��������
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_SPIRIT_TRAIN_RESP, MSG_S2C_SPIRIT_TRAIN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_UnsignedShort(m_wTrainTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_UnsignedShort(m_wTrainTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_Member(WORD, m_wTrainTimes)			// ��������
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_SPRITE_STAR_UP_REQ, MSG_C2S_SPRITE_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_SPRITE_STAR_UP_RESP, MSG_S2C_SPRITE_STAR_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_End

// ����ף������
Declare_Msg_Begin(C2S_SPRITE_BLESS_REQ, MSG_C2S_SPRITE_BLESS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBlessType)
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBlessType)
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBlessType)			// ף������
Declare_Msg_Member(WORD, m_wFormationPos)		// ��Ӫλ��
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_End


// ����ף����Ӧ
Declare_Msg_Begin(S2C_SPRITE_BLESS_RESP, MSG_S2C_SPRITE_BLESS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBlessType)
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBlessType)
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBlessType)			// ף������
Declare_Msg_Member(WORD, m_wFormationPos)		// ��Ӫλ��
Declare_Msg_Member(DWORD, m_dwSpriteId)			// ����ID
Declare_Msg_End


// ����ϳ�����
Declare_Msg_Begin(C2S_SPRITE_COMPOSE_REQ, MSG_C2S_SPRITE_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFragmentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFragmentId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFragmentId)			// ������ƬID
Declare_Msg_End


// ����ϳ���Ӧ
Declare_Msg_Begin(S2C_SPRITE_COMPOSE_RESP, MSG_S2C_SPRITE_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFragmentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFragmentId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFragmentId)			// ������ƬID
Declare_Msg_End


// �����б�����
Declare_Msg_Begin(C2S_GUILD_LIST_REQ, MSG_C2S_GUILD_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б�������Ӧ
Declare_Msg_Begin(S2C_GUILD_LIST_RESP, MSG_S2C_GUILD_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byGuildNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byGuildNum)
for (BYTE i = 0; i < m_byGuildNum; ++i)
{
	GuildBaseData &tGuildData = m_GuildList[i];
	Declare_EncodeMsg_UnsignedInt(tGuildData.dwId);
	Declare_EncodeMsg_String(tGuildData.szGuildName, sizeof(tGuildData.szGuildName) - 1);
	Declare_EncodeMsg_UnsignedShort(tGuildData.wIconId);
	Declare_EncodeMsg_UnsignedShort(tGuildData.wLevel);
	Declare_EncodeMsg_UnsignedChar(tGuildData.byMemberNum);
	Declare_EncodeMsg_String(tGuildData.szCaptainName, sizeof(tGuildData.szCaptainName) - 1);
	Declare_EncodeMsg_UnsignedChar(tGuildData.byIsFull);
	Declare_EncodeMsg_UnsignedChar(tGuildData.byIsApply);
	Declare_EncodeMsg_String(tGuildData.szDeclaration, sizeof(tGuildData.szDeclaration) - 1);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byGuildNum)		// ��������
Declare_Msg_Member_Array(GuildBaseData, m_GuildList, MAX_GUILD_NUM);	// �����б�
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_GUILD_CREATE_REQ, MSG_C2S_GUILD_CREATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wIconId)
Declare_DecodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wIconId)
Declare_EncodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN);
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_GUILD_CREATE_RESP, MSG_S2C_GUILD_CREATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wIconId)
Declare_DecodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wIconId)
Declare_EncodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN);
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_GUILD_DATE_REQ, MSG_C2S_GUILD_DATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������Ӧ 
Declare_Msg_Begin(S2C_GUILD_DATE_RESP, MSG_S2C_GUILD_DATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_GuildData.dwId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPosition)
Declare_EncodeMsg_UnsignedInt(m_GuildData.dwId)
Declare_EncodeMsg_String(m_GuildData.szName, sizeof(m_GuildData.szName) - 1);
Declare_EncodeMsg_UnsignedShort(m_GuildData.wIconId)
Declare_EncodeMsg_UnsignedInt(m_GuildData.dwCreateTime)
Declare_EncodeMsg_UnsignedShort(m_GuildData.wLevel)
Declare_EncodeMsg_UnsignedInt(m_GuildData.dwExp)
Declare_EncodeMsg_UnsignedInt(m_GuildData.dwQuanShui)
Declare_EncodeMsg_UnsignedChar(m_GuildData.byQYTimes) 
Declare_EncodeMsg_UnsignedChar(m_byQYType)
Declare_EncodeMsg_String(m_GuildData.szNotice, sizeof(m_GuildData.szNotice) - 1);
Declare_EncodeMsg_String(m_GuildData.szDeclaration, sizeof(m_GuildData.szDeclaration) - 1);
Declare_EncodeMsg_UnsignedChar(m_byMemberNum)
Declare_EncodeMsg_String(m_szCaptainName, sizeof(m_szCaptainName)-1);
Declare_EncodeMsg_UnsignedShort(m_byQSRewardRecordNum)
Declare_EncodeMsg_Memory(m_arQSRewardRecord, m_byQSRewardRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedChar(m_byLastQYType)
Declare_EncodeMsg_UnsignedInt(m_dwLastQYValue)
Declare_EncodeMsg_String(m_szLastQYTargetName, sizeof(m_szLastQYTargetName)-1);
Declare_EncodeMsg_UnsignedShort(m_wRedPointNum)
Declare_EncodeMsg_Memory(m_arRedPointList, m_wRedPointNum*sizeof(BYTE))
Declare_EncodeMsg_UnsignedInt(m_GuildData.dwActive)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPosition)		// ְλ
Declare_Msg_Member(GuildData, m_GuildData)
Declare_Msg_Member(BYTE, m_byQYType)		// Ȫӿ����
Declare_Msg_Member(BYTE, m_byMemberNum)		// ��Ա����
Declare_Msg_Member_Array(char, m_szCaptainName, MAX_ROLE_LEN);	// �᳤����
Declare_Msg_Member(BYTE, m_byQSRewardRecordNum)		// Ȫˮ������¼����
Declare_Msg_Member_Array(WORD, m_arQSRewardRecord, MAX_GUILD_QS_REWARD_RECORD);	// Ȫˮ������ȡ��¼
Declare_Msg_Member(BYTE, m_byLastQYType) // ���Ȫӿ����(0��ʾ��ľ��Ȫӿ)
Declare_Msg_Member(DWORD, m_dwLastQYValue)	// ���Ȫӿֵ
Declare_Msg_Member_Array(char, m_szLastQYTargetName, MAX_ROLE_LEN);	// ���ȪӿĿ������
Declare_Msg_Member(WORD, m_wRedPointNum) // �������
Declare_Msg_Member_Array(BYTE, m_arRedPointList, GUILD_RED_POINT_MAX);	// ����б�
Declare_Msg_End

// ��Ա�б�����
Declare_Msg_Begin(C2S_GUILD_MEMBER_LIST_REQ, MSG_C2S_GUILD_MEMBER_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ա�б���Ӧ
Declare_Msg_Begin(S2C_GUILD_MEMBER_LIST_RESP, MSG_S2C_GUILD_MEMBER_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byMemberNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byMemberNum)
for (BYTE i = 0; i < m_byMemberNum; ++i)
{
	ClientGuildMember &tMemberData = m_GuildList[i];
	Declare_EncodeMsg_UnsignedInt(tMemberData.dwRoleId);
	Declare_EncodeMsg_String(tMemberData.szRoleName, sizeof(tMemberData.szRoleName) - 1);
	Declare_EncodeMsg_UnsignedChar(tMemberData.byPosition);
	Declare_EncodeMsg_UnsignedShort(tMemberData.wLevel);
	Declare_EncodeMsg_UnsignedInt(tMemberData.dwFightingPower);
	Declare_EncodeMsg_UnsignedShort(tMemberData.wHeadIconId);
	Declare_EncodeMsg_UnsignedInt(tMemberData.dwTotalContribution);
	Declare_EncodeMsg_UnsignedInt(tMemberData.dwTodayContribution);
	Declare_EncodeMsg_UnsignedInt(tMemberData.dwLastLogoutTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byMemberNum)		// ��Ա����
Declare_Msg_Member_Array(ClientGuildMember, m_GuildList, MAX_FRIEND_NUM);	// ��Ա�б�
Declare_Msg_End

// �����б�����
Declare_Msg_Begin(C2S_GUILD_LEAVEMSG_LIST_REQ, MSG_C2S_GUILD_LEAVEMSG_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б���Ӧ
Declare_Msg_Begin(S2C_GUILD_LEAVEMSG_LIST_RESP, MSG_S2C_GUILD_LEAVEMSG_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byLeaveMsgNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byLeaveMsgNum)
for (BYTE i = 0; i < m_byLeaveMsgNum; ++i)
{
	GuildLeaveMessage &tLeaveMsg = m_LeaveMsgList[i];
	Declare_EncodeMsg_UnsignedInt(tLeaveMsg.dwTime);
	Declare_EncodeMsg_String(tLeaveMsg.szName, sizeof(tLeaveMsg.szName));
	Declare_EncodeMsg_UnsignedChar(tLeaveMsg.byPos);
	Declare_EncodeMsg_String(tLeaveMsg.szText, sizeof(tLeaveMsg.szText));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byLeaveMsgNum)		// ������Ϣ����
Declare_Msg_Member_Array(GuildLeaveMessage, m_LeaveMsgList, MAX_FRIEND_NUM);	// ������Ϣ�б�
Declare_Msg_End

// �����б�����
Declare_Msg_Begin(C2S_GUILD_APPLY_LIST_REQ, MSG_C2S_GUILD_APPLY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б���Ӧ
Declare_Msg_Begin(S2C_GUILD_APPLY_LIST_RESP, MSG_S2C_GUILD_APPLY_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wApplyDataNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wApplyDataNum)
for (WORD i = 0; i < m_wApplyDataNum; ++i)
{
	PlayerCommonData &tApplyData = m_ApplyDataList[i];
	Declare_EncodeMsg_UnsignedInt(tApplyData.dwRoleId);
	Declare_EncodeMsg_String(tApplyData.szRoleName, sizeof(tApplyData.szRoleName) - 1);
	Declare_EncodeMsg_UnsignedShort(tApplyData.wLevel); 
	Declare_EncodeMsg_UnsignedShort(tApplyData.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tApplyData.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tApplyData.byVipLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wApplyDataNum)		// ������������
Declare_Msg_Member_Array(PlayerCommonData, m_ApplyDataList, MAX_GUILD_APPLY_NUM);	// ���������б�
Declare_Msg_End


// ��ɢ����
Declare_Msg_Begin(C2S_GUILD_DISMISS_REQ, MSG_C2S_GUILD_DISMISS_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ɢ������Ӧ
Declare_Msg_Begin(S2C_GUILD_DISMISS_RESP, MSG_S2C_GUILD_DISMISS_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������빤��
Declare_Msg_Begin(C2S_GUILD_APPLY_REQ, MSG_C2S_GUILD_APPLY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)
Declare_Msg_End

// ������빤����Ӧ
Declare_Msg_Begin(S2C_GUILD_APPLY_RESP, MSG_S2C_GUILD_APPLY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)
Declare_Msg_End

// �������
Declare_Msg_Begin(C2S_GUILD_APPLY_CHECK_REQ, MSG_C2S_GUILD_APPLY_CHECK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_UnsignedChar(m_byIsAccept)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedChar(m_byIsAccept)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// ��˵�Ŀ��ID
Declare_Msg_Member(BYTE, m_byIsAccept)	// �Ƿ����
Declare_Msg_End

// ���������Ӧ
Declare_Msg_Begin(S2C_GUILD_APPLY_CHECK_RESP, MSG_S2C_GUILD_APPLY_CHECK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedChar(m_byIsAccept)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// ��˵�Ŀ��ID
Declare_Msg_Member(BYTE, m_byIsAccept)	// �Ƿ����
Declare_Msg_End

// ȡ������
Declare_Msg_Begin(C2S_GUILD_APPLY_CANCEL_REQ, MSG_C2S_GUILD_APPLY_CANCEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// ȡ���Ĺ���ID
Declare_Msg_End

// ȡ��������Ӧ
Declare_Msg_Begin(S2C_GUILD_APPLY_CANCEL_RESP, MSG_S2C_GUILD_APPLY_CANCEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// ȡ���Ĺ���ID
Declare_Msg_End

// �߳�����
Declare_Msg_Begin(C2S_GUILD_KICKOUT_REQ, MSG_C2S_GUILD_KICKOUT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// �߳���Ŀ��ID
Declare_Msg_End

// �߳�������Ӧ
Declare_Msg_Begin(S2C_GUILD_KICKOUT_RESP, MSG_S2C_GUILD_KICKOUT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// �߳���Ŀ��ID
Declare_Msg_End

// �˳�����
Declare_Msg_Begin(C2S_GUILD_LEAVE_REQ, MSG_C2S_GUILD_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �˳�������Ӧ
Declare_Msg_Begin(S2C_GUILD_LEAVE_RESP, MSG_S2C_GUILD_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����
Declare_Msg_Begin(C2S_GUILD_LEAVE_MESSAGE_REQ, MSG_C2S_GUILD_LEAVE_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szLeaveMsg, sizeof(m_szLeaveMsg)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szLeaveMsg, sizeof(m_szLeaveMsg));
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szLeaveMsg, MAX_GUILD_LEAVEMESSAGE_LEN);
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_GUILD_LEAVE_MESSAGE_RESP, MSG_S2C_GUILD_LEAVE_MESSAGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_LeaveMsg.dwTime);
Declare_DecodeMsg_String(m_LeaveMsg.szName, sizeof(m_LeaveMsg.szName) - 1);
Declare_DecodeMsg_UnsignedChar(m_LeaveMsg.byPos);
Declare_DecodeMsg_String(m_LeaveMsg.szText, sizeof(m_LeaveMsg.szText) - 1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_LeaveMsg.dwTime);
Declare_EncodeMsg_String(m_LeaveMsg.szName, sizeof(m_LeaveMsg.szName));
Declare_EncodeMsg_UnsignedChar(m_LeaveMsg.byPos);
Declare_EncodeMsg_String(m_LeaveMsg.szText, sizeof(m_LeaveMsg.szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(GuildLeaveMessage, m_LeaveMsg)	// ��������
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_GUILD_SEARCH_REQ, MSG_C2S_GUILD_SEARCH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_String(m_szGuildName, sizeof(m_szGuildName));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// ����ID
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN);
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_GUILD_SEARCH_RESP, MSG_S2C_GUILD_SEARCH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGuildNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGuildNum);
for (WORD i = 0; i < m_wGuildNum; ++i)
{
	GuildBaseData &tBaseData = m_GuildList[i];
	Declare_EncodeMsg_UnsignedInt(tBaseData.dwId);
	Declare_EncodeMsg_String(tBaseData.szGuildName, sizeof(tBaseData.szGuildName));
	Declare_EncodeMsg_UnsignedShort(tBaseData.wIconId);
	Declare_EncodeMsg_UnsignedShort(tBaseData.wLevel);
	Declare_EncodeMsg_UnsignedChar(tBaseData.byMemberNum);
	Declare_EncodeMsg_String(tBaseData.szCaptainName, sizeof(tBaseData.szCaptainName));
	Declare_EncodeMsg_UnsignedChar(tBaseData.byIsFull);
	Declare_EncodeMsg_UnsignedChar(tBaseData.byIsApply);
	Declare_EncodeMsg_String(tBaseData.szDeclaration, sizeof(tBaseData.szDeclaration));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGuildNum)	// ��������
Declare_Msg_Member_Array(GuildBaseData, m_GuildList, MAX_SEARCH_GUILD_NUM);
Declare_Msg_End

// Ȫӿ
Declare_Msg_Begin(C2S_GUILD_QY_REQ, MSG_C2S_GUILD_QY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_End


// Ȫӿ��Ӧ
Declare_Msg_Begin(S2C_GUILD_QY_RESP, MSG_S2C_GUILD_QY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwExp)
Declare_EncodeMsg_UnsignedInt(m_dwContribution)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_Member(DWORD, m_dwExp)	// ��õĹ��ᾭ��
Declare_Msg_Member(DWORD, m_dwContribution)	// ��õĽ���ֵ
Declare_Msg_End

// ��ȡȪˮ����
Declare_Msg_Begin(C2S_GUILD_QS_AWARD_REQ, MSG_C2S_GUILD_QS_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wQuanShui)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wQuanShui)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wQuanShui)	// Ȫˮ
Declare_Msg_End

// ��ȡȪˮ������Ӧ
Declare_Msg_Begin(S2C_GUILD_QS_AWARD_RESP, MSG_S2C_GUILD_QS_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wQuanShui)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wQuanShui)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wQuanShui)	// Ȫˮ
Declare_Msg_End


// �޸Ĺ�����Ϣ����
Declare_Msg_Begin(C2S_GUILD_INFO_MODIFY_REQ, MSG_C2S_GUILD_INFO_MODIFY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wIconId)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wIconId)
Declare_EncodeMsg_String(m_szText, sizeof(m_szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szText, MAX_GUILD_TEXT_NUM);
Declare_Msg_End


// �޸Ĺ�����Ϣ��Ӧ
Declare_Msg_Begin(S2C_GUILD_INFO_MODIFY_RESP, MSG_S2C_GUILD_INFO_MODIFY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wIconId)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wIconId)
Declare_EncodeMsg_String(m_szText, sizeof(m_szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// ����
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szText, MAX_GUILD_TEXT_NUM);
Declare_Msg_End

// �����¼��б�����
Declare_Msg_Begin(C2S_GUILD_EVENT_LIST_REQ, MSG_C2S_GUILD_EVENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����¼��б���Ӧ
Declare_Msg_Begin(S2C_GUILD_EVENT_LIST_RESP, MSG_S2C_GUILD_EVENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byEventNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byEventNum)
for (BYTE i = 0; i < m_byEventNum; ++i)
{
	GuildEvent &tEvent= m_EventList[i];
	Declare_EncodeMsg_UnsignedShort(tEvent.wEventId);
	Declare_EncodeMsg_String(tEvent.szTargetName, sizeof(tEvent.szTargetName) - 1);
	Declare_EncodeMsg_UnsignedInt(tEvent.dwValue);
	Declare_EncodeMsg_UnsignedInt(tEvent.dwTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byEventNum)		// �¼���������
Declare_Msg_Member_Array(GuildEvent, m_EventList, MAX_GUILD_EVENT_NUM);	// �¼��б�
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_GUILD_SKILL_LEVEL_UP_REQ, MSG_C2S_GUILD_SKILL_LEVEL_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wSkillId)
Declare_DecodeMsg_UnsignedShort(m_wSkillLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wSkillId)
Declare_EncodeMsg_UnsignedShort(m_wSkillLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// ����������(1=����,2=����)
Declare_Msg_Member(WORD, m_wSkillId)	// ����ID
Declare_Msg_Member(WORD, m_wSkillLevel)	// ���ܵȼ�
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_GUILD_SKILL_LEVEL_UP_RESP, MSG_S2C_GUILD_SKILL_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wSkillId)
Declare_DecodeMsg_UnsignedShort(m_wSkillLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wSkillId)
Declare_EncodeMsg_UnsignedShort(m_wSkillLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// ����������(1=����,2=����)
Declare_Msg_Member(WORD, m_wSkillId)	// ����ID
Declare_Msg_Member(WORD, m_wSkillLevel)	// ���ܵȼ�
Declare_Msg_End

// ���Ἴ���б�����
Declare_Msg_Begin(C2S_GUILD_SKILL_LIST_REQ, MSG_C2S_GUILD_SKILL_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���Ἴ���б���Ӧ
Declare_Msg_Begin(S2C_GUILD_SKILL_LIST_RESP, MSG_S2C_GUILD_SKILL_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_bySelfSkillNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_bySelfSkillNum)
Declare_EncodeMsg_Memory(m_SelfSkillList, m_bySelfSkillNum*sizeof(SkillData));
Declare_EncodeMsg_UnsignedShort(m_byGuildSkillNum)
Declare_EncodeMsg_Memory(m_GuildSkillList, m_byGuildSkillNum*sizeof(SkillData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_bySelfSkillNum)		// �Լ��ļ�������
Declare_Msg_Member_Array(SkillData, m_SelfSkillList, MAX_GUILD_SKILL_NUM);	// �Լ��ļ����б�
Declare_Msg_Member(BYTE, m_byGuildSkillNum)		// ���Ἴ������
Declare_Msg_Member_Array(SkillData, m_GuildSkillList, MAX_GUILD_SKILL_NUM);	// ���Ἴ���б�
Declare_Msg_End


// ���Ÿ�����������
Declare_Msg_Begin(C2S_GUILD_FB_DATA_REQ, MSG_C2S_GUILD_FB_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���Ÿ���������Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_DATA_RESP, MSG_S2C_GUILD_FB_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_FBData.wCurPassChapter);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTotalHP);
Declare_EncodeMsg_UnsignedInt(m_dwDamage);
Declare_EncodeMsg_UnsignedShort(m_FBData.wCurPassChapter);
Declare_EncodeMsg_UnsignedShort(m_FBData.wTodayStartChapter);
Declare_EncodeMsg_UnsignedChar(m_FBData.wIsFromHistoryChapter);
Declare_EncodeMsg_UnsignedChar(m_FBData.byBattleTimes);
Declare_EncodeMsg_UnsignedChar(m_FBData.byBuyTimes);
Declare_EncodeMsg_UnsignedInt(m_FBData.dwLastBattleTimesRecoverTime);
Declare_EncodeMsg_UnsignedShort(m_FBData.vTakeChapterRewardRecord.size());
for (size_t i = 0; i < m_FBData.vTakeChapterRewardRecord.size(); ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_FBData.vTakeChapterRewardRecord[i]);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTotalHP)	// �ܵ�HP
Declare_Msg_Member(DWORD, m_dwDamage)	// �˺�
Declare_Msg_Member(GuildFBData, m_FBData) // ��������
Declare_Msg_End

// ���Ÿ����½ڽ�������
Declare_Msg_Begin(C2S_GUILD_FB_CHAPTER_REWARD_REQ, MSG_C2S_GUILD_FB_CHAPTER_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ��ȡ���½�
Declare_Msg_End

// ���Ÿ����½ڽ�����Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_CHAPTER_REWARD_RESP, MSG_S2C_GUILD_FB_CHAPTER_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ��ȡ���½�
Declare_Msg_End

// ���Ÿ����ؿ���������
Declare_Msg_Begin(C2S_GUILD_FB_MAP_REWARD_REQ, MSG_C2S_GUILD_FB_MAP_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_UnsignedChar(m_byBoxIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_UnsignedChar(m_byBoxIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ��ȡ���½�
Declare_Msg_Member(BYTE, m_byCamp)		// ��ȡ����Ӫ
Declare_Msg_Member(BYTE, m_byBoxIdx)	// ��ȡ�ı�������(��0��ʼ)
Declare_Msg_End

// ���Ÿ����ؿ�������Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_MAP_REWARD_RESP, MSG_S2C_GUILD_FB_MAP_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_UnsignedChar(m_byBoxIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_UnsignedChar(m_byBoxIdx)
Declare_EncodeMsg_UnsignedChar(m_byRewardIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ��ȡ���½�
Declare_Msg_Member(BYTE, m_byCamp)		// ��ȡ����Ӫ
Declare_Msg_Member(BYTE, m_byBoxIdx)	// ��ȡ�ı�������(��0��ʼ,�ڽ����λ��)
Declare_Msg_Member(BYTE, m_byRewardIdx)	// ��ȡ�Ľ�������(��0��ʼ,�ڱ����λ��)
Declare_Msg_End

// ���Ÿ����ؿ�������¼����
Declare_Msg_Begin(C2S_GUILD_FB_MAP_REWARD_RECORD_REQ, MSG_C2S_GUILD_FB_MAP_REWARD_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ������½�
Declare_Msg_Member(BYTE, m_byCamp)		// �������Ӫ
Declare_Msg_End

// ���Ÿ����ؿ�������¼��Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_MAP_REWARD_RECORD_RESP, MSG_S2C_GUILD_FB_MAP_REWARD_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
m_wRecordNum = min(m_wRecordNum, MAX_GUILD_MEMBER_NUM);
Declare_EncodeMsg_Short(m_wRecordNum)
for (WORD i = 0; i < m_wRecordNum; ++i)
{
	Declare_EncodeMsg_Short(m_RecordList[i].wBoxIdx);
	Declare_EncodeMsg_Short(m_RecordList[i].wRewardIdx);
	Declare_EncodeMsg_String(m_RecordList[i].szRoleName, sizeof(m_RecordList[i].szRoleName)-1);
}
//Declare_EncodeMsg_Memory(m_RecordList, m_wRecordNum*sizeof(GuildFBBox));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// ������½�
Declare_Msg_Member(BYTE, m_byCamp)		// �������Ӫ
Declare_Msg_Member(WORD, m_wRecordNum)		// ��¼����
Declare_Msg_Member_Array(GuildFBBox, m_RecordList, MAX_GUILD_MEMBER_NUM);	// ��¼�б�
Declare_Msg_Member_Array(BYTE, m_RewardStatusList, CAMP_NUM);	// �ؿ�����״̬�б�(0=δͨ��,1=δ��ȡ,2=����ȡ) todo::��ʱ��ʹ��
Declare_Msg_End


// ���Ÿ����˺���¼����
Declare_Msg_Begin(C2S_GUILD_FB_DAMAGE_RECORD_REQ, MSG_C2S_GUILD_FB_DAMAGE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���Ÿ����˺���¼��Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_DAMAGE_RECORD_RESP, MSG_S2C_GUILD_FB_DAMAGE_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDamageNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDamageNum)
for (BYTE i = 0; i < m_wDamageNum; ++i)
{
	GuildFBDamageData &tDamageData = m_DamageList[i];
	Declare_EncodeMsg_UnsignedShort(tDamageData.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tDamageData.dwDamage);
	Declare_EncodeMsg_UnsignedChar(tDamageData.byBattleTimes);
	Declare_EncodeMsg_String(tDamageData.szRoleName, sizeof(tDamageData.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDamageNum)		// �˺�����
Declare_Msg_Member_Array(GuildFBDamageData, m_DamageList, MAX_GUILD_MEMBER_NUM);	// Ŀ���б�
Declare_Msg_End

// ���Ÿ�����ս��������
Declare_Msg_Begin(C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ, MSG_C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// �������
Declare_Msg_End

// ���Ÿ�����ս����������Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP, MSG_S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// �������
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_GUILD_IMPEACH_REQ, MSG_C2S_GUILD_IMPEACH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������Ӧ
Declare_Msg_Begin(S2C_GUILD_IMPEACH_RESP, MSG_S2C_GUILD_IMPEACH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����֪ͨ
Declare_Msg_Begin(S2C_GUILD_IMPEACH_NOTICE, MSG_S2C_GUILD_IMPEACH_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwNewCaptainId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwNewCaptainId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwNewCaptainId)		// �¶ӳ�ID
Declare_Msg_End

// ����ؿ�������������
Declare_Msg_Begin(C2S_GUILD_MONSTER_DATA_REQ, MSG_C2S_GUILD_MONSTER_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ؿ�����������Ӧ
Declare_Msg_Begin(S2C_GUILD_MONSTER_DATA_RESP, MSG_S2C_GUILD_MONSTER_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCampNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCampNum)
for (WORD i = 0; i < m_wCampNum; ++i)
{
	vector<MonsterData> &vMonsterList = m_MonsterList[i];
	//Declare_EncodeMsg_UnsignedChar(m_RewardStatusList[i]);
	Declare_EncodeMsg_String(m_KillerList[i].c_str(), MAX_ROLE_LEN-1);
	Declare_EncodeMsg_UnsignedShort(vMonsterList.size())
	for (size_t k = 0; k < vMonsterList.size(); ++k)
	{
		Declare_EncodeMsg_UnsignedChar(vMonsterList[k].byIndex);
		Declare_EncodeMsg_UnsignedInt(vMonsterList[k].dwDamage);
		Declare_EncodeMsg_UnsignedInt(vMonsterList[k].dwTotalHP);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCampNum)			// ��Ӫ����
Declare_Msg_Member_Array(vector<MonsterData>, m_MonsterList, CAMP_NUM);	// �����б�
Declare_Msg_Member_Array(string, m_KillerList, CAMP_NUM);	// ��ɱ���б�
//Declare_Msg_Member_Array(BYTE, m_RewardStatusList, CAMP_NUM);	// �ؿ�����״̬�б�(0=δͨ��,1=δ��ȡ,2=����ȡ)
Declare_Msg_End

// ���ḱ���½�����
Declare_Msg_Begin(C2S_GUILD_FB_CHAPTER_RESET_REQ, MSG_C2S_GUILD_FB_CHAPTER_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsHistoryRecord)		// �Ƿ����õ���ʷ��¼
Declare_Msg_End


// ���ḱ���½�������Ӧ
Declare_Msg_Begin(S2C_GUILD_FB_CHAPTER_RESET_RESP, MSG_S2C_GUILD_FB_CHAPTER_RESET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsHistoryRecord)		// �Ƿ����õ���ʷ��¼
Declare_Msg_End

// ��������˺�֪ͨ
Declare_Msg_Begin(S2C_GUILD_MONSNTER_DAMAGE_NOTIFY, MSG_S2C_GUILD_MONSNTER_DAMAGE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwDamage)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_UnsignedInt(m_dwDamage)
Declare_EncodeMsg_UnsignedInt(m_dwCurTotalHP)
Declare_EncodeMsg_String(m_szAttackName, sizeof(m_szAttackName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)		// �½�
Declare_Msg_Member(BYTE, m_byCamp)		// ��Ӫ
Declare_Msg_Member(DWORD, m_dwDamage)		// �˺�
Declare_Msg_Member(DWORD, m_dwCurTotalHP)	// ��ǰ��HP
Declare_Msg_Member_Array(char, m_szAttackName, MAX_ROLE_LEN); // ����������
Declare_Msg_End


// �ҵĹؿ�������¼����
Declare_Msg_Begin(C2S_GUILD_MY_MAP_REWARD_RECORD_REQ, MSG_C2S_GUILD_MY_MAP_REWARD_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �ҵĹؿ�������¼��Ӧ
Declare_Msg_Begin(S2C_GUILD_MY_MAP_REWARD_RECORD_RESP, MSG_S2C_GUILD_MY_MAP_REWARD_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRecordNum)
for (WORD i = 0; i < m_wRecordNum; ++i)
{
	MyGuildMapRewardRecord &tRecord = m_RewardRecordList[i];
	Declare_EncodeMsg_UnsignedShort(tRecord.wChapter);
	Declare_EncodeMsg_UnsignedShort(CAMP_NUM);
	for (WORD k = 0; k < CAMP_NUM; ++k)
	{
		Declare_EncodeMsg_UnsignedChar(k);
		Declare_EncodeMsg_UnsignedChar(tRecord.arRecordList[k]);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRecordNum)		// ��¼����
Declare_Msg_Member_Array(MyGuildMapRewardRecord, m_RewardRecordList, MAX_GUILD_CHAPTER_NUM);
Declare_Msg_End




///////////////////////////////////////////////////////////////////////////////////////
// ������ģ��

// ��������սĿ���б�����
Declare_Msg_Begin(C2S_ARENA_TARGET_LIST_REQ, MSG_C2S_ARENA_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������սĿ���б���Ӧ
Declare_Msg_Begin(S2C_ARENA_TARGET_LIST_RESP, MSG_S2C_ARENA_TARGET_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRank)
Declare_EncodeMsg_UnsignedShort(m_wHistoryRank)
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
for (BYTE i = 0; i < m_byTargetNum; ++i)
{
	ArenaTargetData &tTargetData = m_TargetList[i];
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwTargetId);
	Declare_EncodeMsg_UnsignedShort(tTargetData.wRank);
	Declare_EncodeMsg_String(tTargetData.szTargetName, sizeof(tTargetData.szTargetName) - 1);
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwFightingPower);
	Declare_EncodeMsg_UnsignedShort(tTargetData.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwTitleId);
	Declare_EncodeMsg_UnsignedInt(tTargetData.dwNicknameId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRank)				// �Լ�������
Declare_Msg_Member(WORD, m_wHistoryRank)		// ��ʷ����
Declare_Msg_Member(BYTE, m_byTargetNum)			// Ŀ������
Declare_Msg_Member_Array(ArenaTargetData, m_TargetList, MAX_ARENA_TARGET_NUM);	// Ŀ���б�
Declare_Msg_End

// ������ս������
Declare_Msg_Begin(C2S_ARENA_BATTLE_RECORD_REQ, MSG_C2S_ARENA_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������ս����Ӧ
Declare_Msg_Begin(S2C_ARENA_BATTLE_RECORD_RESP, MSG_S2C_ARENA_BATTLE_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRecordNum)
for (BYTE i = 0; i < m_byRecordNum; ++i)
{
	ArenaBattleRecord &tRecord = m_RecordList[i];
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTime);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTargetId);
	Declare_EncodeMsg_String(tRecord.szTargetName, sizeof(tRecord.szTargetName) - 1);
	Declare_EncodeMsg_UnsignedShort(tRecord.wTargetHeadIcon);
	Declare_EncodeMsg_UnsignedChar(tRecord.byIsWin);
	Declare_EncodeMsg_UnsignedChar(tRecord.byIsAttack);
	Declare_EncodeMsg_UnsignedShort(tRecord.wMyOldRank);
	Declare_EncodeMsg_UnsignedShort(tRecord.wMyNewRank);
	Declare_EncodeMsg_UnsignedShort(tRecord.wTargetLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecordNum)		// ��¼����
Declare_Msg_Member_Array(ArenaBattleRecord, m_RecordList, MAX_ARENA_BATTLE_RECORD);	// ��¼�б�
Declare_Msg_End

// ����
Declare_Msg_Begin(C2S_GUILD_APPOINT_REQ, MSG_C2S_GUILD_APPOINT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPos)		// ������ְλ
Declare_Msg_Member(DWORD, m_dwTargetId)	// ����Ŀ��ID
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_GUILD_APPOINT_RESP, MSG_S2C_GUILD_APPOINT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPos)		// ������ְλ
Declare_Msg_Member(DWORD, m_dwTargetId)	// ����Ŀ��ID
Declare_Msg_End


// �ճ������б�����
Declare_Msg_Begin(C2S_DAILY_MISSION_LIST_REQ, MSG_C2S_DAILY_MISSION_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ճ������б�������Ӧ
Declare_Msg_Begin(S2C_DAILY_MISSION_LIST_RESP, MSG_S2C_DAILY_MISSION_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_DailyMission.wPoint)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_DailyMission.wPoint)
Declare_EncodeMsg_UnsignedShort(m_DailyMission.wPointAwardRecordNum)
Declare_EncodeMsg_Memory(m_DailyMission.arPointAwardRecord, m_DailyMission.wPointAwardRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_DailyMission.wDailyMissionNum)
Declare_EncodeMsg_Memory(m_DailyMission.arDailyMissionList, m_DailyMission.wDailyMissionNum*sizeof(DailyMissionData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerDailyMissionData, m_DailyMission)	// �ճ���������
Declare_Msg_End

// �ճ���������֪ͨ
Declare_Msg_Begin(S2C_DAILY_MISSION_DATA_NOTIFY, MSG_S2C_DAILY_MISSION_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_DailyMissionData, sizeof(m_DailyMissionData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_DailyMissionData, sizeof(m_DailyMissionData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DailyMissionData, m_DailyMissionData)	// �ճ���������
Declare_Msg_End

// �ճ���������ȡ����
Declare_Msg_Begin(C2S_DAILY_MISSION_AWARD_REQ, MSG_C2S_DAILY_MISSION_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMissionId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMissionId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMissionId)	// �ճ�����Id
Declare_Msg_End


// �ճ���������ȡ��Ӧ
Declare_Msg_Begin(S2C_DAILY_MISSION_AWARD_RESP, MSG_S2C_DAILY_MISSION_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMissionId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMissionId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMissionId)	// �ճ�����Id
Declare_Msg_End

// �ճ�������ֽ�����ȡ����
Declare_Msg_Begin(C2S_DAILY_MISSION_POINT_AWARD_REQ, MSG_C2S_DAILY_MISSION_POINT_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPoint);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPoint);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPoint)	// ��ȡ�Ļ��ֽ���
Declare_Msg_End

// �ճ�������ֽ�����ȡ����
Declare_Msg_Begin(S2C_DAILY_MISSION_POINT_AWARD_RESP, MSG_S2C_DAILY_MISSION_POINT_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPoint);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPoint);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPoint)	// ��ȡ�Ļ��ֽ���
Declare_Msg_End

// �ɾ��б�����
Declare_Msg_Begin(C2S_ACHIEVEMENT_LIST_REQ, MSG_C2S_ACHIEVEMENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ɾ��б���Ӧ
Declare_Msg_Begin(S2C_ACHIEVEMENT_LIST_RESP, MSG_S2C_ACHIEVEMENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementNum)
Declare_EncodeMsg_Memory(m_AhievementList, m_wAchievementNum*sizeof(AchievementData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementNum)		// �ɾ�����
Declare_Msg_Member_Array(AchievementData, m_AhievementList, MAX_ACHIEVEMENT_NUM);	// �ɾ��б�
Declare_Msg_End


// �ɾ�����֪ͨ
Declare_Msg_Begin(S2C_ACHIEVEMENT_DATA_NOTIFY, MSG_S2C_ACHIEVEMENT_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tAchievementData, sizeof(AchievementData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tAchievementData, sizeof(AchievementData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(AchievementData, m_tAchievementData)		// �ɾ�����
Declare_Msg_End

// �ɾͽ�����ȡ����
Declare_Msg_Begin(C2S_ACHIEVEMENT_AWARD_REQ, MSG_C2S_ACHIEVEMENT_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementId)		// �ɾ�ID
Declare_Msg_End

// �ɾͽ�����ȡ��Ӧ
Declare_Msg_Begin(S2C_ACHIEVEMENT_AWARD_RESP, MSG_S2C_ACHIEVEMENT_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementId)		// �ɾ�ID
Declare_Msg_End


// ����BOSS��������
Declare_Msg_Begin(C2S_WORLD_BOSS_DATA_REQ, MSG_C2S_WORLD_BOSS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����BOSS������Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_DATA_RESP, MSG_S2C_WORLD_BOSS_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossLevel)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_UnsignedChar(m_byBattleTimes)
Declare_EncodeMsg_UnsignedInt(m_dwLastRecoveryTime)
Declare_EncodeMsg_UnsignedChar(m_byBuyBattleTimes)
Declare_EncodeMsg_UnsignedInt(m_dwMyHonor)
Declare_EncodeMsg_UnsignedInt(m_dwMyDamage)
Declare_EncodeMsg_UnsignedShort(m_wMyHonorRank)
Declare_EncodeMsg_UnsignedShort(m_wMyDamageRank)
Declare_EncodeMsg_UnsignedInt(m_dwReliveTime)
Declare_EncodeMsg_UnsignedInt(m_dwRemainHP)
Declare_EncodeMsg_UnsignedShort(m_byHonorRankNum)
for (BYTE i = 0; i < m_byHonorRankNum; ++i)
{
	ClientRoleRankData *pRankData = &m_HonorRankList[i];
	Declare_EncodeMsg_UnsignedInt(pRankData->dwValue);
	Declare_EncodeMsg_String(pRankData->szRoleName, sizeof(pRankData->szRoleName) - 1)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossLevel)				// BOSS�ȼ�
Declare_Msg_Member(BYTE, m_byCamp)					// ������Ӫ
Declare_Msg_Member(BYTE, m_byBattleTimes)			// ��ս����(�Ѿ�ʹ�õ�)
Declare_Msg_Member(DWORD, m_dwLastRecoveryTime)		// �ϴλָ�ʱ��(��ս����)
Declare_Msg_Member(BYTE, m_byBuyBattleTimes)		// �Ѿ��������ս����
Declare_Msg_Member(DWORD, m_dwMyHonor)				// �ҵ�������
Declare_Msg_Member(DWORD, m_dwMyDamage)				// �ҵ����˺�
Declare_Msg_Member(WORD, m_wMyHonorRank)			// �ҵ�����������
Declare_Msg_Member(WORD, m_wMyDamageRank)			// �ҵ����˺�����
Declare_Msg_Member(BYTE, m_byHonorRankNum)			// ������������
Declare_Msg_Member(DWORD, m_dwReliveTime)			// BOSS����ʱ��(0=��ʾ��ľ�йҵ�)
Declare_Msg_Member(DWORD, m_dwRemainHP)				// BOSSʣ��Ѫ��
Declare_Msg_Member_Array(ClientRoleRankData, m_HonorRankList, CAMP_NUM);	// ���������б�
Declare_Msg_End

// ѡ����Ӫ����
Declare_Msg_Begin(C2S_WORLD_BOSS_SELECT_CAMP_REQ, MSG_C2S_WORLD_BOSS_SELECT_CAMP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// ѡ�����Ӫ
Declare_Msg_End

// ѡ����Ӫ��Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_SELECT_CAMP_RESP, MSG_S2C_WORLD_BOSS_SELECT_CAMP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// ѡ�����Ӫ
Declare_Msg_End


// ��ȡ��������
Declare_Msg_Begin(C2S_WORLD_BOSS_REWARD_REQ, MSG_C2S_WORLD_BOSS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardType)
Declare_DecodeMsg_UnsignedShort(m_byRewardNum)
m_byRewardNum = min(m_byRewardNum, MAX_WORLD_BOSS_AWARD_NUM);
Declare_DecodeMsg_Memory(m_RewardList, m_byRewardNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)	// ��������(1=����,2=�˺�)
Declare_Msg_Member(WORD, m_byRewardNum)	// ��������
Declare_Msg_Member_Array(WORD, m_RewardList, MAX_WORLD_BOSS_AWARD_NUM);	// �����б�
Declare_Msg_End

// ��ȡ������Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_REWARD_RESP, MSG_S2C_WORLD_BOSS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byRewardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRewardType)
m_byRewardNum = min(m_byRewardNum, MAX_WORLD_BOSS_AWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Memory(m_RewardList, m_byRewardNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)	// ��������(1=����,2=�˺�)
Declare_Msg_Member(BYTE, m_byRewardNum)		// ��������
Declare_Msg_Member_Array(WORD, m_RewardList, MAX_WORLD_BOSS_AWARD_NUM);	// �����б�
Declare_Msg_End

// ��ȡ������¼����
Declare_Msg_Begin(C2S_WORLD_BOSS_REWARD_RECORD_REQ, MSG_C2S_WORLD_BOSS_REWARD_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRewardType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)		// ��������(1=����,2=�˺�)
Declare_Msg_End

// ��ȡ������¼��Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_REWARD_RECORD_RESP, MSG_S2C_WORLD_BOSS_REWARD_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRewardType)
Declare_EncodeMsg_UnsignedShort(m_byRecordNum)
Declare_EncodeMsg_Memory(m_RecordList, m_byRecordNum*sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)	// ��������(1=����,2=�˺�)
Declare_Msg_Member(BYTE, m_byRecordNum)		// ��¼����
Declare_Msg_Member_Array(WORD, m_RecordList, MAX_WORLD_BOSS_AWARD_NUM);	// ��¼�б�
Declare_Msg_End


// ����BOSSս������
Declare_Msg_Begin(C2S_WORLD_BOSS_BATTLE_RECORD_REQ, MSG_C2S_WORLD_BOSS_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����BOSSս����Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_BATTLE_RECORD_RESP, MSG_S2C_WORLD_BOSS_BATTLE_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byRecordNum)
for (BYTE i = 0; i < m_byRecordNum; i++)
{
	WorldBossBattleRecord &tRecord = m_RecordList[i];
	Declare_DecodeMsg_UnsignedShort(tRecord.wBossLevel);
	Declare_DecodeMsg_UnsignedInt(tRecord.dwLuckAttackTime);
	Declare_DecodeMsg_UnsignedInt(tRecord.dwKillTime);
	Declare_DecodeMsg_String(tRecord.szLuckAttackName, sizeof(tRecord.szLuckAttackName) - 1);
	Declare_DecodeMsg_String(tRecord.szKillName, sizeof(tRecord.szKillName) - 1);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRecordNum)
for (BYTE i = 0; i < m_byRecordNum; ++i)
{
	WorldBossBattleRecord &tRecord = m_RecordList[i];
	Declare_EncodeMsg_UnsignedShort(tRecord.wBossLevel);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwLuckAttackTime);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwKillTime);
	Declare_EncodeMsg_String(tRecord.szLuckAttackName, sizeof(tRecord.szLuckAttackName) - 1);
	Declare_EncodeMsg_String(tRecord.szKillName, sizeof(tRecord.szKillName) - 1);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecordNum)		// ��¼����
Declare_Msg_Member_Array(WorldBossBattleRecord, m_RecordList, MAX_WORLD_BOSS_BATTLE_RECORD_NUM);	// ��¼�б�
Declare_Msg_End

// ����BOSS��ս��������
Declare_Msg_Begin(C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ, MSG_C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// �������
Declare_Msg_End

// ����BOSS��ս����������Ӧ
Declare_Msg_Begin(S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP, MSG_S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// �������
Declare_Msg_End

// ����boss�˺�����
Declare_Msg_Begin(S2C_WORLD_BOSS_DAMAGE_NOTIFY, MSG_S2C_WORLD_BOSS_DAMAGE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwDamage)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwDamage)
Declare_EncodeMsg_UnsignedInt(m_dwCurTotalHP)
Declare_EncodeMsg_String(m_szAttackName, sizeof(m_szAttackName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwDamage)		// �˺�
Declare_Msg_Member(DWORD, m_dwCurTotalHP)	// ��ǰ��HP
Declare_Msg_Member_Array(char, m_szAttackName, MAX_ROLE_LEN); // ����������
Declare_Msg_End

// ����BOSS���ʼ
Declare_Msg_Begin(S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY, MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����BOSS�����
Declare_Msg_Begin(S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY, MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// �����б�����
Declare_Msg_Begin(C2S_REWARD_CENTER_LIST_REQ, MSG_C2S_REWARD_CENTER_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б���Ӧ
Declare_Msg_Begin(S2C_REWARD_CENTER_LIST_RESP, MSG_S2C_REWARD_CENTER_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
for (BYTE i = 0; i < m_byRewardNum; ++i)
{
	ClientRewardData &tReward = m_RewardList[i];
	Declare_EncodeMsg_UnsignedInt(tReward.dwUId);
	Declare_EncodeMsg_UnsignedChar(tReward.wInfoId);
	Declare_EncodeMsg_UnsignedInt(tReward.dwValue1);
	Declare_EncodeMsg_UnsignedInt(tReward.dwTime);
	Declare_EncodeMsg_String(tReward.szText, sizeof(tReward.szText) - 1);
	Declare_EncodeMsg_UnsignedShort(tReward.vItemList.size());
	if (tReward.vItemList.size())
		Declare_EncodeMsg_Memory(tReward.vItemList[0], tReward.vItemList.size()*sizeof(Item));
	Declare_EncodeMsg_UnsignedShort(tReward.vCurrencyList.size());
	if (tReward.vCurrencyList.size())
		Declare_EncodeMsg_Memory(tReward.vCurrencyList[0], tReward.vCurrencyList.size()*sizeof(MoneyData));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardNum)		// ��������
Declare_Msg_Member_Array(ClientRewardData, m_RewardList, MAX_REWARD_CENTER_REWARD_NUM);	// �����б�
Declare_Msg_End

// ������ȡ����
Declare_Msg_Begin(C2S_REWARD_CENTER_GET_REQ, MSG_C2S_REWARD_CENTER_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byRewardNum)
m_byRewardNum = min(m_byRewardNum, MAX_REWARD_CENTER_REWARD_NUM);
Declare_DecodeMsg_Memory(m_arRewardIdList, m_byRewardNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_byRewardNum)		// ��������
Declare_Msg_Member_Array(DWORD, m_arRewardIdList, MAX_REWARD_CENTER_REWARD_NUM);	// �����б�
Declare_Msg_End

// ������ȡ��Ӧ
Declare_Msg_Begin(S2C_REWARD_CENTER_GET_RESP, MSG_S2C_REWARD_CENTER_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byRewardNum = min(m_byRewardNum, MAX_REWARD_CENTER_REWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Memory(m_arRewardIdList, m_byRewardNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardNum)		// ��������
Declare_Msg_Member_Array(DWORD, m_arRewardIdList, MAX_REWARD_CENTER_REWARD_NUM);	// �����б�
Declare_Msg_End

// �콱���Ľ���֪ͨ
Declare_Msg_Begin(S2C_REWARD_NOTICE, MSG_S2C_REWARD_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_RewardData.wInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_RewardData.dwUId);
Declare_EncodeMsg_UnsignedChar(m_RewardData.wInfoId);
Declare_EncodeMsg_UnsignedInt(m_RewardData.dwValue1);
Declare_EncodeMsg_UnsignedInt(m_RewardData.dwTime);
Declare_EncodeMsg_String(m_RewardData.szText, sizeof(m_RewardData.szText) - 1);
Declare_EncodeMsg_UnsignedShort(m_RewardData.vItemList.size());
if (m_RewardData.vItemList.size())
	Declare_EncodeMsg_Memory(m_RewardData.vItemList[0], m_RewardData.vItemList.size()*sizeof(Item));
Declare_EncodeMsg_UnsignedShort(m_RewardData.vCurrencyList.size());
if (m_RewardData.vCurrencyList.size())
	Declare_EncodeMsg_Memory(m_RewardData.vCurrencyList[0], m_RewardData.vCurrencyList.size()*sizeof(MoneyData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ClientRewardData, m_RewardData)		// ��������
Declare_Msg_End

// ǩ����������
Declare_Msg_Begin(C2S_SIGN_IN_DATA_REQ, MSG_C2S_SIGN_IN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ǩ��������Ӧ
Declare_Msg_Begin(S2C_SIGN_IN_DATA_RESP, MSG_S2C_SIGN_IN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCurSignInDay)
Declare_DecodeMsg_UnsignedInt(m_dwLastSignInTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCurSignInDay)
Declare_EncodeMsg_UnsignedInt(m_dwLastSignInTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCurSignInDay)		// ��ǰǩ��������
Declare_Msg_Member(DWORD, m_dwLastSignInTime)	// ���ǩ����ʱ��
Declare_Msg_End

// ǩ������
Declare_Msg_Begin(C2S_SIGN_IN_REQ, MSG_C2S_SIGN_IN_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ǩ����Ӧ
Declare_Msg_Begin(S2C_SIGN_IN_RESP, MSG_S2C_SIGN_IN_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ǩ����������
Declare_Msg_Begin(C2S_LUXURY_SIGN_IN_DATA_REQ, MSG_C2S_LUXURY_SIGN_IN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

Declare_Msg_Begin(S2C_LUXURY_SIGN_IN_DATA_RESP, MSG_S2C_LUXURY_SIGN_IN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSignInDataNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSignInDataNum)
for (WORD i = 0; i < m_wSignInDataNum; ++i)
{
	LuxurySignData &tSignInData = m_arSignInDataLList[i];
	Declare_EncodeMsg_UnsignedShort(tSignInData.wSignId);
	Declare_EncodeMsg_UnsignedShort(tSignInData.wStatus);
	Declare_EncodeMsg_UnsignedShort(tSignInData.wRewardId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSignInDataNum)	// ǩ����������
Declare_Msg_Member_Array(LuxurySignData, m_arSignInDataLList, MAX_LUXURY_SIGN_NUM);	// ǩ�������б�
Declare_Msg_End


Declare_Msg_Begin(C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ,MSG_C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSignInId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSignInId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSignInId)		// ǩ��ID
Declare_Msg_End

Declare_Msg_Begin(S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP, MSG_S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSignInId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSignInId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSignInId)		// ǩ��ID
Declare_Msg_End



// ӭ������������
Declare_Msg_Begin(C2S_YCS_DATA_REQ, MSG_C2S_YCS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ӭ����������Ӧ
Declare_Msg_Begin(S2C_YCS_DATA_RESP, MSG_S2C_YCS_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tData, sizeof(YCSData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tData, sizeof(YCSData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(YCSData, m_tData)	// ӭ��������
Declare_Msg_End

// ӭ����������
Declare_Msg_Begin(C2S_YCS_REWARD_REQ, MSG_C2S_YCS_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ӭ��������Ӧ
Declare_Msg_Begin(S2C_YCS_REWARD_RESP, MSG_S2C_YCS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wType)
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wType)
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wType)		// ��������
Declare_Msg_Member(DWORD, m_dwValue)	// ����ֵ
Declare_Msg_End

// ͭȸ̨��������
Declare_Msg_Begin(C2S_TQT_DATA_REQ, MSG_C2S_TQT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ͭȸ̨������Ӧ
Declare_Msg_Begin(S2C_TQT_DATA_RESP, MSG_S2C_TQT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tData, sizeof(TQTData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_Memory(m_tData, sizeof(TQTData))
Declare_EncodeMsg_UnsignedInt(m_tData.dwLastGetTime)
Declare_EncodeMsg_UnsignedShort(m_tData.wTakeRecordNum)
Declare_EncodeMsg_Memory(m_tData.arTakeRecordList, m_tData.wTakeRecordNum*sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(TQTData, m_tData)
Declare_Msg_End

// ͭȸ̨��������
Declare_Msg_Begin(C2S_TQT_REWARD_REQ, MSG_C2S_TQT_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ͭȸ̨������Ӧ
Declare_Msg_Begin(S2C_TQT_REWARD_RESP, MSG_S2C_TQT_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����7��Ŀ���б�����
Declare_Msg_Begin(C2S_SEVEN_DAY_TARGET_LIST_REQ, MSG_C2S_SEVEN_DAY_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����7��Ŀ���б���Ӧ
Declare_Msg_Begin(S2C_SEVEN_DAY_TARGET_LIST_RESP, MSG_S2C_SEVEN_DAY_TARGET_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byTargetNum)
m_byTargetNum = min(m_byTargetNum, MAX_SEVEN_DAY_TARGET_NUM);
Declare_DecodeMsg_Memory(m_arTargetList, m_byTargetNum*sizeof(SevenDayTargetData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byTargetNum = min(m_byTargetNum, MAX_SEVEN_DAY_TARGET_NUM);
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
Declare_EncodeMsg_Memory(m_arTargetList, m_byTargetNum*sizeof(SevenDayTargetData));
Declare_EncodeMsg_UnsignedShort(m_byRewardRecordNum)
Declare_EncodeMsg_Memory(m_arRewardRecordList, m_byRewardRecordNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byTargetNum)		// Ŀ������
Declare_Msg_Member_Array(SevenDayTargetData, m_arTargetList, MAX_SEVEN_DAY_TARGET_NUM);	// Ŀ���б�
Declare_Msg_Member(BYTE, m_byRewardRecordNum)		// ������¼����
Declare_Msg_Member_Array(WORD, m_arRewardRecordList, MAX_SEVEN_DAY_COMPLETION_REWARD);	// ������¼�б�
Declare_Msg_End


// ��ȡ7��Ŀ�꽱������
Declare_Msg_Begin(C2S_SEVEN_DAY_GET_REWARD_REQ, MSG_C2S_SEVEN_DAY_GET_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTargetId)		// Ŀ��ID
Declare_Msg_End

// ��ȡ������Ӧ
Declare_Msg_Begin(S2C_SEVEN_DAY_GET_REWARD_RESP, MSG_S2C_SEVEN_DAY_GET_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTargetId)		// Ŀ��ID
Declare_Msg_End


// 7��Ŀ������֪ͨ
Declare_Msg_Begin(S2C_SEVEN_DAY_TARGET_DATA_NOTICE, MSG_S2C_SEVEN_DAY_TARGET_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tTargetData, sizeof(SevenDayTargetData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tTargetData, sizeof(SevenDayTargetData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SevenDayTargetData, m_tTargetData)		// Ŀ������
Declare_Msg_End


// ��ȡ��ɶȽ�������
Declare_Msg_Begin(C2S_SEVEN_DAY_COMPLETION_REWARD_REQ, MSG_C2S_SEVEN_DAY_COMPLETION_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wComplecton_num)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wComplecton_num)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wComplecton_num)		// ��ɶ�����
Declare_Msg_End

// ��ȡ��ɶȽ�����Ӧ
Declare_Msg_Begin(S2C_SEVEN_DAY_COMPLETION_REWARD_RESP, MSG_S2C_SEVEN_DAY_COMPLETION_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wComplecton_num)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wComplecton_num)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wComplecton_num)		// ��ɶ�����
Declare_Msg_End


//////////////////////////////////////////////////////
/// ����
// ���������������
Declare_Msg_Begin(C2S_PLAYER_ZHANBU_DATA_REQ, MSG_C2S_PLAYER_ZHANBU_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �������������Ӧ
Declare_Msg_Begin(S2C_PLAYER_ZHANBU_DATA_RESP, MSG_S2C_PLAYER_ZHANBU_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_pZhanBuInfo->dwHistoryCollectionValue);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwHistoryCollectionValue);				// ��ʷ�ղ�ֵ
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wLowCardGroupMapIdx);				// �ͼ�����ؿ�����
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wCurLowCardGroupId);					// ��ǰ�ͼ�����ID
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwLowCardGroupLastRecoverTime);		// �ͼ���������ָ�ʱ��
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes);	// ��ǰ�ͼ�����ӵ�е�ˢ�´���
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes);	// ���յͼ����鷭������
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwHighCardGroupLastRefreshTime);		// �߼��������ˢ��ʱ��
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wAchievementLevel);					// �ɾ͵ȼ�
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes);	// ���յͼ������Ѿ�����ˢ�µĴ���(��Ҫ���ĵ�ˢ�´���)
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes);// ���ո߼������Ѿ�����ˢ�µĴ���(��Ҫ���ĵ�ˢ�´���)
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wLowCardGroupNum);
for (WORD i = 0; i < m_pZhanBuInfo->wLowCardGroupNum; ++i)
{
	CardGroupData &tGroupData = m_pZhanBuInfo->arLowCardGroup[i];
	Declare_EncodeMsg_UnsignedInt(tGroupData.wIsAlreadyOpen ? tGroupData.dwCardId : 0);
	Declare_EncodeMsg_UnsignedChar(tGroupData.wIsAlreadyOpen);
}
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wHighCardGroupNum);
for (WORD i = 0; i < m_pZhanBuInfo->wHighCardGroupNum; ++i)
{
	CardGroupData &tGroupData = m_pZhanBuInfo->arHighCardGroup[i];
	Declare_EncodeMsg_UnsignedInt(tGroupData.wIsAlreadyOpen ? tGroupData.dwCardId : 0);
	Declare_EncodeMsg_UnsignedChar(tGroupData.wIsAlreadyOpen);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// �����������
Declare_Msg_End

// �����б�����
Declare_Msg_Begin(C2S_CARD_LIST_REQ, MSG_C2S_CARD_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����б���Ӧ
Declare_Msg_Begin(S2C_CARD_LIST_RESP, MSG_S2C_CARD_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pZhanBuInfo->wCardNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardNum);
Declare_EncodeMsg_Memory(m_CardList, m_wCardNum * sizeof(ClientCard));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// �����������
Declare_Msg_Member_Array(ClientCard, m_CardList, MAX_CARD_NUM);	// �����б�
Declare_Msg_Member(WORD, m_wCardNum)		// ��������
Declare_Msg_End

// ����ͼ���б�����
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_LIST_REQ, MSG_C2S_FAIRY_POKEDEX_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ͼ���б���Ӧ
// Declare_Msg_Begin(S2C_FAIRY_POKEDEX_LIST_RESP, MSG_S2C_FAIRY_POKEDEX_LIST_RESP)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedShort(m_pZhanBuInfo->byPokedexNum);
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->byPokedexNum);
// Declare_EncodeMsg_Memory(m_pZhanBuInfo->arPokedexList, m_pZhanBuInfo->byPokedexNum * sizeof(FairyPokedex));
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// �����������
// Declare_Msg_End


// ������������
Declare_Msg_Begin(C2S_CARD_LEVEL_UP_REQ, MSG_C2S_CARD_LEVEL_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_UnsignedShort(m_wCostFairyNum)
m_wCostFairyNum = min(m_wCostFairyNum, MAX_CARD_NUM);
Declare_DecodeMsg_Memory(m_arCostFairyList, m_wCostFairyNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// ����������UID
Declare_Msg_Member(WORD, m_wCostFairyNum)		// ��Ϊ���ĵ���������
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// ��Ϊ���ĵ������б�
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_CARD_LEVEL_UP_RESP, MSG_S2C_CARD_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_UnsignedShort(m_wFairyLevel)
Declare_EncodeMsg_UnsignedInt(m_dwFairyExp)
m_wCostFairyNum = min(m_wCostFairyNum, MAX_CARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_wCostFairyNum)
Declare_EncodeMsg_Memory(m_arCostFairyList, m_wCostFairyNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// ����������UID
Declare_Msg_Member(WORD, m_wFairyLevel)		// ����������ĵȼ�
Declare_Msg_Member(DWORD, m_dwFairyExp)		// ���������龭��
Declare_Msg_Member(BYTE, m_wCostFairyNum)		// ��Ϊ���ĵ���������
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// ��Ϊ���ĵ������б�
Declare_Msg_End


// ����ͻ������
Declare_Msg_Begin(C2S_CARD_BREAK_REQ, MSG_C2S_CARD_BREAK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// ����UID
Declare_Msg_End


// ����ͻ����Ӧ
Declare_Msg_Begin(S2C_CARD_BREAK_RESP, MSG_S2C_CARD_BREAK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// ����UID
Declare_Msg_End


// ����ˢ������
Declare_Msg_Begin(C2S_CARD_REFRESH_REQ, MSG_C2S_CARD_REFRESH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRefreshType) // ˢ������(1=�ͼ�����, 2=�߼�����) 
Declare_Msg_End


// ����ˢ����Ӧ
Declare_Msg_Begin(S2C_CARD_REFRESH_RESP, MSG_S2C_CARD_REFRESH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRefreshType) // ˢ������(1=�ͼ�����, 2=�߼�����) 
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_OPEN_CARD_REQ, MSG_C2S_OPEN_CARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wCardIdxListNum)
m_wCardIdxListNum = min(m_wCardIdxListNum, MAX_CARD_GROUP_NUM);
for (WORD i = 0; i < m_wCardIdxListNum; ++i)
{
	Declare_DecodeMsg_UnsignedShort(m_arCardIdxList[i])
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedShort(m_wCardIdxListNum)
for (WORD i = 0; i < m_wCardIdxListNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_arCardIdxList[i])
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType) // ���� (1=�ͼ�����, 2=�߼�����) 
Declare_Msg_Member(WORD, m_wCardIdxListNum) // ������������
Declare_Msg_Member_Array(WORD, m_arCardIdxList, MAX_CARD_GROUP_NUM);	// ���������б�
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_OPEN_CARD_RESP, MSG_S2C_OPEN_CARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
//Declare_DecodeMsg_UnsignedInt(m_dwDestinyCoin)
Declare_DecodeMsg_UnsignedShort(m_wCardListNum)
m_wCardListNum = min(m_wCardListNum, MAX_CARD_GROUP_NUM);
for (WORD i = 0; i < m_wCardListNum; ++i)
{
	Declare_DecodeMsg_UnsignedShort(m_arCardIdxList[i]);
	Declare_DecodeMsg_UnsignedInt(m_arCardIdList[i]);
	Declare_DecodeMsg_UnsignedInt(m_arDestinyCoinList[i]);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
//Declare_EncodeMsg_UnsignedInt(m_dwDestinyCoin)
Declare_EncodeMsg_UnsignedShort(m_wCardListNum)
for (WORD i = 0; i < m_wCardListNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_arCardIdxList[i]);
	Declare_EncodeMsg_UnsignedInt(m_arCardIdList[i]);
	Declare_EncodeMsg_UnsignedInt(m_arDestinyCoinList[i]);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType) // ���� (1=�ͼ�����, 2=�߼�����) 
Declare_Msg_Member(DWORD, m_dwDestinyCoin) // ���˱�
Declare_Msg_Member(WORD, m_wCardListNum) // ��������
Declare_Msg_Member_Array(WORD, m_arCardIdxList, MAX_CARD_GROUP_NUM);	// ���������б�
Declare_Msg_Member_Array(DWORD, m_arCardIdList, MAX_CARD_GROUP_NUM);	// ����ID�б�
Declare_Msg_Member_Array(DWORD, m_arDestinyCoinList, MAX_CARD_GROUP_NUM);	// ���˱��б�
Declare_Msg_End

// �ͼ�����ˢ�´�����������
Declare_Msg_Begin(C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ, MSG_C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ͼ�����ˢ�´���������Ӧ
Declare_Msg_Begin(S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP, MSG_S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurLowCardGroupRefreshTimes)
Declare_DecodeMsg_UnsignedInt(m_dwLowCardGroupLastRecoverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurLowCardGroupRefreshTimes)
Declare_EncodeMsg_UnsignedInt(m_dwLowCardGroupLastRecoverTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurLowCardGroupRefreshTimes)	// ��ǰ�ͼ�����ӵ�е�ˢ�´���
Declare_Msg_Member(DWORD, m_dwLowCardGroupLastRecoverTime)	// ��ǰ�ͼ�����ӵ�е�ˢ�´���
Declare_Msg_End


// �ղ��鼤������
Declare_Msg_Begin(C2S_CELLECTION_GROUP_ACTIVATE_REQ, MSG_C2S_CELLECTION_GROUP_ACTIVATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_End


// �ղ��鼤����Ӧ
Declare_Msg_Begin(S2C_CELLECTION_GROUP_ACTIVATE_RESP, MSG_S2C_CELLECTION_GROUP_ACTIVATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_UnsignedInt(m_dwHistoryCollectionValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_Member(DWORD, m_dwHistoryCollectionValue)	// �ղ���ID
Declare_Msg_End

// �ղ�����������
Declare_Msg_Begin(C2S_CELLECTION_GROUP_STAR_UP_REQ, MSG_C2S_CELLECTION_GROUP_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_End


// �ղ���������Ӧ
Declare_Msg_Begin(S2C_CELLECTION_GROUP_STAR_UP_RESP, MSG_S2C_CELLECTION_GROUP_STAR_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_UnsignedInt(m_dwHistoryCollectionValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_Member(DWORD, m_dwHistoryCollectionValue)	// �ղ���ID
Declare_Msg_End

// �ղ�������(ȡ��)����
Declare_Msg_Begin(C2S_CELLECTION_GROUP_RESET_REQ, MSG_C2S_CELLECTION_GROUP_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_End

// �ղ�������(ȡ��)��Ӧ
Declare_Msg_Begin(S2C_CELLECTION_GROUP_RESET_RESP, MSG_S2C_CELLECTION_GROUP_RESET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_UnsignedShort(m_wStar)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_UnsignedShort(m_wStar)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// �ղ���ID
Declare_Msg_Member(WORD, m_wGroupId)	// �ղ���ID
Declare_Msg_Member(WORD, m_wStar)	// ����
Declare_Msg_End


// �ղ����б�����
Declare_Msg_Begin(C2S_COLLECTION_BOOK_LIST_REQ, MSG_C2S_COLLECTION_BOOK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ղ����б���Ӧ
Declare_Msg_Begin(S2C_COLLECTION_BOOK_LIST_RESP, MSG_S2C_COLLECTION_BOOK_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCollectionBookNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCollectionBookNum)
for (WORD i = 0; i < m_wCollectionBookNum; ++i)
{
	CollectionBook &tBook = m_arCollectionBookList[i];
	Declare_EncodeMsg_UnsignedShort(tBook.wBookId);
	Declare_EncodeMsg_UnsignedShort(tBook.wCollectionGroupNum);
	for (WORD k = 0; k < tBook.wCollectionGroupNum; ++k)
	{
		CollectionGroup &tGroup = tBook.arCollectionGroupList[k];
		Declare_EncodeMsg_UnsignedShort(tGroup.wId);
		Declare_EncodeMsg_UnsignedChar(tGroup.wStar);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCollectionBookNum) // ������������
Declare_Msg_Member_Array(CollectionBook, m_arCollectionBookList, MAX_COLLECTION_BOOK_NUM);	// �ղ����б�
Declare_Msg_End


// װ����������
Declare_Msg_Begin(C2S_COLLECTION_EQUIP_CARD_REQ, MSG_C2S_COLLECTION_EQUIP_CARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_UnsignedInt(m_dwCardUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_UnsignedInt(m_dwCardUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// ��λ(��1��ʼ)
Declare_Msg_Member(WORD, m_wCardSlot)	// ����(��0��ʼ)
Declare_Msg_Member(DWORD, m_dwCardUID)	// ����UID
Declare_Msg_End

// װ��������Ӧ
Declare_Msg_Begin(S2C_COLLECTION_EQUIP_CARD_RESP, MSG_S2C_COLLECTION_EQUIP_CARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_UnsignedInt(m_dwCardUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_UnsignedInt(m_dwCardUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// ��λ(��1��ʼ)
Declare_Msg_Member(WORD, m_wCardSlot)	// ����(��0��ʼ)
Declare_Msg_Member(DWORD, m_dwCardUID)	// ����UID
Declare_Msg_End


// ж�¿�������
Declare_Msg_Begin(C2S_COLLECTION_UNLOAD_CARD_REQ, MSG_C2S_COLLECTION_UNLOAD_CARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// ��λ(��1��ʼ)
Declare_Msg_Member(WORD, m_wCardSlot)	// ����(��0��ʼ)
Declare_Msg_End

// ж�¿�����Ӧ
Declare_Msg_Begin(S2C_COLLECTION_UNLOAD_CARD_RESP, MSG_S2C_COLLECTION_UNLOAD_CARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// ��λ(��1��ʼ)
Declare_Msg_Member(WORD, m_wCardSlot)	// ����(��0��ʼ)
Declare_Msg_End


// ռ���ɾͼ�������
Declare_Msg_Begin(C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ, MSG_C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ռ���ɾͼ�����Ӧ
Declare_Msg_Begin(S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP, MSG_S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurAchievementLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurAchievementLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurAchievementLevel)		// ��ǰ�ɾ͵ȼ�
Declare_Msg_End

// ������ļ����
Declare_Msg_Begin(C2S_FAIRY_RECRUIT_REQ, MSG_C2S_FAIRY_RECRUIT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecruitType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRecruitType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecruitType)		// ��ļ����
Declare_Msg_End


// ������ļ��Ӧ
Declare_Msg_Begin(S2C_FAIRY_RECRUIT_RESP, MSG_S2C_FAIRY_RECRUIT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecruitType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRecruitType)
Declare_EncodeMsg_UnsignedShort(m_byFairyNum)
Declare_EncodeMsg_Memory(m_arCardList, m_byFairyNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecruitType)		// ��ļ����
Declare_Msg_Member(BYTE, m_byFairyNum)		// ��ļ������������
Declare_Msg_Member_Array(DWORD, m_arCardList, MAX_CARD_NUM);	// ��ļ���������б�
Declare_Msg_End

// ����ͼ����������
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_STAR_UP_REQ, MSG_C2S_FAIRY_POKEDEX_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// ����ͼ��ID
Declare_Msg_End

// ����ͼ��������Ӧ
Declare_Msg_Begin(S2C_FAIRY_POKEDEX_STAR_UP_RESP, MSG_S2C_FAIRY_POKEDEX_STAR_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
m_byCostFairyNum = min(m_byCostFairyNum, MAX_CARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byCostFairyNum)
Declare_EncodeMsg_Memory(m_arCostFairyList, m_byCostFairyNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// ���ǵ�ͼ��ID
Declare_Msg_Member(BYTE, m_byCostFairyNum)		// �������ĵ���������
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// �������ĵ������б�
Declare_Msg_End


// ����ͼ����������
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_REVIVE_REQ, MSG_C2S_FAIRY_POKEDEX_REVIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// ������ͼ��ID
Declare_Msg_End


// ����ͼ��������Ӧ
Declare_Msg_Begin(S2C_FAIRY_POKEDEX_REVIVE_RESP, MSG_S2C_FAIRY_POKEDEX_REVIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// ������ͼ��ID
Declare_Msg_End


// ����ؿ�ˢ������
Declare_Msg_Begin(C2S_FAIRY_MAP_REFRESH_REQ, MSG_C2S_FAIRY_MAP_REFRESH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ؿ�ˢ����Ӧ
Declare_Msg_Begin(S2C_FAIRY_MAP_REFRESH_RESP, MSG_S2C_FAIRY_MAP_REFRESH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ͼ����������
Declare_Msg_Begin(C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ, MSG_C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ͼ��������Ӧ
Declare_Msg_Begin(S2C_FAIRY_ACHIEVEMENT_ACTIVATION_RESP, MSG_S2C_FAIRY_ACHIEVEMENT_ACTIVATION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPokedexAchievementLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPokedexAchievementLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPokedexAchievementLevel)		// ͼ���ɾ͵ȼ�
Declare_Msg_End


// ��������֪ͨ
Declare_Msg_Begin(S2C_FAIRY_DATA_NOTICE, MSG_S2C_FAIRY_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Memory(m_tCard, sizeof(ClientCard))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Memory(m_tCard, sizeof(ClientCard))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag)		// ������־(0=����,1=���,����)
Declare_Msg_Member(ClientCard, m_tCard)		// ��������
Declare_Msg_End

// ����ͼ������֪ͨ
Declare_Msg_Begin(S2C_FAIRY_POKEDEX_DATA_NOTICE, MSG_S2C_FAIRY_POKEDEX_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tPokedex, sizeof(FairyPokedex))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tPokedex, sizeof(FairyPokedex))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FairyPokedex, m_tPokedex)		// ����ͼ��
Declare_Msg_End


// ��������������
Declare_Msg_Begin(C2S_SCORE_MATCH_DATA_REQ, MSG_C2S_SCORE_MATCH_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������Ӧ
Declare_Msg_Begin(S2C_SCORE_MATCH_DATA_RESP, MSG_S2C_SCORE_MATCH_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_MatchData.wBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurActivityIdx)
Declare_EncodeMsg_UnsignedShort(m_MatchData.wBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_MatchData.wRefreshTimes)
Declare_EncodeMsg_UnsignedShort(m_MatchData.wBuyBattleTimes)
Declare_EncodeMsg_UnsignedChar(m_MatchData.byCamp)
Declare_EncodeMsg_UnsignedShort(m_MatchData.wWinTimes)
Declare_EncodeMsg_UnsignedShort(m_wBattleTargetNum)
for (WORD i = 0; i < m_wBattleTargetNum && i < m_MatchData.wBattleTargetNum; ++i)
{
	PlayerCommonData &tTarget = m_BattleTargetList[i];
	Declare_EncodeMsg_UnsignedInt(tTarget.dwRoleId);
	Declare_EncodeMsg_UnsignedInt(tTarget.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tTarget.dwTitleId);
	Declare_EncodeMsg_String(tTarget.szRoleName, sizeof(tTarget.szRoleName));
	Declare_EncodeMsg_UnsignedShort(tTarget.wServerId);
	Declare_EncodeMsg_UnsignedInt(tTarget.dwFightingPower);

	Declare_EncodeMsg_UnsignedChar(m_MatchData.arBattleTargetList[i].wStatus);
}
Declare_EncodeMsg_UnsignedShort(m_MatchData.wTaskRewardRecordNum)
Declare_EncodeMsg_Memory(m_MatchData.arTaskRewardRecord, m_MatchData.wTaskRewardRecordNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurActivityIdx)		// ��ǰ�����
Declare_Msg_Member(ScoreMatchData, m_MatchData)		// ��������
Declare_Msg_Member_Array(PlayerCommonData, m_BattleTargetList, MAX_SCORE_MATCH_BATTLE_TARGET_NUM); // ��սĿ���б�
Declare_Msg_Member(WORD, m_wBattleTargetNum)		// ��սĿ������
Declare_Msg_End

// ѡ����Ӫ
Declare_Msg_Begin(C2S_SCORE_MATCH_SELECT_CAMP_REQ, MSG_C2S_SCORE_MATCH_SELECT_CAMP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// ѡ�����Ӫ
Declare_Msg_End

// ѡ����Ӫ��Ӧ
Declare_Msg_Begin(S2C_SCORE_MATCH_SELECT_CAMP_RESP, MSG_S2C_SCORE_MATCH_SELECT_CAMP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// ѡ�����Ӫ
Declare_Msg_End


// ˢ��Ŀ��
Declare_Msg_Begin(C2S_SCORE_MATCH_REFRESH_TARGET_REQ, MSG_C2S_SCORE_MATCH_REFRESH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ˢ��Ŀ����Ӧ
Declare_Msg_Begin(S2C_SCORE_MATCH_REFRESH_TARGET_RESP, MSG_S2C_SCORE_MATCH_REFRESH_TARGET_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������ս����
Declare_Msg_Begin(C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ, MSG_C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������ս������Ӧ
Declare_Msg_Begin(S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP, MSG_S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ս������
Declare_Msg_Begin(C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ, MSG_C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskNum)
m_wTaskNum = min(m_wTaskNum, MAX_SCORE_MATCH_TASK_NUM);
Declare_DecodeMsg_Memory(m_TaskList, m_wTaskNum*sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_wTaskNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskNum)		
Declare_Msg_Member_Array(WORD, m_TaskList, MAX_SCORE_MATCH_TASK_NUM); // ��ȡ�������б�
Declare_Msg_End

// ��ȡ��ս��������Ӧ
Declare_Msg_Begin(S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP, MSG_S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wTaskNum = min(m_wTaskNum, MAX_SCORE_MATCH_TASK_NUM);
Declare_EncodeMsg_UnsignedShort(m_wTaskNum)
Declare_EncodeMsg_Memory(m_TaskList, m_wTaskNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskNum)		// ѡ�����Ӫ
Declare_Msg_Member_Array(WORD, m_TaskList, MAX_SCORE_MATCH_TASK_NUM);
Declare_Msg_End


// ��������������
Declare_Msg_Begin(C2S_WARCRAFT_DATA_REQ, MSG_C2S_WARCRAFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������Ӧ
Declare_Msg_Begin(S2C_WARCRAFT_DATA_RESP, MSG_S2C_WARCRAFT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byWarcraftBattleTimes)
Declare_DecodeMsg_UnsignedChar(m_byBuyWarcraftBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byWarcraftBattleTimes)
Declare_EncodeMsg_UnsignedChar(m_byBuyWarcraftBattleTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byWarcraftBattleTimes)		// ��������ս����
Declare_Msg_Member(BYTE, m_byBuyWarcraftBattleTimes)	// �������������ս����
Declare_Msg_End

// ������������ս����
Declare_Msg_Begin(C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ, MSG_C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������ս������Ӧ
Declare_Msg_Begin(S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP, MSG_S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��սĿ������
Declare_Msg_Begin(C2S_WARCRAFT_TARGET_LIST_REQ, MSG_C2S_WARCRAFT_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��սĿ����Ӧ 
Declare_Msg_Begin(S2C_WARCRAFT_TARGET_LIST_RESP, MSG_S2C_WARCRAFT_TARGET_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byTargetNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRank)
Declare_EncodeMsg_UnsignedShort(m_byTargetNum)
for (BYTE i = 0; i < m_byTargetNum; ++i)
{
	WarcraftTargetData &tTarget = m_TargetList[i];
	Declare_EncodeMsg_UnsignedInt(tTarget.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tTarget.wHeadIcon);
	Declare_EncodeMsg_String(tTarget.szRoleName, sizeof(tTarget.szRoleName)-1);
	Declare_EncodeMsg_UnsignedShort(tTarget.wRank);
	Declare_EncodeMsg_UnsignedInt(tTarget.dwFightingPower);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRank)			// �Լ�������
Declare_Msg_Member(BYTE, m_byTargetNum)		// Ŀ������
Declare_Msg_Member_Array(WarcraftTargetData, m_TargetList, MAX_WARCRAFT_TARGET_NUM);	// Ŀ���б�
Declare_Msg_End

// ���������а�����
Declare_Msg_Begin(C2S_WARCRAFT_RANK_LIST_REQ, MSG_C2S_WARCRAFT_RANK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���������а���Ӧ 
Declare_Msg_Begin(S2C_WARCRAFT_RANK_LIST_RESP, MSG_S2C_WARCRAFT_RANK_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRankNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRankNum)
for (WORD i = 0; i < m_wRankNum; ++i)
{
	ClientWarcraftRankData &tRank = m_RankList[i];
	Declare_EncodeMsg_UnsignedInt(tRank.dwRoleId);
	Declare_EncodeMsg_UnsignedInt(tRank.dwHeadImageId);
	Declare_EncodeMsg_String(tRank.szRoleName, sizeof(tRank.szRoleName) - 1);
	Declare_EncodeMsg_UnsignedShort(tRank.wRank);
	Declare_EncodeMsg_UnsignedInt(tRank.dwFightingPower);
	Declare_EncodeMsg_UnsignedShort(tRank.wBeCheerTimes);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRankNum)		// Ŀ������
Declare_Msg_Member_Array(ClientWarcraftRankData, m_RankList, MAX_WARCRAFT_RANK_NUM);	// Ŀ���б�
Declare_Msg_End

// ��������������
Declare_Msg_Begin(C2S_WARCRAFT_CHEER_REQ, MSG_C2S_WARCRAFT_CHEER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// Ŀ��ID
Declare_Msg_End

// ������������Ӧ
Declare_Msg_Begin(S2C_WARCRAFT_CHEER_RESP, MSG_S2C_WARCRAFT_CHEER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// Ŀ��ID
Declare_Msg_End


// �����ɫ�ƺ���Ϣ
Declare_Msg_Begin(C2S_ROLE_TITLE_INFO_REQ, MSG_C2S_ROLE_TITLE_INFO_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����ɫ�ƺ���Ϣ��Ӧ
Declare_Msg_Begin(S2C_ROLE_TITLE_INFO_RESP, MSG_S2C_ROLE_TITLE_INFO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_TitleInfo.nCurUseTitleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_TitleInfo.nCurUseTitleId)
Declare_EncodeMsg_UnsignedInt(m_TitleInfo.nCurUseNicknameId)
Declare_EncodeMsg_UnsignedShort(m_TitleInfo.wTitleNum)
for (WORD i = 0; i < m_TitleInfo.wTitleNum; ++i)
{
	TitleData &tTitle = m_TitleInfo.arTitleList[i];
	Declare_EncodeMsg_UnsignedInt(tTitle.nTitleId);
	Declare_EncodeMsg_UnsignedInt(tTitle.nOverTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerTitleInfo, m_TitleInfo)	// �ƺ���Ϣ
Declare_Msg_End


// ʹ�óƺ�����
Declare_Msg_Begin(C2S_USE_TITLE_REQ, MSG_C2S_USE_TITLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nTitleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nTitleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nTitleId)	// ʹ�õĳƺ�ID
Declare_Msg_End

// ʹ�óƺ���Ӧ
Declare_Msg_Begin(S2C_USE_TITLE_RESP, MSG_S2C_USE_TITLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nTitleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nTitleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nTitleId)	// ʹ�õĳƺ�ID
Declare_Msg_End


// �������б�
Declare_Msg_Begin(C2S_RED_POINT_LIST_REQ, MSG_C2S_RED_POINT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �������б���Ӧ
Declare_Msg_Begin(S2C_RED_POINT_LIST_RESP, MSG_S2C_RED_POINT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRedPointNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRedPointNum)
Declare_EncodeMsg_Memory(m_RedPointList, m_wRedPointNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRedPointNum)		// �������
Declare_Msg_Member_Array(WORD, m_RedPointList, MAX_RED_POINT_NUM);	// ����б�
Declare_Msg_End

// ���֪ͨ
Declare_Msg_Begin(S2C_RED_POINT_NOTIFY, MSG_S2C_RED_POINT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSystemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSystemId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSystemId)		// ϵͳId
Declare_Msg_End


// VIP�����������
Declare_Msg_Begin(C2S_VIP_GIFT_DATA_REQ, MSG_C2S_VIP_GIFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// VIP���������Ӧ
Declare_Msg_Begin(S2C_VIP_GIFT_DATA_RESP, MSG_S2C_VIP_GIFT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWeekGiftNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDailyInitVipLevel)
Declare_EncodeMsg_Short(m_nVipLevelRewardRecord)
Declare_EncodeMsg_UnsignedShort(m_wWeekGiftNum)
Declare_EncodeMsg_Memory(m_WeekGiftList, m_wWeekGiftNum*sizeof(Gift));
Declare_EncodeMsg_UnsignedShort(m_wTodayRecommendRechargeRewardId)
Declare_EncodeMsg_UnsignedShort(m_wTodayRecommendRechargeRewardStatus)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDailyInitVipLevel)						// ÿ�ճ�ʼVIP�ȼ�
Declare_Msg_Member(short, m_nVipLevelRewardRecord)						// ��ǰ�Ѿ���ȡ��VIP�ȼ�����(-1=��ʾ��û����ȡ)
Declare_Msg_Member(WORD, m_wWeekGiftNum)							// �������
Declare_Msg_Member_Array(Gift, m_WeekGiftList, MAX_VIP_WEEK_GIFT_NUM);	// ÿ������б�
Declare_Msg_Member(WORD, m_wTodayRecommendRechargeRewardId)		// �����Ƽ�����Id
Declare_Msg_Member(WORD, m_wTodayRecommendRechargeRewardStatus)	// �����Ƽ�����״̬(0=������ȡ,1=����ȡ,2=����ȡ)
Declare_Msg_End


// VIPÿ�������ȡ
Declare_Msg_Begin(C2S_VIP_DAILY_GIFT_GET_REQ, MSG_C2S_VIP_DAILY_GIFT_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wVipLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wVipLevel)		// VIP�ȼ�
Declare_Msg_End


// VIPÿ�������ȡ��Ӧ
Declare_Msg_Begin(S2C_VIP_DAILY_GIFT_GET_RESP, MSG_S2C_VIP_DAILY_GIFT_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wVipLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wVipLevel)		// VIP�ȼ�
Declare_Msg_End


// VIP���������
Declare_Msg_Begin(C2S_VIP_WEEK_GIFT_BUY_REQ, MSG_C2S_VIP_WEEK_GIFT_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGiftId)		// ���ID
Declare_Msg_End


// VIP�����������Ӧ
Declare_Msg_Begin(S2C_VIP_WEEK_GIFT_BUY_RESP, MSG_S2C_VIP_WEEK_GIFT_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGiftId)		// ���ID
Declare_Msg_End


// ��ȡ�����Ƽ�����
Declare_Msg_Begin(C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ, MSG_C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ�����Ƽ����
Declare_Msg_Begin(S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP, MSG_S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ȼ������������
Declare_Msg_Begin(C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ, MSG_C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �ȼ����������Ӧ
Declare_Msg_Begin(S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP, MSG_S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityDataNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityDataNum);
Declare_EncodeMsg_Memory(m_ActivityDataList, m_wActivityDataNum*sizeof(LevelShopActivityData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityDataNum)	
Declare_Msg_Member_Array(LevelShopActivityData, m_ActivityDataList, MAX_LEVEL_SHOP_ACTIVITY_NUM);	// ������б�
Declare_Msg_End


// ����������������
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_DATA_REQ, MSG_C2S_OPEN_SERVER_FUND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����������������
Declare_Msg_Begin(S2C_OPEN_SERVER_FUND_DATA_RESP, MSG_S2C_OPEN_SERVER_FUND_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tFundData.wIsAlreadyBuyFund)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwOpenServerFundBuyTimes);
Declare_EncodeMsg_UnsignedChar(m_tFundData.wIsAlreadyBuyFund);
Declare_EncodeMsg_UnsignedShort(m_tFundData.wFundRewardTakeRecordNum);
Declare_EncodeMsg_Memory(m_tFundData.arFundRewardTakeRecord, m_tFundData.wFundRewardTakeRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_tWelfareData.wTakeWelfareRecordNum);
Declare_EncodeMsg_Memory(m_tWelfareData.arTakeWelfareRecordList, m_tWelfareData.wTakeWelfareRecordNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwOpenServerFundBuyTimes) // ���������
Declare_Msg_Member(OpenServerFundData, m_tFundData) // ��������
Declare_Msg_Member(AllPeopleWelfareData, m_tWelfareData) // ȫ��������

Declare_Msg_End

// ��������������
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_BUY_REQ, MSG_C2S_OPEN_SERVER_FUND_BUY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������Ӧ
Declare_Msg_Begin(S2C_OPEN_SERVER_FUND_BUY_RESP, MSG_S2C_OPEN_SERVER_FUND_BUY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������ȡ����
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ, MSG_C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// ����ID
Declare_Msg_End

// ������������ȡ��Ӧ
Declare_Msg_Begin(S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP, MSG_S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// ����ID
Declare_Msg_End

// ȫ������ȡ����
Declare_Msg_Begin(C2S_ALL_PEOPLE_WELFARE_TAKE_REQ, MSG_C2S_ALL_PEOPLE_WELFARE_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// ����ID
Declare_Msg_End

// ȫ������ȡ��Ӧ
Declare_Msg_Begin(S2C_ALL_PEOPLE_WELFARE_TAKE_RESP, MSG_S2C_ALL_PEOPLE_WELFARE_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// ����ID
Declare_Msg_End


// ��ʱ�Ż���������
Declare_Msg_Begin(C2S_LIMIT_PREFERENTIAL_DATA_REQ, MSG_C2S_LIMIT_PREFERENTIAL_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ʱ�Ż�������Ӧ
Declare_Msg_Begin(S2C_LIMIT_PREFERENTIAL_DATA_RESP, MSG_S2C_LIMIT_PREFERENTIAL_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wBuySchedule)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wBuySchedule)
Declare_EncodeMsg_UnsignedShort(m_Data.wRechargeId)
Declare_EncodeMsg_UnsignedInt(m_Data.dwStartPreferentialTime)
Declare_EncodeMsg_UnsignedShort(m_Data.wTakeWelfareRecordNum)
Declare_EncodeMsg_Memory(m_Data.arTakeWelfareRecordList, m_Data.wTakeWelfareRecordNum*sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(LimitPreferentialData, m_Data)		// ����ID
Declare_Msg_End

// ��ʱ�Żݸ�����ȡ����
Declare_Msg_Begin(C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ, MSG_C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// ����ID
Declare_Msg_End

// ��ʱ�Żݸ�����ȡ��Ӧ
Declare_Msg_Begin(S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP, MSG_S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// ����ID
Declare_Msg_End


// �ȼ������������
Declare_Msg_Begin(C2S_LEVEL_GIFT_DATA_REQ, MSG_C2S_LEVEL_GIFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ȼ����������Ӧ
Declare_Msg_Begin(S2C_LEVEL_GIFT_DATA_RESP, MSG_S2C_LEVEL_GIFT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevelGiftTakeRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevelGiftTakeRecordNum);
Declare_EncodeMsg_Memory(m_arLevelGiftTakeRecord, m_wLevelGiftTakeRecordNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevelGiftTakeRecordNum)
Declare_Msg_Member_Array(WORD, m_arLevelGiftTakeRecord, MAX_LEVEL_GIFT_TAKE_RECORD_NUM);	// ������б�
Declare_Msg_End


// ��ȡ�ȼ��������
Declare_Msg_Begin(C2S_TAKE_LEVEL_GIFT_REWARD_REQ, MSG_C2S_TAKE_LEVEL_GIFT_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)		// ����ID
Declare_Msg_End

// ��ȡ�ȼ��������
Declare_Msg_Begin(S2C_TAKE_LEVEL_GIFT_REWARD_RESP, MSG_S2C_TAKE_LEVEL_GIFT_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)		// ����ID
Declare_Msg_End


// ���ս�����������
Declare_Msg_Begin(C2S_NEXT_DAY_REWARD_DATA_REQ, MSG_C2S_NEXT_DAY_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���ս���������Ӧ
Declare_Msg_Begin(S2C_NEXT_DAY_REWARD_DATA_RESP, MSG_S2C_NEXT_DAY_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wIsTakeReward)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wIsTakeReward);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wIsTakeReward)		// �Ƿ�����ȡ����
Declare_Msg_End

// ���ս�����ȡ����
Declare_Msg_Begin(C2S_NEXT_DAY_REWARD_TAKE_REQ, MSG_C2S_NEXT_DAY_REWARD_TAKE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���ս�����ȡ����
Declare_Msg_Begin(S2C_NEXT_DAY_REWARD_TAKE_RESP, MSG_S2C_NEXT_DAY_REWARD_TAKE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End




// �����������
Declare_Msg_Begin(C2S_PLAYER_LEVEL_MESSAGE_REQ, MSG_C2S_PLAYER_LEVEL_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szContact, sizeof(m_szContact)-1)
Declare_DecodeMsg_String(m_szMessage, sizeof(m_szMessage)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szContact, sizeof(m_szContact))
Declare_EncodeMsg_String(m_szMessage, sizeof(m_szMessage))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szContact, MAX_CONTACT_LENGTH)	// ��ϵ��ʽ
Declare_Msg_Member_Array(char, m_szMessage, MAX_LEAVE_MESSAGE_LENGTH) // ��������
Declare_Msg_End

// ���������Ӧ
Declare_Msg_Begin(S2C_PLAYER_LEVEL_MESSAGE_RESP, MSG_S2C_PLAYER_LEVEL_MESSAGE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �ؿ������б�����
Declare_Msg_Begin(C2S_MAP_COMMENT_LIST_REQ, MSG_C2S_MAP_COMMENT_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// �ؿ�ID
Declare_Msg_End


// �ؿ������б���Ӧ
Declare_Msg_Begin(S2C_MAP_COMMENT_LIST_RESP, MSG_S2C_MAP_COMMENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentNum);
for (WORD i = 0; i < m_wCommentNum; ++i)
{
	MapCommentClient &tComment = m_CommentList[i];
	Declare_EncodeMsg_UnsignedShort(tComment.wCommentId);
	Declare_EncodeMsg_String(tComment.szRoleName, sizeof(tComment.szRoleName)-1);
	Declare_EncodeMsg_String(tComment.szText, sizeof(tComment.szText) - 1);
	Declare_EncodeMsg_UnsignedChar(tComment.byVipLevel);
	Declare_EncodeMsg_UnsignedInt(tComment.dwTime);
	Declare_EncodeMsg_UnsignedShort(tComment.wLikeTimes);
	Declare_EncodeMsg_UnsignedShort(tComment.wLikePlayerNum);
	Declare_EncodeMsg_Memory(tComment.arLikePlayerList, tComment.wLikePlayerNum*sizeof(DWORD));
// 	for (WORD k = 0; k < tComment.wLikePlayerNum; ++k)
//  	{
// 		Declare_EncodeMsg_UnsignedInt(tComment.arLikePlayerList[k]);
//  	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// �ؿ�ID
Declare_Msg_Member(WORD, m_wCommentNum)		// ��������
Declare_Msg_Member_Array(MapCommentClient, m_CommentList, MAX_MAP_COMMENT_NUM)	// �����б�
Declare_Msg_End


// �ؿ���������
Declare_Msg_Begin(C2S_MAP_COMMENT_REQ, MSG_C2S_MAP_COMMENT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_String(m_szText, sizeof(m_szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// �ؿ�ID
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_128)	// ��������
Declare_Msg_End


// �ؿ�������Ӧ
Declare_Msg_Begin(S2C_MAP_COMMENT_RESP, MSG_S2C_MAP_COMMENT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_UnsignedShort(m_wCommentId)
Declare_DecodeMsg_UnsignedInt(m_dwTime)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentId);
Declare_EncodeMsg_UnsignedInt(m_dwTime);
Declare_EncodeMsg_String(m_szText, sizeof(m_szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// �ؿ�ID
Declare_Msg_Member(WORD, m_wCommentId)	// ����ID
Declare_Msg_Member(DWORD, m_dwTime)		// ����ʱ��
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_128)	// ��������
Declare_Msg_End


// �ؿ����۵���
Declare_Msg_Begin(C2S_MAP_COMMENT_LIKE_REQ, MSG_C2S_MAP_COMMENT_LIKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_UnsignedShort(m_wCommentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)	// �ؿ�ID
Declare_Msg_Member(WORD, m_wCommentId) // ����ID
Declare_Msg_End

// �ؿ����۵�����Ӧ
Declare_Msg_Begin(S2C_MAP_COMMENT_LIKE_RESP, MSG_S2C_MAP_COMMENT_LIKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_UnsignedShort(m_wCommentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)	// �ؿ�ID
Declare_Msg_Member(WORD, m_wCommentId) // ����ID
Declare_Msg_End

// ��ֵ��������
Declare_Msg_Begin(C2S_RECHARGE_DATA_REQ, MSG_C2S_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ֵ������Ӧ
Declare_Msg_Begin(S2C_RECHARGE_DATA_RESP, MSG_S2C_RECHARGE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pRechargetData->wRechargeNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_pRechargetData->wRechargeNum);
for (WORD i = 0; i < m_pRechargetData->wRechargeNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_pRechargetData->arRechargeList[i].wRechargeId);
	Declare_EncodeMsg_UnsignedShort(m_pRechargetData->arRechargeList[i].wRechargeTimes);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleRechargeData*, m_pRechargetData) // ��ֵ����
Declare_Msg_End


// ��ֵ����
Declare_Msg_Begin(C2S_RECHARGE_REQ, MSG_C2S_RECHARGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_UnsignedShort(m_wRechargeType);
Declare_DecodeMsg_UnsignedShort(m_wGoodsId);
Declare_DecodeMsg_UnsignedInt(m_dwContext);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRechargeId);
Declare_EncodeMsg_UnsignedShort(m_wRechargeType);
Declare_EncodeMsg_UnsignedShort(m_wGoodsId);
Declare_EncodeMsg_UnsignedInt(m_dwContext);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRechargeId)		// ��ֵID
Declare_Msg_Member(WORD, m_wRechargeType)	// ��ֵ����
Declare_Msg_Member(WORD, m_wGoodsId)		// ��ֵ����
Declare_Msg_Member(DWORD, m_dwContext)		// ��ֵ����
Declare_Msg_End


// ��ֵ��Ӧ
Declare_Msg_Begin(S2C_RECHARGE_RESP, MSG_S2C_RECHARGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_UnsignedShort(m_wRechargeType);
Declare_DecodeMsg_UnsignedShort(m_wGoodsId);
Declare_DecodeMsg_UnsignedInt(m_dwContext);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRechargeId);
Declare_EncodeMsg_UnsignedShort(m_wRechargeType);
Declare_EncodeMsg_UnsignedShort(m_wGoodsId);
Declare_EncodeMsg_UnsignedInt(m_dwContext);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRechargeId)		// ��ֵID
Declare_Msg_Member(WORD, m_wRechargeType)	// ��ֵ����
Declare_Msg_Member(WORD, m_wGoodsId)		// ��ֵ����
Declare_Msg_Member(DWORD, m_dwContext)		// ��ֵ����
Declare_Msg_End


// �¿���������
Declare_Msg_Begin(C2S_MONTH_CARD_DATA_REQ, MSG_C2S_MONTH_CARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �¿�������Ӧ
Declare_Msg_Begin(S2C_MONTH_CARD_DATA_RESP, MSG_S2C_MONTH_CARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pRechargetData->wMontchCardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_pRechargetData->wMontchCardNum);
Declare_EncodeMsg_Memory(m_pRechargetData->arMonthCardList, m_pRechargetData->wMontchCardNum*sizeof(MonthCard));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleRechargeData*, m_pRechargetData) // ��ֵ����
Declare_Msg_End

// �¿�����
Declare_Msg_Begin(C2S_MONTH_CARD_BUY_REQ, MSG_C2S_MONTH_CARD_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // �¿�ID
Declare_Msg_End

// �¿�������Ӧ
Declare_Msg_Begin(S2C_MONTH_CARD_BUY_RESP, MSG_S2C_MONTH_CARD_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // �¿�ID
Declare_Msg_End

// ��ȡ�¿�����
Declare_Msg_Begin(C2S_TAKE_MONTH_CARD_REWARD_REQ, MSG_C2S_TAKE_MONTH_CARD_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // �¿�ID
Declare_Msg_End

// ��ȡ�¿�������Ӧ
Declare_Msg_Begin(S2C_TAKE_MONTH_CARD_REWARD_RESP, MSG_S2C_TAKE_MONTH_CARD_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // �¿�ID
Declare_Msg_End


// ��ֵ֪ͨ
Declare_Msg_Begin(S2C_RECHARGE_NOTIFY_RESP, MSG_S2C_RECHARGE_NOTIFY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRechargeType)
Declare_DecodeMsg_UnsignedShort(m_wRechargeId)
Declare_DecodeMsg_UnsignedInt(m_dwContext);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRechargeType);
Declare_EncodeMsg_UnsignedShort(m_wRechargeId);
Declare_EncodeMsg_UnsignedInt(m_dwContext);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRechargeType) // ��ֵ����
Declare_Msg_Member(WORD, m_wRechargeId) // ��ֵID
Declare_Msg_Member(DWORD, m_dwContext) // ������
Declare_Msg_End


// ����Ŀ��
Declare_Msg_Begin(C2S_PVP_SEARCH_TARGET_REQ, MSG_C2S_PVP_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritSkillId) // ��ս���鼼��ID
Declare_Msg_End

// ����Ŀ����Ӧ
Declare_Msg_Begin(S2C_PVP_SEARCH_TARGET_RESP, MSG_S2C_PVP_SEARCH_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritSkillId) // ��ս���鼼��ID
Declare_Msg_End

// ս��׼��֪ͨ
Declare_Msg_Begin(S2C_PVP_READY_NOTIFY, MSG_S2C_PVP_READY_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRobot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRobot);
// �췽����
PVPRoleBattleData &tRed = m_pBattleData->arRoleBattleDataList[PVP_ACT_RED];
Declare_EncodeMsg_UnsignedInt(tRed.dwRoleId);
Declare_EncodeMsg_String(tRed.szRoleName, sizeof(tRed.szRoleName));
Declare_EncodeMsg_UnsignedInt(tRed.dwServerId);
Declare_EncodeMsg_UnsignedInt(tRed.dwFightingPower);
Declare_EncodeMsg_UnsignedInt(tRed.dwToBattleSpiritId);
Declare_EncodeMsg_UnsignedInt(tRed.dwSpiritSkillId);
Declare_EncodeMsg_UnsignedInt(tRed.wHeadImageId);
Declare_EncodeMsg_UnsignedShort(tRed.wSpriteEnhanceLevel);
Declare_EncodeMsg_UnsignedShort(tRed.wSpriteTrainStage);
Declare_EncodeMsg_UnsignedShort(tRed.wSpriteTrainTimes);
Declare_EncodeMsg_UnsignedShort(tRed.wSpriteStar);
Declare_EncodeMsg_UnsignedShort(tRed.wHeroNum);
for (WORD i = 0; i < tRed.wHeroNum; ++i)
{
	PVPHeroData &tHero = tRed.arHeroList[i];
	Declare_EncodeMsg_UnsignedShort(tHero.wHeroInfoId);
	Declare_EncodeMsg_UnsignedShort(tHero.byProNum);
	Declare_EncodeMsg_Memory(tHero.arPorList, tHero.byProNum * sizeof(Property));
	Declare_EncodeMsg_UnsignedInt(tHero.tBlessSpriteData.dwSpriteId);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wStar);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wEnhanceLevel);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wTrainStage);
}
 Declare_EncodeMsg_UnsignedShort(tRed.wSpriteNum);
 for (WORD i = 0; i < tRed.wSpriteNum; ++i)
 {
 	Declare_EncodeMsg_UnsignedInt(tRed.arSpriteList[i].dwId);
 	Declare_EncodeMsg_UnsignedShort(tRed.arSpriteList[i].wTrainStage);
 }
// ��������
PVPRoleBattleData &tBlue = m_pBattleData->arRoleBattleDataList[PVP_ACT_BLUE];
Declare_EncodeMsg_UnsignedInt(tBlue.dwRoleId);
Declare_EncodeMsg_String(tBlue.szRoleName, sizeof(tBlue.szRoleName));
Declare_EncodeMsg_UnsignedInt(tBlue.dwServerId);
Declare_EncodeMsg_UnsignedInt(tBlue.dwFightingPower);
Declare_EncodeMsg_UnsignedInt(tBlue.dwToBattleSpiritId);
Declare_EncodeMsg_UnsignedInt(tBlue.dwSpiritSkillId);
Declare_EncodeMsg_UnsignedInt(tBlue.wHeadImageId);
Declare_EncodeMsg_UnsignedShort(tBlue.wSpriteEnhanceLevel);
Declare_EncodeMsg_UnsignedShort(tBlue.wSpriteTrainStage);
Declare_EncodeMsg_UnsignedShort(tBlue.wSpriteTrainTimes);
Declare_EncodeMsg_UnsignedShort(tBlue.wSpriteStar);
Declare_EncodeMsg_UnsignedShort(tBlue.wHeroNum);
for (WORD i = 0; i < tBlue.wHeroNum; ++i)
{
	PVPHeroData &tHero = tBlue.arHeroList[i];
	Declare_EncodeMsg_UnsignedShort(tHero.wHeroInfoId);
	Declare_EncodeMsg_UnsignedShort(tHero.byProNum);
	Declare_EncodeMsg_Memory(tHero.arPorList, tHero.byProNum * sizeof(Property));
	Declare_EncodeMsg_UnsignedInt(tHero.tBlessSpriteData.dwSpriteId);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wStar);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wEnhanceLevel);
	Declare_EncodeMsg_UnsignedShort(tHero.tBlessSpriteData.wTrainStage);
}
Declare_EncodeMsg_UnsignedShort(tBlue.wSpriteNum);
for (WORD i = 0; i < tBlue.wSpriteNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(tBlue.arSpriteList[i].dwId);
	Declare_EncodeMsg_UnsignedShort(tBlue.arSpriteList[i].wTrainStage);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsRobot) // �Ƿ������
Declare_Msg_Member(PVPBattlefieldData*, m_pBattleData) // ս������
Declare_Msg_End



// ս��׼���������
Declare_Msg_Begin(C2S_PVP_READY_FINISH_REQ, MSG_C2S_PVP_READY_FINISH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ս��׼�������Ӧ
Declare_Msg_Begin(S2C_PVP_READY_FINISH_RESP, MSG_S2C_PVP_READY_FINISH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ս����ʼ֪ͨ
Declare_Msg_Begin(S2C_PVP_BATTLE_START_NOTIFY, MSG_S2C_PVP_BATTLE_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ս������֪ͨ
Declare_Msg_Begin(S2C_PVP_BATTLE_OVER_NOTIFY, MSG_S2C_PVP_BATTLE_OVER_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwWinnerId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwWinnerId);
Declare_EncodeMsg_UnsignedShort(m_wRedOldRank);
Declare_EncodeMsg_UnsignedShort(m_wRedNewRank);
Declare_EncodeMsg_UnsignedShort(m_wRedLastWeekRank);
Declare_EncodeMsg_UnsignedShort(m_wRedScore);
Declare_EncodeMsg_UnsignedShort(m_wBlueOldRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueNewRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueLastWeekRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueScore);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwWinnerId) // ��ʤ��ID
Declare_Msg_Member(WORD, m_wRedOldRank) // �췽������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedNewRank) // �췽������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedLastWeekRank) // �췽��������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedScore) // �췽���λ�û���
Declare_Msg_Member(WORD, m_wBlueOldRank) // ����������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueNewRank) // ����������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueLastWeekRank) // ������������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueScore) // �������λ�û���
Declare_Msg_End


// ʹ�ü���
Declare_Msg_Begin(C2S_PVP_USE_SKILL_REQ, MSG_C2S_PVP_USE_SKILL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwObjId)
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_UnsignedShort(m_wX)
Declare_DecodeMsg_UnsignedShort(m_wY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwObjId);
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_UnsignedShort(m_wX)
Declare_EncodeMsg_UnsignedShort(m_wY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwObjId) // ʹ����ID
Declare_Msg_Member(DWORD, m_dwSkillId) // ʹ����ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// ʹ�ü�����Ӧ
Declare_Msg_Begin(S2C_PVP_USE_SKILL_RESP, MSG_S2C_PVP_USE_SKILL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwObjId)
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_UnsignedShort(m_wX)
Declare_DecodeMsg_UnsignedShort(m_wY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwObjId);
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_UnsignedShort(m_wX)
Declare_EncodeMsg_UnsignedShort(m_wY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwObjId) // ʹ����ID
Declare_Msg_Member(DWORD, m_dwSkillId) // ʹ����ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// ʹ�ü���֪ͨ
Declare_Msg_Begin(S2C_PVP_USE_SKILL_NOTIFY, MSG_S2C_PVP_USE_SKILL_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwObjId)
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_UnsignedShort(m_wX)
Declare_DecodeMsg_UnsignedShort(m_wY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwObjId);
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_UnsignedShort(m_wX)
Declare_EncodeMsg_UnsignedShort(m_wY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwObjId) // ʹ����ID
Declare_Msg_Member(DWORD, m_dwSkillId) // ʹ����ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// HP�ı�����
Declare_Msg_Begin(C2S_PVP_HP_CHANGE_REQ, MSG_C2S_PVP_HP_CHANGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_Int(m_nChangeHP)
Declare_DecodeMsg_Int(m_nDamageType)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
//Declare_DecodeMsg_UnsignedInt(m_dwCostTime)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// ǩ��
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_Int(m_nChangeHP)
Declare_EncodeMsg_Int(m_nDamageType)
//Declare_EncodeMsg_Int(m_dwCostTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(int, m_nChangeHP)	// �ı�HP
Declare_Msg_Member(int, m_nDamageType) // �˺�����
Declare_Msg_Member(DWORD, m_dwPower) // ս��
//Declare_Msg_Member(DWORD, m_dwCostTime) // ����ʱ��
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // ǩ��
Declare_Msg_End


// HP�ı���Ӧ
Declare_Msg_Begin(S2C_PVP_HP_CHANGE_RESP, MSG_S2C_PVP_HP_CHANGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_Int(m_nChangeHP)
Declare_DecodeMsg_Int(m_nDamageType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_Int(m_nChangeHP)
Declare_EncodeMsg_Int(m_nDamageType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(int, m_nChangeHP)	// �ı�HP
Declare_Msg_Member(int, m_nDamageType) // �˺�����
Declare_Msg_End


// HP�ı�֪ͨ
Declare_Msg_Begin(S2C_PVP_HP_CHANGE_NOTIFY, MSG_S2C_PVP_HP_CHANGE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_Int(m_nChangeHP)
Declare_DecodeMsg_Int(m_nDamageType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_Int(m_nChangeHP)
Declare_EncodeMsg_Int(m_nDamageType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(int, m_nChangeHP)	// �ı�HP
Declare_Msg_Member(int, m_nDamageType) // �˺�����
Declare_Msg_End


// ����buff����
Declare_Msg_Begin(C2S_PVP_CREATE_BUFF_REQ, MSG_C2S_PVP_CREATE_BUFF_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwBuffId)
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBuffId)
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBuffId) // buffid
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // �Ƿ����Ծ���ף��Buff
Declare_Msg_End


// ����buff��Ӧ
Declare_Msg_Begin(S2C_PVP_CREATE_BUFF_RESP, MSG_S2C_PVP_CREATE_BUFF_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwBuffId)
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBuffId)
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBuffId) // buffid
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // �Ƿ����Ծ���ף��Buff
Declare_Msg_End


// ����buff֪ͨ
Declare_Msg_Begin(S2C_PVP_CREATE_BUFF_NOTIFY, MSG_S2C_PVP_CREATE_BUFF_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwBuffId)
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBuffId)
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_UnsignedChar(m_byIsSpriteBlessBuff)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBuffId) // buffid
Declare_Msg_Member(DWORD, m_dwSrcObjId) // ����ԴID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // Ŀ����ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // �Ƿ����Ծ���ף��Buff
Declare_Msg_End


// ս���������
Declare_Msg_Begin(C2S_PVP_BATTLE_RESULT_REQ, MSG_C2S_PVP_BATTLE_RESULT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byResult)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byResult);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byResult) // ս�����(ʱ�䵽��ƽ���ɷ��������ж�)
Declare_Msg_End


// ս��������
Declare_Msg_Begin(S2C_PVP_BATTLE_RESULT_RESP, MSG_S2C_PVP_BATTLE_RESULT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byResult)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byResult);
Declare_EncodeMsg_UnsignedShort(m_wRedOldRank);
Declare_EncodeMsg_UnsignedShort(m_wRedNewRank);
Declare_EncodeMsg_UnsignedShort(m_wRedLastWeekRank);
Declare_EncodeMsg_UnsignedShort(m_wRedScore);
Declare_EncodeMsg_UnsignedShort(m_wBlueOldRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueNewRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueLastWeekRank);
Declare_EncodeMsg_UnsignedShort(m_wBlueScore);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byResult) // ���(1=��ʤ,2=ƽ��,3=ʧ��)
Declare_Msg_Member(WORD, m_wRedOldRank) // �췽������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedNewRank) // �췽������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedLastWeekRank) // �췽��������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wRedScore) // �췽���λ�û���
Declare_Msg_Member(WORD, m_wBlueOldRank) // ����������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueNewRank) // ����������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueLastWeekRank) // ������������(0 = δ�ϰ�)
Declare_Msg_Member(WORD, m_wBlueScore) // �������λ�û���
Declare_Msg_End


// ��ȡս������
Declare_Msg_Begin(C2S_PVP_TAKE_BATTLE_REWARD_REQ, MSG_C2S_PVP_TAKE_BATTLE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wTakeRewardNum)
m_wTakeRewardNum = min(m_wTakeRewardNum, MAX_PVP_BATTLE_REWARD_NUM);
Declare_DecodeMsg_Memory(m_arTakeRewardList, sizeof(BYTE)*m_wTakeRewardNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType);
Declare_EncodeMsg_UnsignedShort(m_wTakeRewardNum);
Declare_EncodeMsg_Memory(m_arTakeRewardList, sizeof(BYTE)*m_wTakeRewardNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType) // ����(1=ÿ�յ�,ÿ�ܵ�)
Declare_Msg_Member(WORD, m_wTakeRewardNum) // ��ȡ�Ľ�������
Declare_Msg_Member_Array(BYTE, m_arTakeRewardList, MAX_PVP_BATTLE_REWARD_NUM)	// ��ȡ�Ľ����б�
Declare_Msg_End


// ��ȡս��������Ӧ
Declare_Msg_Begin(S2C_PVP_TAKE_BATTLE_REWARD_RESP, MSG_S2C_PVP_TAKE_BATTLE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedShort(m_wTakeRewardNum)
m_wTakeRewardNum = min(m_wTakeRewardNum, MAX_PVP_BATTLE_REWARD_NUM);
Declare_DecodeMsg_Memory(m_arTakeRewardList, sizeof(BYTE)*m_wTakeRewardNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType);
Declare_EncodeMsg_UnsignedShort(m_wTakeRewardNum);
Declare_EncodeMsg_Memory(m_arTakeRewardList, sizeof(BYTE)*m_wTakeRewardNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType) // ����(1=ÿ�յ�,ÿ�ܵ�)
Declare_Msg_Member(WORD, m_wTakeRewardNum) // ��ȡ�Ľ�������
Declare_Msg_Member_Array(BYTE, m_arTakeRewardList, MAX_PVP_BATTLE_REWARD_NUM)	// ��ȡ�Ľ����б�
Declare_Msg_End


// ���PVP��������
Declare_Msg_Begin(C2S_PLAYER_PVP_DATA_REQ, MSG_C2S_PLAYER_PVP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���PVP������Ӧ
Declare_Msg_Begin(S2C_PLAYER_PVP_DATA_RESP, MSG_S2C_PLAYER_PVP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_PVPData.wTodayBattleTimes)
Declare_DecodeMsg_UnsignedShort(m_PVPData.wWeekWinTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_PVPData.wTodayBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_PVPData.wWeekWinTimes)
Declare_EncodeMsg_UnsignedShort(m_PVPData.wLastWeekRank)
Declare_EncodeMsg_UnsignedShort(m_wMyRank)
Declare_EncodeMsg_UnsignedInt(m_dwMyRankValue)
Declare_EncodeMsg_UnsignedShort(m_PVPData.wDailyRewardTakeRecordNum)
Declare_EncodeMsg_Memory(m_PVPData.arDailyRewardTakeRecord, m_PVPData.wDailyRewardTakeRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_PVPData.wWeeklyRewardTakeRecordNum)
Declare_EncodeMsg_Memory(m_PVPData.arWeeklyRewardTakeRecord, m_PVPData.wWeeklyRewardTakeRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_wRankNum)
for (WORD i = 0; i < m_wRankNum; ++i)
{
	ClientRoleRankData &tRankData = m_RankList[i];
	Declare_EncodeMsg_String(tRankData.szRoleName, sizeof(tRankData.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tRankData.dwValue);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerPVPData, m_PVPData) // PVP����
Declare_Msg_Member(WORD, m_wMyRank) //  �ҵ�����
Declare_Msg_Member(DWORD, m_dwMyRankValue) //  �ҵ�����ֵ
Declare_Msg_Member(WORD, m_wRankNum) //  ǰ��������
Declare_Msg_Member_Array(ClientRoleRankData, m_RankList, MAX_DEFAULT_RANK_MUM)	// ǰ���������б�
Declare_Msg_End


// ѡ���鼼������
Declare_Msg_Begin(C2S_PVP_SELECT_SPRIT_SKILL_REQ, MSG_C2S_PVP_SELECT_SPRIT_SKILL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSkillId) // ����Id
Declare_Msg_End


// ѡ���鼼����Ӧ
Declare_Msg_Begin(S2C_PVP_SELECT_SPRIT_SKILL_RESP, MSG_S2C_PVP_SELECT_SPRIT_SKILL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSkillId) // ����Id
Declare_Msg_End

// ȡ������Ŀ������
Declare_Msg_Begin(C2S_PVP_CANCEL_SEARCH_TARGET_REQ, MSG_C2S_PVP_CANCEL_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ȡ������Ŀ����Ӧ
Declare_Msg_Begin(S2C_PVP_CANCEL_SEARCH_TARGET_RESP, MSG_S2C_PVP_CANCEL_SEARCH_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsInBattle)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsInBattle);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsInBattle) // �Ƿ���ս����
Declare_Msg_End


// �׳���������
Declare_Msg_Begin(C2S_LIMIT_FIRST_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_FIRST_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �׳�������Ӧ
Declare_Msg_Begin(S2C_LIMIT_FIRST_RECHARGE_DATA_RESP, MSG_S2C_LIMIT_FIRST_RECHARGE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pActivityData->wFirstRechargeRewardRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_pActivityData->dwTotleRecharge);
Declare_EncodeMsg_UnsignedShort(m_pActivityData->wFirstRechargeRewardRecordNum);
for (WORD i = 0; i < m_pActivityData->wFirstRechargeRewardRecordNum; ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arFirstRechargeRewardRecord[i]);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(LimitActivityData*, m_pActivityData) // �����
Declare_Msg_End


// ������������
Declare_Msg_Begin(C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����������Ӧ
// Declare_Msg_Begin(S2C_LIMIT_SINGLE_RECHARGE_DATA_RESP, MSG_S2C_LIMIT_SINGLE_RECHARGE_DATA_RESP)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedShort(m_pActivityData->wSingleRechargeRewardRecordNum)
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedShort(m_pActivityData->wSingleRechargeRewardRecordNum);
// for (WORD i = 0; i < m_pActivityData->wSingleRechargeRewardRecordNum; ++i)
// {
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arSingleRechargeRewardRecord[i].wRecordId);
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arSingleRechargeRewardRecord[i].wRecordNum);
// }
// Declare_EncodeMsg_UnsignedShort(m_pActivityData->wRechargeNum);
// for (WORD i = 0; i < m_pActivityData->wRechargeNum; ++i)
// {
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arRechargeList[i].wRechargeId);
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arRechargeList[i].wRechargeTimes);
// }
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // �����
// Declare_Msg_End


// �۳���������
Declare_Msg_Begin(C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �۳�������Ӧ
// Declare_Msg_Begin(S2C_LIMIT_ACCUMULATE_RECHARGE_DATA_RESP, MSG_S2C_LIMIT_ACCUMULATE_RECHARGE_DATA_RESP)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedShort(m_pActivityData->wAccumulateRechargeRewardRecordNum)
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedInt(m_pActivityData->dwAccumulateRecharge);
// Declare_EncodeMsg_UnsignedShort(m_pActivityData->wAccumulateRechargeRewardRecordNum);
// for (WORD i = 0; i < m_pActivityData->wAccumulateRechargeRewardRecordNum; ++i)
// {
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arAccumulateRechargeRewardRecord[i]);
// }
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // �����
// Declare_Msg_End


// ��ʱ��ļ��������
Declare_Msg_Begin(C2S_LIMIT_RECRUIT_DATA_REQ, MSG_C2S_LIMIT_RECRUIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ʱ��ļ������Ӧ
//Declare_Msg_Begin(S2C_LIMIT_RECRUIT_DATA_RESP, MSG_S2C_LIMIT_RECRUIT_DATA_RESP)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedShort(m_pActivityData->wRecruitTimes)
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedShort(m_pActivityData->wRecruitTimes);
// Declare_EncodeMsg_UnsignedShort(m_pActivityData->wRecruitRewardRecordNum);
// for (WORD i = 0; i < m_pActivityData->wRecruitRewardRecordNum; ++i)
// {
// 	Declare_EncodeMsg_UnsignedShort(m_pActivityData->arRecruitRewardRecord[i]);
// }
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // �����
// Declare_Msg_End


// ��ȡ��ʱ���������
Declare_Msg_Begin(C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ, MSG_C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDataId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDataId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDataId) // �߼�����ID
Declare_Msg_End


// ��ȡ��ʱ���������
Declare_Msg_Begin(S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP, MSG_S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDataId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDataId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDataId) // �߼�����ID
Declare_Msg_End


// ������������
Declare_Msg_Begin(C2S_ATTACK_CITY_DATA_REQ, MSG_C2S_ATTACK_CITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����������Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_DATA_RESP, MSG_S2C_ATTACK_CITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_AttackCityData.dwBattleOverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_AttackCityData.dwBattleOverTime);
Declare_EncodeMsg_UnsignedShort(m_AttackCityData.wReliveCD);
Declare_EncodeMsg_UnsignedShort(m_AttackCityData.wWeakCD);
Declare_EncodeMsg_UnsignedChar(m_AttackCityData.byEncourageTimes);
Declare_EncodeMsg_UnsignedShort(m_AttackCityData.vTakeBossRewardRecordList.size());
for (size_t i = 0; i < m_AttackCityData.vTakeBossRewardRecordList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_AttackCityData.vTakeBossRewardRecordList[i]);
}
Declare_EncodeMsg_UnsignedInt(m_dwNo1RoleHeadImgId);
Declare_EncodeMsg_String(m_szNo1RoleName, sizeof(m_szNo1RoleName));
Declare_EncodeMsg_UnsignedShort(m_wFromChapter);
Declare_EncodeMsg_UnsignedInt(m_dwKillBossTime);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerAttackCityData, m_AttackCityData) // ��������
Declare_Msg_Member(DWORD, m_dwNo1RoleHeadImgId) // ������һ�Ľ�ɫͷ��ID
Declare_Msg_Member_Array(char, m_szNo1RoleName, MAX_ROLE_LEN)	// ǰ���������б�
Declare_Msg_Member(WORD, m_wFromChapter) // ���λ��ʼ���½�
Declare_Msg_Member(DWORD, m_dwKillBossTime) // ��ɱBOSSʱ��(�����,0��ʾľ��)
Declare_Msg_End


// ���ǽ������� 
Declare_Msg_Begin(C2S_ATTACK_CITY_ENTER_REQ, MSG_C2S_ATTACK_CITY_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���ǽ�����Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_ENTER_RESP, MSG_S2C_ATTACK_CITY_ENTER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPlayerNum)
for (WORD i = 0; i < m_wPlayerNum; ++i)
{
	PlayerCommonData &tPlayerData = m_PlayerList[i];
	MapPos &tMapPos = m_PlayerMapPos[i];
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwRoleId);
	Declare_DecodeMsg_UnsignedShort(tPlayerData.wHeadIcon);
	Declare_DecodeMsg_String(tPlayerData.szRoleName, sizeof(tPlayerData.szRoleName));
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwSpriteId);
	Declare_DecodeMsg_UnsignedShort(tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tMapPos.wMapY);
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwTitleId);
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwNicknameId);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPlayerNum);
for (WORD i = 0; i < m_wPlayerNum; ++i)
{
	PlayerCommonData &tPlayerData = m_PlayerList[i];
	MapPos &tMapPos= m_PlayerMapPos[i];
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tPlayerData.wHeadIcon);
	Declare_EncodeMsg_String(tPlayerData.szRoleName, sizeof(tPlayerData.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwSpriteId);
	Declare_EncodeMsg_UnsignedShort(tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tMapPos.wMapY);
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwTitleId);
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwNicknameId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPlayerNum) // �߼�����ID
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_ATTACK_CITY_PLAYER_NUM)	// ����б�
Declare_Msg_Member_Array(MapPos, m_PlayerMapPos, MAX_ATTACK_CITY_PLAYER_NUM)	// ��ҵ�ͼ����
Declare_Msg_End

// ���ǽ���֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_ENTER_NOTICE, MSG_S2C_ATTACK_CITY_ENTER_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwRoleId);
Declare_DecodeMsg_UnsignedShort(m_tPlayerData.wHeadIcon);
Declare_DecodeMsg_String(m_tPlayerData.szRoleName, sizeof(m_tPlayerData.szRoleName));
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwSpriteId);
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapX);
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapY);
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwTitleId);
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwNicknameId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwRoleId);
Declare_EncodeMsg_UnsignedShort(m_tPlayerData.wHeadIcon);
Declare_EncodeMsg_String(m_tPlayerData.szRoleName, sizeof(m_tPlayerData.szRoleName));
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwSpriteId);
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapX);
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapY);
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwTitleId);
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwNicknameId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerCommonData, m_tPlayerData) // �������
Declare_Msg_Member(MapPos, m_tMapPos) // ��ҵ�ͼ����
Declare_Msg_End


// �����뿪����
Declare_Msg_Begin(C2S_ATTACK_CITY_LEAVE_REQ, MSG_C2S_ATTACK_CITY_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �����뿪��Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_LEAVE_RESP, MSG_S2C_ATTACK_CITY_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����뿪֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_LEAVE_NOTICE, MSG_S2C_ATTACK_CITY_LEAVE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// ���ID
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_ATTACK_CITY_GOTO_REQ, MSG_C2S_ATTACK_CITY_GOTO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_GOTO_RESP, MSG_S2C_ATTACK_CITY_GOTO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_GOTO_NOTICE, MSG_S2C_ATTACK_CITY_GOTO_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// ���ID
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End

// �����б�֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_MONSTER_LIST_NOTICE, MSG_S2C_ATTACK_CITY_MONSTER_LIST_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurChapter)
Declare_DecodeMsg_UnsignedShort(m_wMonsterNum)
for (WORD i = 0; i < m_wMonsterNum; ++i)
{
	AttackCityMonster &tMonster = m_arMonsterList[i];
	Declare_DecodeMsg_UnsignedInt(tMonster.dwUID);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwMapId);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwDamage);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwTotalHP);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwStartBattleTime);
	Declare_DecodeMsg_UnsignedShort(tMonster.tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tMonster.tMapPos.wMapY);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurChapter)
Declare_EncodeMsg_UnsignedShort(m_wMonsterNum)
for (WORD i = 0; i < m_wMonsterNum; ++i)
{
	AttackCityMonster &tMonster = m_arMonsterList[i];
	Declare_EncodeMsg_UnsignedInt(tMonster.dwUID);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwMapId);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwDamage);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwTotalHP);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwStartBattleTime);
	Declare_EncodeMsg_UnsignedShort(tMonster.tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tMonster.tMapPos.wMapY);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurChapter)	// ��ǰ�½�(����)
Declare_Msg_Member(WORD, m_wMonsterNum)	// ��������
Declare_Msg_Member_Array(AttackCityMonster, m_arMonsterList, MAX_ATTACK_CITY_MONSTER_NUM)	// �����б�
Declare_Msg_End



// ��������֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_MONSTER_DATA_NOTICE, MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwUID);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwMapId);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwDamage);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwTotalHP);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwStartBattleTime);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapX);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapY);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwUID);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwMapId);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwDamage);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwTotalHP);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwStartBattleTime);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapX);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapY);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(AttackCityMonster, m_tMonster)	// ��������
Declare_Msg_End


// �����б�֪ͨ
Declare_Msg_Begin(S2C_TREASURE_BOX_LIST_NOTICE, MSG_S2C_TREASURE_BOX_LIST_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureBoxNum)
for (WORD i = 0; i < m_wTreasureBoxNum; ++i)
{
	TreasureBox &tBox = m_TreasureBoxList[i];
	Declare_DecodeMsg_UnsignedInt(tBox.dwUID);
	Declare_DecodeMsg_UnsignedInt(tBox.dwInfoId);
	Declare_DecodeMsg_UnsignedShort(tBox.tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tBox.tMapPos.wMapY);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTreasureBoxNum)
for (WORD i = 0; i < m_wTreasureBoxNum; ++i)
{
	TreasureBox &tBox = m_TreasureBoxList[i];
	Declare_EncodeMsg_UnsignedInt(tBox.dwUID);
	Declare_EncodeMsg_UnsignedInt(tBox.dwInfoId);
	Declare_EncodeMsg_UnsignedShort(tBox.tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tBox.tMapPos.wMapY);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureBoxNum)	// ��������
Declare_Msg_Member_Array(TreasureBox, m_TreasureBoxList, MAX_ATTACK_CITY_TREASURE_BOX_NUM)	// �����б�
Declare_Msg_End


// ������ʧ֪ͨ
Declare_Msg_Begin(S2C_TREASURE_BOX_DISAPPEAR_NOTICE, MSG_S2C_TREASURE_BOX_DISAPPEAR_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ������ס����
Declare_Msg_Begin(C2S_TREASURE_BOX_LOCK_REQ, MSG_C2S_TREASURE_BOX_LOCK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ������ס��Ӧ
Declare_Msg_Begin(S2C_TREASURE_BOX_LOCK_RESP, MSG_S2C_TREASURE_BOX_LOCK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid(0��ʾ�ѱ���Č�ë��ס)
Declare_Msg_End


// ����ʰȡ����
Declare_Msg_Begin(C2S_TREASURE_BOX_PICK_UP_REQ, MSG_C2S_TREASURE_BOX_PICK_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ����ʰȡ��Ӧ
Declare_Msg_Begin(S2C_TREASURE_BOX_PICK_UP_RESP, MSG_S2C_TREASURE_BOX_PICK_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_UnsignedInt(m_dwInfoId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_UnsignedInt(m_dwInfoId);
Declare_EncodeMsg_UnsignedShort(m_vItemList.size());
for (size_t i = 0; i < m_vItemList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// ��ϢID
Declare_Msg_Member(vector<Item>, m_vItemList)	// ��Ʒ�б�

Declare_Msg_End


// ��������
Declare_Msg_Begin(C2S_ATTACK_CITY_RELIVE_REQ, MSG_C2S_ATTACK_CITY_RELIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsFreeRelive)	// �Ƿ���Ѹ���
Declare_Msg_End


// ������Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_RELIVE_RESP, MSG_S2C_ATTACK_CITY_RELIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsFreeRelive)	// �Ƿ���Ѹ���
Declare_Msg_End


// ������ȡ����
Declare_Msg_Begin(C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ, MSG_C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// �½�
Declare_Msg_End


// ������ȡ��Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP, MSG_S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// �½�
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_ATTACK_CITY_ENCOURAGE_REQ, MSG_C2S_ATTACK_CITY_ENCOURAGE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������Ӧ
Declare_Msg_Begin(S2C_ATTACK_CITY_ENCOURAGE_RESP, MSG_S2C_ATTACK_CITY_ENCOURAGE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �������б�֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE, MSG_S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wListNum)
for (WORD i = 0; i < m_wListNum && i < MAX_DEFAULT_RANK_MUM; ++i)
{
	ClientRoleRankData &sData = m_arRankList[i];
	Declare_DecodeMsg_UnsignedShort(sData.wHeadIcon)
	Declare_DecodeMsg_String(sData.szRoleName, sizeof(sData.szRoleName) - 1)
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wListNum)
for (WORD i = 0; i < m_wListNum && i < MAX_DEFAULT_RANK_MUM; ++i)
{
	ClientRoleRankData &sData = m_arRankList[i];
	Declare_EncodeMsg_UnsignedShort(sData.wHeadIcon)
	Declare_EncodeMsg_String(sData.szRoleName, sizeof(sData.szRoleName))
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wListNum) // �б�����
Declare_Msg_Member_Array(ClientRoleRankData, m_arRankList, MAX_DEFAULT_RANK_MUM);	// �б�����
Declare_Msg_End


// ������������֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_RANK_UPDATE_NOTICE, MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRank);
Declare_DecodeMsg_UnsignedInt(m_dwScore);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRank);
Declare_EncodeMsg_UnsignedInt(m_dwScore);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRank)	// ����
Declare_Msg_Member(DWORD, m_dwScore)	// ����
Declare_Msg_End


// ���ǻ��ʼ֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_ACTIVITY_START_NOTICE, MSG_S2C_ATTACK_CITY_ACTIVITY_START_NOTICE)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���ǻ����֪ͨ
Declare_Msg_Begin(S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE, MSG_S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �޾�֮����������
Declare_Msg_Begin(C2S_ENDLESS_LAND_DATA_REQ, MSG_C2S_ENDLESS_LAND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �޾�֮��������Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_DATA_RESP, MSG_S2C_ENDLESS_LAND_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tEndlessLandData.wCurWave);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wTodayBattleTimes);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wCurWave);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wTodayWaveRecord);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wCurWaveKillNum);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wHistoryWaveRecord);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wCurContinueKillNum);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wContinueKillNumRecord);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wTodayCollisionRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wLastTakeContinueKillReward);
Declare_EncodeMsg_UnsignedChar(m_tEndlessLandData.wIsFirstBattle);
// Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.wContinueKillRewardRecordNum);
// for (WORD i = 0; i < m_tEndlessLandData.wContinueKillRewardRecordNum; ++i)
// {
// 	Declare_EncodeMsg_UnsignedShort(m_tEndlessLandData.arContinueKillRewardRecord[i]);
// }
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerEndlessLandData, m_tEndlessLandData)	// ����޾�֮������
Declare_Msg_End


// �޾�֮�ؽ�������
Declare_Msg_Begin(C2S_ENDLESS_LAND_ENTER_REQ, MSG_C2S_ENDLESS_LAND_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �޾�֮�ؽ�����Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_ENTER_RESP, MSG_S2C_ENDLESS_LAND_ENTER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ʼս������
Declare_Msg_Begin(C2S_ENDLESS_LAND_BATTLE_START_REQ, MSG_C2S_ENDLESS_LAND_BATTLE_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ʼս����Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_BATTLE_START_RESP, MSG_S2C_ENDLESS_LAND_BATTLE_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ս������
Declare_Msg_Begin(C2S_ENDLESS_LAND_BATTLE_OVER_REQ, MSG_C2S_ENDLESS_LAND_BATTLE_OVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsWin);
Declare_DecodeMsg_UnsignedShort(m_wWave);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsWin);
Declare_EncodeMsg_UnsignedShort(m_wWave);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsWin)	// �Ƿ��ʤ
Declare_Msg_Member(WORD, m_wWave)	// ��ս�Ĳ���
Declare_Msg_End

// ����ս����Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_BATTLE_OVER_RESP, MSG_S2C_ENDLESS_LAND_BATTLE_OVER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsWin);
Declare_DecodeMsg_UnsignedShort(m_wRewardWave);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsWin);
Declare_EncodeMsg_UnsignedShort(m_wRewardWave);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsWin)	// �Ƿ��ʤ
Declare_Msg_Member(WORD, m_wRewardWave)	// ������ս��������(0=��ʾľ�н���)
Declare_Msg_End


// ɱ������
Declare_Msg_Begin(C2S_ENDLESS_LAND_KILL_MONSTER_REQ, MSG_C2S_ENDLESS_LAND_KILL_MONSTER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIndex);
Declare_DecodeMsg_UnsignedInt(m_dwMonsterId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIndex);
Declare_EncodeMsg_UnsignedInt(m_dwMonsterId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIndex)	// ���������
Declare_Msg_Member(DWORD, m_dwMonsterId)	// ����Id
Declare_Msg_End


// ɱ����Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_KILL_MONSTER_RESP, MSG_S2C_ENDLESS_LAND_KILL_MONSTER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIndex);
Declare_DecodeMsg_UnsignedInt(m_dwMonsterId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIndex);
Declare_EncodeMsg_UnsignedInt(m_dwMonsterId);
Declare_EncodeMsg_UnsignedShort(m_wDropItemNum);
for (WORD i = 0; i < m_wDropItemNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_DropItemList[i].dwObjId);
	Declare_EncodeMsg_UnsignedShort(m_DropItemList[i].wObjNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIndex)	// ���������
Declare_Msg_Member(DWORD, m_dwMonsterId)	// ����Id
Declare_Msg_Member(WORD, m_wDropItemNum)	// ������Ʒ����
Declare_Msg_Member_Array(MonsterDropData, m_DropItemList, MAX_MAP_DROP_NUM);	// ������Ʒ�б�
Declare_Msg_End

// ��ײ����
Declare_Msg_Begin(C2S_ENDLESS_LAND_COLLISION_REQ, MSG_C2S_ENDLESS_LAND_COLLISION_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ײ��Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_COLLISION_RESP, MSG_S2C_ENDLESS_LAND_COLLISION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTodayCollisionRewardTimes);
Declare_DecodeMsg_UnsignedShort(m_wRewardId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTodayCollisionRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTodayCollisionRewardTimes)	// ������ײ��������
Declare_Msg_Member(WORD, m_wRewardId)	// ����Id
Declare_Msg_End

// ��ȡ��ɱ��������
Declare_Msg_Begin(C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ, MSG_C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wKeepKillNum)	// ��ɱ����
Declare_Msg_End

// ��ȡ��ɱ������Ӧ
Declare_Msg_Begin(S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP, MSG_S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wKeepKillNum)	// ��ɱ����
Declare_Msg_End


// ��λ��������
Declare_Msg_Begin(C2S_NOBILITY_ACTIVATE_REQ, MSG_C2S_NOBILITY_ACTIVATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��λ������Ӧ
Declare_Msg_Begin(S2C_NOBILITY_ACTIVATE_RESP, MSG_S2C_NOBILITY_ACTIVATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNobilityLevel);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNobilityLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNobilityLevel)	// ��λ�ȼ�
Declare_Msg_End


// ��ֵ�ۿ���������
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_DATA_REQ, MSG_C2S_SUPER_DISCOUNT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ֵ�ۿ�������Ӧ
Declare_Msg_Begin(S2C_SUPER_DISCOUNT_DATA_RESP, MSG_S2C_SUPER_DISCOUNT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wStatus);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wStatus);
//Declare_EncodeMsg_UnsignedShort(m_Data.wTakeAccumulateRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_Data.wCurBuyTimes);
Declare_EncodeMsg_UnsignedShort(m_Data.wTookAccumulateRewardNum);
Declare_EncodeMsg_Memory(m_Data.arTookAccumulateRewardList, sizeof(WORD)* m_Data.wTookAccumulateRewardNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SuperDiscountData, m_Data)	// ��λ�ȼ�
Declare_Msg_End

// ��ȡ��ֵ�ۿ۹���������
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_BUY_REWARD_REQ, MSG_C2S_SUPER_DISCOUNT_BUY_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ֵ�ۿ۹�������Ӧ
Declare_Msg_Begin(S2C_SUPER_DISCOUNT_BUY_REWARD_RESP, MSG_S2C_SUPER_DISCOUNT_BUY_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ֵ�ۿ��ۻ���������
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ, MSG_C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// ����
Declare_Msg_End

// ��ȡ��ֵ�ۿ��ۻ�������Ӧ
Declare_Msg_Begin(S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, MSG_S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// ����
Declare_Msg_End



// ��ֵ�ۿ���������(��)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_DATA_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ֵ�ۿ�������Ӧ(��)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_DATA_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wStatus);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wStatus);
Declare_EncodeMsg_UnsignedShort(m_Data.wTakeAccumulateRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_Data.wCurBuyTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SuperDiscountData, m_Data)	// ��λ�ȼ�
Declare_Msg_End

// ��ȡ��ֵ�ۿ۹���������(��)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ֵ�ۿ۹�������Ӧ(��)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ֵ�ۿ��ۻ���������(��)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ��ֵ�ۿ��ۻ�������Ӧ(��)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���ջ��������
Declare_Msg_Begin(C2S_FESTIVAL_ACTIVITY_DATA_REQ, MSG_C2S_FESTIVAL_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_End

// ���ջ������Ӧ
Declare_Msg_Begin(S2C_FESTIVAL_ACTIVITY_DATA_RESP, MSG_S2C_FESTIVAL_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wTargetNum)
for (WORD i = 0; i < m_tActivityData.wTargetNum; ++i)
{
	ActivityTarget &tTarget = m_tActivityData.arTargetList[i];
	Declare_EncodeMsg_UnsignedShort(tTarget.wId);
	Declare_EncodeMsg_UnsignedInt(tTarget.dwContext);
	Declare_EncodeMsg_UnsignedShort(tTarget.wTakeRewardTimes)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_Member(FestivalActivity, m_tActivityData)	// �����
Declare_Msg_End

// ���ջ������ȡ����
Declare_Msg_Begin(C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ, MSG_C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_Member(WORD, m_wTargetId)	// ��ȡ��Ŀ��ID
Declare_Msg_End

// ���ջ������ȡ��Ӧ
Declare_Msg_Begin(S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP, MSG_S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_Member(WORD, m_wTargetId)	// ��ȡ��Ŀ��ID
Declare_Msg_End


// ������Ʒ�һ�����
Declare_Msg_Begin(C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ, MSG_C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_UnsignedShort(m_wExchangeNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_UnsignedShort(m_wExchangeNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_Member(WORD, m_wTargetId)	// ��ȡ��Ŀ��ID
Declare_Msg_Member(WORD, m_wExchangeNum)	// �һ�����
Declare_Msg_End


// ������Ʒ�һ�����
Declare_Msg_Begin(S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP, MSG_S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_UnsignedShort(m_wExchangeNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_UnsignedShort(m_wExchangeNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// �����
Declare_Msg_Member(WORD, m_wTargetId)	// ��ȡ��Ŀ��ID
Declare_Msg_Member(WORD, m_wExchangeNum)	// �һ�����
Declare_Msg_End

// ���ջ�������
Declare_Msg_Begin(C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ, MSG_C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

Declare_Msg_Begin(S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP, MSG_S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRedPointNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRedPointNum)
Declare_EncodeMsg_Memory(m_RedPointList, m_wRedPointNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRedPointNum)		// �������
Declare_Msg_Member_Array(WORD, m_RedPointList, MAX_RED_POINT_NUM);	// ����б�(�к��Ļ����)
Declare_Msg_End

// ��׼�Ƽ���������
Declare_Msg_Begin(C2S_ACCURATE_RECOMMEND_DATA_REQ, MSG_C2S_ACCURATE_RECOMMEND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��׼�Ƽ�������Ӧ
Declare_Msg_Begin(S2C_ACCURATE_RECOMMEND_DATA_RESP, MSG_S2C_ACCURATE_RECOMMEND_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wGoodsNum)
for (WORD i = 0; i < m_Data.wGoodsNum; ++i)
{
	AccurateRecommendGoods &tGoods = m_Data.arGoodsList[i];
	Declare_EncodeMsg_UnsignedShort(tGoods.wGoodsId);
	Declare_EncodeMsg_UnsignedShort(tGoods.wBuyTimes);
	Declare_EncodeMsg_UnsignedShort(tGoods.wCanTakeTimes);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerAccurateRecommendData, m_Data)	// ��ƷID
Declare_Msg_End


// ��ȡ��׼�Ƽ��������
Declare_Msg_Begin(C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ, MSG_C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// ��ƷID
Declare_Msg_End

// ��ȡ��׼�Ƽ������Ӧ
Declare_Msg_Begin(S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP, MSG_S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// ��ƷID
Declare_Msg_End

// ��ʯ�б�����
Declare_Msg_Begin(C2S_GEM_LIST_REQ, MSG_C2S_GEM_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ʯ�б���Ӧ
Declare_Msg_Begin(S2C_GEM_LIST_RESP, MSG_S2C_GEM_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_PlayerGemInfo.wGemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_PlayerGemInfo.wGemNum)
Declare_EncodeMsg_Memory(m_PlayerGemInfo.arGemList, m_PlayerGemInfo.wGemNum*sizeof(Gem));
Declare_EncodeMsg_UnsignedShort(WORD(MAX_BATTLE_HERO_NUM))
Declare_EncodeMsg_Memory(m_PlayerGemInfo.arHoleOpenList, sizeof(m_PlayerGemInfo.arHoleOpenList));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerGemInfo, m_PlayerGemInfo)		// ��ұ�ʯ��Ϣ
Declare_Msg_End

// ��ʯ�ϳ�����
Declare_Msg_Begin(C2S_GEM_COMPOSE_REQ, MSG_C2S_GEM_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGemNum)
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGemNum)
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGemNum)	// ��ʯ����
Declare_Msg_Member(DWORD, m_dwGemId)	// ��ʯID
Declare_Msg_End

// ��ʯ�ϳ���Ӧ
Declare_Msg_Begin(S2C_GEM_COMPOSE_RESP, MSG_S2C_GEM_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGemNum)
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGemNum)
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGemNum)	// ��ʯ����
Declare_Msg_Member(DWORD, m_dwGemId)	// ��ʯID
Declare_Msg_End

// ��ʯ��Ƕ����
Declare_Msg_Begin(C2S_GEM_INLAY_REQ, MSG_C2S_GEM_INLAY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemInlayNum)
m_wGemInlayNum = min(m_wGemInlayNum, MAX_GEM_HOLE_NUM);
for (WORD i = 0; i < m_wGemInlayNum; ++i)
{
	Declare_DecodeMsg_UnsignedInt(m_arGemInlayList[i].dwGemId);
	Declare_DecodeMsg_UnsignedShort(m_arGemInlayList[i].wHole);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemInlayNum)
for (WORD i = 0; i < m_wGemInlayNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_arGemInlayList[i].dwGemId);
	Declare_EncodeMsg_UnsignedShort(m_arGemInlayList[i].wHole);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ���Ͳ�λ
Declare_Msg_Member(WORD, m_wGemInlayNum)	// ��ʯ��Ƕ����
Declare_Msg_Member_Array(GemInlay, m_arGemInlayList, MAX_GEM_HOLE_NUM);	// ��ʯ��Ƕ�б�
Declare_Msg_End


// ��ʯ��Ƕ��Ӧ
Declare_Msg_Begin(S2C_GEM_INLAY_RESP, MSG_S2C_GEM_INLAY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemInlayNum)
m_wGemInlayNum = min(m_wGemInlayNum, MAX_GEM_HOLE_NUM);
for (WORD i = 0; i < m_wGemInlayNum; ++i)
{
	Declare_DecodeMsg_UnsignedInt(m_arGemInlayList[i].dwGemId);
	Declare_DecodeMsg_UnsignedShort(m_arGemInlayList[i].wHole);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemInlayNum)
for (WORD i = 0; i < m_wGemInlayNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_arGemInlayList[i].dwGemId);
	Declare_EncodeMsg_UnsignedShort(m_arGemInlayList[i].wHole);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ���Ͳ�λ
Declare_Msg_Member(WORD, m_wGemInlayNum)	// ��ʯ��Ƕ����
Declare_Msg_Member_Array(GemInlay, m_arGemInlayList, MAX_GEM_HOLE_NUM);	// ��ʯ��Ƕ�б�
Declare_Msg_End

// ��ʯж������
Declare_Msg_Begin(C2S_GEM_UNLOAD_REQ, MSG_C2S_GEM_UNLOAD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ��ʯ��λ
Declare_Msg_Member(WORD, m_wGemHole)	// ��ʯ��λ
Declare_Msg_End


// ��ʯж����Ӧ
Declare_Msg_Begin(S2C_GEM_UNLOAD_RESP, MSG_S2C_GEM_UNLOAD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ��ʯ��λ
Declare_Msg_Member(WORD, m_wGemHole)	// ��ʯ��λ
Declare_Msg_End


// ��ʯ��λ��������
Declare_Msg_Begin(C2S_GEM_HOLE_OPEN_REQ, MSG_C2S_GEM_HOLE_OPEN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ���Ͳ�λ
Declare_Msg_Member(WORD, m_wGemHole)	// ��ʯ��λ
Declare_Msg_End


// ��ʯ��λ������Ӧ
Declare_Msg_Begin(S2C_GEM_HOLE_OPEN_RESP, MSG_S2C_GEM_HOLE_OPEN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// ���Ͳ�λ
Declare_Msg_Member(WORD, m_wGemHole)	// ��ʯ��λ
Declare_Msg_End

// ��ʯ����֪ͨ
Declare_Msg_Begin(S2C_GEM_DATA_NOTIFY, MSG_S2C_GEM_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_UnsignedInt(m_dwGemNum)
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_UnsignedInt(m_dwGemNum)
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGemId)	// ��ʯID
Declare_Msg_Member(DWORD, m_dwGemNum)	// ��ʯ����
Declare_Msg_Member(BYTE, m_byFlag)		// ������־(0=����,1=��ӻ��߸���)
Declare_Msg_End


// ��ʯ�ֽ�����
Declare_Msg_Begin(C2S_GEM_DECOMPOSE_REQ, MSG_C2S_GEM_DECOMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_UnsignedInt(m_dwGemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_UnsignedInt(m_dwGemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGemId)	// ��ʯID
Declare_Msg_Member(DWORD, m_dwGemNum)	// ��ʯ����
Declare_Msg_End


// ��ʯ�ֽ���Ӧ
Declare_Msg_Begin(S2C_GEM_DECOMPOSE_RESP, MSG_S2C_GEM_DECOMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_UnsignedInt(m_dwGemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_UnsignedInt(m_dwGemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGemId)	// ��ʯID
Declare_Msg_Member(DWORD, m_dwGemNum)	// ��ʯ����
Declare_Msg_End


// ���볡������ 
Declare_Msg_Begin(C2S_ENTER_SCENE_REQ, MSG_C2S_ENTER_SCENE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwContext)	// ������
Declare_Msg_End


// ���볡����Ӧ
Declare_Msg_Begin(S2C_ENTER_SCENE_RESP, MSG_S2C_ENTER_SCENE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_wPlayerNum)
for (WORD i = 0; i < m_wPlayerNum; ++i)
{
	PlayerCommonData &tPlayerData = m_PlayerList[i];
	MapPos &tMapPos = m_PlayerMapPos[i];
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwRoleId);
	Declare_DecodeMsg_UnsignedShort(tPlayerData.wHeadIcon);
	Declare_DecodeMsg_String(tPlayerData.szRoleName, sizeof(tPlayerData.szRoleName));
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwSpriteId);
	Declare_DecodeMsg_UnsignedShort(tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tMapPos.wMapY);
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwTitleId);
	Declare_DecodeMsg_UnsignedInt(tPlayerData.dwNicknameId);
	Declare_DecodeMsg_UnsignedShort(tPlayerData.wServerId);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_wPlayerNum);
for (WORD i = 0; i < m_wPlayerNum; ++i)
{
	PlayerCommonData &tPlayerData = m_PlayerList[i];
	MapPos &tMapPos = m_PlayerMapPos[i];
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tPlayerData.wHeadIcon);
	Declare_EncodeMsg_String(tPlayerData.szRoleName, sizeof(tPlayerData.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwSpriteId);
	Declare_EncodeMsg_UnsignedShort(tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tMapPos.wMapY);
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwTitleId);
	Declare_EncodeMsg_UnsignedInt(tPlayerData.dwNicknameId);
	Declare_EncodeMsg_UnsignedShort(tPlayerData.wServerId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwContext)	// ������
Declare_Msg_Member(MapPos, m_tMapPos) // ��ҵ�ͼ����
Declare_Msg_Member(WORD, m_wPlayerNum) // �߼�����ID
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_SCENE_PLAYER_NUM)	// ����б�
Declare_Msg_Member_Array(MapPos, m_PlayerMapPos, MAX_SCENE_PLAYER_NUM)	// ��ҵ�ͼ����
Declare_Msg_End

// ���볡��֪ͨ
Declare_Msg_Begin(S2C_ENTER_SCENE_NOTICE, MSG_S2C_ENTER_SCENE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwRoleId);
Declare_DecodeMsg_UnsignedShort(m_tPlayerData.wHeadIcon);
Declare_DecodeMsg_String(m_tPlayerData.szRoleName, sizeof(m_tPlayerData.szRoleName));
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwSpriteId);
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapX);
Declare_DecodeMsg_UnsignedShort(m_tMapPos.wMapY);
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwTitleId);
Declare_DecodeMsg_UnsignedInt(m_tPlayerData.dwNicknameId);
Declare_DecodeMsg_UnsignedShort(m_tPlayerData.wServerId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwRoleId);
Declare_EncodeMsg_UnsignedShort(m_tPlayerData.wHeadIcon);
Declare_EncodeMsg_String(m_tPlayerData.szRoleName, sizeof(m_tPlayerData.szRoleName));
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwSpriteId);
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapX);
Declare_EncodeMsg_UnsignedShort(m_tMapPos.wMapY);
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwTitleId);
Declare_EncodeMsg_UnsignedInt(m_tPlayerData.dwNicknameId);
Declare_EncodeMsg_UnsignedShort(m_tPlayerData.wServerId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(PlayerCommonData, m_tPlayerData) // �������
Declare_Msg_Member(MapPos, m_tMapPos) // ��ҵ�ͼ����
Declare_Msg_End


// �뿪��������
Declare_Msg_Begin(C2S_LEAVE_SCENE_REQ, MSG_C2S_LEAVE_SCENE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_End


// �뿪������Ӧ
Declare_Msg_Begin(S2C_LEAVE_SCENE_RESP, MSG_S2C_LEAVE_SCENE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_End


// �뿪����֪ͨ
Declare_Msg_Begin(S2C_LEAVE_SCENE_NOTICE, MSG_S2C_LEAVE_SCENE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwRoleId)	// ���ID
Declare_Msg_End


// Ӣ����������
Declare_Msg_Begin(C2S_HERO_GOTO_REQ, MSG_C2S_HERO_GOTO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End


// Ӣ��������Ӧ
Declare_Msg_Begin(S2C_HERO_GOTO_RESP, MSG_S2C_HERO_GOTO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End


// Ӣ������֪ͨ
Declare_Msg_Begin(S2C_HERO_GOTO_NOTIFY, MSG_S2C_HERO_GOTO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_DecodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tSrcPos.wMapY)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapX)
Declare_EncodeMsg_UnsignedShort(m_tDestPos.wMapY)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwRoleId)	// ���ID
Declare_Msg_Member(MapPos, m_tSrcPos)	// Դ��
Declare_Msg_Member(MapPos, m_tDestPos)	// �յ�
Declare_Msg_End


// �����б�֪ͨ
Declare_Msg_Begin(S2C_SCENE_MONSTER_LIST_NOTICE, MSG_S2C_SCENE_MONSTER_LIST_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMonsterNum)
for (WORD i = 0; i < m_wMonsterNum; ++i)
{
	SceneMonster &tMonster = m_arMonsterList[i];
	Declare_DecodeMsg_UnsignedInt(tMonster.dwUID);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwMapId);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwDamage);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwTotalHP);
	Declare_DecodeMsg_UnsignedInt(tMonster.dwStartBattleTime);
	Declare_DecodeMsg_UnsignedShort(tMonster.tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tMonster.tMapPos.wMapY);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMonsterNum)
for (WORD i = 0; i < m_wMonsterNum; ++i)
{
	SceneMonster &tMonster = m_arMonsterList[i];
	Declare_EncodeMsg_UnsignedInt(tMonster.dwUID);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwMapId);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwDamage);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwTotalHP);
	Declare_EncodeMsg_UnsignedInt(tMonster.dwStartBattleTime);
	Declare_EncodeMsg_UnsignedShort(tMonster.tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tMonster.tMapPos.wMapY);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMonsterNum)	// ��������
Declare_Msg_Member_Array(SceneMonster, m_arMonsterList, MAX_ATTACK_CITY_MONSTER_NUM)	// �����б�
Declare_Msg_End


// ��������֪ͨ
Declare_Msg_Begin(S2C_SCENE_MONSTER_DATA_NOTICE, MSG_S2C_SCENE_MONSTER_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwUID);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwMapId);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwDamage);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwTotalHP);
Declare_DecodeMsg_UnsignedInt(m_tMonster.dwStartBattleTime);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapX);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapY);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwUID);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwMapId);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwDamage);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwTotalHP);
Declare_EncodeMsg_UnsignedInt(m_tMonster.dwStartBattleTime);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapX);
// Declare_DecodeMsg_UnsignedShort(m_tMonster.tMapPos.wMapY);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SceneMonster, m_tMonster)	// ��������
Declare_Msg_End


// ������Ʒ�б�֪ͨ
Declare_Msg_Begin(S2C_SCENE_ITEM_LIST_NOTICE, MSG_S2C_SCENE_ITEM_LIST_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSceneItemNum)
for (WORD i = 0; i < m_wSceneItemNum; ++i)
{
	SceneItem &tItem = m_SceneItemList[i];
	Declare_DecodeMsg_UnsignedInt(tItem.dwUID);
	Declare_DecodeMsg_UnsignedInt(tItem.dwInfoId);
	Declare_DecodeMsg_UnsignedShort(tItem.tMapPos.wMapX);
	Declare_DecodeMsg_UnsignedShort(tItem.tMapPos.wMapY);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSceneItemNum)
for (WORD i = 0; i < m_wSceneItemNum; ++i)
{
	SceneItem &tItem = m_SceneItemList[i];
	Declare_EncodeMsg_UnsignedInt(tItem.dwUID);
	Declare_EncodeMsg_UnsignedInt(tItem.dwInfoId);
	Declare_EncodeMsg_UnsignedShort(tItem.tMapPos.wMapX);
	Declare_EncodeMsg_UnsignedShort(tItem.tMapPos.wMapY);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSceneItemNum)	// ��Ʒ����
Declare_Msg_Member_Array(SceneItem, m_SceneItemList, MAX_ATTACK_CITY_TREASURE_BOX_NUM)	// ��Ʒ�б�
Declare_Msg_End


// ������Ʒ��ʧ֪ͨ
Declare_Msg_Begin(S2C_SCENE_ITEM_DISAPPEAR_NOTICE, MSG_S2C_SCENE_ITEM_DISAPPEAR_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ������Ʒ��ס����
Declare_Msg_Begin(C2S_SCENE_ITEM_LOCK_REQ, MSG_C2S_SCENE_ITEM_LOCK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ������Ʒ��ס��Ӧ
Declare_Msg_Begin(S2C_SCENE_ITEM_LOCK_RESP, MSG_S2C_SCENE_ITEM_LOCK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid(0��ʾ�ѱ���Č�ë��ס)
Declare_Msg_End

// ������Ʒ��ס֪ͨ
Declare_Msg_Begin(S2C_SCENE_ITEM_LOCK_NOTIFY, MSG_S2C_SCENE_ITEM_LOCK_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_UnsignedInt(m_dwRoleId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid(0��ʾ�ѱ���Č�ë��ס)
Declare_Msg_Member(DWORD, m_dwRoleId)	// ����Ʒ�Ľ�ɫID
Declare_Msg_End


// ������Ʒʰȡ����
Declare_Msg_Begin(C2S_SCENE_ITEM_PICK_UP_REQ, MSG_C2S_SCENE_ITEM_PICK_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// ��������
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_End


// ������Ʒʰȡ��Ӧ
Declare_Msg_Begin(S2C_SCENE_ITEM_PICK_UP_RESP, MSG_S2C_SCENE_ITEM_PICK_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_UnsignedInt(m_dwInfoId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_UnsignedInt(m_dwInfoId);
Declare_EncodeMsg_UnsignedShort(m_vItemList.size());
for (size_t i = 0; i < m_vItemList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_vItemList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// ��ϢID
Declare_Msg_Member(vector<Item>, m_vItemList)	// ��Ʒ�б�
Declare_Msg_End


// ������Ʒʰȡ֪ͨ
Declare_Msg_Begin(S2C_SCENE_ITEM_PICK_UP_NOTIFY, MSG_S2C_SCENE_ITEM_PICK_UP_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_UnsignedInt(m_dwInfoId);
Declare_DecodeMsg_UnsignedInt(m_dwRoleId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_UnsignedInt(m_dwInfoId);
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// ����uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// ��ϢID
Declare_Msg_Member(DWORD, m_dwRoleId)	// ʰȡ��ɫID
Declare_Msg_End


// ��Ӹ����齱����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LOTTERY_REQ, MSG_C2S_TEAM_DUNGEON_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// �齱����
Declare_Msg_Member(BYTE, m_byIsCostItem)	// �Ƿ����ĵ���
Declare_Msg_End

// ��Ӹ����齱����
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LOTTERY_RESP, MSG_S2C_TEAM_DUNGEON_LOTTERY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
m_wItemNum = min(m_wItemNum, MAX_TEAM_DUNGEON_LOTTERY_TIMES);
for (WORD i = 0; i < m_wItemNum; ++i)
{
	Declare_DecodeMsg_UnsignedInt(m_ItemList[i].dwId);
	Declare_DecodeMsg_UnsignedInt(m_ItemList[i].dwNum);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
for (WORD i = 0; i < m_wItemNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_ItemList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_ItemList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// �齱����
Declare_Msg_Member(WORD, m_wItemNum)	// ��Ʒ����
Declare_Msg_Member_Array(Item, m_ItemList, MAX_TEAM_DUNGEON_LOTTERY_TIMES)	// ��Ʒ�б�
Declare_Msg_End

// ��������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ, MSG_C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRight)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRight)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsRight)	// �Ƿ���
Declare_Msg_Member(BYTE, m_byAnswer)	// ��
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP, MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRight)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRight)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsRight)	// �Ƿ���
Declare_Msg_Member(BYTE, m_byAnswer)	// ��
Declare_Msg_End

// ����֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY, MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// ��
Declare_Msg_Member(BYTE, m_byAnswer)	// ��
Declare_Msg_End

// �Ի���������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_DIALOG_OVER_REQ, MSG_C2S_TEAM_DUNGEON_DIALOG_OVER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �Ի�������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DIALOG_OVER_RESP, MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �Ի�������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY, MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// ��һ��NPC����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_NEXT_NPC_REQ, MSG_C2S_TEAM_DUNGEON_NEXT_NPC_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// ��һ��NPC��Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_NEXT_NPC_RESP, MSG_S2C_TEAM_DUNGEON_NEXT_NPC_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// ��һ��NPC֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY, MSG_S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// Boss����ʼ����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ, MSG_C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//  Boss����ʼ��Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP, MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// Boss����ʼ֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY, MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY, MSG_S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTaskId)
Declare_DecodeMsg_UnsignedShort(m_wQuestionId)
Declare_DecodeMsg_UnsignedChar(m_byIsNewTask)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTaskId)
Declare_EncodeMsg_UnsignedShort(m_wQuestionId)
Declare_EncodeMsg_UnsignedChar(m_byIsNewTask)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTaskId)	// ����ID
Declare_Msg_Member(WORD, m_wQuestionId)	// ��ĿID
Declare_Msg_Member(BYTE, m_byIsNewTask)	// �Ƿ�������
Declare_Msg_End

// ������������
Declare_Msg_Begin(S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY, MSG_S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_DecodeMsg_UnsignedShort(m_wTaskLinkId)
Declare_DecodeMsg_UnsignedShort(m_wMemberNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_EncodeMsg_UnsignedShort(m_wTaskLinkId)
Declare_EncodeMsg_UnsignedShort(m_wMemberNum)
for (WORD i = 0; i < m_wMemberNum; ++i)
{
	PlayerCommonData &tMember = m_MemberList[i];
	Declare_EncodeMsg_UnsignedInt(tMember.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tMember.wLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tMember.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tMember.byVipLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wServerId);
	Declare_EncodeMsg_String(tMember.szRoleName, sizeof(tMember.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTastStartTime)	// ����ʼʱ��
Declare_Msg_Member(WORD, m_wTaskLinkId)	// ������ID
Declare_Msg_Member(WORD, m_wMemberNum)	// ��Ա����
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// ��Ա�б�
Declare_Msg_End

// �뿪��������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LEAVE_REQ, MSG_C2S_TEAM_DUNGEON_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �뿪������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LEAVE_RESP, MSG_S2C_TEAM_DUNGEON_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �뿪����֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LEAVE_NOTIFY, MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byIsTaskTeam)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byIsTaskTeam)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId) // �뿪���ID
Declare_Msg_Member(BYTE, m_byIsTaskTeam) // �Ƿ��������
Declare_Msg_End

// ��ɢ����֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DISMISS_NOTIFY, MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsCaptainLeave)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsCaptainLeave)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsCaptainLeave) // �Ƿ��Ƕӳ��˳�
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_CREATE_TEAM_REQ, MSG_C2S_TEAM_DUNGEON_CREATE_TEAM_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_End

// ����������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CREATE_TEAM_RESP, MSG_S2C_TEAM_DUNGEON_CREATE_TEAM_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����ID
Declare_Msg_End

// �����������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_INVITE_REQ, MSG_C2S_TEAM_DUNGEON_INVITE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// Ŀ��ID
Declare_Msg_End

// ���������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_INVITE_RESP, MSG_S2C_TEAM_DUNGEON_INVITE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// Ŀ��ID
Declare_Msg_End

// �������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_INVITE_NOTIFY, MSG_S2C_TEAM_DUNGEON_INVITE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName));
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wServerId)	// ������ID
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// �ӳ�����
Declare_Msg_Member(WORD, m_wDungeonId)	// ����id
Declare_Msg_End


// ���������������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ, MSG_C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wDungeonId)	// ����id
Declare_Msg_End

// �������������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP, MSG_S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wMemberNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedShort(m_wMemberNum)
for (WORD i = 0; i < m_wMemberNum; ++i)
{
	PlayerCommonData &tMember = m_MemberList[i];
	Declare_EncodeMsg_UnsignedInt(tMember.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tMember.wLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tMember.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tMember.byVipLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wServerId);
	Declare_EncodeMsg_String(tMember.szRoleName, sizeof(tMember.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(WORD, m_wMemberNum)	// ��Ա����
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// ��Ա�б�
Declare_Msg_End


// �����������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY, MSG_S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_NewMember.dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_NewMember.dwRoleId);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wLevel);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wHeadIcon);
Declare_EncodeMsg_UnsignedInt(m_NewMember.dwFightingPower);
Declare_EncodeMsg_UnsignedChar(m_NewMember.byVipLevel);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wServerId);
Declare_EncodeMsg_String(m_NewMember.szRoleName, sizeof(m_NewMember.szRoleName));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerCommonData, m_NewMember)	// �³�Ա
Declare_Msg_End

// �����������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_JOIN_IN_REQ, MSG_C2S_TEAM_DUNGEON_JOIN_IN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wDungeonId)	// ����id
Declare_Msg_End

// ���������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_JOIN_IN_RESP, MSG_S2C_TEAM_DUNGEON_JOIN_IN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wMemberNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedShort(m_wMemberNum)
for (WORD i = 0; i < m_wMemberNum; ++i)
{
	PlayerCommonData &tMember = m_MemberList[i];
	Declare_EncodeMsg_UnsignedInt(tMember.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tMember.wLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tMember.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tMember.byVipLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wServerId);
	Declare_EncodeMsg_String(tMember.szRoleName, sizeof(tMember.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(WORD, m_wMemberNum)	// ��Ա����
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// ��Ա�б�
Declare_Msg_End


// �������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_JOIN_IN_NOTIFY, MSG_S2C_TEAM_DUNGEON_JOIN_IN_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_NewMember.dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_NewMember.dwRoleId);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wLevel);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wHeadIcon);
Declare_EncodeMsg_UnsignedInt(m_NewMember.dwFightingPower);
Declare_EncodeMsg_UnsignedChar(m_NewMember.byVipLevel);
Declare_EncodeMsg_UnsignedShort(m_NewMember.wServerId);
Declare_EncodeMsg_String(m_NewMember.szRoleName, sizeof(m_NewMember.szRoleName));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerCommonData, m_NewMember)	// �³�Ա
Declare_Msg_End

// ���ƥ������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_MATCH_REQ, MSG_C2S_TEAM_DUNGEON_MATCH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedChar(m_byIsTeam);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedChar(m_byIsTeam);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(BYTE, m_byIsTeam)	// �Ƿ����
Declare_Msg_End

// ���ƥ����Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_RESP, MSG_S2C_TEAM_DUNGEON_MATCH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedChar(m_byIsTeam);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedChar(m_byIsTeam);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(BYTE, m_byIsTeam)	// �Ƿ����
Declare_Msg_End

// ���ȡ��ƥ������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ, MSG_C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���ȡ��ƥ������
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP, MSG_S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������Ƽ��б�����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ, MSG_C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������Ƽ��б���Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP, MSG_S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPlayerNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPlayerNum)
for (WORD i = 0; i < m_wPlayerNum; ++i)
{
	PlayerCommonData &tPlayer = m_PlayerList[i];
	Declare_EncodeMsg_UnsignedInt(tPlayer.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tPlayer.wLevel);
	Declare_EncodeMsg_UnsignedShort(tPlayer.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tPlayer.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tPlayer.byVipLevel);
	Declare_EncodeMsg_UnsignedShort(tPlayer.wServerId);
	Declare_EncodeMsg_String(tPlayer.szRoleName, sizeof(tPlayer.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPlayerNum)	// �������
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_TEAM_DUNGEON_RECOMMEND_LIST_NUM)	// ����б�
Declare_Msg_End

// ׼����������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY, MSG_S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wMemberNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wMemberNum)
for (WORD i = 0; i < m_wMemberNum; ++i)
{
	PlayerCommonData &tMember = m_MemberList[i];
	Declare_EncodeMsg_UnsignedInt(tMember.dwRoleId);
	Declare_EncodeMsg_UnsignedShort(tMember.wLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wHeadIcon);
	Declare_EncodeMsg_UnsignedInt(tMember.dwFightingPower);
	Declare_EncodeMsg_UnsignedChar(tMember.byVipLevel);
	Declare_EncodeMsg_UnsignedShort(tMember.wServerId);
	Declare_EncodeMsg_String(tMember.szRoleName, sizeof(tMember.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// ����Ķ���ID
Declare_Msg_Member(WORD, m_wMemberNum)	// ��Ա����
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// ��Ա�б�
Declare_Msg_End

// ����������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ, MSG_C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP, MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY, MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY, MSG_S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ӹ���������֪ͨ
Declare_Msg_Begin(S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY, MSG_S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwEnergy)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwEnergy);
Declare_EncodeMsg_UnsignedShort(m_vItemList.size())
for (size_t i = 0; i < m_vItemList.size(); ++i)
{
	Item &tItem = m_vItemList[i];
	Declare_EncodeMsg_UnsignedInt(tItem.dwId);
	Declare_EncodeMsg_UnsignedInt(tItem.dwNum);
}
Declare_EncodeMsg_UnsignedShort(m_vExtraRewardList.size())
for (size_t i = 0; i < m_vExtraRewardList.size(); ++i)
{
	Item &tItem = m_vExtraRewardList[i];
	Declare_EncodeMsg_UnsignedInt(tItem.dwId);
	Declare_EncodeMsg_UnsignedInt(tItem.dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwEnergy)	// ��������
Declare_Msg_Member(vector<Item>, m_vItemList)	// ��������Ʒ�б�
Declare_Msg_Member(vector<Item>, m_vExtraRewardList)	// ���⽱���б�
Declare_Msg_End

// ���ƥ��֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_NOTIFY, MSG_S2C_TEAM_DUNGEON_MATCH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ȡ�����ƥ��֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY, MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ʼ����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_TASK_START_REQ, MSG_C2S_TEAM_DUNGEON_TASK_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ʼ��Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_TASK_START_RESP, MSG_S2C_TEAM_DUNGEON_TASK_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����ʼ֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_TASK_START_NOTIFY, MSG_S2C_TEAM_DUNGEON_TASK_START_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_EncodeMsg_UnsignedInt(m_dwTaskLinkId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTastStartTime)	// ����ʼʱ��
Declare_Msg_Member(DWORD, m_dwTaskLinkId)	// ������ID
Declare_Msg_End


// �������а�����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_RANK_REQ, MSG_C2S_TEAM_DUNGEON_RANK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ʼʱ��
Declare_Msg_End


// �������а���Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_RANK_RESP, MSG_S2C_TEAM_DUNGEON_RANK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedShort(m_wRankdNum)
for (WORD i = 0; i < m_wRankdNum; ++i)
{
	TeamDungeonRank &tRank = m_arRankList[i];
	Declare_EncodeMsg_UnsignedShort(tRank.wRank);
	Declare_EncodeMsg_UnsignedInt(tRank.dwUseTime);
	Declare_EncodeMsg_UnsignedInt(tRank.dwCaptainHeadImgId);
	Declare_EncodeMsg_UnsignedShort(MAX_TEAM_MEMBER_NUM);
	for (WORD k = 0; k < MAX_TEAM_MEMBER_NUM; ++k)
	{
		TeamDungeonMember &tMember = tRank.arMemberList[k];
		Declare_EncodeMsg_UnsignedShort(tMember.wServerId);
		Declare_EncodeMsg_String(tMember.szRoleName, sizeof(tMember.szRoleName));
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ʼʱ��
Declare_Msg_Member(WORD, m_wRankdNum)	// ���а�����
Declare_Msg_Member_Array(TeamDungeonRank, m_arRankList, MAX_TEAM_DUNGEON_NUM)	// ���а��б�
Declare_Msg_End

// ��Ӹ���BOSS����֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY, MSG_S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBossDamage);
Declare_EncodeMsg_UnsignedInt(m_dwMVPId);
Declare_EncodeMsg_UnsignedInt(m_dwKillerId);
Declare_EncodeMsg_UnsignedShort(m_vMVPRewardList.size())
for (size_t i = 0; i < m_vMVPRewardList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vMVPRewardList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_vMVPRewardList[i].dwNum);
}
Declare_EncodeMsg_UnsignedShort(m_vKillRewardList.size())
for (size_t i = 0; i < m_vKillRewardList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vKillRewardList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_vKillRewardList[i].dwNum);
}
Declare_EncodeMsg_UnsignedShort(m_wMemberNum)
for (WORD i = 0; i < m_wMemberNum; ++i)
{
	TeamDungeonBossReward &tReward = m_MemberRewardList[i];
	Declare_EncodeMsg_UnsignedInt(tReward.dwRoleId);
	Declare_EncodeMsg_UnsignedInt(tReward.dwAttackDamage);
	Declare_EncodeMsg_UnsignedShort(tReward.vRewardList.size());
	for (WORD k = 0; k < tReward.vRewardList.size(); ++k)
	{
		Declare_EncodeMsg_UnsignedInt(tReward.vRewardList[k].dwId);
		Declare_EncodeMsg_UnsignedInt(tReward.vRewardList[k].dwNum);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBossDamage)	// BOSS�ܵ����˺�
Declare_Msg_Member(DWORD, m_dwMVPId)	// MVPId
Declare_Msg_Member(DWORD, m_dwKillerId)	// ��ɱ��ID
Declare_Msg_Member(WORD, m_wMemberNum)	// ��Ա����
Declare_Msg_Member_Array(TeamDungeonBossReward, m_MemberRewardList, MAX_TEAM_MEMBER_NUM)	// ��Ա�����б�
Declare_Msg_Member(vector<Item>, m_vMVPRewardList)	// mvp�����б�
Declare_Msg_Member(vector<Item>, m_vKillRewardList)	// ��ɱ�����б�
Declare_Msg_End


// ��Ӹ�����������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_DATA_REQ, MSG_C2S_TEAM_DUNGEON_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ӹ���������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DATA_RESP, MSG_S2C_TEAM_DUNGEON_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tTeamDungeonData.wTodayBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tTeamDungeonData.wTodayBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_tTeamDungeonData.wPassDungeonNum)
Declare_EncodeMsg_Memory(m_tTeamDungeonData.arPassDungeonList, m_tTeamDungeonData.wPassDungeonNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerTeamDungeonData, m_tTeamDungeonData)	// ��������
Declare_Msg_End


// ��ӿ����������
Declare_Msg_Begin(C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ, MSG_C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChatId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChatId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChatId)	// ����ID
Declare_Msg_End

// ��ӿ��������Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP, MSG_S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedShort(m_wChatId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedShort(m_wChatId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_dwRoleId)	// ��ɫID
Declare_Msg_Member(WORD, m_wChatId)	// ����ID
Declare_Msg_End

// ��Ӹ����齱��¼����
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ, MSG_C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Ӹ����齱��¼��Ӧ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP, MSG_S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLotteryRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_LotteryRecordList.size())
for (size_t i = 0; i < m_LotteryRecordList.size(); ++i)
{
	TeamDungeonLotteryRecord &tRecord = m_LotteryRecordList[i];
	Declare_EncodeMsg_UnsignedShort(tRecord.wServerId);
	Declare_EncodeMsg_String(tRecord.szRoleName, sizeof(tRecord.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tRecord.dwItemId);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwItemNum);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLotteryRecordNum)	// �齱��¼����
Declare_Msg_Member(vector<TeamDungeonLotteryRecord>, m_LotteryRecordList) // �齱��¼�б�
Declare_Msg_End


// ��Ӹ���ƥ��Ŀ������֪ͨ
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY, MSG_S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedShort(m_wTargetNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedShort(m_wTargetNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// ����ID
Declare_Msg_Member(WORD, m_wTargetNum)	// Ŀ������
Declare_Msg_End


// ��¼������������
Declare_Msg_Begin(C2S_LOGIN_REWARD_DATA_REQ, MSG_C2S_LOGIN_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��¼����������Ӧ
Declare_Msg_Begin(S2C_LOGIN_REWARD_DATA_RESP, MSG_S2C_LOGIN_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLoginDays)
Declare_DecodeMsg_UnsignedShort(m_wRewardRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLoginDays)
Declare_EncodeMsg_UnsignedShort(m_wRewardRecordNum)
Declare_EncodeMsg_Memory(m_RewardRecordList, m_wRewardRecordNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLoginDays)	// ��¼����
Declare_Msg_Member(WORD, m_wRewardRecordNum)	// ������¼����
Declare_Msg_Member_Array(WORD, m_RewardRecordList, MAX_LOGIN_REWARD_RECORD_NUM)	// ������¼�б�
Declare_Msg_End

// ��ȡ��¼��������
Declare_Msg_Begin(C2S_TAKE_LOGIN_REWARD_REQ, MSG_C2S_TAKE_LOGIN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDays)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDays)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDays)	// ����
Declare_Msg_End

// ��ȡ��¼������Ӧ
Declare_Msg_Begin(S2C_TAKE_LOGIN_REWARD_RESP, MSG_S2C_TAKE_LOGIN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDays)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDays)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDays)	// ����
Declare_Msg_End

// �ͻ��˸���֪ͨ
Declare_Msg_Begin(S2C_CLIENT_UPDATE_NOTIFY, MSG_S2C_CLIENT_UPDATE_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �����������������
Declare_Msg_Begin(C2S_KING_MATCH_DATA_REQ, MSG_C2S_KING_MATCH_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���������������Ӧ
Declare_Msg_Begin(S2C_KING_MATCH_DATA_RESP, MSG_S2C_KING_MATCH_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurSeason)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurSeason)
Declare_EncodeMsg_UnsignedShort(m_wMyRank)
Declare_EncodeMsg_UnsignedInt(m_dwMyRankValue)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wTotalBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wTotalWinTimes)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wTodayWinTimes)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wStageLevel)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wHistoryStageLevel)
Declare_EncodeMsg_UnsignedInt(m_KingMatchData.dwContext)
Declare_EncodeMsg_UnsignedShort(m_KingMatchData.wSeasonRewardRecordNum)
Declare_EncodeMsg_Memory(m_KingMatchData.arSeasonTaskRewardRecord, m_KingMatchData.wSeasonRewardRecordNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurSeason)		// ��ǰ����
Declare_Msg_Member(WORD, m_wMyRank)			// �ҵ�����
Declare_Msg_Member(DWORD, m_dwMyRankValue)	// �ҵ�����ֵ
Declare_Msg_Member(PlayerKingMatchData, m_KingMatchData)	// ����������
Declare_Msg_End

// ����Ŀ������
Declare_Msg_Begin(C2S_KING_MATCH_SEARCH_TARGET_REQ, MSG_C2S_KING_MATCH_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����Ŀ����Ӧ
Declare_Msg_Begin(S2C_KING_MATCH_SEARCH_TARGET_RESP, MSG_S2C_KING_MATCH_SEARCH_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_TargetData.dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwRoleId);
Declare_EncodeMsg_UnsignedShort(m_TargetData.wLevel);
Declare_EncodeMsg_UnsignedShort(m_TargetData.wHeadIcon);
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwFightingPower);
Declare_EncodeMsg_UnsignedChar(m_TargetData.byVipLevel);
Declare_EncodeMsg_UnsignedShort(m_TargetData.wServerId);
Declare_EncodeMsg_String(m_TargetData.szRoleName, sizeof(m_TargetData.szRoleName));
Declare_EncodeMsg_UnsignedInt(m_TargetData.dwTitleId);
Declare_EncodeMsg_UnsignedShort(m_wStageLevel);
Declare_EncodeMsg_UnsignedChar(m_byStar);
Declare_EncodeMsg_UnsignedShort(m_wRank);
Declare_EncodeMsg_UnsignedInt(m_dwRankValue);

Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerCommonData, m_TargetData)	// Ŀ������
Declare_Msg_Member(WORD, m_wStageLevel)		// ��λ�ȼ�
Declare_Msg_Member(BYTE, m_byStar)			// ����
Declare_Msg_Member(WORD, m_wRank)			// ����
Declare_Msg_Member(DWORD, m_dwRankValue)	// ����ֵ
Declare_Msg_End


// ��ȡ��������������
Declare_Msg_Begin(C2S_TAKE_KING_MATCH_TASK_REWARD_REQ, MSG_C2S_TAKE_KING_MATCH_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWinTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWinTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWinTimes)	// ��ʤ����
Declare_Msg_End


// ��ȡ��������������
Declare_Msg_Begin(S2C_TAKE_KING_MATCH_TASK_REWARD_RESP, MSG_S2C_TAKE_KING_MATCH_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWinTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWinTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWinTimes)	// ��ʤ����
Declare_Msg_End


// �������а�����
Declare_Msg_Begin(C2S_KING_MATCH_SEASON_RANK_REQ, MSG_C2S_KING_MATCH_SEASON_RANK_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �������а���Ӧ
Declare_Msg_Begin(S2C_KING_MATCH_SEASON_RANK_RESP, MSG_S2C_KING_MATCH_SEASON_RANK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSeasonRankNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSeasonRankNum)
for (WORD i = 0; i < m_wSeasonRankNum; ++i)
{
	KingMatchRankRecord tSeasonRankData = m_SeasonRankList[i];
	Declare_EncodeMsg_UnsignedShort(tSeasonRankData.wSeason);
	Declare_EncodeMsg_UnsignedShort(tSeasonRankData.wRoleRankNum);
	for (WORD k = 0; k < tSeasonRankData.wRoleRankNum; ++k)
	{
		PlayerCommonData &tRoleRankData = tSeasonRankData.arRoleRankList[k];
		Declare_EncodeMsg_UnsignedShort(tRoleRankData.wServerId);
		Declare_EncodeMsg_String(tRoleRankData.szRoleName, sizeof(tRoleRankData.szRoleName));
		Declare_EncodeMsg_UnsignedShort(tRoleRankData.wHeadIcon);
		Declare_EncodeMsg_UnsignedShort(tRoleRankData.wLevel);
		Declare_EncodeMsg_UnsignedInt(tRoleRankData.dwTitleId);
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSeasonRankNum)	// ������������
Declare_Msg_Member_Array(KingMatchRankRecord, m_SeasonRankList, RECENTLY_KING_MATCH_SEASON_RANK_NUM)	// ���������б�
Declare_Msg_End


// ����ʥ��������
Declare_Msg_Begin(C2S_CHRISTMAS_DATA_REQ, MSG_C2S_CHRISTMAS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����ʥ����������Ӧ
Declare_Msg_Begin(S2C_CHRISTMAS_DATA_RESP, MSG_S2C_CHRISTMAS_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLowerBossLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTotalScore)
Declare_EncodeMsg_UnsignedInt(m_dwCurScore)
Declare_EncodeMsg_UnsignedShort(m_wLowerBossLevel)
Declare_EncodeMsg_UnsignedInt(m_dwLowerBossHP)
Declare_EncodeMsg_UnsignedInt(m_dwLowerBossDeadTime)
Declare_EncodeMsg_UnsignedShort(m_wHighBossLevel)
Declare_EncodeMsg_UnsignedInt(m_dwHighBossHP)
Declare_EncodeMsg_UnsignedInt(m_dwHighBossDeadTime)
Declare_EncodeMsg_UnsignedShort(m_wAlreadyTakeLowerBossRewardLevel)
Declare_EncodeMsg_UnsignedShort(m_wAlreadyTakeHighBossRewardLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTotalScore)		// �ܻ���
Declare_Msg_Member(DWORD, m_dwCurScore)			// ��ǰ����
Declare_Msg_Member(WORD, m_wLowerBossLevel)		// �ͼ�boss����
Declare_Msg_Member(DWORD, m_dwLowerBossHP)		// �ͼ�bossHP
Declare_Msg_Member(DWORD, m_dwLowerBossDeadTime)// �ͼ�boss����ʱ��
Declare_Msg_Member(WORD, m_wHighBossLevel)		// �߼�boss����
Declare_Msg_Member(DWORD, m_dwHighBossHP)		// �߼�bossHP
Declare_Msg_Member(DWORD, m_dwHighBossDeadTime)	// �߼�boss����ʱ��
Declare_Msg_Member(WORD, m_wAlreadyTakeLowerBossRewardLevel)		// ����ȡ�ĵͼ�boss�����ȼ�
Declare_Msg_Member(WORD, m_wAlreadyTakeHighBossRewardLevel)		// ����ȡ�ĸ߼�boss�����ȼ�
Declare_Msg_End


// ����ս������
Declare_Msg_Begin(C2S_CHRISTMAS_BATTLE_ENTER_REQ, MSG_C2S_CHRISTMAS_BATTLE_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����ս����Ӧ
Declare_Msg_Begin(S2C_CHRISTMAS_BATTLE_ENTER_RESP, MSG_S2C_CHRISTMAS_BATTLE_ENTER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �뿪ս������
Declare_Msg_Begin(C2S_CHRISTMAS_BATTLE_LEAVE_REQ, MSG_C2S_CHRISTMAS_BATTLE_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �뿪ս����Ӧ
Declare_Msg_Begin(S2C_CHRISTMAS_BATTLE_LEAVE_RESP, MSG_S2C_CHRISTMAS_BATTLE_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ����Boss����
Declare_Msg_Begin(C2S_CHRISTMAS_ATTACK_BOSS_REQ, MSG_C2S_CHRISTMAS_ATTACK_BOSS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_UnsignedInt(m_dwSnowBallId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_UnsignedInt(m_dwSnowBallId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_Member(DWORD, m_dwSnowBallId)	// ѩ��ID
Declare_Msg_End


// ����Boss��Ӧ
Declare_Msg_Begin(S2C_CHRISTMAS_ATTACK_BOSS_RESP, MSG_S2C_CHRISTMAS_ATTACK_BOSS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_UnsignedInt(m_dwSnowBallId)
Declare_EncodeMsg_UnsignedInt(m_dwDamage)
Declare_EncodeMsg_UnsignedInt(m_tRewardItem.dwId)
Declare_EncodeMsg_UnsignedInt(m_tRewardItem.dwNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_Member(DWORD, m_dwSnowBallId)	// ѩ��ID
Declare_Msg_Member(DWORD, m_dwDamage)		// �˺�
Declare_Msg_Member(Item, m_tRewardItem)		// ������Ʒ
Declare_Msg_End

// ����Boss֪ͨ
Declare_Msg_Begin(S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY, MSG_S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_String(m_szAttackerName, sizeof(m_szAttackerName));
Declare_EncodeMsg_UnsignedInt(m_dwDamage)
Declare_EncodeMsg_UnsignedInt(m_tRewardItem.dwId)
Declare_EncodeMsg_UnsignedInt(m_tRewardItem.dwNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_Member_Array(char, m_szAttackerName, MAX_ROLE_LEN)	// ����������
Declare_Msg_Member(DWORD, m_dwDamage)		// �˺�
Declare_Msg_Member(Item, m_tRewardItem)		// ������Ʒ
Declare_Msg_End


// ��ȡ��������
Declare_Msg_Begin(C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ, MSG_C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_End


// ��ȡ������Ӧ
Declare_Msg_Begin(S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP, MSG_S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_End


//  Boss����֪ͨ
Declare_Msg_Begin(S2C_CHRISTMAS_BOSS_DEAD_NOTIFY, MSG_S2C_CHRISTMAS_BOSS_DEAD_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_UnsignedInt(m_dwDeadTime)
Declare_EncodeMsg_UnsignedShort(m_wBossLevel)
Declare_EncodeMsg_UnsignedInt(m_dwBossHP)
Declare_EncodeMsg_UnsignedInt(m_dwKilLerId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss����
Declare_Msg_Member(DWORD, m_dwDeadTime)		// ����ʱ��
Declare_Msg_Member(WORD, m_wBossLevel)		// boss�ȼ�(�����Ժ��Boss�ȼ�)
Declare_Msg_Member(DWORD, m_dwBossHP)		// bossHP(�����Ժ��BossHP)
Declare_Msg_Member(DWORD, m_dwKilLerId)		// ��ɱ��ID
Declare_Msg_End


// ���������
Declare_Msg_Begin(C2S_FACEBOOK_ACTIVITY_DATA_REQ, MSG_C2S_FACEBOOK_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �������Ӧ
Declare_Msg_Begin(S2C_FACEBOOK_ACTIVITY_DATA_RESP, MSG_S2C_FACEBOOK_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tActivityData.wIsAlreadyTakeAccountBindingReward)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wIsAlreadyTakeAccountBindingReward)
Declare_EncodeMsg_UnsignedInt(m_tActivityData.dwReadyToShareTime)
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wSummonSuccessFriendNum)
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wInviteSuccessFriendNum)
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wSummonRewardRecordNum)
Declare_EncodeMsg_Memory(m_tActivityData.arSummonRewardRecord, m_tActivityData.wSummonRewardRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wInviteRewardRecordNum)
Declare_EncodeMsg_Memory(m_tActivityData.arInviteRewardRecord, m_tActivityData.wInviteRewardRecordNum*sizeof(WORD));
Declare_EncodeMsg_UnsignedShort(m_tActivityData.wSummonFriendRecordNum)
for (WORD i = 0; i < m_tActivityData.wSummonFriendRecordNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_tActivityData.arSummonFriendRecord[i].dwFriendId)
}
Declare_EncodeMsg_UnsignedInt(m_tActivityData.dwReadyToCommendTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FacebookActivityData, m_tActivityData)		// �����
Declare_Msg_End


// ��ȡ�ʺŰ󶨽�������
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ȡ�ʺŰ󶨽�����Ӧ
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ȡ����������
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ȡ��������Ӧ
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ���۽�������
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��ȡ���۽�����Ӧ
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// ������������
Declare_Msg_Begin(C2S_FACEBOOK_FRIEND_INVITE_REQ, MSG_C2S_FACEBOOK_FRIEND_INVITE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_InviteFriendInfo.wFriendNum)
m_InviteFriendInfo.wFriendNum = min(m_InviteFriendInfo.wFriendNum, MAX_FACEBOOK_FRIEND_NUM);
for (WORD i = 0; i < m_InviteFriendInfo.wFriendNum; ++i)
{
	Declare_DecodeMsg_String(m_InviteFriendInfo.arFriendList[i], sizeof(m_InviteFriendInfo.arFriendList[i]));
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_InviteFriendInfo.wFriendNum)
for (WORD i = 0; i < m_InviteFriendInfo.wFriendNum; ++i)
{
	Declare_EncodeMsg_String(m_InviteFriendInfo.arFriendList[i], sizeof(m_InviteFriendInfo.arFriendList[i]));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendInfo)		// ����ĺ�����Ϣ
Declare_Msg_End


// ����������Ӧ
Declare_Msg_Begin(S2C_FACEBOOK_FRIEND_INVITE_RESP, MSG_S2C_FACEBOOK_FRIEND_INVITE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_InviteFriendInfo.wFriendNum)
m_InviteFriendInfo.wFriendNum = min(m_InviteFriendInfo.wFriendNum, MAX_FACEBOOK_FRIEND_NUM);
for (WORD i = 0; i < m_InviteFriendInfo.wFriendNum; ++i)
{
	Declare_DecodeMsg_String(m_InviteFriendInfo.arFriendList[i], sizeof(m_InviteFriendInfo.arFriendList[i]));
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_InviteFriendInfo.wFriendNum)
for (WORD i = 0; i < m_InviteFriendInfo.wFriendNum; ++i)
{
	Declare_EncodeMsg_String(m_InviteFriendInfo.arFriendList[i], sizeof(m_InviteFriendInfo.arFriendList[i]));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendInfo)		// ����ĺ�����Ϣ
Declare_Msg_End


// �����ٻ�����
Declare_Msg_Begin(C2S_FACEBOOK_FRIEND_SUMMON_REQ, MSG_C2S_FACEBOOK_FRIEND_SUMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFriendNum)
m_wFriendNum = min(m_wFriendNum, MAX_FRIEND_NUM);
Declare_DecodeMsg_Memory(m_arFiendList, m_wFriendNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFriendNum)
Declare_EncodeMsg_Memory(m_arFiendList, m_wFriendNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFriendNum)		// ��������
Declare_Msg_Member_Array(DWORD, m_arFiendList, MAX_FRIEND_NUM)	// �����б�
Declare_Msg_End

// �����ٻ���Ӧ
Declare_Msg_Begin(S2C_FACEBOOK_FRIEND_SUMMON_RESP, MSG_S2C_FACEBOOK_FRIEND_SUMMON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFriendNum)
m_wFriendNum = min(m_wFriendNum, MAX_FRIEND_NUM);
Declare_DecodeMsg_Memory(m_arFiendList, m_wFriendNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFriendNum)
Declare_EncodeMsg_Memory(m_arFiendList, m_wFriendNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFriendNum)		// ��������
Declare_Msg_Member_Array(DWORD, m_arFiendList, MAX_FRIEND_NUM)	// �����б�
Declare_Msg_End


// ��ȡ�������뽱������
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wInviteNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wInviteNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wInviteNum)		// ���������
Declare_Msg_End


// ��ȡ�������뽱����Ӧ
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wInviteNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wInviteNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wInviteNum)		// ���������
Declare_Msg_End


// ��ȡ�����ٻ���������
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSummonNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSummonNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSummonNum)		// �ٻص�����
Declare_Msg_End


// ��ȡ�����ٻ���������
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSummonNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSummonNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSummonNum)		// �ٻص�����
Declare_Msg_End


// �Ѿ�����ĺ��Ѽ�¼����
Declare_Msg_Begin(C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ, MSG_C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �Ѿ�����ĺ��Ѽ�¼��Ӧ
Declare_Msg_Begin(S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP, MSG_S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_InviteFriendRecord.wFriendNum)
m_InviteFriendRecord.wFriendNum = min(m_InviteFriendRecord.wFriendNum, MAX_FACEBOOK_FRIEND_NUM);
for (WORD i = 0; i < m_InviteFriendRecord.wFriendNum; ++i)
{
	Declare_DecodeMsg_String(m_InviteFriendRecord.arFriendList[i], sizeof(m_InviteFriendRecord.arFriendList[i]));
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_InviteFriendRecord.wFriendNum)
for (WORD i = 0; i < m_InviteFriendRecord.wFriendNum; ++i)
{
	Declare_EncodeMsg_String(m_InviteFriendRecord.arFriendList[i], sizeof(m_InviteFriendRecord.arFriendList[i]));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendRecord)		// ����ĺ��Ѽ�¼
Declare_Msg_End


// ���߽�����������
Declare_Msg_Begin(C2S_ONLINE_REWARD_DATA_REQ, MSG_C2S_ONLINE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���߽���������Ӧ
Declare_Msg_Begin(S2C_ONLINE_REWARD_DATA_RESP, MSG_S2C_ONLINE_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_OnlineRewardData.dwOnlineTime)
Declare_EncodeMsg_UnsignedShort(MAX_ONLINE_REWARD_DAY)
for (int i = 0; i < MAX_ONLINE_REWARD_DAY; ++i)
{
	Declare_EncodeMsg_UnsignedShort(i + 1);// ����
	Declare_EncodeMsg_UnsignedShort(MAX_ONLINE_REWARD_TIME_LIST);
	for (int k = 0; k < MAX_ONLINE_REWARD_TIME_LIST; ++k)
	{
		Declare_EncodeMsg_UnsignedChar(m_OnlineRewardData.arRewardRewardList[i][k]); // ��־�б�
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(OnlineRewardData, m_OnlineRewardData)		// ���߽�������
Declare_Msg_End

// ��ȡ���߽�������
Declare_Msg_Begin(C2S_TAKE_ONLINE_REWARD_REQ, MSG_C2S_TAKE_ONLINE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ���߽�����Ӧ
Declare_Msg_Begin(S2C_TAKE_ONLINE_REWARD_RESP, MSG_S2C_TAKE_ONLINE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 7���¼��������
Declare_Msg_Begin(C2S_SEVEN_DAY_LOGIN_DATA_REQ, MSG_C2S_SEVEN_DAY_LOGIN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 7���¼������Ӧ
Declare_Msg_Begin(S2C_SEVEN_DAY_LOGIN_DATA_RESP, MSG_S2C_SEVEN_DAY_LOGIN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(MAX_SEVEN_DAY_LOGIN_DAYS); // �б���
for (int i = 0; i < MAX_SEVEN_DAY_LOGIN_DAYS; ++i)
{
	Declare_EncodeMsg_UnsignedChar(i + 1); // ����
	Declare_EncodeMsg_UnsignedChar(m_Data.arDailyDataList[i]); // ÿ������
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SevenDayLoginData, m_Data)	// ��¼����
Declare_Msg_End


// 7���¼������ȡ����
Declare_Msg_Begin(C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ, MSG_C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// Ҫ��ȡ������
Declare_Msg_End

// 7���¼������ȡ��Ӧ
Declare_Msg_Begin(S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP, MSG_S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// Ҫ��ȡ������
Declare_Msg_End


// �����ֵ��������
Declare_Msg_Begin(C2S_RECHARGE_REWARD_DATA_REQ, MSG_C2S_RECHARGE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����ֵ����������Ӧ
Declare_Msg_Begin(S2C_RECHARGE_REWARD_DATA_RESP, MSG_S2C_RECHARGE_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wLastActivityDataId)
Declare_EncodeMsg_UnsignedChar(m_Data.wIsTookExtraReward)
Declare_EncodeMsg_UnsignedShort(MAX_RECHARGE_REWARD_DAY_NUM)
for (WORD i = 0; i < MAX_RECHARGE_REWARD_DAY_NUM; i++)
{
	Declare_EncodeMsg_UnsignedChar(m_Data.arDailyDataList[i])
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RecahrgeRewardData, m_Data)
Declare_Msg_End

// ��ȡ��ֵ��������
Declare_Msg_Begin(C2S_TAKE_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// Ҫ��ȡ������
Declare_Msg_End

// ��ȡ��ֵ������Ӧ
Declare_Msg_Begin(S2C_TAKE_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// Ҫ��ȡ������
Declare_Msg_End

// ��ȡ���⽱��
Declare_Msg_Begin(C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ, MSG_C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ���⽱��
Declare_Msg_Begin(S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP, MSG_S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����۳佱������
Declare_Msg_Begin(C2S_TOTAL_RECHARGE_REWARD_DATA_REQ, MSG_C2S_TOTAL_RECHARGE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����۳佱��������Ӧ
Declare_Msg_Begin(S2C_TOTAL_RECHARGE_REWARD_DATA_RESP, MSG_S2C_TOTAL_RECHARGE_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_Data.dwTotalAmount)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wLastActivityDataId)
Declare_EncodeMsg_UnsignedInt(m_Data.dwTotalAmount)
Declare_EncodeMsg_UnsignedShort(m_Data.wRewardRecordNum)
Declare_EncodeMsg_Memory(m_Data.arRewardRecordList, m_Data.wRewardRecordNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(TotalRechargeRewardData, m_Data)		// ����
Declare_Msg_End

// ��ȡ�۳佱������
Declare_Msg_Begin(C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// ����ID
Declare_Msg_End

// ��ȡ�۳佱����Ӧ
Declare_Msg_Begin(S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// ����ID
Declare_Msg_End


// Ť���齱����
Declare_Msg_Begin(C2S_ND_LOTTERY_REQ, MSG_C2S_ND_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// �齱����
Declare_Msg_Member(BYTE, m_byIsCostItem)	// �Ƿ����ĵ���
Declare_Msg_End

// Ť���齱��Ӧ
Declare_Msg_Begin(S2C_ND_LOTTERY_RESP, MSG_S2C_ND_LOTTERY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
m_wItemNum = min(m_wItemNum, MAX_ND_LOTTERY_TIMES);
for (WORD i = 0; i < m_wItemNum; ++i)
{
	Declare_DecodeMsg_UnsignedInt(m_ItemList[i].dwId);
	Declare_DecodeMsg_UnsignedInt(m_ItemList[i].dwNum);
}
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
for (WORD i = 0; i < m_wItemNum; ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_ItemList[i].dwId);
	Declare_EncodeMsg_UnsignedInt(m_ItemList[i].dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// �齱����
Declare_Msg_Member(WORD, m_wItemNum)	// ��Ʒ����
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ND_LOTTERY_TIMES)	// ��Ʒ�б�
Declare_Msg_End


// Ť���齱��¼����
Declare_Msg_Begin(C2S_ND_LOTTERY_RECORD_REQ, MSG_C2S_ND_LOTTERY_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// Ť���齱��¼��Ӧ
Declare_Msg_Begin(S2C_ND_LOTTERY_RECORD_RESP, MSG_S2C_ND_LOTTERY_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLotteryRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_LotteryRecordList.size())
for (size_t i = 0; i < m_LotteryRecordList.size(); ++i)
{
	NDLotteryRecord &tRecord = m_LotteryRecordList[i];
	Declare_EncodeMsg_UnsignedShort(tRecord.wServerId);
	Declare_EncodeMsg_String(tRecord.szRoleName, sizeof(tRecord.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tRecord.dwItemId);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwItemNum);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLotteryRecordNum)	// �齱��¼����
Declare_Msg_Member(vector<NDLotteryRecord>, m_LotteryRecordList) // �齱��¼�б�
Declare_Msg_End


// ������Ʒ��������
Declare_Msg_Begin(C2S_HOT_SALE_GOODS_DATA_REQ, MSG_C2S_HOT_SALE_GOODS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������Ʒ������Ӧ
Declare_Msg_Begin(S2C_HOT_SALE_GOODS_DATA_RESP, MSG_S2C_HOT_SALE_GOODS_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wGoodsNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wGoodsNum)
for (WORD i = 0; i < m_Data.wGoodsNum; ++i)
{
	HotSaleGoods &tGoods = m_Data.arGoodsList[i];
	Declare_EncodeMsg_UnsignedShort(tGoods.wGoodsId);
	Declare_EncodeMsg_UnsignedShort(tGoods.wCurDataIdx);
	Declare_EncodeMsg_UnsignedShort(tGoods.wBuyTimes);
	Declare_EncodeMsg_UnsignedShort(tGoods.wTakeTimes);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerHotSaleGoodsInfo, m_Data)	// ��ƷID
Declare_Msg_End


// ��ȡ������Ʒ��ֵ��������
Declare_Msg_Begin(C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// ��ƷID
Declare_Msg_End


// ��ȡ������Ʒ��ֵ������Ӧ
Declare_Msg_Begin(S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// ��ƷID
Declare_Msg_End



// ת����������
Declare_Msg_Begin(C2S_TURNTABLE_DATA_REQ, MSG_C2S_TURNTABLE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ת��������Ӧ
Declare_Msg_Begin(S2C_TURNTABLE_DATA_RESP, MSG_S2C_TURNTABLE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurActivityDataId)
Declare_DecodeMsg_UnsignedInt(m_dwDiamond)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwCurRankScore)
Declare_EncodeMsg_UnsignedShort(m_wCurActivityDataId)
Declare_EncodeMsg_UnsignedInt(m_dwDiamond)
Declare_EncodeMsg_UnsignedShort(m_LotteryRecordList.size())
for (int i = 0; i < m_LotteryRecordList.size(); ++i)
{
	ClientTurntableLotteryRecord &tRecord = m_LotteryRecordList[i];
	Declare_EncodeMsg_UnsignedInt(tRecord.dwTime);
	Declare_EncodeMsg_UnsignedShort(tRecord.wIndex);
	Declare_EncodeMsg_UnsignedInt(tRecord.dwContext);
	Declare_EncodeMsg_String(tRecord.szRoleName, sizeof(tRecord.szRoleName));
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwCurRankScore)			// ��ǰ���л���
Declare_Msg_Member(WORD, m_wCurActivityDataId)	// ��ǰ�����ID
Declare_Msg_Member(DWORD, m_dwDiamond)			// ������ʯ����
Declare_Msg_Member(vector<ClientTurntableLotteryRecord>, m_LotteryRecordList) // �齱��¼�б�
Declare_Msg_End


// ת�̳齱����
Declare_Msg_Begin(C2S_TURNTABLE_LOTTERY_REQ, MSG_C2S_TURNTABLE_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLotteryType)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLotteryType)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLotteryType)	// �齱����
Declare_Msg_Member(BYTE, m_byIsCostItem)	// �Ƿ����ĵ���
Declare_Msg_End

// ת�̳齱��Ӧ
Declare_Msg_Begin(S2C_TURNTABLE_LOTTERY_RESP, MSG_S2C_TURNTABLE_LOTTERY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTmp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLotteryType)
Declare_EncodeMsg_UnsignedShort(m_LotteryResultList.size())
for (int i = 0; i < m_LotteryResultList.size(); ++i)
{
	TurntableLotteryResultData &tLotteryResult = m_LotteryResultList[i];
	Declare_EncodeMsg_UnsignedShort(tLotteryResult.wIndex);
	Declare_EncodeMsg_UnsignedInt(tLotteryResult.dwContext);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLotteryType)	// �齱����
Declare_Msg_Member(WORD, m_wTmp)			// ��ʱ����
Declare_Msg_Member(vector<TurntableLotteryResultData>, m_LotteryResultList) // �齱����б�
Declare_Msg_End


// ��Ӫ��ļ��������
Declare_Msg_Begin(C2S_CAMP_RECRUIT_DATA_REQ, MSG_C2S_CAMP_RECRUIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ��Ӫ��ļ������Ӧ
Declare_Msg_Begin(S2C_CAMP_RECRUIT_DATA_RESP, MSG_S2C_CAMP_RECRUIT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurDataId)
Declare_DecodeMsg_UnsignedShort(m_wTodayAlreadyRecruitTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurDataId)
Declare_EncodeMsg_UnsignedShort(m_wTodayAlreadyRecruitTimes)
// �����б�
Declare_EncodeMsg_UnsignedShort(m_vCampScoreList.size())
for (int i = 0; i < m_vCampScoreList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedChar(i);	// ��Ӫ
	Declare_EncodeMsg_UnsignedInt(m_vCampScoreList[i]); // ����
}
// ������¼�б�
Declare_EncodeMsg_UnsignedShort(m_RewardRecordList.size())
for (int i = 0; i < m_RewardRecordList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedChar(i);	// ��Ӫ
	vector<WORD> &vList = m_RewardRecordList[i];
	Declare_EncodeMsg_UnsignedShort(vList.size());
	for (int k = 0; k < vList.size(); ++k)
	{
		Declare_EncodeMsg_UnsignedShort(vList[k])
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurDataId)					// ��ǰ�����ID
Declare_Msg_Member(WORD, m_wTodayAlreadyRecruitTimes)	// ��������ļ����
Declare_Msg_Member(vector<DWORD>, m_vCampScoreList)		// ��Ӫ�����б�
Declare_Msg_Member(vector<vector<WORD>>, m_RewardRecordList)		// ������¼�б�
Declare_Msg_End


// ��Ӫ��ļ����
Declare_Msg_Begin(C2S_CAMP_RECRUIT_REQ, MSG_C2S_CAMP_RECRUIT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRecuitType)
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRecuitType)
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRecuitType)	// ��ļ����(1=��ͨ,2=һ��)
Declare_Msg_Member(WORD, m_wCamp)		// ��Ӫ
Declare_Msg_End


// ��Ӫ��ļ��Ӧ
Declare_Msg_Begin(S2C_CAMP_RECRUIT_RESP, MSG_S2C_CAMP_RECRUIT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRecuitType)
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRecuitType)
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_vHeroList.size())
for (int i = 0; i < m_vHeroList.size(); ++i)
{
	Item &tItem = m_vHeroList[i];
	Declare_EncodeMsg_UnsignedInt(tItem.dwId);
	Declare_EncodeMsg_UnsignedInt(tItem.dwNum);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRecuitType)			// ��ļ����(1=��ͨ,2=һ��)
Declare_Msg_Member(WORD, m_wCamp)				// ��Ӫ
Declare_Msg_Member(vector<Item>, m_vHeroList)	// ��ļ����Ӣ���б�
Declare_Msg_End



// ��Ӫ��ļ��������������
Declare_Msg_Begin(C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ, MSG_C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_UnsignedShort(m_wRewardIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_wRewardIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// ��Ӫ
Declare_Msg_Member(WORD, m_wRewardIdx)	// ��������
Declare_Msg_End


// ��Ӫ��ļ������������Ӧ
Declare_Msg_Begin(S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP, MSG_S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_UnsignedShort(m_wRewardIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_wRewardIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// ��Ӫ
Declare_Msg_Member(WORD, m_wRewardIdx)	// ��������
Declare_Msg_End


// ��Ӫ��ļ��¼����
Declare_Msg_Begin(C2S_CAMP_RECRUIT_RECORD_REQ, MSG_C2S_CAMP_RECRUIT_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// ��Ӫ
Declare_Msg_End


// ��Ӫ��ļ��¼��Ӧ
Declare_Msg_Begin(S2C_CAMP_RECRUIT_RECORD_RESP, MSG_S2C_CAMP_RECRUIT_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_vRecruitRecordList.size())
for (int i = 0; i < m_vRecruitRecordList.size(); ++i)
{
	CampRecruitRecord &tRecord = m_vRecruitRecordList[i];
	Declare_EncodeMsg_String(tRecord.szRoleName, sizeof(tRecord.szRoleName));
	Declare_EncodeMsg_UnsignedInt(tRecord.tRewardData.dwId);
	Declare_EncodeMsg_UnsignedInt(tRecord.tRewardData.dwNum);
	//Declare_EncodeMsg_UnsignedInt(tRecord.dwRecruitTime);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// ��Ӫ
Declare_Msg_Member(vector<CampRecruitRecord>, m_vRecruitRecordList)	// ��ļ��¼�б�
Declare_Msg_End


// unlock���������
Declare_Msg_Begin(C2S_UNLOCK_ACTIVITY_DATA_REQ, MSG_C2S_UNLOCK_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// unlock�������Ӧ
Declare_Msg_Begin(S2C_UNLOCK_ACTIVITY_DATA_RESP, MSG_S2C_UNLOCK_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_ActivityData.wIsScoreForShop)
Declare_DecodeMsg_UnsignedShort(m_ActivityData.wWatchingVideosTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ActivityData.wIsScoreForShop)
Declare_EncodeMsg_UnsignedShort(m_ActivityData.wWatchingVideosTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(UnlockActivityData, m_ActivityData)	// �����
Declare_Msg_End

// �̳��������֪ͨ����
Declare_Msg_Begin(C2S_SHOP_SCORE_FINISH_REQ, MSG_C2S_SHOP_SCORE_FINISH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �̳��������֪ͨ��Ӧ
Declare_Msg_Begin(S2C_SHOP_SCORE_FINISH_RESP, MSG_S2C_SHOP_SCORE_FINISH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// �ۿ���Ƶ֪ͨ����
Declare_Msg_Begin(C2S_WATCHING_VIDEO_NOTIFY_REQ, MSG_C2S_WATCHING_VIDEO_NOTIFY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ۿ���Ƶ֪ͨ��Ӧ
Declare_Msg_Begin(S2C_WATCHING_VIDEO_NOTIFY_RESP, MSG_S2C_WATCHING_VIDEO_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������������
Declare_Msg_Begin(C2S_NEW_ROLE_GUIDE_UPDATE_REQ, MSG_C2S_NEW_ROLE_GUIDE_UPDATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nKey)
Declare_DecodeMsg_UnsignedInt(m_nValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nKey)
Declare_EncodeMsg_UnsignedInt(m_nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nKey)		// ����Key
Declare_Msg_Member(int, m_nValue)	// ����Value
Declare_Msg_End

// ����ȼ�������������
Declare_Msg_Begin(C2S_WORLD_LEVEL_TASK_DATA_REQ, MSG_C2S_WORLD_LEVEL_TASK_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ȼ�����������Ӧ
Declare_Msg_Begin(S2C_WORLD_LEVEL_TASK_DATA_RESP, MSG_S2C_WORLD_LEVEL_TASK_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_Data.wIsAlreadyTakeFinalReward)
Declare_EncodeMsg_UnsignedShort(m_Data.wTaskEventNum)
for (WORD i = 0; i < m_Data.wTaskEventNum; ++i)
{
	WorldLevelTaskEvent &tTaskEvent = m_Data.arTaskEventList[i];
	Declare_EncodeMsg_UnsignedShort(tTaskEvent.wTaskId);
	Declare_EncodeMsg_UnsignedInt(tTaskEvent.nContext);
	Declare_EncodeMsg_UnsignedChar(tTaskEvent.wIsAlreadyTake);
	Declare_EncodeMsg_UnsignedShort(tTaskEvent.wTaskLevel);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerWorldLevelTaskData, m_Data)	// ����Value
Declare_Msg_End

// ��ȡ����ȼ�����������
Declare_Msg_Begin(C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ, MSG_C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTaskId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskId)	// ����Id
Declare_Msg_End

// ��ȡ����ȼ���������Ӧ
Declare_Msg_Begin(S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP, MSG_S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTaskId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskId)	// ����Id
Declare_Msg_End

// ��ȡ����ȼ��������ս�������
Declare_Msg_Begin(C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ, MSG_C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��ȡ����ȼ��������ս�����Ӧ
Declare_Msg_Begin(S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP, MSG_S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���������Դս����
Declare_Msg_Begin(C2S_PLAYER_RESOURCE_WAR_DATA_REQ, MSG_C2S_PLAYER_RESOURCE_WAR_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �����Դս������Ӧ
Declare_Msg_Begin(S2C_PLAYER_RESOURCE_WAR_DATA_RESP, MSG_S2C_PLAYER_RESOURCE_WAR_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_RoleResourceData.wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_RoleResourceData.wChapter)
Declare_EncodeMsg_UnsignedShort(m_RoleResourceData.wResourceId)
Declare_EncodeMsg_UnsignedInt(m_RoleResourceData.dwOccupyStartTime)
Declare_EncodeMsg_UnsignedInt(m_RoleResourceData.dwOccupyOverTime)
Declare_EncodeMsg_UnsignedShort(m_RoleResourceData.wKeepOccupyTimes)
// Declare_EncodeMsg_UnsignedInt(m_RoleResourceData.dwLastBaseRewardTime)
// Declare_EncodeMsg_UnsignedInt(m_RoleResourceData.dwLastExtraRewardTime)
// Declare_EncodeMsg_UnsignedShort(m_RoleResourceData.wCurExtraRewardNeedTime)
Declare_EncodeMsg_UnsignedInt(m_dwTotalAccupyTime)
Declare_EncodeMsg_UnsignedShort(m_wTechnologyLevel)
Declare_EncodeMsg_UnsignedShort(m_wGuildAccupyNum)
Declare_EncodeMsg_String(m_szGuildName, MAX_ROLE_LEN)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ResourceWarResouceData, m_RoleResourceData)	// ��ɫ��Դ����
Declare_Msg_Member(DWORD, m_dwTotalAccupyTime)			// �ܵ�ռ��ʱ��
Declare_Msg_Member(WORD, m_wTechnologyLevel)			// �Ƽ��ȼ�
Declare_Msg_Member(WORD, m_wGuildAccupyNum)				// ����ռ������(����Լ�ռ�����Դ���ڵ��½�)
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN) // ��������
Declare_Msg_End

// ������Դս�����б�
Declare_Msg_Begin(C2S_RESOURCE_WAR_ENEMY_LIST_REQ, MSG_C2S_RESOURCE_WAR_ENEMY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��������б���Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_ENEMY_LIST_RESP, MSG_S2C_RESOURCE_WAR_ENEMY_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vEnemyList.size())
for (int i = 0; i < m_vEnemyList.size(); ++i)
{
	ResourceWarEnemyClient &tEnemy = m_vEnemyList[i];
	Declare_EncodeMsg_UnsignedInt(tEnemy.tCommonData.dwRoleId);
	Declare_EncodeMsg_String(tEnemy.tCommonData.szRoleName, sizeof(tEnemy.tCommonData.szRoleName));
	Declare_EncodeMsg_UnsignedShort(tEnemy.tCommonData.wLevel);
	Declare_EncodeMsg_UnsignedInt(tEnemy.tCommonData.dwFightingPower);
	Declare_EncodeMsg_UnsignedShort(tEnemy.tCommonData.wHeadIcon);
	Declare_EncodeMsg_String(tEnemy.tCommonData.szGuildName, sizeof(tEnemy.tCommonData.szGuildName));
	Declare_EncodeMsg_UnsignedShort(tEnemy.wAttackTimes);
	Declare_EncodeMsg_UnsignedShort(tEnemy.wChapter);
	Declare_EncodeMsg_UnsignedShort(tEnemy.wResourceId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<ResourceWarEnemyClient>, m_vEnemyList)	// �����б�
Declare_Msg_End


// ����ս��
Declare_Msg_Begin(C2S_RESOURCE_WAR_BATTLE_RECORD_REQ, MSG_C2S_RESOURCE_WAR_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ս����Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_BATTLE_RECORD_RESP, MSG_S2C_RESOURCE_WAR_BATTLE_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vBattleRecordList.size())
for (int i = 0; i < m_vBattleRecordList.size(); ++i)
{
	ResourceWardRecordClient &tBattleRecord = m_vBattleRecordList[i];
	Declare_EncodeMsg_String(tBattleRecord.szAttackerName, sizeof(tBattleRecord.szAttackerName));
	Declare_EncodeMsg_String(tBattleRecord.szDefenderName, sizeof(tBattleRecord.szDefenderName));
	Declare_EncodeMsg_UnsignedInt(tBattleRecord.dwBattleTime);
	Declare_EncodeMsg_UnsignedShort(tBattleRecord.wChapter);
	Declare_EncodeMsg_UnsignedShort(tBattleRecord.wResourceId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<ResourceWardRecordClient>, m_vBattleRecordList)	// ս���б�
Declare_Msg_End


// ����ռ�����Դ
Declare_Msg_Begin(C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ, MSG_C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����ռ�����Դ��Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP, MSG_S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ӳ�ռ�����Դ
Declare_Msg_Begin(C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ, MSG_C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �ӳ�ռ�����Դ��Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP, MSG_S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ������Դս�½�����
Declare_Msg_Begin(C2S_RESOURCE_WAR_CHAPTER_DATA_REQ, MSG_C2S_RESOURCE_WAR_CHAPTER_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// �½�
Declare_Msg_End

// ������Դս�½�������Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_CHAPTER_DATA_RESP, MSG_S2C_RESOURCE_WAR_CHAPTER_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_UnsignedShort(m_vChapterResourceList.size())
for (int i = 0; i < m_vChapterResourceList.size(); ++i)
{
	ResourceWarDataClient &tResourceData = m_vChapterResourceList[i];
	Declare_EncodeMsg_UnsignedInt(tResourceData.tOwnerInfo.dwRoleId);
	Declare_EncodeMsg_String(tResourceData.tOwnerInfo.szRoleName, sizeof(tResourceData.tOwnerInfo.szRoleName));
	Declare_EncodeMsg_UnsignedShort(tResourceData.tOwnerInfo.wLevel);
	Declare_EncodeMsg_UnsignedInt(tResourceData.tOwnerInfo.dwFightingPower);
	Declare_EncodeMsg_UnsignedShort(tResourceData.tOwnerInfo.wHeadIcon);
	Declare_EncodeMsg_String(tResourceData.tOwnerInfo.szGuildName, sizeof(tResourceData.tOwnerInfo.szGuildName));
	Declare_EncodeMsg_UnsignedShort(tResourceData.tOwnerInfo.wServerId);
	Declare_EncodeMsg_UnsignedShort(tResourceData.wResourceId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// �½�
Declare_Msg_Member(vector<ResourceWarDataClient>, m_vChapterResourceList)	// �½���Դ�б�
Declare_Msg_End


// ������Դսͬ����Դ
Declare_Msg_Begin(C2S_RESOURCE_WAR_SAME_RESOURCE_REQ, MSG_C2S_RESOURCE_WAR_SAME_RESOURCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wResourceType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wResourceType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wResourceType)	// ��Դ����
Declare_Msg_End

// ������Դսͬ����Դ��Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_SAME_RESOURCE_RESP, MSG_S2C_RESOURCE_WAR_SAME_RESOURCE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wResourceType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wResourceType)
Declare_EncodeMsg_UnsignedShort(m_vResourceDataList.size())
for (int i = 0; i < m_vResourceDataList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_vResourceDataList[i].wChapter);
	Declare_EncodeMsg_UnsignedShort(m_vResourceDataList[i].wResourceId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wResourceType)	// ��Դ����
Declare_Msg_Member(vector<ResourceWarBaseData>, m_vResourceDataList)	// ��Դ�����б�
Declare_Msg_End

// ������Դս������Դ
Declare_Msg_Begin(C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ, MSG_C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ������Դս������Դ��Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP, MSG_S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vResourceDataList.size())
for (int i = 0; i < m_vResourceDataList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedShort(m_vResourceDataList[i].wChapter);
	Declare_EncodeMsg_UnsignedShort(m_vResourceDataList[i].wResourceId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<ResourceWarBaseData>, m_vResourceDataList)	// ��Դ�����б�
Declare_Msg_End


// ��Դս�Ƽ���������
Declare_Msg_Begin(C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ, MSG_C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ��Դս�Ƽ�������Ӧ
Declare_Msg_Begin(S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP, MSG_S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTechnologyLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTechnologyLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTechnologyLevel)	// �Ƽ��ȼ�
Declare_Msg_End


// ���󹤻�ս��ɫ����
Declare_Msg_Begin(C2S_GUILD_WAR_ROLE_DATA_REQ, MSG_C2S_GUILD_WAR_ROLE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���󹤻�ս��ɫ����
Declare_Msg_Begin(S2C_GUILD_WAR_ROLE_DATA_RESP, MSG_S2C_GUILD_WAR_ROLE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_GuildWarRoleData.wBuyHighStarBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wBuyHighStarBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wUsedLowStarBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wUsedHighStarBattleTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleGuildWarData, m_GuildWarRoleData)	// ����ս��ɫ����
Declare_Msg_End


// ���󱾾�����ռ�������
Declare_Msg_Begin(C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���󱾾�����ռ���������Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP, MSG_S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vOccupyStarList.size())
for (int i = 0; i < m_vOccupyStarList.size(); ++i)
{
	GuildWarOccupyStarDataForClient &tData = m_vOccupyStarList[i];
	Declare_EncodeMsg_UnsignedShort(tData.wStarId);
	Declare_EncodeMsg_UnsignedChar(tData.byIsHaveReward);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<GuildWarOccupyStarDataForClient>, m_vOccupyStarList)	// ռ��������б�
Declare_Msg_End

// ���󱾾�������ս�������б�
Declare_Msg_Begin(C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ���󱾾�������ս�������б���Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP, MSG_S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vDeclareWarStarList.size())
for (int i = 0; i < m_vDeclareWarStarList.size(); ++i)
{
	GuildWarDeclareWarData &tData = m_vDeclareWarStarList[i];
	Declare_EncodeMsg_UnsignedShort(tData.wStarId);
	Declare_EncodeMsg_String(tData.szGuildName, sizeof(tData.szGuildName));
	Declare_EncodeMsg_UnsignedInt(tData.dwActive);
	Declare_EncodeMsg_UnsignedShort(tData.wServerId);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<GuildWarDeclareWarData>, m_vDeclareWarStarList)	// ��ս�����б�
Declare_Msg_End


// ���������б�
Declare_Msg_Begin(C2S_GUILD_WAR_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// ���������б���Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_STAR_LIST_RESP, MSG_S2C_GUILD_WAR_STAR_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vOccupyStarList.size())
for (int i = 0; i < m_vOccupyStarList.size(); ++i)
{
	GuildWarStarBaseDataForClient &tData = m_vOccupyStarList[i];
	Declare_EncodeMsg_UnsignedShort(tData.wStarId);
	Declare_EncodeMsg_String(tData.szGuildName, sizeof(tData.szGuildName));
	Declare_EncodeMsg_UnsignedShort(tData.wIconId);
	Declare_EncodeMsg_UnsignedChar(tData.byIsInBattle);
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<GuildWarStarBaseDataForClient>, m_vOccupyStarList)	// ռ��������б�
Declare_Msg_End


// ������ս
Declare_Msg_Begin(C2S_GUILD_WAR_DECLARE_WAR_REQ, MSG_C2S_GUILD_WAR_DECLARE_WAR_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ��ս������ID
Declare_Msg_End

// ��ս��Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_DECLARE_WAR_RESP, MSG_S2C_GUILD_WAR_DECLARE_WAR_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ��ս������ID
Declare_Msg_End

// ���󲼷�
Declare_Msg_Begin(C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ, MSG_C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_UnsignedShort(m_wTowerType)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byIsToBattle)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedShort(m_wTowerType)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byIsToBattle)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// Ҫ��ս������ID
Declare_Msg_Member(WORD, m_wTowerType)	
Declare_Msg_Member(DWORD, m_dwRoleId)	
Declare_Msg_Member(BYTE, m_byIsToBattle)	
Declare_Msg_End


// ������Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP, MSG_S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_UnsignedShort(m_wTowerType)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byIsToBattle)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedShort(m_wTowerType)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byIsToBattle)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// Ҫ��ս������ID
Declare_Msg_Member(WORD, m_wTowerType)
Declare_Msg_Member(DWORD, m_dwRoleId)
Declare_Msg_Member(BYTE, m_byIsToBattle)
Declare_Msg_End


// ������
Declare_Msg_Begin(C2S_GUILD_WAR_EVACUATE_REQ, MSG_C2S_GUILD_WAR_EVACUATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ��ս������ID
Declare_Msg_End

// ������Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_EVACUATE_RESP, MSG_S2C_GUILD_WAR_EVACUATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ��ս������ID
Declare_Msg_End

// ������Ը߼�������ս����
Declare_Msg_Begin(C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ, MSG_C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// ����Ը߼�������ս������Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP, MSG_S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurBuyTimes)	// ��ǰ�ѹ���Ĵ���
Declare_Msg_End

// ������ȡ������
Declare_Msg_Begin(C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ, MSG_C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTakeType)
Declare_DecodeMsg_UnsignedShort(m_wStarNum)
m_wStarNum = min(GUILD_WAR_OCCUPY_STAR_MAX_NUM, m_wStarNum);
Declare_DecodeMsg_Memory(m_arStarList, m_wStarNum * sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTakeType)
m_wStarNum = min(GUILD_WAR_OCCUPY_STAR_MAX_NUM, m_wStarNum);
Declare_EncodeMsg_UnsignedShort(m_wStarNum)
Declare_EncodeMsg_Memory(m_arStarList, m_wStarNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTakeType)	// ��ȡ����
Declare_Msg_Member(WORD, m_wStarNum)	// ��������
Declare_Msg_Member_Array(WORD, m_arStarList, GUILD_WAR_OCCUPY_STAR_MAX_NUM);	// �����б�
Declare_Msg_End

// ��ȡ��������Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP, MSG_S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTakeType)
Declare_DecodeMsg_UnsignedShort(m_wStarNum)
m_wStarNum = min(GUILD_WAR_OCCUPY_STAR_MAX_NUM, m_wStarNum);
Declare_DecodeMsg_Memory(m_arStarList, m_wStarNum * sizeof(WORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTakeType)
m_wStarNum = min(GUILD_WAR_OCCUPY_STAR_MAX_NUM, m_wStarNum);
Declare_EncodeMsg_UnsignedShort(m_wStarNum)
Declare_EncodeMsg_Memory(m_arStarList, m_wStarNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTakeType)	// ��ȡ����
Declare_Msg_Member(WORD, m_wStarNum)	// ��������
Declare_Msg_Member_Array(WORD, m_arStarList, GUILD_WAR_OCCUPY_STAR_MAX_NUM);	// �����б�
Declare_Msg_End

// ����������ϸ
Declare_Msg_Begin(C2S_GUILD_WAR_STAR_DETAIL_REQ, MSG_C2S_GUILD_WAR_STAR_DETAIL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ���������ID
Declare_Msg_End

// ������ϸ��Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_STAR_DETAIL_RESP, MSG_S2C_GUILD_WAR_STAR_DETAIL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedInt(m_tBelongGuildData.dwGuildId)
Declare_EncodeMsg_String(m_tBelongGuildData.szGuildName, sizeof(m_tBelongGuildData.szGuildName));
Declare_EncodeMsg_UnsignedInt(m_tBelongGuildData.dwActive)
Declare_EncodeMsg_UnsignedShort(m_tBelongGuildData.wServerId)
Declare_EncodeMsg_UnsignedShort(m_tBelongGuildData.wIconId)

Declare_EncodeMsg_UnsignedInt(m_tAttackGuildData.dwGuildId)
Declare_EncodeMsg_String(m_tAttackGuildData.szGuildName, sizeof(m_tAttackGuildData.szGuildName));
Declare_EncodeMsg_UnsignedInt(m_tAttackGuildData.dwActive)
Declare_EncodeMsg_UnsignedShort(m_tAttackGuildData.wServerId)
Declare_EncodeMsg_UnsignedShort(m_tAttackGuildData.wIconId)

Declare_EncodeMsg_UnsignedShort(m_vDeclareWarGuildList.size())
for (int i = 0; i < m_vDeclareWarGuildList.size(); ++i)
{
	GuildWarStarBaseData &tDeclareWarGuild = m_vDeclareWarGuildList[i];
	Declare_EncodeMsg_String(tDeclareWarGuild.szGuildName, sizeof(tDeclareWarGuild.szGuildName));
	Declare_EncodeMsg_UnsignedInt(tDeclareWarGuild.dwContext)
	Declare_EncodeMsg_UnsignedShort(tDeclareWarGuild.wServerId)
	Declare_EncodeMsg_UnsignedShort(tDeclareWarGuild.wIconId)
}
Declare_EncodeMsg_UnsignedChar(m_byIsBattleOver)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// Ҫ���������ID
Declare_Msg_Member(BYTE, m_byIsBattleOver)	// �Ƿ�ս������
Declare_Msg_Member(StarBelongGuildData, m_tBelongGuildData)	// ��õ�����Ĺ�������
Declare_Msg_Member(StarAttackGuildData, m_tAttackGuildData)	// ��������Ĺ�������
Declare_Msg_Member(vector<GuildWarStarBaseData>, m_vDeclareWarGuildList)	// �Ա�������ս�Ĺ����б�
Declare_Msg_End


// ���󹤻�ս���б�
Declare_Msg_Begin(C2S_GUILD_WAR_TOWER_LIST_REQ, MSG_C2S_GUILD_WAR_TOWER_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// Ҫ���������ID
Declare_Msg_End

// ����ս���б���Ӧ
Declare_Msg_Begin(S2C_GUILD_WAR_TOWER_LIST_RESP, MSG_S2C_GUILD_WAR_TOWER_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedShort(m_vTowerList.size())
vector<GuildWarTowerForClient>::iterator tower = m_vTowerList.begin();
for (; tower != m_vTowerList.end(); ++tower)
{
	Declare_EncodeMsg_UnsignedShort(tower->wTowerId);
	Declare_EncodeMsg_UnsignedInt(tower->dwDurability);
	//Declare_EncodeMsg_UnsignedShort(tower->wType);
	Declare_EncodeMsg_UnsignedShort(tower->wMonsterBeAttackTimes);
	Declare_EncodeMsg_UnsignedShort(tower->vDefenderList.size());
	vector<StarDefenderForClient>::iterator defender = tower->vDefenderList.begin();
	for (; defender != tower->vDefenderList.end(); ++defender)
	{
		Declare_EncodeMsg_UnsignedInt(defender->dwRoleId);
		Declare_EncodeMsg_UnsignedShort(defender->wBeAttackTimes);
		Declare_EncodeMsg_UnsignedShort(defender->wHeadIcon);
		Declare_EncodeMsg_UnsignedInt(defender->dwFightingPower);
		Declare_EncodeMsg_String(defender->szRoleName, sizeof(defender->szRoleName));
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)	// ������ID
Declare_Msg_Member(WORD, m_wStarId)		// Ҫ���������ID
Declare_Msg_Member(vector<GuildWarTowerForClient>, m_vTowerList)	// ���б�
Declare_Msg_End


// �������ݸ���֪ͨ
Declare_Msg_Begin(S2C_GUILD_WAR_STAR_UPDATE_NOTIFY, MSG_S2C_GUILD_WAR_STAR_UPDATE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_UnsignedShort(m_wTowerId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedShort(m_wTowerId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// ����ID
Declare_Msg_Member(WORD, m_wTowerId)	// ��ID
Declare_Msg_End


// �һ�������������
Declare_Msg_Begin(C2S_HANG_UP_DROP_DATA_REQ, MSG_C2S_HANG_UP_DROP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �һ�����������Ӧ
Declare_Msg_Begin(S2C_HANG_UP_DROP_DATA_RESP, MSG_S2C_HANG_UP_DROP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_HangUpDropData.dwLastObtainRewardTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_HangUpDropData.dwLastObtainRewardTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerHangUpDropData, m_HangUpDropData)		// �һ���������
Declare_Msg_End


// ��ȡ�һ����佱������
Declare_Msg_Begin(C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ, MSG_C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// �һ�����������Ӧ
Declare_Msg_Begin(S2C_HANG_UP_DROP_REWARD_OBTAIN_RESP, MSG_S2C_HANG_UP_DROP_REWARD_OBTAIN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_vRewardItemList.size())
for (int i = 0; i < m_vRewardItemList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedInt(m_vRewardItemList[i].dwId)
	Declare_EncodeMsg_UnsignedInt(m_vRewardItemList[i].dwNum)
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(vector<Item>, m_vRewardItemList)		// ������Ʒ�б�
Declare_Msg_End