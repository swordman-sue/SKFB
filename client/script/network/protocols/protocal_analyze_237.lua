Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 等级礼包数据请求 消息Id: 23701
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ] = function()
    Net.CSLevelShopActivityDataReq = Net.CSLevelShopActivityDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLevelShopActivityDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSLevelShopActivityDataReq
end

-- 消息说明: 等级礼包数据响应 消息Id: 23702
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP] = function()
    Net.SCLevelShopActivityDataResp = Net.SCLevelShopActivityDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLevelShopActivityDataResp:__init()
        self:AutoConstruct({
            "gift_list__T__level@H##activate_time@I", -- 礼包列表
        })
    end
    return Net.SCLevelShopActivityDataResp
end

