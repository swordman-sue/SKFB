Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明: 玩家留言请求 消息Id: 24101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_PLAYER_LEVEL_MESSAGE_REQ] = function()
    Net.CSPlayerLevelMessageReq = Net.CSPlayerLevelMessageReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSPlayerLevelMessageReq:__init()
        self:AutoConstruct({
            "contack__s",   --联系方式
            "text__s",  --留言内容
        })
    end
    return Net.CSPlayerLevelMessageReq
end

-- 消息说明: 玩家留言响应 消息Id: 24102
Net.ProtocolRegistry[Net.ProtocolNo.S2C_PLAYER_LEVEL_MESSAGE_RESP] = function()
    Net.SCPlayerLevelMessageResp = Net.SCPlayerLevelMessageResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCPlayerLevelMessageResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCPlayerLevelMessageResp
end