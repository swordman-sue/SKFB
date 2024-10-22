Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 签到数据请求 消息Id: 22701
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SIGN_IN_DATA_REQ] = function()
    Net.CSSignInDataReq = Net.CSSignInDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSignInDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSSignInDataReq
end

-- 消息说明: 签到数据响应 消息Id: 22702
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SIGN_IN_DATA_RESP] = function()
    Net.SCSignInDataResp = Net.SCSignInDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSignInDataResp:__init()
        self:AutoConstruct({
            "cur_sign_in_day__C",   --当前签到的天数
            "last_sign_in_time__I", --最近签到的时间
        })
    end
    return Net.SCSignInDataResp
end

-- 消息说明: 签到请求 消息Id: 22703
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SIGN_IN_REQ] = function()
    Net.CSSignInReq = Net.CSSignInReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSignInReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSignInReq
end

-- 消息说明: 签到响应 消息Id: 22704
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SIGN_IN_RESP] = function()
    Net.SCSignInResp = Net.SCSignInResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSignInResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCSignInResp
end

-- 消息说明: 豪华签到数据请求 消息Id: 22705
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LUXURY_SIGN_IN_DATA_REQ] = function()
    Net.CSLuxurySignInDataReq = Net.CSLuxurySignInDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLuxurySignInDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSLuxurySignInDataReq
end

-- 消息说明: 豪华签到数据响应 消息Id: 22706
-- sign_in_id@H    // 签到Id
-- status@H    // 奖励状态(0=未达到领取条件,1=可以领取,2=已领取)
-- reward_id@H // 奖励Id
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LUXURY_SIGN_IN_DATA_RESP] = function()
    Net.SCLuxurySignInDataResp = Net.SCLuxurySignInDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLuxurySignInDataResp:__init()
        self:AutoConstruct({
            "sign_in_data_list__T__sign_in_id@H##status@H##reward_id@H", -- 签到数据列表
        })
    end
    return Net.SCLuxurySignInDataResp
end

-- 消息说明: 领取豪华签到奖励 消息Id: 22707
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ] = function()
    Net.CSTakeLuxurySignInRewardReq = Net.CSTakeLuxurySignInRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeLuxurySignInRewardReq:__init()
        self:AutoConstruct({
            "sign_in_id__H",    --签到ID
        })
    end
    return Net.CSTakeLuxurySignInRewardReq
end

-- 消息说明: 领取豪华签到奖励响应 消息Id: 22708
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP] = function()
    Net.SCTakeLuxurySignInRewardResp = Net.SCTakeLuxurySignInRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeLuxurySignInRewardResp:__init()
        self:AutoConstruct({
            "sign_in_id__H",    --签到ID
        })
    end
    return Net.SCTakeLuxurySignInRewardResp
end