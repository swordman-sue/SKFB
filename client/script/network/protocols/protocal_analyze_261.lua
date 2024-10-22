Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:请求充值回馈数据 消息Id: 261001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECHARGE_REWARD_DATA_REQ] = function()
    Net.CSRechargeRewardDataReq = Net.CSRechargeRewardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRechargeRewardDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSRechargeRewardDataReq
end

-- 消息说明:充值回馈数据响应 消息Id: 261002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECHARGE_REWARD_DATA_RESP] = function()
    Net.SCRechargeRewardDataResp = Net.SCRechargeRewardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRechargeRewardDataResp:__init()
        self:AutoConstruct({
            "activity_data_id__H", --活动数据ID
            "is_took_extra_reward__C", --是否已领取额外奖励
            "daily_data_list__T__data@C", --每日数据列表(0=还没充值,1=已充值,2=已领取
        })
    end
    return Net.SCRechargeRewardDataResp
end

-- 消息说明:领取充值奖励请求 消息Id: 261003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_RECHARGE_REWARD_REQ] = function()
    Net.CSTakeRechargeRewardReq = Net.CSTakeRechargeRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeRechargeRewardReq:__init()
        self:AutoConstruct({
            "day__H", --要领取的天数 
        })
    end
    return Net.CSTakeRechargeRewardReq
end

-- 消息说明:领取充值奖励响应 消息Id: 261004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_RECHARGE_REWARD_RESP] = function()
    Net.SCTakeRechargeRewardResp = Net.SCTakeRechargeRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeRechargeRewardResp:__init()
        self:AutoConstruct({
            "day__H", --要领取的天数
        })
    end
    return Net.SCTakeRechargeRewardResp
end

-- 消息说明:领取额外奖励请求 消息Id: 261005
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ] = function()
    Net.CSTakeRechargeExtraRewardReq = Net.CSTakeRechargeExtraRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeRechargeExtraRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeRechargeExtraRewardReq
end

-- 消息说明:领取额外奖励响应 消息Id: 261006
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP] = function()
    Net.SCTakeRechargeExtraRewardResp = Net.SCTakeRechargeExtraRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeRechargeExtraRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeRechargeExtraRewardResp
end