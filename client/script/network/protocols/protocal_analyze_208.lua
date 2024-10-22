
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 请求排行榜列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RANK_LIST_REQ] = function()
    Net.CSRankListReq = Net.CSRankListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankListReq:__init()
        self:AutoConstruct({
            "type__C",  --排行榜类型
            "sub_type__C",  --排行榜子类型(目标只有世界BOSS排行用上)
        })
    end
    return Net.CSRankListReq
end

--请求排行榜列表响应
-- role_id :   角色Id
-- server_id :     服务器Id
-- role_name:  角色名称
-- head_icon:  角色图标
-- level:      角色等级
-- battle_value:   战力
-- context:    上下文
-- extend:     扩展值
-- extend1:    扩展值1
-- guild_name: 工会名称
-- vip_level:  VIP等级
-- title_id:   称号ID
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RANK_LIST_RESP] = function()
    Net.CSRankListResp = Net.CSRankListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankListResp:__init()
        self:AutoConstruct({
            "type__C",   --排行榜类型
            "sub_type__C",  --排行榜子类型(目标只有世界BOSS排行用上)
            "rank__H",  --排名(自己的)
            "rank_value__I",--排名值(自己的)
            "rank_list__T__role_id@I##server_id@I##role_name@s##head_icon@H##level@C##battle_value@I##context@I##extend@I##extend1@I##guild_name@s##vip_level@C##title_id@I",
        })
    end
    return Net.CSRankListResp
end

--点赞like
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RANK_LIKE_REQ] = function()
    Net.CSRankLikeReq = Net.CSRankLikeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankLikeReq:__init()
        self:AutoConstruct({
            "type__C",  --点赞类型
            "target_id__I", --点赞目标ID
        })
    end
    return Net.CSRankLikeReq
end


--点赞响应 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RANK_LIKE_RESP] = function()
    Net.CSRankLikeResp = Net.CSRankLikeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankLikeResp:__init()
        self:AutoConstruct({
            "type__C",  --点赞类型
            "target_id__I", --点赞目标ID
        })
    end
    return Net.CSRankLikeResp
end


--留言
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RANK_LEAVE_MESSAGE_REQ] = function()
    Net.CSRankMessageReq = Net.CSRankMessageReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankMessageReq:__init()
        self:AutoConstruct({
            "message__s",   --留言内容
        })
    end
    return Net.CSRankMessageReq
end


--留言响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RANK_LEAVE_MESSAGE_RESP] = function()
    Net.CSRankLeaveMessageResp = Net.CSRankLeaveMessageResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankLeaveMessageResp:__init()
        self:AutoConstruct({
            "message__s",   --留言内容
        })
    end
    return Net.CSRankLeaveMessageResp
end


--请求留言列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RANK_LEAVE_MESSAGE_LIST_REQ] = function()
    Net.CSRankMessageListReq = Net.CSRankMessageListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankMessageListReq:__init()
        self:AutoConstruct({
           
        })
    end
    return Net.CSRankMessageListReq
end


--请求留言列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RANK_LEAVE_MESSAGE_LIST_RESP] = function()
    Net.CSRankLeaveMessageListResp = Net.CSRankLeaveMessageListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRankLeaveMessageListResp:__init()
        self:AutoConstruct({
            "leave_message_list__T__role_id@I##message@s",
        })
    end
    return Net.CSRankLeaveMessageListResp
end

-- 消息说明: 请求工会排行榜列表 消息Id: 20809
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_RANK_LIST_REQ] = function()
    Net.CSGuildRankListReq = Net.CSGuildRankListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildRankListReq:__init()
        self:AutoConstruct({
            "rank_type__C", --排行榜类型(0=等级,1=副本)
        })
    end
    return Net.CSGuildRankListReq
end

-- 消息说明: 请求工会排行榜列表响应 消息Id: 20810
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_RANK_LIST_RESP] = function()
    Net.SCGuildRankListResp = Net.SCGuildRankListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildRankListResp:__init()
        self:AutoConstruct({
            "rank_type__C", --排行榜类型(0=等级,1=副本)
            "rank__H",  --排名(自己工会的)
            "rank_value__I",--排名值(自己工会的)
            "rank_list__T__guild_id@I##guild_name@s##caption_name@s##icon_id@H##level@C##value@I##member_num@C",
        })
    end
    return Net.SCGuildRankListResp
end

-- 消息说明: 请求我的排名 消息Id: 20811
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MY_RANK_REQ] = function()
    Net.CSMYRankReq = Net.CSMYRankReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMYRankReq:__init()
        self:AutoConstruct({
            "rank_type__H", --排名类型
        })
    end
    return Net.CSMYRankReq
end

-- 消息说明: 请求我的排名响应 消息Id: 20812
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MY_RANK_RESP] = function()
    Net.SCMYRankResp = Net.SCMYRankResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMYRankResp:__init()
        self:AutoConstruct({
            "rank_type__H", --排名类型
            "my_rank__H",   --我的排名 0=表示木有进入排名
        })
    end
    return Net.SCMYRankResp
end
