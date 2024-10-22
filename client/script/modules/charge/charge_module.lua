--
-- @brief 充值模块 
-- @author: yjg
-- @date: 2017年6月15日18:25:10
--

Modules = Modules or {}

Modules.ChargeModule = Modules.ChargeModule or BaseClass(BaseModule)

function Modules.ChargeModule:__init()
    self.__is_show_red_dot  = true

    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_CANCEL,function ( _, recharge_type, recharge_id )
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("payment_cancel"))
    end)

    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_FAILED,function ( _, recharge_type, recharge_id )
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("payment_failed"))
    end)
end

function Modules.ChargeModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 充值数据请求 消息Id: 24301
function Modules.ChargeModule:RechargeDataReq()
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECHARGE_DATA_REQ)
    GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 充值数据响应 消息Id: 24302
function Modules.ChargeModule:RechargeDataResp(info)
    self.rechargeData = info

    --统计红点-月卡
    CallModuleFunc(ModuleType.ACTIVITY, "StatisticsLunaWelfare")

    GlobalEventSystem:FireNextFrame( Macros.Event.ChargeModule.CHARGE_DATA_RESP )
end

-- 消息说明: 充值请求 消息Id: 24303
function Modules.ChargeModule:RechargeOrderIDReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECHARGE_REQ)
    data.recharge_id = info.recharge_id
    data.recharge_type = info.recharge_type  
    data.goods_id = info.goods_id or 0
    data.context = 0

    GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 充值响应 消息Id: 24304
-- 2017年6月16日17:29:52 首充不需要提示 提个毛 提个鸡
function Modules.ChargeModule:RechargeResp( info )
    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_success"))
end

-- 消息说明: 月卡数据请求 消息Id: 24305
function Modules.ChargeModule:MonthCardDataReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MONTH_CARD_DATA_REQ)
    GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 月卡数据响应 消息Id: 24306
function Modules.ChargeModule:MonthCardDataResp( info )
    self.monthCard = info

    --统计红点-月卡
    CallModuleFunc(ModuleType.ACTIVITY, "StatisticsLunaWelfare")

    GlobalEventSystem:FireNextFrame( Macros.Event.ChargeModule.CHARGE_DATA_RESP )
end

-- 消息说明: 领取月卡奖励 消息Id: 24309
function Modules.ChargeModule:TakeMonthCardRewardReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_MONTH_CARD_REWARD_REQ)
    data.card_id = info.card_id
    GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 领取月卡奖励响应 消息Id: 24310
function Modules.ChargeModule:TakeMonthCardRewardResp( info )
    local cnf = CPPGameLib.GetConfig( "MonthCard", info.card_id )
    local t1 = {}
    table.insert(t1,{ type = Macros.Game.RolePropType.DIAMOND , num = cnf.daily_reward_diamond })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

-- 消息说明: 充值通知
function Modules.ChargeModule:RechargeNotifyResp(protocol_data)
    --派发充值成功事件
    GlobalEventSystem:FireNextFrame(Macros.Event.ChargeModule.RECHARGE_SUCCESS, protocol_data.recharge_type, protocol_data.recharge_id)

    --获取充值金额
    local recharge_money
    if protocol_data.recharge_type == Macros.Game.RechargeType.MONTH_CARD then
        --月卡充值
        local config_recharge = CPPGameLib.GetConfig("MonthCard", protocol_data.recharge_id)
        if config_recharge then
            recharge_money = Modules.ChargeModule.GetMoney(config_recharge, "sell_price", CPPSystemTools.GetDefCurrencyType())
        end
    else
        --普通充值
        local config_recharge = CPPGameLib.GetConfig("Recharge", protocol_data.recharge_id)
        if config_recharge then
            recharge_money = Modules.ChargeModule.GetMoney(config_recharge, "money", CPPSystemTools.GetDefCurrencyType())
        end
    end

    CPPGameLib.Switch(protocol_data.recharge_type)
    {
        --普通充值
        [Macros.Game.RechargeType.COMMON] = function()
            --自定义事件跟踪
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_COMMON, protocol_data.recharge_id, recharge_money)                             
        end,

        --月卡充值
        [Macros.Game.RechargeType.MONTH_CARD] = function()
            --自定义事件跟踪
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_MONTH_CARD, protocol_data.recharge_id, recharge_money)                             
        end,        

        --直购特惠
        [Macros.Game.RechargeType.DIRECT_BUY] = function()
            --Adjust事件跟踪
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_DIRECT_BUY, protocol_data.recharge_id, recharge_money)
        end,

        --热卖商品
        [Macros.Game.RechargeType.HOT_SALE] = function()
            local hot_sale_good_info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP, "GetHotSaleGoodsCnf", protocol_data.context)
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_HOT_SALE, hot_sale_good_info and hot_sale_good_info.item_id or nil, recharge_money)
        end,

        --VIP今日推荐礼包
        [Macros.Game.RechargeType.RECOMMEND_BUY] = function()
            local vip_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_RECOMMEND_BUY, vip_level, recharge_money)
        end,

        --超值折扣
        [Macros.Game.RechargeType.SUPER_DISCOUNTVIEW] = function()
            CPPSdkTools.ReportAdjustEventTrack(Macros.SDK.AdjustEvent.PAY_SUPER_DISCOUNTVIEW, recharge_id, recharge_money)
        end,
    }
end

--充值处理
function Modules.ChargeModule:RechargeOrderIDReq(type, info)
    if CPPSystemTools.IsWin32() then
        return
    end    

    --请求订单号
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SDK_ORDER_ID_REQ)
    data.recharge_type = type
    data.recharge_id = info.recharge_id or 0
    data.context = info.goods_id or 0
    GlobalNetAdapter:OnSend(data)

    LogTool.LogInfo("[Modules.ChargeModule:RechargeOrderIDReq] 请求订单号 recharge_type:%d recharge_id:%d goods_id:%d", type, info.recharge_id or 0, info.goods_id or 0)    
