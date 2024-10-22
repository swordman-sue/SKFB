Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--成就列表请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ACHIEVEMENT_LIST_REQ] = function()
    Net.CSAchievementListReq = Net.CSAchievementListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAchievementListReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSAchievementListReq
end

--成就列表响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACHIEVEMENT_LIST_RESP] = function()
    Net.SCAchievementListResp = Net.SCAchievementListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAchievementListResp:__init()
        self:AutoConstruct({
            "achievement_list__T__id@H##level@C##value@I##cur_reward_lv@C", -- 成就列表
        })
    end
    return Net.SCAchievementListResp
end

--成就数据通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACHIEVEMENT_DATA_NOTIFY] = function()
    Net.SCAchievementDataNotifyResp = Net.SCAchievementDataNotifyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAchievementDataNotifyResp:__init()
        self:AutoConstruct({
            "id__H",        --成就ID
            "level__C",     --成就等级
            "value__I",     --成就值
            "cur_reward_lv__C", -- 当前已经领取的奖励等级
        })
    end
    return Net.SCAchievementDataNotifyResp
end

--成就奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ACHIEVEMENT_AWARD_REQ] = function()
    Net.CSAchievementAwardReq = Net.CSAchievementAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAchievementAwardReq:__init()
        self:AutoConstruct({
            "achievement_id__H",        --成就ID
        })
    end
    return Net.CSAchievementAwardReq
end

--成就奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ACHIEVEMENT_AWARD_RESP] = function()
    Net.SCAchievementAwardResp = Net.SCAchievementAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAchievementAwardResp:__init()
        self:AutoConstruct({
            "achievement_id__H",        --成就ID
        })
    end
    return Net.SCAchievementAwardResp
end

