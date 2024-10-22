
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--请求英雄基本数据列表: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_BASIC_LIST_REQ] = function()
    Net.CSHeroBasicListReq = Net.CSHeroBasicListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroBasicListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSHeroBasicListReq
end

--请求英雄基本数据列表响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_BASIC_LIST_RESP] = function()
    Net.SCHeroBasicListResp = Net.SCHeroBasicListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroBasicListResp:__init()
        self:AutoConstruct({
            "hero_list__T__uid@I##info_id@H##level@C##quality_level@C##quality_stage@C##potential_level@C##awaken_star@C##awaken_level@C##break_level@C##is_to_battle@C##exp@I##train_pro_list@D|HeroTrainProList|##awaken_item_list@D|AwakenItemList|", -- 英雄基本数据列表     
        })
    end
    return Net.SCHeroBasicListResp
end

--英雄基本信息通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_BASIC_INFO_NOTIFY] = function()
    Net.SCHeroBasicInfoResp = Net.SCHeroBasicInfoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroBasicInfoResp:__init()
        self:AutoConstruct({
            "flag__C",      --标志(1=添加或更新,0=删除)
            "hero_base_data_list__T__uid@I##info_id@H##level@C##quality_level@C##quality_stage@C##potential_level@C##awaken_star@C##awaken_level@C##break_level@C##is_to_battle@C##exp@I##train_pro_list@D|HeroTrainProList|##awaken_item_list@D|AwakenItemList|", -- 英雄基本数据列表		
        })
    end
    return Net.SCHeroBasicInfoResp
end

--请求英雄详细信息: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_DETAIL_INFO_REQ] = function()
    Net.CSHeroDetailInfoReq = Net.CSHeroDetailInfoReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroDetailInfoReq:__init()
        self:AutoConstruct({
            "uid__I",  --英雄uid
        })
    end
    return Net.CSHeroDetailInfoReq
end

--请求英雄详细信息响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_DETAIL_INFO_RESP] = function()
    Net.SCHeroDetailInfoResp = Net.SCHeroDetailInfoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroDetailInfoResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "hero_info_id__H",  --英雄配置id
            "hero_level__C",    --英雄等级
            "quality_level__C", --品质等级
			"quality_stage__C", --品阶
            "break_level__C",   --突破等级
            "potential_level__C", --天命等级
            "potential_exp__I",   --天命经验
            "awaken_star__C",   --觉醒星级
            "awaken_level__C",  --觉醒等级
            "awaken_item_list__T__item_id@H", -- 装备的觉醒道具列表

            -- pro_type@C : 属性类型
            -- pro_level@C : 属性当前等级
            -- pro_value@I : 属性值
            -- tmp_pro_value@I : 临时属性值(上次培养的结果)
            -- pro_reward_flag@C : 属性奖励标志(1=表示可以领取 0=已领取或者冒得领取)
            "train_pro_list__T__pro_type@C##pro_level@C##pro_value@I##tmp_pro_value@i##pro_reward_flag@C", -- 培养属性列表   
        })
    end
    return Net.SCHeroDetailInfoResp
end

--英雄阵位列表通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_LINEUP_LIST_RESP] = function()
    Net.SCHeroHeroLineupListResp = Net.SCHeroHeroLineupListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroHeroLineupListResp:__init()
        self:AutoConstruct({
            "formation_list__T__uid@I##bless_sprite_id@I##attack_treasure_uid@I##defence_treasure_uid@I##weapon_equipment_uid@I##hat_equipment_uid@I##belt_equipment_uid@I##clothes_equipment_uid@I##gem_list@D|GemList|##card_list@D|CardList|", -- 阵型数据列表
            "reinforcement_list__T__uid@I##", -- 援军列表
        })
    end
    return Net.SCHeroHeroLineupListResp
end

--请求英雄上阵: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_GOTO_BATTLE_REQ] = function()
    Net.CSHeroGotoBattleReq = Net.CSHeroGotoBattleReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroGotoBattleReq:__init()
        self:AutoConstruct({
            "uid__I",      --英雄uid
            "pos__C",           --上阵位置
        })
    end
    return Net.CSHeroGotoBattleReq
end

--请求英雄上阵响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_GOTO_BATTLE_RESP] = function()
    Net.SCHeroGotoBattleResp = Net.SCHeroGotoBattleResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroGotoBattleResp:__init()
        self:AutoConstruct({
            "uid__I",      --英雄uid
            "pos__C",           --上阵位置
        })
    end
    return Net.SCHeroGotoBattleResp
end

--升级请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_UPGRADE_REQ] = function()
    Net.CSHeroUpgradeReq = Net.CSHeroUpgradeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroUpgradeReq:__init()
        self:AutoConstruct({
            "uid__I",                  --英雄uid
            "cost_hero_list__T__uid@I",     -- 消耗英雄列表
        })
    end
    return Net.CSHeroUpgradeReq