end

--订单号反馈
function Modules.ChargeModule:RechargeOrderIDResp(protocol_data)
    LogTool.LogInfo(
        "[Modules.ChargeModule:RechargeOrderIDResp] 请求订单号反馈 recharge_type:%d recharge_id:%d order_id:%s sign:%s", 
        protocol_data.recharge_type, protocol_data.recharge_id, protocol_data.order_id, protocol_data.sign)

    --构造充值数据
    local data = {}
    data.recharge_type = protocol_data.recharge_type
    data.order_id = protocol_data.order_id
    data.sign = protocol_data.sign
    data.extra_params = protocol_data.extra_params or {}

    --充值ID、数量、游戏币名
    data.raw_recharge_id = protocol_data.recharge_id
    data.recharge_id = Modules.ChargeModule.GetRechargeID(data.raw_recharge_id)
    data.recharge_num = 1
    data.recharge_name = CPPGameLib.GetString("role_info_diamond")

    --现金的国际货币代码
    data.currency_type = CPPSystemTools.GetCurrencyType()

    --商品ID
    data.goods_id = protocol_data.context
    local config_recharge
    CPPGameLib.Switch(data.recharge_type)
    {
        --月卡购买
        [Macros.Game.RechargeType.MONTH_CARD] = function()
            config_recharge = CPPGameLib.GetConfig("MonthCard", data.raw_recharge_id)
            if config_recharge then
                data.goods_name = config_recharge.name
                data.goods_desc = config_recharge.describe[1] or ""
                data.money = Modules.ChargeModule.GetMoney(config_recharge, "sell_price", CPPSystemTools.GetDefCurrencyType())
                data.recharge_amount = config_recharge.buy_reward_diamond * data.recharge_num
            end
        end,

        --普通档位
        [Default] = function()
            config_recharge = CPPGameLib.GetConfig("Recharge", data.raw_recharge_id)

            data.goods_name = config_recharge.name
            local goods_desc = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText", data.raw_recharge_id)
            data.goods_desc = string.format(config_recharge.describe[2] or "%s" , goods_desc or "")
            data.money = Modules.ChargeModule.GetMoney(config_recharge, "money", CPPSystemTools.GetDefCurrencyType())
            data.recharge_amount = config_recharge.frecharge_diamond * data.recharge_num

            if not self:GetRechargeRecord(data.raw_recharge_id) then
                data.recharge_amount_ex = config_recharge.first_recharge_diamond * data.recharge_num
            end
        end,    
    }

    local channel_type = CPPSdkTools.GetCPChannelType()

    --是否需要以Http Post的方式来请求订单号
    if CPPSdkTools.IsReqOrderIdByHttpPost() then        
        CPPGameLib.Switch(channel_type)
        {
        }

    else
        --打开SDK支付界面
        CPPSdkTools.PayPanel(data)
    end
end

--获得月卡数据
function Modules.ChargeModule:GetMonthCard( )
    if self.monthCard then
        return self.monthCard
    end
end

--获得充值数据
function Modules.ChargeModule:GetRechargeData( )
    if self.rechargeData then
        return self.rechargeData
    end
end

--获得充值记录
function Modules.ChargeModule:GetRechargeRecord( id )
    if not self.rechargeData then
        return
    end
    for i , v in ipairs( self.rechargeData.recharge_list ) do
        if v.recharge_id == id then
            return v
        end
    end
end

--获得充值列表
function Modules.ChargeModule:GetRechargeLst()
    --充值额度上限
    local upper_limit_recharge = self:GetUpperLimitRecharge()
    local t1 = {}
    --月卡、年卡档位
    local monthCardCnf = CPPGameLib.CopyTbl( CPPGameLib.GetConfig("MonthCard", nil, nil , true ) )
    if monthCardCnf then
        for i , v in pairs( monthCardCnf ) do
            if v.sell_price <= upper_limit_recharge and GlobalCustomizedLogicAdapterMgr:Call("CheckChargeMonthCardEnabled") then
                v.cardType = v.type
                v.type = Macros.Game.RechargeType.MONTH_CARD
                v.id = i
                table.insert(t1 , v)
            end
        end
    end

    --普通档位
    local rechargeCnf = CPPGameLib.GetConfig("Recharge", nil, nil , true ) 
    for i , v in pairs( rechargeCnf ) do
        if v.money <= upper_limit_recharge then
            if v.is_hide then
            else
                v.type = Macros.Game.RechargeType.COMMON
                v.id = i
                table.insert(t1 , v)
            end
        end
    end

    table.sort( t1 , function (  a , b )
        return a.id < b .id
    end )
    return t1
end

--获取充值额度上限
function Modules.ChargeModule:GetUpperLimitRecharge()
    local config_upper_limit = CPPGameLib.GetConfig("AppleIAPUpperLimit", CPPSdkTools.GetCPChannelType(), false)
    if config_upper_limit then
        return config_upper_limit.upper_limit
    end
    return MathTool.MAX_NUM
end

--获取累充加成系数
function Modules.ChargeModule:GetAccumulatedChargeFactor()
    return ConfigAdapter.Common.GetAddUpMultiple()
end

--设置首充红点
function Modules.ChargeModule:SetFirstRechargeRed( num )
    self.firstRechargeRed = num

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.FIRST_RECHARGE )
end

--获得首充红点
function Modules.ChargeModule:GetFirstRechargeRed( )
    return self.firstRechargeRed
end

