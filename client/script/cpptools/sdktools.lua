
require "cpptools/sdkconstants"

--[[
%% @module: SDK接口类
%% @author: swordman sue
%% @created: 2016-07-06
%% @description: 进一步封装CPPSdkTools
--]]

CPPSdkTools = CPPSdkTools or {}

--SDKCommonHandle中的操作类型
local SdkCommonHandleType = 
{
	-- 创角
	CT_CREATE_ROLE = 1,
	-- 进入游戏
	CT_ENTER_GAME = 2,
	-- 角色升级
	CT_LEVEL_UP = 3,
	-- 调用操作函数
	CT_CALL_FUNC = 4,
}

--SDKCommonHandle中的操作函数类型
local SdkCommonHandleFuncType = 
{
	--Unused
	FuncType_UNDEFINED = "FuncType_UNDEFINED",
	--进入论坛  
	FuncType_ENTER_BBS = "FuncType_ENTER_BBS", 
	--进入用户中心
	FuncType_ENTER_USER_CENTER = "FuncType_ENTER_USER_CENTER", 
	--显示浮动工具栏
	FuncType_SHOW_TOOLBAR = "FuncType_SHOW_TOOLBAR", 
	--隐藏浮动工具栏
	FuncType_HIDE_TOOLBAR = "FuncType_HIDE_TOOLBAR", 
	--客服中心
	FuncType_Customer_Service = "FuncType_Customer_Service", 
	--切换账号
	FuncType_SWITCH_ACCOUNT = "FuncType_SWITCH_ACCOUNT", 
	--实名认证
	FuncType_REAL_NAME_REGISTER = "FuncType_REAL_NAME_REGISTER", 
	--防沉迷
	FuncType_ANTI_ADDICTION_QUERY = "FuncType_ANTI_ADDICTION_QUERY", 
	--分享
	FuncType_SHARE = "FuncType_SHARE", 
	--公告
	FuncType_NOTICE = "FuncType_NOTICE", 				
	--绑定账号
	FuncType_BIND_ACCOUNT = "FuncType_BIND_ACCOUNT", 				

	--获取Facebook可邀请好友列表
	FuncType_FB_GET_INVITABLE_FRIENDS = "FuncType_FB_GET_INVITABLE_FRIENDS",
	--获取Facebook可召回好友列表
	FuncType_FB_GET_INACTIVE_FRIENDS = "FuncType_FB_GET_INACTIVE_FRIENDS",
	--邀请Facebook好友
	FuncType_FB_INVITE_FRIENDS = "FuncType_FB_INVITE_FRIENDS",
	--召回Facebook可好友
	FuncType_FB_RECALL_FRIENDS = "FuncType_FB_RECALL_FRIENDS",	

	--adjust事件跟踪
	FuncType_ADJUST_EVENT_TRACK = "FuncType_ADJUST_EVENT_TRACK",
	--自定义事件跟踪
	FuncType_CUSTOM_EVENT_TRACK = "FuncType_CUSTOM_EVENT_TRACK",

	--打开相册
	FuncType_PICTRUE_EVENT_OPEN = "FuncType_PICTRUE_EVENT_OPEN",
	--选择图片
	FuncType_PICTRUE_EVENT_SEND = "FuncType_PICTRUE_EVENT_SEND",

	--加载广告
	FuncType_UPLOAD_ADVERTISING = "FuncType_UPLOAD_ADVERTISING",
	--播放广告
	FuncType_PLAY_ADVERTISING = "FuncType_PLAY_ADVERTISING",
	--广告结束
	FuncType_END_ADVERTISING = "FuncType_END_ADVERTISING",
}

--支付信息
local pay_data

--SDK通用支付回调处理
local SdkCommonPayHandle

--是否发送商品ID列表到SDK(苹果内购/谷歌内购)
local ReportIAPRechargeIDMap = 
{
}

--是否上传Adjust事件跟踪
local ReportAdjustEventTrackMap = 
{

}

--获取SDK定义渠道类型
--example:接入QuickSDK的包调用此接口返回的是QK定义的渠道类型
function CPPSdkTools.GetSDKChannelType()
	if StartConfig.SDKChannelType then
		return StartConfig.SDKChannelType
	end

	local default_value = 11111
	local channel_type = tonumber(CPPSdkTools.GetChannelId()) or default_value
	if 0 == channel_type then
		return default_value
	end
	return channel_type
