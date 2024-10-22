
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--请求圣器列表: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_LIST_REQ] = function()
    Net.CSTreasureListReq = Net.CSTreasureListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTreasureListReq
end

--请求圣器列表响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_LIST_RESP] = function()
    Net.SCTreasureListResp = Net.SCTreasureListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureListResp:__init()
        self:AutoConstruct({
        	"treasure_list__T__uid@I##info_id@H##strengthen_level@C##strengthen_exp@I##refine_level@C##hero_infoid@H"
        })
    end
    return Net.SCTreasureListResp
end

--圣器强化: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_STRENGTHEN_REQ] = function()
    Net.CSTreasureStrengthenReq = Net.CSTreasureStrengthenReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureStrengthenReq:__init()
        self:AutoConstruct({
            "treasure_uid__I",  --圣器uid
			"cost_treasure_list__T__uid@I",  --消耗品
        })
    end
    return Net.CSTreasureStrengthenReq
end

--圣器强化响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_STRENGTHEN_RESP] = function()
    Net.SCTreasureStrengthenResp = Net.SCTreasureStrengthenResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureStrengthenResp:__init()
        self:AutoConstruct({
            "uid__I",  --圣器uid
            "old_level__H",     --旧等级
            "new_level__H",     --新等级
            "exp__I",       --获得的经验
        })
    end
    return Net.SCTreasureStrengthenResp
end

--圣器精练: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_REFINE_REQ] = function()
    Net.CSTreasureRefineReq = Net.CSTreasureRefineReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureRefineReq:__init()
        self:AutoConstruct({
            "treasure_uid__I",  --圣器信息uid
        })
    end
    return Net.CSTreasureRefineReq
end

--圣器精练响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_REFINE_RESP] = function()
    Net.SCTreasureRefineResp = Net.SCTreasureRefineResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureRefineResp:__init()
        self:AutoConstruct({
            "uid__I",  --圣器信息uid
            "refine_level__C",  --圣器精炼等级
        })
    end
    return Net.SCTreasureRefineResp
end

--圣器合成请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_COMPOSE_REQ] = function()
    Net.CSTreasureComposeReq = Net.CSTreasureComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureComposeReq:__init()
        self:AutoConstruct({
            "treasure_info_id__I",  --圣器信息uid
            "treasure_num__H",  --圣器数量
        })
    end
    return Net.CSTreasureComposeReq
end

--圣器合成请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_COMPOSE_RESP] = function()
    Net.SCTreasureComposeResp = Net.SCTreasureComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureComposeResp:__init()
        self:AutoConstruct({
            "treasure_info_id__I",  --圣器信息uid
            "treasure_num__H",  --圣器数量
        })
    end
    return Net.SCTreasureComposeResp
end

--圣器基本信息通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_INFO_NOTIFY] = function()
    Net.SCTreasureInfoNotify = Net.SCTreasureInfoNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureInfoNotify:__init()
        self:AutoConstruct({
            "flag__C",  --标志(更新标志:1=添加,更新,0=删除)
            "uid__I",   --圣器uid
            "info_id__H",   --圣器信息ID
            "strengthen_level__C",  --强化等级
            "strengthen_exp__I",    --强化经验
            "refine_level__C",  --圣器等级
            "hero_infoid__H",  --穿戴的圣器信息ID
        })
    end
    return Net.SCTreasureInfoNotify
end

--穿戴圣器请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_PUT_ON_REQ] = function()
    Net.CSTreasurePutOnReq = Net.CSTreasurePutOnReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasurePutOnReq:__init()
        self:AutoConstruct({
            "formation_pos__C", --阵型位置
            "treasure_pos__C",  --圣器位置(0=表示攻击型圣器，1=表示防御型圣器)
            "treasure_uid__I",  --圣器uid
        })
    end
    return Net.CSTreasurePutOnReq
end

--穿戴圣器响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_PUT_ON_RESP] = function()
    Net.SCTreasurePutOnResp = Net.SCTreasurePutOnResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasurePutOnResp:__init()
        self:AutoConstruct({
            "formation_pos__C", --阵型位置
            "treasure_pos__C",  --圣器位置
            "treasure_uid__I",  --圣器uid
        })
    end
    return Net.SCTreasurePutOnResp
end

-- 消息说明: 圣器升品请求 消息Id: 21414
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TREASURE_QUALITY_UP_REQ] = function()
    Net.CSTreasureQualityUpReq = Net.CSTreasureQualityUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTreasureQualityUpReq:__init()
        self:AutoConstruct({
            "uid__I",   --圣器uid
        })
    end
    return Net.CSTreasureQualityUpReq
end

-- 消息说明: 圣器升品请求 消息Id: 21415
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TREASURE_QUALITY_UP_RESP] = function()
    Net.SCTreasureQualityUpResp = Net.SCTreasureQualityUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTreasureQualityUpResp:__init()
        self:AutoConstruct({
            "uid__I",       --圣器uid
            "info_id__I",       --圣器信息ID
            "enhance_level__H", --强化等级
            "exp__I",       --圣器经验
            "refine_level__H",  --精炼等级
        })
    end
    return Net.SCTreasureQualityUpResp
end