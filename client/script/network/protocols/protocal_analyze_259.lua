Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:在线奖励数据请求 消息Id: 25901
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ONLINE_REWARD_DATA_REQ] = function()
    Net.CSOnlineRewardDataReq = Net.CSOnlineRewardDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSOnlineRewardDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSOnlineRewardDataReq
end

-- 消息说明:在线奖励数据响应 消息Id: 25902
-- record_data:
-- "flag__C", --flag(0=表示还没领取,1=表示已领取)
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ONLINE_REWARD_DATA_RESP] = function()
    Net.SCOnlineRewardDataResp = Net.SCOnlineRewardDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCOnlineRewardDataResp:__init()
        self:AutoConstruct({
            "online_time__I",   --在线时间(秒)
            "reward_record_list__T__day@H##record_data@D|RecordData|", --奖励记录列表[days][flag] (0=表示还没领取,1=表示已领取)
        })
    end
    return Net.SCOnlineRewardDataResp
end

-- 消息说明:领取在线奖励请求 消息Id: 25903
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_ONLINE_REWARD_REQ] = function()
    Net.CSTakeOnlineRewardReq = Net.CSTakeOnlineRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeOnlineRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeOnlineRewardReq
end

-- 消息说明:领取在线奖励响应 消息Id: 25904
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_ONLINE_REWARD_RESP] = function()
    Net.SCTakeOnlineRewardResp = Net.SCTakeOnlineRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeOnlineRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeOnlineRewardResp
end