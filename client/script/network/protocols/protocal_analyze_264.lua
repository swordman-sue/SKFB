Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:抽奖请求 消息Id: 26401
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ND_LOTTERY_REQ] = function()
    Net.CSNDLotteryReq = Net.CSNDLotteryReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSNDLotteryReq:__init()
        self:AutoConstruct({
            "times__H",     --抽奖次数
            "is_cost_item__C",  --是否消耗道具(1=表示消耗道具 0=表示消耗钻石)
        })
    end
    return Net.CSNDLotteryReq
end

-- 消息说明:抽奖响应 消息Id: 26402
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ND_LOTTERY_RESP] = function()
    Net.SCNDLotteryResp = Net.SCNDLotteryResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNDLotteryResp:__init()
        self:AutoConstruct({
            "times__H", --抽奖次数
            "item_list__T__item_id@I##item_num@I", --获得的物品列表
        })
    end
    return Net.SCNDLotteryResp
end

-- 消息说明:抽奖记录请求 消息Id: 26403
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ND_LOTTERY_RECORD_REQ] = function()
    Net.CSNDLotteryRecordReq = Net.CSNDLotteryRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSNDLotteryRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSNDLotteryRecordReq
end

-- 消息说明:抽奖记录响应 消息Id: 26404
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ND_LOTTERY_RECORD_RESP] = function()
    Net.SCNDLotteryRecordResp = Net.SCNDLotteryRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNDLotteryRecordResp:__init()
        self:AutoConstruct({
            "lottery_record_list__T__server_id@H##role_name@s##item_id@I##item_num@I##record_time@I", -- 抽奖记录列表
        })
    end
    return Net.SCNDLotteryRecordResp
end