Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 查找目标 消息Id: 24401
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_SEARCH_TARGET_REQ] = function()
    Net.CSPVPSearchTarget = Net.CSPVPSearchTarget or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPSearchTarget:__init()
        self:AutoConstruct({
            "spirit_skill_id__I",   --精灵技能ID            
        })
    end
    return Net.CSPVPSearchTarget
end

-- 消息说明: 查找目标响应 消息Id: 24402
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_SEARCH_TARGET_RESP] = function()
    Net.SCPVPSearchTarget = Net.SCPVPSearchTarget or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPSearchTarget:__init()
        self:AutoConstruct({
            "spirit_skill_id__I",   --精灵技能ID            
        })
    end
    return Net.SCPVPSearchTarget
end

-- 消息说明: 战斗准备通知 消息Id: 24403
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_READY_NOTIFY] = function()
    Net.SCPVPReadyNotify = Net.SCPVPReadyNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPReadyNotify:__init()
        self:AutoConstruct({
            "is_robot__C",      --是否机器人
            "red_role_id__I",   --红方角色ID
            "red_role_name__s", --红方角色名称
            "red_server_id__I", --红方所在服务器ID
            "red_fighting_power__I",--红方战力
            "red_spirit_id__I", --红方出战精灵ID
            "red_spirit_skill_id__I",--红方出战精灵技能ID
            "red_head_image_id__I", --红方角色头像ID
            "red_spirit_enhance_level__H",--红方出战精灵强化等级
            "red_sprite_train_stage__H",--红方出战精灵培养阶
            "red_sprite_train_times__H",--红方出战精灵培次数
            "red_sprite_star__H",--红方出战精灵星数
            "red_hero_list__T__info_id@H##pro_list@D|ProList|##bless_sprite_id@I##bless_sprite_star@H##bless_sprite_enhance_level@H##bless_sprite_train_stage@H", -- 红方英雄列表
            "red_sprite_list__T__id@I##train_stage@H", -- 红方精灵列表
            "blue_role_id__I",  --蓝方角色ID
            "blue_role_name__s",    --蓝方角色名称
            "blue_server_id__I",    --蓝方所在服务器ID
            "blue_fighting_power__I",--蓝方战力
            "blue_spirit_id__I",    --蓝方出战精灵ID
            "blue_spirit_skill_id__I",--蓝方出战精灵技能ID
            "blue_head_image_id__I",--蓝方角色头像ID
            "blue_spirit_enhance_level__H",--蓝方出战精灵强化等级
            "blue_sprite_train_stage__H",--蓝方出战精灵培养阶
            "blue_sprite_train_times__H",--蓝方出战精灵培次数
            "blue_sprite_star__H",--蓝方出战精灵星数
            "blue_hero_list__T__info_id@H##pro_list@D|ProList|##bless_sprite_id@I##bless_sprite_star@H##bless_sprite_enhance_level@H##bless_sprite_train_stage@H", -- 蓝方英雄列表
            "blue_sprite_list__T__id@I##train_stage@H", -- 蓝方精灵列表
        })
    end
    return Net.SCPVPReadyNotify
end

-- 消息说明: 战斗准备完成请求 消息Id: 24404
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_READY_FINISH_REQ] = function()
    Net.CSPVPReadyFinish = Net.CSPVPReadyFinish or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPReadyFinish:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSPVPReadyFinish
end

-- 消息说明: 战斗准备完成响应 消息Id: 24405
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_READY_FINISH_RESP] = function()
    Net.SCPVPReadyFinish = Net.SCPVPReadyFinish or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPReadyFinish:__init()
        self:AutoConstruct({

        })
    end
    return Net.SCPVPReadyFinish
end

-- 消息说明: 战斗开始通知 消息Id: 24406
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_BATTLE_START_NOTIFY] = function()
    Net.SCPVPBattleStartNotify = Net.SCPVPBattleStartNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPBattleStartNotify:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCPVPBattleStartNotify
end