end

--获取CP渠道类型
function CPPSdkTools.GetCPChannelType()
	if StartConfig.CPChannelType then
		return StartConfig.CPChannelType
	end

	local default_value = 11111
	local channel_type_ex = tonumber(CPPSdkTools.GetChannelIdEx()) or default_value
	if 0 == channel_type_ex then
		return default_value
	end
	return channel_type_ex	
end

--获取登录渠道类型
function CPPSdkTools.GetChannelTypeForLogin()
	return CPPSdkTools.GetSDKChannelType()
end

--获取子渠道类型(用于渠道分包)
function CPPSdkTools.GetSubChannelType()
	if StartConfig.SubChannelType then
		return StartConfig.SubChannelType
	end

	return tonumber(CPPSdkTools.GetKeyDataChannelId()) or 1
end

--获取支付订单号请求url
function CPPSdkTools.GetPhpPayOrderUrlEx()
	--审核中
	if CPPSdkTools.IsInExamine() then
		return CPPSdkTools.GetPhpPayOrderUrl()
	end

	--非审核中
	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", cp_channel_type, false)
	if config_channel_setting and config_channel_setting.pay_order_url then
		return config_channel_setting.pay_order_url
	end
	return CPPSdkTools.GetPhpPayOrderUrl()
end

--获取支付通知回调
function CPPSdkTools.GetPhpPayNotifyUrlEx()
	--审核中
	if CPPSdkTools.IsInExamine() then
		return CPPSdkTools.GetPhpPayNotifyUrl()
	end

	--非审核中
	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", cp_channel_type, false)
	if config_channel_setting and config_channel_setting.pay_notify_url then
		return config_channel_setting.pay_notify_url
	end
	return CPPSdkTools.GetPhpPayNotifyUrl()
end

--检测定向SDK更新
function CPPSdkTools.CheckUpdateVersion()
	local force_update_map = 
	{
	}

	if not force_update_map[CPPSystemTools.system] then
		return
	end

	local force_update_info = force_update_map[CPPSystemTools.system][CPPSdkTools.GetCPChannelType()]
	if not force_update_info then
		return
	end

	if (force_update_info.app_version and force_update_info.app_version > CPPSystemTools.GetAppVersionCode()) or 
		(force_update_info.sdk_version and force_update_info.sdk_version > CPPSdkTools.GetSdkVersion()) then
		return true
	end
	return false
end

--执行定向SDK更新
function CPPSdkTools.DoSdkUpdateVersion()
	CPPGameLib.Switch(CPPSdkTools.GetCPChannelType())
	{
	}
end

--是否接入自有登陆系统
function CPPSdkTools.IsSelfLoginSystem()
	return not CPPSdkTools.IsThirdAccountPlat()
end

--是否接入QuickSDK
function CPPSdkTools.IsQuickSDK()
	return CPPSdkTools.GetSdkName() == "QuickSDK"
end

--是否国内渠道
function CPPSdkTools.IsInlandChannel()
	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	if cp_channel_type == 0 then
		return true
	end
	
	local channelSetting = CPPGameLib.GetConfig("ChannelSetting", cp_channel_type, false)
	if channelSetting and channelSetting.show_recharge then
		return true
	end
	return false
end

--是否需要以Http Post的方式来请求订单号
function CPPSdkTools.IsReqOrderIdByHttpPost()	
end

--是否开启facebook登录
function CPPSdkTools.IsFacebookLoginToggle()
end

--是否开启facebook活动(好友邀请、召回、分享)
function CPPSdkTools.IsFacebookActivityToggle()
end

--是否开启游客登录
function CPPSdkTools.IsVisitorLoginToggle()
end

--是否处于审核(appstore、googleplay)
function CPPSdkTools.IsInExamine()	
	if StartConfig.IsInExamine ~= nil then
		return StartConfig.IsInExamine
	end

	--通过后台配置检测
	local is_in_examine = tonumber(CPPResManager:GetSetting(Macros.Setting.IsInExamine)) or 0
	if is_in_examine == Macros.Global.TRUE then
		return true
	end
