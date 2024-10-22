Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 卡牌列表请求 消息Id: 23103
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CARD_LIST_REQ] = function()
    Net.CSCardListReq = Net.CSCardListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCardListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSCardListReq
end

-- uid : CARD唯一ID
-- info_id : 信息ID
-- level : CARD等级
-- exp : CARD经验
-- break_level: 突破等级
-- hero_id: 穿戴的英雄ID
-- 消息说明: 卡牌列表响应 消息Id: 23104
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CARD_LIST_RESP] = function()
    Net.SCCardListResp = Net.SCCardListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCardListResp:__init()
        self:AutoConstruct({
            "card_list__T__uid@I##info_id@I##level@H##exp@I##break_level@H##hero_id@I", -- 占卜列表
        })
    end
    return Net.SCCardListResp
end

-- 消息说明: 卡牌升级请求 消息Id: 23107
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CARD_LEVEL_UP_REQ] = function()
    Net.CSCardLevelUpReq = Net.CSCardLevelUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCardLevelUpReq:__init()
        self:AutoConstruct({
            "uid__I",   --升级的卡牌UID
            "cost_card_list__T__id@I", -- 作为消耗的卡牌列表
        })
    end
    return Net.CSCardLevelUpReq
end

-- 消息说明: 卡牌升级响应 消息Id: 23108
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CARD_LEVEL_UP_RESP] = function()
    Net.SCCardLevelUpResp = Net.SCCardLevelUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCardLevelUpResp:__init()
        self:AutoConstruct({
            "fairy_uid__I",     --升级的卡牌UID
            "fairy_level__H",   --卡牌当前等级
            "fairy_exp__I",     --卡牌当前经验
            "cost_card_list__T__id@I", -- 作为消耗的卡牌列表
        })
    end
    return Net.SCCardLevelUpResp
end

-- 消息说明: 卡牌突破请求 消息Id: 23109
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CARD_BREAK_REQ] = function()
    Net.CSCardBreakReq = Net.CSCardBreakReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCardBreakReq:__init()
        self:AutoConstruct({
            "card_uid__I",  --卡牌UID
        })
    end
    return Net.CSCardBreakReq
end

-- 消息说明: 卡牌突破响应 消息Id: 23110
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CARD_BREAK_RESP] = function()
    Net.SCCardBreakResp = Net.SCCardBreakResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCardBreakResp:__init()
        self:AutoConstruct({
            "card_uid__I",  --卡牌UID
        })
    end
    return Net.SCCardBreakResp
end

-- 消息说明: 卡牌数据通知 消息Id: 23121
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CARD_DATA_NOTICE] = function()
    Net.SCCardDataNotice = Net.SCCardDataNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCardDataNotice:__init()
        self:AutoConstruct({
            "flag__C",      --操作标志(0=减少,1=添加,更新)
            "uid__I",       --卡牌唯一ID
            "info_id__I",       --卡牌信息ID
            "level__H",     --卡牌等级
            "exp__I",       --卡牌经验
            "break_level__H",   --卡牌等级
            "hero_id__I",  --装备此卡牌的英雄信息ID
        })
    end
    return Net.SCCardDataNotice
end

-- 消息说明: 玩家占卜数据请求 消息Id: 23101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLAYER_ZHANBU_DATA_REQ] = function()
    Net.CSPlayerZhanbuDataReq = Net.CSPlayerZhanbuDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlayerZhanbuDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSPlayerZhanbuDataReq
end

-- 消息说明: 玩家占卜数据响应 消息Id: 23102
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLAYER_ZHANBU_DATA_RESP] = function()
    Net.SCPlayerZhanbuDataResp = Net.SCPlayerZhanbuDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlayerZhanbuDataResp:__init()
        self:AutoConstruct({
            "history_collection_value__I",  --历史收藏值
            "low_card_group_map_idx__H",    --低级卡组关卡索引
            "cur_low_card_group_id__H", --当前低级卡组ID
            "low_card_group_last_recover_time__I",  --低级卡组最近恢复时间
            "cur_low_card_group_refresh_times__H",  --当前低级卡组拥有的刷新次数
            "today_low_card_group_open_card_times__H",  --今日低级卡组翻卡次数
            "high_card_group_last_refresh_time__I", --高级卡组最近刷新时间
            "achievement_level__H", --成就等级
            "today_low_card_group_cost_refresh_times__H",   --今日低级卡组已经花费刷新的次数(需要消耗的刷新次数)
            "today_high_card_group_cost_refresh_times__H",  --今日高级卡组已经花费刷新的次数(需要消耗的刷新次数)
            "low_card_group_list__T__card_id@I##is_already_open@C", -- 低级卡组列表
            "high_card_group_list__T__card_id@I##is_already_open@C", -- 高级卡组列表
        })
    end
    return Net.SCPlayerZhanbuDataResp
