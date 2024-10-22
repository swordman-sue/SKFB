Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 请求7天目标列表请求 消息Id: 23001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SEVEN_DAY_TARGET_LIST_REQ] = function()
    Net.CSSevenDayTargetListReq = Net.CSSevenDayTargetListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSevenDayTargetListReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSSevenDayTargetListReq
end

-- 消息说明: 请求7天目标列表响应 消息Id: 23002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SEVEN_DAY_TARGET_LIST_RESP] = function()
    Net.SCSevenDayTargetListResp = Net.SCSevenDayTargetListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSevenDayTargetListResp:__init()
        self:AutoConstruct({
            "target_list__T__id@H##value@I##is_get_reward@C", -- 目标列表
            "reward_record_list__T__completion_num@H", -- 完成度奖励记录
        })
    end
    return Net.SCSevenDayTargetListResp
end

-- 消息说明: 领取7天目标奖励请求 消息Id: 23003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SEVEN_DAY_GET_REWARD_REQ] = function()
    Net.CSSevenDayGetRewardReq = Net.CSSevenDayGetRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSevenDayGetRewardReq:__init()
        self:AutoConstruct({
            "target_id__H",     --目标Id
        })
    end
    return Net.CSSevenDayGetRewardReq
end

-- 消息说明: 领取7天目标奖励响应 消息Id: 23004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SEVEN_DAY_GET_REWARD_RESP] = function()
    Net.SCSevenDayGetRewardResp = Net.SCSevenDayGetRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSevenDayGetRewardResp:__init()
        self:AutoConstruct({
            "target_id__H",     --目标Id
        })
    end
    return Net.SCSevenDayGetRewardResp
end

-- 消息说明: 7天目标数据通知 消息Id: 23005
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SEVEN_DAY_TARGET_DATA_NOTICE] = function()
    Net.SCSevenDayTargetDataNotice = Net.SCSevenDayTargetDataNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSevenDayTargetDataNotice:__init()
        self:AutoConstruct({
            "id__H",        --目标Id
            "value__I",     --目标值
            "is_get_reward__C", --是否已领取奖励(1=已领取)
        })
    end
    return Net.SCSevenDayTargetDataNotice
end

-- 消息说明: 领取完成度奖励请求 消息Id: 23006
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SEVEN_DAY_COMPLETION_REWARD_REQ] = function()
    Net.CSSevenDayCompletionRewardReq = Net.CSSevenDayCompletionRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSevenDayCompletionRewardReq:__init()
        self:AutoConstruct({
            "completion_num__H",        --完成度数量
        })
    end
    return Net.CSSevenDayCompletionRewardReq
end

-- 消息说明: 领取完成度奖励响应 消息Id: 23007
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SEVEN_DAY_COMPLETION_REWARD_RESP] = function()
    Net.SCSevenDayCompletionRewardResp = Net.SCSevenDayCompletionRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSevenDayCompletionRewardResp:__init()
        self:AutoConstruct({
            "completion_num__H",        --完成度数量
        })
    end
    return Net.SCSevenDayCompletionRewardResp
end