end

--审核状态下，特殊处理系统配置
function CPPSdkTools.HandleSystemEntryConfig(config_name, config)
	if not config_name or not config then
		return
	end

	if not CPPSdkTools.IsInExamine() then
		return
	end

    if config_name == "SystemEntry" then
        config.level = 1
        config.prepare_level = 1
    end	
end

--获取审核状态下的开放系统表
function CPPSdkTools.GetOpenSysMapInExamine()
	if not CPPSdkTools.IsInExamine() then
		return
	end

	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
	if config_channel_setting and config_channel_setting.open_sys_list then
		local open_sys_map = {}
		for _, sys_id in ipairs(config_channel_setting.open_sys_list) do
			open_sys_map[sys_id] = true
		end
		return open_sys_map
	end	
end

--是否显示渠道登录页
function CPPSdkTools.IsSDKLoginBG()
	if CPPSdkTools.IsSelfLoginSystem() then
		--使用原版资源		
		return false
	end

	if CPPSdkTools.IsInExamine() then
		--AppStore审核中，使用渠道资源
		return true
	end

	--部分联运渠道要求，一直使用渠道资源
	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
	if config_channel_setting and config_channel_setting.use_channel_login_bg == Macros.Global.TRUE then
		return true
	end
end

--是否显示渠道加载页
function CPPSdkTools.IsSDKLoadingBG()
	if CPPSdkTools.IsSelfLoginSystem() then
		--使用原版资源
		return false
	end

	if CPPSdkTools.IsInExamine() then
		--AppStore审核中，使用渠道资源
		return true
	end

	--部分联运渠道要求，一直使用渠道资源
	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
	if config_channel_setting and config_channel_setting.use_channel_loading_bg == Macros.Global.TRUE then
		return true
	end
end

--是否显示渠道logo
function CPPSdkTools.IsSDKLogo()
	if CPPSdkTools.IsSelfLoginSystem() then
		--使用原版资源
		return false
	end

	if CPPSdkTools.IsInExamine() then
		--AppStore审核中，使用渠道资源
		return true
	end

	--部分联运渠道要求，一直使用渠道资源
	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
	if config_channel_setting and config_channel_setting.use_channel_logo == Macros.Global.TRUE then
		return true
	end
end

--是否显示切换账号按钮
function CPPSdkTools.IsShowSwitchAccountBtn()
	if CPPSdkTools.IsSelfLoginSystem() then
		return true
	end

	local sdk_channel_type = CPPSdkTools.GetSDKChannelType()
	if CPPSdkTools.IsQuickSDK() and 
		(sdk_channel_type == Macros.SDK.QKChannelType.MI or 
			sdk_channel_type == Macros.SDK.QKChannelType.YOUXI_FAN) then
		return false
	end

	return true
end

--是否显示游戏公告按钮
function CPPSdkTools.IsShowActivityNoticeBtn()
	if CPPSdkTools.IsSelfLoginSystem() then
		return true
	end
	return true
end

--是否显示提交建议按钮
function CPPSdkTools.IsShowSuggestionBtn()
	if CPPSdkTools.IsInExamine() then
		return
	end
end

--是否显示首充界面价格单位
function CPPSdkTools.IsShowFirstRechargeUnit()
	local cp_channel_type_list = {}

	local flag = true	
	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	for i , v in ipairs( cp_channel_type_list ) do
		if v == cp_channel_type then
			flag = false
			break
		end
	end

	return flag
end

--------------------------------------------------------------------------------------------------------------------------------
--通用操作
--------------------------------------------------------------------------------------------------------------------------------
--上传创建角色
function CPPSdkTools.ReportCreateRole()
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end

	local info = {}
	info.type = SdkCommonHandleType.CT_CREATE_ROLE
	CPPSdkTools.BuildRoleInfo(info)

	--渠道数据统计
	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))
end

--上传进入游戏
function CPPSdkTools.ReportEnterGame()
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end
	
	local info = {}
	info.type = SdkCommonHandleType.CT_ENTER_GAME
	CPPSdkTools.BuildRoleInfo(info)

	--渠道数据统计
	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))
end

