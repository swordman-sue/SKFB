Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 世界BOSS数据请求 消息Id: 22501
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_DATA_REQ] = function()
    Net.CSWorldBossDataReq = Net.CSWorldBossDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSWorldBossDataReq
end

-- 消息说明: 世界BOSS数据响应 消息Id: 22502
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_DATA_RESP] = function()
    Net.SCWorldBossDataResp = Net.SCWorldBossDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossDataResp:__init()
        self:AutoConstruct({
            "boss_level__H",    --BOSS等级
            "camp__C",      --选择的阵营(99=表示还木有选择)
            "battle_times__C",  --挑战次数(已经使用的)
            "last_recovery_time__I",--上次恢复时间(挑战次数)
            "buy_battle_times__C",  --已经购买的挑战次数
            "honor__I",     --我的荣誉
            "damage__I",        --我的伤害
            "honor_rank__H",    --我的荣誉排名
            "damage_rank__H",   --我的伤害排名
            "relive_time__I",   --BOSS复活时间(0=表示还木有挂掉)
            "remain_hp__I",     --BOSS剩余血量
            "rank_list__T__honor@I##role_name@s", -- 排行列表
        })
    end
    return Net.SCWorldBossDataResp
end

-- 消息说明: 选择阵营请求 消息Id: 22503
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_SELECT_CAMP_REQ] = function()
    Net.CSWorldBossSelectCampReq = Net.CSWorldBossSelectCampReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossSelectCampReq:__init()
        self:AutoConstruct({
            "camp__C",      --选择的阵营
        })
    end
    return Net.CSWorldBossSelectCampReq
end

-- 消息说明: 选择阵营响应 消息Id: 22504
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_SELECT_CAMP_RESP] = function()
    Net.SCWorldBossSelectCampResp = Net.SCWorldBossSelectCampResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossSelectCampResp:__init()
        self:AutoConstruct({
            "camp__C",      --选择的阵营
        })
    end
    return Net.SCWorldBossSelectCampResp
end

-- 消息说明: 领取奖励请求 消息Id: 22505
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_REWARD_REQ] = function()
    Net.CSWorldBossRewardReq = Net.CSWorldBossRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossRewardReq:__init()
        self:AutoConstruct({
            "reward_type__C",       --奖励类型(1=荣誉,2=伤害)
            "reward_list__T__reward_id@H", -- 奖励列表
        })
    end
    return Net.CSWorldBossRewardReq
end

-- 消息说明: 领取奖励响应 消息Id: 22506
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_REWARD_RESP] = function()
    Net.SCWorldBossRewardResp = Net.SCWorldBossRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossRewardResp:__init()
        self:AutoConstruct({
            "reward_type__C",       --奖励类型(1=荣誉,2=伤害)
            "reward_list__T__reward_id@H", -- 奖励列表
        })
    end
    return Net.SCWorldBossRewardResp
end

-- 消息说明: 领取奖励记录请求 消息Id: 22507
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_REWARD_RECORD_REQ] = function()
    Net.CSWorldBossRewardRecordReq = Net.CSWorldBossRewardRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossRewardRecordReq:__init()
        self:AutoConstruct({
            "reward_type__C",   --奖励类型(1=荣誉,2=伤害)
        })
    end
    return Net.CSWorldBossRewardRecordReq
end

-- 消息说明: 领取奖励记录响应 消息Id: 22508
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_REWARD_RECORD_RESP] = function()
    Net.SCWorldBossRewardRecordResp = Net.SCWorldBossRewardRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossRewardRecordResp:__init()
        self:AutoConstruct({
            "reward_type__C",   --奖励类型(1=荣誉,2=伤害)
            "reward_list__T__reward_id@H", -- 奖励记录列表
        })
    end
    return Net.SCWorldBossRewardRecordResp
end

-- 消息说明: 世界BOSS战报请求 消息Id: 22509
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_BATTLE_RECORD_REQ] = function()
    Net.CSWorldBossBattleRecordReq = Net.CSWorldBossBattleRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossBattleRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSWorldBossBattleRecordReq
end

-- 消息说明: 世界BOSS战报响应 消息Id: 22510
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_BATTLE_RECORD_RESP] = function()
    Net.SCWorldBossBattleRecordResp = Net.SCWorldBossBattleRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossBattleRecordResp:__init()
        self:AutoConstruct({
            "record_list__T__boss_lv@H##luck_attack_time@I##kill_time@I##luck_attack_name@s##kill_name@s", -- 记录列表
        })
    end
    return Net.SCWorldBossBattleRecordResp
end

-- 消息说明: 世界BOSS挑战次数购买 消息Id: 22511
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ] = function()
    Net.CSWorldBossBattleTimesBuyReq = Net.CSWorldBossBattleTimesBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldBossBattleTimesBuyReq:__init()
        self:AutoConstruct({
            "buy_times__C",     --购买次数
        })
    end
    return Net.CSWorldBossBattleTimesBuyReq
end

-- 消息说明: 世界BOSS挑战次数购买响应 消息Id: 22512
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP] = function()
    Net.SCWorldBossBattleTimesBuyResp = Net.SCWorldBossBattleTimesBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossBattleTimesBuyResp:__init()
        self:AutoConstruct({
            "buy_times__C",     --购买次数
        })
    end
    return Net.SCWorldBossBattleTimesBuyResp
end

-- boss_lv: 战报boss等级
-- luck_reward_name: 获得幸运奖励玩家名称
-- kill_reward_name: 获得击杀奖励玩家名称

-- 消息说明: 世界BOSS伤害公告 消息Id: 22511
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_DAMAGE_NOTIFY] = function()
    Net.SCWorldBossDamageNotify = Net.SCWorldBossDamageNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossDamageNotify:__init()
        self:AutoConstruct({
            "damage__I",        --伤害HP
            "cur_total_hp__I",  --当前总的HP
            "attack_name__s",   --攻击者名称
        })
    end
    return Net.SCWorldBossDamageNotify
end

-- 消息说明: 世界BOSS活动开始 消息Id: 22514
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY] = function()
    Net.SCWorldBossActivityStartNotify = Net.SCWorldBossActivityStartNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossActivityStartNotify:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCWorldBossActivityStartNotify
end

-- 消息说明: 世界BOSS活动结束 消息Id: 22515
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY] = function()
    Net.SCWorldBossActivityOverNotify = Net.SCWorldBossActivityOverNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldBossActivityOverNotify:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCWorldBossActivityOverNotify
end