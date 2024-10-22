Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:请求圣诞节数据 消息Id: 25701
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHRISTMAS_DATA_REQ] = function()
    Net.CSChristmasDataReq = Net.CSChristmasDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChristmasDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSChristmasDataReq
end

-- 消息说明:请求圣诞节数据响应 消息Id: 25702
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_DATA_RESP] = function()
    Net.SCChristmasDataResp = Net.SCChristmasDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasDataResp:__init()
        self:AutoConstruct({
            "total_score__I",   --总积分
            "cur_score__I",     --当前积分
            "lower_boss_level__H",  --低级boss等级
            "lower_boss_hp__I", --低级bossHP
            "lower_boss_death_time__I", --低级boss死亡时间(0=表示还没挂掉,非0也要判断一哈是否复活时间已到)
            "high_boss_level__H",       --高级boss等级
            "high_boss_hp__I",      --高级bossHP
            "high_boss_death_time__I",  --高级boss死亡时间(0=表示还没挂掉,非0也要判断一哈是否复活时间已到)
            "already_take_lower_boss_reward_level__H",--已领取的低级boss奖励等级
            "already_take_high_boss_reward_level__H",--已领取的高级boss奖励等级           
        })
    end
    return Net.SCChristmasDataResp
end

-- 消息说明:进入战斗请求 消息Id: 25703
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHRISTMAS_BATTLE_ENTER_REQ] = function()
    Net.CSChristmasBattleEnterReq = Net.CSChristmasBattleEnterReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChristmasBattleEnterReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSChristmasBattleEnterReq
end

-- 消息说明:进入战斗响应 消息Id: 25704
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_BATTLE_ENTER_RESP] = function()
    Net.SCChristmasBattleEnterResp = Net.SCChristmasBattleEnterResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasBattleEnterResp:__init()
        self:AutoConstruct({                  
        })
    end
    return Net.SCChristmasBattleEnterResp
end

-- 消息说明:离开战斗请求 消息Id: 25705
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHRISTMAS_BATTLE_LEAVE_REQ] = function()
    Net.CSChristmasBattleLeaveReq = Net.CSChristmasBattleLeaveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChristmasBattleLeaveReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSChristmasBattleLeaveReq
end

-- 消息说明:离开战斗响应 消息Id: 25706
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_BATTLE_LEAVE_RESP] = function()
    Net.SCChristmasBattleLeaveResp = Net.SCChristmasBattleLeaveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasBattleLeaveResp:__init()
        self:AutoConstruct({                  
        })
    end
    return Net.SCChristmasBattleLeaveResp
end

-- 消息说明:攻击Boss请求 消息Id: 25707
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHRISTMAS_ATTACK_BOSS_REQ] = function()
    Net.CSChristmasAttackBossReq = Net.CSChristmasAttackBossReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChristmasAttackBossReq:__init()
        self:AutoConstruct({
            "boss_type__H",     --boss类型
            "snowball_id__I",   --雪球ID
        })
    end
    return Net.CSChristmasAttackBossReq
end

-- 消息说明:攻击Boss响应 消息Id: 25708
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_ATTACK_BOSS_RESP] = function()
    Net.SCChristmasAttackBossResp = Net.SCChristmasAttackBossResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasAttackBossResp:__init()
        self:AutoConstruct({        
            "boss_type__H",     --boss类型
            "snowball_id__I",   --雪球ID
            "damage__I",        --伤害(有可能会溢出,即大于当前boss剩余血量)
            "reward_item_id__I",    --奖励的物品ID
            "reward_item_num__I",   --奖励的物品数量          
        })
    end
    return Net.SCChristmasAttackBossResp
end

-- 消息说明:攻击Boss通知 消息Id: 25709
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY] = function()
    Net.SCChristmasAttackBossNotify = Net.SCChristmasAttackBossNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasAttackBossNotify:__init()
        self:AutoConstruct({        
            "boss_type__H",     --boss类型
            "attacker_name__s", --攻击者名称
            "damage__I",        --伤害(有可能会溢出,即大于当前boss剩余血量)
            "reward_item_id__I",    --奖励的物品ID
            "reward_item_num__I",   --奖励的物品数量        
        })
    end
    return Net.SCChristmasAttackBossNotify
end

-- 消息说明:领取奖励请求 消息Id: 25710
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ] = function()
    Net.CSChristmasTakeBossRewardReq = Net.CSChristmasTakeBossRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChristmasTakeBossRewardReq:__init()
        self:AutoConstruct({
            "boss_type__H",     --boss类型
        })
    end
    return Net.CSChristmasTakeBossRewardReq
end

-- 消息说明:领取奖励响应 消息Id: 25711
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP] = function()
    Net.SCChristmasTakeBossRewardResp = Net.SCChristmasTakeBossRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasTakeBossRewardResp:__init()
        self:AutoConstruct({        
            "boss_type__H",     --boss类型         
        })
    end
    return Net.SCChristmasTakeBossRewardResp
end

-- 消息说明:Boss死亡通知 消息Id: 25712
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHRISTMAS_BOSS_DEATH_NOTIFY] = function()
    Net.SCChristmasBossDeathNotify = Net.SCChristmasBossDeathNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChristmasBossDeathNotify:__init()
        self:AutoConstruct({        
            "boss_type__H",     --boss类型
            "boss_death_time__I",   --boss死亡时间(0=表示还没挂掉,非0也要判断一哈是否复活时间已到)
            "boss_level__H",    --boss等级(复活以后的Boss等级)
            "boss_hp__I",       --bossHP(复活以后的BossHP)  
            "killer_id__I",     --击杀者ID      
        })
    end
    return Net.SCChristmasBossDeathNotify
end