-- 消息说明: 战斗结束通知 消息Id: 24407
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_BATTLE_OVER_NOTIFY] = function()
    Net.SCPVPBattleOverNotify = Net.SCPVPBattleOverNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPBattleOverNotify:__init()
        self:AutoConstruct({
            "winner_id__I", --获胜者ID
            "red_old_rank__H",  --红方旧排名(0=未上榜)
            "red_new_rank__H",  --红方新排名(0=未上榜)
            "red_last_week_rank__H",--红方上周排名(0=未上榜)
            "red_score__H",     --红方本次获得积分
            "blue_old_rank__H", --蓝方旧排名(0=未上榜)
            "blue_new_rank__H", --蓝方新排名(0=未上榜)
            "blue_last_week_rank__H",--蓝方上周排名(0=未上榜)
            "blue_score__H",    --蓝方本次获得积分
        })
    end
    return Net.SCPVPBattleOverNotify
end

-- 消息说明: 使用技能 消息Id: 24408
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_USE_SKILL_REQ] = function()
    Net.CSPVPUseSkill = Net.CSPVPUseSkill or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPUseSkill:__init()
        self:AutoConstruct({
            "obj_id__I",    --使用者ID
            "skill_id__I",  --技能ID
            "x__H", --x
            "y__H", --y
        })
    end
    return Net.CSPVPUseSkill
end

-- 消息说明: 使用技能响应 消息Id: 24409
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_USE_SKILL_RESP] = function()
    Net.SCPVPUseSkillResp = Net.SCPVPUseSkillResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPUseSkillResp:__init()
        self:AutoConstruct({
            "obj_id__I",    --使用者ID
            "skill_id__I",  --技能ID
            "x__H", --x
            "y__H", --y
        })
    end
    return Net.SCPVPUseSkillResp
end

-- 消息说明: 使用技能通知 消息Id: 24410
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_USE_SKILL_NOTIFY] = function()
    Net.SCPVPUseSkillNotify = Net.SCPVPUseSkillNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPUseSkillNotify:__init()
        self:AutoConstruct({
            "obj_id__I",    --使用者ID
            "skill_id__I",  --技能ID
            "x__H", --x
            "y__H", --y
        })
    end
    return Net.SCPVPUseSkillNotify
end

-- 消息说明: HP改变请求 消息Id: 24411
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_HP_CHANGE_REQ] = function()
    Net.CSPVPHpChange = Net.CSPVPHpChange or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPHpChange:__init()
        self:AutoConstruct({
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "change_hp__i",     --改变的HP
            "damage_type__i",   --伤害的类型
            "ass_hole__I",      --参数(战力)
            "fuck__s",          --预留字段            
        })
    end
    return Net.CSPVPHpChange
end

-- 消息说明: HP改变响应 消息Id: 24412
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_HP_CHANGE_RESP] = function()
    Net.SCPVPHpChangeResp = Net.SCPVPHpChangeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPHpChangeResp:__init()
        self:AutoConstruct({
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "change_hp__i",     --改变的HP
            "damage_type__i",   --伤害的类型
        })
    end
    return Net.SCPVPHpChangeResp
end

-- 消息说明: HP改变通知 消息Id: 24413
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_HP_CHANGE_NOTIFY] = function()
    Net.SCPVPHpChangeNotify = Net.SCPVPHpChangeNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPHpChangeNotify:__init()
        self:AutoConstruct({
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "change_hp__i",     --改变的HP
            "damage_type__i",   --伤害的类型
        })
    end
    return Net.SCPVPHpChangeNotify
end

-- 消息说明: 创建buff请求 消息Id: 24414
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_CREATE_BUFF_REQ] = function()
    Net.CSPVPCreateBuff = Net.CSPVPCreateBuff or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPCreateBuff:__init()
        self:AutoConstruct({
            "buff_id__I",       --buffid
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "is_sprite_bless_buff__C",  --是否来自精灵祝福Buff
        })
    end
    return Net.CSPVPCreateBuff
end

-- 消息说明: 创建buff响应 消息Id: 24415
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_CREATE_BUFF_RESP] = function()
    Net.SCPVPCreateBuffResp = Net.SCPVPCreateBuffResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPCreateBuffResp:__init()
        self:AutoConstruct({
            "buff_id__I",       --buffid
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "is_sprite_bless_buff__C",  --是否来自精灵祝福Buff
        })
    end
    return Net.SCPVPCreateBuffResp
end

-- 消息说明: 创建buff通知 消息Id: 24416
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_CREATE_BUFF_NOTIFY] = function()
    Net.SCPVPCreateBuffNotify = Net.SCPVPCreateBuffNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPCreateBuffNotify:__init()
        self:AutoConstruct({
            "buff_id__I",       --buffid
            "src_obj_id__I",    --产生源对象ID
            "tgt_obj_id__I",    --目标对象ID
            "is_sprite_bless_buff__C",  --是否来自精灵祝福Buff
        })
    end
    return Net.SCPVPCreateBuffNotify
