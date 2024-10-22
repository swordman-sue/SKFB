Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: VIP礼包数据请求 消息Id: 23601
Net.ProtocolRegistry[Net.ProtocolNo.C2S_VIP_GIFT_DATA_REQ] = function()
    Net.CSVipGiftDataReq = Net.CSVipGiftDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSVipGiftDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSVipGiftDataReq
end

-- 消息说明: VIP礼包数据响应 消息Id: 23602
-- id@H : 礼包ID
-- num@H : 礼包数量
Net.ProtocolRegistry[Net.ProtocolNo.S2C_VIP_GIFT_DATA_RESP] = function()
    Net.SCVipGiftDataResp = Net.SCVipGiftDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCVipGiftDataResp:__init()
        self:AutoConstruct({
            "daily_init_vip_level__H",  --每日初始VIP等级
            "vip_level_reward_record__h",   --VIP等级奖励记录(已经领取的,-1=表示还没有领取)
            "week_gift_list__T__id@H##num@H", -- 每周礼包列表
            "today_recommend_gift_id__H",   --今日推荐礼包Id
            "today_recommend_gift_status__H",   --今日推荐礼包状态(0=不能领取,1=可领取,2=已领取)
        })
    end
    return Net.SCVipGiftDataResp
end

-- 消息说明: VIP每日礼包领取 消息Id: 23603
Net.ProtocolRegistry[Net.ProtocolNo.C2S_VIP_DAILY_GIFT_GET_REQ] = function()
    Net.CSVipDailyGiftGetReq = Net.CSVipDailyGiftGetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSVipDailyGiftGetReq:__init()
        self:AutoConstruct({
            "vip_level__H", --VIP等级
        })
    end
    return Net.CSVipDailyGiftGetReq
end

-- 消息说明: VIP每日礼包领取响应 消息Id: 23604
Net.ProtocolRegistry[Net.ProtocolNo.S2C_VIP_DAILY_GIFT_GET_RESP] = function()
    Net.SCVipDailyGiftGetResp = Net.SCVipDailyGiftGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCVipDailyGiftGetResp:__init()
        self:AutoConstruct({
            "vip_level__H", --VIP等级
        })
    end
    return Net.SCVipDailyGiftGetResp
end

-- 消息说明: VIP周礼包购买 消息Id: 23605
Net.ProtocolRegistry[Net.ProtocolNo.C2S_VIP_WEEK_GIFT_BUY_REQ] = function()
    Net.CSVipWeekGiftBuyReq = Net.CSVipWeekGiftBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSVipWeekGiftBuyReq:__init()
        self:AutoConstruct({
            "gift_id__H",   --礼包ID
        })
    end
    return Net.CSVipWeekGiftBuyReq
end

-- 消息说明: VIP周礼包购买响应 消息Id: 23606
Net.ProtocolRegistry[Net.ProtocolNo.S2C_VIP_WEEK_GIFT_BUY_RESP] = function()
    Net.SCVipWeekGiftBuyResp = Net.SCVipWeekGiftBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCVipWeekGiftBuyResp:__init()
        self:AutoConstruct({
            "gift_id__H",   --礼包ID
        })
    end
    return Net.SCVipWeekGiftBuyResp
end

-- 消息说明: 领取今日推荐礼包 消息Id: 23607
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_TODAY_RECOMMEND_GIFT_REQ] = function()
    Net.CSTakeTodayRecommendGiftReq = Net.CSTakeTodayRecommendGiftReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeTodayRecommendGiftReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeTodayRecommendGiftReq
end

-- 消息说明: 领取今日推荐礼包响应 消息Id: 23608
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_TODAY_RECOMMEND_GIFT_RESP] = function()
    Net.SCTakeTodayRecommendGiftResp = Net.SCTakeTodayRecommendGiftResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeTodayRecommendGiftResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeTodayRecommendGiftResp
end