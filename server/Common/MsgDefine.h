#pragma once

#include "Const.h"
#include "ErrorCode.h"
#include "DataDefine.h"
#include "MessageIdDefine.h"
#include "PacketStruct_Code.h"
#include "DBCDefine.h"
#include <wtypes.h>

/*--------------------------------------------------------------------------------------------------------------------------*/	
// 1) 客户端和GateSvr之间消息定义
/*--------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------------*/	
// 2) 客户端和MapSvr之间消息定义
/*--------------------------------------------------------------------------------------------------------------------------*/


// 游戏错误提示
Declare_Msg_Begin(S2C_GAME_ERROR_NOTIFY, MSG_S2C_GAME_ERROR_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wErrorCode)
Declare_DecodeMsg_UnsignedShort(m_wFromMsgId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wErrorCode)
Declare_EncodeMsg_UnsignedShort(m_wFromMsgId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wErrorCode)		// 错误码
Declare_Msg_Member(WORD, m_wFromMsgId)		// 引起错误的消息ID
Declare_Msg_End

// 帐号登录
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
Declare_Msg_Member(BYTE, m_byTerminalType)						// 终端类型(1=android 2=ios 3=电脑)
Declare_Msg_Member(DWORD, m_dwCPChannelID)	// CP渠道ID
Declare_Msg_Member(DWORD, m_dwSDKChannelID)	// SDK渠道ID
Declare_Msg_Member(BYTE, m_byLoginType)		// 登录类型(0=测试,1=用户名注册,2=用户名登录,3=手机号登录,4=手机号注册)
Declare_Msg_Member(WORD, m_wCode)	// 认证码
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// 帐号
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN)	// 密码
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// 设备ID deviceid
Declare_Msg_End


// 帐号登录响应
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
//Declare_Msg_Member(DWORD, m_dwVersion)	// 版本
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// 帐号
Declare_Msg_Member(BYTE, m_byIsBindMobile)	// 是否已绑定手机号
Declare_Msg_Member(WORD, m_wLastLoginServerNum)
Declare_Msg_Member_Array(LastLoginServerData, m_LastLoginServerList, MAX_LOGIN_SERVER_LIST)	// 最近登录服务器列表
Declare_Msg_End


// 帐号注册请求
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
//Declare_Msg_Member_Array(char, m_szCode, 32) // 认证码
Declare_Msg_Member(BYTE, m_byType)	// 类型(1用户名注册,2=手机号注册)
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN ) // 帐号
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // 密码
Declare_Msg_End


// 帐号注册响应
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
//Declare_Msg_Member_Array(char, m_szCode, 32) // 认证码
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // 帐号
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // 密码
Declare_Msg_End


// 帐号密码修改请求
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
//Declare_Msg_Member_Array(char, m_szCode, 32) // 认证码
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // 帐号
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // 密码
Declare_Msg_End


// 帐号密码修改响应
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
//Declare_Msg_Member_Array(char, m_szCode, 32) // 认证码
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // 帐号
Declare_Msg_Member_Array(char, m_szPassword, MAX_ACCOUNT_PASSWORD_LEN) // 密码
Declare_Msg_End


// 领取活动礼包
Declare_Msg_Begin(C2S_TAKE_ACTIVITY_GIFT_REQ, MSG_C2S_TAKE_ACTIVITY_GIFT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szCode, sizeof(m_szCode)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szCode, sizeof(m_szCode))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szCode, MAX_ACTIVATE_CODE_LEN)	// 激活码
Declare_Msg_End

// 领取活动礼包响应
Declare_Msg_Begin(S2C_TAKE_ACTIVITY_GIFT_RESP, MSG_S2C_TAKE_ACTIVITY_GIFT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGiftId)		// 获得的礼包ID
Declare_Msg_End

// 请求活动公告
Declare_Msg_Begin(C2S_ACTIVITY_NOTICE_REQ, MSG_C2S_ACTIVITY_NOTICE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求活动公告响应
Declare_Msg_Begin(S2C_ACTIVITY_NOTICE_RESP, MSG_S2C_ACTIVITY_NOTICE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNoticeId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNoticeId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNoticeId)		// 公告ID
Declare_Msg_End


// 认证码请求
Declare_Msg_Begin(C2S_AUTHENTICATION_CODE_REQ, MSG_C2S_AUTHENTICATION_CODE_REQ)
Declare_DecodeMsg_Function_Begin
//Declare_DecodeMsg_UnsignedChar(m_byType);
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedChar(m_byType);
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_Function_End
//Declare_Msg_Member(BYTE, m_byType)		// 1=注册,2=修改密码
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// 手机号
Declare_Msg_End

// 认证码响应
Declare_Msg_Begin(S2C_AUTHENTICATION_CODE_RESP, MSG_S2C_AUTHENTICATION_CODE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 手机号绑定请求
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
Declare_Msg_Member(WORD, m_wCode)		// 认证码
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN) // 帐号
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// 手机号
Declare_Msg_End

// 手机号绑定响应
Declare_Msg_Begin(S2C_MOBILE_BIND_RESP, MSG_S2C_MOBILE_BIND_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


 // SDK登录请求
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
//Declare_Msg_Member(DWORD, m_dwVersion)							// 版本
Declare_Msg_Member(BYTE, m_byTerminalType)						// 终端类型(1=android 2=ios 3=电脑)
Declare_Msg_Member(DWORD, m_dwCPChannelType)					// CP渠道类型
Declare_Msg_Member(DWORD, m_dwSDKChannelType)					// SDK渠道类型
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// 帐号
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)							// 上下文
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)					// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// 设备ID deviceid
Declare_Msg_End


// SDK帐号登录响应
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
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)			// 帐号
Declare_Msg_Member(BYTE, m_byIsBindMobile)	// 是否已绑定手机号
Declare_Msg_Member(WORD, m_wLastLoginServerNum)
Declare_Msg_Member_Array(LastLoginServerData, m_LastLoginServerList, MAX_LOGIN_SERVER_LIST)	// 最近登录服务器列表
Declare_Msg_End

// 检测用户信息请求
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
Declare_Msg_Member(WORD, m_wLoginType)	// 登录类型(1=quick_sdk, 2=云顶ios, 3=翔通)
Declare_Msg_Member(BYTE, m_byTerminalType)	// 终端类型(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwCPChannelType)	// CP渠道类型
Declare_Msg_Member(DWORD, m_dwSDKChannelType)	// SDK渠道类型
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// 帐号
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// 设备ID deviceid
Declare_Msg_End

// SDK登录响应
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
Declare_Msg_Member(int, m_nResult)	// 结果
Declare_Msg_Member(WORD, m_wLoginType)	// 登录类型(1=quick_sdk, 2=云顶ios, 3=翔通)
Declare_Msg_Member(BYTE, m_byTerminalType)	// 终端类型(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwCPChannelType)	// CP渠道类型
Declare_Msg_Member(DWORD, m_dwSDKChannelType)	// SDK渠道类型
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// 帐号
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member_Array(char, m_szIP, IP_LEN)			// IP
Declare_Msg_Member_Array(char, m_szDeviceId, MAX_DEVICE_ID_LEN)	// 设备ID deviceid
Declare_Msg_End


// SDK订单号请求
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
Declare_Msg_Member(BYTE, m_byRechargeType)	// 充值类型(1=普通充值,2=月卡)
Declare_Msg_Member(WORD, m_wRechargeId)		// 充值id(月卡ID,充值Id)
Declare_Msg_Member(DWORD, m_dwContext)		// 上下文
Declare_Msg_End


// SDK订单号响应
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
Declare_Msg_Member_Array(char, m_szOrderId, MAX_ORDER_ID_LEN)		// 订单号
Declare_Msg_Member_Array(char, m_szSign, MAX_ORDER_SIGN_LEN)		// 签名
Declare_Msg_Member(BYTE, m_byRechargeType)	// 充值类型(1=普通充值,2=月卡)
Declare_Msg_Member(WORD, m_wRechargeId)		// 充值id(月卡ID,充值Id)
Declare_Msg_Member(DWORD, m_dwContext)		// 上下文
Declare_Msg_End


// SDK订单号请求
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
Declare_Msg_Member(int, m_nAmount)							// 交易金额	单位：分，币种：人民币，必须是整数
Declare_Msg_Member_Array(char, m_szTitle, BUFF_LEN_32)		// 商品的标题	例：羽毛球拍  最长45位
Declare_Msg_Member_Array(char, m_szDesc, BUFF_LEN_128)		// 订单描述	例：正品纳米科技台湾产 最长100位
Declare_Msg_Member_Array(char, m_szExtInfo, BUFF_LEN_64)	// CP透传参数	CP扩展参数，异步通知时会透传给CP服务器，最大64位, 不能为空
Declare_Msg_End


// SDK订单号响应
Declare_Msg_Begin(O2M_SDK_ORDER_ID_RESP, MSG_O2M_SDK_ORDER_ID_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szAccessKey, sizeof(m_szAccessKey)-1)
Declare_DecodeMsg_String(m_szOrderNumber, sizeof(m_szOrderNumber)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szAccessKey, sizeof(m_szAccessKey))
Declare_EncodeMsg_String(m_szOrderNumber, sizeof(m_szOrderNumber))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szAccessKey, BUFF_LEN_64)		// vivoSDK需要的参数 vivoSDK使用
Declare_Msg_Member_Array(char, m_szOrderNumber, BUFF_LEN_64)	// 交易流水号 vivo订单号
Declare_Msg_End


// CP订单号请求
Declare_Msg_Begin(O2M_CP_ORDER_ID_REQ, MSG_O2M_CP_ORDER_ID_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// 订单号的角色Id
Declare_Msg_End


// CP订单号响应
Declare_Msg_Begin(M2O_CP_ORDER_ID_RESP, MSG_M2O_CP_ORDER_ID_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedInt(m_dwOrderId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedInt(m_dwOrderId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// 订单号的角色Id
Declare_Msg_Member(DWORD, m_dwOrderId)	// 订单号ID
Declare_Msg_End



// 登录请求
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
//Declare_Msg_Member(WORD, m_wLoginType)		// 登录类型(1=quick_sdk, 2=云顶ios, 3=翔通)
Declare_Msg_Member(BYTE, m_byPlatformType)	// 平台类型(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwChannelType)	// 渠道类型
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// 帐号
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)	// 上下文
Declare_Msg_End

// 登录响应
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
//Declare_Msg_Member(WORD, m_wLoginType)	// 登录类型
Declare_Msg_Member(BYTE, m_byPlatformType)	// 平台类型(1=android 2=ios)
Declare_Msg_Member(DWORD, m_dwChannelType)	// 渠道类型
Declare_Msg_Member_Array(char, m_szAccount, MAX_ACCOUNT_LEN)	// 帐号
Declare_Msg_Member_Array(char, m_szToken, MAX_TOKEN_LEN)		// token
Declare_Msg_Member(DWORD, m_dwContext)	// 上下文
Declare_Msg_End



// 在线玩家数量请求
Declare_Msg_Begin(O2M_ONLINE_PLAYER_NUM_REQ, MSG_O2M_ONLINE_PLAYER_NUM_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 在线玩家数量响应
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
Declare_Msg_Member(WORD, m_wServerNum)	// 服务器数量
Declare_Msg_Member_Array(WORD, m_ServerIdList, MAX_SERVER_NUM)	// 服务器ID列表
Declare_Msg_Member_Array(WORD, m_PlayerNumList, MAX_SERVER_NUM)	// 玩家数量列表
Declare_Msg_End


// 充值成功上报请求
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
Declare_Msg_Member_Array(char, m_szIP, BUFF_LEN_32)	// ip  是  string  玩家充值时的 IP 地址
Declare_Msg_Member(WORD, m_wRechargeId)	// iap_id  是  string  商品编码
Declare_Msg_Member_Array(char, m_szOrderId, BUFF_LEN_64)// order_id  是  string  订单号（支付平台提供的订单号）
Declare_Msg_Member(DWORD, m_dwChannel)	// channel  是  int  渠道代码(要求客户端用YundingKeyData.getChannel()获取最终的渠道并传给服务端)
Declare_Msg_End


// 充值成功上报响应
Declare_Msg_Begin(S2C_RECHARGE_SUCCESS_REPORT_RESP, MSG_S2C_RECHARGE_SUCCESS_REPORT_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 充值成功上报(单向)
Declare_Msg_Begin(M2O_RECHARGE_SUCCESS_REPORT_NOTIFY, MSG_M2O_RECHARGE_SUCCESS_REPORT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RechargeSuccessReportData, m_ReportData)
Declare_Msg_End

// mycar充值成功上报
Declare_Msg_Begin(M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY, MSG_M2O_MYCARD_RECHARGE_SUCCESS_REPORT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_ReportData, sizeof(m_ReportData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(MyCardRechargeSuccessReportData, m_ReportData)
Declare_Msg_End



// 数据统计通知
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
Declare_Msg_Member_Array(char, m_szContext, BUFF_LEN_512)		// 通知内容
Declare_Msg_End


// 充值发货请求
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
Declare_Msg_Member(RechargeCallbackData, m_CallbackData)		// 充值回调数据
Declare_Msg_End


// 充值发货响应
Declare_Msg_Begin(M2O_RECHARGE_SEND_GOODS_RESP, MSG_M2O_RECHARGE_SEND_GOODS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szOrderId, sizeof(m_szOrderId)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szOrderId, sizeof(m_szOrderId))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szOrderId, MAX_ORDER_ID_LEN)		// 订单号
Declare_Msg_End



// YGame充值通知请求
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
Declare_Msg_Member(WORD, m_wServerId)		// 服务器ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// 角色Id
Declare_Msg_Member(int, m_nAmount)			// 额度
Declare_Msg_Member(int, m_nDiamond)			// 钻石
Declare_Msg_Member_Array(char, m_szPaymentId, MAX_YGAME_PAYMENT_ID_LEN)	// 支付ID
Declare_Msg_End

// YGame充值通知响应
Declare_Msg_Begin(M2O_YGAME_RECHARGE_SEND_GOODS_RESP, MSG_M2O_YGAME_RECHARGE_SEND_GOODS_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// YGame提现通知请求
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
Declare_Msg_Member(WORD, m_wServerId)		// 服务器ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// 角色Id
Declare_Msg_Member(int, m_nYcoin)			// 额度
Declare_Msg_Member(int, m_nDiamond)			// 钻石
Declare_Msg_End

// YGame提现通知响应
Declare_Msg_Begin(M2O_YGAME_CASHBACK_NOTIFY_RESP, MSG_M2O_YGAME_CASHBACK_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// YGame奖励通知请求
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
Declare_Msg_Member(WORD, m_wServerId)		// 服务器ID
Declare_Msg_Member(DWORD, m_dwRoleId)		// 角色Id
Declare_Msg_Member(DWORD, m_dwDiamond)		// 奖励的钻石
Declare_Msg_End

// YGame奖励通知响应
Declare_Msg_Begin(M2O_YGAME_BONUS_NOTIFY_RESP, MSG_M2O_YGAME_BONUS_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 新的一天
Declare_Msg_Begin(S2C_NEW_DAY_NOTIFY, MSG_S2C_NEW_DAY_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 选择服务器请求
Declare_Msg_Begin(C2S_LOGIN_SELECT_SERVER_REQ, MSG_C2S_LOGIN_SELECT_SERVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSelectZoneId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSelectZoneId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSelectZoneId)		// 选择的分区ID
Declare_Msg_End

// 选择服务器响应
Declare_Msg_Begin(S2C_LOGIN_SELECT_SERVER_RESP, MSG_S2C_LOGIN_SELECT_SERVER_RESP)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedChar(m_byHaveRole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedChar(m_byHaveRole)
Declare_EncodeMsg_Function_End
	Declare_Msg_Member(BYTE, m_byHaveRole)		// 是否有角色
Declare_Msg_End

// 创建角色请求
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
	Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// 角色名称
	Declare_Msg_Member(BYTE, m_bySex)		// 角色性别
	Declare_Msg_Member(BYTE, m_byType)		// 创建类型(0=默认,1=新手引导)
Declare_Msg_End



// 开始游戏请求
Declare_Msg_Begin(C2S_START_GAME_REQ, MSG_C2S_START_GAME_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 开始游戏响应
Declare_Msg_Begin(S2C_START_GAME_RESP, MSG_S2C_START_GAME_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwClientVersion)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwClientVersion)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwClientVersion)	// 客户端版本
Declare_Msg_End


// 创建角色响应
Declare_Msg_Begin(S2C_LOGIN_CREATE_ROLE_RESP, MSG_S2C_LOGIN_CREATE_ROLE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 角色名称修改请求
Declare_Msg_Begin(C2S_ROLE_NAME_MODIFY_REQ, MSG_C2S_ROLE_NAME_MODIFY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szRoleName, sizeof(m_szRoleName)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// 角色名称
Declare_Msg_End

// 角色名称修改响应
Declare_Msg_Begin(S2C_ROLE_NAME_MODIFY_RESP, MSG_S2C_ROLE_NAME_MODIFY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szRoleName, sizeof(m_szRoleName)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byResult)
Declare_EncodeMsg_String(m_szRoleName, sizeof(m_szRoleName))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byResult)	// 结果
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// 角色名称
Declare_Msg_End

// 系统公告
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
Declare_Msg_Member(WORD, m_wServerId)	// 广播的服务器ID(0=表示所有服务器)
Declare_Msg_Member(WORD, m_wNoticeId)	// 公告ID
Declare_Msg_Member_Array(char, m_szText, MAX_SYSTEM_NOTICE_LEN + 1)	// 公告内容
Declare_Msg_Member(WORD, m_wParamListNum)	// 参数列表数量
Declare_Msg_Member_Array(SystemNoticeParam, m_ParamList, MAX_SYSTEM_NOTICE_PARAM_NUM)	// 公告内容
Declare_Msg_End



// 进入游戏通知
Declare_Msg_Begin(S2C_LOGIN_ENTER_GAME_NOTIFY, MSG_S2C_LOGIN_ENTER_GAME_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 角色信息同步
Declare_Msg_Begin(S2C_ROLE_INFO_NOTIFY, MSG_S2C_ROLE_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwRoleId)									// 角色ID
Declare_DecodeMsg_String(m_RoleInfo.szRoleName, sizeof(m_RoleInfo.szRoleName) - 1)	// 角色名称
Declare_DecodeMsg_String(m_RoleInfo.szAccount, sizeof(m_RoleInfo.szAccount) - 1)		// 角色帐号
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwStamina)									// 体力
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wLevel)									// 等级
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwExp)										// 经验
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.wSex)									// 性别
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwGold)									// 金币
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwHeadImage)								// 头像图标
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwDiamond)									// 钻石	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwEnergy)									// 精力
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nArenaPrestige)							// 竞技场声望			
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nHeroSoul)									// 魂晶
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nGodSoul)									// 神魂 
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nWanted)									// 通缉令
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwFightingPower)							// 战力	
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.sVipData.wLevel)							// Vip等级	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.sVipData.dwExp)							// Vip经验	
Declare_DecodeMsg_UnsignedChar(m_RoleInfo.wClickLikeTimes)							// 点赞次数	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwRebelValue)								// 叛军值(战功)	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nTowerPrestige)							// 爬塔声望(威名)	
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)											// 工会ID	
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwCurContribution)							// 工会贡献值	
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wDailyInitLevel)							// 每日初始等级
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastStaminaRecoverTime)					// 体力上次恢复时间
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastEnergyRecoverTime)					// 精力上次恢复时间
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.dwLastWantedRecoverTime)					// 通缉令(征讨令)上次恢复时间
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wGuideNum)								// 引导列表
Declare_DecodeMsg_Memory(m_RoleInfo.arGuideList, m_RoleInfo.wGuideNum*sizeof(GuideData));
Declare_DecodeMsg_UnsignedInt(m_dwOpenServerTime)									// 开服时间
Declare_DecodeMsg_UnsignedInt(m_dwServerId)											// 服务器ID
Declare_DecodeMsg_UnsignedInt(m_RoleInfo.nHonor)									// 荣誉
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wFriendshipPoint)					// 总的友情点
Declare_DecodeMsg_UnsignedShort(m_RoleInfo.wTodayFriendshipPoint)					// 今日友情点
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRoleId)									// 角色ID
Declare_EncodeMsg_String(m_RoleInfo.szRoleName, sizeof(m_RoleInfo.szRoleName) - 1)	// 角色名称
Declare_EncodeMsg_String(m_RoleInfo.szAccount, sizeof(m_RoleInfo.szAccount) - 1)		// 角色帐号
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwStamina)									// 体力
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wLevel)									// 等级
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwExp)										// 经验
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.wSex)									// 性别
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwGold)									// 金币
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwHeadImage)								// 头像图标
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwDiamond)									// 钻石	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwEnergy)									// 精力
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nArenaPrestige)							// 竞技场声望			
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nHeroSoul)									// 魂晶
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nGodSoul)									// 神魂 
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nWanted)									// 通缉令
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwFightingPower)							// 战力	
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.sVipData.wLevel)							// Vip等级	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.sVipData.dwExp)							// Vip经验	
Declare_EncodeMsg_UnsignedChar(m_RoleInfo.wClickLikeTimes)							// 点赞次数	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRebelValue)								// 叛军值(战功)	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nTowerPrestige)							// 爬塔声望(威名)	
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)											// 工会ID	
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwCurContribution)							// 工会贡献值	
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wDailyInitLevel)							// 每日初始等级
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastStaminaRecoverTime)					// 体力上次恢复时间
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastEnergyRecoverTime)					// 精力上次恢复时间
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwLastWantedRecoverTime)					// 通缉令(征讨令)上次恢复时间
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wGuideNum)								// 引导列表
Declare_EncodeMsg_Memory(m_RoleInfo.arGuideList, m_RoleInfo.wGuideNum*sizeof(GuideData));
Declare_EncodeMsg_UnsignedInt(m_dwOpenServerTime)									// 开服时间
Declare_EncodeMsg_UnsignedInt(m_dwServerId)											// 服务器ID
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nHonor)									// 荣誉
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wFriendshipPoint)						// 总的友情点
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wTodayFriendshipPoint)					// 今日友情点
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nEndlessStore)								// 无尽石
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwRoleCreateTime)							// 角色创建时间
Declare_EncodeMsg_UnsignedShort(m_RoleInfo.wNobilityLevel)							// 爵位等级
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nGemCoin)									// 宝石币
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nDestinyCoin)								// 命运硬币
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nStarSoul)									// 星魂
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nCrystal)									// 水晶
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nScoreMatchHonor)							// 积分赛荣誉
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nYCoin)									// YCoin
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.nResouceWarScore)							// 资源战积分
Declare_EncodeMsg_UnsignedInt(m_RoleInfo.dwTotalRechargeAmount)						// 总的充值额度
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleBaseInfo, m_RoleInfo)	// 角色基本信息
Declare_Msg_Member(DWORD, m_dwGuildId)	// 工会ID
Declare_Msg_Member(DWORD, m_dwOpenServerTime)	// 开服时间
Declare_Msg_Member(DWORD, m_dwServerId)	// 服务器ID
Declare_Msg_End


// 玩家属性改变
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

