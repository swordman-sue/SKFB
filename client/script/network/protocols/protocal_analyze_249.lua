Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:爵位激活请求 消息Id: 24901
Net.ProtocolRegistry[Net.ProtocolNo.C2S_NOBILITY_ACTIVATE_REQ] = function()
    Net.CSNobilityActivateReq = Net.CSNobilityActivateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSNobilityActivateReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSNobilityActivateReq
end

-- 消息说明:爵位激活响应 消息Id: 24902
Net.ProtocolRegistry[Net.ProtocolNo.S2C_NOBILITY_ACTIVATE_RESP] = function()
    Net.SCNobilityActivateResp = Net.SCNobilityActivateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCNobilityActivateResp:__init()
        self:AutoConstruct({
            "nobility_level__H",    --爵位等级
        })
    end
    return Net.SCNobilityActivateResp
end