Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--竞技场数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ARENA_DATA_REQ] = function()
    Net.CSArenaDataReq = Net.CSArenaDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSArenaDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSArenaDataReq
end

--竞技场数据请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ARENA_DATA_RESP] = function()
    Net.SCArenaDataResp = Net.SCArenaDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCArenaDataResp:__init()
        self:AutoConstruct({
            "rank__H",      --我的排名
            "history_rank__H",  --我的历史排名
            "target_list__T__target_id@I##rank@H##target_name@s##battle_value@I##head_icon@H##title_id@I##nickname_id@I", -- 目标列表
        })
    end
    return Net.SCArenaDataResp
end

--竞技场战报数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ARENA_FIGHTREPORTDATA_REQ] = function()
    Net.CSArenaFightReportDataReq = Net.CSArenaFightReportDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSArenaFightReportDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSArenaFightReportDataReq
end

--竞技场战报数据请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ARENA_FIGHTREPORTDATA_RESP] = function()
    Net.SCArenaFightReportDataResp = Net.SCArenaFightReportDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCArenaFightReportDataResp:__init()
        self:AutoConstruct({
            "record_list__T__battle_time@I##target_id@I##target_name@s##head_icon@H##is_win@C##is_attack@C##before_rank@H##after_rank@H##target_level@H", -- 目标列表
        })
    end
    return Net.SCArenaFightReportDataResp
end
