Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 攻城数据请求 消息Id: 24601
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_DATA_REQ] = function()
    Net.CSAttackCityDataReq = Net.CSAttackCityDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAttackCityDataReq
end

-- 消息说明: 攻城数据响应 消息Id: 24602
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_DATA_RESP] = function()
    Net.SCAttackCityDataResp = Net.SCAttackCityDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityDataResp:__init()
        self:AutoConstruct({
            "battle_over_time__I", --战斗结束时间
            "relive_cd__H", --复活CD
            "weak_cd__H", --虚弱CD
            "encourage_times__C", --鼓舞次数
            "take_boss_reward_record_list__T__chapter@H", -- 领取boss奖励记录列表
            "no1_head_img_id__I", --第一名的角色头像ID(上次,0表示木有)
            "no1_role_name__s", --第一名的角色名称(上周)
            "from_chapter__H", --本次活动开始的章节
            "kill_boss_time__I",--击杀BOSS时间(最近的,0表示木有)
        })
    end
    return Net.SCAttackCityDataResp
end

-- 消息说明: 攻城进入请求 消息Id: 24603
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_ENTER_REQ] = function()
    Net.CSAttackCityEnterReq = Net.CSAttackCityEnterReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityEnterReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAttackCityEnterReq
end

-- 消息说明: 攻城进入响应 消息Id: 24604
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_ENTER_RESP] = function()
    Net.SCAttackCityEnterResp = Net.SCAttackCityEnterResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityEnterResp:__init()
        self:AutoConstruct({
            "player_list__T__role_id@I##head_img_id@H##role_name@s##sprite_id@I##map_x@H##map_y@H##title_id@I##nickname_id@I", -- 玩家列表
        })
    end
    return Net.SCAttackCityEnterResp
end

-- 消息说明: 攻城进入通知 消息Id: 24605
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_ENTER_NOTICE] = function()
    Net.SCAttackCityEnterNotice = Net.SCAttackCityEnterNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityEnterNotice:__init()
        self:AutoConstruct({
            "role_id__I", --角色ID
            "head_img_id__H", --头像ID
            "role_name__s", --角色名称
            "sprite_id__I", --精灵ID
            "map_x__H", --地图坐标x
            "map_y__H", --地图坐标y
            "title_id__I", --称号Id
            "nickname_id__I", --昵称Id
        })
    end
    return Net.SCAttackCityEnterNotice
end

-- 消息说明: 攻城离开请求 消息Id: 24606
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_LEAVE_REQ] = function()
    Net.CSAttackCityLeaveReq = Net.CSAttackCityLeaveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityLeaveReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAttackCityLeaveReq
end

-- 消息说明: 攻城离开响应 消息Id: 24607
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_LEAVE_RESP] = function()
    Net.SCAttackCityLeaveResp = Net.SCAttackCityLeaveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityLeaveResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCAttackCityLeaveResp
end

-- 消息说明: 攻城离开通知 消息Id: 24608
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_LEAVE_NOTICE] = function()
    Net.SCAttackCityLeaveNotice = Net.SCAttackCityLeaveNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityLeaveNotice:__init()
        self:AutoConstruct({
            "role_id__I", --角色ID
        })
    end
    return Net.SCAttackCityLeaveNotice
end

-- 消息说明: 攻城行走请求 消息Id: 24609
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_GOTO_REQ] = function()
    Net.CSAttackCityGotoReq = Net.CSAttackCityGotoReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityGotoReq:__init()
        self:AutoConstruct({
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.CSAttackCityGotoReq
end

-- 消息说明: 攻城行走响应 消息Id: 24610
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_GOTO_RESP] = function()
    Net.SCAttackCityGotoResp = Net.SCAttackCityGotoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityGotoResp:__init()
        self:AutoConstruct({
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.SCAttackCityGotoResp
end

-- 消息说明: 攻城行走通知 消息Id: 24611
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_GOTO_NOTICE] = function()
    Net.SCAttackCityGotoNotice = Net.SCAttackCityGotoNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityGotoNotice:__init()
        self:AutoConstruct({
            "role_id__I", --角色ID
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.SCAttackCityGotoNotice
end

-- 消息说明: 怪物列表通知 消息Id: 24612
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_MONSTER_LIST_NOTICE] = function()
    Net.SCAttackCityMonsterListNotice = Net.SCAttackCityMonsterListNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityMonsterListNotice:__init()
        self:AutoConstruct({
            "cur_chapter__H", --当前章节(波数)
            "monster_list__T__uid@I##map_id@I##damage@I##total_hp@I##start_battle_time@I##map_x@H##map_y@H", -- 玩家列表
        })
    end
    return Net.SCAttackCityMonsterListNotice
end

-- 消息说明: 怪物数据通知(主要是伤害有变化) 消息Id: 24613
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_MONSTER_DATA_NOTICE] = function()
    Net.SCAttackCityMonsterDataNotice = Net.SCAttackCityMonsterDataNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityMonsterDataNotice:__init()
        self:AutoConstruct({
            "uid__I", --怪物UID
            "map_id__I", --关卡地图ID
            "damage__I", --伤害
            "total_hp__I", --总HP
            "start_battle_time__I", --开始战斗时间
        })
    end
    return Net.SCAttackCityMonsterDataNotice
end

-- 消息说明: 宝箱列表通知 消息Id: 24614
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_BOX_LIST_NOTICE] = function()
    Net.SCTreasureBoxListNotice = Net.SCTreasureBoxListNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureBoxListNotice:__init()
        self:AutoConstruct({
            "treasure_box_list__T__uid@I##info_id@I##map_x@H##map_y@H", -- 宝箱列表
        })
    end
    return Net.SCTreasureBoxListNotice
end

-- 消息说明: 宝箱消失通知 消息Id: 24615
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_BOX_DISAPPEAR_NOTICE] = function()
    Net.SCTreasureBoxDisappearNotice = Net.SCTreasureBoxDisappearNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureBoxDisappearNotice:__init()
        self:AutoConstruct({
            "uid__I", --宝箱uid
        })
    end
    return Net.SCTreasureBoxDisappearNotice
