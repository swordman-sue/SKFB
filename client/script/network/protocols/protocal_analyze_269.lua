Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:世界等级任务数据请求 消息Id: 26901
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WORLD_LEVEL_TASK_DATA_REQ] = function()
    Net.CSWorldLevelTaskDataReq = Net.CSWorldLevelTaskDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWorldLevelTaskDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSWorldLevelTaskDataReq
end

-- 消息说明:世界等级任务数据响应 消息Id: 26902
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WORLD_LEVEL_TASK_DATA_RESP] = function()
    Net.SCWorldLevelTaskDataResp = Net.SCWorldLevelTaskDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWorldLevelTaskDataResp:__init()
        self:AutoConstruct({
            "is_already_take_final_reward__C",  --是否已领取最终奖励
            "task_event_list__T__task_id@H##context@I##is_already_take@C##task_level@H", -- 任务事件列表(任务ID，上下文， 是否已领取[1=是,0=否], 任务等级)
        })
    end
    return Net.SCWorldLevelTaskDataResp
end

-- 消息说明:领取世界等级任务奖励请求 消息Id: 26903
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ] = function()
    Net.CSTakeWorldLevelTaksRewardReq = Net.CSTakeWorldLevelTaksRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeWorldLevelTaksRewardReq:__init()
        self:AutoConstruct({
            "task_id__H",   --要领取的任务ID
        })
    end
    return Net.CSTakeWorldLevelTaksRewardReq
end

-- 消息说明:领取世界等级任务奖励响应 消息Id: 26904
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP] = function()
    Net.SCTakeWorldLevelTaksRewardResp = Net.SCTakeWorldLevelTaksRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeWorldLevelTaksRewardResp:__init()
        self:AutoConstruct({
            "task_id__H",   --要领取的任务ID
        })
    end
    return Net.SCTakeWorldLevelTaksRewardResp
end

-- 消息说明:领取世界等级任务最终奖励请求 消息Id: 26905
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ] = function()
    Net.CSTakeWorldLevelTaskFinalRewardReq = Net.CSTakeWorldLevelTaskFinalRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeWorldLevelTaskFinalRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeWorldLevelTaskFinalRewardReq
end

-- 消息说明:领取世界等级任务最终奖励响应 消息Id: 26906
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP] = function()
    Net.SCTakeWorldLevelTaskFinalRewardResp = Net.SCTakeWorldLevelTaskFinalRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeWorldLevelTaskFinalRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeWorldLevelTaskFinalRewardResp
end