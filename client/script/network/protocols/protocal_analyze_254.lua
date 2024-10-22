Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--答题请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ] = function()
    Net.CSTeamDungeonAnswerQuestionReq = Net.CSTeamDungeonAnswerQuestionReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonAnswerQuestionReq:__init()
        self:AutoConstruct({
            "is_right__C",      --是否答对
            "answer__C",        --答案
        })
    end
    return Net.CSTeamDungeonAnswerQuestionReq
end

--答题响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP] = function()
    Net.SCTeamDungeonAnswerQuestionResp = Net.SCTeamDungeonAnswerQuestionResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAnswerQuestionResp:__init()
        self:AutoConstruct({        
            "is_right__C",      --是否答对
            "answer__C",        --答案       
        })
    end
    return Net.SCTeamDungeonAnswerQuestionResp
end

--答题通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY] = function()
    Net.SCTeamDungeonAnswerQuestionNotify = Net.SCTeamDungeonAnswerQuestionNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAnswerQuestionNotify:__init()
        self:AutoConstruct({
            "role_id__I",       --回答者角色ID
            "answer__C",        --答案
        })
    end
    return Net.SCTeamDungeonAnswerQuestionNotify
end

--对话结束请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_DIALOG_OVER_REQ] = function()
    Net.CSTeamDungeonDialogOverReq = Net.CSTeamDungeonDialogOverReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonDialogOverReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonDialogOverReq
end

--对话结束响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_DIALOG_OVER_RESP] = function()
    Net.SCTeamDungeonDialogOverResp = Net.SCTeamDungeonDialogOverResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonDialogOverResp:__init()
        self:AutoConstruct({            
        })
    end
    return Net.SCTeamDungeonDialogOverResp
end

--下一个NPC请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_NEXT_NPC_REQ] = function()
    Net.CSTeamDungeonNextNPCReq = Net.CSTeamDungeonNextNPCReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonNextNPCReq:__init()
        self:AutoConstruct({
            "npc__H",       --NPCID
        })
    end
    return Net.CSTeamDungeonNextNPCReq
end

--下一个NPC响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_NEXT_NPC_RESP] = function()
    Net.SCTeamDungeonNextNPCResp = Net.SCTeamDungeonNextNPCResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonNextNPCResp:__init()
        self:AutoConstruct({  
            "npc__H",       --NPCID          
        })
    end
    return Net.SCTeamDungeonNextNPCResp
end

--下一个NPC通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY] = function()
    Net.SCTeamDungeonNextNPCNotify = Net.SCTeamDungeonNextNPCNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonNextNPCNotify:__init()
        self:AutoConstruct({
            "npc__H",       --NPCID
        })
    end
    return Net.SCTeamDungeonNextNPCNotify
end

--Boss任务开始请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ] = function()
    Net.CSTeamDungeonBossTaskStartReq = Net.CSTeamDungeonBossTaskStartReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonBossTaskStartReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonBossTaskStartReq
end

--Boss任务开始响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP] = function()
    Net.SCTeamDungeonBossTaskStartResp = Net.SCTeamDungeonBossTaskStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonBossTaskStartResp:__init()
        self:AutoConstruct({         
        })
    end
    return Net.SCTeamDungeonBossTaskStartResp
end

--新任务通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY] = function()
    Net.SCTeamDungeonNewTaskNotify = Net.SCTeamDungeonNewTaskNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonNewTaskNotify:__init()
        self:AutoConstruct({
            "task_id__I",       --任务ID
            "question_id__H",   --题目ID(答题任务时有效)
            "is_new_task__C",   --是否新任务(1=是,0=否)
        })
    end
    return Net.SCTeamDungeonNewTaskNotify
end

--队伍任务数据
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY] = function()
    Net.SCTeamDungeonTaskDataNotify = Net.SCTeamDungeonTaskDataNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonTaskDataNotify:__init()
        self:AutoConstruct({
            "task_start_time__I",   --任务开始时间(整个任务链开始时间)
            "task_link_id__H",  --任务链ID
            "member_list__T__role_id@I##level@H##head_icon@H##power@I##vip_level@C##server_id@H##role_name@s", --成员列表(第一个为队长)
        })
    end
    return Net.SCTeamDungeonTaskDataNotify
end

--抽奖请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_LOTTERY_REQ] = function()
    Net.CSTeamDungeonLotteryReq = Net.CSTeamDungeonLotteryReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonLotteryReq:__init()
        self:AutoConstruct({
            "times__H", --抽奖次数
            "is_cost_item__C",  --是否消耗道具(1=是，0=否)
        })
    end
    return Net.CSTeamDungeonLotteryReq
