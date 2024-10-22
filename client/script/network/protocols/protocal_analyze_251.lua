Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:节日活动数据请求 消息Id: 25101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_DATA_REQ] = function()
    Net.CSFestivalActivityDataReq = Net.CSFestivalActivityDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFestivalActivityDataReq:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
        })
    end
    return Net.CSFestivalActivityDataReq
end


-- 消息说明:节日活动数据响应 消息Id: 25102
-- id:             // 目标ID
-- context:        // 上下文(阶段值,完成次数)
-- take_reward_times:      // 领取奖励次数
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_DATA_RESP] = function()
    Net.SCFestivalActivityDataResp = Net.SCFestivalActivityDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFestivalActivityDataResp:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
            "target_list__T__id@H##context@I##take_reward_times@H",--活动目标列表
        })
    end
    return Net.SCFestivalActivityDataResp
end

-- 消息说明:节日活动奖励领取请求 消息Id: 25103
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ] = function()
    Net.CSTakeFestivalActivityRewardReq = Net.CSTakeFestivalActivityRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFestivalActivityRewardReq:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
            "target_id__H",     --领取的目标ID
        })
    end
    return Net.CSTakeFestivalActivityRewardReq
end

-- 消息说明:节日活动奖励领取响应 消息Id: 25104
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP] = function()
    Net.SCTakeFestivalActivityRewardResp = Net.SCTakeFestivalActivityRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFestivalActivityRewardResp:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
            "target_id__H",     --领取的目标ID
        })
    end
    return Net.SCTakeFestivalActivityRewardResp
end

-- 消息说明:节日商品兑换请求 消息Id: 25105
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ] = function()
    Net.CSFestivalActivityGoodsExchangeReq = Net.CSFestivalActivityGoodsExchangeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFestivalActivityGoodsExchangeReq:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
            "target_id__H",     --领取的目标ID
            "exchange_num__H",  --兑换数量
        })
    end
    return Net.CSFestivalActivityGoodsExchangeReq
end

-- 消息说明:节日商品兑换响应 消息Id: 25106
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP] = function()
    Net.SCFestivalActivityGoodsExchangeResp = Net.SCFestivalActivityGoodsExchangeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFestivalActivityGoodsExchangeResp:__init()
        self:AutoConstruct({
            "activity_type__H", --活动类型
            "target_id__H",     --领取的目标ID
            "exchange_num__H",  --兑换数量
        })
    end
    return Net.SCFestivalActivityGoodsExchangeResp
end

-- 消息说明:节日活动红点请求 消息Id: 25107
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ] = function()
    Net.CSFestivalActivityRedPointReq = Net.CSFestivalActivityRedPointReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFestivalActivityRedPointReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFestivalActivityRedPointReq
end

-- 消息说明:节日活动红点响应 消息Id: 25108+
-- 1、循环单充
-- 2、循环累充
-- 3、开服限时召唤
-- 4、活动单充
-- 5、活动累充
-- 6、打折兑换（钻石换物品）
-- 7、活跃目标
-- 8、材料兑换（物品换物品）
-- 9、登陆有礼
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP] = function()
    Net.SCFestivalActivityRedPointResp = Net.SCFestivalActivityRedPointResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFestivalActivityRedPointResp:__init()
        self:AutoConstruct({
            "red_piont_list__T__activity_type@H",--红点列表
        })
    end
    return Net.SCFestivalActivityRedPointResp
end
