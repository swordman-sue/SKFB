Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:登录奖励数据请求 消息Id: 25501
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOGIN_REWARD_DATA_REQ] = function()
    Net.CSLoginRewardDataReq = Net.CSLoginRewardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLoginRewardDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSLoginRewardDataReq
end

-- 消息说明:登录奖励数据响应 消息Id: 25502
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOGIN_REWARD_DATA_RESP] = function()
    Net.SCLoginRewardDataResp = Net.SCLoginRewardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLoginRewardDataResp:__init()
        self:AutoConstruct({
            "login_days__H",    --登录的天数
            "reward_record_list__T__days@H", -- 奖励记录列表
        })
    end
    return Net.SCLoginRewardDataResp
end

-- 消息说明:领取登录奖励请求 消息Id: 25503
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_LOGIN_REWARD_REQ] = function()
    Net.CSTakeLoginRewardReq = Net.CSTakeLoginRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeLoginRewardReq:__init()
        self:AutoConstruct({
            "days__H",  --领取的奖励天数
        })
    end
    return Net.CSTakeLoginRewardReq
end

-- 消息说明:领取登录奖励响应 消息Id: 25504
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_LOGIN_REWARD_RESP] = function()
    Net.SCTakeLoginRewardResp = Net.SCTakeLoginRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeLoginRewardResp:__init()
        self:AutoConstruct({
            "days__H",  --领取的奖励天数
        })
    end
    return Net.SCTakeLoginRewardResp
end