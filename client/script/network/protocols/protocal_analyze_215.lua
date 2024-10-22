
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--请求叛军数据: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REBEL_DATA_REQ] = function()
    Net.CSRebelDataReq = Net.CSRebelDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRebelDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSRebelDataReq
end


--请求叛军数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REBEL_DATA_RESP] = function()
    Net.SCRebelDataResp = Net.SCRebelDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRebelDataResp:__init()
        self:AutoConstruct({
            "today_exploit__I", --今日累积功勋
            "today_damage__I",  --今日累积伤害
            "exploit_rank__H",  --功勋排名
            "damage_rank__H",   --伤害排名
            "is_already_share__C",  --是否已经分享过
            "init_role_level__H",   --角色每日初始等级
            "rebel_list__T__uid@I##discoverer_id@I##discoverer_name@s##total_damage@I##level@H##map_id@H##create_time@I##quality@C",
            -- 叛军ID
            -- 发现者ID
            -- 发现者名称
            -- 总伤害
            -- 叛军等级
            -- 叛军关卡ID
            -- 创建时间
        })
    end
    return Net.SCRebelDataResp
end

--领取功勋奖励: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REBEL_EXPLOIT_AWARD_REQ] = function()
    Net.CSRebelExploitAwardReq = Net.CSRebelExploitAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRebelExploitAwardReq:__init()
        self:AutoConstruct({
            "award_list__T__award_id@H",
            -- 奖励ID
        })
    end
    return Net.CSRebelExploitAwardReq
end

--领取功勋奖励响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REBEL_EXPLOIT_AWARD_RESP] = function()
    Net.SCRebelExploitAwardResp = Net.SCRebelExploitAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRebelExploitAwardResp:__init()
        self:AutoConstruct({
            "award_record_list__T__award_id@H",
            -- 奖励ID
        })
    end
    return Net.SCRebelExploitAwardResp
end

--功勋奖励记录请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ] = function()
    Net.CSRebelExploitAwardRecordReq = Net.CSRebelExploitAwardRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRebelExploitAwardRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSRebelExploitAwardRecordReq
end

--功勋奖励记录响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP] = function()
    Net.SCRebelExploitAwardRecordResp = Net.SCRebelExploitAwardRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRebelExploitAwardRecordResp:__init()
        self:AutoConstruct({
            "award_record_list__T__award_id@H",
        })
    end
    return Net.SCRebelExploitAwardRecordResp
end

--叛军分享: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REBEL_SHARE_REQ] = function()
    Net.CSRebelShareReq = Net.CSRebelShareReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRebelShareReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSRebelShareReq
end

--叛军分享响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REBEL_SHARE_RESP] = function()
    Net.SCRebelShaReResp = Net.SCRebelShaReResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRebelShaReResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCRebelShaReResp
end

--发现叛军通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REBEL_DISCOVER_NOTIFY] = function()
    Net.SCRebelDiscoverNotify = Net.SCRebelDiscoverNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRebelDiscoverNotify:__init()
        self:AutoConstruct({
            "quality__C",   --叛军品质
            "level__H", --叛军等级
            "map_id__H", --叛军关卡
            "rebel_id__I",  --叛军Id
        })
    end
    return Net.SCRebelDiscoverNotify
end