end

--抽奖响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_LOTTERY_RESP] = function()
    Net.SCTeamDungeonLotteryResp = Net.SCTeamDungeonLotteryResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonLotteryResp:__init()
        self:AutoConstruct({   
            "times__H", --抽奖次数
            "item_list__T__item_id@I##item_num@I", --获得的物品列表   
        })
    end
    return Net.SCTeamDungeonLotteryResp
end

--创建队伍请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_CREATE_TEAM_REQ] = function()
    Net.CSTeamDungeonCreateTeamReq = Net.CSTeamDungeonCreateTeamReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonCreateTeamReq:__init()
        self:AutoConstruct({
            "dungeon_id__H",    --副本ID
        })
    end
    return Net.CSTeamDungeonCreateTeamReq
end

--创建队伍响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_CREATE_TEAM_RESP] = function()
    Net.SCTeamDungeonCreateTeamResp = Net.SCTeamDungeonCreateTeamResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonCreateTeamResp:__init()
        self:AutoConstruct({   
            "dungeon_id__H",    --副本ID
            "team_id__I",   --队伍ID   
        })
    end
    return Net.SCTeamDungeonCreateTeamResp
end

--组队邀请请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_INVITE_REQ] = function()
    Net.CSTeamDungeonInviteReq = Net.CSTeamDungeonInviteReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonInviteReq:__init()
        self:AutoConstruct({
            "target_id__I", --邀请的目标ID
        })
    end
    return Net.CSTeamDungeonInviteReq
end

--组队邀请响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_INVITE_RESP] = function()
    Net.SCTeamDungeonInviteResp = Net.SCTeamDungeonInviteResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonInviteResp:__init()
        self:AutoConstruct({   
            "target_id__I", --邀请的目标ID 
        })
    end
    return Net.SCTeamDungeonInviteResp
end

--组队邀请通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_INVITE_NOTIFY] = function()
    Net.SCTeamDungeonInviteNotify = Net.SCTeamDungeonInviteNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonInviteNotify:__init()
        self:AutoConstruct({   
            "team_id__I",     --邀请的队伍ID
            "server_id__H",   --服务器ID
            "role_name__s",--邀请者角色名称
            "dungeon_id__H",   --副本id           
        })
    end
    return Net.SCTeamDungeonInviteNotify
end

--接受组队邀请请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ] = function()
    Net.CSTeamDungeonAcceptInviteReq = Net.CSTeamDungeonAcceptInviteReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonAcceptInviteReq:__init()
        self:AutoConstruct({
            "team_id__I",   --邀请的队伍ID
            "dungeon_id__H",   --副本id
        })
    end
    return Net.CSTeamDungeonAcceptInviteReq
end

--接受组队邀请响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP] = function()
    Net.SCTeamDungeonAcceptInviteResp = Net.SCTeamDungeonAcceptInviteResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAcceptInviteResp:__init()
        self:AutoConstruct({   
            "team_id__I",   --邀请的队伍ID
            "dungeon_id__H",   --副本id
            "member_list__T__role_id@I##level@H##head_icon@H##power@I##vip_level@C##server_id@H##role_name@s",
        })
    end
    return Net.SCTeamDungeonAcceptInviteResp
end

--接受组队邀请通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY] = function()
    Net.SCTeamDungeonAcceptInviteNotify = Net.SCTeamDungeonAcceptInviteNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAcceptInviteNotify:__init()
        self:AutoConstruct({   
            "role_id__I",   --角色ID
            "level__H", --角色等级
            "head_img_id__H",--头像ID
            "power__I", --战力
            "vip_level__C", --vip等级            
            "server_id__H",    --服务器ID
            "role_name__s", --角色名称
        })
    end
    return Net.SCTeamDungeonAcceptInviteNotify
end

--请求玩家推荐列表请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ] = function()
    Net.CSTeamDungeonRecommendListReq = Net.CSTeamDungeonRecommendListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonRecommendListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonRecommendListReq
end

--请求玩家推荐列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP] = function()
    Net.SCTeamDungeonRecommendListResp = Net.SCTeamDungeonRecommendListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonRecommendListResp:__init()
        self:AutoConstruct({   
            "member_list__T__role_id@I##level@H##head_icon@H##power@I##vip_level@C##server_id@H##role_name@s",
        })
    end
    return Net.SCTeamDungeonRecommendListResp
end

--组队匹配请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_MATCH_REQ] = function()
    Net.CSTeamDungeonMatchReq = Net.CSTeamDungeonMatchReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonMatchReq:__init()
        self:AutoConstruct({
            "dungeon_id__H",    --副本ID
            "is_team__C",   --是否队伍
        })
    end
    return Net.CSTeamDungeonMatchReq