// 角色详细请求
Declare_Msg_Begin(C2S_ROLE_DETAILS_REQ, MSG_C2S_ROLE_DETAILS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_Member(DWORD, m_dwTargetId)	// 查找的目标ID
Declare_Msg_End

// 角色详细响应
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
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_Member(ClientRoleDetails, m_Details)
Declare_Msg_End

// 角色属性恢复请求
Declare_Msg_Begin(C2S_ROLE_PRO_RECOVER_REQ, MSG_C2S_ROLE_PRO_RECOVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// 属性类型
Declare_Msg_End

// 角色属性恢复响应
Declare_Msg_Begin(S2C_ROLE_PRO_RECOVER_RESP, MSG_S2C_ROLE_PRO_RECOVER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwRecoverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwRecoverTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)				// 属性类型
Declare_Msg_Member(DWORD, m_dwRecoverTime)		// 恢复时间
Declare_Msg_End


// 头像替换请求
Declare_Msg_Begin(C2S_HEAD_IMAGE_REPLACE_REQ, MSG_C2S_HEAD_IMAGE_REPLACE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeadImageId)		// 头像ID
Declare_Msg_End

// 头像替换响应
Declare_Msg_Begin(S2C_HEAD_IMAGE_REPLACE_RESP, MSG_S2C_HEAD_IMAGE_REPLACE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeadImageId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeadImageId)		// 头像ID
Declare_Msg_End


// 战斗开始(请求)
Declare_Msg_Begin(C2S_BATTLE_START_REQ, MSG_C2S_BATTLE_START_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwParam1)
Declare_DecodeMsg_UnsignedInt(m_dwParam2)
Declare_DecodeMsg_UnsignedInt(m_dwParam3)
Declare_DecodeMsg_UnsignedInt(m_dwParam4)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// 签名
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwParam1)
Declare_EncodeMsg_UnsignedInt(m_dwParam2)
Declare_EncodeMsg_UnsignedInt(m_dwParam3)
Declare_EncodeMsg_UnsignedInt(m_dwParam4)
Declare_EncodeMsg_UnsignedInt(m_dwPower)
Declare_EncodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// 签名
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// 战斗类型
Declare_Msg_Member(DWORD, m_dwParam1)		// 参数1
Declare_Msg_Member(DWORD, m_dwParam2)		// 参数2
Declare_Msg_Member(DWORD, m_dwParam3)		// 参数3
Declare_Msg_Member(DWORD, m_dwParam4)		// 参数4
Declare_Msg_Member(DWORD, m_dwPower)		// 战力
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // 帐号
Declare_Msg_End

// 战斗开始(响应)
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
Declare_Msg_Member(BYTE, m_byType)					// 战斗类型
Declare_Msg_Member(DWORD, m_dwTargetFightingPower)	// 目标战力
Declare_Msg_Member(DWORD, m_dwHelpBattleHeroInfoId)	// 助战英雄信息ID
//Declare_Msg_Member(BYTE, m_byIsPassed)				// 是否已通关
Declare_Msg_Member(DWORD, m_dwParam1)				// 参数1
Declare_Msg_Member(DWORD, m_dwParam2)				// 参数2
Declare_Msg_Member(DWORD, m_dwParam3)				// 参数3
Declare_Msg_Member(DWORD, m_dwParam4)				// 参数3
Declare_Msg_Member(BYTE, m_byMonsterDropNum)									// 掉落物品数量
Declare_Msg_Member_Array(MonsterDropData, m_DropItemList, MAX_MAP_DROP_NUM);	// 掉落物品列表
Declare_Msg_Member(BYTE, m_byMonsterNum)										// 怪物数量
Declare_Msg_Member_Array(MonsterData, m_MonsterList, MAX_BATTLE_MONSTER_NUM);	// 怪物列表
Declare_Msg_Member(WORD, m_wTargetToBattleHeroNum)									// 上阵英雄数量
Declare_Msg_Member_Array(ToBattleHeroData, m_TargetToBattleHeroList, MAX_BATTLE_HERO_NUM);// 目标上阵英雄列表
Declare_Msg_Member(WORD, m_wHelpBattleHeroProNum) // 助战英雄属性数量
Declare_Msg_Member_Array(Property, m_vHelpBattleHeroProList, HERO_PRO_MAX);// 助战英雄属性列表
Declare_Msg_Member(vector<SpiritData>, m_TargetSpriteList) // 目标精灵列表
Declare_Msg_End

// 战斗结束(请求)
Declare_Msg_Begin(C2S_BATTLE_END_REQ, MSG_C2S_BATTLE_END_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_nWinValue)
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_UnsignedInt(m_dwContext1)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
Declare_DecodeMsg_UnsignedInt(m_dwTotalDamge)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// 签名
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
Declare_Msg_Member(BYTE, m_byType)		// 战斗类型
Declare_Msg_Member(int, m_nWinValue)		// 是否获胜值
Declare_Msg_Member(BYTE, m_byStar)		// 获得星数	
Declare_Msg_Member(DWORD, m_dwContext)		// 上下文
Declare_Msg_Member(DWORD, m_dwContext1)		// 上下文1
Declare_Msg_Member(DWORD, m_dwPower)		// 战力
Declare_Msg_Member(DWORD, m_dwTotalDamge)		// 总伤害
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // 签名
Declare_Msg_Member(BYTE, m_byMonsterNum)		// 怪物数量
Declare_Msg_Member_Array(MonsterData, m_MonsterList, MAX_BATTLE_MONSTER_NUM);	// 怪物列表
Declare_Msg_End

// 战斗结束(响应)
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
Declare_Msg_Member(BYTE, m_byIsWin)			// 是否获胜
Declare_Msg_Member(BYTE, m_byStar)			// 获得星数	
Declare_Msg_Member(BYTE, m_byIsFirstWin)	// 是否第一次获得
Declare_Msg_Member(BYTE, m_byIsMeKill)		// 是否是我击杀
Declare_Msg_Member(BYTE, m_byParamNum)		// 参数数量
Declare_Msg_Member_Array(DWORD, m_arParamList, MAX_BATTLE_PARAM_NUM);	// 参数列表
Declare_Msg_Member(WORD, m_wRewardItemNum)
Declare_Msg_Member_Array(Item, m_arRewardItemList, MAX_BATTLE_REWARD_ITEM_NUM);	// 奖励的物品列表
Declare_Msg_End

// 请求助战目标
Declare_Msg_Begin(C2S_HELP_BATTLE_TARGET_REQ, MSG_C2S_HELP_BATTLE_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求助战目标响应
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
Declare_Msg_Member(WORD, m_wTargetNum)	// 目标数量
Declare_Msg_Member_Array(HelpBattleTargetData, m_arTargetList, MAX_HELP_BATTLE_TARGET_NUM);	// 目标列表
Declare_Msg_Member(WORD, m_wTodayUsedHelpBattleTimes)	// 今日已使用助战的次数
Declare_Msg_End

// 点赞
Declare_Msg_Begin(C2S_RANK_LIKE_REQ, MSG_C2S_RANK_LIKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// 类型
Declare_Msg_Member(DWORD, m_dwTargetId)		// 目标ID
Declare_Msg_End

// 点赞响应
Declare_Msg_Begin(S2C_RANK_LIKE_RESP, MSG_S2C_RANK_LIKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// 类型
Declare_Msg_Member(DWORD, m_dwTargetId)		// 目标ID
Declare_Msg_End

// 留言
Declare_Msg_Begin(C2S_RANK_LEAVE_MESSAGE_REQ, MSG_C2S_RANK_LEAVE_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szText, sizeof(m_szText))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_64)	//
Declare_Msg_End

// 留言响应
Declare_Msg_Begin(S2C_RANK_LEAVE_MESSAGE_RESP, MSG_S2C_RANK_LEAVE_MESSAGE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szText, sizeof(m_szText))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_64)	//
Declare_Msg_End

// 留言列表请求
Declare_Msg_Begin(C2S_RANK_LEAVE_MESSAGE_LIST_REQ, MSG_C2S_RANK_LEAVE_MESSAGE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 留言列表响应
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
Declare_Msg_Member(BYTE, m_wLeaveMessageNum)		// 留言列表数量
Declare_Msg_Member_Array(LeaveMessageData, m_LeaveMessageList, MAX_RANK_LEAVE_MESSAGE_NUM);	// 留言列表
Declare_Msg_End

// 请求排行榜列表
Declare_Msg_Begin(C2S_RANK_LIST_REQ, MSG_C2S_RANK_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedChar(m_bySubType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedChar(m_bySubType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)			// 请求类型
Declare_Msg_Member(BYTE, m_bySubType)		// 请求子类型
Declare_Msg_End

// 请求排行榜列表响应
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
Declare_Msg_Member(BYTE, m_byType)									// 请求类型
Declare_Msg_Member(BYTE, m_bySubType)		// 请求子类型
Declare_Msg_Member(WORD, m_wRank)									// 排名
Declare_Msg_Member(DWORD, m_dwRankValue)							// 排名值
Declare_Msg_Member(WORD, m_wListNum)								// 列表数量
Declare_Msg_Member_Array(ClientRoleRankData, m_arRankList, MAX_RANK_MUM);	// 列表数据
Declare_Msg_End



// 请求工会排行榜列表
Declare_Msg_Begin(C2S_GUILD_RANK_LIST_REQ, MSG_C2S_GUILD_RANK_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRankType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRankType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRankType)		// 请求排行类型
Declare_Msg_End

// 工会排行榜请求响应
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
Declare_Msg_Member(BYTE, m_byRankType)								// 请求类型
Declare_Msg_Member(WORD, m_wRank)									// 排名
Declare_Msg_Member(DWORD, m_dwRankValue)							// 排名值
Declare_Msg_Member(WORD, m_wListNum)								// 列表数量
Declare_Msg_Member_Array(ClientGuildRankData, m_arRankList, MAX_GUILD_RANK_MUM);	// 列表数据
Declare_Msg_End


// 请求我的排名
Declare_Msg_Begin(C2S_MY_RANK_REQ, MSG_C2S_MY_RANK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRankType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRankType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRankType)
Declare_Msg_End

// 请求我的排名响应
Declare_Msg_Begin(S2C_MY_RANK_RESP, MSG_S2C_MY_RANK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRankType)
Declare_DecodeMsg_UnsignedShort(m_wMyRank)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRankType)
Declare_EncodeMsg_UnsignedShort(m_wMyRank)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRankType)	// 排名类型
Declare_Msg_Member(WORD, m_wMyRank)		// 我的排名 0=表示木有进入排名
Declare_Msg_End

//加载角色完成通知
// Declare_Msg_Begin(M2C_LOAD_CHAR_FINISH_CMD, MSG_M2C_LOAD_CHAR_FINISH_CMD)
// Declare_DecodeMsg_Empty_Function_Begin
// Declare_DecodeMsg_Empty_Function_End
// 
// Declare_EncodeMsg_Empty_Function_Begin
// Declare_EncodeMsg_Empty_Function_End
// 
// Declare_Msg_End

// 更新引导请求
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

// 更新引导响应
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


//心跳包
Declare_Msg_Begin(C2S_HEART_BEAT_REQ, MSG_C2S_HEART_BEAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwValue)
Declare_Msg_End

//心跳包
Declare_Msg_Begin(S2C_HEART_BEAT_RESP, MSG_S2C_HEART_BEAT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwValue)
Declare_Msg_End


//服务器时间
Declare_Msg_Begin(C2S_SERVER_TIME_REQ, MSG_C2S_SERVER_TIME_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//同步服务器时间
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


// 邮件列表请求
Declare_Msg_Begin(C2S_EMAIL_LIST_REQ, MSG_C2S_EMAIL_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 邮件列表响应
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
	//Declare_EncodeMsg_UnsignedInt(sEmail.dwID)			// 唯一ID
	Declare_EncodeMsg_UnsignedShort(sEmail.wInfoId)			// 信息ID
	Declare_EncodeMsg_UnsignedInt(sEmail.dwReceiveTime)		// 接收时间
	Declare_EncodeMsg_UnsignedShort(sEmail.wContextNum);
	for (BYTE j = 0; j < sEmail.wContextNum; ++j)
	{
		EmailContext &sContext = sEmail.arContextList[j];
		Declare_EncodeMsg_UnsignedChar(sContext.wType);						// 上下文类型
		Declare_EncodeMsg_String(sContext.szText, sizeof(sContext.szText));	// 上下文缓存
	}
	Declare_EncodeMsg_String(sEmail.szTitle, sizeof(sEmail.szTitle));	// 标题
	Declare_EncodeMsg_String(sEmail.szText, sizeof(sEmail.szText));	// 邮件内容
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE,  m_byEmailNum)								// 邮件数量
Declare_Msg_Member_Array(EmailInfo, m_EmailList, MAX_EMAIL_COUNT)   // 邮件列表
Declare_Msg_End


//新邮件
Declare_Msg_Begin(S2C_EMAIL_NEW_NOTIFY, MSG_S2C_EMAIL_NEW_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tEmailInfo.wInfoId) // 信息ID
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
//Declare_EncodeMsg_UnsignedInt(m_tEmailInfo.dwID)			// 唯一ID
Declare_EncodeMsg_UnsignedShort(m_tEmailInfo.wInfoId)		// 信息ID
Declare_EncodeMsg_UnsignedInt(m_tEmailInfo.dwReceiveTime)	// 接收时间
Declare_EncodeMsg_UnsignedShort(m_tEmailInfo.wContextNum);
for (WORD i = 0; i < m_tEmailInfo.wContextNum; ++i)
{
	EmailContext &sContext = m_tEmailInfo.arContextList[i];
	Declare_EncodeMsg_UnsignedChar(sContext.wType);	// 上下文类型
	Declare_EncodeMsg_String(sContext.szText, sizeof(sContext.szText));	// 上下文缓存
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(EmailInfo, m_tEmailInfo)
Declare_Msg_End


//C2M 聊天请求
Declare_Msg_Begin(C2S_CHAT_REQ, MSG_C2S_CHAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_tData.byChatType)						// 聊天类型
	Declare_DecodeMsg_UnsignedChar(m_tData.byIsVoice)					// 是否语音
	Declare_DecodeMsg_UnsignedInt(m_tData.dwRoleId)						// 目标角色ID
	Declare_DecodeMsg_String(m_tData.szTargetName, sizeof(m_tData.szTargetName)-1)		// 目标名称
	Declare_DecodeMsg_String(m_tData.szChatText, sizeof(m_tData.szChatText)-1)	// 聊天内容
	Declare_DecodeMsg_String(m_tData.szVoice, sizeof(m_tData.szVoice)-1)		// 语音内容
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_tData.byChatType)						// 聊天类型
	Declare_EncodeMsg_UnsignedChar(m_tData.byIsVoice)					// 是否语音
	Declare_EncodeMsg_UnsignedInt(m_tData.dwRoleId)						// 目标角色ID
	Declare_EncodeMsg_String(m_tData.szTargetName, sizeof(m_tData.szTargetName))		// 目标名称
	Declare_EncodeMsg_String(m_tData.szChatText, sizeof(m_tData.szChatText))		// 聊天内容
	Declare_EncodeMsg_String(m_tData.szVoice, sizeof(m_tData.szVoice))		// 语音内容
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ChatReqData, m_tData)
Declare_Msg_End

//聊天应答
Declare_Msg_Begin(S2C_CHAT_RESP, MSG_S2C_CHAT_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//聊天通知
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
Declare_Msg_Member(BYTE, m_byGMLevel)	// 聊天频道
Declare_Msg_End

// 聊天语音请求
Declare_Msg_Begin(C2S_CHAT_VOICE_REQ, MSG_C2S_CHAT_VOICE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwVoiceId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwVoiceId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwVoiceId)	// 语音ID
Declare_Msg_End


// 聊天语音响应
Declare_Msg_Begin(S2C_CHAT_VOICE_RESP, MSG_S2C_CHAT_VOICE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szVoice, MAX_CHAT_VOICE_LEN - 1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szVoice, MAX_CHAT_VOICE_LEN)
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szVoice, MAX_CHAT_VOICE_LEN)	//聊天语音
Declare_Msg_End


// 聊天记录列表请求
Declare_Msg_Begin(C2S_CHAT_RECORD_LIST_REQ, MSG_C2S_CHAT_RECORD_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byChatType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byChatType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byChatType)	// 聊天频道
Declare_Msg_End

// 聊天记录列表响应
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
Declare_Msg_Member(BYTE, m_byChatType)	// 聊天频道
Declare_Msg_Member(WORD, m_wRecordNum)							// 记录数量
Declare_Msg_Member_Array(ChatRecord, m_arRecordList, MAX_CHAT_RECORD_COUNT)	// 聊天记录列表
Declare_Msg_End


// 系统公告
Declare_Msg_Begin(S2C_SYS_BROADCAST_NOTIFY, MSG_S2C_SYS_BROADCAST_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwBroadCastId)
Declare_DecodeMsg_String(m_szText, MAX_BROADCAST_TEXT_LEN)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwBroadCastId)
Declare_EncodeMsg_String(m_szText, MAX_BROADCAST_TEXT_LEN)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwBroadCastId)							// 公告ID
Declare_Msg_Member_Array(char, m_szText, MAX_BROADCAST_TEXT_LEN + 1)	//字符串1
Declare_Msg_End


// 商品购买
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
Declare_Msg_Member(BYTE, m_byType)		// 商店商品类型
Declare_Msg_Member(WORD, m_wParam1)		// 参数1
Declare_Msg_Member(WORD, m_wParam2)		// 参数2
Declare_Msg_Member(WORD, m_wParam3)		// 参数2
Declare_Msg_End

// 商品购买响应
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
Declare_Msg_Member(BYTE, m_byType)		// 商店商品类型
Declare_Msg_Member(WORD, m_wParam1)		// 参数1
Declare_Msg_Member(WORD, m_wParam2)		// 参数2
Declare_Msg_Member(WORD, m_wParam3)		// 参数2
Declare_Msg_End

// 商品购买记录请求
Declare_Msg_Begin(C2S_SHOP_GOODS_BUY_RECORD_REQ, MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// 商店商品类型
Declare_Msg_End

// 商品购买记录响应
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
Declare_Msg_Member(BYTE, m_byType)			// 商店商品类型
Declare_Msg_Member(BYTE, m_byBuyRecordNum)	// 购买记录数量
Declare_Msg_Member_Array(ShopGoodsBuyRecord, m_arBuyRecord, MAX_GOOGS_BUY_RECORD_NUM);	// 购买记录
Declare_Msg_End

// 商品出售
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
Declare_Msg_Member(BYTE, m_byObjType)	// 对象类型
Declare_Msg_Member(BYTE, m_byGoodsNum)	// 商品数量
Declare_Msg_Member_Array(DWORD, m_GoodsList, MAX_SELL_GOODS_NUM);	// 商品列表
Declare_Msg_End

// 商品出售响应
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
Declare_Msg_Member(BYTE, m_byObjType)	// 对象类型
Declare_Msg_Member(BYTE, m_byGoodsNum)	// 商品数量
Declare_Msg_Member_Array(DWORD, m_GoodsList, MAX_SELL_GOODS_NUM);	// 商品列表
Declare_Msg_Member(BYTE, m_byCurrencyNum)	// 货币数量
Declare_Msg_Member_Array(MoneyData, m_CurrencyList, MAX_MONEY_TYPE_NUM);	// 获得的货币列表
Declare_Msg_End


// 随时商品列表请求
Declare_Msg_Begin(C2S_SHOP_LIMIT_GOODS_LIST_REQ, MSG_C2S_SHOP_LIMIT_GOODS_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// 商店商品类型
Declare_Msg_End

// 随时商品列表响应
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
Declare_Msg_Member(BYTE, m_byType)			// 商店商品类型
Declare_Msg_Member(DWORD, m_dwRefreshTime)	// 刷新时间
Declare_Msg_Member(BYTE, m_byGoodsNum)	// 商品数量
Declare_Msg_Member_Array(Goods, m_arGoodsList, MAX_LIMIT_GOODS_NUM);	// 商品列表
Declare_Msg_End

// 商店刷新请求
Declare_Msg_Begin(C2S_SHOP_REFRESH_REQ, MSG_C2S_SHOP_REFRESH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byShopType)
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byShopType)
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byShopType)			// 商店商品类型
Declare_Msg_Member(BYTE, m_byRefreshType)		// 刷新类型
Declare_Msg_End

// 商店刷新响应
Declare_Msg_Begin(S2C_SHOP_REFRESH_RESP, MSG_S2C_SHOP_REFRESH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byShopType)
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byShopType)
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byShopType)			// 商店商品类型
Declare_Msg_Member(BYTE, m_byRefreshType)		// 刷新类型
Declare_Msg_End


