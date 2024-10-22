Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 首充数据请求 消息Id: 24501
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FIRST_RECHARGE_DATA_REQ] = function()
    Net.CSFirstRechargeDataReq = Net.CSFirstRechargeDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFirstRechargeDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFirstRechargeDataReq
end

-- 消息说明: 首充数据响应 消息Id: 24502
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FIRST_RECHARGE_DATA_RESP] = function()
    Net.SCFirstRechargeDataResp = Net.SCFirstRechargeDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFirstRechargeDataResp:__init()
        self:AutoConstruct({
		"total_recharge__I",	--总的充值
		"reward_record_list__T__data_id@H",--奖励记录(已领取的)
        })
    end
    return Net.SCFirstRechargeDataResp
end

-- 消息说明: 单充数据请求 消息Id: 24503
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SINGLE_RECHARGE_DATA_REQ] = function()
    Net.CSSingleRechargeDataReq = Net.CSSingleRechargeDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSingleRechargeDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSingleRechargeDataReq
end

-- 消息说明: 单充数据响应 消息Id: 24504
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SINGLE_RECHARGE_DATA_RESP] = function()
    Net.SCSingleRechargeDataResp = Net.SCSingleRechargeDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSingleRechargeDataResp:__init()
        self:AutoConstruct({
		"reward_record_list__T__data_id@H##num@H",--奖励记录(已领取的)
		"recharge_list__T__id@H##times@H",--充值列表[充值Id,充值次数]
        })
    end
    return Net.SCSingleRechargeDataResp
end

-- 消息说明: 累充数据请求 消息Id: 24505
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ACCUMULATE_RECHARGE_DATA_REQ] = function()
    Net.CSAccumulateRechargeDataReq = Net.CSAccumulateRechargeDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAccumulateRechargeDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAccumulateRechargeDataReq
end

-- 消息说明: 累充数据响应 消息Id: 24506
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACCUMULATE_RECHARGE_DATA_RESP] = function()
    Net.SCAccumulateRechargeDataResp = Net.SCAccumulateRechargeDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAccumulateRechargeDataResp:__init()
        self:AutoConstruct({
            "accumulate_recharge__I",   --累积充值
            "reward_record_list__T__data_id@H",--奖励记录(已领取的)
        })
    end
    return Net.SCAccumulateRechargeDataResp
end

-- 消息说明: 限时招募数据请求 消息Id: 24507
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LIMIT_RECRUIT_DATA_REQ] = function()
    Net.CSLimitRecruitDataReq = Net.CSLimitRecruitDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLimitRecruitDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSLimitRecruitDataReq
end

-- 消息说明: 限时招募数据响应 消息Id: 24508
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LIMIT_RECRUIT_DATA_RESP] = function()
    Net.SCLimitRecruitDataResp = Net.SCLimitRecruitDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLimitRecruitDataResp:__init()
        self:AutoConstruct({
            "recruit_times__H", --招募次数
            "reward_record_list__T__data_id@H",--奖励记录(已领取的)
        })
    end
    return Net.SCLimitRecruitDataResp
end

-- 消息说明: 领取限时活动奖励请求 消息Id: 24509
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ] = function()
    Net.CSTakeLimitActivityRewardReq = Net.CSTakeLimitActivityRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeLimitActivityRewardReq:__init()
        self:AutoConstruct({
            "data_id__H",   --数据ID
        })
    end
    return Net.CSTakeLimitActivityRewardReq
end

-- 消息说明: 领取限时活动奖励响应 消息Id: 24510
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP] = function()
    Net.SCTakeLimitActivityRewardResp = Net.SCTakeLimitActivityRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeLimitActivityRewardResp:__init()
        self:AutoConstruct({
            "data_id__H",   --数据ID
        })
    end
    return Net.SCTakeLimitActivityRewardResp
end