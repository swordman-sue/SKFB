Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:活动数据请求 消息Id: 25801
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FACEBOOK_ACTIVITY_DATA_REQ] = function()
    Net.CSFacebookActivityDataReq = Net.CSFacebookActivityDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFacebookActivityDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFacebookActivityDataReq
end

-- 消息说明:活动数据响应 消息Id: 25802
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FACEBOOK_ACTIVITY_DATA_RESP] = function()
    Net.SCFacebookActivityDataResp = Net.SCFacebookActivityDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFacebookActivityDataResp:__init()
        self:AutoConstruct({
            "is_already_take_account_binding_reward__H",--是否已领取帐号绑定奖励(1=领，0=木有)
            "pass_chapter_share_status__I",--通关章节分享状态(0=表示还木有通关副本,1=表示已分享过并获胜奖励,大于1=表示通关副本章节的时间())
            "summon_success_friend_num__H",--召回成功的好友数量
            "invite_success_friend_num__H",--邀请成功的好友数量
            "summon_reward_record_list__T__summon_num@H", -- 召回奖励记录(已领取的)
            "invite_reward_record_list__T__invite_num@H", -- 邀请奖励记录(已领取的)
            "summon_friend_record_list__T__friend_role_id@I", -- 已发出召回的好友记录(游戏内好友)
            "ready_to_commend_time__I",--准备评论时间(0=表示木有,1=表示已评论过, 大于1表示准备评论时间)
        })
    end
    return Net.SCFacebookActivityDataResp
end

-- 消息说明:领取帐号绑定奖励请求 消息Id: 25803
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_REQ] = function()
    Net.CSTakeFacebookAccountBindingRewardReq = Net.CSTakeFacebookAccountBindingRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFacebookAccountBindingRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeFacebookAccountBindingRewardReq
end

-- 消息说明:领取帐号绑定奖励响应 消息Id: 25804
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP] = function()
    Net.SCTakeFacebookAccountBindingRewardResp = Net.SCTakeFacebookAccountBindingRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFacebookAccountBindingRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeFacebookAccountBindingRewardResp
end

-- 消息说明:领取分享奖励请求 消息Id: 25805
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FACEBOOK_SHARE_REWARD_REQ] = function()
    Net.CSTakeFacebookShareRewardReq = Net.CSTakeFacebookShareRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFacebookShareRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeFacebookShareRewardReq
end

-- 消息说明:领取分享奖励响应 消息Id: 25806
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP] = function()
    Net.SCTakeFacebookShareRewardResp = Net.SCTakeFacebookShareRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFacebookShareRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeFacebookShareRewardResp
end

-- 消息说明:好友邀请请求 消息Id: 25807
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FACEBOOK_FRIEND_INVITE_REQ] = function()
    Net.CSFacebookFriendInviteReq = Net.CSFacebookFriendInviteReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFacebookFriendInviteReq:__init()
        self:AutoConstruct({
            "friend_list__T__facebook_account@s",   -- 邀请的好友列表
        })
    end
    return Net.CSFacebookFriendInviteReq
end

-- 消息说明:好友邀请响应 消息Id: 25808
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FACEBOOK_FRIEND_INVITE_RESP] = function()
    Net.SCFacebookFriendInviteResp = Net.SCFacebookFriendInviteResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFacebookFriendInviteResp:__init()
        self:AutoConstruct({
            "friend_list__T__facebook_account@s",   -- 邀请的好友列表
        })
    end
    return Net.SCFacebookFriendInviteResp
end

-- 消息说明:好友召唤请求 消息Id: 25809
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FACEBOOK_FRIEND_SUMMON_REQ] = function()
    Net.CSFacebookFriendSummonReq = Net.CSFacebookFriendSummonReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFacebookFriendSummonReq:__init()
        self:AutoConstruct({
            "friend_list__T__role_id@I",    -- 召唤的好友列表
        })
    end
    return Net.CSFacebookFriendSummonReq
end

-- 消息说明:好友召唤响应 消息Id: 25810
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FACEBOOK_FRIEND_SUMMON_RESP] = function()
    Net.SCFacebookFriendSummonResp = Net.SCFacebookFriendSummonResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFacebookFriendSummonResp:__init()
        self:AutoConstruct({
            "friend_list__T__role_id@I",    -- 召唤的好友列表
        })
    end
    return Net.SCFacebookFriendSummonResp
end


-- 消息说明:领取好友邀请奖励请求 消息Id: 25811
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_REQ] = function()
    Net.CSTakeFacebookFriendInviteRewardReq = Net.CSTakeFacebookFriendInviteRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFacebookFriendInviteRewardReq:__init()
        self:AutoConstruct({
            "invite_num__H",    --领取邀请数量对应的奖励
        })
    end
    return Net.CSTakeFacebookFriendInviteRewardReq
end

-- 消息说明:领取好友邀请奖励响应 消息Id: 25812
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP] = function()
    Net.SCTakeFacebookFriendInviteRewardResp = Net.SCTakeFacebookFriendInviteRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFacebookFriendInviteRewardResp:__init()
        self:AutoConstruct({
            "invite_num__H",    --领取邀请数量对应的奖励
        })
    end
    return Net.SCTakeFacebookFriendInviteRewardResp
end


-- 消息说明:领取好友召唤奖励请求 消息Id: 25813
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_REQ] = function()
    Net.CSTakeFacebookFriendSummonRewardReq = Net.CSTakeFacebookFriendSummonRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFacebookFriendSummonRewardReq:__init()
        self:AutoConstruct({
            "summon_num__H",    --领取召唤数量对应的奖励
        })
    end
    return Net.CSTakeFacebookFriendSummonRewardReq
end

-- 消息说明:领取好友召唤奖励响应 消息Id: 25814
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP] = function()
    Net.SCTakeFacebookFriendSummonRewardResp = Net.SCTakeFacebookFriendSummonRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFacebookFriendSummonRewardResp:__init()
        self:AutoConstruct({
            "summon_num__H",    --领取召唤数量对应的奖励
        })
    end
    return Net.SCTakeFacebookFriendSummonRewardResp
end

-- 消息说明:已经邀请的好友记录请求 消息Id: 25815
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FACEBOOK_FRIEND_INVITE_RECORD_REQ] = function()
    Net.CSFacebookFriendInviteRecordReq = Net.CSFacebookFriendInviteRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFacebookFriendInviteRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFacebookFriendInviteRecordReq
end

-- 消息说明:已经邀请的好友记录响应(响应内容待完善) 消息Id: 25816
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP] = function()
    Net.SCFacebookFriendInviteRecordResp = Net.SCFacebookFriendInviteRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFacebookFriendInviteRecordResp:__init()
        self:AutoConstruct({
            "friend_list__T__facebook_account@s",   -- 好友列表
        })
    end
    return Net.SCFacebookFriendInviteRecordResp
end

-- 消息说明:领取评论奖励请求 消息Id: 25817
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_FACEBOOK_COMMEND_REWARD_REQ] = function()
    Net.CSTakeFacebookCommendRewardReq = Net.CSTakeFacebookCommendRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeFacebookCommendRewardReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSTakeFacebookCommendRewardReq
end

-- 消息说明:领取评论奖励响应 消息Id: 25818
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP] = function()
    Net.SCTakeFacebookCommendRewardResp = Net.SCTakeFacebookCommendRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeFacebookCommendRewardResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCTakeFacebookCommendRewardResp
end