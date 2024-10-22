
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 巡逻事件列表参数说明:
-- event_id: 事件ID
-- happen_time: 事件发生时间
-- is_double_award: 是否双倍奖励


-- 暴动事件列表参数说明:
-- happen_time: 暴动发生事件
-- suppress_time: 暴动镇压时间(0=表示木有人帮镇压)
-- friend_name: 帮助镇压的好友名称

--玩家领地数据请求: 消息Id: 21901
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLAYER_LAND_DATA_REQ] = function()
    Net.CSPlayerLandDataReq = Net.CSPlayerLandDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlayerLandDataReq:__init()
        self:AutoConstruct({
            "target_id__I",     --请求的目标玩家ID
        })
    end
    return Net.CSPlayerLandDataReq
end

--玩家领地数据请求响应: 消息Id: 21902
-- city_id:        // 城池Id
-- start_patrol_time:  // 开始巡逻时间(0=表示木有在巡逻)
-- time_type:      // 时间类型
-- is_riot:        // 是否暴动
-- skill_level:        // 技能等级is_can_get_reward
-- is_can_get_reward:       // 是否已经领取奖励(1=已经领取,0=还木有领取)
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLAYER_LAND_DATA_RESP] = function()
    Net.SCPlayerLandDataResp = Net.SCPlayerLandDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlayerLandDataResp:__init()
        self:AutoConstruct({
            "target_id__I",     --请求的目标玩家ID
            "riot_suppress_times__C",   --已经镇压的暴动次数
            "patrol_total_time__I", --巡逻总时间
            "city_list__T__city_id@H##start_patrol_time@I##patrol_type@C##time_type@C##is_riot@C##skill_level@C##is_can_get_reward@C##hero_info_id@H",    -- 城池列表
        })
    end
    return Net.SCPlayerLandDataResp
end

--巡逻请求: 消息Id: 21903
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LAND_PATROL_REQ] = function()
    Net.CSLandPatrolReq = Net.CSLandPatrolReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLandPatrolReq:__init()
        self:AutoConstruct({
            "patrol_hero_uid__I",   --巡逻英雄UID
            "city_id__H",       --城池ID
            "patrol_type__C",   --巡逻类型
            "time_type__C",     --时间类型(1=4小时,2=8小时,3=12小时)
        })
    end
    return Net.CSLandPatrolReq
end

--巡逻响应: 消息Id: 21904
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LAND_PATROL_RESP] = function()
    Net.SCLandPatrolResp = Net.SCLandPatrolResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLandPatrolResp:__init()
        self:AutoConstruct({
            "patrol_hero_uid__I",   --巡逻英雄UID
            "city_id__H",       --城池ID
            "patrol_type__C",   --巡逻类型
            "time_type__C",     --时间类型(1=4小时,2=8小时,3=12小时)
        })
    end
    return Net.SCLandPatrolResp
end

--领取巡逻奖励: 消息Id: 21907
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LAND_GET_PATROL_AWARD_REQ] = function()
    Net.CSLandGetPatrolAwardReq = Net.CSLandGetPatrolAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLandGetPatrolAwardReq:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
        })
    end
    return Net.CSLandGetPatrolAwardReq
end

--领取巡逻奖励响应: 消息Id: 21908
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LAND_GET_PATROL_AWARD_RESP] = function()
    Net.SCLandGetPatrolAwardResp = Net.SCLandGetPatrolAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLandGetPatrolAwardResp:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
        })
    end
    return Net.SCLandGetPatrolAwardResp
end

--领地技能升级(开启,提升): 消息Id: 21909
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LAND_SKILL_LEVELUP_REQ] = function()
    Net.CSLandSkillLevelupReq = Net.CSLandSkillLevelupReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLandSkillLevelupReq:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
        })
    end
    return Net.CSLandSkillLevelupReq
end

--领地技能升级响应(开启,提升): 消息Id: 21910
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LAND_SKILL_LEVELUP_RESP] = function()
    Net.SCLandSkillLevelupResp = Net.SCLandSkillLevelupResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLandSkillLevelupResp:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
            "skill_level__C",   --当前技能等级
        })
    end
    return Net.SCLandSkillLevelupResp
end

--领地镇压暴动: 消息Id: 21911
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LAND_SUPPRESS_RIOT_REQ] = function()
    Net.CSLandSuppressRiotReq = Net.CSLandSuppressRiotReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLandSuppressRiotReq:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
            "friend_id__I", --好友ID
        })
    end
    return Net.CSLandSuppressRiotReq
end

--领地镇压暴动响应: 消息Id: 21912
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LAND_SUPPRESS_RIOT_RESP] = function()
    Net.SCLandSuppressRiotResp = Net.SCLandSuppressRiotResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLandSuppressRiotResp:__init()
        self:AutoConstruct({
            "city_id__H",       --城池ID
            "friend_id__C", --好友ID
        })
    end
    return Net.SCLandSuppressRiotResp
end

--请求领地好友列表: 消息Id: 21913
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_LAND_LIST_REQ] = function()
    Net.CSFriendLandListReq = Net.CSFriendLandListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendLandListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSFriendLandListReq
end

--请求领地好友列表响应: 消息Id: 21914
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_LAND_LIST_RESP] = function()
    Net.SCFriendLandListResp = Net.SCFriendLandListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFriendLandListResp:__init()
        self:AutoConstruct({
            "friend_land_list__T__role_id@I##name@s##level@H##head_icon@H##last_logout_time@I##city_num@C##patrol_city_num@C##riot_city_num@C", -- 好友领地列表
        })
    end
    return Net.SCFriendLandListResp
end

--领地城池详情请求: 消息Id: 21915
Net.ProtocolRegistry[Net.ProtocolNo.C2S_LAND_CITY_DETAILS_REQ] = function()
    Net.CSLandCityDetailsReq = Net.CSLandCityDetailsReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSLandCityDetailsReq:__init()
        self:AutoConstruct({
            "role_id__I",    --玩家角色ID
            "city_id__H",   --城池ID
        })
    end
    return Net.CSLandCityDetailsReq
end

--领地城池详情请求响应: 消息Id: 21916
Net.ProtocolRegistry[Net.ProtocolNo.S2C_LAND_CITY_DETAILS_RESP] = function()
    Net.SCLandCityDetailsResp = Net.SCLandCityDetailsResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCLandCityDetailsResp:__init()
        self:AutoConstruct({
            "role_id__I",       --玩家角色ID
            "city_id__H",       --城池ID
            "skill_level__C",   --城池技能等级
            "patrol_hero_info_id__H",--巡逻的英雄信息ID
            "start_patrol_time__I", --开始巡逻时间
            "patrol_type__C",   --巡逻方式
            "time_type__C",     --时间类型
            "is_riot__C",       --是否暴动(1=暴动,0=不暴动)
            "reward_item_num__C",   --奖励的碎片数量
            "patrol_event_list__T__event_id@H##happen_time@I##is_double_award@C", -- 巡逻事件列表
            "riot_event_list__T__happen_time@I##suppress_time@I##friend_name@s", -- 暴动事件列表
        })
    end
    return Net.SCLandCityDetailsResp
end