--上传角色升级
function CPPSdkTools.ReportRoleLevelUp()	
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end

	local info = {}
	info.type = SdkCommonHandleType.CT_LEVEL_UP
	CPPSdkTools.BuildRoleInfo(info)
	info.levelup_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

	--渠道数据统计
	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))
end

--上传VIP等级变化
function CPPSdkTools.ReportVipLevelUp()	
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end
end


--上传注销账号
function CPPSdkTools.ReportLogout()
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end
end

--上传钻石变化
function CPPSdkTools.ReportDiamondChange(old_var, new_var)	
	if CPPSdkTools.IsSelfLoginSystem() then
		return
	end
end

--上传Adjust事件跟踪
function CPPSdkTools.ReportAdjustEventTrack(event_type, data_id, revenues)		
	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	if not ReportAdjustEventTrackMap[cp_channel_type] then
		return
	end

	local config_adjust_event = CPPGameLib.GetConfig("AdjustEventTrack", event_type, false)
	if not config_adjust_event then
		return
	end

	data_id = data_id or 1
	local event_info = config_adjust_event.data_id_list[data_id]
	if not event_info then
		return
	end

	local event_code = CPPSystemTools.IsAndroid() and event_info.event_code_android or event_info.event_code_ios
	if not event_code then
		return
	end

	revenues = revenues or 0
	revenues = revenues * 100

	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_ADJUST_EVENT_TRACK
	info.event_code = event_code
	info.revenues = revenues
	info.currency = Macros.Game.CashCurrencyType.USD

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))		
end

--打开相册
function CPPSdkTools.OpenAlbum( )	
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_PICTRUE_EVENT_OPEN

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))		
end

--进入客服中心
function CPPSdkTools.EnterCustomerService()
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_Customer_Service

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--绑定账号
function CPPSdkTools.BindAccount()
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_BIND_ACCOUNT

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--分享
function CPPSdkTools.ShareLink(url)
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_SHARE
	info.url = url

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--获取FB未安装应用好友列表
function CPPSdkTools.GetFBInvitableFriendsList(user_id, is_first_page)
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_FB_GET_INVITABLE_FRIENDS
	info.user_id = user_id
	info.is_first_page = is_first_page or 1

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--邀请好友
function CPPSdkTools.SendFBInviteRequest(title, message, friends_list)
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_FB_INVITE_FRIENDS
	info.title = title
	info.message = message
	info.friends_list = friends_list

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--获取FB已安装应用好友列表
function CPPSdkTools.GetFBInactiveFriendsList(user_id, is_first_page)
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_FB_GET_INACTIVE_FRIENDS
	info.user_id = user_id
	info.is_first_page = is_first_page or 1

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--召回好友
function CPPSdkTools.SendFBRecallRequest(title, message, friends_list)
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_FB_RECALL_FRIENDS
	info.title = title
	info.message = message
	info.friends_list = friends_list

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--加载广告
function CPPSdkTools.UploadAdvertising()
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_UPLOAD_ADVERTISING
	CPPSdkTools.BuildRoleInfo(info)

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--播放广告
function CPPSdkTools.PlayAdvertising()
	local info = {}
	info.type = SdkCommonHandleType.CT_CALL_FUNC
	info.func_type = SdkCommonHandleFuncType.FuncType_PLAY_ADVERTISING
	CPPSdkTools.BuildRoleInfo(info)

	CPPSdkTools.CommonHandle(CPPGameLib.JSON.encode(info))	
end

