Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 充值数据请求 消息Id: 24301
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECHARGE_DATA_REQ] = function()
    Net.CSRechargeDataReq = Net.CSRechargeDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRechargeDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSRechargeDataReq
end

-- 消息说明: 充值数据响应 消息Id: 24302
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECHARGE_DATA_RESP] = function()
    Net.SCRechargeDataResp = Net.SCRechargeDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRechargeDataResp:__init()
        self:AutoConstruct({
            "recharge_list__T__recharge_id@H##times@H", -- 充值列表
        })
    end
    return Net.SCRechargeDataResp
end

-- 消息说明: 充值请求 消息Id: 24303
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECHARGE_REQ] = function()
    Net.CSRechargeReq = Net.CSRechargeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRechargeReq:__init()
        self:AutoConstruct({
            "recharge_id__H",   --充值ID
            "recharge_type__H", --充值类型
            "goods_id__H",      --商品ID
            "context__I",       --上下文
        })
    end
    return Net.CSRechargeReq
end

-- 消息说明: 充值响应 消息Id: 24304
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECHARGE_RESP] = function()
    Net.SCRechargeResp = Net.SCRechargeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRechargeResp:__init()
        self:AutoConstruct({
            "recharge_id__H",   --充值ID
            "recharge_type__H", --充值类型
            "goods_id__H",      --商品ID
            "context__I",       --上下文
        })
    end
    return Net.SCRechargeResp
end

-- 消息说明: 月卡数据请求 消息Id: 24305
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MONTH_CARD_DATA_REQ] = function()
    Net.CSMonthCardDataReq = Net.CSMonthCardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMonthCardDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSMonthCardDataReq
end

-- 消息说明: 月卡数据响应 消息Id: 24306
-- card_id@H : 月卡ID
-- buy_time@I : 购买时间
-- take_reward_time@I : 最近一次领取奖励时间
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MONTH_CARD_DATA_RESP] = function()
    Net.SCMonthCardDataResp = Net.SCMonthCardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMonthCardDataResp:__init()
        self:AutoConstruct({
            "month_card_list__T__card_id@H##buy_time@I##last_take_reward_time@I", -- 月卡列表
        })
    end
    return Net.SCMonthCardDataResp
end

-- 消息说明: 月卡购买 消息Id: 24307
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MONTH_CARD_BUY_REQ] = function()
    Net.CSMonthCardBuyReq = Net.CSMonthCardBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMonthCardBuyReq:__init()
        self:AutoConstruct({
            "card_id__H",       --月卡ID
        })
    end
    return Net.CSMonthCardBuyReq
end

-- 消息说明: 月卡购买响应 消息Id: 24308
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MONTH_CARD_BUY_RESP] = function()
    Net.SCMonthCardBuyResp = Net.SCMonthCardBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMonthCardBuyResp:__init()
        self:AutoConstruct({
            "card_id__H",       --月卡ID
        })
    end
    return Net.SCMonthCardBuyResp
end

-- 消息说明: 领取月卡奖励 消息Id: 24309
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_MONTH_CARD_REWARD_REQ] = function()
    Net.CSTakeMonthCardRewardReq = Net.CSTakeMonthCardRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeMonthCardRewardReq:__init()
        self:AutoConstruct({
            "card_id__H",       --月卡ID
        })
    end
    return Net.CSTakeMonthCardRewardReq
end

-- 消息说明: 领取月卡奖励响应 消息Id: 24310
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_MONTH_CARD_REWARD_RESP] = function()
    Net.SCTakeMonthCardRewardResp = Net.SCTakeMonthCardRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeMonthCardRewardResp:__init()
        self:AutoConstruct({
            "card_id__H",       --月卡ID
        })
    end
    return Net.SCTakeMonthCardRewardResp
end

-- 消息说明: 充值通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECHARGE_NOTIFY_RESP] = function()
    Net.SCRechargeNotifyResp = Net.SCRechargeNotifyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRechargeNotifyResp:__init()
        self:AutoConstruct({
            "recharge_type__H", --充值类型
            "recharge_id__H",   --充值ID
            "context__I",   --上下文(取订单号的时候上传的)
        })
    end
    return Net.SCRechargeNotifyResp
end