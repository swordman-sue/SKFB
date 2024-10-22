
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--客户端发送
Net.ProtocolRegistry[Net.ProtocolNo.MSG_C2S_CHAT_REQ] = function()
    Net.SCSendStartResp = Net.SCSendStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSendStartResp:__init()
        self:AutoConstruct({
            "chat_type__C",     --聊天类型
            "is_voice__C",      --是否语音
            "target_id__I",     --目标角色Id
            "target_name__s",   --目标角色名称
            "chat_text__s",     --聊天内容
            "voice__s",     --语音
        })
    end
    return Net.SCSendStartResp
end

--服务器返回
Net.ProtocolRegistry[Net.ProtocolNo.MSG_S2C_CHAT_RESP] = function()
    Net.SCReturnStartResp = Net.SCReturnStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCReturnStartResp:__init()
        self:AutoConstruct({

        })
    end
    return Net.SCReturnStartResp
end

--服务器广播
Net.ProtocolRegistry[Net.ProtocolNo.MSG_S2C_CHAT_NOTIFY] = function()
    Net.SCChatStartResp = Net.SCChatStartResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChatStartResp:__init()
        self:AutoConstruct({
            "chat_type__C",     --聊天类型
            "role_id__I",       --发送者角色Id
            "head_icon__I",     --发送者头像
            "server_id__H",     --服务器ID
            "vip_level__C",     --Vip等级
            "role_name__s",     --目标角色名称
            "cur_use_title_id__I",  --当前使用的称号ID
            "voice_id__I",      --语音ID(0=表示木有语音)
            "chat_text__s",     --聊天内容容
            "gm_level__C",
        })
    end
    return Net.SCChatStartResp
end

--消息说明: 聊天记录列表请求 消息Id: 20407
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHAT_RECORD_LIST_REQ] = function()
    Net.CSChatRecordListReq = Net.CSChatRecordListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChatRecordListReq:__init()
        self:AutoConstruct({
            "chat_type__C",     --聊天类型
        })
    end
    return Net.CSChatRecordListReq
end

--消息说明: 聊天记录列表请求 消息Id: 20407
Net.ProtocolRegistry[Net.ProtocolNo. S2C_CHAT_RECORD_LIST_RESP] = function()
    Net.SCChatRecordListResp = Net.SCChatRecordListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChatRecordListResp:__init()
        self:AutoConstruct({
            "chat_type__C",     --聊天类型
            "record_list__T__role_id@I##head_icon@I##server_id@H##vip_level@C##role_name@s##cur_use_title_id@I##chat_text@s##time@I##gm_level@C", -- 记录列表
        })
    end
    return Net.SCChatRecordListResp
end