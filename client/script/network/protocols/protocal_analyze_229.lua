Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 铜雀台数据请求 消息Id: 22901
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TQT_DATA_REQ] = function()
    Net.C2STqtDdataReq = Net.C2STqtDdataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.C2STqtDdataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.C2STqtDdataReq
end

-- 消息说明: 铜雀台数据响应 消息Id: 22902
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TQT_DATA_RESP] = function()
    Net.SCTqtDataResp = Net.SCTqtDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTqtDataResp:__init()
        self:AutoConstruct({
            "last_get_time__I", --最近一次领取的时间
            "take_record_list__T__id@H", -- 领取记录列表
        })
    end
    return Net.SCTqtDataResp
end

-- 消息说明: 铜雀台奖励请求 消息Id: 22903
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TQT_REWARD_REQ] = function()
    Net.CSTqtRewardReq = Net.CSTqtRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTqtRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTqtRewardReq
end

-- 消息说明: 铜雀台奖励响应 消息Id: 22904
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TQT_REWARD_RESP] = function()
    Net.SCTqtRewardResp = Net.SCTqtRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTqtRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTqtRewardResp
end