Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 无尽之地数据请求 消息Id: 24701
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_DATA_REQ] = function()
    Net.CSEndlessLandDataReq = Net.CSEndlessLandDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSEndlessLandDataReq
end

-- 消息说明: 无尽之地数据响应 消息Id: 24702
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_DATA_RESP] = function()
    Net.SCEndlessLandDataResp = Net.SCEndlessLandDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandDataResp:__init()
        self:AutoConstruct({
            "today_battle_times__H", --是否今日首次挑战
            "cur_wave__H",  --当前挑战的波数(正在打的)
            "today_wave_record__H", --今日波数记录(已通关的)
            "cur_wave_kill_num__H", --当前波击杀怪的数量
            "history_wave_record__H",--历史波数记录(已通关的)
            "cur_keep_kill_num__H",--当前连杀数量
            "keep_kill_num_record__H",--连杀数量记录
            "today_collision_reward_times__H",--碰撞奖励次数(今日已经获得的次数)
            "last_take_continue_kill_reward__H",--最近领取的连杀奖励(连杀的数量,0表示还没领取)
            "is_first_battle__C",--是否首次挑战(1=是,0=否)
        })
    end
    return Net.SCEndlessLandDataResp
end

-- 消息说明: 无尽之地进入请求 消息Id: 24703
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_ENTER_REQ] = function()
    Net.CSEndlessLandEnterReq = Net.CSEndlessLandEnterReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandEnterReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSEndlessLandEnterReq
end

-- 消息说明: 无尽之地进入响应 消息Id: 24704
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_ENTER_RESP] = function()
    Net.SCEndlessLandEnterResp = Net.SCEndlessLandEnterResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandEnterResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCEndlessLandEnterResp
end

-- 消息说明: 杀怪请求 消息Id: 24705
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_KILL_MONSTER_REQ] = function()
    Net.CSEndlessLandKillMonsterReq = Net.CSEndlessLandKillMonsterReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandKillMonsterReq:__init()
        self:AutoConstruct({
            "index__C", --怪物的索引
            "monster_id__I",    --怪物ID
        })
    end
    return Net.CSEndlessLandKillMonsterReq
end

-- 消息说明: 杀怪响应 消息Id: 24706
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_KILL_MONSTER_RESP] = function()
    Net.SCEndlessLandKillMonsterResp = Net.SCEndlessLandKillMonsterResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandKillMonsterResp:__init()
        self:AutoConstruct({
            "index__C", --怪物的索引
            "monster_id__I",    --怪物ID
            "drop_item_list__T__item_id@I##item_num@H", -- 掉落物品列表            
        })
    end
    return Net.SCEndlessLandKillMonsterResp
end

-- 消息说明: 碰撞请求 消息Id: 24707
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_COLLISION_REQ] = function()
    Net.CSEndlessLandCollisionReq = Net.CSEndlessLandCollisionReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandCollisionReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSEndlessLandCollisionReq
end

-- 消息说明: 碰撞响应 消息Id: 24708
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_COLLISION_RESP] = function()
    Net.SCEndlessLandCollisionResp = Net.SCEndlessLandCollisionResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandCollisionResp:__init()
        self:AutoConstruct({
            "today_collision_reward_times__H",  --今日碰撞奖励次数
            "reward_id__H", --奖励Id
        })
    end
    return Net.SCEndlessLandCollisionResp
end

-- 消息说明: 领取连杀奖励请求 消息Id: 24709
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ] = function()
    Net.CSEndlessLandKeepKillRewardReq = Net.CSEndlessLandKeepKillRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandKeepKillRewardReq:__init()
        self:AutoConstruct({
            "keep_kill_num__H",  --连杀数量(奖励对应的连杀数量)
        })
    end
    return Net.CSEndlessLandKeepKillRewardReq
end

-- 消息说明: 领取连杀奖励响应 消息Id: 24710
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP] = function()
    Net.SCEndlessLandKeepKillRewardResp = Net.SCEndlessLandKeepKillRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandKeepKillRewardResp:__init()
        self:AutoConstruct({
            "keep_kill_num__H", --连杀数量(奖励对应的连杀数量)
        })
    end
    return Net.SCEndlessLandKeepKillRewardResp
end

-- 消息说明: 结束战斗请求 消息Id: 24713
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ENDLESS_LAND_BATTLE_OVER_REQ] = function()
    Net.CSEndlessLandBattleOverReq = Net.CSEndlessLandBattleOverReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEndlessLandBattleOverReq:__init()
        self:AutoConstruct({
            "is_win__C",    --是否获胜
            "wave__H",  --本次挑战的波数
        })
    end
    return Net.CSEndlessLandBattleOverReq
end

-- 消息说明: 结束战斗响应 消息Id: 24714
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ENDLESS_LAND_BATTLE_OVER_RESP] = function()
    Net.SCEndlessLandBattleOverResp = Net.SCEndlessLandBattleOverResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEndlessLandBattleOverResp:__init()
        self:AutoConstruct({
            "is_win__C",    --是否获胜
            "reward_wave__H",--本次挑战奖励波数(0=表示木有奖励)
        })
    end
    return Net.SCEndlessLandBattleOverResp
end

