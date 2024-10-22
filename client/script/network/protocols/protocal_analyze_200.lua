
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--帐号登陆: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOGIN_ACCOUNT_REQ] = function()
    Net.CSLoginAccount = Net.CSLoginAccount or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLoginAccount:__init()
        self:AutoConstruct({
            "platform_type__C", --平台类型(1=android 2=ios)
            "cp_channel_id__I", --CP渠道ID
            "sdk_channel_id__I",    --SDK渠道ID
            "type__C",  --登录类型(0=测试,1=用户名注册,2=用户名登录,3=手机登录,4=手机注册,5=一键注册)
            "code__H",  --认证码
            "account__s",   --帐号名称
            "password__s",  --帐号密码
            "ip__s",    --客户端IP
            "device_id__s", --客户端设备ID
        })
    end
    return Net.CSLoginAccount
end

--帐号登陆响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_ACCOUNT_RESP] = function()
    Net.SCLoginAccount = Net.SCLoginAccount or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginAccount:__init()
        self:AutoConstruct({        
            "account__s",   --帐号名称    
            "is_bind_mobile__C",        --是否已绑定手机号
            "server_list__T__serverid@I##head_icon@I##level@H##role_name@s", -- 服务器列表        
        })
    end
    return Net.SCLoginAccount
end

--认证码请求: 消息Id: 20018
Net.ProtocolRegistry[Net.ProtocolNo.C2S_AUTHENTICATION_CODE_REQ] = function()
    Net.CSAuthenticationCodeReq = Net.CSAuthenticationCodeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAuthenticationCodeReq:__init()
        self:AutoConstruct({         
            "mobile__s",    --手机号   
        })
    end
    return Net.CSAuthenticationCodeReq
end

--认证码响应: 消息Id: 20019
Net.ProtocolRegistry[Net.ProtocolNo.S2C_AUTHENTICATION_CODE_RESP] = function()
    Net.SCAuthenticationCodeResp = Net.SCAuthenticationCodeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAuthenticationCodeResp:__init()
        self:AutoConstruct({         
        })
    end
    return Net.SCAuthenticationCodeResp
end

--手机号绑定: 消息Id: 20020
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MOBILE_BIND_REQ] = function()
    Net.CSMobileBindReq = Net.CSMobileBindReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMobileBindReq:__init()
        self:AutoConstruct({     
            "code__H",  --认证码
            "account__s",   --帐号名称
            "mobile__s",    --手机号
        })
    end
    return Net.CSMobileBindReq
end

--手机号绑定: 消息Id: 20021
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MOBILE_BIND_RESP] = function()
    Net.SCMobileBindResp = Net.SCMobileBindResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMobileBindResp:__init()
        self:AutoConstruct({     
        })
    end
    return Net.SCMobileBindResp
end

--选择服务器: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOGIN_SELECT_SERVER_REQ] = function()
    Net.CSLoginSelectServer = Net.CSLoginSelectServer or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLoginSelectServer:__init()
        self:AutoConstruct({
            "select_server_id__I",
        })
    end
    return Net.CSLoginSelectServer
end

--选择服务器响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_SELECT_SERVER_RESP] = function()
    Net.SCLoginSelectServer = Net.SCLoginSelectServer or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginSelectServer:__init()
        self:AutoConstruct({
            "have_role__C",
        })
    end
    return Net.SCLoginSelectServer
end

--创建角色: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOGIN_CREATE_ROLE_REQ] = function()
    Net.CSLoginCreateRole = Net.CSLoginCreateRole or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLoginCreateRole:__init()
        self:AutoConstruct({
            "role_name__s",
            "sex__C",
			"type__C",	--(0=默认,1=新手引导)
        })
    end
    return Net.CSLoginCreateRole
end

--创建角色响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_CREATE_ROLE_RESP] = function()
    Net.SCLoginCreateRole = Net.SCLoginCreateRole or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginCreateRole:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCLoginCreateRole
end

--角色名称修改请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOGIN_CHANGE_ROLE_NAME_REQ] = function()
    Net.CSLoginChangeRoleName = Net.CSLoginChangeRoleName or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLoginChangeRoleName:__init()
        self:AutoConstruct({
            "role_name__s", --角色名称
        })
    end
    return Net.CSLoginChangeRoleName
end

--角色名称修改响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_CHANGE_ROLE_NAME_RESP] = function()
    Net.SCLoginChangeRoleName = Net.SCLoginChangeRoleName or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginChangeRoleName:__init()
        self:AutoConstruct({
            "result__C",    --结果(0=表示成功,1=名称已被使用,2=有敏感字)
            "role_name__s", --角色名称
        })
    end
    return Net.SCLoginChangeRoleName
end

--进入服务器通知:
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_ENTER_GAME_NOTIFY] = function()
    Net.SCLoginEnterGameNotify = Net.SCLoginEnterGameNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginEnterGameNotify:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCLoginEnterGameNotify
end

--开始游戏请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_START_GAME_REQ] = function()
    Net.CSStartGameReq = Net.CSStartGameReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSStartGameReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSStartGameReq
end

--开始游戏响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_START_GAME_RESP] = function()
    Net.SCStartGameResp = Net.SCStartGameResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCStartGameResp:__init()
        self:AutoConstruct({
            "client_version__I",    --客户端版本            
        })
    end
    return Net.SCStartGameResp