end

-- 消息说明: 战斗结果请求 消息Id: 24417
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_BATTLE_RESULT_REQ] = function()
    Net.CSPVPBattleResult = Net.CSPVPBattleResult or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPBattleResult:__init()
        self:AutoConstruct({
            "result__C",     --是否获胜            
        })
    end
    return Net.CSPVPBattleResult
end

-- 消息说明: 战斗结果响应 消息Id: 24418
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_BATTLE_RESULT_RESP] = function()
    Net.SCPVPBattleResult = Net.SCPVPBattleResult or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPBattleResult:__init()
        self:AutoConstruct({
            "result__C",     --是否获胜       
            "red_old_rank__H",  --红方旧排名(0=未上榜)
            "red_new_rank__H",  --红方新排名(0=未上榜)
            "red_last_week_rank__H",--红方上周排名(0=未上榜)
            "red_score__H",     --红方本次获得积分
            "blue_old_rank__H", --蓝方旧排名(0=未上榜)
            "blue_new_rank__H", --蓝方新排名(0=未上榜)
            "blue_last_week_rank__H",--蓝方上周排名(0=未上榜)
            "blue_score__H",    --蓝方本次获得积分     
        })
    end
    return Net.SCPVPBattleResult
end

-- 消息说明: 领取挑战奖励 消息Id: 24419
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_TAKE_BATTLE_REWARD_REQ] = function()
    Net.CSPVPTakeBattleRewardReq = Net.CSPVPTakeBattleRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPTakeBattleRewardReq:__init()
        self:AutoConstruct({
            "type__C",   --类型(1=每日的,2=每周的)
            "take_reward_list__T__times@C", -- 领取的奖励列表
        })
    end
    return Net.CSPVPTakeBattleRewardReq
end

-- 消息说明: 领取挑战奖励响应 消息Id: 24420
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_TAKE_BATTLE_REWARD_RESP] = function()
    Net.SCPVPTakeBattleRewardResp = Net.SCPVPTakeBattleRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPTakeBattleRewardResp:__init()
        self:AutoConstruct({
            "type__C",   --类型(1=每日的,2=每周的)
            "take_reward_list__T__times@C", -- 领取的奖励列表
        })
    end
    return Net.SCPVPTakeBattleRewardResp
end

-- 消息说明: 玩家PVP数据请求 消息Id: 24421
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLAYER_PVP_DATA_REQ] = function()
    Net.CSPlayerPVPDataReq = Net.CSPlayerPVPDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlayerPVPDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSPlayerPVPDataReq
end

-- 消息说明: 玩家PVP数据响应 消息Id: 24422
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLAYER_PVP_DATA_RESP] = function()
    Net.SCPlayerPVPDataResp = Net.SCPlayerPVPDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlayerPVPDataResp:__init()
        self:AutoConstruct({
            "today_battle_times__H", --今日已挑战次数
            "week_battle_times__H",  --本周已挑战次数
            "last_week_rank__H",     --上周排名
            "my_rank__H",    --我的排行(0=表示木有上榜)
            "my_score__I",   --我的积分
            "daily_battle_reward_record_list__T__times@H", -- 每日挑战奖励领取记录列表
            "week_battle_reward_record_list__T__times@H", -- 每周挑战奖励领取记录列表
            "rank_list__T__role_name@s##score@I", -- 前几名排名列表
        })
    end
    return Net.SCPlayerPVPDataResp
end

-- 消息说明: 取消查找目标请求 消息Id: 24425
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PVP_CANCEL_SEARCH_TARGET_REQ] = function()
    Net.CSPVPCancelSearchTargetReq = Net.CSPVPCancelSearchTargetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPVPCancelSearchTargetReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSPVPCancelSearchTargetReq
end

-- 消息说明: 取消查找目标响应 消息Id: 24426
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PVP_CANCEL_SEARCH_TARGET_RESP] = function()
    Net.SCPVPCancelSearchTargetResp = Net.SCPVPCancelSearchTargetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPVPCancelSearchTargetResp:__init()
        self:AutoConstruct({
            "is_in_battle__C", --是否在战斗中
        })
    end
    return Net.SCPVPCancelSearchTargetResp
end
