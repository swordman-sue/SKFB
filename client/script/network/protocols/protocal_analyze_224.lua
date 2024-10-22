Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--日常任务列表请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_DAILY_MISSION_LIST_REQ] = function()
    Net.CSDaliyMissionListReq = Net.CSDaliyMissionListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSDaliyMissionListReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSDaliyMissionListReq
end

--日常任务列表请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_DAILY_MISSION_LIST_RESP] = function()
    Net.SCDaliyMissionListResp = Net.SCDaliyMissionListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCDaliyMissionListResp:__init()
        self:AutoConstruct({
            "point__H",     --积分
            "point_reward_record__T__point@H", -- 积分奖励记录列表
            "mission_list__T__mission_id@H##is_get_reward@C##value@I", -- 任务列表
        })
    end
    return Net.SCDaliyMissionListResp
end

--任务数据通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_DAILY_MISSION_DATA_NOTIFY] = function()
    Net.SCDaliyMissionDataNotifyResp = Net.SCDaliyMissionDataNotifyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCDaliyMissionDataNotifyResp:__init()
        self:AutoConstruct({
            "mission_id__H",    --任务ID
            "is_get_reward__C", --是否已领取奖励
            "value__I",     --任务值
        })
    end
    return Net.SCDaliyMissionDataNotifyResp
end

--日常任务奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_DAILY_MISSION_AWARD_REQ] = function()
    Net.CSDaliyMissionAwardReq = Net.CSDaliyMissionAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSDaliyMissionAwardReq:__init()
        self:AutoConstruct({
            "mission_id__H",    --日常任务Id
        })
    end
    return Net.CSDaliyMissionAwardReq
end

--日常任务奖励领取响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_DAILY_MISSION_AWARD_RESP] = function()
    Net.SCDaliyMissionAwardResp = Net.SCDaliyMissionAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCDaliyMissionAwardResp:__init()
        self:AutoConstruct({
            "mission_id__H",    --日常任务Id
        })
    end
    return Net.SCDaliyMissionAwardResp
end

--日常任务积分奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_DAILY_MISSION_POINT_AWARD_REQ] = function()
    Net.CSDaliyMissionPointAwardReq = Net.CSDaliyMissionPointAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSDaliyMissionPointAwardReq:__init()
        self:AutoConstruct({
            "point__H", --积分
        })
    end
    return Net.CSDaliyMissionPointAwardReq
end

--日常任务积分奖励领取响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_DAILY_MISSION_POINT_AWARD_RESP] = function()
    Net.SCDaliyMissionPointAwardResp = Net.SCDaliyMissionPointAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCDaliyMissionPointAwardResp:__init()
        self:AutoConstruct({
            "point__H", --积分
        })
    end
    return Net.SCDaliyMissionPointAwardResp
end