end

--升级响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_UPGRADE_RESP] = function()
    Net.SCHeroUpgradeResp = Net.SCHeroUpgradeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroUpgradeResp:__init()
        self:AutoConstruct({
            "uid__I",  --英雄uid
        })
    end
    return Net.SCHeroUpgradeResp
end

--进阶请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_BREACH_REQ] = function()
    Net.CSHeroBreakReq = Net.CSHeroBreakReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroBreakReq:__init()
        self:AutoConstruct({
            "uid__I",                                   --英雄uid
            "same_name_hero_list__T__uid@i",          --同名英雄列表
            "same_quality_hero_list__T__uid@i",       --同品质英雄列表
            "same_profession_hero_list__T__uid@i",    --同品质且职业英雄列表
        })
    end
    return Net.CSHeroBreakReq
end

--进阶响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_BREACH_RESP] = function()
    Net.SCHeroBreakResp = Net.SCHeroBreakResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroBreakResp:__init()
        self:AutoConstruct({
            "uid__I",  --英雄uid
        })
    end
    return Net.SCHeroBreakResp
end

--培养请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_TRAIN_REQ] = function()
    Net.CSHeroTrainReq = Net.CSHeroTrainReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroTrainReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "cost_type__C",     --消耗类型 (读配置表)
            "train_times__C",   --培养次数
        })
    end
    return Net.CSHeroTrainReq
end

--培养响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_TRAIN_RESP] = function()
    Net.SCHeroTrainResp = Net.SCHeroTrainResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroTrainResp:__init()
        self:AutoConstruct({
            "hero_uid__I",          --英雄uid
            "pro_list__T__type@H##value@i", --培养属性列表          
        })
    end
    return Net.SCHeroTrainResp
end

--潜能请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_DESTINY_REQ] = function()
    Net.CSHeroDestinyReq = Net.CSHeroDestinyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroDestinyReq:__init()
        self:AutoConstruct({
            "uid__I",      --英雄uid
        })
    end
    return Net.CSHeroDestinyReq
end

--潜能响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_DESTINY_RESP] = function()
    Net.SCHeroDestinyResp = Net.SCHeroDestinyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroDestinyResp:__init()
        self:AutoConstruct({
            "uid__I",      --英雄uid
            "potential_level__C", --潜能等级
            "potential_exp__I",   --潜能经验            
        })
    end
    return Net.SCHeroDestinyResp
end

--合成请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_COMPOSE_REQ] = function()
    Net.CSHeroComposeReq = Net.CSHeroComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroComposeReq:__init()
        self:AutoConstruct({
            "info_id__H",      --英雄信息id
        })
    end
    return Net.CSHeroComposeReq
end

--合成响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_COMPOSE_RESP] = function()
    Net.SCHeroComposeResp = Net.SCHeroComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroComposeResp:__init()
        self:AutoConstruct({
            "info_id__H",      --英雄信息id
        })
    end
    return Net.SCHeroComposeResp
end

--召唤请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_RECRUIT_REQ] = function()
    Net.CSHeroRecruitReq = Net.CSHeroRecruitReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroRecruitReq:__init()
        self:AutoConstruct({
            "type__C",      --召唤类型
        })
    end
    return Net.CSHeroRecruitReq
end

--召唤响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_RECRUIT_RESP] = function()
    Net.SCHeroRecruitResp = Net.SCHeroRecruitResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroRecruitResp:__init()
        self:AutoConstruct({
            "type__C",
            "hero_list__T__info_id@H",  -- 召唤获得的英雄列表
        })
    end
    return Net.SCHeroRecruitResp
end

--召唤数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_RECRUITDATA_REQ] = function()
    Net.CSHeroRecruitDataReq = Net.CSHeroRecruitDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroRecruitDataReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSHeroRecruitDataReq
end

--召唤数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_RECRUITDATA_RESP] = function()
    Net.SCHeroRecruitDataResp = Net.SCHeroRecruitDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroRecruitDataResp:__init()
        self:AutoConstruct({
            "normal_summon_times__C",   --普通召唤次数
            "normal_summon_time__I",    --普通召唤时间
            "advanced_summon_times__C", --高级召唤次数
            "advanced_summon_time__I",  --高级召唤时间
        })
    end
    return Net.SCHeroRecruitDataResp
end

--收集数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_COLLECTION_LIST_REQ] = function()
    Net.CSHeroCollectionListReq = Net.CSHeroCollectionListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroCollectionListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSHeroCollectionListReq
end

--收集数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_COLLECTION_LIST_RESP] = function()
    Net.SCHeroCollectionListResp = Net.SCHeroCollectionListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroCollectionListResp:__init()
        self:AutoConstruct({
            "hero_collection_list__T__hero_info_id@H" -- 英雄列表
        })
    end
    return Net.SCHeroCollectionListResp
end

