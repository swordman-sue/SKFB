Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:请求工会战角色数据 消息Id: 27101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_ROLE_DATA_REQ] = function()
    Net.CSGuildWarRoleDataReq = Net.CSGuildWarRoleDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarRoleDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildWarRoleDataReq
end

-- 消息说明:请求工会战角色数据 消息Id: 27102
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_ROLE_DATA_RESP] = function()
    Net.SCGuildWarRoleDataResp = Net.SCGuildWarRoleDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarRoleDataResp:__init()
        self:AutoConstruct({
            "buy_high_star_battle_times__H",        --购买高级星球挑战次数
            "used_low_star_battle_times__H",        --低级星球已挑战次数
            "used_high_star_battle_times__H",       --高级星球已挑战次数
        })
    end
    return Net.SCGuildWarRoleDataResp
end

-- 消息说明:请求本军团已占领的星球 消息Id: 27103
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ] = function()
    Net.CSGuildWarOccupyStarListReq = Net.CSGuildWarOccupyStarListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarOccupyStarListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildWarOccupyStarListReq
end

-- 消息说明:请求本军团已占领的星球响应 消息Id: 27104
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP] = function()
    Net.SCGuildWarOccupyStarListResp = Net.SCGuildWarOccupyStarListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarOccupyStarListResp:__init()
        self:AutoConstruct({
            "occupy_star_list__T__star_id@H##is_have_reward@C", -- 占领的星球列表[星球ID,是否有奖励]
        })
    end
    return Net.SCGuildWarOccupyStarListResp
end

-- 消息说明:请求本军团已宣战的星球列表 消息Id: 27105
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ] = function()
    Net.CSGuildWarDeclareWarStarListReq = Net.CSGuildWarDeclareWarStarListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarDeclareWarStarListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildWarDeclareWarStarListReq
end

-- 消息说明:请求本军团已宣战的星球列表响应 消息Id: 27106
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP] = function()
    Net.SCGuildWarDeclareWarStarListResp = Net.SCGuildWarDeclareWarStarListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarDeclareWarStarListResp:__init()
        self:AutoConstruct({
            "declare_war_star_list__T__star_id@H##guild_name@s##active@I##server_id@H", -- 占领的星球列表[星球ID,占领的工会名称(空表示木有),占领的工会活跃,占领的工会服务器ID]
        })
    end
    return Net.SCGuildWarDeclareWarStarListResp
end

-- 消息说明:请求星球列表 消息Id: 27107
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_STAR_LIST_REQ] = function()
    Net.CSGuildWarStarListReq = Net.CSGuildWarStarListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarStarListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildWarStarListReq
end

-- 消息说明:请求星球列表响应 消息Id: 27108
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_STAR_LIST_RESP] = function()
    Net.SCGuildWarStarListResp = Net.SCGuildWarStarListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarStarListResp:__init()
        self:AutoConstruct({
            "star_list__T__star_id@H##guild_name@s##icon_id@H##is_battle@C", -- 星球列表[星球ID,占领的工会名称(空表示木有),工会图标ID,是否战斗中]
        })
    end
    return Net.SCGuildWarStarListResp
end

-- 消息说明:请求宣战 消息Id: 27109
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_DECLARE_WAR_REQ] = function()
    Net.CSGuildWarDeclareWarReq = Net.CSGuildWarDeclareWarReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarDeclareWarReq:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.CSGuildWarDeclareWarReq
end

-- 消息说明:宣战响应 消息Id: 27110
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_DECLARE_WAR_RESP] = function()
    Net.SCGuildWarDeclareWarResp = Net.SCGuildWarDeclareWarResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarDeclareWarResp:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.SCGuildWarDeclareWarResp
end

-- 消息说明:请求布防 消息Id: 27111
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ] = function()
    Net.CSGuildWarOrganizeDefenseReq = Net.CSGuildWarOrganizeDefenseReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarOrganizeDefenseReq:__init()
        self:AutoConstruct({
            "star_id__H",       --布防的星球ID
            "tower_type__H",    --布防的塔类型
            "role_id__I",       --布防的目标角色ID
            "is_to_battle__C",  --是否上阵(1=上阵,2=下阵)
        })
    end
    return Net.CSGuildWarOrganizeDefenseReq
end

-- 消息说明:布防响应 消息Id: 27112
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP] = function()
    Net.SCGuildWarOrganizeDefenseResp = Net.SCGuildWarOrganizeDefenseResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarOrganizeDefenseResp:__init()
        self:AutoConstruct({
            "star_id__H",       --布防的星球ID
            "tower_type__H",    --布防的塔类型
            "role_id__I",       --布防的目标角色ID
            "is_to_battle__C",  --是否上阵(1=上阵,2=下阵)
        })
    end
    return Net.SCGuildWarOrganizeDefenseResp
end

-- 消息说明:请求撤离 消息Id: 27113
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_EVACUATE_REQ] = function()
    Net.CSGuildWarEvacuateReq = Net.CSGuildWarEvacuateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarEvacuateReq:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.CSGuildWarEvacuateReq
end