// 英雄商店数据请求
Declare_Msg_Begin(C2S_HERO_SHOP_DATA_REQ, MSG_C2S_HERO_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 英雄商店数据响应
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
Declare_Msg_Member(RefreshShopData, m_HeroShop)		// 商店商品类型
Declare_Msg_End

// 觉醒商店数据请求
Declare_Msg_Begin(C2S_AWAKEN_SHOP_DATA_REQ, MSG_C2S_AWAKEN_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 觉醒商店数据响应
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
Declare_Msg_Member(RefreshShopData, m_AwakenShop)		// 商店商品类型
Declare_Msg_End


// 星魂商店数据请求
Declare_Msg_Begin(C2S_STAR_SOUL_SHOP_DATA_REQ, MSG_C2S_STAR_SOUL_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 星魂商店数据响应
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
Declare_Msg_Member(StarSoulShopData, m_ShopData)		// 商店商品类型
Declare_Msg_End

// 直购商店数据请求
Declare_Msg_Begin(C2S_DIRECT_SHOP_DATA_REQ, MSG_C2S_DIRECT_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 直购商店数据响应
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
Declare_Msg_Member(DirectBuyShopData, m_ShopData)		// 商店商品类型
Declare_Msg_End

// 领取直购商品奖励请求
Declare_Msg_Begin(C2S_TAKE_DIRECT_BUY_GOODS_REQ, MSG_C2S_TAKE_DIRECT_BUY_GOODS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)		// 领取的商品ID
Declare_Msg_End

// 领取直购商品奖励请求
Declare_Msg_Begin(S2C_TAKE_DIRECT_BUY_GOODS_RESP, MSG_S2C_TAKE_DIRECT_BUY_GOODS_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)		// 领取的商品ID
Declare_Msg_End

// 水晶商店数据请求
Declare_Msg_Begin(C2S_CRYSTAL_SHOP_DATA_REQ, MSG_C2S_CRYSTAL_SHOP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 水晶商店数据响应
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
Declare_Msg_Member(CrystalShopData, m_ShopData)		// 商店数据
Declare_Msg_End

// 副本扫荡请求
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
Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
//Declare_Msg_Member(BYTE, m_byCleanTimes)	// 扫荡次数
Declare_Msg_Member(DWORD, m_dwParam1)		// 参数1
Declare_Msg_Member(DWORD, m_dwParam2)		// 参数2
Declare_Msg_End

// 副本扫荡响应
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
Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
Declare_Msg_Member(DWORD, m_dwParam1)		// 参数1
Declare_Msg_Member(DWORD, m_dwParam2)		// 参数2
Declare_Msg_Member(BYTE, m_byResult)		// 结果(0=正常, 1=遇到叛军, 2=英雄背包已满, 3=装备背包已满)
Declare_Msg_Member(vector<Item>, m_vDropItemList)	// 掉落物品列表
Declare_Msg_End


// 主线副本数据请求
Declare_Msg_Begin(C2S_FB_MAIN_DATA_REQ, MSG_C2S_FB_MAIN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 主线副本数据响应
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
Declare_Msg_Member(MainFBData, m_Data)	// 副本数据
Declare_Msg_End



// 日常副本数据请求
Declare_Msg_Begin(C2S_FB_DAILY_DATA_REQ, MSG_C2S_FB_DAILY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 英雄副本数据请求
Declare_Msg_Begin(C2S_FB_HERO_DATA_REQ, MSG_C2S_FB_HERO_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 英雄副本数据响应
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
Declare_Msg_Member(HeroFBData, m_tFBData)	// 副本数据
Declare_Msg_End


// 精英副本数据请求
Declare_Msg_Begin(C2S_FB_ELITE_DATA_REQ, MSG_C2S_FB_ELITE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 精英副本数据响应
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
Declare_Msg_Member(EliteFBData, m_Data)	// 副本数据
Declare_Msg_End

// 日常副本数据请求
Declare_Msg_Begin(C2S_TOWER_DATA_REQ, MSG_C2S_TOWER_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 日常副本数据响应
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
Declare_Msg_Member(DailyFBData, m_tFBData)	// 日常副本数据
Declare_Msg_End

// 爬塔副本数据
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
Declare_Msg_Member(TowerFBData, m_FBData)		// 副本数据
Declare_Msg_Member(BYTE, m_byRandProNum)		// 随机属性数量
Declare_Msg_End

// 购买挑战次数请求
Declare_Msg_Begin(C2S_TOWER_BUY_TIMES_REQ, MSG_C2S_TOWER_BUY_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 购买挑战次数响应
Declare_Msg_Begin(S2C_TOWER_BUY_TIMES_RESP, MSG_S2C_TOWER_BUY_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 重置关卡
Declare_Msg_Begin(C2S_TOWER_RESET_REQ, MSG_C2S_TOWER_RESET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 重置关卡响应
Declare_Msg_Begin(S2C_TOWER_RESET_RESP, MSG_S2C_TOWER_RESET_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 选择属性
Declare_Msg_Begin(C2S_TOWER_SELECT_PRO_REQ, MSG_C2S_TOWER_SELECT_PRO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedChar(m_byProIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStar)
Declare_EncodeMsg_UnsignedChar(m_byProIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStar)			// 星数
Declare_Msg_Member(BYTE, m_byProIndex)		// 属性索引
Declare_Msg_End


// 选择属性响应
Declare_Msg_Begin(S2C_TOWER_SELECT_PRO_RESP, MSG_S2C_TOWER_SELECT_PRO_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStar)
Declare_DecodeMsg_UnsignedChar(m_byProIndex)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStar)
Declare_EncodeMsg_UnsignedChar(m_byProIndex)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStar)			// 星数
Declare_Msg_Member(BYTE, m_byProIndex)		// 属性索引
Declare_Msg_End

// 购买星数商品
Declare_Msg_Begin(C2S_TOWER_BUY_STAR_GOODS_REQ, MSG_C2S_TOWER_BUY_STAR_GOODS_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 购买星数商品响应
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
Declare_Msg_Member(BYTE, m_byIsToHistoryMap)		// 是否扫荡到历史关卡
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
Declare_Msg_Member(BYTE, m_byIsToHistoryMap)		// 是否扫荡到历史关卡
Declare_Msg_Member(BYTE, m_byBegin)		// 开始
Declare_Msg_Member(BYTE, m_byEnd)		// 结束
Declare_Msg_End

// 副本奖励领取
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
Declare_Msg_Member(BYTE, m_byAwardType)		// 奖励类型
Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
Declare_Msg_Member(WORD, m_wChapter)		// 章节
Declare_Msg_Member(BYTE, m_byIndex)			// 索引
Declare_Msg_End

// 副本奖励领取响应
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
Declare_Msg_Member(BYTE, m_byAwardType)		// 奖励类型
Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
Declare_Msg_Member(WORD, m_wChapter)		// 章节
Declare_Msg_Member(BYTE, m_byIndex)			// 索引
Declare_Msg_End


// 副本重置请求
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
 Declare_Msg_Member(BYTE, m_byMapType)		// 关卡类型
 Declare_Msg_Member(DWORD, m_dwParam1)		// 参数1
 Declare_Msg_Member(DWORD, m_dwParam2)		// 参数2
 Declare_Msg_End

 // 副本重置响应
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
 Declare_Msg_Member(BYTE, m_byMapType)		// 关卡类型
 Declare_Msg_Member(DWORD, m_dwParam1)		// 参数1
 Declare_Msg_Member(DWORD, m_dwParam2)		// 参数2
 Declare_Msg_End

 // 一键领取奖励请求(关卡+星数)
 Declare_Msg_Begin(C2S_FB_ONE_KEY_TAKE_REWARD_REQ, MSG_C2S_FB_ONE_KEY_TAKE_REWARD_REQ)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byFBType)
 Declare_DecodeMsg_UnsignedShort(m_wChapter)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byFBType)
 Declare_EncodeMsg_UnsignedShort(m_wChapter)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
 Declare_Msg_Member(WORD, m_wChapter)		// 关卡类型
 Declare_Msg_End

 // 一键领取奖励请求(关卡+星数)
 Declare_Msg_Begin(S2C_FB_ONE_KEY_TAKE_REWARD_RESP, MSG_S2C_FB_ONE_KEY_TAKE_REWARD_RESP)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedChar(m_byFBType)
 Declare_DecodeMsg_UnsignedShort(m_wChapter)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedChar(m_byFBType)
 Declare_EncodeMsg_UnsignedShort(m_wChapter)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(BYTE, m_byFBType)		// 副本类型
 Declare_Msg_Member(WORD, m_wChapter)		// 关卡类型
 Declare_Msg_End

// 领取主线副本章节奖励请求
 Declare_Msg_Begin(C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ, MSG_C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ)
 Declare_DecodeMsg_Function_Begin
 Declare_DecodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_DecodeMsg_Function_End
 Declare_EncodeMsg_Function_Begin
 Declare_EncodeMsg_UnsignedShort(m_wRewardLevel)
 Declare_EncodeMsg_Function_End
 Declare_Msg_Member(WORD, m_wRewardLevel)		// 奖励等级
 Declare_Msg_End

 // 领取主线副本章节奖励响应
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
 Declare_Msg_Member(WORD, m_wRewardLevel)		// 奖励等级
 Declare_Msg_Member(vector<Item>, m_vItemList)	// 物品列表
 Declare_Msg_End

// 物品列表请求
Declare_Msg_Begin(C2S_ITEM_LIST_REQ, MSG_C2S_ITEM_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 物品列表响应
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
Declare_Msg_Member(WORD, m_wItemNum)	// 物品数量
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ITEM_NUM);	// 物品列表
Declare_Msg_End

// 物品使用请求
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
Declare_Msg_Member(DWORD, m_dwItemId)		// 使用的物品ID
Declare_Msg_Member(WORD, m_wUseNum)			// 使用的数量
Declare_Msg_Member(DWORD, m_dwGiftItemId)	// 选择使用的礼包物品ID
Declare_Msg_End

// 物品使用响应
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
Declare_Msg_Member(DWORD, m_dwItemId)	// 使用的物品ID
Declare_Msg_Member(WORD, m_wUseNum)		// 使用的数量
Declare_Msg_Member(BYTE, m_byItemNum)	// 物品数量
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ITEM_NUM);	// 物品列表
Declare_Msg_Member(BYTE, m_byMoneyNum)	// 货币数量
Declare_Msg_Member_Array(MoneyData, m_MoneyList, MAX_MONEY_TYPE_NUM);	// 货币列表
Declare_Msg_End

// 物品翻牌请求
Declare_Msg_Begin(C2S_ITEM_FP_REQ, MSG_C2S_ITEM_FP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// 翻牌类型
Declare_Msg_End

// 物品翻牌响应
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
Declare_Msg_Member(BYTE, m_byType)			// 翻牌类型
Declare_Msg_Member(DWORD, m_dwFPItemId)		// 翻牌的物品ID
Declare_Msg_Member(Item, m_tCreateItem)		// 产生的随机物品
Declare_Msg_Member(MoneyData, m_tMoneyData)	// 产生的货币
Declare_Msg_End


// 物品合成请求
Declare_Msg_Begin(C2S_ITEM_COMPOSE_REQ, MSG_C2S_ITEM_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// 合成的物品ID
Declare_Msg_End

// 物品合成响应
Declare_Msg_Begin(S2C_ITEM_COMPOSE_RESP, MSG_S2C_ITEM_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// 合成的物品ID
Declare_Msg_End

// 物品分解请求
Declare_Msg_Begin(C2S_ITEM_DECOMPOSE_REQ, MSG_C2S_ITEM_DECOMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// 分解的物品ID
Declare_Msg_Member(WORD, m_wItemNum)	// 分解的物品数量
Declare_Msg_End

// 物品分解响应
Declare_Msg_Begin(S2C_ITEM_DECOMPOSE_RESP, MSG_S2C_ITEM_DECOMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wItemId)
Declare_DecodeMsg_UnsignedShort(m_wItemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wItemId)
Declare_EncodeMsg_UnsignedShort(m_wItemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wItemId)		// 分解的物品ID
Declare_Msg_Member(WORD, m_wItemNum)	// 分解的物品数量
Declare_Msg_End

// 物品数据
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
Declare_Msg_Member(DWORD, m_dwItemId)	// 物品ID
Declare_Msg_Member(DWORD, m_dwUseNum)		// 物品数量
Declare_Msg_Member(BYTE, m_byFlag)		// 操作标志(0=减少,1=添加)
Declare_Msg_End


// 请求英雄基础信息列表
Declare_Msg_Begin(C2S_HERO_BASE_LIST_REQ, MSG_C2S_HERO_BASE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求英雄基础信息列表响应
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
Declare_Msg_Member(WORD, m_wHeroBaseNum)	// 英雄数量
Declare_Msg_Member_Array(ClientHeroBaseData, m_HeroBaseList, MAX_HERO_BAG_NUM);	// 英雄列表
Declare_Msg_End

// 请求英雄详细信息
Declare_Msg_Begin(C2S_HERO_INFO_REQ, MSG_C2S_HERO_INFO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// 详细ID
Declare_Msg_End

// 请求英雄详细信息响应
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
	Declare_EncodeMsg_UnsignedChar(i + 1);							// 属性类型
	Declare_EncodeMsg_UnsignedChar(m_Info.arTrainProLvList[i]);		// 属性等级
	Declare_EncodeMsg_UnsignedInt(m_Info.arTrainProValueList[i]);		// 属性值
	Declare_EncodeMsg_UnsignedInt(m_Info.arLastTrainTmpProList[i]);	// 属性临时值
	Declare_EncodeMsg_UnsignedChar(m_Info.arTrainRewardFlag[i]);		// 奖励标志
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(ClientHero, m_Info)	// 详细信息
Declare_Msg_End

// 觉醒请求
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
Declare_Msg_Member(DWORD, m_dwUId)	// 觉醒的英雄
Declare_Msg_Member(WORD, m_wCostHeroNum)	// 消耗的英雄数量
Declare_Msg_Member_Array(DWORD, m_CollectionList, MAX_HERO_AWAKEN_COST_HERO_NUM);	// 作为消耗的英雄
Declare_Msg_End


// 觉醒响应
Declare_Msg_Begin(S2C_HERO_AWAKEN_RESP, MSG_S2C_HERO_AWAKEN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byAwakenStar)
Declare_EncodeMsg_UnsignedChar(m_byAwakenLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// 觉醒的英雄
Declare_Msg_Member(BYTE, m_byAwakenStar)	// 觉醒星级
Declare_Msg_Member(BYTE, m_byAwakenLevel)	// 觉醒等级
Declare_Msg_End

// 装备觉醒道具请求
Declare_Msg_Begin(C2S_HERO_EQUIP_AWAKEN_ITEM_REQ, MSG_C2S_HERO_EQUIP_AWAKEN_ITEM_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// 觉醒的英雄
Declare_Msg_Member(WORD, m_wAwakenItemId)	// 装备的觉醒道具
Declare_Msg_End

// 装备觉醒道具响应
Declare_Msg_Begin(S2C_HERO_EQUIP_AWAKEN_ITEM_RESP, MSG_S2C_HERO_EQUIP_AWAKEN_ITEM_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedShort(m_wAwakenItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)			// 觉醒的英雄
Declare_Msg_Member(WORD, m_wAwakenItemId)	// 装备的觉醒道具
Declare_Msg_End

// 请求英雄上阵
Declare_Msg_Begin(C2S_HERO_TO_BATTLE_REQ, MSG_C2S_HERO_TO_BATTLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// 详细ID
Declare_Msg_Member(BYTE, m_byPos)	// 上阵位置
Declare_Msg_End

// 请求英雄上阵响应
Declare_Msg_Begin(S2C_HERO_TO_BATTLE_RESP, MSG_S2C_HERO_TO_BATTLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// 详细ID
Declare_Msg_Member(BYTE, m_byPos)	// 上阵位置
Declare_Msg_End

// 请求英雄收集列表
Declare_Msg_Begin(C2S_HERO_COLLECTION_LIST_REQ, MSG_C2S_HERO_COLLECTION_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求英雄列表响应
Declare_Msg_Begin(S2C_HERO_COLLECTION_LIST_RESP, MSG_S2C_HERO_COLLECTION_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byCollectionNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byCollectionNum)
Declare_EncodeMsg_Memory(m_CollectionList, m_byCollectionNum * sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCollectionNum)							// 英雄收集数量
Declare_Msg_Member_Array(WORD, m_CollectionList, MAX_HERO_NUM);	// 英雄收集列表
Declare_Msg_End


// 突破请求
Declare_Msg_Begin(C2S_HERO_BREAK_REQ, MSG_C2S_HERO_BREAK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
// 同名英雄
Declare_DecodeMsg_UnsignedShort(m_wCostSameNameHeroNum)
m_wCostSameNameHeroNum = min(m_wCostSameNameHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameNameHeroList, m_wCostSameNameHeroNum * sizeof(DWORD));
// 同品质英雄
Declare_DecodeMsg_UnsignedShort(m_wCostSameQualityHeroNum)
m_wCostSameQualityHeroNum = min(m_wCostSameQualityHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameQualityHeroList, m_wCostSameQualityHeroNum * sizeof(DWORD));
// 同职业英雄
Declare_DecodeMsg_UnsignedShort(m_wCostSameProfessionHeroNum)
m_wCostSameProfessionHeroNum = min(m_wCostSameProfessionHeroNum, MAX_HERO_NUM);
Declare_DecodeMsg_Memory(m_arCostSameProfessionHeroList, m_wCostSameProfessionHeroNum * sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
// 同名英雄
Declare_EncodeMsg_UnsignedShort(m_wCostSameNameHeroNum)
m_wCostSameNameHeroNum = min(m_wCostSameNameHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameNameHeroList, m_wCostSameNameHeroNum * sizeof(DWORD));
// 同品质英雄
Declare_EncodeMsg_UnsignedShort(m_wCostSameQualityHeroNum)
m_wCostSameQualityHeroNum = min(m_wCostSameQualityHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameQualityHeroList, m_wCostSameQualityHeroNum * sizeof(DWORD));
// 同职业英雄
Declare_EncodeMsg_UnsignedShort(m_wCostSameProfessionHeroNum)
m_wCostSameProfessionHeroNum = min(m_wCostSameProfessionHeroNum, MAX_HERO_NUM);
Declare_EncodeMsg_Memory(m_arCostSameProfessionHeroList, m_wCostSameProfessionHeroNum * sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)	// UID
Declare_Msg_Member_Array(DWORD, m_arCostSameNameHeroList, MAX_HERO_NUM);		// 同名英雄列表
Declare_Msg_Member(WORD, m_wCostSameNameHeroNum)								// 同名英雄数量
Declare_Msg_Member_Array(DWORD, m_arCostSameQualityHeroList, MAX_HERO_NUM);		// 同品质英雄列表 
Declare_Msg_Member(WORD, m_wCostSameQualityHeroNum)								// 同品质英雄数量
Declare_Msg_Member_Array(DWORD, m_arCostSameProfessionHeroList, MAX_HERO_NUM);	// 同职业英雄列表
Declare_Msg_Member(WORD, m_wCostSameProfessionHeroNum)							// 同职业英雄数量
Declare_Msg_End


// 突破请求
Declare_Msg_Begin(S2C_HERO_BREAK_RESP, MSG_S2C_HERO_BREAK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)	// UID
Declare_Msg_End

// 英雄合成请求
Declare_Msg_Begin(C2S_HERO_COMPOSE_REQ, MSG_C2S_HERO_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wHeroInfoId)	// 英雄配置ID
Declare_Msg_End

// 英雄合成响应
Declare_Msg_Begin(S2C_HERO_COMPOSE_RESP, MSG_S2C_HERO_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wHeroInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wHeroInfoId)	// 英雄配置ID
Declare_Msg_End

// 培养请求
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
Declare_Msg_Member(BYTE, m_byCostType)		// 消耗类型 (1=消耗(物品), 2=消耗(物品+钻石))
Declare_Msg_Member(BYTE, m_byTrainTimes)	// 培养次数
Declare_Msg_End

//////////////////////////////////////////////// todo::到此处理.

// 培养响应
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
Declare_Msg_Member_Array(Property, m_ProList, MAX_HERO_TRAIN_PROP_MUM);	// 英雄培养属性列表
Declare_Msg_Member(BYTE, m_byProNum)	// 属性数量
Declare_Msg_End

// 英雄培养替换请求
Declare_Msg_Begin(C2S_HERO_TRAIN_REPLACE_REQ, MSG_C2S_HERO_TRAIN_REPLACE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// 英雄培养替换响应
Declare_Msg_Begin(S2C_HERO_TRAIN_REPLACE_RESP, MSG_S2C_HERO_TRAIN_REPLACE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// 英雄培养奖励领取请求
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
Declare_Msg_Member(BYTE, m_byProType)		// 属性类型
Declare_Msg_End

// 英雄培养奖励领取响应
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
Declare_Msg_Member(BYTE, m_byProType)		// 属性类型
Declare_Msg_End


// 英雄进化请求
Declare_Msg_Begin(C2S_HERO_EVOLUTION_REQ, MSG_C2S_HERO_EVOLUTION_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End

// 英雄进化响应
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
Declare_Msg_Member(WORD, m_wQuality)		// 品质
Declare_Msg_Member(WORD, m_wStage)		// 品阶
Declare_Msg_End


// VIP召唤请求
Declare_Msg_Begin(C2S_HERO_VIP_SUMMON_REQ, MSG_C2S_HERO_VIP_SUMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_dwCamp)		// 阵营
Declare_Msg_End


// VIP召唤响应
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
Declare_Msg_Member(DWORD, m_dwCamp)	// 阵营
Declare_Msg_Member(vector<Item>, m_vHeroList)	// 获得的英雄列表
Declare_Msg_End


// 英雄基本信息通知
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
// Declare_Msg_Member(BYTE, m_byFlag)						// 数据标志(1=添加，更新，0=删除)
// Declare_Msg_Member(ClientHeroBaseData, m_BaseData)		// 英雄基本数据
// Declare_Msg_End

// 英雄基本信息通知
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
Declare_Msg_Member(BYTE, m_byFlag)	// 数据标志(1=添加，更新，0=删除)
Declare_Msg_Member(WORD, m_wHeroNum)// 英雄数量
Declare_Msg_Member_Array(ClientHeroBaseData, m_HeroBaseDataList, MAX_HERO_BAG_NUM) // 英雄基本数据列表
Declare_Msg_End


// 英雄升级请求
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
Declare_Msg_Member(WORD, m_wCostHeroNum)				// 消耗的英雄数量
Declare_Msg_Member_Array(DWORD, m_arCostHeroList, MAX_HERO_BAG_NUM)	// 消耗的英雄列表
Declare_Msg_End

// 英雄升级响应
Declare_Msg_Begin(S2C_HERO_LEVEL_UP_RESP, MSG_S2C_HERO_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUId)		// UID
Declare_Msg_End


// 召唤请求
Declare_Msg_Begin(C2S_HERO_SUMMON_REQ, MSG_C2S_HERO_SUMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)		// 召唤类型
Declare_Msg_End


// 召唤响应
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
Declare_Msg_Member(BYTE, m_byType)		// 召唤类型
Declare_Msg_Member(BYTE, m_byHeroNum)		// 召唤获得的英雄数量
Declare_Msg_Member_Array(WORD, m_HeroList, HERO_ONE_KEY_SUMMON_TIMES);	// 召唤获得的英雄列表
Declare_Msg_End

// 天命(潜能)请求
Declare_Msg_Begin(C2S_HERO_DESTINY_REQ, MSG_C2S_HERO_DESTINY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// 英雄UID
Declare_Msg_End

// 天命(潜能)响应
Declare_Msg_Begin(S2C_HERO_DESTINY_RESP, MSG_S2C_HERO_DESTINY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_UnsignedChar(m_byDestinyLevel)
Declare_EncodeMsg_UnsignedInt(m_dwDestinyExp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// 英雄UID
Declare_Msg_Member(BYTE, m_byDestinyLevel)	// 天命等级
Declare_Msg_Member(DWORD, m_dwDestinyExp)	// 天命经验
Declare_Msg_End

// 出售请求
Declare_Msg_Begin(C2S_HERO_SELL_REQ, MSG_C2S_HERO_SELL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// 英雄UID
Declare_Msg_End

// 出售响应
Declare_Msg_Begin(S2C_HERO_SELL_RESP, MSG_S2C_HERO_SELL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwHeroUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwHeroUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwHeroUID)		// 英雄UID
Declare_Msg_End

// 援军上阵
Declare_Msg_Begin(C2S_REINFORCEMENT_TO_BATTLE_REQ, MSG_C2S_REINFORCEMENT_TO_BATTLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// 详细ID
Declare_Msg_Member(BYTE, m_byPos)	// 上阵位置
Declare_Msg_End

// 援军上阵响应
Declare_Msg_Begin(S2C_REINFORCEMENT_TO_BATTLE_RESP, MSG_S2C_REINFORCEMENT_TO_BATTLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUId)
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUId)
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUId)	// 详细ID
Declare_Msg_Member(BYTE, m_byPos)	// 上阵位置
Declare_Msg_End

// 召唤数据请求
Declare_Msg_Begin(C2S_HERO_SUMMON_DATA_REQ, MSG_C2S_HERO_SUMMON_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 召唤数据请求
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
Declare_Msg_Member(BYTE, m_byNormalFreeTimes)			// 普通免费召唤次数
Declare_Msg_Member(DWORD, m_dwNormalFreeTime)			// 普通免费召唤时间
Declare_Msg_Member(DWORD, m_dwAdvancedFreeTime)			// 高级免费召唤时间
Declare_Msg_Member(BYTE, m_byAdvancedTimes)				// 高级召唤次数
Declare_Msg_End


// 请求宝物列表
Declare_Msg_Begin(C2S_TREASURE_LIST_REQ, MSG_C2S_TREASURE_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求宝物列表响应
Declare_Msg_Begin(S2C_TREASURE_LIST_RESP, MSG_S2C_TREASURE_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wTreasureNum = min(m_wTreasureNum, MAX_TREASURE_NUM);
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Memory(m_TreasureList, m_wTreasureNum * sizeof(TreasureBaseData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureNum)	// 宝物数量
Declare_Msg_Member_Array(TreasureBaseData, m_TreasureList, MAX_TREASURE_NUM);	// 宝物列表
Declare_Msg_End


// 宝物基本信息通知
Declare_Msg_Begin(S2C_TREASURE_INFO_NOTIFY, MSG_S2C_TREASURE_INFO_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Memory(m_sBaseData, sizeof(TreasureBaseData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Memory(m_sBaseData, sizeof(TreasureBaseData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag) // 1=更新，添加， 0=删除
Declare_Msg_Member(TreasureBaseData, m_sBaseData)	// 宝物基本信息
Declare_Msg_End

// 宝物合成请求
Declare_Msg_Begin(C2S_TREASURE_COMPOSE_REQ, MSG_C2S_TREASURE_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)	// 宝物信息ID
Declare_Msg_Member(WORD, m_wTreasureNum)	// 宝物数量
Declare_Msg_End

// 宝物合成响应
Declare_Msg_Begin(S2C_TREASURE_COMPOSE_RESP, MSG_S2C_TREASURE_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)	// 宝物信息ID
Declare_Msg_Member(WORD, m_wTreasureNum)	// 宝物数量
Declare_Msg_End

// 宝物精练
Declare_Msg_Begin(C2S_TREASURE_REFINE_REQ, MSG_C2S_TREASURE_REFINE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// 宝物UID
Declare_Msg_End

// 宝物精练响应
Declare_Msg_Begin(S2C_TREASURE_REFINE_RESP, MSG_S2C_TREASURE_REFINE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_DecodeMsg_UnsignedChar(m_byRefineLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUID)
Declare_EncodeMsg_UnsignedChar(m_byRefineLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// 宝物UID
Declare_Msg_Member(BYTE, m_byRefineLevel)	// 精炼等级
Declare_Msg_End

// 宝物强化
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
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// 宝物UID
Declare_Msg_Member(WORD, m_wCostItemNum)	// 消耗物品数量
Declare_Msg_Member_Array(DWORD, m_CostItemList, MAX_TREASURE_ENHANCE_COST_ITEM);
Declare_Msg_End

// 宝物强化响应
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
Declare_Msg_Member(DWORD, m_dwTreasureUID)	// 宝物UID
Declare_Msg_Member(WORD, m_wOldLevel)		// 旧的等级
Declare_Msg_Member(WORD, m_wNewLevel)		// 旧的等级
Declare_Msg_Member(DWORD, m_dwExp)			// 获得的经验
Declare_Msg_End

// 宝物穿戴请求
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
Declare_Msg_Member(BYTE, m_byFormationPos)	// 阵型位置
Declare_Msg_Member(BYTE, m_byTreasurePos)	// 宝物位置
Declare_Msg_Member(DWORD, m_dwUID)			// 宝物uid
Declare_Msg_End

// 宝物穿戴请求
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
Declare_Msg_Member(BYTE, m_byFormationPos)	// 阵型位置
Declare_Msg_Member(BYTE, m_byTreasurePos)	// 宝物位置
Declare_Msg_Member(DWORD, m_dwUID)			// 宝物uid
Declare_Msg_End

// 宝物熔炼请求
Declare_Msg_Begin(C2S_TREASURE_SMELT_REQ, MSG_C2S_TREASURE_SMELT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)			// 宝物uid
Declare_Msg_End

// 宝物熔炼响应
Declare_Msg_Begin(S2C_TREASURE_SMELT_RESP, MSG_S2C_TREASURE_SMELT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureInfoId)			// 宝物uid
Declare_Msg_End

// 宝物升品请求
Declare_Msg_Begin(C2S_TREASURE_QUALITY_UP_REQ, MSG_C2S_TREASURE_QUALITY_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTreasureUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTreasureUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTreasureUId)			// 宝物uid
Declare_Msg_End

// 宝物升品响应
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
Declare_Msg_Member(Treasure, m_tTreasure)			// 宝物uid
Declare_Msg_End


// 叛军数据请求
Declare_Msg_Begin(C2S_REBEL_DATA_REQ, MSG_C2S_REBEL_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 叛军数据请求响应
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
Declare_Msg_Member(ClientPlayerRebelData, m_RebelData)	 // 叛军数据
Declare_Msg_End


// 领取叛军功勋奖励请求
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
Declare_Msg_Member(WORD, m_wAwardNum)								// 奖励数量
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// 奖励列表
Declare_Msg_End

// 领取叛军功勋奖励响应
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
Declare_Msg_Member(WORD, m_wAwardNum)								// 奖励数量
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// 奖励列表
Declare_Msg_End

// 功勋奖励记录请求
Declare_Msg_Begin(C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ, MSG_C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 功勋奖励记录请求
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
Declare_Msg_Member(WORD, m_byAwardNum)								// 奖励数量
Declare_Msg_Member_Array(WORD, m_AwardList, MAX_EXPLOIT_AWARD_NUM);	// 奖励列表
Declare_Msg_End


// 叛军分享
Declare_Msg_Begin(C2S_REBEL_SHARE_REQ, MSG_C2S_REBEL_SHARE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 叛军分享响应
Declare_Msg_Begin(S2C_REBEL_SHARE_RESP, MSG_S2C_REBEL_SHARE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 发现叛军通知
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
Declare_Msg_Member(BYTE, m_byQaulity)		// 叛军品质
Declare_Msg_Member(WORD, m_wLevel)			// 叛军等级
Declare_Msg_Member(WORD, m_wMapId)			// 叛军等级
Declare_Msg_Member(DWORD, m_dwRebelId)		// 叛军Id
Declare_Msg_End

// 分解返还请求
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
Declare_Msg_Member(BYTE, m_byObjType)										// 分解类型
Declare_Msg_Member(WORD, m_byTargetNum)										// 分解目标数量
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// 分解目标列表
Declare_Msg_End


// 分解返还响应
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
Declare_Msg_Member(BYTE, m_byObjType)											// 分解类型
Declare_Msg_Member_Array(Item, m_ReturnObjectList, MAX_RECYCLE_RETURN_OBJ_NUM);	// 返还对象列表
Declare_Msg_Member(BYTE, m_byObjectNum)											// 返还对象列表数量
Declare_Msg_Member_Array(MoneyData, m_arMoneyList, MAX_MONEY_TYPE_NUM);			// 返还货币列表
Declare_Msg_Member(BYTE, m_byMoneyNum)											// 返还货币数量
Declare_Msg_End

// 重生返还请求
Declare_Msg_Begin(C2S_RECYCLE_RESET_RETURN_REQ, MSG_C2S_RECYCLE_RESET_RETURN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// 重生类型
Declare_Msg_Member(DWORD, m_dwTargetUId)	// 重生目标
Declare_Msg_End

// 重生返还响应
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
Declare_Msg_Member(BYTE, m_byObjType)											// 重生类型
Declare_Msg_Member(MoneyData, m_tMoney)											// 返还货币(todo::已经作废, 但是为了防止旧版本报错,暂时保留)
Declare_Msg_Member_Array(Item, m_ReturnObjectList, MAX_RECYCLE_RETURN_OBJ_NUM);	// 返还对象列表
Declare_Msg_Member(BYTE, m_byObjectNum)											// 返还对象列表数量
Declare_Msg_Member_Array(MoneyData, m_arMoneyList, MAX_MONEY_TYPE_NUM);			// 返还货币列表
Declare_Msg_Member(WORD, m_wMoneyNum)											// 返还货币数量
Declare_Msg_End


// 分解回收请求
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
Declare_Msg_Member(BYTE, m_byObjType)										// 分解类型
Declare_Msg_Member(WORD, m_byTargetNum)										// 分解目标数量
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// 分解目标列表
Declare_Msg_End

// 分解回收响应
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
Declare_Msg_Member(BYTE, m_byObjType)										// 分解类型
Declare_Msg_Member(BYTE, m_byTargetNum)										// 分解目标数量
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_RECYCLE_DECOMPOSE_NUM);	// 分解目标列表
Declare_Msg_End

// 重生回收请求
Declare_Msg_Begin(C2S_RECYCLE_RESET_REQ, MSG_C2S_RECYCLE_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// 重生类型
Declare_Msg_Member(DWORD, m_dwTargetUId)	// 重生目标
Declare_Msg_End

// 重生回收响应
Declare_Msg_Begin(S2C_RECYCLE_RESET_RESP, MSG_S2C_RECYCLE_RESET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byObjType)
Declare_DecodeMsg_UnsignedInt(m_dwTargetUId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byObjType)
Declare_EncodeMsg_UnsignedInt(m_dwTargetUId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byObjType)		// 重生类型
Declare_Msg_Member(DWORD, m_dwTargetUId)	// 重生目标
Declare_Msg_End



// 请求装备列表
Declare_Msg_Begin(C2S_EQUIPMENT_LIST_REQ, MSG_C2S_EQUIPMENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求装备列表响应
Declare_Msg_Begin(S2C_EQUIPMENT_LIST_RESP, MSG_S2C_EQUIPMENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_wEquipmentNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wEquipmentNum = min(m_wEquipmentNum, MAX_EQUIPMENT_NUM);
Declare_EncodeMsg_UnsignedShort(m_wEquipmentNum)
Declare_EncodeMsg_Memory(m_EquipmentList, m_wEquipmentNum * sizeof(EquipmentClientData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wEquipmentNum)	// 装备数量
Declare_Msg_Member_Array(EquipmentClientData, m_EquipmentList, MAX_EQUIPMENT_NUM);	// 装备列表
Declare_Msg_End


// 请求装备详情
Declare_Msg_Begin(C2S_EQUIPMENT_INFO_REQ, MSG_C2S_EQUIPMENT_INFO_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)
Declare_Msg_End

// 请求装备详情响应
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

// 装备信息通知(暂时不使用)
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
Declare_Msg_Member(EquipmentClientData, m_sEquipment)	// 装备信息
Declare_Msg_End


// 装备合成请求
Declare_Msg_Begin(C2S_EQUIPMENT_COMPOSE_REQ, MSG_C2S_EQUIPMENT_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwInfoId)
Declare_Msg_End

// 装备合成响应
Declare_Msg_Begin(S2C_EQUIPMENT_COMPOSE_RESP, MSG_S2C_EQUIPMENT_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwInfoId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwInfoId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwInfoId)
Declare_Msg_End

// 穿戴装备请求
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
Declare_Msg_Member(BYTE, m_byFormationPos)	// 阵型位置
Declare_Msg_Member(BYTE, m_byEquipmentPos)	// 装备位置
Declare_Msg_Member(DWORD, m_dwUID)			// 装备uid
Declare_Msg_End

// 穿戴装备请求
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
Declare_Msg_Member(BYTE, m_byFormationPos)	// 阵型位置
Declare_Msg_Member(BYTE, m_byEquipmentPos)	// 装备位置
Declare_Msg_Member(DWORD, m_dwUID)			// 装备uid
Declare_Msg_End



// 装备强化请求
Declare_Msg_Begin(C2S_EQUIPMENT_STRENG_REQ, MSG_C2S_EQUIPMENT_STRENG_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// 强化类型
Declare_Msg_Member(DWORD, m_dwUID)	// 装备UID	
Declare_Msg_End

// 装备强化响应
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
Declare_Msg_Member(BYTE, m_byType)				// 强化类型
Declare_Msg_Member(BYTE, m_byCritTimes)			// 暴击次数
Declare_Msg_Member(WORD, m_wStrengthenLevel)	// 强化等级
Declare_Msg_Member(DWORD, m_dwUID)				// 装备UID	
Declare_Msg_End

// 一键强化请求
Declare_Msg_Begin(C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ, MSG_C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_bySlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_bySlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_bySlot)	// 阵型槽位
Declare_Msg_End

// 一键强化响应
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
Declare_Msg_Member(BYTE, m_bySlot)			// 强化类型
Declare_Msg_Member(WORD, m_wEquipmentNum)	// 强化装备数量类
Declare_Msg_Member_Array(WORD, m_arOldEnhanceLevel, EQUIPMENT_TYPE_MAX); // 强化之前等级列表
Declare_Msg_Member_Array(WORD, m_arNewEnhanceLevel, EQUIPMENT_TYPE_MAX); // 强化之后等级列表
Declare_Msg_End

// 装备精练
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
Declare_Msg_Member(DWORD, m_dwEquipmentUID)	// 装备UID
Declare_Msg_Member(WORD, m_wCostListNum)		// 消耗列表数量
Declare_Msg_Member_Array(EquipmentRefineCostData, m_arCostList, MAX_EQUIPMENT_REFINE_COST_NUM); // 消耗列表
Declare_Msg_End

// 装备精练响应
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
Declare_Msg_Member(DWORD, m_dwEquipmentUID)		// 装备UID
Declare_Msg_Member(BYTE, m_byOldRefineLevel)	// 旧的精炼等级
Declare_Msg_Member(BYTE, m_byNewRefineLevel)	// 新的精炼等级
Declare_Msg_Member(DWORD, m_dwRefineExp)		// 精练经验
Declare_Msg_Member(DWORD, m_dwAddRefineExp)		// 本次添加的精练经验
Declare_Msg_End


// 装备升星请求
Declare_Msg_Begin(C2S_EQUIPMENT_STAR_UP_REQ, MSG_C2S_EQUIPMENT_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 装备UID	
Declare_Msg_End


// 装备升星响应
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
Declare_Msg_Member(BYTE, m_byCurStar)		// 当前星
Declare_Msg_Member(BYTE, m_byCurStarLevel)	// 当前星级
Declare_Msg_Member(DWORD, m_dwUID)	// 装备UID	
Declare_Msg_End


// 装备升品请求
Declare_Msg_Begin(C2S_EQUIPMENT_QUALITY_UP_REQ, MSG_C2S_EQUIPMENT_QUALITY_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 装备UID	
Declare_Msg_End

// 装备升品响应
Declare_Msg_Begin(S2C_EQUIPMENT_QUALITY_UP_RESP, MSG_S2C_EQUIPMENT_QUALITY_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 装备UID	
Declare_Msg_End

//机器人相关
//机器人登录请求
Declare_Msg_Begin(C2M_ROBOT_LOGIN_SYN, MSG_C2M_ROBOT_LOGIN_SYN)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szAccID, MAX_ACCOUNT_LEN)
	Declare_DecodeMsg_UnsignedInt(m_dwZoneID)
Declare_DecodeMsg_Function_End

Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szAccID, MAX_ACCOUNT_LEN)
	Declare_EncodeMsg_UnsignedInt(m_dwZoneID)
Declare_EncodeMsg_Function_End

Declare_Msg_Member_Array(char, m_szAccID, MAX_ACCOUNT_LEN + 1)				//机器人帐号ID		
Declare_Msg_Member(DWORD, m_dwZoneID)										//分区ID
Declare_Msg_End

//机器人登录应答
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
// 3) GateSvr和MapSvr之间消息注册
/*--------------------------------------------------------------------------------------------------------------------------*/
//角色断开连接通知
Declare_Msg_Begin( G2M_CLIENT_DISCONNECT_CMD, MSG_G2M_CLIENT_DISCONNECT_CMD )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End

Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//踢出玩家通知
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
// 4) MapSvr和OpenSvr之间消息注册
/*--------------------------------------------------------------------------------------------------------------------------*/

//M2O 服务器信息
Declare_Msg_Begin( M2O_SERVERID_CMD, MSG_M2O_SERVERID_CMD )
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwServerID)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwServerID)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwServerID)
Declare_Msg_End


//心跳包(open <--> map)
Declare_Msg_Begin( O2M_HEART_BEAT, MSG_O2M_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//心跳包(open <--> map)
Declare_Msg_Begin( M2O_HEART_BEAT, MSG_M2O_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//心跳包(gate <--> map)
Declare_Msg_Begin( G2M_HEART_BEAT, MSG_G2M_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//心跳包(map <--> gate)
Declare_Msg_Begin( M2G_HEART_BEAT, MSG_M2G_HEART_BEAT )
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//客户端信息
Declare_Msg_Begin(M2G_CLIENT_INFO, MSG_M2G_CLIENT_INFO)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wServerId)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wServerId)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wServerId)	//服务器ID
Declare_Msg_Member(DWORD, m_dwRoleId)	//角色ID
Declare_Msg_End


// 角色升级
Declare_Msg_Begin(S2C_ROLE_LEVEL_UP_NOTIFY, MSG_S2C_ROLE_LEVEL_UP_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//角色等级
Declare_Msg_End


// 逻辑通用消息 map --> open
Declare_Msg_Begin(M2O_LOGIC_COMMON_REQ, MSG_M2O_LOGIC_COMMON_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//角色等级
Declare_Msg_End

// 逻辑通用消息 map --> open
Declare_Msg_Begin(O2M_LOGIC_COMMON_RESP, MSG_O2M_LOGIC_COMMON_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)	//角色等级
Declare_Msg_End

// 认证码请求
Declare_Msg_Begin(M2O_AUTHENTICATION_CODE_REQ, MSG_M2O_AUTHENTICATION_CODE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szMobile, sizeof(m_szMobile)-1)
Declare_DecodeMsg_UnsignedInt(m_dwTemplId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szMobile, sizeof(m_szMobile))
Declare_EncodeMsg_UnsignedInt(m_dwTemplId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// 手机号
Declare_Msg_Member(DWORD, m_dwTemplId)	// 短信模板ID
Declare_Msg_End

// 认证码响应
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
Declare_Msg_Member_Array(char, m_szMobile, MAX_MOBILE_LEN)	// 手机号
Declare_Msg_Member(WORD, m_wCode)	// 认证码
Declare_Msg_Member(short, m_nErrorCode)	// 错误码(0=表示成功)
Declare_Msg_End

// 热云统计通知
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

// 阵型列表通知
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
Declare_Msg_Member(BYTE, m_byFormationNum)	// 阵型列表数量
Declare_Msg_Member_Array(Formation, m_FormactionList, MAX_BATTLE_HERO_NUM);	// 阵型列表
Declare_Msg_Member(BYTE, m_byReinforcementNum)	// 援军列表数量
Declare_Msg_Member_Array(DWORD, m_arReinforcementList, MAX_REINFORCEMENT_NUM); // 援军列表
Declare_Msg_End

// GM
Declare_Msg_Begin(C2M_GM_REQ, MSG_C2M_GM_REQ)
Declare_DecodeMsg_Function_Begin
	Declare_DecodeMsg_UnsignedShort(m_wCmd)				// 类型
	//Declare_DecodeMsg_UnsignedInt(m_dwTargetAccountId)	// 类型
	Declare_DecodeMsg_UnsignedInt(m_dwValue1)			// 类型
	Declare_DecodeMsg_UnsignedInt(m_dwValue2)			// 类型
	Declare_DecodeMsg_UnsignedInt(m_dwValue3)			// 类型
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
	Declare_EncodeMsg_UnsignedShort(m_wCmd)		
	//Declare_EncodeMsg_UnsignedInt(m_dwTargetAccountId)		
	Declare_EncodeMsg_UnsignedInt(m_dwValue1)		
	Declare_EncodeMsg_UnsignedInt(m_dwValue2)	
	Declare_EncodeMsg_UnsignedInt(m_dwValue3)
Declare_EncodeMsg_Function_End				
Declare_Msg_Member(WORD, m_wCmd)				// GM指令
//Declare_Msg_Member(DWORD, m_dwTargetAccountId)	// 调整的目标帐号ID
Declare_Msg_Member(DWORD, m_dwValue1)			// 调整值1
Declare_Msg_Member(DWORD, m_dwValue2)			// 调整值2
Declare_Msg_Member(DWORD, m_dwValue3)			// 调整值3
Declare_Msg_End

////////////////////////////////////////////////////////////////////////////////////////////////
// 好友模块

// 请求好友列表
Declare_Msg_Begin(C2S_FRIEND_LIST_REQ, MSG_C2S_FRIEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求好友列表响应
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
Declare_Msg_Member(BYTE, m_byGetEnergyTimes)	// 领取精力次数
Declare_Msg_Member(BYTE, m_byFriendNum)			// 好友数量
Declare_Msg_Member_Array(ClientFriendData, m_FriendList, MAX_FRIEND_NUM);	// 好友列表
Declare_Msg_End

// 请求黑名单列表
Declare_Msg_Begin(C2S_BLACK_LIST_REQ, MSG_C2S_BLACK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求黑名单列表响应
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
Declare_Msg_Member(BYTE, m_byBlackNum)	// 黑名单数量
Declare_Msg_Member_Array(ClientFriendData, m_BlackList, MAX_BLACK_NUM);	// 黑名单列表
Declare_Msg_End

// 请求申请列表
Declare_Msg_Begin(C2S_FRIEND_APPLY_LIST_REQ, MSG_C2S_FRIEND_APPLY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求申请列表响应
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
Declare_Msg_Member(BYTE, m_byApplyNum)	// 申请数量
Declare_Msg_Member_Array(ClientFriendData, m_ApplyList, MAX_FRIEND_APPLY_NUM);	// 申请列表
Declare_Msg_End

// 请求推荐列表
Declare_Msg_Begin(C2S_FRIEND_RECOMMEND_LIST_REQ, MSG_C2S_FRIEND_RECOMMEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 推荐列表响应
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
Declare_Msg_Member(BYTE, m_byRecommendNum)	// 推荐数量
Declare_Msg_Member_Array(ClientFriendData, m_RecommendList, MAX_FRIEND_RECOMMEND_NUM);	// 推荐列表
Declare_Msg_End

// 添加好友
Declare_Msg_Begin(C2S_FRIEND_ADD_REQ, MSG_C2S_FRIEND_ADD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 添加好友响应
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


// 删除好友
Declare_Msg_Begin(C2S_FRIEND_DEL_REQ, MSG_C2S_FRIEND_DEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 删除好友响应
Declare_Msg_Begin(S2C_FRIEND_DEL_RESP, MSG_S2C_FRIEND_DEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 申请好友回复
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

// 申请好友回复响应
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


// 添加黑名单
Declare_Msg_Begin(C2S_BLACK_ADD_REQ, MSG_C2S_BLACK_ADD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 添加黑名单响应
Declare_Msg_Begin(S2C_BLACK_ADD_RESP, MSG_S2C_BLACK_ADD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 删除黑名单
Declare_Msg_Begin(C2S_BLACK_DEL_REQ, MSG_C2S_BLACK_DEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 删除黑名单响应
Declare_Msg_Begin(S2C_BLACK_DEL_RESP, MSG_S2C_BLACK_DEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 赠送精力
Declare_Msg_Begin(C2S_FRIEND_ENERGY_GIVE_REQ, MSG_C2S_FRIEND_ENERGY_GIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 赠送精力响应
Declare_Msg_Begin(S2C_FRIEND_ENERGY_GIVE_RESP, MSG_S2C_FRIEND_ENERGY_GIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)
Declare_Msg_End

// 领取精力
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
Declare_Msg_Member(WORD, m_byTargetNum)			// 目标数量
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_FRIEND_NUM);	// 目标列表
Declare_Msg_End

// 领取精力响应
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
Declare_Msg_Member(WORD, m_byTargetNum)			// 目标数量
Declare_Msg_Member_Array(DWORD, m_TargetList, MAX_FRIEND_NUM);	// 目标列表
Declare_Msg_End


// 好友数据更新通知
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
Declare_Msg_Member(BYTE, m_byType)	// 类型(1=好友,2=黑友)
Declare_Msg_Member(ClientFriendData, m_FriendData)
Declare_Msg_End


// 查找目标
Declare_Msg_Begin(C2S_FRIEND_TARGET_FIND_REQ, MSG_C2S_FRIEND_TARGET_FIND_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szTargetName, sizeof(m_szTargetName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szTargetName, sizeof(m_szTargetName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szTargetName, MAX_ROLE_LEN);
Declare_Msg_End


// 查找目标
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
Declare_Msg_Member(PlayerCommonData, m_TargetData)	// 目标数据
Declare_Msg_End


//////////////////////////////////////////////////////////////////////////////
// 夺宝模块

// 夺宝数据请求
Declare_Msg_Begin(C2S_DUOBAO_DATA_REQ, MSG_C2S_DUOBAO_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 夺宝数据响应
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
Declare_Msg_Member(PlayerDuobaoInfo, m_DuobaoInfo)	// 夺宝数据
Declare_Msg_End


// 请求夺宝挑战目标列表
Declare_Msg_Begin(C2S_DUOBAO_TARGET_LIST_REQ, MSG_C2S_DUOBAO_TARGET_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_DecodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTreasureInfoId)
Declare_EncodeMsg_UnsignedShort(m_wTreasureItemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTreasureInfoId)		// 宝物信息ID
Declare_Msg_Member(WORD, m_wTreasureItemId)		// 宝物碎片ID
Declare_Msg_End

// 请求夺宝挑战目标列表响应
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
Declare_Msg_Member(WORD, m_wTreasureInfoId)		// 宝物信息ID
Declare_Msg_Member(WORD, m_wTreasureItemId)		// 宝物碎片ID
Declare_Msg_Member(BYTE, m_byPlayerNum)			// 玩家数量
Declare_Msg_Member_Array(DuobaoTargetPlayerData, m_PlayerList, DUOBAO_TARGET_NUM);
Declare_Msg_Member(BYTE, m_byMonsterNum)			// 怪物数量
Declare_Msg_Member_Array(DuobaoTargetMonsterData, m_MonsterList, DUOBAO_TARGET_NUM);
Declare_Msg_End

// 夺宝5次请求
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
Declare_Msg_Member(WORD, m_wTreasureInfoId)	// 宝物信息ID
Declare_Msg_Member(WORD, m_wTreasureItemId)	// 宝物碎片ID(碎片其实是物品)
Declare_Msg_Member(WORD, m_wDBTimes)	// 夺宝次数
Declare_Msg_End

// 夺宝5次响应
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
Declare_Msg_Member(vector<Item>, m_ItemList);	// 获得的物品列表
Declare_Msg_Member(BYTE, m_byFlag)				// 标志(1=获得，0=没有获得)
Declare_Msg_End


// 领取夺宝进度奖励请求
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
Declare_Msg_Member_Array(WORD, m_PregrossIdList, MAX_DOUBAO_PREGROSS_NUM);			// 进度ID列表
Declare_Msg_End


// 领取夺宝进度奖励响应
Declare_Msg_Begin(S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP, MSG_S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPreprossIdNum)
Declare_EncodeMsg_Memory(m_PregrossIdList, m_wPreprossIdNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPreprossIdNum)
Declare_Msg_Member_Array(WORD, m_PregrossIdList, MAX_DOUBAO_PREGROSS_NUM);			// 进度ID列表
Declare_Msg_End


// 玩家领地数据请求
Declare_Msg_Begin(C2S_PLAYER_LAND_DATA_REQ, MSG_C2S_PLAYER_LAND_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)			// 请求的目标玩家ID
Declare_Msg_End

// 玩家领地数据请求响应
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
Declare_Msg_Member(DWORD, m_dwTargetId)				// 请求的目标玩家ID
Declare_Msg_Member(BYTE, m_byRiotSuppressTimes)		// 暴动镇压次数
Declare_Msg_Member(DWORD, m_dwPatrolTotalTime)		// 巡逻总时间
Declare_Msg_Member(BYTE, m_byCityNum)			// 城池数量
Declare_Msg_Member_Array(LandCityBaseData, m_CityList, MAX_FRIEND_NUM);			// 城池列表
Declare_Msg_End

// 巡逻请求
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
Declare_Msg_Member(DWORD, m_dwPatrolHeroUID)	// 巡逻英雄uid
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID
Declare_Msg_Member(BYTE, m_byPatrolType)	// 巡逻类型
Declare_Msg_Member(BYTE, m_byTimeType)		// 时间类型(1=4小时,2=8小时,3=12小时)
Declare_Msg_End

// 巡逻响应
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
Declare_Msg_Member(DWORD, m_dwPatrolHeroUID)	// 巡逻英雄uid
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID
Declare_Msg_Member(BYTE, m_byPatrolType)	// 巡逻类型
Declare_Msg_Member(BYTE, m_byTimeType)		// 时间类型(1=4小时,2=8小时,3=12小时)
Declare_Msg_End

// 查看巡逻奖励
Declare_Msg_Begin(C2S_LAND_VIEW_PATROL_AWARD_REQ, MSG_C2S_LAND_VIEW_PATROL_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID
Declare_Msg_End

// 查看巡逻奖励响应
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
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID(0=表示查看所有)
Declare_Msg_Member(DWORD, m_dwGold)			// 奖励金币
Declare_Msg_Member(DWORD, m_dwDiamond)		// 奖励钻石
Declare_Msg_Member(DWORD, m_dwEnergy)		// 奖励精力
Declare_Msg_Member(DWORD, m_dwHeroSoul)		// 奖励雄魂
Declare_Msg_Member(DWORD, m_dwGodSoul)		// 奖励神魂
Declare_Msg_Member(BYTE, m_byAwardNum)		// 奖励数量
Declare_Msg_Member_Array(Item, m_AwardList, DUOBAO_TARGET_NUM);	// 奖励列表
Declare_Msg_End

// 领取巡逻奖励
Declare_Msg_Begin(C2S_LAND_GET_PATROL_AWARD_REQ, MSG_C2S_LAND_GET_PATROL_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID(0=表示所有)
Declare_Msg_End

// 领取巡逻奖励响应
Declare_Msg_Begin(S2C_LAND_GET_PATROL_AWARD_RESP, MSG_S2C_LAND_GET_PATROL_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// 领地城池ID(0=表示所有)
Declare_Msg_End

// 领地技能升级(开启,提升)
Declare_Msg_Begin(C2S_LAND_SKILL_LEVELUP_REQ, MSG_C2S_LAND_SKILL_LEVELUP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID
Declare_Msg_End

// 领地技能升级响应(开启,提升)
Declare_Msg_Begin(S2C_LAND_SKILL_LEVELUP_RESP, MSG_S2C_LAND_SKILL_LEVELUP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedChar(m_bySkillLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedChar(m_bySkillLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)			// 领地ID
Declare_Msg_Member(BYTE, m_bySkillLevel)	// 当前技能等级
Declare_Msg_End

// 领地镇压暴动
Declare_Msg_Begin(C2S_LAND_SUPPRESS_RIOT_REQ, MSG_C2S_LAND_SUPPRESS_RIOT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedInt(m_dwFriendId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedInt(m_dwFriendId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)				// 领地ID
Declare_Msg_Member(DWORD, m_dwFriendId)			// 好友ID
Declare_Msg_End

// 领地镇压暴动响应
Declare_Msg_Begin(S2C_LAND_SUPPRESS_RIOT_RESP, MSG_S2C_LAND_SUPPRESS_RIOT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCityId)
Declare_DecodeMsg_UnsignedInt(m_dwFriendId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCityId)
Declare_EncodeMsg_UnsignedInt(m_dwFriendId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCityId)				// 领地ID
Declare_Msg_Member(DWORD, m_dwFriendId)			// 好友ID
Declare_Msg_End

// 请求好友领地列表
Declare_Msg_Begin(C2S_FRIEND_LAND_LIST_REQ, MSG_C2S_FRIEND_LAND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求好友领地列表响应
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
Declare_Msg_Member(BYTE, m_byLandNum)		// 领地数量
Declare_Msg_Member_Array(FriendLandData, m_LandList, MAX_FRIEND_NUM);	// 领地列表
Declare_Msg_End

// 领地城池详情请求
Declare_Msg_Begin(C2S_LAND_CITY_DETAILS_REQ, MSG_C2S_LAND_CITY_DETAILS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwPlayerId)
Declare_DecodeMsg_UnsignedShort(m_dwCityId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwPlayerId)
Declare_EncodeMsg_UnsignedShort(m_dwCityId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwPlayerId)			// 玩家ID
Declare_Msg_Member(WORD, m_dwCityId)			// 城池ID
Declare_Msg_End

// 领地城池详情请求响应
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
Declare_Msg_Member(DWORD, m_dwPlayerId)			// 玩家ID
Declare_Msg_Member(LandCityData, m_tCityData)	// 城池数据
Declare_Msg_End

// 精灵数据请求
Declare_Msg_Begin(C2S_SPIRIT_DATA_REQ, MSG_C2S_SPIRIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 精灵数据响应
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
Declare_Msg_Member(PlayerSpriteInfo, m_tSpriteInfo)	 // 精灵信息
Declare_Msg_End


// 世界之心数据请求
Declare_Msg_Begin(C2S_HEART_OF_WORLD_DATA_REQ, MSG_C2S_HEART_OF_WORLD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 世界之心数据响应
Declare_Msg_Begin(S2C_HEART_OF_WORLD_DATA_RESP, MSG_S2C_HEART_OF_WORLD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStage)
Declare_DecodeMsg_UnsignedChar(m_byLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStage)
Declare_EncodeMsg_UnsignedChar(m_byLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStage)			// 阶段
Declare_Msg_Member(BYTE, m_byLevel)			// 等级
Declare_Msg_End

// 世界之心升级请求
Declare_Msg_Begin(C2S_HEART_OF_WORLD_LEVEL_UP_REQ, MSG_C2S_HEART_OF_WORLD_LEVEL_UP_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 世界之心升级响应
Declare_Msg_Begin(S2C_HEART_OF_WORLD_LEVEL_UP_RESP, MSG_S2C_HEART_OF_WORLD_LEVEL_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byStage)
Declare_DecodeMsg_UnsignedChar(m_byLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byStage)
Declare_EncodeMsg_UnsignedChar(m_byLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byStage)			// 阶段
Declare_Msg_Member(BYTE, m_byLevel)			// 等级
Declare_Msg_End


// 精灵强化请求
Declare_Msg_Begin(C2S_SPIRIT_ENHANCE_REQ, MSG_C2S_SPIRIT_ENHANCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// 精灵ID
Declare_Msg_End


// 精灵强化响应
Declare_Msg_Begin(S2C_SPIRIT_ENHANCE_RESP, MSG_S2C_SPIRIT_ENHANCE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// 精灵ID
Declare_Msg_End


// 精灵使用请求
Declare_Msg_Begin(C2S_SPIRIT_USE_REQ, MSG_C2S_SPIRIT_USE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// 精灵ID
Declare_Msg_End

// 精灵使用响应
Declare_Msg_Begin(S2C_SPIRIT_USE_RESP, MSG_S2C_SPIRIT_USE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritId)			// 精灵ID
Declare_Msg_End

// 精灵培养请求
Declare_Msg_Begin(C2S_SPIRIT_TRAIN_REQ, MSG_C2S_SPIRIT_TRAIN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_UnsignedShort(m_wTrainTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_UnsignedShort(m_wTrainTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_Member(WORD, m_wTrainTimes)			// 培养次数
Declare_Msg_End

// 精灵培养响应
Declare_Msg_Begin(S2C_SPIRIT_TRAIN_RESP, MSG_S2C_SPIRIT_TRAIN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_UnsignedShort(m_wTrainTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_UnsignedShort(m_wTrainTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_Member(WORD, m_wTrainTimes)			// 培养次数
Declare_Msg_End

// 精灵升星请求
Declare_Msg_Begin(C2S_SPRITE_STAR_UP_REQ, MSG_C2S_SPRITE_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_End

// 精灵升星响应
Declare_Msg_Begin(S2C_SPRITE_STAR_UP_RESP, MSG_S2C_SPRITE_STAR_UP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpriteId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpriteId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_End

// 精灵祝福请求
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
Declare_Msg_Member(WORD, m_wBlessType)			// 祝福类型
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵营位置
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_End


// 精灵祝福响应
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
Declare_Msg_Member(WORD, m_wBlessType)			// 祝福类型
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵营位置
Declare_Msg_Member(DWORD, m_dwSpriteId)			// 精灵ID
Declare_Msg_End


// 精灵合成请求
Declare_Msg_Begin(C2S_SPRITE_COMPOSE_REQ, MSG_C2S_SPRITE_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFragmentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFragmentId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFragmentId)			// 精灵碎片ID
Declare_Msg_End


// 精灵合成响应
Declare_Msg_Begin(S2C_SPRITE_COMPOSE_RESP, MSG_S2C_SPRITE_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFragmentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFragmentId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFragmentId)			// 精灵碎片ID
Declare_Msg_End


// 工会列表请求
Declare_Msg_Begin(C2S_GUILD_LIST_REQ, MSG_C2S_GUILD_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 工会列表请求响应
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
Declare_Msg_Member(BYTE, m_byGuildNum)		// 工会数量
Declare_Msg_Member_Array(GuildBaseData, m_GuildList, MAX_GUILD_NUM);	// 工会列表
Declare_Msg_End

// 创建工会
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

// 创建工会响应
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

// 工会数据请求
Declare_Msg_Begin(C2S_GUILD_DATE_REQ, MSG_C2S_GUILD_DATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 工会数据响应 
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
Declare_Msg_Member(BYTE, m_byPosition)		// 职位
Declare_Msg_Member(GuildData, m_GuildData)
Declare_Msg_Member(BYTE, m_byQYType)		// 泉涌类型
Declare_Msg_Member(BYTE, m_byMemberNum)		// 成员数据
Declare_Msg_Member_Array(char, m_szCaptainName, MAX_ROLE_LEN);	// 会长名称
Declare_Msg_Member(BYTE, m_byQSRewardRecordNum)		// 泉水奖励记录数量
Declare_Msg_Member_Array(WORD, m_arQSRewardRecord, MAX_GUILD_QS_REWARD_RECORD);	// 泉水奖励领取记录
Declare_Msg_Member(BYTE, m_byLastQYType) // 最近泉涌类型(0表示还木有泉涌)
Declare_Msg_Member(DWORD, m_dwLastQYValue)	// 最近泉涌值
Declare_Msg_Member_Array(char, m_szLastQYTargetName, MAX_ROLE_LEN);	// 最近泉涌目标名称
Declare_Msg_Member(WORD, m_wRedPointNum) // 红点数量
Declare_Msg_Member_Array(BYTE, m_arRedPointList, GUILD_RED_POINT_MAX);	// 红点列表
Declare_Msg_End

// 成员列表请求
Declare_Msg_Begin(C2S_GUILD_MEMBER_LIST_REQ, MSG_C2S_GUILD_MEMBER_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 成员列表响应
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
Declare_Msg_Member(BYTE, m_byMemberNum)		// 成员数量
Declare_Msg_Member_Array(ClientGuildMember, m_GuildList, MAX_FRIEND_NUM);	// 成员列表
Declare_Msg_End

// 留言列表请求
Declare_Msg_Begin(C2S_GUILD_LEAVEMSG_LIST_REQ, MSG_C2S_GUILD_LEAVEMSG_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 留言列表响应
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
Declare_Msg_Member(BYTE, m_byLeaveMsgNum)		// 留言消息数量
Declare_Msg_Member_Array(GuildLeaveMessage, m_LeaveMsgList, MAX_FRIEND_NUM);	// 留言消息列表
Declare_Msg_End

// 申请列表请求
Declare_Msg_Begin(C2S_GUILD_APPLY_LIST_REQ, MSG_C2S_GUILD_APPLY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 申请列表响应
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
Declare_Msg_Member(WORD, m_wApplyDataNum)		// 申请数据数量
Declare_Msg_Member_Array(PlayerCommonData, m_ApplyDataList, MAX_GUILD_APPLY_NUM);	// 申请数据列表
Declare_Msg_End


// 解散工会
Declare_Msg_Begin(C2S_GUILD_DISMISS_REQ, MSG_C2S_GUILD_DISMISS_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 解散工会响应
Declare_Msg_Begin(S2C_GUILD_DISMISS_RESP, MSG_S2C_GUILD_DISMISS_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 申请加入工会
Declare_Msg_Begin(C2S_GUILD_APPLY_REQ, MSG_C2S_GUILD_APPLY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)
Declare_Msg_End

// 申请加入工会响应
Declare_Msg_Begin(S2C_GUILD_APPLY_RESP, MSG_S2C_GUILD_APPLY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)
Declare_Msg_End

// 审核申请
Declare_Msg_Begin(C2S_GUILD_APPLY_CHECK_REQ, MSG_C2S_GUILD_APPLY_CHECK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_UnsignedChar(m_byIsAccept)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedChar(m_byIsAccept)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 审核的目标ID
Declare_Msg_Member(BYTE, m_byIsAccept)	// 是否接受
Declare_Msg_End

// 审核申请响应
Declare_Msg_Begin(S2C_GUILD_APPLY_CHECK_RESP, MSG_S2C_GUILD_APPLY_CHECK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_UnsignedChar(m_byIsAccept)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 审核的目标ID
Declare_Msg_Member(BYTE, m_byIsAccept)	// 是否接受
Declare_Msg_End

// 取消申请
Declare_Msg_Begin(C2S_GUILD_APPLY_CANCEL_REQ, MSG_C2S_GUILD_APPLY_CANCEL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// 取消的工会ID
Declare_Msg_End

// 取消申请响应
Declare_Msg_Begin(S2C_GUILD_APPLY_CANCEL_RESP, MSG_S2C_GUILD_APPLY_CANCEL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// 取消的工会ID
Declare_Msg_End

// 踢出工会
Declare_Msg_Begin(C2S_GUILD_KICKOUT_REQ, MSG_C2S_GUILD_KICKOUT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 踢出的目标ID
Declare_Msg_End

// 踢出工会响应
Declare_Msg_Begin(S2C_GUILD_KICKOUT_RESP, MSG_S2C_GUILD_KICKOUT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 踢出的目标ID
Declare_Msg_End

// 退出工会
Declare_Msg_Begin(C2S_GUILD_LEAVE_REQ, MSG_C2S_GUILD_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 退出工会响应
Declare_Msg_Begin(S2C_GUILD_LEAVE_RESP, MSG_S2C_GUILD_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 留言
Declare_Msg_Begin(C2S_GUILD_LEAVE_MESSAGE_REQ, MSG_C2S_GUILD_LEAVE_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szLeaveMsg, sizeof(m_szLeaveMsg)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szLeaveMsg, sizeof(m_szLeaveMsg));
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szLeaveMsg, MAX_GUILD_LEAVEMESSAGE_LEN);
Declare_Msg_End

// 留言响应
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
Declare_Msg_Member(GuildLeaveMessage, m_LeaveMsg)	// 留言内容
Declare_Msg_End

// 搜索工会
Declare_Msg_Begin(C2S_GUILD_SEARCH_REQ, MSG_C2S_GUILD_SEARCH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGuildId)
Declare_DecodeMsg_String(m_szGuildName, sizeof(m_szGuildName)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGuildId)
Declare_EncodeMsg_String(m_szGuildName, sizeof(m_szGuildName));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGuildId)	// 工会ID
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN);
Declare_Msg_End

// 搜索工会响应
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
Declare_Msg_Member(WORD, m_wGuildNum)	// 工会数量
Declare_Msg_Member_Array(GuildBaseData, m_GuildList, MAX_SEARCH_GUILD_NUM);
Declare_Msg_End

// 泉涌
Declare_Msg_Begin(C2S_GUILD_QY_REQ, MSG_C2S_GUILD_QY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_End


// 泉涌响应
Declare_Msg_Begin(S2C_GUILD_QY_RESP, MSG_S2C_GUILD_QY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byType)
Declare_EncodeMsg_UnsignedInt(m_dwExp)
Declare_EncodeMsg_UnsignedInt(m_dwContribution)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_Member(DWORD, m_dwExp)	// 获得的工会经验
Declare_Msg_Member(DWORD, m_dwContribution)	// 获得的建设值
Declare_Msg_End

// 领取泉水奖励
Declare_Msg_Begin(C2S_GUILD_QS_AWARD_REQ, MSG_C2S_GUILD_QS_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wQuanShui)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wQuanShui)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wQuanShui)	// 泉水
Declare_Msg_End

// 领取泉水奖励响应
Declare_Msg_Begin(S2C_GUILD_QS_AWARD_RESP, MSG_S2C_GUILD_QS_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wQuanShui)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wQuanShui)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wQuanShui)	// 泉水
Declare_Msg_End


// 修改工会信息请求
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
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szText, MAX_GUILD_TEXT_NUM);
Declare_Msg_End


// 修改工会信息响应
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
Declare_Msg_Member(BYTE, m_byType)	// 类型
Declare_Msg_Member(WORD, m_wIconId)
Declare_Msg_Member_Array(char, m_szText, MAX_GUILD_TEXT_NUM);
Declare_Msg_End

// 工会事件列表请求
Declare_Msg_Begin(C2S_GUILD_EVENT_LIST_REQ, MSG_C2S_GUILD_EVENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 工会事件列表响应
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
Declare_Msg_Member(BYTE, m_byEventNum)		// 事件数量上限
Declare_Msg_Member_Array(GuildEvent, m_EventList, MAX_GUILD_EVENT_NUM);	// 事件列表
Declare_Msg_End

// 技能升级请求
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
Declare_Msg_Member(BYTE, m_byType)		// 升级的类型(1=工会,2=个人)
Declare_Msg_Member(WORD, m_wSkillId)	// 技能ID
Declare_Msg_Member(WORD, m_wSkillLevel)	// 技能等级
Declare_Msg_End

// 技能升级响应
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
Declare_Msg_Member(BYTE, m_byType)		// 升级的类型(1=工会,2=个人)
Declare_Msg_Member(WORD, m_wSkillId)	// 技能ID
Declare_Msg_Member(WORD, m_wSkillLevel)	// 技能等级
Declare_Msg_End

// 工会技能列表请求
Declare_Msg_Begin(C2S_GUILD_SKILL_LIST_REQ, MSG_C2S_GUILD_SKILL_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 工会技能列表响应
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
Declare_Msg_Member(BYTE, m_bySelfSkillNum)		// 自己的技能数量
Declare_Msg_Member_Array(SkillData, m_SelfSkillList, MAX_GUILD_SKILL_NUM);	// 自己的技能列表
Declare_Msg_Member(BYTE, m_byGuildSkillNum)		// 工会技能数量
Declare_Msg_Member_Array(SkillData, m_GuildSkillList, MAX_GUILD_SKILL_NUM);	// 工会技能列表
Declare_Msg_End


// 军团副本数据请求
Declare_Msg_Begin(C2S_GUILD_FB_DATA_REQ, MSG_C2S_GUILD_FB_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 军团副本数据响应
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
Declare_Msg_Member(DWORD, m_dwTotalHP)	// 总的HP
Declare_Msg_Member(DWORD, m_dwDamage)	// 伤害
Declare_Msg_Member(GuildFBData, m_FBData) // 副本数据
Declare_Msg_End

// 军团副本章节奖励请求
Declare_Msg_Begin(C2S_GUILD_FB_CHAPTER_REWARD_REQ, MSG_C2S_GUILD_FB_CHAPTER_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 领取的章节
Declare_Msg_End

// 军团副本章节奖励响应
Declare_Msg_Begin(S2C_GUILD_FB_CHAPTER_REWARD_RESP, MSG_S2C_GUILD_FB_CHAPTER_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 领取的章节
Declare_Msg_End

// 军团副本关卡奖励请求
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
Declare_Msg_Member(WORD, m_wChapter)	// 领取的章节
Declare_Msg_Member(BYTE, m_byCamp)		// 领取的阵营
Declare_Msg_Member(BYTE, m_byBoxIdx)	// 领取的宝箱索引(从0开始)
Declare_Msg_End

// 军团副本关卡奖励响应
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
Declare_Msg_Member(WORD, m_wChapter)	// 领取的章节
Declare_Msg_Member(BYTE, m_byCamp)		// 领取的阵营
Declare_Msg_Member(BYTE, m_byBoxIdx)	// 领取的宝箱索引(从0开始,在界面的位置)
Declare_Msg_Member(BYTE, m_byRewardIdx)	// 领取的奖励索引(从0开始,在表里的位置)
Declare_Msg_End

// 军团副本关卡奖励记录请求
Declare_Msg_Begin(C2S_GUILD_FB_MAP_REWARD_RECORD_REQ, MSG_C2S_GUILD_FB_MAP_REWARD_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Short(m_wChapter)
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Short(m_wChapter)
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 请求的章节
Declare_Msg_Member(BYTE, m_byCamp)		// 请求的阵营
Declare_Msg_End

// 军团副本关卡奖励记录响应
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
Declare_Msg_Member(WORD, m_wChapter)	// 请求的章节
Declare_Msg_Member(BYTE, m_byCamp)		// 请求的阵营
Declare_Msg_Member(WORD, m_wRecordNum)		// 记录数量
Declare_Msg_Member_Array(GuildFBBox, m_RecordList, MAX_GUILD_MEMBER_NUM);	// 记录列表
Declare_Msg_Member_Array(BYTE, m_RewardStatusList, CAMP_NUM);	// 关卡奖励状态列表(0=未通关,1=未领取,2=已领取) todo::暂时不使用
Declare_Msg_End


// 军团副本伤害记录请求
Declare_Msg_Begin(C2S_GUILD_FB_DAMAGE_RECORD_REQ, MSG_C2S_GUILD_FB_DAMAGE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 军团副本伤害记录响应
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
Declare_Msg_Member(WORD, m_wDamageNum)		// 伤害数量
Declare_Msg_Member_Array(GuildFBDamageData, m_DamageList, MAX_GUILD_MEMBER_NUM);	// 目标列表
Declare_Msg_End

// 军团副本挑战次数购买
Declare_Msg_Begin(C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ, MSG_C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// 购买次数
Declare_Msg_End

// 军团副本挑战次数购买响应
Declare_Msg_Begin(S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP, MSG_S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// 购买次数
Declare_Msg_End

// 弹劾请求
Declare_Msg_Begin(C2S_GUILD_IMPEACH_REQ, MSG_C2S_GUILD_IMPEACH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 弹劾响应
Declare_Msg_Begin(S2C_GUILD_IMPEACH_RESP, MSG_S2C_GUILD_IMPEACH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 弹劾通知
Declare_Msg_Begin(S2C_GUILD_IMPEACH_NOTICE, MSG_S2C_GUILD_IMPEACH_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwNewCaptainId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwNewCaptainId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwNewCaptainId)		// 新队长ID
Declare_Msg_End

// 工会关卡怪物数据请求
Declare_Msg_Begin(C2S_GUILD_MONSTER_DATA_REQ, MSG_C2S_GUILD_MONSTER_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 工会关卡怪物数据响应
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
Declare_Msg_Member(WORD, m_wCampNum)			// 阵营数量
Declare_Msg_Member_Array(vector<MonsterData>, m_MonsterList, CAMP_NUM);	// 怪物列表
Declare_Msg_Member_Array(string, m_KillerList, CAMP_NUM);	// 击杀者列表
//Declare_Msg_Member_Array(BYTE, m_RewardStatusList, CAMP_NUM);	// 关卡奖励状态列表(0=未通关,1=未领取,2=已领取)
Declare_Msg_End

// 工会副本章节重置
Declare_Msg_Begin(C2S_GUILD_FB_CHAPTER_RESET_REQ, MSG_C2S_GUILD_FB_CHAPTER_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsHistoryRecord)		// 是否重置到历史记录
Declare_Msg_End


// 工会副本章节重置响应
Declare_Msg_Begin(S2C_GUILD_FB_CHAPTER_RESET_RESP, MSG_S2C_GUILD_FB_CHAPTER_RESET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsHistoryRecord)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsHistoryRecord)		// 是否重置到历史记录
Declare_Msg_End

// 工会怪物伤害通知
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
Declare_Msg_Member(WORD, m_wChapter)		// 章节
Declare_Msg_Member(BYTE, m_byCamp)		// 阵营
Declare_Msg_Member(DWORD, m_dwDamage)		// 伤害
Declare_Msg_Member(DWORD, m_dwCurTotalHP)	// 当前总HP
Declare_Msg_Member_Array(char, m_szAttackName, MAX_ROLE_LEN); // 攻击者名称
Declare_Msg_End


// 我的关卡奖励记录请求
Declare_Msg_Begin(C2S_GUILD_MY_MAP_REWARD_RECORD_REQ, MSG_C2S_GUILD_MY_MAP_REWARD_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 我的关卡奖励记录响应
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
Declare_Msg_Member(WORD, m_wRecordNum)		// 记录数量
Declare_Msg_Member_Array(MyGuildMapRewardRecord, m_RewardRecordList, MAX_GUILD_CHAPTER_NUM);
Declare_Msg_End




///////////////////////////////////////////////////////////////////////////////////////
// 竞技场模块

// 竞技场挑战目标列表请求
Declare_Msg_Begin(C2S_ARENA_TARGET_LIST_REQ, MSG_C2S_ARENA_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 竞技场挑战目标列表响应
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
Declare_Msg_Member(WORD, m_wRank)				// 自己的排行
Declare_Msg_Member(WORD, m_wHistoryRank)		// 历史排名
Declare_Msg_Member(BYTE, m_byTargetNum)			// 目标数量
Declare_Msg_Member_Array(ArenaTargetData, m_TargetList, MAX_ARENA_TARGET_NUM);	// 目标列表
Declare_Msg_End

// 竞技场战报请求
Declare_Msg_Begin(C2S_ARENA_BATTLE_RECORD_REQ, MSG_C2S_ARENA_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 竞技场战报响应
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
Declare_Msg_Member(BYTE, m_byRecordNum)		// 记录数量
Declare_Msg_Member_Array(ArenaBattleRecord, m_RecordList, MAX_ARENA_BATTLE_RECORD);	// 记录列表
Declare_Msg_End

// 任命
Declare_Msg_Begin(C2S_GUILD_APPOINT_REQ, MSG_C2S_GUILD_APPOINT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPos)		// 任命的职位
Declare_Msg_Member(DWORD, m_dwTargetId)	// 任命目标ID
Declare_Msg_End

// 任命响应
Declare_Msg_Begin(S2C_GUILD_APPOINT_RESP, MSG_S2C_GUILD_APPOINT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPos)
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPos)
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPos)		// 任命的职位
Declare_Msg_Member(DWORD, m_dwTargetId)	// 任命目标ID
Declare_Msg_End


// 日常任务列表请求
Declare_Msg_Begin(C2S_DAILY_MISSION_LIST_REQ, MSG_C2S_DAILY_MISSION_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 日常任务列表请求响应
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
Declare_Msg_Member(PlayerDailyMissionData, m_DailyMission)	// 日常任务数据
Declare_Msg_End

// 日常任务数据通知
Declare_Msg_Begin(S2C_DAILY_MISSION_DATA_NOTIFY, MSG_S2C_DAILY_MISSION_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_DailyMissionData, sizeof(m_DailyMissionData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_DailyMissionData, sizeof(m_DailyMissionData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DailyMissionData, m_DailyMissionData)	// 日常任务数据
Declare_Msg_End

// 日常任务奖励领取请求
Declare_Msg_Begin(C2S_DAILY_MISSION_AWARD_REQ, MSG_C2S_DAILY_MISSION_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMissionId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMissionId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMissionId)	// 日常任务Id
Declare_Msg_End


// 日常任务奖励领取响应
Declare_Msg_Begin(S2C_DAILY_MISSION_AWARD_RESP, MSG_S2C_DAILY_MISSION_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMissionId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMissionId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMissionId)	// 日常任务Id
Declare_Msg_End

// 日常任务积分奖励领取请求
Declare_Msg_Begin(C2S_DAILY_MISSION_POINT_AWARD_REQ, MSG_C2S_DAILY_MISSION_POINT_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPoint);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPoint);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPoint)	// 领取的积分奖励
Declare_Msg_End

// 日常任务积分奖励领取请求
Declare_Msg_Begin(S2C_DAILY_MISSION_POINT_AWARD_RESP, MSG_S2C_DAILY_MISSION_POINT_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPoint);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPoint);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPoint)	// 领取的积分奖励
Declare_Msg_End

// 成就列表请求
Declare_Msg_Begin(C2S_ACHIEVEMENT_LIST_REQ, MSG_C2S_ACHIEVEMENT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 成就列表响应
Declare_Msg_Begin(S2C_ACHIEVEMENT_LIST_RESP, MSG_S2C_ACHIEVEMENT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementNum)
Declare_EncodeMsg_Memory(m_AhievementList, m_wAchievementNum*sizeof(AchievementData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementNum)		// 成就数量
Declare_Msg_Member_Array(AchievementData, m_AhievementList, MAX_ACHIEVEMENT_NUM);	// 成就列表
Declare_Msg_End


// 成就数据通知
Declare_Msg_Begin(S2C_ACHIEVEMENT_DATA_NOTIFY, MSG_S2C_ACHIEVEMENT_DATA_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tAchievementData, sizeof(AchievementData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tAchievementData, sizeof(AchievementData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(AchievementData, m_tAchievementData)		// 成就数据
Declare_Msg_End

// 成就奖励领取请求
Declare_Msg_Begin(C2S_ACHIEVEMENT_AWARD_REQ, MSG_C2S_ACHIEVEMENT_AWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementId)		// 成就ID
Declare_Msg_End

// 成就奖励领取响应
Declare_Msg_Begin(S2C_ACHIEVEMENT_AWARD_RESP, MSG_S2C_ACHIEVEMENT_AWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wAchievementId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wAchievementId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wAchievementId)		// 成就ID
Declare_Msg_End


// 世界BOSS数据请求
Declare_Msg_Begin(C2S_WORLD_BOSS_DATA_REQ, MSG_C2S_WORLD_BOSS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 世界BOSS数据响应
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
Declare_Msg_Member(WORD, m_wBossLevel)				// BOSS等级
Declare_Msg_Member(BYTE, m_byCamp)					// 所属阵营
Declare_Msg_Member(BYTE, m_byBattleTimes)			// 挑战次数(已经使用的)
Declare_Msg_Member(DWORD, m_dwLastRecoveryTime)		// 上次恢复时间(挑战次数)
Declare_Msg_Member(BYTE, m_byBuyBattleTimes)		// 已经购买的挑战次数
Declare_Msg_Member(DWORD, m_dwMyHonor)				// 我的总荣誉
Declare_Msg_Member(DWORD, m_dwMyDamage)				// 我的总伤害
Declare_Msg_Member(WORD, m_wMyHonorRank)			// 我的总荣誉排名
Declare_Msg_Member(WORD, m_wMyDamageRank)			// 我的总伤害排名
Declare_Msg_Member(BYTE, m_byHonorRankNum)			// 荣誉排行数量
Declare_Msg_Member(DWORD, m_dwReliveTime)			// BOSS复活时间(0=表示还木有挂掉)
Declare_Msg_Member(DWORD, m_dwRemainHP)				// BOSS剩余血量
Declare_Msg_Member_Array(ClientRoleRankData, m_HonorRankList, CAMP_NUM);	// 荣誉排行列表
Declare_Msg_End

// 选择阵营请求
Declare_Msg_Begin(C2S_WORLD_BOSS_SELECT_CAMP_REQ, MSG_C2S_WORLD_BOSS_SELECT_CAMP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// 选择的阵营
Declare_Msg_End

// 选择阵营响应
Declare_Msg_Begin(S2C_WORLD_BOSS_SELECT_CAMP_RESP, MSG_S2C_WORLD_BOSS_SELECT_CAMP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// 选择的阵营
Declare_Msg_End


// 领取奖励请求
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
Declare_Msg_Member(BYTE, m_byRewardType)	// 奖励类型(1=荣誉,2=伤害)
Declare_Msg_Member(WORD, m_byRewardNum)	// 奖励数量
Declare_Msg_Member_Array(WORD, m_RewardList, MAX_WORLD_BOSS_AWARD_NUM);	// 奖励列表
Declare_Msg_End

// 领取奖励响应
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
Declare_Msg_Member(BYTE, m_byRewardType)	// 奖励类型(1=荣誉,2=伤害)
Declare_Msg_Member(BYTE, m_byRewardNum)		// 奖励数量
Declare_Msg_Member_Array(WORD, m_RewardList, MAX_WORLD_BOSS_AWARD_NUM);	// 奖励列表
Declare_Msg_End

// 领取奖励记录请求
Declare_Msg_Begin(C2S_WORLD_BOSS_REWARD_RECORD_REQ, MSG_C2S_WORLD_BOSS_REWARD_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRewardType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)		// 奖励类型(1=荣誉,2=伤害)
Declare_Msg_End

// 领取奖励记录响应
Declare_Msg_Begin(S2C_WORLD_BOSS_REWARD_RECORD_RESP, MSG_S2C_WORLD_BOSS_REWARD_RECORD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRewardType)
Declare_EncodeMsg_UnsignedShort(m_byRecordNum)
Declare_EncodeMsg_Memory(m_RecordList, m_byRecordNum*sizeof(WORD))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardType)	// 奖励类型(1=荣誉,2=伤害)
Declare_Msg_Member(BYTE, m_byRecordNum)		// 记录数量
Declare_Msg_Member_Array(WORD, m_RecordList, MAX_WORLD_BOSS_AWARD_NUM);	// 记录列表
Declare_Msg_End


// 世界BOSS战报请求
Declare_Msg_Begin(C2S_WORLD_BOSS_BATTLE_RECORD_REQ, MSG_C2S_WORLD_BOSS_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 世界BOSS战报响应
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
Declare_Msg_Member(BYTE, m_byRecordNum)		// 记录数量
Declare_Msg_Member_Array(WorldBossBattleRecord, m_RecordList, MAX_WORLD_BOSS_BATTLE_RECORD_NUM);	// 记录列表
Declare_Msg_End

// 世界BOSS挑战次数购买
Declare_Msg_Begin(C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ, MSG_C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// 购买次数
Declare_Msg_End

// 世界BOSS挑战次数购买响应
Declare_Msg_Begin(S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP, MSG_S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byBuyTimes)		// 购买次数
Declare_Msg_End

// 世界boss伤害公告
Declare_Msg_Begin(S2C_WORLD_BOSS_DAMAGE_NOTIFY, MSG_S2C_WORLD_BOSS_DAMAGE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwDamage)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwDamage)
Declare_EncodeMsg_UnsignedInt(m_dwCurTotalHP)
Declare_EncodeMsg_String(m_szAttackName, sizeof(m_szAttackName)-1);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwDamage)		// 伤害
Declare_Msg_Member(DWORD, m_dwCurTotalHP)	// 当前总HP
Declare_Msg_Member_Array(char, m_szAttackName, MAX_ROLE_LEN); // 攻击者名称
Declare_Msg_End

// 世界BOSS活动开始
Declare_Msg_Begin(S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY, MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 世界BOSS活动结束
Declare_Msg_Begin(S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY, MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// 奖励列表请求
Declare_Msg_Begin(C2S_REWARD_CENTER_LIST_REQ, MSG_C2S_REWARD_CENTER_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 奖励列表响应
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
Declare_Msg_Member(BYTE, m_byRewardNum)		// 奖励数量
Declare_Msg_Member_Array(ClientRewardData, m_RewardList, MAX_REWARD_CENTER_REWARD_NUM);	// 奖励列表
Declare_Msg_End

// 奖励领取请求
Declare_Msg_Begin(C2S_REWARD_CENTER_GET_REQ, MSG_C2S_REWARD_CENTER_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_byRewardNum)
m_byRewardNum = min(m_byRewardNum, MAX_REWARD_CENTER_REWARD_NUM);
Declare_DecodeMsg_Memory(m_arRewardIdList, m_byRewardNum*sizeof(DWORD));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_byRewardNum)		// 奖励数量
Declare_Msg_Member_Array(DWORD, m_arRewardIdList, MAX_REWARD_CENTER_REWARD_NUM);	// 奖励列表
Declare_Msg_End

// 奖励领取响应
Declare_Msg_Begin(S2C_REWARD_CENTER_GET_RESP, MSG_S2C_REWARD_CENTER_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRewardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_byRewardNum = min(m_byRewardNum, MAX_REWARD_CENTER_REWARD_NUM);
Declare_EncodeMsg_UnsignedShort(m_byRewardNum)
Declare_EncodeMsg_Memory(m_arRewardIdList, m_byRewardNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRewardNum)		// 奖励数量
Declare_Msg_Member_Array(DWORD, m_arRewardIdList, MAX_REWARD_CENTER_REWARD_NUM);	// 奖励列表
Declare_Msg_End

// 领奖中心奖励通知
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
Declare_Msg_Member(ClientRewardData, m_RewardData)		// 奖励数量
Declare_Msg_End

// 签到数据请求
Declare_Msg_Begin(C2S_SIGN_IN_DATA_REQ, MSG_C2S_SIGN_IN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 签到数据响应
Declare_Msg_Begin(S2C_SIGN_IN_DATA_RESP, MSG_S2C_SIGN_IN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCurSignInDay)
Declare_DecodeMsg_UnsignedInt(m_dwLastSignInTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCurSignInDay)
Declare_EncodeMsg_UnsignedInt(m_dwLastSignInTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCurSignInDay)		// 当前签到的天数
Declare_Msg_Member(DWORD, m_dwLastSignInTime)	// 最近签到的时间
Declare_Msg_End

// 签到请求
Declare_Msg_Begin(C2S_SIGN_IN_REQ, MSG_C2S_SIGN_IN_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 签到响应
Declare_Msg_Begin(S2C_SIGN_IN_RESP, MSG_S2C_SIGN_IN_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 豪华签到数据请求
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
Declare_Msg_Member(WORD, m_wSignInDataNum)	// 签到数据数量
Declare_Msg_Member_Array(LuxurySignData, m_arSignInDataLList, MAX_LUXURY_SIGN_NUM);	// 签到数据列表
Declare_Msg_End


Declare_Msg_Begin(C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ,MSG_C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSignInId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSignInId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSignInId)		// 签到ID
Declare_Msg_End

Declare_Msg_Begin(S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP, MSG_S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSignInId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSignInId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSignInId)		// 签到ID
Declare_Msg_End



// 迎财神数据请求
Declare_Msg_Begin(C2S_YCS_DATA_REQ, MSG_C2S_YCS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 迎财神数据响应
Declare_Msg_Begin(S2C_YCS_DATA_RESP, MSG_S2C_YCS_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tData, sizeof(YCSData))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tData, sizeof(YCSData))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(YCSData, m_tData)	// 迎财神数据
Declare_Msg_End

// 迎财神奖励请求
Declare_Msg_Begin(C2S_YCS_REWARD_REQ, MSG_C2S_YCS_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 迎财神奖励响应
Declare_Msg_Begin(S2C_YCS_REWARD_RESP, MSG_S2C_YCS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wType)
Declare_DecodeMsg_UnsignedInt(m_dwValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wType)
Declare_EncodeMsg_UnsignedInt(m_dwValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wType)		// 货币类型
Declare_Msg_Member(DWORD, m_dwValue)	// 货币值
Declare_Msg_End

// 铜雀台数据请求
Declare_Msg_Begin(C2S_TQT_DATA_REQ, MSG_C2S_TQT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 铜雀台数据响应
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

// 铜雀台奖励请求
Declare_Msg_Begin(C2S_TQT_REWARD_REQ, MSG_C2S_TQT_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 铜雀台奖励响应
Declare_Msg_Begin(S2C_TQT_REWARD_RESP, MSG_S2C_TQT_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求7天目标列表请求
Declare_Msg_Begin(C2S_SEVEN_DAY_TARGET_LIST_REQ, MSG_C2S_SEVEN_DAY_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求7天目标列表响应
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
Declare_Msg_Member(BYTE, m_byTargetNum)		// 目标数量
Declare_Msg_Member_Array(SevenDayTargetData, m_arTargetList, MAX_SEVEN_DAY_TARGET_NUM);	// 目标列表
Declare_Msg_Member(BYTE, m_byRewardRecordNum)		// 奖励记录数量
Declare_Msg_Member_Array(WORD, m_arRewardRecordList, MAX_SEVEN_DAY_COMPLETION_REWARD);	// 奖励记录列表
Declare_Msg_End


// 领取7天目标奖励请求
Declare_Msg_Begin(C2S_SEVEN_DAY_GET_REWARD_REQ, MSG_C2S_SEVEN_DAY_GET_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTargetId)		// 目标ID
Declare_Msg_End

// 领取奖励响应
Declare_Msg_Begin(S2C_SEVEN_DAY_GET_REWARD_RESP, MSG_S2C_SEVEN_DAY_GET_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTargetId)		// 目标ID
Declare_Msg_End


// 7天目标数据通知
Declare_Msg_Begin(S2C_SEVEN_DAY_TARGET_DATA_NOTICE, MSG_S2C_SEVEN_DAY_TARGET_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tTargetData, sizeof(SevenDayTargetData));
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tTargetData, sizeof(SevenDayTargetData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SevenDayTargetData, m_tTargetData)		// 目标数据
Declare_Msg_End


// 领取完成度奖励请求
Declare_Msg_Begin(C2S_SEVEN_DAY_COMPLETION_REWARD_REQ, MSG_C2S_SEVEN_DAY_COMPLETION_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wComplecton_num)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wComplecton_num)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wComplecton_num)		// 完成度数量
Declare_Msg_End

// 领取完成度奖励响应
Declare_Msg_Begin(S2C_SEVEN_DAY_COMPLETION_REWARD_RESP, MSG_S2C_SEVEN_DAY_COMPLETION_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wComplecton_num)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wComplecton_num)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wComplecton_num)		// 完成度数量
Declare_Msg_End


//////////////////////////////////////////////////////
/// 仙灵
// 玩家仙灵数据请求
Declare_Msg_Begin(C2S_PLAYER_ZHANBU_DATA_REQ, MSG_C2S_PLAYER_ZHANBU_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 玩家仙灵数据响应
Declare_Msg_Begin(S2C_PLAYER_ZHANBU_DATA_RESP, MSG_S2C_PLAYER_ZHANBU_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_pZhanBuInfo->dwHistoryCollectionValue);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwHistoryCollectionValue);				// 历史收藏值
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wLowCardGroupMapIdx);				// 低级卡组关卡索引
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wCurLowCardGroupId);					// 当前低级卡组ID
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwLowCardGroupLastRecoverTime);		// 低级卡组最近恢复时间
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes);	// 当前低级卡组拥有的刷新次数
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes);	// 今日低级卡组翻卡次数
Declare_EncodeMsg_UnsignedInt(m_pZhanBuInfo->dwHighCardGroupLastRefreshTime);		// 高级卡组最近刷新时间
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wAchievementLevel);					// 成就等级
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes);	// 今日低级卡组已经花费刷新的次数(需要消耗的刷新次数)
Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes);// 今日高级卡组已经花费刷新的次数(需要消耗的刷新次数)
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
Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// 玩家仙灵数据
Declare_Msg_End

// 仙灵列表请求
Declare_Msg_Begin(C2S_CARD_LIST_REQ, MSG_C2S_CARD_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 仙灵列表响应
Declare_Msg_Begin(S2C_CARD_LIST_RESP, MSG_S2C_CARD_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pZhanBuInfo->wCardNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardNum);
Declare_EncodeMsg_Memory(m_CardList, m_wCardNum * sizeof(ClientCard));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// 玩家仙灵数据
Declare_Msg_Member_Array(ClientCard, m_CardList, MAX_CARD_NUM);	// 卡牌列表
Declare_Msg_Member(WORD, m_wCardNum)		// 卡牌数量
Declare_Msg_End

// 仙灵图鉴列表请求
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_LIST_REQ, MSG_C2S_FAIRY_POKEDEX_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 仙灵图鉴列表响应
// Declare_Msg_Begin(S2C_FAIRY_POKEDEX_LIST_RESP, MSG_S2C_FAIRY_POKEDEX_LIST_RESP)
// Declare_DecodeMsg_Function_Begin
// Declare_DecodeMsg_UnsignedShort(m_pZhanBuInfo->byPokedexNum);
// Declare_DecodeMsg_Function_End
// Declare_EncodeMsg_Function_Begin
// Declare_EncodeMsg_UnsignedShort(m_pZhanBuInfo->byPokedexNum);
// Declare_EncodeMsg_Memory(m_pZhanBuInfo->arPokedexList, m_pZhanBuInfo->byPokedexNum * sizeof(FairyPokedex));
// Declare_EncodeMsg_Function_End
// Declare_Msg_Member(PlayerZhanBuInfo*, m_pZhanBuInfo)		// 玩家仙灵数据
// Declare_Msg_End


// 仙灵升级请求
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
Declare_Msg_Member(DWORD, m_dwFairyUId)		// 升级的仙灵UID
Declare_Msg_Member(WORD, m_wCostFairyNum)		// 作为消耗的仙灵数量
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// 作为消耗的仙灵列表
Declare_Msg_End

// 仙灵升级响应
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
Declare_Msg_Member(DWORD, m_dwFairyUId)		// 升级的仙灵UID
Declare_Msg_Member(WORD, m_wFairyLevel)		// 仙灵升级后的等级
Declare_Msg_Member(DWORD, m_dwFairyExp)		// 升级的仙灵经验
Declare_Msg_Member(BYTE, m_wCostFairyNum)		// 作为消耗的仙灵数量
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// 作为消耗的仙灵列表
Declare_Msg_End


// 仙灵突破请求
Declare_Msg_Begin(C2S_CARD_BREAK_REQ, MSG_C2S_CARD_BREAK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// 仙灵UID
Declare_Msg_End


// 仙灵突破响应
Declare_Msg_Begin(S2C_CARD_BREAK_RESP, MSG_S2C_CARD_BREAK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwFairyUId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwFairyUId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwFairyUId)		// 仙灵UID
Declare_Msg_End


// 卡牌刷新请求
Declare_Msg_Begin(C2S_CARD_REFRESH_REQ, MSG_C2S_CARD_REFRESH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRefreshType) // 刷新类型(1=低级卡组, 2=高级卡组) 
Declare_Msg_End


// 卡牌刷新响应
Declare_Msg_Begin(S2C_CARD_REFRESH_RESP, MSG_S2C_CARD_REFRESH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRefreshType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRefreshType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRefreshType) // 刷新类型(1=低级卡组, 2=高级卡组) 
Declare_Msg_End

// 翻卡请求
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
Declare_Msg_Member(BYTE, m_byType) // 类型 (1=低级卡组, 2=高级卡组) 
Declare_Msg_Member(WORD, m_wCardIdxListNum) // 卡牌索引数量
Declare_Msg_Member_Array(WORD, m_arCardIdxList, MAX_CARD_GROUP_NUM);	// 卡牌索引列表
Declare_Msg_End

// 翻卡响应
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
Declare_Msg_Member(BYTE, m_byType) // 类型 (1=低级卡组, 2=高级卡组) 
Declare_Msg_Member(DWORD, m_dwDestinyCoin) // 命运币
Declare_Msg_Member(WORD, m_wCardListNum) // 卡牌数量
Declare_Msg_Member_Array(WORD, m_arCardIdxList, MAX_CARD_GROUP_NUM);	// 卡牌索引列表
Declare_Msg_Member_Array(DWORD, m_arCardIdList, MAX_CARD_GROUP_NUM);	// 卡牌ID列表
Declare_Msg_Member_Array(DWORD, m_arDestinyCoinList, MAX_CARD_GROUP_NUM);	// 命运币列表
Declare_Msg_End

// 低级卡组刷新次数更新请求
Declare_Msg_Begin(C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ, MSG_C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 低级卡组刷新次数更新响应
Declare_Msg_Begin(S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP, MSG_S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurLowCardGroupRefreshTimes)
Declare_DecodeMsg_UnsignedInt(m_dwLowCardGroupLastRecoverTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurLowCardGroupRefreshTimes)
Declare_EncodeMsg_UnsignedInt(m_dwLowCardGroupLastRecoverTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurLowCardGroupRefreshTimes)	// 当前低级卡组拥有的刷新次数
Declare_Msg_Member(DWORD, m_dwLowCardGroupLastRecoverTime)	// 当前低级卡组拥有的刷新次数
Declare_Msg_End


// 收藏组激活请求
Declare_Msg_Begin(C2S_CELLECTION_GROUP_ACTIVATE_REQ, MSG_C2S_CELLECTION_GROUP_ACTIVATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_End


// 收藏组激活响应
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
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_Member(DWORD, m_dwHistoryCollectionValue)	// 收藏组ID
Declare_Msg_End

// 收藏组升星请求
Declare_Msg_Begin(C2S_CELLECTION_GROUP_STAR_UP_REQ, MSG_C2S_CELLECTION_GROUP_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_End


// 收藏组升星响应
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
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_Member(DWORD, m_dwHistoryCollectionValue)	// 收藏组ID
Declare_Msg_End

// 收藏组重置(取卡)请求
Declare_Msg_Begin(C2S_CELLECTION_GROUP_RESET_REQ, MSG_C2S_CELLECTION_GROUP_RESET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBookId)
Declare_DecodeMsg_UnsignedShort(m_wGroupId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBookId)
Declare_EncodeMsg_UnsignedShort(m_wGroupId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_End

// 收藏组重置(取卡)响应
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
Declare_Msg_Member(WORD, m_wBookId)		// 收藏书ID
Declare_Msg_Member(WORD, m_wGroupId)	// 收藏组ID
Declare_Msg_Member(WORD, m_wStar)	// 星数
Declare_Msg_End


// 收藏书列表请求
Declare_Msg_Begin(C2S_COLLECTION_BOOK_LIST_REQ, MSG_C2S_COLLECTION_BOOK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 收藏书列表响应
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
Declare_Msg_Member(WORD, m_wCollectionBookNum) // 卡牌索引数量
Declare_Msg_Member_Array(CollectionBook, m_arCollectionBookList, MAX_COLLECTION_BOOK_NUM);	// 收藏书列表
Declare_Msg_End


// 装备卡牌请求
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
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵位(从1开始)
Declare_Msg_Member(WORD, m_wCardSlot)	// 卡槽(从0开始)
Declare_Msg_Member(DWORD, m_dwCardUID)	// 卡牌UID
Declare_Msg_End

// 装备卡牌响应
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
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵位(从1开始)
Declare_Msg_Member(WORD, m_wCardSlot)	// 卡槽(从0开始)
Declare_Msg_Member(DWORD, m_dwCardUID)	// 卡牌UID
Declare_Msg_End


// 卸下卡牌请求
Declare_Msg_Begin(C2S_COLLECTION_UNLOAD_CARD_REQ, MSG_C2S_COLLECTION_UNLOAD_CARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵位(从1开始)
Declare_Msg_Member(WORD, m_wCardSlot)	// 卡槽(从0开始)
Declare_Msg_End

// 卸下卡牌响应
Declare_Msg_Begin(S2C_COLLECTION_UNLOAD_CARD_RESP, MSG_S2C_COLLECTION_UNLOAD_CARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationPos)
Declare_DecodeMsg_UnsignedShort(m_wCardSlot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationPos)
Declare_EncodeMsg_UnsignedShort(m_wCardSlot)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationPos)		// 阵位(从1开始)
Declare_Msg_Member(WORD, m_wCardSlot)	// 卡槽(从0开始)
Declare_Msg_End


// 占卜成就激活请求
Declare_Msg_Begin(C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ, MSG_C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 占卜成就激活响应
Declare_Msg_Begin(S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP, MSG_S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurAchievementLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurAchievementLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurAchievementLevel)		// 当前成就等级
Declare_Msg_End

// 仙灵招募请求
Declare_Msg_Begin(C2S_FAIRY_RECRUIT_REQ, MSG_C2S_FAIRY_RECRUIT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecruitType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRecruitType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecruitType)		// 招募类型
Declare_Msg_End


// 仙灵招募响应
Declare_Msg_Begin(S2C_FAIRY_RECRUIT_RESP, MSG_S2C_FAIRY_RECRUIT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byRecruitType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byRecruitType)
Declare_EncodeMsg_UnsignedShort(m_byFairyNum)
Declare_EncodeMsg_Memory(m_arCardList, m_byFairyNum*sizeof(DWORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byRecruitType)		// 招募类型
Declare_Msg_Member(BYTE, m_byFairyNum)		// 招募到的仙灵数量
Declare_Msg_Member_Array(DWORD, m_arCardList, MAX_CARD_NUM);	// 招募到的仙灵列表
Declare_Msg_End

// 仙灵图鉴升星请求
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_STAR_UP_REQ, MSG_C2S_FAIRY_POKEDEX_STAR_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// 仙灵图鉴ID
Declare_Msg_End

// 仙灵图鉴升星响应
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
Declare_Msg_Member(WORD, m_wPokedexId)		// 升星的图鉴ID
Declare_Msg_Member(BYTE, m_byCostFairyNum)		// 升星消耗的仙灵数量
Declare_Msg_Member_Array(DWORD, m_arCostFairyList, MAX_CARD_NUM);	// 升星消耗的仙灵列表
Declare_Msg_End


// 仙灵图鉴重生请求
Declare_Msg_Begin(C2S_FAIRY_POKEDEX_REVIVE_REQ, MSG_C2S_FAIRY_POKEDEX_REVIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// 重生的图鉴ID
Declare_Msg_End


// 仙灵图鉴重生响应
Declare_Msg_Begin(S2C_FAIRY_POKEDEX_REVIVE_RESP, MSG_S2C_FAIRY_POKEDEX_REVIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wPokedexId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wPokedexId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wPokedexId)		// 重生的图鉴ID
Declare_Msg_End


// 仙灵关卡刷新请求
Declare_Msg_Begin(C2S_FAIRY_MAP_REFRESH_REQ, MSG_C2S_FAIRY_MAP_REFRESH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 仙灵关卡刷新响应
Declare_Msg_Begin(S2C_FAIRY_MAP_REFRESH_RESP, MSG_S2C_FAIRY_MAP_REFRESH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 仙灵图鉴激活请求
Declare_Msg_Begin(C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ, MSG_C2S_FAIRY_ACHIEVEMENT_ACTIVATION_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 仙灵图鉴激活响应
Declare_Msg_Begin(S2C_FAIRY_ACHIEVEMENT_ACTIVATION_RESP, MSG_S2C_FAIRY_ACHIEVEMENT_ACTIVATION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byPokedexAchievementLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byPokedexAchievementLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byPokedexAchievementLevel)		// 图鉴成就等级
Declare_Msg_End


// 卡牌数据通知
Declare_Msg_Begin(S2C_FAIRY_DATA_NOTICE, MSG_S2C_FAIRY_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byFlag)
Declare_DecodeMsg_Memory(m_tCard, sizeof(ClientCard))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byFlag)
Declare_EncodeMsg_Memory(m_tCard, sizeof(ClientCard))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byFlag)		// 操作标志(0=减少,1=添加,更新)
Declare_Msg_Member(ClientCard, m_tCard)		// 仙灵数据
Declare_Msg_End

// 仙灵图鉴数据通知
Declare_Msg_Begin(S2C_FAIRY_POKEDEX_DATA_NOTICE, MSG_S2C_FAIRY_POKEDEX_DATA_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Memory(m_tPokedex, sizeof(FairyPokedex))
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_Memory(m_tPokedex, sizeof(FairyPokedex))
Declare_EncodeMsg_Function_End
Declare_Msg_Member(FairyPokedex, m_tPokedex)		// 仙灵图鉴
Declare_Msg_End


// 积分赛数据请求
Declare_Msg_Begin(C2S_SCORE_MATCH_DATA_REQ, MSG_C2S_SCORE_MATCH_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 积分赛数据响应
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
Declare_Msg_Member(WORD, m_wCurActivityIdx)		// 当前活动索引
Declare_Msg_Member(ScoreMatchData, m_MatchData)		// 比赛数据
Declare_Msg_Member_Array(PlayerCommonData, m_BattleTargetList, MAX_SCORE_MATCH_BATTLE_TARGET_NUM); // 挑战目标列表
Declare_Msg_Member(WORD, m_wBattleTargetNum)		// 挑战目标数量
Declare_Msg_End

// 选择阵营
Declare_Msg_Begin(C2S_SCORE_MATCH_SELECT_CAMP_REQ, MSG_C2S_SCORE_MATCH_SELECT_CAMP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// 选择的阵营
Declare_Msg_End

// 选择阵营响应
Declare_Msg_Begin(S2C_SCORE_MATCH_SELECT_CAMP_RESP, MSG_S2C_SCORE_MATCH_SELECT_CAMP_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byCamp)		// 选择的阵营
Declare_Msg_End


// 刷新目标
Declare_Msg_Begin(C2S_SCORE_MATCH_REFRESH_TARGET_REQ, MSG_C2S_SCORE_MATCH_REFRESH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 刷新目标响应
Declare_Msg_Begin(S2C_SCORE_MATCH_REFRESH_TARGET_RESP, MSG_S2C_SCORE_MATCH_REFRESH_TARGET_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 购买挑战次数
Declare_Msg_Begin(C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ, MSG_C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 购买挑战次数响应
Declare_Msg_Begin(S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP, MSG_S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取挑战任务奖励
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
Declare_Msg_Member_Array(WORD, m_TaskList, MAX_SCORE_MATCH_TASK_NUM); // 领取的任务列表
Declare_Msg_End

// 领取挑战任务奖励响应
Declare_Msg_Begin(S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP, MSG_S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
m_wTaskNum = min(m_wTaskNum, MAX_SCORE_MATCH_TASK_NUM);
Declare_EncodeMsg_UnsignedShort(m_wTaskNum)
Declare_EncodeMsg_Memory(m_TaskList, m_wTaskNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskNum)		// 选择的阵营
Declare_Msg_Member_Array(WORD, m_TaskList, MAX_SCORE_MATCH_TASK_NUM);
Declare_Msg_End


// 争霸赛数据请求
Declare_Msg_Begin(C2S_WARCRAFT_DATA_REQ, MSG_C2S_WARCRAFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 争霸赛数据响应
Declare_Msg_Begin(S2C_WARCRAFT_DATA_RESP, MSG_S2C_WARCRAFT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byWarcraftBattleTimes)
Declare_DecodeMsg_UnsignedChar(m_byBuyWarcraftBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byWarcraftBattleTimes)
Declare_EncodeMsg_UnsignedChar(m_byBuyWarcraftBattleTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byWarcraftBattleTimes)		// 争霸赛挑战次数
Declare_Msg_Member(BYTE, m_byBuyWarcraftBattleTimes)	// 购买的争霸赛挑战次数
Declare_Msg_End

// 争霸赛购买挑战次数
Declare_Msg_Begin(C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ, MSG_C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 争霸赛购买挑战次数响应
Declare_Msg_Begin(S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP, MSG_S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 挑战目标请求
Declare_Msg_Begin(C2S_WARCRAFT_TARGET_LIST_REQ, MSG_C2S_WARCRAFT_TARGET_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 挑战目标响应 
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
Declare_Msg_Member(WORD, m_wRank)			// 自己的排行
Declare_Msg_Member(BYTE, m_byTargetNum)		// 目标数量
Declare_Msg_Member_Array(WarcraftTargetData, m_TargetList, MAX_WARCRAFT_TARGET_NUM);	// 目标列表
Declare_Msg_End

// 争霸赛排行榜请求
Declare_Msg_Begin(C2S_WARCRAFT_RANK_LIST_REQ, MSG_C2S_WARCRAFT_RANK_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 争霸赛排行榜响应 
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
Declare_Msg_Member(WORD, m_wRankNum)		// 目标数量
Declare_Msg_Member_Array(ClientWarcraftRankData, m_RankList, MAX_WARCRAFT_RANK_NUM);	// 目标列表
Declare_Msg_End

// 争霸赛助威请求
Declare_Msg_Begin(C2S_WARCRAFT_CHEER_REQ, MSG_C2S_WARCRAFT_CHEER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 目标ID
Declare_Msg_End

// 争霸赛助威响应
Declare_Msg_Begin(S2C_WARCRAFT_CHEER_RESP, MSG_S2C_WARCRAFT_CHEER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 目标ID
Declare_Msg_End


// 请求角色称号信息
Declare_Msg_Begin(C2S_ROLE_TITLE_INFO_REQ, MSG_C2S_ROLE_TITLE_INFO_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求角色称号信息响应
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
Declare_Msg_Member(PlayerTitleInfo, m_TitleInfo)	// 称号信息
Declare_Msg_End


// 使用称号请求
Declare_Msg_Begin(C2S_USE_TITLE_REQ, MSG_C2S_USE_TITLE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nTitleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nTitleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nTitleId)	// 使用的称号ID
Declare_Msg_End

// 使用称号响应
Declare_Msg_Begin(S2C_USE_TITLE_RESP, MSG_S2C_USE_TITLE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nTitleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nTitleId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nTitleId)	// 使用的称号ID
Declare_Msg_End


// 请求红点列表
Declare_Msg_Begin(C2S_RED_POINT_LIST_REQ, MSG_C2S_RED_POINT_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求红点列表响应
Declare_Msg_Begin(S2C_RED_POINT_LIST_RESP, MSG_S2C_RED_POINT_LIST_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRedPointNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRedPointNum)
Declare_EncodeMsg_Memory(m_RedPointList, m_wRedPointNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRedPointNum)		// 红点数量
Declare_Msg_Member_Array(WORD, m_RedPointList, MAX_RED_POINT_NUM);	// 红点列表
Declare_Msg_End

// 红点通知
Declare_Msg_Begin(S2C_RED_POINT_NOTIFY, MSG_S2C_RED_POINT_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSystemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSystemId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSystemId)		// 系统Id
Declare_Msg_End


// VIP礼包数据请求
Declare_Msg_Begin(C2S_VIP_GIFT_DATA_REQ, MSG_C2S_VIP_GIFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// VIP礼包数据响应
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
Declare_Msg_Member(WORD, m_wDailyInitVipLevel)						// 每日初始VIP等级
Declare_Msg_Member(short, m_nVipLevelRewardRecord)						// 当前已经领取的VIP等级奖励(-1=表示还没有领取)
Declare_Msg_Member(WORD, m_wWeekGiftNum)							// 礼包数量
Declare_Msg_Member_Array(Gift, m_WeekGiftList, MAX_VIP_WEEK_GIFT_NUM);	// 每周礼包列表
Declare_Msg_Member(WORD, m_wTodayRecommendRechargeRewardId)		// 今日推荐奖励Id
Declare_Msg_Member(WORD, m_wTodayRecommendRechargeRewardStatus)	// 今日推荐奖励状态(0=不能领取,1=可领取,2=已领取)
Declare_Msg_End


// VIP每日礼包领取
Declare_Msg_Begin(C2S_VIP_DAILY_GIFT_GET_REQ, MSG_C2S_VIP_DAILY_GIFT_GET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wVipLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wVipLevel)		// VIP等级
Declare_Msg_End


// VIP每日礼包领取响应
Declare_Msg_Begin(S2C_VIP_DAILY_GIFT_GET_RESP, MSG_S2C_VIP_DAILY_GIFT_GET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wVipLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wVipLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wVipLevel)		// VIP等级
Declare_Msg_End


// VIP周礼包购买
Declare_Msg_Begin(C2S_VIP_WEEK_GIFT_BUY_REQ, MSG_C2S_VIP_WEEK_GIFT_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGiftId)		// 礼包ID
Declare_Msg_End


// VIP周礼包购买响应
Declare_Msg_Begin(S2C_VIP_WEEK_GIFT_BUY_RESP, MSG_S2C_VIP_WEEK_GIFT_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGiftId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGiftId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGiftId)		// 礼包ID
Declare_Msg_End


// 领取今日推荐奖励
Declare_Msg_Begin(C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ, MSG_C2S_TAKE_TODAY_RECOMMEND_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取今日推荐礼包
Declare_Msg_Begin(S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP, MSG_S2C_TAKE_TODAY_RECOMMEND_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 等级礼包数据请求
Declare_Msg_Begin(C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ, MSG_C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 等级礼包数据响应
Declare_Msg_Begin(S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP, MSG_S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityDataNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityDataNum);
Declare_EncodeMsg_Memory(m_ActivityDataList, m_wActivityDataNum*sizeof(LevelShopActivityData));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityDataNum)	
Declare_Msg_Member_Array(LevelShopActivityData, m_ActivityDataList, MAX_LEVEL_SHOP_ACTIVITY_NUM);	// 活动数据列表
Declare_Msg_End


// 开服基金数据请求
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_DATA_REQ, MSG_C2S_OPEN_SERVER_FUND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 开服基金数据请求
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
Declare_Msg_Member(DWORD, m_dwOpenServerFundBuyTimes) // 基金购买次数
Declare_Msg_Member(OpenServerFundData, m_tFundData) // 基金数据
Declare_Msg_Member(AllPeopleWelfareData, m_tWelfareData) // 全民福利数据

Declare_Msg_End

// 开服基金购买请求
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_BUY_REQ, MSG_C2S_OPEN_SERVER_FUND_BUY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 开服基金购买响应
Declare_Msg_Begin(S2C_OPEN_SERVER_FUND_BUY_RESP, MSG_S2C_OPEN_SERVER_FUND_BUY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 开服基金奖励领取请求
Declare_Msg_Begin(C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ, MSG_C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// 奖励ID
Declare_Msg_End

// 开服基金奖励领取响应
Declare_Msg_Begin(S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP, MSG_S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// 奖励ID
Declare_Msg_End

// 全民福利领取请求
Declare_Msg_Begin(C2S_ALL_PEOPLE_WELFARE_TAKE_REQ, MSG_C2S_ALL_PEOPLE_WELFARE_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// 奖励ID
Declare_Msg_End

// 全民福利领取响应
Declare_Msg_Begin(S2C_ALL_PEOPLE_WELFARE_TAKE_RESP, MSG_S2C_ALL_PEOPLE_WELFARE_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// 奖励ID
Declare_Msg_End


// 限时优惠数据请求
Declare_Msg_Begin(C2S_LIMIT_PREFERENTIAL_DATA_REQ, MSG_C2S_LIMIT_PREFERENTIAL_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 限时优惠数据响应
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
Declare_Msg_Member(LimitPreferentialData, m_Data)		// 奖励ID
Declare_Msg_End

// 限时优惠福利领取请求
Declare_Msg_Begin(C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ, MSG_C2S_LIMIT_PREFERENTIAL_WELFARE_TAKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// 福利ID
Declare_Msg_End

// 限时优惠福利领取响应
Declare_Msg_Begin(S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP, MSG_S2C_LIMIT_PREFERENTIAL_WELFARE_TAKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWelfareId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWelfareId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWelfareId)		// 福利ID
Declare_Msg_End


// 等级礼包数据请求
Declare_Msg_Begin(C2S_LEVEL_GIFT_DATA_REQ, MSG_C2S_LEVEL_GIFT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 等级礼包数据响应
Declare_Msg_Begin(S2C_LEVEL_GIFT_DATA_RESP, MSG_S2C_LEVEL_GIFT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevelGiftTakeRecordNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevelGiftTakeRecordNum);
Declare_EncodeMsg_Memory(m_arLevelGiftTakeRecord, m_wLevelGiftTakeRecordNum*sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevelGiftTakeRecordNum)
Declare_Msg_Member_Array(WORD, m_arLevelGiftTakeRecord, MAX_LEVEL_GIFT_TAKE_RECORD_NUM);	// 活动数据列表
Declare_Msg_End


// 领取等级礼包请求
Declare_Msg_Begin(C2S_TAKE_LEVEL_GIFT_REWARD_REQ, MSG_C2S_TAKE_LEVEL_GIFT_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)		// 福利ID
Declare_Msg_End

// 领取等级礼包请求
Declare_Msg_Begin(S2C_TAKE_LEVEL_GIFT_REWARD_RESP, MSG_S2C_TAKE_LEVEL_GIFT_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLevel)		// 福利ID
Declare_Msg_End


// 次日奖励数据请求
Declare_Msg_Begin(C2S_NEXT_DAY_REWARD_DATA_REQ, MSG_C2S_NEXT_DAY_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 次日奖励数据响应
Declare_Msg_Begin(S2C_NEXT_DAY_REWARD_DATA_RESP, MSG_S2C_NEXT_DAY_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wIsTakeReward)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wIsTakeReward);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wIsTakeReward)		// 是否已领取奖励
Declare_Msg_End

// 次日奖励领取请求
Declare_Msg_Begin(C2S_NEXT_DAY_REWARD_TAKE_REQ, MSG_C2S_NEXT_DAY_REWARD_TAKE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 次日奖励领取请求
Declare_Msg_Begin(S2C_NEXT_DAY_REWARD_TAKE_RESP, MSG_S2C_NEXT_DAY_REWARD_TAKE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End




// 玩家留言请求
Declare_Msg_Begin(C2S_PLAYER_LEVEL_MESSAGE_REQ, MSG_C2S_PLAYER_LEVEL_MESSAGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_String(m_szContact, sizeof(m_szContact)-1)
Declare_DecodeMsg_String(m_szMessage, sizeof(m_szMessage)-1)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_String(m_szContact, sizeof(m_szContact))
Declare_EncodeMsg_String(m_szMessage, sizeof(m_szMessage))
Declare_EncodeMsg_Function_End
Declare_Msg_Member_Array(char, m_szContact, MAX_CONTACT_LENGTH)	// 联系方式
Declare_Msg_Member_Array(char, m_szMessage, MAX_LEAVE_MESSAGE_LENGTH) // 留言内容
Declare_Msg_End

// 玩家留言响应
Declare_Msg_Begin(S2C_PLAYER_LEVEL_MESSAGE_RESP, MSG_S2C_PLAYER_LEVEL_MESSAGE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 关卡评论列表请求
Declare_Msg_Begin(C2S_MAP_COMMENT_LIST_REQ, MSG_C2S_MAP_COMMENT_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// 关卡ID
Declare_Msg_End


// 关卡评论列表响应
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
Declare_Msg_Member(WORD, m_wMapId)		// 关卡ID
Declare_Msg_Member(WORD, m_wCommentNum)		// 评论数量
Declare_Msg_Member_Array(MapCommentClient, m_CommentList, MAX_MAP_COMMENT_NUM)	// 评论列表
Declare_Msg_End


// 关卡评论请求
Declare_Msg_Begin(C2S_MAP_COMMENT_REQ, MSG_C2S_MAP_COMMENT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_String(m_szText, sizeof(m_szText)-1);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_String(m_szText, sizeof(m_szText));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)		// 关卡ID
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_128)	// 评论内容
Declare_Msg_End


// 关卡评论响应
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
Declare_Msg_Member(WORD, m_wMapId)		// 关卡ID
Declare_Msg_Member(WORD, m_wCommentId)	// 评论ID
Declare_Msg_Member(DWORD, m_dwTime)		// 评论时间
Declare_Msg_Member_Array(char, m_szText, BUFF_LEN_128)	// 评论内容
Declare_Msg_End


// 关卡评论点赞
Declare_Msg_Begin(C2S_MAP_COMMENT_LIKE_REQ, MSG_C2S_MAP_COMMENT_LIKE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_UnsignedShort(m_wCommentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)	// 关卡ID
Declare_Msg_Member(WORD, m_wCommentId) // 评论ID
Declare_Msg_End

// 关卡评论点赞响应
Declare_Msg_Begin(S2C_MAP_COMMENT_LIKE_RESP, MSG_S2C_MAP_COMMENT_LIKE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wMapId)
Declare_DecodeMsg_UnsignedShort(m_wCommentId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wMapId);
Declare_EncodeMsg_UnsignedShort(m_wCommentId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wMapId)	// 关卡ID
Declare_Msg_Member(WORD, m_wCommentId) // 评论ID
Declare_Msg_End

// 充值数据请求
Declare_Msg_Begin(C2S_RECHARGE_DATA_REQ, MSG_C2S_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 充值数据响应
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
Declare_Msg_Member(RoleRechargeData*, m_pRechargetData) // 充值数据
Declare_Msg_End


// 充值请求
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
Declare_Msg_Member(WORD, m_wRechargeId)		// 充值ID
Declare_Msg_Member(WORD, m_wRechargeType)	// 充值类型
Declare_Msg_Member(WORD, m_wGoodsId)		// 充值类型
Declare_Msg_Member(DWORD, m_dwContext)		// 充值类型
Declare_Msg_End


// 充值响应
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
Declare_Msg_Member(WORD, m_wRechargeId)		// 充值ID
Declare_Msg_Member(WORD, m_wRechargeType)	// 充值类型
Declare_Msg_Member(WORD, m_wGoodsId)		// 充值类型
Declare_Msg_Member(DWORD, m_dwContext)		// 充值类型
Declare_Msg_End


// 月卡数据请求
Declare_Msg_Begin(C2S_MONTH_CARD_DATA_REQ, MSG_C2S_MONTH_CARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 月卡数据响应
Declare_Msg_Begin(S2C_MONTH_CARD_DATA_RESP, MSG_S2C_MONTH_CARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_pRechargetData->wMontchCardNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_pRechargetData->wMontchCardNum);
Declare_EncodeMsg_Memory(m_pRechargetData->arMonthCardList, m_pRechargetData->wMontchCardNum*sizeof(MonthCard));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleRechargeData*, m_pRechargetData) // 充值数据
Declare_Msg_End

// 月卡购买
Declare_Msg_Begin(C2S_MONTH_CARD_BUY_REQ, MSG_C2S_MONTH_CARD_BUY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // 月卡ID
Declare_Msg_End

// 月卡购买响应
Declare_Msg_Begin(S2C_MONTH_CARD_BUY_RESP, MSG_S2C_MONTH_CARD_BUY_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // 月卡ID
Declare_Msg_End

// 领取月卡奖励
Declare_Msg_Begin(C2S_TAKE_MONTH_CARD_REWARD_REQ, MSG_C2S_TAKE_MONTH_CARD_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // 月卡ID
Declare_Msg_End

// 领取月卡奖励响应
Declare_Msg_Begin(S2C_TAKE_MONTH_CARD_REWARD_RESP, MSG_S2C_TAKE_MONTH_CARD_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCardId) // 月卡ID
Declare_Msg_End


// 充值通知
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
Declare_Msg_Member(WORD, m_wRechargeType) // 充值类型
Declare_Msg_Member(WORD, m_wRechargeId) // 充值ID
Declare_Msg_Member(DWORD, m_dwContext) // 上下文
Declare_Msg_End


// 查找目标
Declare_Msg_Begin(C2S_PVP_SEARCH_TARGET_REQ, MSG_C2S_PVP_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritSkillId) // 出战精灵技能ID
Declare_Msg_End

// 查找目标响应
Declare_Msg_Begin(S2C_PVP_SEARCH_TARGET_RESP, MSG_S2C_PVP_SEARCH_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSpiritSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSpiritSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSpiritSkillId) // 出战精灵技能ID
Declare_Msg_End

// 战斗准备通知
Declare_Msg_Begin(S2C_PVP_READY_NOTIFY, MSG_S2C_PVP_READY_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRobot)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRobot);
// 红方数据
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
// 蓝方数据
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
Declare_Msg_Member(BYTE, m_byIsRobot) // 是否机器人
Declare_Msg_Member(PVPBattlefieldData*, m_pBattleData) // 战斗数据
Declare_Msg_End



// 战斗准备完成请求
Declare_Msg_Begin(C2S_PVP_READY_FINISH_REQ, MSG_C2S_PVP_READY_FINISH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 战斗准备完成响应
Declare_Msg_Begin(S2C_PVP_READY_FINISH_RESP, MSG_S2C_PVP_READY_FINISH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 战斗开始通知
Declare_Msg_Begin(S2C_PVP_BATTLE_START_NOTIFY, MSG_S2C_PVP_BATTLE_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 战斗结束通知
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
Declare_Msg_Member(DWORD, m_dwWinnerId) // 获胜者ID
Declare_Msg_Member(WORD, m_wRedOldRank) // 红方旧排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedNewRank) // 红方新排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedLastWeekRank) // 红方上周排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedScore) // 红方本次获得积分
Declare_Msg_Member(WORD, m_wBlueOldRank) // 蓝方旧排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueNewRank) // 蓝方新排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueLastWeekRank) // 蓝方上周排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueScore) // 蓝方本次获得积分
Declare_Msg_End


// 使用技能
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
Declare_Msg_Member(DWORD, m_dwObjId) // 使用者ID
Declare_Msg_Member(DWORD, m_dwSkillId) // 使用者ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// 使用技能响应
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
Declare_Msg_Member(DWORD, m_dwObjId) // 使用者ID
Declare_Msg_Member(DWORD, m_dwSkillId) // 使用者ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// 使用技能通知
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
Declare_Msg_Member(DWORD, m_dwObjId) // 使用者ID
Declare_Msg_Member(DWORD, m_dwSkillId) // 使用者ID
Declare_Msg_Member(WORD, m_wX) // 
Declare_Msg_Member(WORD, m_wY) // 
Declare_Msg_End


// HP改变请求
Declare_Msg_Begin(C2S_PVP_HP_CHANGE_REQ, MSG_C2S_PVP_HP_CHANGE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_DecodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_DecodeMsg_Int(m_nChangeHP)
Declare_DecodeMsg_Int(m_nDamageType)
Declare_DecodeMsg_UnsignedInt(m_dwPower)
//Declare_DecodeMsg_UnsignedInt(m_dwCostTime)
Declare_DecodeMsg_String(m_szSign, sizeof(m_szSign)-1)	// 签名
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSrcObjId)
Declare_EncodeMsg_UnsignedInt(m_dwTgtObjId)
Declare_EncodeMsg_Int(m_nChangeHP)
Declare_EncodeMsg_Int(m_nDamageType)
//Declare_EncodeMsg_Int(m_dwCostTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(int, m_nChangeHP)	// 改变HP
Declare_Msg_Member(int, m_nDamageType) // 伤害类型
Declare_Msg_Member(DWORD, m_dwPower) // 战力
//Declare_Msg_Member(DWORD, m_dwCostTime) // 消耗时间
Declare_Msg_Member_Array(char, m_szSign, MAX_BATTLE_PARAM_SIGN_LEN) // 签名
Declare_Msg_End


// HP改变响应
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
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(int, m_nChangeHP)	// 改变HP
Declare_Msg_Member(int, m_nDamageType) // 伤害类型
Declare_Msg_End


// HP改变通知
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
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(int, m_nChangeHP)	// 改变HP
Declare_Msg_Member(int, m_nDamageType) // 伤害类型
Declare_Msg_End


// 创建buff请求
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
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // 是否来自精灵祝福Buff
Declare_Msg_End


// 创建buff响应
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
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // 是否来自精灵祝福Buff
Declare_Msg_End


// 创建buff通知
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
Declare_Msg_Member(DWORD, m_dwSrcObjId) // 产生源ID
Declare_Msg_Member(DWORD, m_dwTgtObjId) // 目标者ID
Declare_Msg_Member(BYTE, m_byIsSpriteBlessBuff) // 是否来自精灵祝福Buff
Declare_Msg_End


// 战斗结果请求
Declare_Msg_Begin(C2S_PVP_BATTLE_RESULT_REQ, MSG_C2S_PVP_BATTLE_RESULT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byResult)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byResult);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byResult) // 战斗结果(时间到的平局由服务器来判定)
Declare_Msg_End


// 战斗结果结果
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
Declare_Msg_Member(BYTE, m_byResult) // 结果(1=获胜,2=平局,3=失败)
Declare_Msg_Member(WORD, m_wRedOldRank) // 红方旧排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedNewRank) // 红方新排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedLastWeekRank) // 红方上周排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wRedScore) // 红方本次获得积分
Declare_Msg_Member(WORD, m_wBlueOldRank) // 蓝方旧排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueNewRank) // 蓝方新排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueLastWeekRank) // 蓝方上周排名(0 = 未上榜)
Declare_Msg_Member(WORD, m_wBlueScore) // 蓝方本次获得积分
Declare_Msg_End


// 领取战斗奖励
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
Declare_Msg_Member(BYTE, m_byType) // 类型(1=每日的,每周的)
Declare_Msg_Member(WORD, m_wTakeRewardNum) // 领取的奖励数量
Declare_Msg_Member_Array(BYTE, m_arTakeRewardList, MAX_PVP_BATTLE_REWARD_NUM)	// 领取的奖励列表
Declare_Msg_End


// 领取战斗奖励响应
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
Declare_Msg_Member(BYTE, m_byType) // 类型(1=每日的,每周的)
Declare_Msg_Member(WORD, m_wTakeRewardNum) // 领取的奖励数量
Declare_Msg_Member_Array(BYTE, m_arTakeRewardList, MAX_PVP_BATTLE_REWARD_NUM)	// 领取的奖励列表
Declare_Msg_End


// 玩家PVP数据请求
Declare_Msg_Begin(C2S_PLAYER_PVP_DATA_REQ, MSG_C2S_PLAYER_PVP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 玩家PVP数据响应
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
Declare_Msg_Member(PlayerPVPData, m_PVPData) // PVP数据
Declare_Msg_Member(WORD, m_wMyRank) //  我的排行
Declare_Msg_Member(DWORD, m_dwMyRankValue) //  我的排行值
Declare_Msg_Member(WORD, m_wRankNum) //  前几名数量
Declare_Msg_Member_Array(ClientRoleRankData, m_RankList, MAX_DEFAULT_RANK_MUM)	// 前几名排名列表
Declare_Msg_End


// 选择精灵技能请求
Declare_Msg_Begin(C2S_PVP_SELECT_SPRIT_SKILL_REQ, MSG_C2S_PVP_SELECT_SPRIT_SKILL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSkillId) // 技能Id
Declare_Msg_End


// 选择精灵技能响应
Declare_Msg_Begin(S2C_PVP_SELECT_SPRIT_SKILL_RESP, MSG_S2C_PVP_SELECT_SPRIT_SKILL_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwSkillId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwSkillId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwSkillId) // 技能Id
Declare_Msg_End

// 取消查找目标请求
Declare_Msg_Begin(C2S_PVP_CANCEL_SEARCH_TARGET_REQ, MSG_C2S_PVP_CANCEL_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 取消查找目标响应
Declare_Msg_Begin(S2C_PVP_CANCEL_SEARCH_TARGET_RESP, MSG_S2C_PVP_CANCEL_SEARCH_TARGET_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsInBattle)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsInBattle);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsInBattle) // 是否在战斗中
Declare_Msg_End


// 首充数据请求
Declare_Msg_Begin(C2S_LIMIT_FIRST_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_FIRST_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 首充数据响应
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
Declare_Msg_Member(LimitActivityData*, m_pActivityData) // 活动数据
Declare_Msg_End


// 单充数据请求
Declare_Msg_Begin(C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_SINGLE_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 单充数据响应
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
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // 活动数据
// Declare_Msg_End


// 累充数据请求
Declare_Msg_Begin(C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ, MSG_C2S_LIMIT_ACCUMULATE_RECHARGE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 累充数据响应
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
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // 活动数据
// Declare_Msg_End


// 限时招募数据请求
Declare_Msg_Begin(C2S_LIMIT_RECRUIT_DATA_REQ, MSG_C2S_LIMIT_RECRUIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 限时招募数据响应
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
// Declare_Msg_Member(LimitActivityData*, m_pActivityData) // 活动数据
// Declare_Msg_End


// 领取限时活动奖励请求
Declare_Msg_Begin(C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ, MSG_C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDataId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDataId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDataId) // 逻辑数据ID
Declare_Msg_End


// 领取限时活动奖励请求
Declare_Msg_Begin(S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP, MSG_S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDataId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDataId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDataId) // 逻辑数据ID
Declare_Msg_End


// 攻城数据请求
Declare_Msg_Begin(C2S_ATTACK_CITY_DATA_REQ, MSG_C2S_ATTACK_CITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 攻城数据响应
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
Declare_Msg_Member(PlayerAttackCityData, m_AttackCityData) // 攻城数据
Declare_Msg_Member(DWORD, m_dwNo1RoleHeadImgId) // 排名第一的角色头像ID
Declare_Msg_Member_Array(char, m_szNo1RoleName, MAX_ROLE_LEN)	// 前几名排名列表
Declare_Msg_Member(WORD, m_wFromChapter) // 本次活动开始的章节
Declare_Msg_Member(DWORD, m_dwKillBossTime) // 击杀BOSS时间(最近的,0表示木有)
Declare_Msg_End


// 攻城进入请求 
Declare_Msg_Begin(C2S_ATTACK_CITY_ENTER_REQ, MSG_C2S_ATTACK_CITY_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 攻城进入响应
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
Declare_Msg_Member(WORD, m_wPlayerNum) // 逻辑数据ID
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_ATTACK_CITY_PLAYER_NUM)	// 玩家列表
Declare_Msg_Member_Array(MapPos, m_PlayerMapPos, MAX_ATTACK_CITY_PLAYER_NUM)	// 玩家地图坐标
Declare_Msg_End

// 攻城进入通知
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
Declare_Msg_Member(PlayerCommonData, m_tPlayerData) // 玩家数据
Declare_Msg_Member(MapPos, m_tMapPos) // 玩家地图坐标
Declare_Msg_End


// 攻城离开请求
Declare_Msg_Begin(C2S_ATTACK_CITY_LEAVE_REQ, MSG_C2S_ATTACK_CITY_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 攻城离开响应
Declare_Msg_Begin(S2C_ATTACK_CITY_LEAVE_RESP, MSG_S2C_ATTACK_CITY_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 攻城离开通知
Declare_Msg_Begin(S2C_ATTACK_CITY_LEAVE_NOTICE, MSG_S2C_ATTACK_CITY_LEAVE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// 玩家ID
Declare_Msg_End

// 攻城行走请求
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
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End

// 攻城行走响应
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
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End

// 攻城行走响应
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
Declare_Msg_Member(DWORD, m_dwRoleId)	// 玩家ID
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End

// 怪物列表通知
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
Declare_Msg_Member(WORD, m_wCurChapter)	// 当前章节(波数)
Declare_Msg_Member(WORD, m_wMonsterNum)	// 怪物数量
Declare_Msg_Member_Array(AttackCityMonster, m_arMonsterList, MAX_ATTACK_CITY_MONSTER_NUM)	// 怪物列表
Declare_Msg_End



// 怪物数据通知
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
Declare_Msg_Member(AttackCityMonster, m_tMonster)	// 怪物数据
Declare_Msg_End


// 宝箱列表通知
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
Declare_Msg_Member(WORD, m_wTreasureBoxNum)	// 宝箱数量
Declare_Msg_Member_Array(TreasureBox, m_TreasureBoxList, MAX_ATTACK_CITY_TREASURE_BOX_NUM)	// 宝箱列表
Declare_Msg_End


// 宝箱消失通知
Declare_Msg_Begin(S2C_TREASURE_BOX_DISAPPEAR_NOTICE, MSG_S2C_TREASURE_BOX_DISAPPEAR_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 宝箱锁住请求
Declare_Msg_Begin(C2S_TREASURE_BOX_LOCK_REQ, MSG_C2S_TREASURE_BOX_LOCK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 宝箱锁住响应
Declare_Msg_Begin(S2C_TREASURE_BOX_LOCK_RESP, MSG_S2C_TREASURE_BOX_LOCK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid(0表示已被别的屌毛锁住)
Declare_Msg_End


// 宝箱拾取请求
Declare_Msg_Begin(C2S_TREASURE_BOX_PICK_UP_REQ, MSG_C2S_TREASURE_BOX_PICK_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 宝箱拾取响应
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
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// 信息ID
Declare_Msg_Member(vector<Item>, m_vItemList)	// 物品列表

Declare_Msg_End


// 复活请求
Declare_Msg_Begin(C2S_ATTACK_CITY_RELIVE_REQ, MSG_C2S_ATTACK_CITY_RELIVE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsFreeRelive)	// 是否免费复活
Declare_Msg_End


// 复活响应
Declare_Msg_Begin(S2C_ATTACK_CITY_RELIVE_RESP, MSG_S2C_ATTACK_CITY_RELIVE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsFreeRelive);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsFreeRelive)	// 是否免费复活
Declare_Msg_End


// 奖励领取请求
Declare_Msg_Begin(C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ, MSG_C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 章节
Declare_Msg_End


// 奖励领取响应
Declare_Msg_Begin(S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP, MSG_S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 章节
Declare_Msg_End

// 鼓舞请求
Declare_Msg_Begin(C2S_ATTACK_CITY_ENCOURAGE_REQ, MSG_C2S_ATTACK_CITY_ENCOURAGE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 鼓舞响应
Declare_Msg_Begin(S2C_ATTACK_CITY_ENCOURAGE_RESP, MSG_S2C_ATTACK_CITY_ENCOURAGE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 高排行列表通知
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
Declare_Msg_Member(WORD, m_wListNum) // 列表数量
Declare_Msg_Member_Array(ClientRoleRankData, m_arRankList, MAX_DEFAULT_RANK_MUM);	// 列表数据
Declare_Msg_End


// 攻城排名列新通知
Declare_Msg_Begin(S2C_ATTACK_CITY_RANK_UPDATE_NOTICE, MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRank);
Declare_DecodeMsg_UnsignedInt(m_dwScore);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRank);
Declare_EncodeMsg_UnsignedInt(m_dwScore);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRank)	// 排名
Declare_Msg_Member(DWORD, m_dwScore)	// 积分
Declare_Msg_End


// 攻城活动开始通知
Declare_Msg_Begin(S2C_ATTACK_CITY_ACTIVITY_START_NOTICE, MSG_S2C_ATTACK_CITY_ACTIVITY_START_NOTICE)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 攻城活动结束通知
Declare_Msg_Begin(S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE, MSG_S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 无尽之地数据请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_DATA_REQ, MSG_C2S_ENDLESS_LAND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 无尽之地数据响应
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
Declare_Msg_Member(PlayerEndlessLandData, m_tEndlessLandData)	// 玩家无尽之地数据
Declare_Msg_End


// 无尽之地进入请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_ENTER_REQ, MSG_C2S_ENDLESS_LAND_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 无尽之地进入响应
Declare_Msg_Begin(S2C_ENDLESS_LAND_ENTER_RESP, MSG_S2C_ENDLESS_LAND_ENTER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 开始战斗请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_BATTLE_START_REQ, MSG_C2S_ENDLESS_LAND_BATTLE_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 开始战斗响应
Declare_Msg_Begin(S2C_ENDLESS_LAND_BATTLE_START_RESP, MSG_S2C_ENDLESS_LAND_BATTLE_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 结束战斗请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_BATTLE_OVER_REQ, MSG_C2S_ENDLESS_LAND_BATTLE_OVER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsWin);
Declare_DecodeMsg_UnsignedShort(m_wWave);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsWin);
Declare_EncodeMsg_UnsignedShort(m_wWave);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsWin)	// 是否获胜
Declare_Msg_Member(WORD, m_wWave)	// 挑战的波数
Declare_Msg_End

// 结束战斗响应
Declare_Msg_Begin(S2C_ENDLESS_LAND_BATTLE_OVER_RESP, MSG_S2C_ENDLESS_LAND_BATTLE_OVER_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsWin);
Declare_DecodeMsg_UnsignedShort(m_wRewardWave);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsWin);
Declare_EncodeMsg_UnsignedShort(m_wRewardWave);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsWin)	// 是否获胜
Declare_Msg_Member(WORD, m_wRewardWave)	// 本次挑战奖励波数(0=表示木有奖励)
Declare_Msg_End


// 杀怪请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_KILL_MONSTER_REQ, MSG_C2S_ENDLESS_LAND_KILL_MONSTER_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIndex);
Declare_DecodeMsg_UnsignedInt(m_dwMonsterId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIndex);
Declare_EncodeMsg_UnsignedInt(m_dwMonsterId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIndex)	// 怪物的索引
Declare_Msg_Member(DWORD, m_dwMonsterId)	// 怪物Id
Declare_Msg_End


// 杀怪响应
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
Declare_Msg_Member(BYTE, m_byIndex)	// 怪物的索引
Declare_Msg_Member(DWORD, m_dwMonsterId)	// 怪物Id
Declare_Msg_Member(WORD, m_wDropItemNum)	// 掉落物品数量
Declare_Msg_Member_Array(MonsterDropData, m_DropItemList, MAX_MAP_DROP_NUM);	// 掉落物品列表
Declare_Msg_End

// 碰撞请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_COLLISION_REQ, MSG_C2S_ENDLESS_LAND_COLLISION_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 碰撞响应
Declare_Msg_Begin(S2C_ENDLESS_LAND_COLLISION_RESP, MSG_S2C_ENDLESS_LAND_COLLISION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTodayCollisionRewardTimes);
Declare_DecodeMsg_UnsignedShort(m_wRewardId);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTodayCollisionRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_wRewardId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTodayCollisionRewardTimes)	// 今日碰撞奖励次数
Declare_Msg_Member(WORD, m_wRewardId)	// 奖励Id
Declare_Msg_End

// 领取连杀奖励请求
Declare_Msg_Begin(C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ, MSG_C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wKeepKillNum)	// 连杀数量
Declare_Msg_End

// 领取连杀奖励响应
Declare_Msg_Begin(S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP, MSG_S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wKeepKillNum);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wKeepKillNum)	// 连杀数量
Declare_Msg_End


// 爵位激活请求
Declare_Msg_Begin(C2S_NOBILITY_ACTIVATE_REQ, MSG_C2S_NOBILITY_ACTIVATE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 爵位激活响应
Declare_Msg_Begin(S2C_NOBILITY_ACTIVATE_RESP, MSG_S2C_NOBILITY_ACTIVATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNobilityLevel);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNobilityLevel);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNobilityLevel)	// 爵位等级
Declare_Msg_End


// 超值折扣数据请求
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_DATA_REQ, MSG_C2S_SUPER_DISCOUNT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 超值折扣数据响应
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
Declare_Msg_Member(SuperDiscountData, m_Data)	// 爵位等级
Declare_Msg_End

// 领取超值折扣购买奖励请求
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_BUY_REWARD_REQ, MSG_C2S_SUPER_DISCOUNT_BUY_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取超值折扣购买奖励响应
Declare_Msg_Begin(S2C_SUPER_DISCOUNT_BUY_REWARD_RESP, MSG_S2C_SUPER_DISCOUNT_BUY_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取超值折扣累积奖励请求
Declare_Msg_Begin(C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ, MSG_C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// 次数
Declare_Msg_End

// 领取超值折扣累积奖励响应
Declare_Msg_Begin(S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, MSG_S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// 次数
Declare_Msg_End



// 超值折扣数据请求(新)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_DATA_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 超值折扣数据响应(新)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_DATA_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_Data.wStatus);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_Data.wStatus);
Declare_EncodeMsg_UnsignedShort(m_Data.wTakeAccumulateRewardTimes);
Declare_EncodeMsg_UnsignedShort(m_Data.wCurBuyTimes);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SuperDiscountData, m_Data)	// 爵位等级
Declare_Msg_End

// 领取超值折扣购买奖励请求(新)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取超值折扣购买奖励响应(新)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取超值折扣累积奖励请求(新)
Declare_Msg_Begin(C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ, MSG_C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取超值折扣累积奖励响应(新)
Declare_Msg_Begin(S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, MSG_S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 节日活动数据请求
Declare_Msg_Begin(C2S_FESTIVAL_ACTIVITY_DATA_REQ, MSG_C2S_FESTIVAL_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_End

// 节日活动数据响应
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
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_Member(FestivalActivity, m_tActivityData)	// 活动数据
Declare_Msg_End

// 节日活动奖励领取请求
Declare_Msg_Begin(C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ, MSG_C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_Member(WORD, m_wTargetId)	// 领取的目标ID
Declare_Msg_End

// 节日活动奖励领取响应
Declare_Msg_Begin(S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP, MSG_S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wActivityType)
Declare_DecodeMsg_UnsignedShort(m_wTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wActivityType)
Declare_EncodeMsg_UnsignedShort(m_wTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_Member(WORD, m_wTargetId)	// 领取的目标ID
Declare_Msg_End


// 节日商品兑换请求
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
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_Member(WORD, m_wTargetId)	// 领取的目标ID
Declare_Msg_Member(WORD, m_wExchangeNum)	// 兑换数量
Declare_Msg_End


// 节日商品兑换请求
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
Declare_Msg_Member(WORD, m_wActivityType)	// 活动类型
Declare_Msg_Member(WORD, m_wTargetId)	// 领取的目标ID
Declare_Msg_Member(WORD, m_wExchangeNum)	// 兑换数量
Declare_Msg_End

// 节日活动红点请求
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
Declare_Msg_Member(WORD, m_wRedPointNum)		// 红点数量
Declare_Msg_Member_Array(WORD, m_RedPointList, MAX_RED_POINT_NUM);	// 红点列表(有红点的活动类型)
Declare_Msg_End

// 精准推荐数据请求
Declare_Msg_Begin(C2S_ACCURATE_RECOMMEND_DATA_REQ, MSG_C2S_ACCURATE_RECOMMEND_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 精准推荐数据响应
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
Declare_Msg_Member(PlayerAccurateRecommendData, m_Data)	// 商品ID
Declare_Msg_End


// 领取精准推荐礼包请求
Declare_Msg_Begin(C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ, MSG_C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// 商品ID
Declare_Msg_End

// 领取精准推荐礼包响应
Declare_Msg_Begin(S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP, MSG_S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// 商品ID
Declare_Msg_End

// 宝石列表请求
Declare_Msg_Begin(C2S_GEM_LIST_REQ, MSG_C2S_GEM_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 宝石列表响应
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
Declare_Msg_Member(PlayerGemInfo, m_PlayerGemInfo)		// 玩家宝石信息
Declare_Msg_End

// 宝石合成请求
Declare_Msg_Begin(C2S_GEM_COMPOSE_REQ, MSG_C2S_GEM_COMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGemNum)
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGemNum)
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGemNum)	// 宝石数量
Declare_Msg_Member(DWORD, m_dwGemId)	// 宝石ID
Declare_Msg_End

// 宝石合成响应
Declare_Msg_Begin(S2C_GEM_COMPOSE_RESP, MSG_S2C_GEM_COMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGemNum)
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGemNum)
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGemNum)	// 宝石数量
Declare_Msg_Member(DWORD, m_dwGemId)	// 宝石ID
Declare_Msg_End

// 宝石镶嵌请求
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
Declare_Msg_Member(WORD, m_wFormationSlot)	// 阵型槽位
Declare_Msg_Member(WORD, m_wGemInlayNum)	// 宝石镶嵌数量
Declare_Msg_Member_Array(GemInlay, m_arGemInlayList, MAX_GEM_HOLE_NUM);	// 宝石镶嵌列表
Declare_Msg_End


// 宝石镶嵌响应
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
Declare_Msg_Member(WORD, m_wFormationSlot)	// 阵型槽位
Declare_Msg_Member(WORD, m_wGemInlayNum)	// 宝石镶嵌数量
Declare_Msg_Member_Array(GemInlay, m_arGemInlayList, MAX_GEM_HOLE_NUM);	// 宝石镶嵌列表
Declare_Msg_End

// 宝石卸下请求
Declare_Msg_Begin(C2S_GEM_UNLOAD_REQ, MSG_C2S_GEM_UNLOAD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// 宝石孔位
Declare_Msg_Member(WORD, m_wGemHole)	// 宝石孔位
Declare_Msg_End


// 宝石卸下响应
Declare_Msg_Begin(S2C_GEM_UNLOAD_RESP, MSG_S2C_GEM_UNLOAD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// 宝石孔位
Declare_Msg_Member(WORD, m_wGemHole)	// 宝石孔位
Declare_Msg_End


// 宝石孔位开启请求
Declare_Msg_Begin(C2S_GEM_HOLE_OPEN_REQ, MSG_C2S_GEM_HOLE_OPEN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// 阵型槽位
Declare_Msg_Member(WORD, m_wGemHole)	// 宝石孔位
Declare_Msg_End


// 宝石孔位开启响应
Declare_Msg_Begin(S2C_GEM_HOLE_OPEN_RESP, MSG_S2C_GEM_HOLE_OPEN_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wFormationSlot)
Declare_DecodeMsg_UnsignedShort(m_wGemHole)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wFormationSlot)
Declare_EncodeMsg_UnsignedShort(m_wGemHole)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wFormationSlot)	// 阵型槽位
Declare_Msg_Member(WORD, m_wGemHole)	// 宝石孔位
Declare_Msg_End

// 宝石数据通知
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
Declare_Msg_Member(DWORD, m_dwGemId)	// 宝石ID
Declare_Msg_Member(DWORD, m_dwGemNum)	// 宝石数量
Declare_Msg_Member(BYTE, m_byFlag)		// 操作标志(0=减少,1=添加或者更新)
Declare_Msg_End


// 宝石分解请求
Declare_Msg_Begin(C2S_GEM_DECOMPOSE_REQ, MSG_C2S_GEM_DECOMPOSE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_UnsignedInt(m_dwGemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_UnsignedInt(m_dwGemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGemId)	// 宝石ID
Declare_Msg_Member(DWORD, m_dwGemNum)	// 宝石数量
Declare_Msg_End


// 宝石分解响应
Declare_Msg_Begin(S2C_GEM_DECOMPOSE_RESP, MSG_S2C_GEM_DECOMPOSE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwGemId)
Declare_DecodeMsg_UnsignedInt(m_dwGemNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwGemId)
Declare_EncodeMsg_UnsignedInt(m_dwGemNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwGemId)	// 宝石ID
Declare_Msg_Member(DWORD, m_dwGemNum)	// 宝石数量
Declare_Msg_End


// 进入场景请求 
Declare_Msg_Begin(C2S_ENTER_SCENE_REQ, MSG_C2S_ENTER_SCENE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwContext)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwContext)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwContext)	// 上下文
Declare_Msg_End


// 进入场景响应
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwContext)	// 上下文
Declare_Msg_Member(MapPos, m_tMapPos) // 玩家地图坐标
Declare_Msg_Member(WORD, m_wPlayerNum) // 逻辑数据ID
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_SCENE_PLAYER_NUM)	// 玩家列表
Declare_Msg_Member_Array(MapPos, m_PlayerMapPos, MAX_SCENE_PLAYER_NUM)	// 玩家地图坐标
Declare_Msg_End

// 进入场景通知
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(PlayerCommonData, m_tPlayerData) // 玩家数据
Declare_Msg_Member(MapPos, m_tMapPos) // 玩家地图坐标
Declare_Msg_End


// 离开场景请求
Declare_Msg_Begin(C2S_LEAVE_SCENE_REQ, MSG_C2S_LEAVE_SCENE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_End


// 离开场景响应
Declare_Msg_Begin(S2C_LEAVE_SCENE_RESP, MSG_S2C_LEAVE_SCENE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_End


// 离开场景通知
Declare_Msg_Begin(S2C_LEAVE_SCENE_NOTICE, MSG_S2C_LEAVE_SCENE_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene)
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene)
Declare_EncodeMsg_UnsignedInt(m_dwRoleId);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwRoleId)	// 玩家ID
Declare_Msg_End


// 英雄行走请求
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End


// 英雄行走响应
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End


// 英雄行走通知
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwRoleId)	// 玩家ID
Declare_Msg_Member(MapPos, m_tSrcPos)	// 源点
Declare_Msg_Member(MapPos, m_tDestPos)	// 终点
Declare_Msg_End


// 怪物列表通知
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
Declare_Msg_Member(WORD, m_wMonsterNum)	// 怪物数量
Declare_Msg_Member_Array(SceneMonster, m_arMonsterList, MAX_ATTACK_CITY_MONSTER_NUM)	// 怪物列表
Declare_Msg_End


// 怪物数据通知
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
Declare_Msg_Member(SceneMonster, m_tMonster)	// 怪物数据
Declare_Msg_End


// 场景物品列表通知
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
Declare_Msg_Member(WORD, m_wSceneItemNum)	// 物品数量
Declare_Msg_Member_Array(SceneItem, m_SceneItemList, MAX_ATTACK_CITY_TREASURE_BOX_NUM)	// 物品列表
Declare_Msg_End


// 场景物品消失通知
Declare_Msg_Begin(S2C_SCENE_ITEM_DISAPPEAR_NOTICE, MSG_S2C_SCENE_ITEM_DISAPPEAR_NOTICE)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 场景物品锁住请求
Declare_Msg_Begin(C2S_SCENE_ITEM_LOCK_REQ, MSG_C2S_SCENE_ITEM_LOCK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 场景物品锁住响应
Declare_Msg_Begin(S2C_SCENE_ITEM_LOCK_RESP, MSG_S2C_SCENE_ITEM_LOCK_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid(0表示已被别的屌毛锁住)
Declare_Msg_End

// 场景物品锁住通知
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
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid(0表示已被别的屌毛锁住)
Declare_Msg_Member(DWORD, m_dwRoleId)	// 锁物品的角色ID
Declare_Msg_End


// 场景物品拾取请求
Declare_Msg_Begin(C2S_SCENE_ITEM_PICK_UP_REQ, MSG_C2S_SCENE_ITEM_PICK_UP_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wScene);
Declare_DecodeMsg_UnsignedInt(m_dwUID);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wScene);
Declare_EncodeMsg_UnsignedInt(m_dwUID);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wScene)	// 场景类型
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_End


// 场景物品拾取响应
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
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// 信息ID
Declare_Msg_Member(vector<Item>, m_vItemList)	// 物品列表
Declare_Msg_End


// 场景物品拾取通知
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
Declare_Msg_Member(DWORD, m_dwUID)	// 宝箱uid
Declare_Msg_Member(DWORD, m_dwInfoId)	// 信息ID
Declare_Msg_Member(DWORD, m_dwRoleId)	// 拾取角色ID
Declare_Msg_End


// 组队副本抽奖请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LOTTERY_REQ, MSG_C2S_TEAM_DUNGEON_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// 抽奖次数
Declare_Msg_Member(BYTE, m_byIsCostItem)	// 是否消耗道具
Declare_Msg_End

// 组队副本抽奖请求
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
Declare_Msg_Member(WORD, m_wTimes)	// 抽奖次数
Declare_Msg_Member(WORD, m_wItemNum)	// 物品数量
Declare_Msg_Member_Array(Item, m_ItemList, MAX_TEAM_DUNGEON_LOTTERY_TIMES)	// 物品列表
Declare_Msg_End

// 答题请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ, MSG_C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRight)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRight)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsRight)	// 是否答对
Declare_Msg_Member(BYTE, m_byAnswer)	// 答案
Declare_Msg_End

// 答题响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP, MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsRight)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsRight)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsRight)	// 是否答对
Declare_Msg_Member(BYTE, m_byAnswer)	// 答案
Declare_Msg_End

// 答题通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY, MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byAnswer)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byAnswer)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId)	// 答案
Declare_Msg_Member(BYTE, m_byAnswer)	// 答案
Declare_Msg_End

// 对话结束请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_DIALOG_OVER_REQ, MSG_C2S_TEAM_DUNGEON_DIALOG_OVER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 对话结束响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DIALOG_OVER_RESP, MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 对话结束响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY, MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// 下一个NPC请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_NEXT_NPC_REQ, MSG_C2S_TEAM_DUNGEON_NEXT_NPC_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// 下一个NPC响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_NEXT_NPC_RESP, MSG_S2C_TEAM_DUNGEON_NEXT_NPC_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// 下一个NPC通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY, MSG_S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wNpcId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wNpcId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wNpcId)	// NPC
Declare_Msg_End

// Boss任务开始请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ, MSG_C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

//  Boss任务开始响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP, MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// Boss任务开始通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY, MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 新任务通知
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
Declare_Msg_Member(DWORD, m_dwTaskId)	// 任务ID
Declare_Msg_Member(WORD, m_wQuestionId)	// 题目ID
Declare_Msg_Member(BYTE, m_byIsNewTask)	// 是否新任务
Declare_Msg_End

// 队伍任务数据
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
Declare_Msg_Member(DWORD, m_dwTastStartTime)	// 任务开始时间
Declare_Msg_Member(WORD, m_wTaskLinkId)	// 任务链ID
Declare_Msg_Member(WORD, m_wMemberNum)	// 成员数量
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// 成员列表
Declare_Msg_End

// 离开队伍请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LEAVE_REQ, MSG_C2S_TEAM_DUNGEON_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 离开队伍响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LEAVE_RESP, MSG_S2C_TEAM_DUNGEON_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 离开队伍通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_LEAVE_NOTIFY, MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedChar(m_byIsTaskTeam)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedChar(m_byIsTaskTeam)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwRoleId) // 离开玩家ID
Declare_Msg_Member(BYTE, m_byIsTaskTeam) // 是否任务队伍
Declare_Msg_End

// 解散队伍通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DISMISS_NOTIFY, MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedChar(m_byIsCaptainLeave)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedChar(m_byIsCaptainLeave)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(BYTE, m_byIsCaptainLeave) // 是否是队长退出
Declare_Msg_End

// 创建队伍请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_CREATE_TEAM_REQ, MSG_C2S_TEAM_DUNGEON_CREATE_TEAM_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_End

// 创建队伍响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CREATE_TEAM_RESP, MSG_S2C_TEAM_DUNGEON_CREATE_TEAM_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(DWORD, m_dwTeamId)	// 队伍ID
Declare_Msg_End

// 组队邀请请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_INVITE_REQ, MSG_C2S_TEAM_DUNGEON_INVITE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 目标ID
Declare_Msg_End

// 组队邀请响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_INVITE_RESP, MSG_S2C_TEAM_DUNGEON_INVITE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTargetId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTargetId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTargetId)	// 目标ID
Declare_Msg_End

// 组队邀请通知
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
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wServerId)	// 服务器ID
Declare_Msg_Member_Array(char, m_szRoleName, MAX_ROLE_LEN)	// 队长名称
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本id
Declare_Msg_End


// 接受组队邀请请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ, MSG_C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本id
Declare_Msg_End

// 接受组队邀请响应
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
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(WORD, m_wMemberNum)	// 成员数量
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// 成员列表
Declare_Msg_End


// 接受组队邀请通知
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
Declare_Msg_Member(PlayerCommonData, m_NewMember)	// 新成员
Declare_Msg_End

// 加入组队请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_JOIN_IN_REQ, MSG_C2S_TEAM_DUNGEON_JOIN_IN_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTeamId)
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTeamId)
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本id
Declare_Msg_End

// 加入组队响应
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
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(WORD, m_wMemberNum)	// 成员数量
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// 成员列表
Declare_Msg_End


// 加入组队通知
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
Declare_Msg_Member(PlayerCommonData, m_NewMember)	// 新成员
Declare_Msg_End

// 组队匹配请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_MATCH_REQ, MSG_C2S_TEAM_DUNGEON_MATCH_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedChar(m_byIsTeam);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedChar(m_byIsTeam);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(BYTE, m_byIsTeam)	// 是否队伍
Declare_Msg_End

// 组队匹配响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_RESP, MSG_S2C_TEAM_DUNGEON_MATCH_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedChar(m_byIsTeam);
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedChar(m_byIsTeam);
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(BYTE, m_byIsTeam)	// 是否队伍
Declare_Msg_End

// 组队取消匹配请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ, MSG_C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 组队取消匹配请求
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP, MSG_S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求玩家推荐列表请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ, MSG_C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求玩家推荐列表响应
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
Declare_Msg_Member(WORD, m_wPlayerNum)	// 玩家数量
Declare_Msg_Member_Array(PlayerCommonData, m_PlayerList, MAX_TEAM_DUNGEON_RECOMMEND_LIST_NUM)	// 玩家列表
Declare_Msg_End

// 准备队伍数据通知
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
Declare_Msg_Member(DWORD, m_dwTeamId)	// 邀请的队伍ID
Declare_Msg_Member(WORD, m_wMemberNum)	// 成员数量
Declare_Msg_Member_Array(PlayerCommonData, m_MemberList, MAX_TEAM_MEMBER_NUM)	// 成员列表
Declare_Msg_End

// 接任务请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ, MSG_C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 接任务响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP, MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 接任务通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY, MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 任务链完成通知
Declare_Msg_Begin(S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY, MSG_S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 组队副本任务奖励通知
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
Declare_Msg_Member(DWORD, m_dwEnergy)	// 法阵能量
Declare_Msg_Member(vector<Item>, m_vItemList)	// 奖励的物品列表
Declare_Msg_Member(vector<Item>, m_vExtraRewardList)	// 额外奖励列表
Declare_Msg_End

// 组队匹配通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_NOTIFY, MSG_S2C_TEAM_DUNGEON_MATCH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 取消组队匹配通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY, MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 任务开始请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_TASK_START_REQ, MSG_C2S_TEAM_DUNGEON_TASK_START_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 任务开始响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_TASK_START_RESP, MSG_S2C_TEAM_DUNGEON_TASK_START_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 任务开始通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_TASK_START_NOTIFY, MSG_S2C_TEAM_DUNGEON_TASK_START_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwTastStartTime)
Declare_EncodeMsg_UnsignedInt(m_dwTaskLinkId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(DWORD, m_dwTastStartTime)	// 任务开始时间
Declare_Msg_Member(DWORD, m_dwTaskLinkId)	// 任务链ID
Declare_Msg_End


// 副本排行榜请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_RANK_REQ, MSG_C2S_TEAM_DUNGEON_RANK_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 任务开始时间
Declare_Msg_End


// 副本排行榜响应
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
Declare_Msg_Member(WORD, m_wDungeonId)	// 任务开始时间
Declare_Msg_Member(WORD, m_wRankdNum)	// 排行榜数量
Declare_Msg_Member_Array(TeamDungeonRank, m_arRankList, MAX_TEAM_DUNGEON_NUM)	// 排行榜列表
Declare_Msg_End

// 组队副本BOSS奖励通知
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
Declare_Msg_Member(DWORD, m_dwBossDamage)	// BOSS受到的伤害
Declare_Msg_Member(DWORD, m_dwMVPId)	// MVPId
Declare_Msg_Member(DWORD, m_dwKillerId)	// 击杀者ID
Declare_Msg_Member(WORD, m_wMemberNum)	// 成员数量
Declare_Msg_Member_Array(TeamDungeonBossReward, m_MemberRewardList, MAX_TEAM_MEMBER_NUM)	// 成员奖励列表
Declare_Msg_Member(vector<Item>, m_vMVPRewardList)	// mvp奖励列表
Declare_Msg_Member(vector<Item>, m_vKillRewardList)	// 击杀奖励列表
Declare_Msg_End


// 组队副本数据请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_DATA_REQ, MSG_C2S_TEAM_DUNGEON_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 组队副本数据响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_DATA_RESP, MSG_S2C_TEAM_DUNGEON_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_tTeamDungeonData.wTodayBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_tTeamDungeonData.wTodayBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_tTeamDungeonData.wPassDungeonNum)
Declare_EncodeMsg_Memory(m_tTeamDungeonData.arPassDungeonList, m_tTeamDungeonData.wPassDungeonNum * sizeof(WORD));
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerTeamDungeonData, m_tTeamDungeonData)	// 副本天数
Declare_Msg_End


// 组队快捷聊天请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ, MSG_C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChatId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChatId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChatId)	// 聊天ID
Declare_Msg_End

// 组队快捷聊天响应
Declare_Msg_Begin(S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP, MSG_S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_dwRoleId)
Declare_DecodeMsg_UnsignedShort(m_wChatId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_dwRoleId)
Declare_EncodeMsg_UnsignedShort(m_wChatId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_dwRoleId)	// 角色ID
Declare_Msg_Member(WORD, m_wChatId)	// 聊天ID
Declare_Msg_End

// 组队副本抽奖记录请求
Declare_Msg_Begin(C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ, MSG_C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 组队副本抽奖记录响应
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
Declare_Msg_Member(WORD, m_wLotteryRecordNum)	// 抽奖记录数量
Declare_Msg_Member(vector<TeamDungeonLotteryRecord>, m_LotteryRecordList) // 抽奖记录列表
Declare_Msg_End


// 组队副本匹配目标数量通知
Declare_Msg_Begin(S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY, MSG_S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDungeonId)
Declare_DecodeMsg_UnsignedShort(m_wTargetNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDungeonId)
Declare_EncodeMsg_UnsignedShort(m_wTargetNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDungeonId)	// 副本ID
Declare_Msg_Member(WORD, m_wTargetNum)	// 目标数量
Declare_Msg_End


// 登录奖励数据请求
Declare_Msg_Begin(C2S_LOGIN_REWARD_DATA_REQ, MSG_C2S_LOGIN_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 登录奖励数据响应
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
Declare_Msg_Member(WORD, m_wLoginDays)	// 登录天数
Declare_Msg_Member(WORD, m_wRewardRecordNum)	// 奖励记录数量
Declare_Msg_Member_Array(WORD, m_RewardRecordList, MAX_LOGIN_REWARD_RECORD_NUM)	// 奖励记录列表
Declare_Msg_End

// 领取登录奖励请求
Declare_Msg_Begin(C2S_TAKE_LOGIN_REWARD_REQ, MSG_C2S_TAKE_LOGIN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDays)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDays)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDays)	// 天数
Declare_Msg_End

// 领取登录奖励响应
Declare_Msg_Begin(S2C_TAKE_LOGIN_REWARD_RESP, MSG_S2C_TAKE_LOGIN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDays)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDays)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDays)	// 天数
Declare_Msg_End

// 客户端更新通知
Declare_Msg_Begin(S2C_CLIENT_UPDATE_NOTIFY, MSG_S2C_CLIENT_UPDATE_NOTIFY)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求玩家王者赛数据
Declare_Msg_Begin(C2S_KING_MATCH_DATA_REQ, MSG_C2S_KING_MATCH_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 玩家王者赛数据响应
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
Declare_Msg_Member(WORD, m_wCurSeason)		// 当前赛季
Declare_Msg_Member(WORD, m_wMyRank)			// 我的排名
Declare_Msg_Member(DWORD, m_dwMyRankValue)	// 我的排名值
Declare_Msg_Member(PlayerKingMatchData, m_KingMatchData)	// 王者赛数据
Declare_Msg_End

// 查找目标请求
Declare_Msg_Begin(C2S_KING_MATCH_SEARCH_TARGET_REQ, MSG_C2S_KING_MATCH_SEARCH_TARGET_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 查找目标响应
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
Declare_Msg_Member(PlayerCommonData, m_TargetData)	// 目标数据
Declare_Msg_Member(WORD, m_wStageLevel)		// 段位等级
Declare_Msg_Member(BYTE, m_byStar)			// 星数
Declare_Msg_Member(WORD, m_wRank)			// 排名
Declare_Msg_Member(DWORD, m_dwRankValue)	// 排名值
Declare_Msg_End


// 领取赛季任务奖励请求
Declare_Msg_Begin(C2S_TAKE_KING_MATCH_TASK_REWARD_REQ, MSG_C2S_TAKE_KING_MATCH_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWinTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWinTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWinTimes)	// 获胜次数
Declare_Msg_End


// 领取赛季任务奖励请求
Declare_Msg_Begin(S2C_TAKE_KING_MATCH_TASK_REWARD_RESP, MSG_S2C_TAKE_KING_MATCH_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wWinTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wWinTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wWinTimes)	// 获胜次数
Declare_Msg_End


// 赛季排行榜请求
Declare_Msg_Begin(C2S_KING_MATCH_SEASON_RANK_REQ, MSG_C2S_KING_MATCH_SEASON_RANK_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 赛季排行榜响应
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
Declare_Msg_Member(WORD, m_wSeasonRankNum)	// 赛季排行数量
Declare_Msg_Member_Array(KingMatchRankRecord, m_SeasonRankList, RECENTLY_KING_MATCH_SEASON_RANK_NUM)	// 赛季排行列表
Declare_Msg_End


// 请求圣诞节数据
Declare_Msg_Begin(C2S_CHRISTMAS_DATA_REQ, MSG_C2S_CHRISTMAS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求圣诞节数据响应
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
Declare_Msg_Member(DWORD, m_dwTotalScore)		// 总积分
Declare_Msg_Member(DWORD, m_dwCurScore)			// 当前积分
Declare_Msg_Member(WORD, m_wLowerBossLevel)		// 低级boss数据
Declare_Msg_Member(DWORD, m_dwLowerBossHP)		// 低级bossHP
Declare_Msg_Member(DWORD, m_dwLowerBossDeadTime)// 低级boss死亡时间
Declare_Msg_Member(WORD, m_wHighBossLevel)		// 高级boss数据
Declare_Msg_Member(DWORD, m_dwHighBossHP)		// 高级bossHP
Declare_Msg_Member(DWORD, m_dwHighBossDeadTime)	// 高级boss死亡时间
Declare_Msg_Member(WORD, m_wAlreadyTakeLowerBossRewardLevel)		// 已领取的低级boss奖励等级
Declare_Msg_Member(WORD, m_wAlreadyTakeHighBossRewardLevel)		// 已领取的高级boss奖励等级
Declare_Msg_End


// 进入战斗请求
Declare_Msg_Begin(C2S_CHRISTMAS_BATTLE_ENTER_REQ, MSG_C2S_CHRISTMAS_BATTLE_ENTER_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 进入战斗响应
Declare_Msg_Begin(S2C_CHRISTMAS_BATTLE_ENTER_RESP, MSG_S2C_CHRISTMAS_BATTLE_ENTER_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 离开战斗请求
Declare_Msg_Begin(C2S_CHRISTMAS_BATTLE_LEAVE_REQ, MSG_C2S_CHRISTMAS_BATTLE_LEAVE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 离开战斗响应
Declare_Msg_Begin(S2C_CHRISTMAS_BATTLE_LEAVE_RESP, MSG_S2C_CHRISTMAS_BATTLE_LEAVE_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 攻击Boss请求
Declare_Msg_Begin(C2S_CHRISTMAS_ATTACK_BOSS_REQ, MSG_C2S_CHRISTMAS_ATTACK_BOSS_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_UnsignedInt(m_dwSnowBallId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_UnsignedInt(m_dwSnowBallId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_Member(DWORD, m_dwSnowBallId)	// 雪球ID
Declare_Msg_End


// 攻击Boss响应
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
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_Member(DWORD, m_dwSnowBallId)	// 雪球ID
Declare_Msg_Member(DWORD, m_dwDamage)		// 伤害
Declare_Msg_Member(Item, m_tRewardItem)		// 奖励物品
Declare_Msg_End

// 攻击Boss通知
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
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_Member_Array(char, m_szAttackerName, MAX_ROLE_LEN)	// 攻击者名称
Declare_Msg_Member(DWORD, m_dwDamage)		// 伤害
Declare_Msg_Member(Item, m_tRewardItem)		// 奖励物品
Declare_Msg_End


// 领取奖励请求
Declare_Msg_Begin(C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ, MSG_C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_End


// 领取奖励响应
Declare_Msg_Begin(S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP, MSG_S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wBossType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wBossType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_End


//  Boss死亡通知
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
Declare_Msg_Member(WORD, m_wBossType)		// boss类型
Declare_Msg_Member(DWORD, m_dwDeadTime)		// 死亡时间
Declare_Msg_Member(WORD, m_wBossLevel)		// boss等级(复活以后的Boss等级)
Declare_Msg_Member(DWORD, m_dwBossHP)		// bossHP(复活以后的BossHP)
Declare_Msg_Member(DWORD, m_dwKilLerId)		// 击杀者ID
Declare_Msg_End


// 活动数据请求
Declare_Msg_Begin(C2S_FACEBOOK_ACTIVITY_DATA_REQ, MSG_C2S_FACEBOOK_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 活动数据响应
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
Declare_Msg_Member(FacebookActivityData, m_tActivityData)		// 活动数据
Declare_Msg_End


// 领取帐号绑定奖励请求
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 领取帐号绑定奖励响应
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 领取分享奖励请求
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 领取分享奖励响应
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取评论奖励请求
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 领取评论奖励响应
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End



// 好友邀请请求
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
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendInfo)		// 邀请的好友信息
Declare_Msg_End


// 好友邀请响应
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
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendInfo)		// 邀请的好友信息
Declare_Msg_End


// 好友召唤请求
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
Declare_Msg_Member(WORD, m_wFriendNum)		// 好友数量
Declare_Msg_Member_Array(DWORD, m_arFiendList, MAX_FRIEND_NUM)	// 好友列表
Declare_Msg_End

// 好友召唤响应
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
Declare_Msg_Member(WORD, m_wFriendNum)		// 好友数量
Declare_Msg_Member_Array(DWORD, m_arFiendList, MAX_FRIEND_NUM)	// 好友列表
Declare_Msg_End


// 领取好友邀请奖励请求
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wInviteNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wInviteNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wInviteNum)		// 邀请的数量
Declare_Msg_End


// 领取好友邀请奖励响应
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wInviteNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wInviteNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wInviteNum)		// 邀请的数量
Declare_Msg_End


// 领取好友召唤奖励请求
Declare_Msg_Begin(C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ, MSG_C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSummonNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSummonNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSummonNum)		// 召回的数量
Declare_Msg_End


// 领取好友召唤奖励请求
Declare_Msg_Begin(S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP, MSG_S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wSummonNum)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wSummonNum)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wSummonNum)		// 召回的数量
Declare_Msg_End


// 已经邀请的好友记录请求
Declare_Msg_Begin(C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ, MSG_C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 已经邀请的好友记录响应
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
Declare_Msg_Member(FacebookFriendInfo, m_InviteFriendRecord)		// 邀请的好友记录
Declare_Msg_End


// 在线奖励数据请求
Declare_Msg_Begin(C2S_ONLINE_REWARD_DATA_REQ, MSG_C2S_ONLINE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 在线奖励数据响应
Declare_Msg_Begin(S2C_ONLINE_REWARD_DATA_RESP, MSG_S2C_ONLINE_REWARD_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_OnlineRewardData.dwOnlineTime)
Declare_EncodeMsg_UnsignedShort(MAX_ONLINE_REWARD_DAY)
for (int i = 0; i < MAX_ONLINE_REWARD_DAY; ++i)
{
	Declare_EncodeMsg_UnsignedShort(i + 1);// 天数
	Declare_EncodeMsg_UnsignedShort(MAX_ONLINE_REWARD_TIME_LIST);
	for (int k = 0; k < MAX_ONLINE_REWARD_TIME_LIST; ++k)
	{
		Declare_EncodeMsg_UnsignedChar(m_OnlineRewardData.arRewardRewardList[i][k]); // 标志列表
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(OnlineRewardData, m_OnlineRewardData)		// 在线奖励数据
Declare_Msg_End

// 领取在线奖励请求
Declare_Msg_Begin(C2S_TAKE_ONLINE_REWARD_REQ, MSG_C2S_TAKE_ONLINE_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取在线奖励响应
Declare_Msg_Begin(S2C_TAKE_ONLINE_REWARD_RESP, MSG_S2C_TAKE_ONLINE_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 7天登录数据请求
Declare_Msg_Begin(C2S_SEVEN_DAY_LOGIN_DATA_REQ, MSG_C2S_SEVEN_DAY_LOGIN_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 7天登录数据响应
Declare_Msg_Begin(S2C_SEVEN_DAY_LOGIN_DATA_RESP, MSG_S2C_SEVEN_DAY_LOGIN_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(MAX_SEVEN_DAY_LOGIN_DAYS); // 列表长度
for (int i = 0; i < MAX_SEVEN_DAY_LOGIN_DAYS; ++i)
{
	Declare_EncodeMsg_UnsignedChar(i + 1); // 天数
	Declare_EncodeMsg_UnsignedChar(m_Data.arDailyDataList[i]); // 每日数据
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(SevenDayLoginData, m_Data)	// 登录数据
Declare_Msg_End


// 7天登录奖励领取请求
Declare_Msg_Begin(C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ, MSG_C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// 要领取的天数
Declare_Msg_End

// 7天登录奖励领取响应
Declare_Msg_Begin(S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP, MSG_S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// 要领取的天数
Declare_Msg_End


// 请求充值回馈数据
Declare_Msg_Begin(C2S_RECHARGE_REWARD_DATA_REQ, MSG_C2S_RECHARGE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求充值回馈数据响应
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

// 领取充值奖励请求
Declare_Msg_Begin(C2S_TAKE_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// 要领取的天数
Declare_Msg_End

// 领取充值奖励响应
Declare_Msg_Begin(S2C_TAKE_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wDay)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wDay)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wDay)		// 要领取的天数
Declare_Msg_End

// 领取额外奖励
Declare_Msg_Begin(C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ, MSG_C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取额外奖励
Declare_Msg_Begin(S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP, MSG_S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求累充奖励数据
Declare_Msg_Begin(C2S_TOTAL_RECHARGE_REWARD_DATA_REQ, MSG_C2S_TOTAL_RECHARGE_REWARD_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求累充奖励数据响应
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
Declare_Msg_Member(TotalRechargeRewardData, m_Data)		// 数据
Declare_Msg_End

// 领取累充奖励请求
Declare_Msg_Begin(C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// 奖励ID
Declare_Msg_End

// 领取累充奖励响应
Declare_Msg_Begin(S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRewardId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRewardId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRewardId)		// 奖励ID
Declare_Msg_End


// 扭蛋抽奖请求
Declare_Msg_Begin(C2S_ND_LOTTERY_REQ, MSG_C2S_ND_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTimes)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTimes)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTimes)	// 抽奖次数
Declare_Msg_Member(BYTE, m_byIsCostItem)	// 是否消耗道具
Declare_Msg_End

// 扭蛋抽奖响应
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
Declare_Msg_Member(WORD, m_wTimes)	// 抽奖次数
Declare_Msg_Member(WORD, m_wItemNum)	// 物品数量
Declare_Msg_Member_Array(Item, m_ItemList, MAX_ND_LOTTERY_TIMES)	// 物品列表
Declare_Msg_End


// 扭蛋抽奖记录请求
Declare_Msg_Begin(C2S_ND_LOTTERY_RECORD_REQ, MSG_C2S_ND_LOTTERY_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 扭蛋抽奖记录响应
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
Declare_Msg_Member(WORD, m_wLotteryRecordNum)	// 抽奖记录数量
Declare_Msg_Member(vector<NDLotteryRecord>, m_LotteryRecordList) // 抽奖记录列表
Declare_Msg_End


// 热卖商品数据请求
Declare_Msg_Begin(C2S_HOT_SALE_GOODS_DATA_REQ, MSG_C2S_HOT_SALE_GOODS_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 热卖商品数据响应
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
Declare_Msg_Member(PlayerHotSaleGoodsInfo, m_Data)	// 商品ID
Declare_Msg_End


// 领取热卖商品充值奖励请求
Declare_Msg_Begin(C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ, MSG_C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// 商品ID
Declare_Msg_End


// 领取热卖商品充值奖励响应
Declare_Msg_Begin(S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP, MSG_S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wGoodsId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wGoodsId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wGoodsId)	// 商品ID
Declare_Msg_End



// 转盘数据请求
Declare_Msg_Begin(C2S_TURNTABLE_DATA_REQ, MSG_C2S_TURNTABLE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 转盘数据响应
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
Declare_Msg_Member(DWORD, m_dwCurRankScore)			// 当前排行积分
Declare_Msg_Member(WORD, m_wCurActivityDataId)	// 当前活动数据ID
Declare_Msg_Member(DWORD, m_dwDiamond)			// 奖池钻石数量
Declare_Msg_Member(vector<ClientTurntableLotteryRecord>, m_LotteryRecordList) // 抽奖记录列表
Declare_Msg_End


// 转盘抽奖请求
Declare_Msg_Begin(C2S_TURNTABLE_LOTTERY_REQ, MSG_C2S_TURNTABLE_LOTTERY_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wLotteryType)
Declare_DecodeMsg_UnsignedChar(m_byIsCostItem)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wLotteryType)
Declare_EncodeMsg_UnsignedChar(m_byIsCostItem)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wLotteryType)	// 抽奖类型
Declare_Msg_Member(BYTE, m_byIsCostItem)	// 是否消耗道具
Declare_Msg_End

// 转盘抽奖响应
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
Declare_Msg_Member(WORD, m_wLotteryType)	// 抽奖类型
Declare_Msg_Member(WORD, m_wTmp)			// 临时数据
Declare_Msg_Member(vector<TurntableLotteryResultData>, m_LotteryResultList) // 抽奖结果列表
Declare_Msg_End


// 阵营招募数据请求
Declare_Msg_Begin(C2S_CAMP_RECRUIT_DATA_REQ, MSG_C2S_CAMP_RECRUIT_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 阵营招募数据响应
Declare_Msg_Begin(S2C_CAMP_RECRUIT_DATA_RESP, MSG_S2C_CAMP_RECRUIT_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurDataId)
Declare_DecodeMsg_UnsignedShort(m_wTodayAlreadyRecruitTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurDataId)
Declare_EncodeMsg_UnsignedShort(m_wTodayAlreadyRecruitTimes)
// 积分列表
Declare_EncodeMsg_UnsignedShort(m_vCampScoreList.size())
for (int i = 0; i < m_vCampScoreList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedChar(i);	// 阵营
	Declare_EncodeMsg_UnsignedInt(m_vCampScoreList[i]); // 积分
}
// 奖励记录列表
Declare_EncodeMsg_UnsignedShort(m_RewardRecordList.size())
for (int i = 0; i < m_RewardRecordList.size(); ++i)
{
	Declare_EncodeMsg_UnsignedChar(i);	// 阵营
	vector<WORD> &vList = m_RewardRecordList[i];
	Declare_EncodeMsg_UnsignedShort(vList.size());
	for (int k = 0; k < vList.size(); ++k)
	{
		Declare_EncodeMsg_UnsignedShort(vList[k])
	}
}
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurDataId)					// 当前活动数据ID
Declare_Msg_Member(WORD, m_wTodayAlreadyRecruitTimes)	// 今日已招募次数
Declare_Msg_Member(vector<DWORD>, m_vCampScoreList)		// 阵营积分列表
Declare_Msg_Member(vector<vector<WORD>>, m_RewardRecordList)		// 奖励记录列表
Declare_Msg_End


// 阵营招募请求
Declare_Msg_Begin(C2S_CAMP_RECRUIT_REQ, MSG_C2S_CAMP_RECRUIT_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wRecuitType)
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wRecuitType)
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wRecuitType)	// 招募类型(1=普通,2=一键)
Declare_Msg_Member(WORD, m_wCamp)		// 阵营
Declare_Msg_End


// 阵营招募响应
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
Declare_Msg_Member(WORD, m_wRecuitType)			// 招募类型(1=普通,2=一键)
Declare_Msg_Member(WORD, m_wCamp)				// 阵营
Declare_Msg_Member(vector<Item>, m_vHeroList)	// 招募到的英雄列表
Declare_Msg_End



// 阵营招募积分任务奖励请求
Declare_Msg_Begin(C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ, MSG_C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_UnsignedShort(m_wRewardIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_wRewardIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// 阵营
Declare_Msg_Member(WORD, m_wRewardIdx)	// 奖励索引
Declare_Msg_End


// 阵营招募积分任务奖励响应
Declare_Msg_Begin(S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP, MSG_S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_UnsignedShort(m_wRewardIdx)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_UnsignedShort(m_wRewardIdx)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// 阵营
Declare_Msg_Member(WORD, m_wRewardIdx)	// 奖励索引
Declare_Msg_End


// 阵营招募记录请求
Declare_Msg_Begin(C2S_CAMP_RECRUIT_RECORD_REQ, MSG_C2S_CAMP_RECRUIT_RECORD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCamp)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCamp)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCamp)		// 阵营
Declare_Msg_End


// 阵营招募记录响应
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
Declare_Msg_Member(WORD, m_wCamp)		// 阵营
Declare_Msg_Member(vector<CampRecruitRecord>, m_vRecruitRecordList)	// 招募记录列表
Declare_Msg_End


// unlock活动数据请求
Declare_Msg_Begin(C2S_UNLOCK_ACTIVITY_DATA_REQ, MSG_C2S_UNLOCK_ACTIVITY_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// unlock活动数据响应
Declare_Msg_Begin(S2C_UNLOCK_ACTIVITY_DATA_RESP, MSG_S2C_UNLOCK_ACTIVITY_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_ActivityData.wIsScoreForShop)
Declare_DecodeMsg_UnsignedShort(m_ActivityData.wWatchingVideosTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_ActivityData.wIsScoreForShop)
Declare_EncodeMsg_UnsignedShort(m_ActivityData.wWatchingVideosTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(UnlockActivityData, m_ActivityData)	// 活动数据
Declare_Msg_End

// 商城评分完成通知请求
Declare_Msg_Begin(C2S_SHOP_SCORE_FINISH_REQ, MSG_C2S_SHOP_SCORE_FINISH_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 商城评分完成通知响应
Declare_Msg_Begin(S2C_SHOP_SCORE_FINISH_RESP, MSG_S2C_SHOP_SCORE_FINISH_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 观看视频通知请求
Declare_Msg_Begin(C2S_WATCHING_VIDEO_NOTIFY_REQ, MSG_C2S_WATCHING_VIDEO_NOTIFY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 观看视频通知响应
Declare_Msg_Begin(S2C_WATCHING_VIDEO_NOTIFY_RESP, MSG_S2C_WATCHING_VIDEO_NOTIFY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 新手引导更新
Declare_Msg_Begin(C2S_NEW_ROLE_GUIDE_UPDATE_REQ, MSG_C2S_NEW_ROLE_GUIDE_UPDATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_nKey)
Declare_DecodeMsg_UnsignedInt(m_nValue)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_nKey)
Declare_EncodeMsg_UnsignedInt(m_nValue)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(int, m_nKey)		// 引导Key
Declare_Msg_Member(int, m_nValue)	// 引导Value
Declare_Msg_End

// 世界等级任务数据请求
Declare_Msg_Begin(C2S_WORLD_LEVEL_TASK_DATA_REQ, MSG_C2S_WORLD_LEVEL_TASK_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 世界等级任务数据响应
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
Declare_Msg_Member(PlayerWorldLevelTaskData, m_Data)	// 引导Value
Declare_Msg_End

// 领取世界等级任务奖励请求
Declare_Msg_Begin(C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ, MSG_C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTaskId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskId)	// 任务Id
Declare_Msg_End

// 领取世界等级任务奖励响应
Declare_Msg_Begin(S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP, MSG_S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTaskId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTaskId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTaskId)	// 任务Id
Declare_Msg_End

// 领取世界等级任务最终奖励请求
Declare_Msg_Begin(C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ, MSG_C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 领取世界等级任务最终奖励响应
Declare_Msg_Begin(S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP, MSG_S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求玩家资源战数据
Declare_Msg_Begin(C2S_PLAYER_RESOURCE_WAR_DATA_REQ, MSG_C2S_PLAYER_RESOURCE_WAR_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 玩家资源战数据响应
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
Declare_Msg_Member(ResourceWarResouceData, m_RoleResourceData)	// 角色资源数据
Declare_Msg_Member(DWORD, m_dwTotalAccupyTime)			// 总的占领时间
Declare_Msg_Member(WORD, m_wTechnologyLevel)			// 科技等级
Declare_Msg_Member(WORD, m_wGuildAccupyNum)				// 工会占领人数(玩家自己占领的资源所在的章节)
Declare_Msg_Member_Array(char, m_szGuildName, MAX_GUILD_NAME_LEN) // 工会名称
Declare_Msg_End

// 请求资源战敌人列表
Declare_Msg_Begin(C2S_RESOURCE_WAR_ENEMY_LIST_REQ, MSG_C2S_RESOURCE_WAR_ENEMY_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求敌人列表响应
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
Declare_Msg_Member(vector<ResourceWarEnemyClient>, m_vEnemyList)	// 敌人列表
Declare_Msg_End


// 请求战报
Declare_Msg_Begin(C2S_RESOURCE_WAR_BATTLE_RECORD_REQ, MSG_C2S_RESOURCE_WAR_BATTLE_RECORD_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求战报响应
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
Declare_Msg_Member(vector<ResourceWardRecordClient>, m_vBattleRecordList)	// 战报列表
Declare_Msg_End


// 放弃占领的资源
Declare_Msg_Begin(C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ, MSG_C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 放弃占领的资源响应
Declare_Msg_Begin(S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP, MSG_S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 延长占领的资源
Declare_Msg_Begin(C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ, MSG_C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 延长占领的资源响应
Declare_Msg_Begin(S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP, MSG_S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求资源战章节数据
Declare_Msg_Begin(C2S_RESOURCE_WAR_CHAPTER_DATA_REQ, MSG_C2S_RESOURCE_WAR_CHAPTER_DATA_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wChapter)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wChapter)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wChapter)	// 章节
Declare_Msg_End

// 请求资源战章节数据响应
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
Declare_Msg_Member(WORD, m_wChapter)	// 章节
Declare_Msg_Member(vector<ResourceWarDataClient>, m_vChapterResourceList)	// 章节资源列表
Declare_Msg_End


// 请求资源战同类资源
Declare_Msg_Begin(C2S_RESOURCE_WAR_SAME_RESOURCE_REQ, MSG_C2S_RESOURCE_WAR_SAME_RESOURCE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wResourceType)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wResourceType)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wResourceType)	// 资源类型
Declare_Msg_End

// 请求资源战同类资源响应
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
Declare_Msg_Member(WORD, m_wResourceType)	// 资源类型
Declare_Msg_Member(vector<ResourceWarBaseData>, m_vResourceDataList)	// 资源数据列表
Declare_Msg_End

// 请求资源战工会资源
Declare_Msg_Begin(C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ, MSG_C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求资源战工会资源响应
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
Declare_Msg_Member(vector<ResourceWarBaseData>, m_vResourceDataList)	// 资源数据列表
Declare_Msg_End


// 资源战科技升级请求
Declare_Msg_Begin(C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ, MSG_C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 资源战科技升级响应
Declare_Msg_Begin(S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP, MSG_S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wTechnologyLevel)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wTechnologyLevel)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wTechnologyLevel)	// 科技等级
Declare_Msg_End


// 请求工会战角色数据
Declare_Msg_Begin(C2S_GUILD_WAR_ROLE_DATA_REQ, MSG_C2S_GUILD_WAR_ROLE_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求工会战角色数据
Declare_Msg_Begin(S2C_GUILD_WAR_ROLE_DATA_RESP, MSG_S2C_GUILD_WAR_ROLE_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_GuildWarRoleData.wBuyHighStarBattleTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wBuyHighStarBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wUsedLowStarBattleTimes)
Declare_EncodeMsg_UnsignedShort(m_GuildWarRoleData.wUsedHighStarBattleTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(RoleGuildWarData, m_GuildWarRoleData)	// 工会战角色数据
Declare_Msg_End


// 请求本军团已占领的星球
Declare_Msg_Begin(C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求本军团已占领的星球响应
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
Declare_Msg_Member(vector<GuildWarOccupyStarDataForClient>, m_vOccupyStarList)	// 占领的星球列表
Declare_Msg_End

// 请求本军团已宣战的星球列表
Declare_Msg_Begin(C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 请求本军团已宣战的星球列表响应
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
Declare_Msg_Member(vector<GuildWarDeclareWarData>, m_vDeclareWarStarList)	// 宣战星球列表
Declare_Msg_End


// 请求星球列表
Declare_Msg_Begin(C2S_GUILD_WAR_STAR_LIST_REQ, MSG_C2S_GUILD_WAR_STAR_LIST_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End


// 请求星球列表响应
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
Declare_Msg_Member(vector<GuildWarStarBaseDataForClient>, m_vOccupyStarList)	// 占领的星球列表
Declare_Msg_End


// 请求宣战
Declare_Msg_Begin(C2S_GUILD_WAR_DECLARE_WAR_REQ, MSG_C2S_GUILD_WAR_DECLARE_WAR_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// 要宣战的星球ID
Declare_Msg_End

// 宣战响应
Declare_Msg_Begin(S2C_GUILD_WAR_DECLARE_WAR_RESP, MSG_S2C_GUILD_WAR_DECLARE_WAR_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// 要宣战的星球ID
Declare_Msg_End

// 请求布防
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
Declare_Msg_Member(WORD, m_wStarId)		// 要宣战的星球ID
Declare_Msg_Member(WORD, m_wTowerType)	
Declare_Msg_Member(DWORD, m_dwRoleId)	
Declare_Msg_Member(BYTE, m_byIsToBattle)	
Declare_Msg_End


// 布防响应
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
Declare_Msg_Member(WORD, m_wStarId)		// 要宣战的星球ID
Declare_Msg_Member(WORD, m_wTowerType)
Declare_Msg_Member(DWORD, m_dwRoleId)
Declare_Msg_Member(BYTE, m_byIsToBattle)
Declare_Msg_End


// 请求撤离
Declare_Msg_Begin(C2S_GUILD_WAR_EVACUATE_REQ, MSG_C2S_GUILD_WAR_EVACUATE_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// 要宣战的星球ID
Declare_Msg_End

// 撤离响应
Declare_Msg_Begin(S2C_GUILD_WAR_EVACUATE_RESP, MSG_S2C_GUILD_WAR_EVACUATE_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// 要宣战的星球ID
Declare_Msg_End

// 请求购买对高级星球挑战次数
Declare_Msg_Begin(C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ, MSG_C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 购买对高级星球挑战次数响应
Declare_Msg_Begin(S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP, MSG_S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wCurBuyTimes)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wCurBuyTimes)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wCurBuyTimes)	// 当前已购买的次数
Declare_Msg_End

// 请求领取星球奖励
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
Declare_Msg_Member(WORD, m_wTakeType)	// 领取类型
Declare_Msg_Member(WORD, m_wStarNum)	// 星球数量
Declare_Msg_Member_Array(WORD, m_arStarList, GUILD_WAR_OCCUPY_STAR_MAX_NUM);	// 星球列表
Declare_Msg_End

// 领取星球奖励响应
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
Declare_Msg_Member(WORD, m_wTakeType)	// 领取类型
Declare_Msg_Member(WORD, m_wStarNum)	// 星球数量
Declare_Msg_Member_Array(WORD, m_arStarList, GUILD_WAR_OCCUPY_STAR_MAX_NUM);	// 星球列表
Declare_Msg_End

// 请求星球详细
Declare_Msg_Begin(C2S_GUILD_WAR_STAR_DETAIL_REQ, MSG_C2S_GUILD_WAR_STAR_DETAIL_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)	// 要请求的星球ID
Declare_Msg_End

// 星球详细响应
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
Declare_Msg_Member(WORD, m_wStarId)	// 要请求的星球ID
Declare_Msg_Member(BYTE, m_byIsBattleOver)	// 是否战斗结束
Declare_Msg_Member(StarBelongGuildData, m_tBelongGuildData)	// 获得得星球的工会数据
Declare_Msg_Member(StarAttackGuildData, m_tAttackGuildData)	// 攻击星球的工会数据
Declare_Msg_Member(vector<GuildWarStarBaseData>, m_vDeclareWarGuildList)	// 对本星球宣战的工会列表
Declare_Msg_End


// 请求工会战塔列表
Declare_Msg_Begin(C2S_GUILD_WAR_TOWER_LIST_REQ, MSG_C2S_GUILD_WAR_TOWER_LIST_REQ)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// 要请求的星球ID
Declare_Msg_End

// 工会战塔列表响应
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
Declare_Msg_Member(WORD, m_wServerId)	// 服务器ID
Declare_Msg_Member(WORD, m_wStarId)		// 要请求的星球ID
Declare_Msg_Member(vector<GuildWarTowerForClient>, m_vTowerList)	// 塔列表
Declare_Msg_End


// 星球数据更新通知
Declare_Msg_Begin(S2C_GUILD_WAR_STAR_UPDATE_NOTIFY, MSG_S2C_GUILD_WAR_STAR_UPDATE_NOTIFY)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedShort(m_wStarId)
Declare_DecodeMsg_UnsignedShort(m_wTowerId)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedShort(m_wStarId)
Declare_EncodeMsg_UnsignedShort(m_wTowerId)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(WORD, m_wStarId)		// 星球ID
Declare_Msg_Member(WORD, m_wTowerId)	// 塔ID
Declare_Msg_End


// 挂机掉落数据请求
Declare_Msg_Begin(C2S_HANG_UP_DROP_DATA_REQ, MSG_C2S_HANG_UP_DROP_DATA_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 挂机掉落数据响应
Declare_Msg_Begin(S2C_HANG_UP_DROP_DATA_RESP, MSG_S2C_HANG_UP_DROP_DATA_RESP)
Declare_DecodeMsg_Function_Begin
Declare_DecodeMsg_UnsignedInt(m_HangUpDropData.dwLastObtainRewardTime)
Declare_DecodeMsg_Function_End
Declare_EncodeMsg_Function_Begin
Declare_EncodeMsg_UnsignedInt(m_HangUpDropData.dwLastObtainRewardTime)
Declare_EncodeMsg_Function_End
Declare_Msg_Member(PlayerHangUpDropData, m_HangUpDropData)		// 挂机掉落数据
Declare_Msg_End


// 领取挂机掉落奖励请求
Declare_Msg_Begin(C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ, MSG_C2S_HANG_UP_DROP_REWARD_OBTAIN_REQ)
Declare_DecodeMsg_Empty_Function_Begin
Declare_DecodeMsg_Empty_Function_End
Declare_EncodeMsg_Empty_Function_Begin
Declare_EncodeMsg_Empty_Function_End
Declare_Msg_End

// 挂机掉落数据响应
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
Declare_Msg_Member(vector<Item>, m_vRewardItemList)		// 奖励物品列表
Declare_Msg_End