end

--组队匹配响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_RESP] = function()
    Net.SCTeamDungeonMatchResp = Net.SCTeamDungeonMatchResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonMatchResp:__init()
        self:AutoConstruct({   
            "dungeon_id__H",    --副本ID
            "is_team__C",   --是否队伍
        })
    end
    return Net.SCTeamDungeonMatchResp
end

--组队取消匹配请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ] = function()
    Net.CSTeamDungeonCancelMatchReq = Net.CSTeamDungeonCancelMatchReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonCancelMatchReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonCancelMatchReq
end

--组队取消匹配响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP] = function()
    Net.SCTeamDungeonCancelMatchResp = Net.SCTeamDungeonCancelMatchResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonCancelMatchResp:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonCancelMatchResp
end

--离开队伍请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_LEAVE_REQ] = function()
    Net.CSTeamDungeonLeaveReq = Net.CSTeamDungeonLeaveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonLeaveReq:__init()
        self:AutoConstruct({   
        })
    end
    return Net.CSTeamDungeonLeaveReq
end

--离开队伍响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_LEAVE_RESP] = function()
    Net.SCTeamDungeonLeaveResp = Net.SCTeamDungeonLeaveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonLeaveResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTeamDungeonLeaveResp
end

--离开队伍通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_LEAVE_NOTIFY] = function()
    Net.SCTeamDungeonLeaveNotify = Net.SCTeamDungeonLeaveNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonLeaveNotify:__init()
        self:AutoConstruct({   
            "role_id__I",   --离开玩家ID
            "is_task_team__C",--是否任务队伍
        })
    end
    return Net.SCTeamDungeonLeaveNotify
end

--解散队伍通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_DISMISS_NOTIFY] = function()
    Net.SCTeamDungeonDismissNotify = Net.SCTeamDungeonDismissNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonDismissNotify:__init()
        self:AutoConstruct({   
            "is_captain_leave__C",--是否队长离开
        })
    end
    return Net.SCTeamDungeonDismissNotify
end

--对话结束通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY] = function()
    Net.SCTeamDungeonDialogOverNotify = Net.SCTeamDungeonDialogOverNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonDialogOverNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonDialogOverNotify
end

--接任务请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ] = function()
    Net.CSTeamDungeonAcceptTaskReq = Net.CSTeamDungeonAcceptTaskReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonAcceptTaskReq:__init()
        self:AutoConstruct({   
        })
    end
    return Net.CSTeamDungeonAcceptTaskReq
end

--接任务响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP] = function()
    Net.SCTeamDungeonAcceptTaskResp = Net.SCTeamDungeonAcceptTaskResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAcceptTaskResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTeamDungeonAcceptTaskResp
end

--接任务通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY] = function()
    Net.SCTeamDungeonAcceptTaskNotify = Net.SCTeamDungeonAcceptTaskNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonAcceptTaskNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonAcceptTaskNotify
end

--任务链完成通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY] = function()
    Net.SCTeamDungeonTaskLinkFinishNotify = Net.SCTeamDungeonTaskLinkFinishNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonTaskLinkFinishNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonTaskLinkFinishNotify
end

--组队副本任务奖励通知
Net.ProtocolRegistry[Net.ProtocolNo.S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY] = function()
    Net.SCTeamDungeonTaskRewardNotify = Net.SCTeamDungeonTaskRewardNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonTaskRewardNotify:__init()
        self:AutoConstruct({   
            "energy__I",    --法阵能量
            "item_list__T__item_id@I##item_num@I",--普通奖励列表
            "extra_reward_list__T__item_id@I##item_num@I", --额外奖励列表
        })
    end
    return Net.SCTeamDungeonTaskRewardNotify
end

--组队副本匹配成功通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_NOTIFY] = function()
    Net.SCTeamDungeonMatchNotify = Net.SCTeamDungeonMatchNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonMatchNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonMatchNotify
end

--取消组队匹配通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY] = function()
    Net.SCTeamDungeonCancelMatchNotify = Net.SCTeamDungeonCancelMatchNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonCancelMatchNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonCancelMatchNotify
end

--任务开始请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_TASK_START_REQ] = function()
    Net.CSTeamDungeonTaskStartReq = Net.CSTeamDungeonTaskStartReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonTaskStartReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonTaskStartReq
end

--任务开始响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_TASK_START_RESP] = function()
    Net.SCTeamDungeonTaskStartResp = Net.SCTeamDungeonTaskStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonTaskStartResp:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonTaskStartResp
end

