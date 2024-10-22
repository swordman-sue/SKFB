Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--夺宝数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLUNDEREDTREASURES_DATA_REQ] = function()
    Net.CSPlunderedTreasuresDataReq = Net.CSPlunderedTreasuresDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlunderedTreasuresDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSPlunderedTreasuresDataReq
end

--夺宝数据请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLUNDEREDTREASURES_DATA_RESP] = function()
    Net.SCPlunderedTreasuresDataResp = Net.SCPlunderedTreasuresDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlunderedTreasuresDataResp:__init()
        self:AutoConstruct({
            "over_time__I", --免战结束时间
            "daily_doubao_times__H", --每日夺宝次数
            "pregross_reward_record__T__pregross_id@H", -- 进度奖励记录(进度ID)
        })
    end
    return Net.SCPlunderedTreasuresDataResp
end

--夺宝对手数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLUNDEREDTREASURES_TARGET_LIST_REQ] = function()
    Net.CSPlunderedTreasuresTargetListReq = Net.CSPlunderedTreasuresTargetListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlunderedTreasuresTargetListReq:__init()
        self:AutoConstruct({
            "treasure_info_id__H",  --圣器信息ID
            "treasure_item_id__H",  --圣器碎片ID
        })
    end
    return Net.CSPlunderedTreasuresTargetListReq
end

--夺宝对手数据请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLUNDEREDTREASURES_TARGET_LIST_RESP] = function()
    Net.SCPlunderedTreasuresTargetListResp = Net.SCPlunderedTreasuresTargetListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlunderedTreasuresTargetListResp:__init()
        self:AutoConstruct({
            "treasure_info_id__H",  --圣器信息ID
            "treasure_item_id__H",  --圣器碎片ID
            "player_list__T__role_id@I##level@H##head_image_id@H##fighting_power@I##role_name@s##hero_list@D|HeroList|", -- 挑战玩家目标列表
            "monster_list__T__map_id@H", -- 挑战怪物目标列表
        })
    end
    return Net.SCPlunderedTreasuresTargetListResp
end

--夺宝5次数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLUNDEREDTREASURES_FIVETIMES_REQ] = function()
    Net.CSPlunderedTreasuresFiveTimesReq = Net.CSPlunderedTreasuresFiveTimesReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlunderedTreasuresFiveTimesReq:__init()
        self:AutoConstruct({
            "treasure_info_id__H",  --圣器信息ID
            "treasure_item_id__H",  --圣器碎片ID
            "duobao_times__H",  --夺宝次数
        })
    end
    return Net.CSPlunderedTreasuresFiveTimesReq
end

--夺宝5次请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLUNDEREDTREASURES_FIVETIMES_RESP] = function()
    Net.CSPlunderedTreasuresFiveTimesResp = Net.CSPlunderedTreasuresFiveTimesResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlunderedTreasuresFiveTimesResp:__init()
        self:AutoConstruct({
            "item_list__T__item_id@I##item_num@I", -- 翻牌获得的物品列表
            "flag__C",  --标志(1=获得，0=没有获得)
        })
    end
    return Net.CSPlunderedTreasuresFiveTimesResp
end

--领取夺宝进度奖励请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_DUOBAO_PREGRESS_REWARD_REQ] = function()
    Net.CSTakeDuoBaoPregressRewardReq = Net.CSTakeDuoBaoPregressRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeDuoBaoPregressRewardReq:__init()
        self:AutoConstruct({
            "pregross_id_list__T__id@H", -- 要领取的进度ID列表
        })
    end
    return Net.CSTakeDuoBaoPregressRewardReq
end

--领取夺宝进度奖励响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_DUOBAO_PREGRESS_REWARD_RESP] = function()
    Net.SCTakeDuoBaoPregressRewardResp = Net.SCTakeDuoBaoPregressRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeDuoBaoPregressRewardResp:__init()
        self:AutoConstruct({
            "pregross_id_list__T__id@H", -- 要领取的进度ID列表
        })
    end
    return Net.SCTakeDuoBaoPregressRewardResp
end