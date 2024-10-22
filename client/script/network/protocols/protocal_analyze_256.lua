Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:请求玩家王者赛数据 消息Id: 25601
Net.ProtocolRegistry[Net.ProtocolNo.C2S_KING_MATCH_DATA_REQ] = function()
    Net.CSKingMatchDataReq = Net.CSKingMatchDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSKingMatchDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSKingMatchDataReq
end

-- 消息说明:玩家王者赛数据响应 消息Id: 25602
Net.ProtocolRegistry[Net.ProtocolNo.S2C_KING_MATCH_DATA_RESP] = function()
    Net.SCKingMatchDataResp = Net.SCKingMatchDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCKingMatchDataResp:__init()
        self:AutoConstruct({
            "cur_season__H",        --当前赛季(当前准备进行及正在进行的赛季)
            "my_rank__H",           --我的排名
            "my_rank_value__I",     --我的排名值
            "total_battle_times__H",--总的挑战次数
            "win_times__H",         --总的获胜次数
            "today_win_times__H",   --今日获胜次数
            "level__H",       --段位等级
            "history_level__H",   --历史段位等级
            "star__I",           --上下文件(星数,积分)
            "season_task_reward_record__T__win_times@H",--赛季任务奖励记录            
        })
    end
    return Net.SCKingMatchDataResp
end

-- 消息说明:查找目标请求 消息Id: 25603
Net.ProtocolRegistry[Net.ProtocolNo.C2S_KING_MATCH_SEARCH_TARGET_REQ] = function()
    Net.CSKingMatchSearchTargetReq = Net.CSKingMatchSearchTargetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSKingMatchSearchTargetReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSKingMatchSearchTargetReq
end

-- 消息说明:查找目标响应 消息Id: 25604
Net.ProtocolRegistry[Net.ProtocolNo.S2C_KING_MATCH_SEARCH_TARGET_RESP] = function()
    Net.SCKingMatchSearchTargetResp = Net.SCKingMatchSearchTargetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCKingMatchSearchTargetResp:__init()
        self:AutoConstruct({
        "role_id__I",   --角色ID
        "level__H", --角色等级
        "head_img_id__H",--头像ID
        "power__I", --战力
        "vip_level__C", --vip等级
        "server__H",    --服务器ID
        "role_name__s", --角色名称
        "title_id__I",  --称号ID
        "stage_level__H",--段位等级
        "star__C",--星数
        "rank__H",  --我的排名
        "rank_value__I",--我的排名值
        })
    end
    return Net.SCKingMatchSearchTargetResp
end

-- 消息说明:领取赛季任务奖励请求 消息Id: 25605
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_KING_MATCH_TASK_REWARD_REQ] = function()
    Net.CSTakeKingMatchTaskRewardReq = Net.CSTakeKingMatchTaskRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeKingMatchTaskRewardReq:__init()
        self:AutoConstruct({
            "win_times__H", --领取的获胜次数奖励
        })
    end
    return Net.CSTakeKingMatchTaskRewardReq
end

-- 消息说明:领取赛季任务奖励响应 消息Id: 25606
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_KING_MATCH_TASK_REWARD_RESP] = function()
    Net.SCTakeKingMatchTaskRewardResp = Net.SCTakeKingMatchTaskRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeKingMatchTaskRewardResp:__init()
        self:AutoConstruct({
            "win_times__H", --领取的获胜次数奖励
        })
    end
    return Net.SCTakeKingMatchTaskRewardResp
end

-- 消息说明:赛季排行榜请求 消息Id: 25607
Net.ProtocolRegistry[Net.ProtocolNo.C2S_KING_MATCH_SEASON_RANK_REQ] = function()
    Net.CSKingMatchSeasonRankReq = Net.CSKingMatchSeasonRankReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSKingMatchSeasonRankReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSKingMatchSeasonRankReq
end

-- 消息说明:赛季排行榜响应(待完善...) 消息Id: 25608
-- role_id_I :     角色Id
-- server_id_H :   服务器Id
-- role_name_s:    角色名称
-- head_icon_H:    角色图标
-- level_H:        角色等级
-- battle_value_I: 战力
-- context_I:  上下文
-- extend_I:       扩展值
-- extend1_I:  扩展值1
-- guild_name_s:   工会名称
-- vip_level_C:    VIP等级
-- title_id_I：  称号ID
Net.ProtocolRegistry[Net.ProtocolNo.S2C_KING_MATCH_SEASON_RANK_RESP] = function()
    Net.SCKingMatchSeasonRankResp = Net.SCKingMatchSeasonRankResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCKingMatchSeasonRankResp:__init()
        self:AutoConstruct({
            "season_rank__T__season@H##rank_list@D|RankList|",--赛季排行榜列表
        })
    end
    return Net.SCKingMatchSeasonRankResp
end