--援军上阵请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REINFORCEMENT_TO_BATTLE_REQ] = function()
    Net.CSHeroReinforcementToBattleReq = Net.CSHeroReinforcementToBattleReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroReinforcementToBattleReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "pos__C",       --上阵位置
        })
    end
    return Net.CSHeroReinforcementToBattleReq
end

--援军上阵响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REINFORCEMENT_TO_BATTLE_RESP] = function()
    Net.SCHeroReinforcementToBattleResp = Net.SCHeroReinforcementToBattleResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroReinforcementToBattleResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "pos__C",       --上阵位置
        })
    end
    return Net.SCHeroReinforcementToBattleResp
end

--觉醒请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_AWAKEN_REQ] = function()
    Net.CSHeroAwakenReq = Net.CSHeroAwakenReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroAwakenReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --请求觉醒的英雄uid
            "cost_hero_list__T__cost_hero_uid@I", --作为消耗的英雄uid（0=表示不需要)
        })
    end
    return Net.CSHeroAwakenReq
end

--觉醒响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_AWAKEN_RESP] = function()
    Net.SCHeroAwakenResp = Net.SCHeroAwakenResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroAwakenResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --请求觉醒的英雄uid
            "awaken_star__C",   --当前觉醒星级
            "awaken_level__C",  --当前觉醒等级
        })
    end
    return Net.SCHeroAwakenResp
end

--装备觉醒道具请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_EQUIP_AWAKEN_ITEM_REQ] = function()
    Net.CSHeroEquipAwakenItemReq = Net.CSHeroEquipAwakenItemReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroEquipAwakenItemReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --觉醒的英雄uid
            "awaken_item_id__H",    --装备的觉醒道具
        })
    end
    return Net.CSHeroEquipAwakenItemReq
end

--装备觉醒道具响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_EQUIP_AWAKEN_ITEM_RESP] = function()
    Net.SCHeroEquipAwakenItemReq = Net.SCHeroEquipAwakenItemReq or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroEquipAwakenItemReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --觉醒的英雄uid
            "awaken_item_id__H",    --装备的觉醒道具
        })
    end
    return Net.SCHeroEquipAwakenItemReq
end

--英雄培养替换请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_TRAIN_REPLACE_REQ] = function()
    Net.CSHeroTrainReplaceReq = Net.CSHeroTrainReplaceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroTrainReplaceReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --操作替换的英雄uid
        })
    end
    return Net.CSHeroTrainReplaceReq
end

--英雄培养替换响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_TRAIN_REPLACE_RESP] = function()
    Net.SCHeroTrainReplaceResp = Net.SCHeroTrainReplaceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroTrainReplaceResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --操作替换的英雄uid
        })
    end
    return Net.SCHeroTrainReplaceResp
end

--培养奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_TRAIN_REWARD_GET_REQ] = function()
    Net.CSHeroTrainRewardGetReq = Net.CSHeroTrainRewardGetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroTrainRewardGetReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "pro_type__C",      --属性类型
        })
    end
    return Net.CSHeroTrainRewardGetReq
end

--培养奖励领取响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_TRAIN_REWARD_GET_RESP] = function()
    Net.SCHeroTrainRewardGetResp = Net.SCHeroTrainRewardGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroTrainRewardGetResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "pro_type__C",      --属性类型
        })
    end
    return Net.SCHeroTrainRewardGetResp
end

--英雄进化请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_EVOLUTION_REQ] = function()
    Net.CSHeroEvolutionReq = Net.CSHeroEvolutionReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroEvolutionReq:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
        })
    end
    return Net.CSHeroEvolutionReq
end

--英雄进化响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_EVOLUTION_RESP] = function()
    Net.SCHeroEvolutionResp = Net.SCHeroEvolutionResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroEvolutionResp:__init()
        self:AutoConstruct({
            "hero_uid__I",      --英雄uid
            "quality__C",   --当前品质
            "stage__C", --当前品阶
        })
    end
    return Net.SCHeroEvolutionResp
end

--VIP召唤请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_VIP_SUMMON_REQ] = function()
    Net.CSHeroVipSummonReq = Net.CSHeroVipSummonReq or BaseClass(Net.BaseProtocalStruct)
    function Net.CSHeroVipSummonReq:__init()
        self:AutoConstruct({
            "camp__I",      --阵营
        })
    end
    return Net.CSHeroVipSummonReq
end

--VIP召唤响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_VIP_SUMMON_RESP] = function()
    Net.SCHeroVipSummonResp = Net.SCHeroVipSummonResp or BaseClass(Net.BaseProtocalStruct)
    function Net.SCHeroVipSummonResp:__init()
        self:AutoConstruct({
            "camp__I",      --阵营
            "hero_list__T__info_id@I##num@H",  -- 召唤获得的英雄列表
        })
    end
    return Net.SCHeroVipSummonResp
end
