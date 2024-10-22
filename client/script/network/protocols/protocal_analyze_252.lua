Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:精准推荐数据请求 消息Id: 25201
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ACCURATE_RECOMMEND_DATA_REQ] = function()
    Net.CSAccurateRecommendDataReq = Net.CSAccurateRecommendDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAccurateRecommendDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAccurateRecommendDataReq
end

-- 消息说明:精准推荐数据响应 消息Id: 25202
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACCURATE_RECOMMEND_DATA_RESP] = function()
    Net.SCAccurateRecommendDataResp = Net.SCAccurateRecommendDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAccurateRecommendDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##buy_times@H##can_take_times@H",--商品列表

            -- goods_id:       // 商品ID
            -- buy_times:      // 已购买次数
            -- can_take_times:     // 可领取次数
        })
    end
    return Net.SCAccurateRecommendDataResp
end

-- 消息说明:领取精准推荐礼包请求 消息Id: 25203
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ] = function()
    Net.CSTakeAccurateRecommendGiftReq = Net.CSTakeAccurateRecommendGiftReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeAccurateRecommendGiftReq:__init()
        self:AutoConstruct({
            "goods_id__H",  --商品ID
        })
    end
    return Net.CSTakeAccurateRecommendGiftReq
end

-- 消息说明:领取精准推荐礼包响应 消息Id: 25204
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP] = function()
    Net.SCTakeAccurateRecommendGiftResp = Net.SCTakeAccurateRecommendGiftResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeAccurateRecommendGiftResp:__init()
        self:AutoConstruct({
            "goods_id__H",  --商品ID
        })
    end
    return Net.SCTakeAccurateRecommendGiftResp
end