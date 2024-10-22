
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--请求战斗: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BATTLE_START_REQ] = function()
    Net.CSBattleStartReq = Net.CSBattleStartReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBattleStartReq:__init()
        self:AutoConstruct({
            "type__C",          --战斗类型
            "param1__I",        --参数
            "param2__I",        --参数
            "param3__I",        --参数
            "param4__I",        --参数
            "ass_hole__I",      --参数5(战力)
            "fuck__s",          --预留字段
        })
    end
    return Net.CSBattleStartReq
end

--请求战斗响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BATTLE_START_RESP] = function()
    Net.SCBattleStartResp = Net.SCBattleStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCBattleStartResp:__init()
        self:AutoConstruct({
            "type__C",          		--战斗类型
			"target_fighting_power__I", --目标战力(玩家)
            "help_battle_hero_info_id__I",  --助战英雄信息ID            
            "param1__I",        		--参数
            "param2__I",        		--参数
            "param3__I",        		--参数
            "param4__I",        		--参数
            "drop_list__T__monster_id@I##obj_type@C##obj_id@I##obj_num@H", -- 掉落列表
            "monster_list__T__index@C##damage@I##total_hp@I", -- 怪物列表
            "hero_list__T__info_id@H##pos@C##level@H##quality_level@C##pro_list@D|ProList|##bless_sprite_id@I##bless_sprite_star@H##bless_sprite_enhance_level@H##bless_sprite_train_stage@H", -- 上阵英雄列表(目标)            
            "target_sprite_list__T__id@I##train_stage@H", -- 目标精灵列表
            "help_battle_hero_pro_list__T__type@H##value@I", -- 助战英雄属性列表            
        }) 
    end
    return Net.SCBattleStartResp
end

--请求战斗结算: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BATTLE_END_REQ] = function()
    Net.CSBattleEndReq = Net.CSBattleEndReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBattleEndReq:__init()
        self:AutoConstruct({
            "type__C",          --战斗类型
            "is_win__I",        --是否获胜
            "star__C",          --星数
            "context1__I",       --上下文
            "context2__I",       --上下文
            "ass_hole__I",      --参数(战力)
            "damage__I",        --伤害
            "fuck__s",          --预留字段
            "monster_list__T__index@C##damage@I", -- 怪物列表            
        })
    end
    return Net.CSBattleEndReq
end

--战斗结算响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BATTLE_END_RESP] = function()
    Net.SCBattleStartResp = Net.SCBattleStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCBattleStartResp:__init()
        self:AutoConstruct({
            "is_win__C",
            "star__C",
			"is_first_win__C",  --是否第一次获胜
			"is_me_kill__C",  --是否我击杀
            "param_list__T__param@I", -- 参数列表          
            "item_list__T__id@I##num@I", -- 奖励物品列表
        })
    end
    return Net.SCBattleStartResp
end

--请求助战目标
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BATTLE_ASSIST_TARGET_REQ] = function()
    Net.CSBattleAssistTargetReq = Net.CSBattleAssistTargetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBattleAssistTargetReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSBattleAssistTargetReq
end

--请求助战目标响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BATTLE_ASSIST_TARGET_RESP] = function()
    Net.SCBattleAssistTargetResp = Net.SCBattleAssistTargetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCBattleAssistTargetResp:__init()
        self:AutoConstruct({
            "help_battle_target_list__T__id@I##name@s##level@H##fighting_power@I##hero_info_id@I##target_type@C##help_battle_times@C##vip_level@C", -- 助战目标列表
            "today_used_help_battle_times__H",  --今日使用的助战次数
            -- id :         助战目标ID
            -- name :       助战目标名称
            -- level :      助战目标等级
            -- fighting_power : 助战目标战力
            -- hero_info_id :   助战目标的英雄信息ID
            -- target_type 0 陌生人 1好友 2军团
            -- help_battle_times : 助战次数(已经帮助的)
            -- vip_level: vip等级
        })
    end
    return Net.SCBattleAssistTargetResp
end

