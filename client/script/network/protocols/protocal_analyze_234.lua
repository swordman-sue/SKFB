Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 请求角色称号信息 消息Id: 23401
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ROLE_TITLE_INFO_REQ] = function()
    Net.CSRoleTitleInfoReq = Net.CSRoleTitleInfoReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRoleTitleInfoReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSRoleTitleInfoReq
end

-- id:         称号ID
-- over_time :     结束时间(0=永久有效)
-- 消息说明: 请求角色称号信息响应 消息Id: 23402
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_TITLE_INFO_RESP] = function()
    Net.SCRoleTitleInfoResp = Net.SCRoleTitleInfoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRoleTitleInfoResp:__init()
        self:AutoConstruct({
            "cur_use_title_id__I",  --当前使用的称号ID(0表示木有)
            "cur_use_nickname_id__I",--当前使用的昵称ID(0表示木有)
            "title_list__T__id@I##over_time@I", -- 称号列表
        })
    end
    return Net.SCRoleTitleInfoResp
end

-- 消息说明: 使用称号请求 消息Id: 23403
Net.ProtocolRegistry[Net.ProtocolNo.C2S_USE_TITLE_REQ] = function()
    Net.CSUseTitleReq = Net.CSUseTitleReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSUseTitleReq:__init()
        self:AutoConstruct({
            "title_id__I",      --使用的称号ID
        })
    end
    return Net.CSUseTitleReq
end

-- 消息说明: 使用称号响应 消息Id: 23403
Net.ProtocolRegistry[Net.ProtocolNo.S2C_USE_TITLE_RESP] = function()
    Net.SCUseTitleResp = Net.SCUseTitleResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCUseTitleResp:__init()
        self:AutoConstruct({
            "title_id__I",      --使用的称号ID
        })
    end
    return Net.SCUseTitleResp
end