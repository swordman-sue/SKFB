Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:超值折扣数据请求 消息Id: 25001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SUPER_DISCOUNT_DATA_REQ] = function()
    Net.CSSuperDiscountDataReq = Net.CSSuperDiscountDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscountDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSuperDiscountDataReq
end

-- 消息说明:超值折扣数据响应 消息Id: 25002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SUPER_DISCOUNT_DATA_RESP] = function()
    Net.SCSuperDiscountDataResp = Net.SCSuperDiscountDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSuperDiscountDataResp:__init()
        self:AutoConstruct({
            "status__H",                --购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
            "cur_buy_time__H",          --当前购买次数
            "took_accumulate_reward_list__T__times@H", -- 已领取累积奖励记录[领取的次数]
        })
    end
    return Net.SCSuperDiscountDataResp
end

-- 消息说明:领取超值折扣购买奖励请求 消息Id: 25003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SUPER_DISCOUNT_BUY_REWARD_REQ] = function()
    Net.CSSuperDiscountBuyRewardReq = Net.CSSuperDiscountBuyRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscountBuyRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSuperDiscountBuyRewardReq
end

-- 消息说明:领取超值折扣购买奖励响应 消息Id: 25004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SUPER_DISCOUNT_BUY_REWARD_RESP] = function()
    Net.SCSuperDiscountBuyRewardResp = Net.SCSuperDiscountBuyRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSuperDiscountBuyRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCSuperDiscountBuyRewardResp
end

-- 消息说明:领取超值折扣累积奖励请求 消息Id: 25005
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ] = function()
    Net.CSSuperDiscounitAccumulateRewardReq = Net.CSSuperDiscounitAccumulateRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscounitAccumulateRewardReq:__init()
        self:AutoConstruct({
            "times__H", --领取次数
        })
    end
    return Net.CSSuperDiscounitAccumulateRewardReq
end

-- 消息说明:领取超值折扣累积奖励响应 消息Id: 25006
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP] = function()
    Net.SCSuperDiscounitAccumulateRewardResp = Net.SCSuperDiscounitAccumulateRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSuperDiscounitAccumulateRewardResp:__init()
        self:AutoConstruct({
            "times__H", --领取次数
        })
    end
    return Net.SCSuperDiscounitAccumulateRewardResp
end