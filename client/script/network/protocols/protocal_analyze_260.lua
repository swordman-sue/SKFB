Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:7天登录数据请求 消息Id: 26001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SEVEN_DAY_LOGIN_DATA_REQ] = function()
    Net.CSSevenDayLoginDataReq = Net.CSSevenDayLoginDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSevenDayLoginDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSSevenDayLoginDataReq
end

-- 消息说明:7天登录数据响应 消息Id: 26002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SEVEN_DAY_LOGIN_DATA_RESP] = function()
    Net.SCSevenDayLoginDataResp = Net.SCSevenDayLoginDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSevenDayLoginDataResp:__init()
        self:AutoConstruct({
            "daily_data_list__T__day@C##flag@C", --奖励记录列表[days][flag] (1=表示已登录过,2=表示已领取)
        })
    end
    return Net.SCSevenDayLoginDataResp
end

-- 消息说明:7天登录奖励领取请求 消息Id: 26003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ] = function()
    Net.CSTakeSevenDayLoginRewardReq = Net.CSTakeSevenDayLoginRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeSevenDayLoginRewardReq:__init()
        self:AutoConstruct({
            "day__H",       --要领取的天数
        })
    end
    return Net.CSTakeSevenDayLoginRewardReq
end

-- 消息说明:7天登录奖励领取响应 消息Id: 26004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP] = function()
    Net.SCTakeSevenDayLoginRewardResp = Net.SCTakeSevenDayLoginRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeSevenDayLoginRewardResp:__init()
        self:AutoConstruct({
            "day__H",       --要领取的天数
        })
    end
    return Net.SCTakeSevenDayLoginRewardResp
end