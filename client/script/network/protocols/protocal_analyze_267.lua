Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:阵营招募数据请求 消息Id: 26701
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CAMP_RECRUIT_DATA_REQ] = function()
    Net.CSCampRecruitDataReq = Net.CSCampRecruitDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCampRecruitDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSCampRecruitDataReq
end

-- 消息说明:阵营招募数据响应 消息Id: 26702
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CAMP_RECRUIT_DATA_RESP] = function()
    Net.SCCampRecruitDataResp = Net.SCCampRecruitDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCampRecruitDataResp:__init()
        self:AutoConstruct({
            "cur_data_id__H",   --当前排行积分
            "today_already_recruit_times__H",   --今日已招募次数
            "camp_score_list__T__camp@C##score@I",  --积分列表(阵营,积分)
            "score_task_reward_record_list__T__camp@C##record_list@D|RecordList|", --积分任务奖励记录列表

            -- RecordList说明:
            -- reward_idx@H: 奖励位置下标(从0开始)
        })
    end
    return Net.SCCampRecruitDataResp
end

-- 消息说明:阵营招募请求 消息Id: 26703
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CAMP_RECRUIT_REQ] = function()
    Net.CSCampRecruitReq = Net.CSCampRecruitReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCampRecruitReq:__init()
        self:AutoConstruct({
            "recruit_type__H",  --招募类型(1=普通,2=一键)
            "camp__H",  --阵营
        })
    end
    return Net.CSCampRecruitReq
end

-- 消息说明:阵营招募响应 消息Id: 26704
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CAMP_RECRUIT_RESP] = function()
    Net.SCCampRecruitResp = Net.SCCampRecruitResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCampRecruitResp:__init()
        self:AutoConstruct({
            "recruit_type__H",  --招募类型(1=普通,2=一键)
            "camp__H",  --阵营
            "hero_list__T__hero_id@I##hero_num@I",  --招募到的英雄列表(英雄Id,英雄数量)
        })
    end
    return Net.SCCampRecruitResp
end

-- 消息说明:阵营招募积分任务奖励请求 消息Id: 26705
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ] = function()
    Net.CSCampRecruitScoreTaskRewardReq = Net.CSCampRecruitScoreTaskRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCampRecruitScoreTaskRewardReq:__init()
        self:AutoConstruct({
            "camp__H",  --阵营
            "reward_idx__H",    --奖励位置索引(从0开始)
        })
    end
    return Net.CSCampRecruitScoreTaskRewardReq
end

-- 消息说明:阵营招募积分任务奖励响应 消息Id: 26706
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP] = function()
    Net.SCCampRecruitScoreTaskRewardResp = Net.SCCampRecruitScoreTaskRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCampRecruitScoreTaskRewardResp:__init()
        self:AutoConstruct({
            "camp__H",  --阵营
            "reward_idx__H",    --奖励位置索引(从0开始)
        })
    end
    return Net.SCCampRecruitScoreTaskRewardResp
end

-- 消息说明:阵营招募记录请求 消息Id: 26707
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CAMP_RECRUIT_RECORD_REQ] = function()
    Net.CSCampRecruitRecordReq = Net.CSCampRecruitRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCampRecruitRecordReq:__init()
        self:AutoConstruct({
            "camp__H",  --阵营
        })
    end
    return Net.CSCampRecruitRecordReq
end

-- 消息说明:阵营招募记录响应 消息Id: 26708
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CAMP_RECRUIT_RECORD_RESP] = function()
    Net.SCCampRecruitRecordResp = Net.SCCampRecruitRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCampRecruitRecordResp:__init()
        self:AutoConstruct({
            "camp__H",  --阵营
            "hero_list__T__role_name@s##hero_id@I##hero_num@I",     --招募到的英雄列表(英雄Id,英雄数量)
        })
    end
    return Net.SCCampRecruitRecordResp
end