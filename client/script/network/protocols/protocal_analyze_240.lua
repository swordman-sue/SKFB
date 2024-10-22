Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 等级礼包数据请求 消息Id: 24001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LEVEL_GIFT_DATA_REQ] = function()
    Net.CSLevelGiftDataReq = Net.CSLevelGiftDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLevelGiftDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSLevelGiftDataReq
end

-- 消息说明: 等级礼包数据响应 消息Id: 24002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LEVEL_GIFT_DATA_RESP] = function()
    Net.SCLevelGiftDataResp = Net.SCLevelGiftDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLevelGiftDataResp:__init()
        self:AutoConstruct({
            "level_gift_take_record__T__level@H", -- 等级礼包领取记录列表
        })
    end
    return Net.SCLevelGiftDataResp
end

-- 消息说明: 领取等级礼包请求 消息Id: 24003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_LEVEL_GIFT_REWARD_REQ] = function()
    Net.CSTakeLevelGiftRewardReq = Net.CSTakeLevelGiftRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeLevelGiftRewardReq:__init()
        self:AutoConstruct({
            "level__H", --领取的等级
        })
    end
    return Net.CSTakeLevelGiftRewardReq
end

-- 消息说明: 领取等级礼包请求响应 消息Id: 24004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_LEVEL_GIFT_REWARD_RESP] = function()
    Net.SCTakeLevelGiftRewardResp = Net.SCTakeLevelGiftRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeLevelGiftRewardResp:__init()
        self:AutoConstruct({
            "level__H", --领取的等级
        })
    end
    return Net.SCTakeLevelGiftRewardResp
end