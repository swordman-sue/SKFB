
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--爬塔数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_TOWER_DATA_REQ] = function()
    Net.CSFbTowerDataReq = Net.CSFbTowerDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFbTowerDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFbTowerDataReq
end

-- 爬塔数据响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_TOWER_DATA_RESP] = function()
    Net.SCFbTowerDataResp = Net.SCFbTowerDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFbTowerDataResp:__init()
        self:AutoConstruct({
            "map_star_list__T__star@C",         -- 关卡星数列表
            "cur_map_level__C",         --当前关卡等级(已经打过的)
            "map_level_record__C",          --关卡等级记录
            "keep_three_star_passed_record__C", --连续三星通关记录(章节)(0=表示木可以扫荡的章节s)
            "select_pro_list__T__type@H##value@I",  -- 选择的属性列表
            "is_buy_star_goods__C",         --是否已经购买星数商品
            "star_goods_index__C",          --星数商品索引
            "is_need_reset__C",         --是否需要重置
            "today_reset_times__C",         --今日重置次数
            "cur_star__H",              --当前星数(选择属性扣掉剩下的)
            "star_max_record__H",           --历史最佳星数记录
            "elite_map_level__H",           --精英关卡等级(已经打过的)
            "elite_map_battle_times__C",        --精英关卡已经挑战的次数
            "buy_elite_map_battle_times__C",    --已经购买的精英关卡挑战次数
            "rand_pro_list__T__pro_index@C",    --随机属性索引列表(3星,6星,9星)
        })
    end
    return Net.SCFbTowerDataResp
end

--  购买挑战次数请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOWER_BUY_TIMES_REQ] = function()
    Net.CSTowerBuyTimesReq = Net.CSTowerBuyTimesReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTowerBuyTimesReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTowerBuyTimesReq
end

--  购买挑战次数响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOWER_BUY_TIMES_RESP] = function()
    Net.SCTowerBuyTimesResp = Net.SCTowerBuyTimesResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTowerBuyTimesResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTowerBuyTimesResp
end

--  重置关卡
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOWER_RESET_REQ] = function()
    Net.CSTowerResetReq = Net.CSTowerResetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTowerResetReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTowerResetReq
end

--  重置关卡响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOWER_RESET_RESP] = function()
    Net.SCTowerResetResp = Net.SCTowerResetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTowerResetResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTowerResetResp
end

--  选择属性
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOWER_SELECT_PRO_REQ] = function()
    Net.CSTowerSelectProReq = Net.CSTowerSelectProReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTowerSelectProReq:__init()
        self:AutoConstruct({
            "star__C",          --星数
            "pro_index__C",         --属性索引
        })
    end
    return Net.CSTowerSelectProReq
end

--  选择属性响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOWER_SELECT_PRO_RESP] = function()
    Net.SCTowerSelectProResp = Net.SCTowerSelectProResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTowerSelectProResp:__init()
        self:AutoConstruct({
            "star__C",          --星数
            "pro_index__C",         --属性索引
        })
    end
    return Net.SCTowerSelectProResp
end

--  购买星数商品
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOWER_BUY_STAR_GOODS_REQ] = function()
    Net.CSTowerBuyStarGoosReq = Net.CSTowerBuyStarGoosReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTowerBuyStarGoosReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTowerBuyStarGoosReq
end

--  购买星数商品响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOWER_BUY_STAR_GOODS_RESP] = function()
    Net.SCTowerBuyStarGoosResp = Net.SCTowerBuyStarGoosResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTowerBuyStarGoosResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTowerBuyStarGoosResp
end

--  爬塔一键扫荡请求 消息Id: 21811
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TOWER_ONE_KEY_CLEAN_REQ] = function()
    Net.CSTowerOneKeyCleanReq = Net.CSTowerOneKeyCleanReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTowerOneKeyCleanReq:__init()
        self:AutoConstruct({
            "is_to_history_record__C",  --是否扫荡到历史记录(1=是)
        })
    end
    return Net.CSTowerOneKeyCleanReq
end

--  爬塔一键扫荡响应 消息Id: 21812
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TOWER_ONE_KEY_CLEAN_RESP] = function()
    Net.SCTowerOneKeyCleanResp = Net.SCTowerOneKeyCleanResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTowerOneKeyCleanResp:__init()
        self:AutoConstruct({
            "is_to_history_record__C",  --是否扫荡到历史记录(1=是)
            "begin__C",         --开始
            "ended__C",           --结束
        })
    end
    return Net.SCTowerOneKeyCleanResp
end