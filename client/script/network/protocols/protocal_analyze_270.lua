Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:请求玩家资源战数据 消息Id: 27001
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLAYER_RESOURCE_WAR_DATA_REQ] = function()
    Net.CSPlayerResourceWarDataReq = Net.CSPlayerResourceWarDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlayerResourceWarDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSPlayerResourceWarDataReq
end

-- 消息说明:玩家资源战数据响应 消息Id: 27002
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLAYER_RESOURCE_WAR_DATA_RESP] = function()
    Net.SCPlayerResourceWarDataResp = Net.SCPlayerResourceWarDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlayerResourceWarDataResp:__init()
        self:AutoConstruct({
            "chapter__H",           --章节
            "resource_id__H",       --资源id
            "occupy_start_time__I",     --占领开始时间
            "occupy_over_time__I",      --占领结束时间
            "keep_occupy_times__H",     --延长的占领次数
            -- "last_base_reward_time__I", --最近的基础奖励时间
            -- "last_extra_reward_time__I",    --最近的额外奖励时间
            -- "cur_extra_reward_need_time__H",--当前额外奖励需要的时间(分钟)
            "total_accupy_time__I",     --总的占领时间
            "technology_level__H",      --科技等级
            "guild_accupy_num__H",      --工会占领人数(玩家自己占领的资源所在的章节)
            "guild_name__s",      --工会名称
        })
    end
    return Net.SCPlayerResourceWarDataResp
end

-- 消息说明:请求资源战敌人列表 消息Id: 27003
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_ENEMY_LIST_REQ] = function()
    Net.CSResourceWarEnemyListReq = Net.CSResourceWarEnemyListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarEnemyListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarEnemyListReq
end

-- 消息说明:请求敌人列表响应 消息Id: 27004
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_ENEMY_LIST_RESP] = function()
    Net.SCResourceWarEnemyListResp = Net.SCResourceWarEnemyListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarEnemyListResp:__init()
        self:AutoConstruct({
            "enemy_list__T__role_id@I##role_name@s##level@H##power@I##head_img@H##guild_name@s##attack_times@H##chapter@H##resource_id@H", -- 敌人列表
        })
    end
    return Net.SCResourceWarEnemyListResp
end

-- 消息说明:请求战报 消息Id: 27005
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_BATTLE_RECORD_REQ] = function()
    Net.CSResourceWarBattleRecordReq = Net.CSResourceWarBattleRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarBattleRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarBattleRecordReq
end

-- 消息说明:请求战报响应 消息Id: 27006
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_BATTLE_RECORD_RESP] = function()
    Net.SCResourceWarBattleRecordResp = Net.SCResourceWarBattleRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarBattleRecordResp:__init()
        self:AutoConstruct({
            "battle_record_list__T__attacker_name@s##defender_name@s##battle_time@I##chapter@H##resource_id@H", -- 敌人列表
        })
    end
    return Net.SCResourceWarBattleRecordResp
end

-- 消息说明:放弃占领的资源 消息Id: 27007
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ] = function()
    Net.CSResourceWarGiveUpOccupyReq = Net.CSResourceWarGiveUpOccupyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarGiveUpOccupyReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarGiveUpOccupyReq
end

-- 消息说明:放弃占领的资源响应 消息Id: 27008
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP] = function()
    Net.SCResourceWarGiveUpOccupyResp = Net.SCResourceWarGiveUpOccupyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarGiveUpOccupyResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCResourceWarGiveUpOccupyResp
end

-- 消息说明:延长占领的资源 消息Id: 27009
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ] = function()
    Net.CSResourceWarKeepOccupyReq = Net.CSResourceWarKeepOccupyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarKeepOccupyReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarKeepOccupyReq
end

-- 消息说明:延长占领的资源响应 消息Id: 27010
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP] = function()
    Net.SCResourceWarKeepOccupyResp = Net.SCResourceWarKeepOccupyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarKeepOccupyResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCResourceWarKeepOccupyResp
end

-- 消息说明:请求资源战章节数据 消息Id: 27011
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_CHAPTER_DATA_REQ] = function()
    Net.CSResourceWarChapterDataReq = Net.CSResourceWarChapterDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarChapterDataReq:__init()
        self:AutoConstruct({
            "chapter__H",--章节
        })
    end
    return Net.CSResourceWarChapterDataReq
end

-- 消息说明:请求资源战章节数据响应 消息Id: 27012
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_CHAPTER_DATA_RESP] = function()
    Net.SCResourceWarChapterDataResp = Net.SCResourceWarChapterDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarChapterDataResp:__init()
        self:AutoConstruct({
            "chapter__H",--章节
            "chapter_resource_list__T__role_id@I##role_name@s##level@H##power@I##head_img@H##guild_name@s##server_id@H##resource_id@H", -- 章节资源列表
        })
    end
    return Net.SCResourceWarChapterDataResp
end

-- 消息说明:请求资源战同类资源 消息Id: 27013
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_SAME_RESOURCE_REQ] = function()
    Net.CSResourceWarSameResourceReq = Net.CSResourceWarSameResourceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarSameResourceReq:__init()
        self:AutoConstruct({
            "resource_type__H",--资源类型
        })
    end
    return Net.CSResourceWarSameResourceReq
end

-- 消息说明:请求资源战同类资源响应 消息Id: 27014
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_SAME_RESOURCE_RESP] = function()
    Net.SCResourceWarSameResourceResp = Net.SCResourceWarSameResourceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarSameResourceResp:__init()
        self:AutoConstruct({
            "resource_type__H",--资源类型
            "resource_data_list__T__chapter@H##resource_id@H", -- 资源数据列表
        })
    end
    return Net.SCResourceWarSameResourceResp
end

-- 消息说明:请求资源战工会资源 消息Id: 27015
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ] = function()
    Net.CSResourceWarGuildResourceReq = Net.CSResourceWarGuildResourceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarGuildResourceReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarGuildResourceReq
end

-- 消息说明:请求资源战工会资源响应 消息Id: 27016
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP] = function()
    Net.SCResourceWarGuildResourceResp = Net.SCResourceWarGuildResourceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarGuildResourceResp:__init()
        self:AutoConstruct({
            "resource_data_list__T__chapter@H##resource_id@H", -- 资源数据列表
        })
    end
    return Net.SCResourceWarGuildResourceResp
end

-- 消息说明:资源战科技升级请求 消息Id: 27017
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ] = function()
    Net.CSResourceWarTechnologyUpgradeReq = Net.CSResourceWarTechnologyUpgradeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSResourceWarTechnologyUpgradeReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSResourceWarTechnologyUpgradeReq
end

-- 消息说明:资源战科技升级响应 消息Id: 27018
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP] = function()
    Net.SCResourceWarTechnologyUpgradeResp = Net.SCResourceWarTechnologyUpgradeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCResourceWarTechnologyUpgradeResp:__init()
        self:AutoConstruct({
            "technology_level__H",--科技等级
        })
    end
    return Net.SCResourceWarTechnologyUpgradeResp
end