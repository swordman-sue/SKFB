Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:请求累充奖励数据 消息Id: 26201
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOTAL_RECHARGE_REWARD_DATA_REQ] = function()
    Net.CSTotalRechargeRewardDataReq = Net.CSTotalRechargeRewardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTotalRechargeRewardDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTotalRechargeRewardDataReq
end

-- 消息说明:请求累充奖励数据响应 消息Id: 26202
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOTAL_RECHARGE_REWARD_DATA_RESP] = function()
    Net.SCTotalRechargeRewardDataResp = Net.SCTotalRechargeRewardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTotalRechargeRewardDataResp:__init()
        self:AutoConstruct({
            "cur_data_id__H", --当前活动数据ID
            "total_amount__I", --总的充值额度
            "reward_record_list__T__reward_id@H", --奖励记录列表(奖励ID)
        })
    end
    return Net.SCTotalRechargeRewardDataResp
end

-- 消息说明:领取累充奖励请求 消息Id: 26203
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ] = function()
    Net.CSTakeTotalRechargeRewardReq = Net.CSTakeTotalRechargeRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeTotalRechargeRewardReq:__init()
        self:AutoConstruct({
            "reward_id__H", --奖励ID
        })
    end
    return Net.CSTakeTotalRechargeRewardReq
end

-- 消息说明:领取累充奖励响应 消息Id: 26204
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP] = function()
    Net.SCTakeTotalRechargeRewardResp = Net.SCTakeTotalRechargeRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeTotalRechargeRewardResp:__init()
        self:AutoConstruct({
            "reward_id__H", --奖励ID
        })
    end
    return Net.SCTakeTotalRechargeRewardResp
end