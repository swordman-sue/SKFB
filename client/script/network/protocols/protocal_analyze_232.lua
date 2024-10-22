Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 积分赛数据请求 消息Id: 23201
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCORE_MATCH_DATA_REQ] = function()
    Net.CSScoreMatchDataReq = Net.CSScoreMatchDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSScoreMatchDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSScoreMatchDataReq
end

-- 消息说明: 积分赛数据响应 消息Id: 23202
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCORE_MATCH_DATA_RESP] = function()
    Net.SCScoreMatchDataResp = Net.SCScoreMatchDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCScoreMatchDataResp:__init()
        self:AutoConstruct({
            "cur_activity_idx__H",      --当前活动索引
            "battle_times__H",          --已挑战次数
            "refresh_times__H",         --已刷新次数
            "buy_battle_times__H",      --已购买挑战的次数
            "camp__C",                  --阵营
            "win_times__H",             --获胜次数
            "battle_target_list__T__target_id@I##head_img@I##title_id@I##target_name@s##server_id@H##battle_value@I##is_dead@C",--挑战目标列表
            "task_reward_record__T__task_id@H",--任务奖励记录列表(已领取)

            -- 参数说明: 目标ID, 头像ID, 当前使用的称号(0=表示木有), 目标所在服务器, 目标战力,  是否已经死亡
        })
    end
    return Net.SCScoreMatchDataResp
end

-- 消息说明: 选择阵营 消息Id: 23203
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCORE_MATCH_SELECT_CAMP_REQ] = function()
    Net.CSScoreMatchSelectCampReq = Net.CSScoreMatchSelectCampReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSScoreMatchSelectCampReq:__init()
        self:AutoConstruct({
            "camp__C",                  --选择的阵营
        })
    end
    return Net.CSScoreMatchSelectCampReq
end

-- 消息说明:选择阵营响应 消息Id: 23204
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCORE_MATCH_SELECT_CAMP_RESP] = function()
    Net.SCScoreMatchSelectCampResp = Net.SCScoreMatchSelectCampResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCScoreMatchSelectCampResp:__init()
        self:AutoConstruct({
            "camp__C",                  --选择的阵营
        })
    end
    return Net.SCScoreMatchSelectCampResp
end

-- 消息说明: 刷新目标 消息Id: 23205
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCORE_MATCH_REFRESH_TARGET_REQ] = function()
    Net.CSScoreMatchRefreshTargetReq = Net.CSScoreMatchRefreshTargetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSScoreMatchRefreshTargetReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSScoreMatchRefreshTargetReq
end

-- 消息说明:刷新目标响应 消息Id: 23206
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCORE_MATCH_REFRESH_TARGET_RESP] = function()
    Net.SCScoreMatchRefreshTargetResp = Net.SCScoreMatchRefreshTargetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCScoreMatchRefreshTargetResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCScoreMatchRefreshTargetResp
end

-- 消息说明: 购买挑战次数请求 消息Id: 23207
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ] = function()
    Net.CSScoreMatchBuyBattleTimesReq = Net.CSScoreMatchBuyBattleTimesReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSScoreMatchBuyBattleTimesReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSScoreMatchBuyBattleTimesReq
end

-- 消息说明:购买挑战次数响应 消息Id: 23208
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP] = function()
    Net.SCScoreMatchBuyBattleTimesResp = Net.SCScoreMatchBuyBattleTimesResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCScoreMatchBuyBattleTimesResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCScoreMatchBuyBattleTimesResp
end

-- 消息说明: 领取挑战任务奖励 消息Id: 23209
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ] = function()
    Net.CSScoreMatchTakeTaskRewardReq = Net.CSScoreMatchTakeTaskRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSScoreMatchTakeTaskRewardReq:__init()
        self:AutoConstruct({
            "task_list__T__task_id@H",--任务列表
        })
    end
    return Net.CSScoreMatchTakeTaskRewardReq
end

-- 消息说明:领取挑战任务奖励响应 消息Id: 23210
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP] = function()
    Net.SCScoreMatchTakeTaskRewardResp = Net.SCScoreMatchTakeTaskRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCScoreMatchTakeTaskRewardResp:__init()
        self:AutoConstruct({
            "task_list__T__task_id@H",--任务列表
        })
    end
    return Net.SCScoreMatchTakeTaskRewardResp
end