end

-- 消息说明: 宝箱锁住请求 消息Id: 24616
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_BOX_LOCK_REQ] = function()
    Net.CSTreasureBoxLockReq = Net.CSTreasureBoxLockReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureBoxLockReq:__init()
        self:AutoConstruct({
            "uid__I", --宝箱uid
        })
    end
    return Net.CSTreasureBoxLockReq
end

-- 消息说明: 宝箱锁住响应 消息Id: 24617
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_BOX_LOCK_RESP] = function()
    Net.SCTreasureBoxLockResp = Net.SCTreasureBoxLockResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureBoxLockResp:__init()
        self:AutoConstruct({
            "uid__I", --宝箱uid
        })
    end
    return Net.SCTreasureBoxLockResp
end

-- 消息说明: 宝箱拾取请求 消息Id: 24618
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_BOX_PICK_UP_REQ] = function()
    Net.CSTreasureBoxPickUpReq = Net.CSTreasureBoxPickUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureBoxPickUpReq:__init()
        self:AutoConstruct({
            "uid__I", --宝箱uid
        })
    end
    return Net.CSTreasureBoxPickUpReq
end

-- 消息说明: 宝箱拾取响应 消息Id: 24619
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_BOX_PICK_UP_RESP] = function()
    Net.SCTreasureBoxPickUpResp = Net.SCTreasureBoxPickUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureBoxPickUpResp:__init()
        self:AutoConstruct({
            "uid__I", --宝箱uid
            "info_id__I", --宝箱信息ID(物品ID)
            "item_list__T__id@I##num@I", -- 拾取后获得的物品列表
        })
    end
    return Net.SCTreasureBoxPickUpResp
end

-- 消息说明: 复活请求 消息Id: 24620
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_RELIVE_REQ] = function()
    Net.CSAttackCityReliveReq = Net.CSAttackCityReliveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityReliveReq:__init()
        self:AutoConstruct({
            "is_free_relive__C", --是否免费复活
        })
    end
    return Net.CSAttackCityReliveReq
end

-- 消息说明: 复活响应 消息Id: 24621
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_RELIVE_RESP] = function()
    Net.SCAttackCityReliveResp = Net.SCAttackCityReliveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityReliveResp:__init()
        self:AutoConstruct({
            "is_free_relive__C", --是否免费复活
        })
    end
    return Net.SCAttackCityReliveResp
end

-- 消息说明: 鼓舞请求 消息Id: 24622
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_ENCOURAGE_REQ] = function()
    Net.CSAttackCityEncourageReq = Net.CSAttackCityEncourageReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityEncourageReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSAttackCityEncourageReq
end

-- 消息说明: 鼓舞响应 消息Id: 24623
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_ENCOURAGE_RESP] = function()
    Net.SCAttackCityEncourageResp = Net.SCAttackCityEncourageResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityEncourageResp:__init()
        self:AutoConstruct({

        })
    end
    return Net.SCAttackCityEncourageResp
end

-- 消息说明: 奖励领取请求 消息Id: 24624
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ] = function()
    Net.CSAttackCityTakeBossRewardReq = Net.CSAttackCityTakeBossRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAttackCityTakeBossRewardReq:__init()
        self:AutoConstruct({
            "chapter__H", --领取的章节
        })
    end
    return Net.CSAttackCityTakeBossRewardReq
end

-- 消息说明: 奖励领取响应 消息Id: 24625
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP] = function()
    Net.SCAttackCityTakeBossRewardResp = Net.SCAttackCityTakeBossRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityTakeBossRewardResp:__init()
        self:AutoConstruct({
            "chapter__H", --领取的章节
        })
    end
    return Net.SCAttackCityTakeBossRewardResp
end

-- 消息说明: 高排行列表通知 消息Id: 24626
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE] = function()
    Net.SCAttackCityTopRankListNotice = Net.SCAttackCityTopRankListNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityTopRankListNotice:__init()
        self:AutoConstruct({
            "rank_list__T__head_img_id@H##role_name@s", -- 排行列表
        })
    end
    return Net.SCAttackCityTopRankListNotice
end

-- 消息说明: 攻城排名列新通知 消息Id: 24627
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_RANK_UPDATE_NOTICE] = function()
    Net.SCAttackCityRankUpdateNotice = Net.SCAttackCityRankUpdateNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityRankUpdateNotice:__init()
        self:AutoConstruct({
            "rank__H", --我的排名
            "score__H", --我的积分
        })
    end
    return Net.SCAttackCityRankUpdateNotice
end

-- 消息说明: 攻城活动开始通知 消息Id: 24628
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_ACTIVITY_START_NOTICE] = function()
    Net.SCAttackCityActivityStartNotice = Net.SCAttackCityActivityStartNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityActivityStartNotice:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCAttackCityActivityStartNotice
end

-- 消息说明: 攻城活动结束通知 消息Id: 24629
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE] = function()
    Net.SCAttackCityActivityOverNotice = Net.SCAttackCityActivityOverNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAttackCityActivityOverNotice:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCAttackCityActivityOverNotice
end