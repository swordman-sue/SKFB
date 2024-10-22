
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 请求好友列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_LIST_REQ] = function()
    Net.CSFriendListReq = Net.CSFriendListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFriendListReq
end

--请求好友列表响应 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_LIST_RESP] = function()
    Net.CSFriendListResp = Net.CSFriendListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendListResp:__init()
        self:AutoConstruct({
            "get_energy_times__C",     --已经领取的精力次数
            "friend_list__T__role_id@I##level@C##head_icon@H##battle_value@I##role_name@s##guild_name@s##is_give_energy@C##get_energy_state@C##offline_time@I##vip_level@C", -- 好友列表
        -- 参数说明:
        -- get_energy_state (0=表示不能领取,1=表示可以领取,2=表示已经领取过)
        -- is_give_energy (bool)
                })
    end
    return Net.CSFriendListResp
end



--请求黑名单列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BLACK_LIST_REQ] = function()
    Net.CSBlcakListReq = Net.CSBlcakListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlcakListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSBlcakListReq
end

--请求黑名单列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BLACK_LIST_RESP] = function()
    Net.CSBlcakListResp = Net.CSBlcakListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlcakListResp:__init()
        self:AutoConstruct({
            "black_list__T__role_id@I##level@C##head_icon@H##battle_value@I##role_name@s##guild_name@s##offline_time@I##vip_level@C", -- 黑名单列表

        })
    end
    return Net.CSBlcakListResp
end



--请求申请列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_APPLY_LIST_REQ] = function()
    Net.CSFriendApplyListReq = Net.CSFriendApplyListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendApplyListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSFriendApplyListReq
end

--请求申请列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_APPLY_LIST_RESP] = function()
    Net.CSFriendApplyListResp = Net.CSFriendApplyListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendApplyListResp:__init()
        self:AutoConstruct({
            "apply_list__T__role_id@I##level@C##head_icon@H##battle_value@I##role_name@s##guild_name@s##offline_time@I##vip_level@C", -- 申请列表
        })
    end
    return Net.CSFriendApplyListResp
end


--请求推荐列表
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_RECOMMEND_LIST_REQ] = function()
    Net.CSFriendRecommendListReq = Net.CSFriendRecommendListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendRecommendListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSFriendRecommendListReq
end

--请求推荐列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_RECOMMEND_LIST_RESP] = function()
    Net.CSFriendRecommendListResp = Net.CSFriendRecommendListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendRecommendListResp:__init()
        self:AutoConstruct({
            "recommend_list__T__role_id@I##level@C##head_icon@H##battle_value@I##role_name@s##guild_name@s##offline_time@I##vip_level@C", -- 推荐列表
        })
    end
    return Net.CSFriendRecommendListResp
end

--添加好友
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_ADD_REQ] = function()
    Net.CSFriendAddReq = Net.CSFriendAddReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendAddReq:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendAddReq
end

--添加好友响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_ADD_RESP] = function()
    Net.CSFriendAddResp = Net.CSFriendAddResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendAddResp:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
            "result__H",        --结果(0=表示成功,1=自己的列表已满,2=目标的列表已满)
        })
    end
    return Net.CSFriendAddResp
end




--删除好友
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_DEL_REQ] = function()
    Net.CSFriendDelReq = Net.CSFriendDelReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendDelReq:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendDelReq
end

--删除好友响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_DEL_RESPS] = function()
    Net.CSFriendDelResp = Net.CSFriendDelResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendDelResp:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendDelResp
end




