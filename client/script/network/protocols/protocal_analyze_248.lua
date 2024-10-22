Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 次日奖励数据请求 消息Id: 24801
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NEXT_DAY_REWARD_DATA_REQ] = function()
    Net.CSNextDayRewardDataReq = Net.CSNextDayRewardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSNextDayRewardDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSNextDayRewardDataReq
end

-- 消息说明: 次日奖励数据响应 消息Id: 24802
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEXT_DAY_REWARD_DATA_RESP] = function()
    Net.SCNextDayRewardDataResp = Net.SCNextDayRewardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNextDayRewardDataResp:__init()
        self:AutoConstruct({
            "is_take_reward__H", --是否已领取奖励(0=木有领取,1=已领取)
        })
    end
    return Net.SCNextDayRewardDataResp
end

-- 消息说明: 次日奖励领取请求 消息Id: 24803
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NEXT_DAY_REWARD_TAKE_REQ] = function()
    Net.CSNextDayRewardTakeReq = Net.CSNextDayRewardTakeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSNextDayRewardTakeReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSNextDayRewardTakeReq
end

-- 消息说明: 次日奖励领取响应 消息Id: 24804
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NEXT_DAY_REWARD_TAKE_RESP] = function()
    Net.SCNextDayRewardTakeResp = Net.SCNextDayRewardTakeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNextDayRewardTakeResp:__init()
        self:AutoConstruct({

        })
    end
    return Net.SCNextDayRewardTakeResp
end