--------------------------------------------------------------------------------------------------------------------------------
--登录、支付、通用操作回调
--------------------------------------------------------------------------------------------------------------------------------
--处理CommonHandleCallback
function CPPSdkTools.HandleCommonHandleCallback(code, json_str)
	local func_type
	local json_data, pos, err = CPPGameLib.JSON.decode(json_str)
	if json_data and not err then
		func_type = json_data.type
	else
		func_type = json_str
	end

	CPPGameLib.Switch(func_type)
	{
		--客服中心
		[SdkCommonHandleFuncType.FuncType_Customer_Service] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_FAILED then
				--拉起SDK客服中心失败，打开CP客服弹窗
				CallModuleFunc(ModuleType.SUNDRY, "ConnectGM_CP")								
			end
		end,		

		--绑定账号
		[SdkCommonHandleFuncType.FuncType_BIND_ACCOUNT] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
				--账号绑定成功
				CallModuleFunc(ModuleType.LOGIN, "ModifyLoginInfo", "is_visitor", false)

				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("facebook_binding_success"))	            

			else
				if json_data.fb_account_has_binded == Macros.Global.TRUE then
					--facebook账号已被绑定
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("facebook_binding_exist_tips"))
				else
					--账号绑定失败
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("facebook_binding_failed"))
				end
			end
		end,		

		--获取未安装应用好友列表
		[SdkCommonHandleFuncType.FuncType_FB_GET_INVITABLE_FRIENDS] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_get_fb_invite_list_error_tips"))
			end
		end,		

		--邀请Facebook好友
		[SdkCommonHandleFuncType.FuncType_FB_INVITE_FRIENDS] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
			else 
			end
		end,

		--获取Facebook可召回好友列表
		[SdkCommonHandleFuncType.FuncType_FB_GET_INACTIVE_FRIENDS] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_get_fb_summon_list_error_tips"))
			end
		end,		

		--召回Facebook可好友
		[SdkCommonHandleFuncType.FuncType_FB_RECALL_FRIENDS] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
			else 
			end
		end,				

		--分享
		[SdkCommonHandleFuncType.FuncType_SHARE] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
			end
		end,

		--选择相片
		[SdkCommonHandleFuncType.FuncType_PICTRUE_EVENT_SEND] = function()
			if code == Macros.SDK.Constants.COMMON_CALLFUNC_SUCCESS then
				CallModuleFunc(ModuleType.SUNDRY, "UploadAlbumImage", json_data)									
			end
		end,

		--加载广告
		[SdkCommonHandleFuncType.FuncType_UPLOAD_ADVERTISING] = function()
			if code == Macros.SDK.Constants.SUCCESS_CODE then
			end
		end,

		--广告结束
		[SdkCommonHandleFuncType.FuncType_END_ADVERTISING] = function()
			if code == Macros.SDK.Constants.SUCCESS_CODE then
			end
		end,
	}
end

--打开SDK登陆界面
function CPPSdkTools.LoginPanelEx(params)
	Macros.SDK.Constants.is_sdk_logining = true

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.OPEN_SDK_LOGIN_VIEW)									

	if params then
		local json_param = CPPGameLib.JSON.encode(params)
		CPPSdkTools.LoginPannelWithParam(json_param)
	else
		CPPSdkTools.LoginPanel()
	end
end

--切换SDK账号
function CPPSdkTools.SwitchAccountEx()
	Macros.SDK.Constants.is_sdk_logining = true
	
	CPPSdkTools.SwitchAccount()		
end

--是否走CP实现的苹果内购流程
function CPPSdkTools.IsCPIAPToggle()
	if not CPPSystemTools.IsApptorePackage() then
		return
	end

	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	return Macros.SDK.CPIAPToggleMap[cp_channel_type]
end

--打开SDK支付界面
function CPPSdkTools.PayPanel(data, is_sp_handle)
	pay_data = data

	--构造支付信息
    CPPSdkTools.BuildPayInfo(data, is_sp_handle)

    --调用SDK支付接口
    local json_str = CPPGameLib.JSON.encode(data)
    CPPSdkTools.Pay(json_str)
end

--处理PayHandleCallback
function CPPSdkTools.HandlePayHandleCallback(code, json_str)
	SdkCommonPayHandle(code, json_str)

    pay_data = nil
end

--构造SDK所需的角色信息
function CPPSdkTools.BuildRoleInfo(info)
	if not info then
		return
	end

	local server_info = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo")
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	if not server_info or not role_info then
		return
	end

	info.account = role_info.account
	info.server_id = server_info.id
	info.server_name = server_info.name
	info.role_id = role_info.role_id
	info.role_name = role_info.role_name
	info.role_level = role_info.level
	info.vip_level = role_info.vip_level
	info.vip_exp = role_info.vip_exp
	info.game_money = role_info.diamond
	info.game_money_ii = role_info.gold
	info.guild_id = role_info.guild_id
	info.guild_name = "" --TODO：获取公会名称
	info.role_create_time = role_info.role_create_time
	info.sex = 0
	info.battle_value = role_info.battle_value
	info.guild_role_id = "0"
	info.guild_role_name = "无"
	info.profession = "0"
	info.profession_name = "无"
	info.friend_list = ""
	info.online_time = 0
	info.levelup_time = 0 
	info.ip = GlobalNetAdapter:GetIPAddress()

	return true
