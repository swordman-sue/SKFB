Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:转盘数据请求 消息Id: 26601
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TURNTABLE_DATA_REQ] = function()
    Net.CSTurntableDataReq = Net.CSTurntableDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTurntableDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTurntableDataReq
end

-- 消息说明:转盘数据响应 消息Id: 26602
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TURNTABLE_DATA_RESP] = function()
    Net.SCTurntableDataResp = Net.SCTurntableDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTurntableDataResp:__init()
        self:AutoConstruct({
            "cur_rank_score__I",    --当前排行积分
            "cur_activity_data_id__H",  --当前活动数据ID
            "diamond__I",   --钻石
            "lottery_record_list__T__time@I##index@H##context@I##role_name@s", --抽奖记录列表(抽奖时间戳, 数据下标,上下文[物品数量,钻石],角色名称)
        })
    end
    return Net.SCTurntableDataResp
end

-- 消息说明:转盘抽奖请求 消息Id: 26603
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TURNTABLE_LOTTERY_REQ] = function()
    Net.CSTurntableLotteryReq = Net.CSTurntableLotteryReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTurntableLotteryReq:__init()
        self:AutoConstruct({
            "lottery_type__H",  --抽奖类型(0=普通抽奖,1=一键抽奖)
            "is_cost_item__C",  --是否消耗道具(1=是，0=否)
        })
    end
    return Net.CSTurntableLotteryReq
end

-- 消息说明:转盘抽奖响应 消息Id: 26604
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TURNTABLE_LOTTERY_RESP] = function()
    Net.SCTurntableLotteryResp = Net.SCTurntableLotteryResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTurntableLotteryResp:__init()
        self:AutoConstruct({
            "lottery_type__H",  --抽奖类型(0=普通抽奖,1=一键抽奖)
            "lottery_result_list__T__index@H##context@I", -- 抽奖结果列表(数据下标,上下文[物品数量,钻石])
        })
    end
    return Net.SCTurntableLotteryResp
end