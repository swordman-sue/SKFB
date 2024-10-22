Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 摇钱树数据请求 消息Id: 22801
Net.ProtocolRegistry[Net.ProtocolNo.C2S_YCS_DATA_REQ] = function()
    Net.CSYcsDataReq = Net.CSYcsDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSYcsDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSYcsDataReq
end

-- 消息说明: 摇钱树数据响应 消息Id: 22802
Net.ProtocolRegistry[Net.ProtocolNo.S2C_YCS_DATA_RESP] = function()
    Net.SCYcsDataResp = Net.SCYcsDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCYcsDataResp:__init()
        self:AutoConstruct({
            "today_get_times__C",   --今日领取次数
            "total_get_times__C",   --总的领取次数
            "total_reward_value__I",--总的奖励累积值
            "last_get_time__I", --最近一次领取的时间
        })
    end
    return Net.SCYcsDataResp
end

-- 消息说明: 摇钱树奖励请求 消息Id: 22803
Net.ProtocolRegistry[Net.ProtocolNo.C2S_YCS_REWARD_REQ] = function()
    Net.CSYcsRewardReq = Net.CSYcsRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSYcsRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSYcsRewardReq
end

-- 消息说明: 摇钱树奖励响应 消息Id: 22804
Net.ProtocolRegistry[Net.ProtocolNo.S2C_YCS_REWARD_RESP] = function()
    Net.SCYcsRewardResp = Net.SCYcsRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCYcsRewardResp:__init()
        self:AutoConstruct({
            "type__H",  --货币类型
            "value__I", --货币值
        })
    end
    return Net.SCYcsRewardResp
end