end

-- 消息说明: 卡牌刷新请求 消息Id: 23111
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CARD_REFRESH_REQ] = function()
    Net.CSCardRefreshReq = Net.CSCardRefreshReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCardRefreshReq:__init()
        self:AutoConstruct({
            "refresh_type__C",      --刷新类型(1=低级卡组, 2=高级卡组)
        })
    end
    return Net.CSCardRefreshReq
end

-- 消息说明: 卡牌刷新响应 消息Id: 23112
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CARD_REFRESH_RESP] = function()
    Net.SCCardRefreshResp = Net.SCCardRefreshResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCardRefreshResp:__init()
        self:AutoConstruct({
            "refresh_type__C",      --刷新类型(1=低级卡组, 2=高级卡组)
        })
    end
    return Net.SCCardRefreshResp
end


-- 消息说明: 翻卡请求 消息Id: 23113
Net.ProtocolRegistry[Net.ProtocolNo.C2S_OPEN_CARD_REQ] = function()
    Net.CSOpenCardReq = Net.CSOpenCardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSOpenCardReq:__init()
        self:AutoConstruct({
            "type__C",      --翻卡类型(1=低级卡组, 2=高级卡组)
            "card_idx_list__T__card_idx@H", -- 卡牌索引列表(请求翻开的卡牌索引)
        })
    end
    return Net.CSOpenCardReq
end

-- 消息说明: 翻卡响应 消息Id: 23114
Net.ProtocolRegistry[Net.ProtocolNo.S2C_OPEN_CARD_RESP] = function()
    Net.SCOpenCardResp = Net.SCOpenCardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCOpenCardResp:__init()
        self:AutoConstruct({
            "type__C",      --翻卡类型(1=低级卡组, 2=高级卡组)
            "card_idx_list__T__card_idx@H##card_id@I##destiny_coin@I", -- 卡牌索引列表(请求翻开的卡牌索引)
        })
    end
    return Net.SCOpenCardResp
end

-- 消息说明: 低级卡组刷新次数更新请求 消息Id: 23115
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ] = function()
    Net.CSLowCardGroupRefreshTimesUpdateReq = Net.CSLowCardGroupRefreshTimesUpdateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLowCardGroupRefreshTimesUpdateReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSLowCardGroupRefreshTimesUpdateReq
end

-- 消息说明: 低级卡组刷新次数更新响应 消息Id: 23116
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP] = function()
    Net.SCLowCardGroupRefreshTimesUpdateResp = Net.SCLowCardGroupRefreshTimesUpdateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLowCardGroupRefreshTimesUpdateResp:__init()
        self:AutoConstruct({
            "cur_low_card_group_refresh_times__H",  --当前低级卡组拥有的刷新次数
            "low_card_group_last_recover_time__I",  --低级卡组最近恢复时间
        })
    end
    return Net.SCLowCardGroupRefreshTimesUpdateResp
end

-- 消息说明: 收藏组激活请求 消息Id: 23117
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CELLECTION_GROUP_ACTIVATE_REQ] = function()
    Net.CSCellectionGroupActivateReq = Net.CSCellectionGroupActivateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCellectionGroupActivateReq:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
        })
    end
    return Net.CSCellectionGroupActivateReq
end

-- 消息说明: 收藏组激活响应 消息Id: 23118
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CELLECTION_GROUP_ACTIVATE_RESP] = function()
    Net.SCCellectionGroupActivateResp = Net.SCCellectionGroupActivateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCellectionGroupActivateResp:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
            "history_collection_calue__I",      --历史收藏值
        })
    end
    return Net.SCCellectionGroupActivateResp
end


-- 消息说明: 收藏组升星请求 消息Id: 23119
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CELLECTION_GROUP_STAR_UP_REQ] = function()
    Net.CSCellectionGroupStarUpReq = Net.CSCellectionGroupStarUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCellectionGroupStarUpReq:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
        })
    end
    return Net.CSCellectionGroupStarUpReq
end

-- 消息说明: 收藏组升星响应 消息Id: 23120
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CELLECTION_GROUP_STAR_UP_RESP] = function()
    Net.SCCellectionGroupStarUpResp = Net.SCCellectionGroupStarUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCellectionGroupStarUpResp:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
            "history_collection_calue__I",      --历史收藏值
        })
    end
    return Net.SCCellectionGroupStarUpResp
end

-- 消息说明: 收藏组重置(取卡)请求 消息Id: 23122
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CELLECTION_GROUP_RESET_REQ] = function()
    Net.CSCellectionGroupRestReq = Net.CSCellectionGroupRestReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCellectionGroupRestReq:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
        })
    end
    return Net.CSCellectionGroupRestReq
