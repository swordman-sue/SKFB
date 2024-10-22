Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 开服基金数据请求 消息Id: 23801
Net.ProtocolRegistry[Net.ProtocolNo.C2S_OPEN_SERVER_FUND_DATA_REQ] = function()
    Net.CSOpenServerFundDataReq = Net.CSOpenServerFundDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSOpenServerFundDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSOpenServerFundDataReq
end

-- 消息说明: 开服基金数据响应 消息Id: 23802
Net.ProtocolRegistry[Net.ProtocolNo.S2C_OPEN_SERVER_FUND_DATA_RESP] = function()
    Net.SCOpenServerFundDataResp = Net.SCOpenServerFundDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCOpenServerFundDataResp:__init()
        self:AutoConstruct({
            "fund_buy_tims__I", --基金购买次数
            "is_already_buy_fund__C",   --是否已购买基金
            "fund_reward_take_record__T__reward_id@H", -- 基金奖励领取记录
            "all_people_welfare_take_record__T__welfare_id@H", -- 全民福利领取记录
        })
    end
    return Net.SCOpenServerFundDataResp
end

-- 消息说明: 开服基金购买请求 消息Id: 23803
Net.ProtocolRegistry[Net.ProtocolNo.C2S_OPEN_SERVER_FUND_BUY_REQ] = function()
    Net.CSOpenServerFundBuyReq = Net.CSOpenServerFundBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSOpenServerFundBuyReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSOpenServerFundBuyReq
end

-- 消息说明: 开服基金购买响应 消息Id: 23804
Net.ProtocolRegistry[Net.ProtocolNo.S2C_OPEN_SERVER_FUND_BUY_RESP] = function()
    Net.SCOpenServerFundBuyResp = Net.SCOpenServerFundBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCOpenServerFundBuyResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCOpenServerFundBuyResp
end

-- 消息说明: 开服基金奖励领取请求 消息Id: 23805
Net.ProtocolRegistry[Net.ProtocolNo.C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ] = function()
    Net.CSOpenServerFundRewardTakeReq = Net.CSOpenServerFundRewardTakeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSOpenServerFundRewardTakeReq:__init()
        self:AutoConstruct({
            "reward_id__H", --奖励ID
        })
    end
    return Net.CSOpenServerFundRewardTakeReq
end

-- 消息说明: 开服基金奖励领取响应 消息Id: 23806
Net.ProtocolRegistry[Net.ProtocolNo.S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP] = function()
    Net.SCOpenServerFundRewardTakeResp = Net.SCOpenServerFundRewardTakeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCOpenServerFundRewardTakeResp:__init()
        self:AutoConstruct({
            "reward_id__H", --奖励ID
        })
    end
    return Net.SCOpenServerFundRewardTakeResp
end

-- 消息说明: 全民福利领取请求 消息Id: 23807
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ALL_PEOPLE_WELFARE_TAKE_REQ] = function()
    Net.CSAllPeopleWelfareTakeReq = Net.CSAllPeopleWelfareTakeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAllPeopleWelfareTakeReq:__init()
        self:AutoConstruct({
            "welfare_id__H",    --奖励ID
        })
    end
    return Net.CSAllPeopleWelfareTakeReq
end

-- 消息说明: 全民福利领取响应 消息Id: 23808
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ALL_PEOPLE_WELFARE_TAKE_RESP] = function()
    Net.SCAllPeopleWelfareTakeResp = Net.SCAllPeopleWelfareTakeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAllPeopleWelfareTakeResp:__init()
        self:AutoConstruct({
            "welfare_id__H",    --奖励ID
        })
    end
    return Net.SCAllPeopleWelfareTakeResp
end