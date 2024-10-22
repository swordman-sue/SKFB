Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:超值折扣数据请求 消息Id: 26301
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_DATA_REQ] = function()
    Net.CSSuperDiscountDataReq = Net.CSSuperDiscountDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscountDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSuperDiscountDataReq
end

-- 消息说明:超值折扣数据响应 消息Id: 26302
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_DATA_RESP] = function()
    Net.SCSuperDiscountDataResp = Net.SCSuperDiscountDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSuperDiscountDataResp:__init()
        self:AutoConstruct({
            "status__H",                --购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
            "take_accumulate_reward_times__H",  --已领取累积奖励的次数
            "cur_buy_time__H",          --当前购买次数(领取奖励时会重置)
        })
    end
    return Net.SCSuperDiscountDataResp
end

-- 消息说明:领取超值折扣购买奖励请求 消息Id: 26303
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ] = function()
    Net.CSSuperDiscountBuyRewardReq = Net.CSSuperDiscountBuyRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscountBuyRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSuperDiscountBuyRewardReq
end


-- 消息说明:领取超值折扣购买奖励响应 消息Id: 26304
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP] = function()
    Net.SuperDiscountBuyRewardResp = Net.SuperDiscountBuyRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SuperDiscountBuyRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SuperDiscountBuyRewardResp
end

-- 消息说明:领取超值折扣累积奖励请求 消息Id: 26305
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ] = function()
    Net.CSSuperDiscountAccumulateRewardReq = Net.CSSuperDiscountAccumulateRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSuperDiscountAccumulateRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSuperDiscountAccumulateRewardReq
end

-- 消息说明:领取超值折扣累积奖励响应 消息Id: 26306
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP] = function()
    Net.SCSuperDiscountAccumulateRewardResp = Net.SCSuperDiscountAccumulateRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSuperDiscountAccumulateRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCSuperDiscountAccumulateRewardResp
end