end

-- 消息说明: 收藏组重置(取卡)响应 消息Id: 23123
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CELLECTION_GROUP_RESET_RESP] = function()
    Net.SCCellectionGroupRestResp = Net.SCCellectionGroupRestResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCellectionGroupRestResp:__init()
        self:AutoConstruct({
            "book_id__H",       --收藏书ID
            "group_id__H",      --收藏组ID
            "star__H",      --星数
        })
    end
    return Net.SCCellectionGroupRestResp
end

-- 消息说明: 收藏书列表请求 消息Id: 23124
Net.ProtocolRegistry[Net.ProtocolNo.C2S_COLLECTION_BOOK_LIST_REQ] = function()
    Net.CSCollectionBookListReq = Net.CSCollectionBookListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCollectionBookListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSCollectionBookListReq
end

-- 消息说明: 收藏书列表响应 消息Id: 23125
-- GroupList说明:
-- "group_id__H",      --收藏组ID
-- "star__C",      --星数
Net.ProtocolRegistry[Net.ProtocolNo.S2C_COLLECTION_BOOK_LIST_RESP] = function()
    Net.SCCollectionBookListResp = Net.SCCollectionBookListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCollectionBookListResp:__init()
        self:AutoConstruct({
            "book_list__T__book_id@H##group_list@D|GroupList|", -- 书本列表
        })
    end
    return Net.SCCollectionBookListResp
end

-- 消息说明: 装备卡牌请求 消息Id: 23126
Net.ProtocolRegistry[Net.ProtocolNo.C2S_COLLECTION_EQUIP_CARD_REQ] = function()
    Net.CSCollectionEquipCardReq = Net.CSCollectionEquipCardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCollectionEquipCardReq:__init()
        self:AutoConstruct({
            "formation_pos__H", --阵位(从1开始)
            "card_slot__H",     --卡槽(从1开始)
            "card_uid__I",      --卡牌UID
        })
    end
    return Net.CSCollectionEquipCardReq
end

-- 消息说明: 装备卡牌响应 消息Id: 23127
Net.ProtocolRegistry[Net.ProtocolNo.S2C_COLLECTION_EQUIP_CARD_RESP] = function()
    Net.SCCollectionEquipCardResp = Net.SCCollectionEquipCardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCollectionEquipCardResp:__init()
        self:AutoConstruct({
            "formation_pos__H", --阵位(从1开始)
            "card_slot__H",     --卡槽(从1开始)
            "card_uid__I",      --卡牌UID
        })
    end
    return Net.SCCollectionEquipCardResp
end

-- 消息说明: 卸下卡牌请求 消息Id: 23128
Net.ProtocolRegistry[Net.ProtocolNo.C2S_COLLECTION_UNLOAD_CARD_REQ] = function()
    Net.CSCollectionUnloadCardReq = Net.CSCollectionUnloadCardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCollectionUnloadCardReq:__init()
        self:AutoConstruct({
            "formation_pos__H", --阵位(从1开始)
            "card_slot__H",     --卡槽(从1开始)
        })
    end
    return Net.CSCollectionUnloadCardReq
end

-- 消息说明: 卸下卡牌响应 消息Id: 23129
Net.ProtocolRegistry[Net.ProtocolNo.S2C_COLLECTION_UNLOAD_CARD_RESP] = function()
    Net.SCCollectionUnloadCardResp = Net.SCCollectionUnloadCardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCollectionUnloadCardResp:__init()
        self:AutoConstruct({
            "formation_pos__H", --阵位(从1开始)
            "card_slot__H",     --卡槽(从1开始)
        })
    end
    return Net.SCCollectionUnloadCardResp
end

-- 消息说明: 占卜成就激活请求 消息Id: 23130
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ] = function()
    Net.CSZhanBuAchievementAcivateReq = Net.CSZhanBuAchievementAcivateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSZhanBuAchievementAcivateReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSZhanBuAchievementAcivateReq
end

-- 消息说明: 占卜成就激活响应 消息Id: 23131
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP] = function()
    Net.SCZhanBuAchievementAcivateResp = Net.SCZhanBuAchievementAcivateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCZhanBuAchievementAcivateResp:__init()
        self:AutoConstruct({
            "cur_achievement_level__H", --当前成就等级
        })
    end
    return Net.SCZhanBuAchievementAcivateResp
end

-- 消息说明: 占卜成就激活请求 消息Id: 23130
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ] = function()
    Net.CSZhanBuAchievementAcivateReq = Net.CSZhanBuAchievementAcivateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSZhanBuAchievementAcivateReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSZhanBuAchievementAcivateReq
end