--申请好友回复
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_APPLY_REPLY_REQ] = function()
    Net.CSFriendApplyReplyReq = Net.CSFriendApplyReplyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendApplyReplyReq:__init()
        self:AutoConstruct({
            "is_accept__C",     --是否接受
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendApplyReplyReq
end

--申请好友回复响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_APPLY_REPLY_RESP] = function()
    Net.CSFriendApplyReplyResp = Net.CSFriendApplyReplyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendApplyReplyResp:__init()
        self:AutoConstruct({
            "is_accept__C",     --是否接受
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendApplyReplyResp
end

--添加黑名单
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BLACK_ADD_REQ] = function()
    Net.CSBlcakAddReq = Net.CSBlcakAddReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlcakAddReq:__init()
        self:AutoConstruct({

            "target_id__I",     --目标ID
        })
    end
    return Net.CSBlcakAddReq
end

--添加黑名单响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BLACK_ADD_RESP] = function()
    Net.CSBlcakAddResp = Net.CSBlcakAddResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlcakAddResp:__init()
        self:AutoConstruct({

            "target_id__I",     --目标ID
        })
    end
    return Net.CSBlcakAddResp
end

--删除黑名单
Net.ProtocolRegistry[Net.ProtocolNo.C2S_BLACK_DEL_REQ] = function()
    Net.CSBlcakDelReq = Net.CSBlcakDelReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlcakDelReq:__init()
        self:AutoConstruct({

            "target_id__I",     --目标ID
        })
    end
    return Net.CSBlcakDelReq
end

--删除黑名单响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_BLACK_DEL_RESP] = function()
    Net.CSBlackDelResp = Net.CSBlackDelResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSBlackDelResp:__init()
        self:AutoConstruct({

            "target_id__I",     --目标ID
        })
    end
    return Net.CSBlackDelResp
end

--赠送精力
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_ENERGY_GIVE_REQ] = function()
    Net.CSFriendEnergyGiveReq = Net.CSFriendEnergyGiveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendEnergyGiveReq:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendEnergyGiveReq
end

--赠送精力响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_ENERGY_GIVE_RESP] = function()
    Net.CSFriendEnergyGiveResp = Net.CSFriendEnergyGiveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendEnergyGiveResp:__init()
        self:AutoConstruct({
            "target_id__I",     --目标ID
        })
    end
    return Net.CSFriendEnergyGiveResp
end

--领取精力
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_ENERGY_GET_REQ] = function()
    Net.CSFriendEnergyGetReq = Net.CSFriendEnergyGetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendEnergyGetReq:__init()
        self:AutoConstruct({
            "target_list__T__target_id@I", -- 领取的目标列表
        })
    end
    return Net.CSFriendEnergyGetReq
end

--领取精力响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_ENERGY_GET_RESP] = function()
    Net.CSFriendEnergyGetResp = Net.CSFriendEnergyGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendEnergyGetResp:__init()
        self:AutoConstruct({
            "target_list__T__target_id@I", -- 领取的目标列表
        })
    end
    return Net.CSFriendEnergyGetResp
end

--好友数据更新通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_DATA_GET_RESP] = function()
    Net.CSFriendDataGetResp = Net.CSFriendDataGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendDataGetResp:__init()
        self:AutoConstruct({
            "flag__C",      --标志(0=删除,1=添加)
            "type__C",      --类型(1=好友,2=黑友)
            "role_id__I",       --角色ID  
            "level__C",     --角色等级
            "head_icon__H",     --头像
            "battle_value__I",  --战力
            "role_name__s",     --申请角色名称
            "guild_name__s",    --工会名称
            "is_give_energy__C",    --是否已赠送精力 
            "get_energy_state__C",  --是否已领取精力
            "offline_time__I",  --离线时间
            "vip_level__C",     --VIP等级 
        })
    end
    return Net.CSFriendDataGetResp
end

--查找目标
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FRIEND_TARGET_FIND_REQ] = function()
    Net.CSFriendTargetFindReq = Net.CSFriendTargetFindReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFriendTargetFindReq:__init()
        self:AutoConstruct({
            "target_name__s",   --目标名称
        })
    end
    return Net.CSFriendTargetFindReq
end

--目标查找响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FRIEND_TARGET_FIND_RESP] = function()
    Net.SCFriendTargetFindResp = Net.SCFriendTargetFindResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFriendTargetFindResp:__init()
        self:AutoConstruct({
            "role_id__I",       --角色ID  
            "level__C",     --角色等级
            "head_icon__H",     --头像
            "battle_value__I",  --战力
            "role_name__s",     --角色名称
            "guild_name__s",    --工会名称
            "offline_time__I",  --离线时间
            "vip_level__C",     --VIP等级 
        })
    end
    return Net.SCFriendTargetFindResp
end