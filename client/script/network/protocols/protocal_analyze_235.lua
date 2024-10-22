Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 请求红点列表 消息Id: 23501
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RED_POINT_LIST_REQ] = function()
    Net.CSRedPointListReq = Net.CSRedPointListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRedPointListReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSRedPointListReq
end

-- 消息说明: 请求红点列表响应 消息Id: 23502
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RED_POINT_LIST_RESP] = function()
    Net.SCRedPointListResp = Net.SCRedPointListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRedPointListResp:__init()
        self:AutoConstruct({
            "red_point_list__T__system_id@H", -- 红点列表
        })
    end
    return Net.SCRedPointListResp
end

-- 消息说明: 请求红点列表响应 消息Id: 23503
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RED_POINT_NOTIFY] = function()
    Net.SCRedPointNotify = Net.SCRedPointNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRedPointNotify:__init()
        self:AutoConstruct({
            "system_id__H",     --系统Id
        })
    end
    return Net.SCRedPointNotify
end