--准备队伍数据通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY] = function()
    Net.SCTeamDungeonReadyTeamDataNotify = Net.SCTeamDungeonReadyTeamDataNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonReadyTeamDataNotify:__init()
        self:AutoConstruct({   
            "team_id__I",   --队伍ID
            "member_list__T__role_id@I##level@H##head_icon@H##power@I##vip_level@C##server_id@H##role_name@s",
        })
    end
    return Net.SCTeamDungeonReadyTeamDataNotify
end

--副本排行榜请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_RANK_REQ] = function()
    Net.CSTeamDungeonRankReq = Net.CSTeamDungeonRankReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonRankReq:__init()
        self:AutoConstruct({
            "dungeon_id__H",    --副本id
        })
    end
    return Net.CSTeamDungeonRankReq
end

--副本排行榜响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_RANK_RESP] = function()
    Net.SCTeamDungeonRankResp = Net.SCTeamDungeonRankResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonRankResp:__init()
        self:AutoConstruct({   
            "dungeon_id__H",    --副本id
            "rank_list__T__rank@H##cost_time@I##captain_head_img_id@I##role_name_list@D|RoleNameList|",
        })
    end
    return Net.SCTeamDungeonRankResp
end

--消息说明: 组队副本BOSS奖励通知 消息Id: 25447
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY] = function()
    Net.SCTeamDungeonBossRewardNontify = Net.SCTeamDungeonBossRewardNontify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonBossRewardNontify:__init()
        self:AutoConstruct({   
            "boss_damage__I",   --Boss伤害
            "mvp_id__I",    --mvp_id
            "killer_id__I", --击杀者ID
            "mvp_reward_list__T__item_id@I##item_num@I",    -- MVP奖励
            "kill_reward_list__T__item_id@I##item_num@I",   -- 击杀奖励
            "member_reward_list__T__role_id@I##attack_damage@I##reward_list@D|RerwadList|", -- 成员奖励
        })
    end
    return Net.SCTeamDungeonBossRewardNontify
end

--消息说明: Boss任务开始通知 消息Id: 25450
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY] = function()
    Net.SCTeamDungeonBossTaskStartNotify = Net.SCTeamDungeonBossTaskStartNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonBossTaskStartNotify:__init()
        self:AutoConstruct({   
        })
    end
    return Net.SCTeamDungeonBossTaskStartNotify
end

--组队副本数据请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_DATA_REQ] = function()
    Net.CSTeamDungeonDataReq = Net.CSTeamDungeonDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonDataReq
end

--组队副本数据响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_DATA_RESP] = function()
    Net.SCTeamDungeonDataResp = Net.SCTeamDungeonDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonDataResp:__init()
        self:AutoConstruct({   
            "daily_battle_times__H", --每日挑战次数
            "pass_dungeon_list__T__dungeon_id@H", -- 通关副本列表
        })
    end
    return Net.SCTeamDungeonDataResp
end

--组队副本抽奖记录请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ] = function()
    Net.CSTeamDungeonLotteryRecordReq = Net.CSTeamDungeonLotteryRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonLotteryRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTeamDungeonLotteryRecordReq
end

--组队副本抽奖记录响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP] = function()
    Net.SCTeamDungeonLotteryRecordResp = Net.SCTeamDungeonLotteryRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonLotteryRecordResp:__init()
        self:AutoConstruct({   
            "lottery_record_list__T__server_id@H##role_name@s##item_id@I##item_num@I##record_time@I", -- 抽奖记录列表
        })
    end
    return Net.SCTeamDungeonLotteryRecordResp
end

--组队副本匹配目标数量通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY] = function()
    Net.SCTeamDungeonMatchTargetNumNotify = Net.SCTeamDungeonMatchTargetNumNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonMatchTargetNumNotify:__init()
        self:AutoConstruct({   
            "dungeon_id__H", --副本ID
            "target_num__H", --匹配的目标数量
        })
    end
    return Net.SCTeamDungeonMatchTargetNumNotify
end

--组队快捷聊天请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ] = function()
    Net.CSTeamDungeonShortCutChatReq = Net.CSTeamDungeonShortCutChatReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTeamDungeonShortCutChatReq:__init()
        self:AutoConstruct({
            "chat_id__H", --聊天ID
        })
    end
    return Net.CSTeamDungeonShortCutChatReq
end

--组队快捷聊天响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP] = function()
    Net.SCTeamDungeonShortCutChatResp = Net.SCTeamDungeonShortCutChatResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTeamDungeonShortCutChatResp:__init()
        self:AutoConstruct({   
            "role_id__I", --角色ID
            "chat_id__H", --聊天ID
        })
    end
    return Net.SCTeamDungeonShortCutChatResp
end