end

--消息说明: 领取活动礼包 消息Id: 20014
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_ACTIVITY_GIFT_REQ] = function()
    Net.CSTakeActivityGiftReq = Net.CSTakeActivityGiftReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeActivityGiftReq:__init()
        self:AutoConstruct({
            "code__s",      --激活码
        })
    end
    return Net.CSTakeActivityGiftReq
end

--消息说明: 领取活动礼包响应 消息Id: 20015
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_ACTIVITY_GIFT_RESP] = function()
    Net.SCTakeActivityGiftResp = Net.SCTakeActivityGiftResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeActivityGiftResp:__init()
        self:AutoConstruct({
            "gift_id__H",   --获得的礼包ID
        })
    end
    return Net.SCTakeActivityGiftResp
end

--消息说明: 请求活动公告 消息Id: 20016
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ACTIVITY_NOTICE_REQ] = function()
    Net.CSActivityNoticeReq = Net.CSActivityNoticeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSActivityNoticeReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSActivityNoticeReq
end

--消息说明: 请求活动公告响应 消息Id: 20017
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACTIVITY_NOTICE_RESP] = function()
    Net.SCActivityNoticeResp = Net.SCActivityNoticeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCActivityNoticeResp:__init()
        self:AutoConstruct({
            "notice_id__H", --公告ID(0=表示木有公告)
        })
    end
    return Net.SCActivityNoticeResp
end

--消息说明: 新一天的通知 消息Id: 20022
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEW_DAY_NOTIFY] = function()
    Net.SCNewDayNotify = Net.SCNewDayNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNewDayNotify:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCNewDayNotify
end

--消息说明: SDK登陆请求 消息Id: 20023
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SDK_LOGIN_REQ] = function()
    Net.CSSDKLoginReq = Net.CSSDKLoginReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSDKLoginReq:__init()
        self:AutoConstruct({
            "platform_type__C", --平台类型(1=android 2=ios)
            "cp_channel_id__I", --CP渠道ID
            "sdk_channel_id__I",    --SDK渠道ID
            "account__s",   --帐号名称
            "token__s", --文本上下文(token)
            "context__I",   --数字上下文(时间戳)
            "ip__s",    --客户端IP
            "device_id__s", --客户端设备ID
        })
    end
    return Net.CSSDKLoginReq
end

--消息说明: SDK登陆请求 消息Id: 20024
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SDK_LOGIN_RESP] = function()
    Net.SCSDKLoginResp = Net.SCSDKLoginResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSDKLoginResp:__init()
        self:AutoConstruct({
            "account__s",   --帐号名称
            "is_bind_mobile__C", --是否已绑定手机号
            "server_list__T__serverid@I##head_icon@I##level@H##role_name@s", -- 服务器列表
        })
    end
    return Net.SCSDKLoginResp
end

--消息说明: SDK订单号请求 消息Id: 20025
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SDK_ORDER_ID_REQ] = function()
    Net.CSSDKOrderIDReq = Net.CSSDKOrderIDReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSDKOrderIDReq:__init()
        self:AutoConstruct({
            "recharge_type__C", --充值类型(1=普通充值,2=月卡)
            "recharge_id__H", --id(月卡ID,充值Id)
            "context__I", --上下文            
        })
    end
    return Net.CSSDKOrderIDReq
end

--消息说明: SDK订单号响应 消息Id: 20026
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SDK_ORDER_ID_RESP] = function()
    Net.SCSDKOrderIDResp = Net.SCSDKOrderIDResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSDKOrderIDResp:__init()
        self:AutoConstruct({
            "order_id__s",  --订单号
            "sign__s",  --签名
            "recharge_type__C", --充值类型(1=普通充值,2=月卡)
            "recharge_id__H", --id(月卡ID,充值Id)
            "context__I", --上下文            
        })
    end
    return Net.SCSDKOrderIDResp
end

--消息说明: 充值成功上报请求 消息Id: 20027
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECHARGE_SUCCESS_REPORT_REQ] = function()
    Net.CSRechargeSuccessReportReq = Net.CSRechargeSuccessReportReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRechargeSuccessReportReq:__init()
        self:AutoConstruct({
            "ip__s",    --玩家充值时的 IP 地址
            "recharge_id__H", --商品编码
            "order_id__s",  --订单号（支付平台提供的订单号）
            "channel_type__I", --渠道代码(要求客户端用YundingKeyData.getChannel()获取最终的渠道并传给服务端)
        })
    end
    return Net.CSRechargeSuccessReportReq
end

--消息说明: 充值成功上报响应 消息Id: 20028
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECHARGE_SUCCESS_REPORT_RESP] = function()
    Net.SCRechargeSuccessReportResp = Net.SCRechargeSuccessReportResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRechargeSuccessReportResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCRechargeSuccessReportResp
end

--消息说明: 游戏中强行用户更新功能 消息Id: 20029
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CLIENT_FORCE_UPDATE_RESP] = function()
    Net.SCClientForceUpdateResp = Net.SCClientForceUpdateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCClientForceUpdateResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCClientForceUpdateResp
end