end

--构造SDK所需充值信息
function CPPSdkTools.BuildPayInfo(info, is_sp_handle)
	if not info then
		return
	end

	CPPSdkTools.BuildRoleInfo(info)

	local cp_channel_type = CPPSdkTools.GetCPChannelType()
	local sdk_channel_type = CPPSdkTools.GetSDKChannelType()

	if not is_sp_handle then
		--传透参数
		--------------------------------------------------------------------------------------------------------------------------------
		local extra_params = {}
		extra_params.role_id = info.role_id or 0
		--充值ID
		extra_params.recharge_id = info.raw_recharge_id or 0
		--充值类型
		extra_params.recharge_type = info.recharge_type or Macros.Game.RechargeType.COMMON
		--商品ID
		extra_params.goods_id = info.goods_id or 0

		--SDK渠道类型
	    extra_params.channel_type = sdk_channel_type

	    --CP渠道类型
	    extra_params.cp_channel_type = cp_channel_type

	    --CP订单号
	    extra_params.cp_order_id = info.order_id

	    --是否审核中
	    extra_params.is_in_examine = CPPSdkTools.IsInExamine() and Macros.Global.TRUE or Macros.Global.FALSE

	    extra_params.ip = GlobalNetAdapter:GetIPAddress()

		--URLEncode穿透参数
	    if not CPPSdkTools.IsQuickSDK() then
			info.extra_params = CPPGameLib.UrlEncode(CPPGameLib.JSON.encode(extra_params))		
		else
			info.extra_params = CPPGameLib.JSON.encode(extra_params)		
		end
	end

	--------------------------------------------------------------------------------------------------------------------------------
	--当前商品ID、商品ID列表(苹果内购/谷歌内购)
	--------------------------------------------------------------------------------------------------------------------------------	
	if ReportIAPRechargeIDMap[cp_channel_type] and not info.is_mycard then
		local recharge_id_map = Modules.ChargeModule.GetRechargeIDMap()
		if recharge_id_map then
			local iap_info = {}
			local recharge_id_list = {}
		    for k, v in pairs(recharge_id_map) do
		    	if type(v) == "table" then
			    	table.insert(recharge_id_list, v)
			    end
		    end
			table.sort(recharge_id_list, SortTools.KeyLowerSorter("recharge_id"))
			for _, recharge_info in ipairs(recharge_id_list) do
				if info.recharge_id == recharge_info.recharge_id then
					iap_info.cur_iap_id = recharge_info.iap_recharge_id
				end
				iap_info.iap_id_list = iap_info.iap_id_list or {}
				table.insert(iap_info.iap_id_list, recharge_info.iap_recharge_id)
			end

			info.iap_info = CPPGameLib.JSON.encode(iap_info)
		end
	end
	--------------------------------------------------------------------------------------------------------------------------------	

	info.notify_url = CPPSdkTools.GetPhpPayNotifyUrlEx()
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--SDK通用支付回调处理
SdkCommonPayHandle = function(code, json_str)	
    CPPGameLib.Switch(code)
    {
        --充值成功
        [Macros.SDK.Constants.SUCCESS_CODE] = function()
        end,

        --充值取消
        [Macros.SDK.Constants.CANCEL_CODE] = function()
            LogTool.LogInfo("SDK充值取消")
            GlobalEventSystem:FireNextFrame(Macros.Event.ChargeModule.RECHARGE_CANCEL, pay_data and pay_data.recharge_type, pay_data and pay_data.raw_recharge_id)
        end,

        --充值失败
        [Macros.SDK.Constants.FAILURE_CODE] = function()
            LogTool.LogInfo("SDK充值失败")
            GlobalEventSystem:FireNextFrame(Macros.Event.ChargeModule.RECHARGE_FAILED, pay_data and pay_data.recharge_type, pay_data and pay_data.raw_recharge_id)
        end,

        [Default] = function()
        end
    }
end