-- 消息说明:撤离响应 消息Id: 27114
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_EVACUATE_RESP] = function()
    Net.SCGuildWarEvacuateResp = Net.SCGuildWarEvacuateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarEvacuateResp:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.SCGuildWarEvacuateResp
end

-- 消息说明:请求购买对高级星球挑战次数 消息Id: 27115
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ] = function()
    Net.CSGuildWarBuyHighStarBattleTimesReq = Net.CSGuildWarBuyHighStarBattleTimesReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarBuyHighStarBattleTimesReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildWarBuyHighStarBattleTimesReq
end

-- 消息说明:购买对高级星球挑战次数响应 消息Id: 27116
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP] = function()
    Net.SCGuildWarBuyHighStarBattleTimesResp = Net.SCGuildWarBuyHighStarBattleTimesResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarBuyHighStarBattleTimesResp:__init()
        self:AutoConstruct({
            "cur_buy_times__H",     --当前已购买的次数
        })
    end
    return Net.SCGuildWarBuyHighStarBattleTimesResp
end

-- 消息说明:请求领取星球奖励 消息Id: 27117
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ] = function()
    Net.CSGuildWarTakeStarRewardReq = Net.CSGuildWarTakeStarRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarTakeStarRewardReq:__init()
        self:AutoConstruct({
            "take_type__H",     --领取类型
            "take_star_list__T__star_id@H", -- 要领取的星球列表
        })
    end
    return Net.CSGuildWarTakeStarRewardReq
end

-- 消息说明:领取星球奖励响应 消息Id: 27118
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP] = function()
    Net.SCGuildWarTakeStarRewardResp = Net.SCGuildWarTakeStarRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarTakeStarRewardResp:__init()
        self:AutoConstruct({
            "take_type__H",     --领取类型
            "take_star_list__T__star_id@H", -- 要领取的星球列表
        })
    end
    return Net.SCGuildWarTakeStarRewardResp
end

-- 消息说明:求星球详细 消息Id: 27119
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_STAR_DETAIL_REQ] = function()
    Net.CSGuildWarStarDetailReq = Net.CSGuildWarStarDetailReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarStarDetailReq:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.CSGuildWarStarDetailReq
end

-- 消息说明:星球详细响应(todo::响应的内容待完善,得根据客户的表现需要) 消息Id: 27120
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_STAR_DETAIL_RESP] = function()
    Net.SCGuildWarStarDetailResp = Net.SCGuildWarStarDetailResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarStarDetailResp:__init()
        self:AutoConstruct({
            "star_id__H",                  --星球ID
            "belong_guild_id__I",           --所属工会ID(0=表示没有占领)
            "belong_guild_name__s",         --所属工会名称
            "belong_guild_active__I",       --所属工会的活跃
            "belong_guild_server_id__H",    --所属工会的服务器ID
            "belong_guild_icon_id__H",      --所属工会的图标ID
            "attack_guild_id__I",           --攻击工会ID(0=表示没有攻击者)
            "attack_guild_name__s",         --所属工会名称
            "attack_guild_active__I",       --攻击工会的活跃
            "attack_guild_server_id__H",    --攻击工会的服务器ID
            "attack_guild_icon_id__H",      --攻击工会的图标ID
            "declare_war_guild_list__T__guild_name@s##active@I##server_id@H##icon_id@H", -- 对本星球宣战的工会列表
            "is_over__C" ,                     --(1=结束,0=木有结束)
        })
    end
    return Net.SCGuildWarStarDetailResp
end

-- 消息说明: 请求塔列表 消息Id: 27121
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_WAR_TOWER_LIST_REQ] = function()
    Net.CSGuildWarTowerListReq = Net.CSGuildWarTowerListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildWarTowerListReq:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
        })
    end
    return Net.CSGuildWarTowerListReq
end

-- 消息说明: 工会战塔列表响应 消息Id: 27122
-- "role_id__I",           // 角色ID
-- "be_attack_times__H",   // 被攻击次数
-- "head_img_id__H",       // 头像ID
-- "power_I",              // 战力
-- "role_name_s",          // 角色名称

Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_TOWER_LIST_RESP] = function()
    Net.SCGuildWarTowerListResp = Net.SCGuildWarTowerListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarTowerListResp:__init()
        self:AutoConstruct({
            "star_id__H",   --星球ID
            "tower_list__T__tower_id@H##cur_durability@I##monster_beattack_times@H##defender_list@D|DefenderList|", -- 广州塔列表(塔ID, 当前耐久度, 怪物被攻击次数, 防守者列表)
        })
    end
    return Net.SCGuildWarTowerListResp
end

-- 消息说明: 星球数据更新通知 消息Id: 27123
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_WAR_STAR_UPDATE_NOTIFY] = function()
    Net.SCGuildWarStarUpdateNotify = Net.SCGuildWarStarUpdateNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildWarStarUpdateNotify:__init()
        self:AutoConstruct({
            "star_id__H",       --星球ID
            "tower_id__H",      --塔ID
        })
    end
    return Net.SCGuildWarStarUpdateNotify
end