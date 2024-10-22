Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--消息说明: 工会数据请求: 消息Id: 22101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_DATE_REQ] = function()
    Net.CSGuildDateReq = Net.CSGuildDateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildDateReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildDateReq
end

-- 消息说明: 工会数据响应 消息Id: 22102
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_DATE_RESP] = function()
    Net.SCGuildDateResp = Net.SCGuildDateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildDateResp:__init()
        self:AutoConstruct({
            "position__C",      --职位
            "guild_id__I",      --工会Id
            "guild_name__s",    --工会名称
            "icon_id__H",       --工会图标ID
            "create_time__I",   --创建时间
            "level__H",     --工会等级
            "exp__I",       --工会经验
            "quan_shui__I",     --泉水
            "qy_total_times__C",    --所有成员的泉涌总次数
            "qy_type__C",      --成员自己的泉涌类型
            "notice__s",        --公告
            "declaration__s",   --宣言
            "member_num__C",    --成员数量
            "leader_name__s",   --会长名称
            "qs_reward_record_list__T__quan_shui@H", --泉水奖励领取记录列表
			"last_qy_type__C",		--最近泉涌类型
			"last_qy_value__I",		--最近泉涌值
			"last_qy_target_name__s",	--最近泉涌目标名称
            "red_point_list__T__type@C",    --红点列表(有红点的列表) 
            "active__I",        --工会活跃度
        })
    end
    return Net.SCGuildDateResp
end

-- 消息说明: 创建工会 消息Id: 22103
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_CREATE_REQ] = function()
    Net.CSGuildCreateReq = Net.CSGuildCreateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildCreateReq:__init()
        self:AutoConstruct({
            "icon_id__H",       --工会图标ID
            "guild_name__s",    --工会名称
        })
    end
    return Net.CSGuildCreateReq
end

-- 消息说明: 创建工会响应 消息Id: 22104
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_CREATE_RESP] = function()
    Net.SCGuildCreateResp = Net.SCGuildCreateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildCreateResp:__init()
        self:AutoConstruct({
            "icon_id__H",       --工会图标ID
            "guild_name__s",    --工会名称
        })
    end
    return Net.SCGuildCreateResp
end

-- 消息说明: 工会列表请求 消息Id: 22133
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_LIST_REQ] = function()
    Net.CSGuildListReq = Net.CSGuildListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildListReq
end

-- 消息说明: 工会列表请求响应 消息Id: 22134
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_LIST_RESP] = function()
    Net.SCGuildListResp = Net.SCGuildListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildListResp:__init()
        self:AutoConstruct({
            "guild_list__T__guild_id@I##guild_name@s##icon_id@H##guild_lv@H##member_num@C##leader_name@s##is_full@C##is_apply@C##declaration@s",
        })
    end
    return Net.SCGuildListResp
end

-- 消息说明: 成员列表请求 消息Id: 22131
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_MEMBER_LIST_REQ] = function()
    Net.CSGuildMemberListReq = Net.CSGuildMemberListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildMemberListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildMemberListReq
end

-- 消息说明: 成员列表响应 消息Id: 22132
-- position 1 = 会长 2 = 副会长 3 = 成员
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_MEMBER_LIST_RESP] = function()
    Net.SCGuildMemberListResp = Net.SCGuildMemberListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildMemberListResp:__init()
        self:AutoConstruct({
            "member_list__T__role_id@I##name@s##position@C##level@H##battle_value@I##head_icon@H##total_contribution@I##today_contribution@I##offline_time@I",
        })
    end
    return Net.SCGuildMemberListResp
end

-- 消息说明: 申请列表请求 消息Id: 22127
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_APPLY_LIST_REQ] = function()
    Net.CSGuildApplyListReq = Net.CSGuildApplyListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildApplyListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildApplyListReq
end

-- 消息说明: 申请列表请求响应 消息Id: 22128
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_APPLY_LIST_RESP] = function()
    Net.SCGuildApplyListResp = Net.SCGuildApplyListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildApplyListResp:__init()
        self:AutoConstruct({
            "apply_list__T__role_id@I##name@s##level@H##head_icon@H##battle_value@I##vip_level@C",
        })
    end
    return Net.SCGuildApplyListResp
end

-- 消息说明: 留言列表请求 消息Id: 22129
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_LEAVEMSG_LIST_REQ] = function()
    Net.CSGuildLeavemsgListReq = Net.CSGuildLeavemsgListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildLeavemsgListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildLeavemsgListReq
end

-- 消息说明: 留言列表响应 消息Id: 22130
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_LEAVEMSG_LIST_RESP] = function()
    Net.SCGuildLeavemsgListResp = Net.SCGuildLeavemsgListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildLeavemsgListResp:__init()
        self:AutoConstruct({
            "leave_msg_list__T__time@I##name@s##pos@C##text@s",
        })
    end
    return Net.SCGuildLeavemsgListResp
end

-- 消息说明: 解散工会 消息Id: 22105
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_DISMISS_REQ] = function()
    Net.CSGuildDismissReq = Net.CSGuildDismissReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildDismissReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildDismissReq
end

-- 消息说明: 解散工会响应 消息Id: 22106
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_DISMISS_RESP] = function()
    Net.SCGuildDismissResp = Net.SCGuildDismissResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildDismissResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCGuildDismissResp
end

-- 消息说明: 申请加入工会 消息Id: 22107
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_APPLY_REQ] = function()
    Net.CSGuildApplyReq = Net.CSGuildApplyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildApplyReq:__init()
        self:AutoConstruct({
            "guild_id__I",      --申请加入的工会ID
        })
    end
    return Net.CSGuildApplyReq
end

-- 消息说明: 申请加入工会响应 消息Id: 22108
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_APPLY_RESP] = function()
    Net.SCGuildApplyResp = Net.SCGuildApplyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildApplyResp:__init()
        self:AutoConstruct({
            "guild_id__I",      --申请加入的工会ID
        })
    end
    return Net.SCGuildApplyResp
end

-- 消息说明: 审核申请 消息Id: 22109
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_APPLY_CHECK_REQ] = function()
    Net.CSGuildApplyCheckReq = Net.CSGuildApplyCheckReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildApplyCheckReq:__init()
        self:AutoConstruct({
            "target_id__I",     --审核的目标ID
            "is_accept__C",     --是否接受
        })
    end
    return Net.CSGuildApplyCheckReq
end

-- 消息说明: 审核申请响应 消息Id: 22110
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_APPLY_CHECK_RESP] = function()
    Net.SCGuildApplyCheckResp = Net.SCGuildApplyCheckResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildApplyCheckResp:__init()
        self:AutoConstruct({
            "target_id__I",     --审核的目标ID
            "is_accept__C",     --是否接受
        })
    end
    return Net.SCGuildApplyCheckResp
end

-- 消息说明: 取消申请 消息Id: 22111
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_APPLY_CANCEL_REQ] = function()
    Net.CSGuildApplyCancelReq = Net.CSGuildApplyCancelReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildApplyCancelReq:__init()
        self:AutoConstruct({
            "guild_id__I",      --取消的工会ID
        })
    end
    return Net.CSGuildApplyCancelReq
end

-- 消息说明: 取消申请响应 消息Id: 22112
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_APPLY_CANCEL_RESP] = function()
    Net.SCGuildApplyCancelResp = Net.SCGuildApplyCancelResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildApplyCancelResp:__init()
        self:AutoConstruct({
            "guild_id__I",      --取消的工会ID
        })
    end
    return Net.SCGuildApplyCancelResp
end

-- 消息说明: 踢出工会 消息Id: 22113
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_KICKOUT_REQ] = function()
    Net.CSGuildKickoutReq = Net.CSGuildKickoutReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildKickoutReq:__init()
        self:AutoConstruct({
            "guild_id__I",      --踢出的目标ID
        })
    end
    return Net.CSGuildKickoutReq
end

-- 消息说明: 踢出工会响应 消息Id: 22114
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_KICKOUT_RESP] = function()
    Net.SCGuildKickoutResp = Net.SCGuildKickoutResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildKickoutResp:__init()
        self:AutoConstruct({
            "guild_id__I",      --踢出的目标ID
        })
    end
    return Net.SCGuildKickoutResp
end

-- 消息说明: 退出工会 消息Id: 22115
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_LEAVE_REQ] = function()
    Net.CSGuildLeaveReq = Net.CSGuildLeaveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildLeaveReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildLeaveReq
end

-- 消息说明: 退出工会响应 消息Id: 22116
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_LEAVE_RESP] = function()
    Net.SCGuildLeaveResp = Net.SCGuildLeaveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildLeaveResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCGuildLeaveResp
end

-- 消息说明: 留言 消息Id: 22119
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_LEAVEM_ESSAGE_REQ] = function()
    Net.CSGuildLeaveEssageReq = Net.CSGuildLeaveEssageReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildLeaveEssageReq:__init()
        self:AutoConstruct({
            "leave_msg__s",     --留言内容
        })
    end
    return Net.CSGuildLeaveEssageReq
end

-- 消息说明: 留言响应 消息Id: 22120
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_LEAVEM_ESSAGE_RESP] = function()
    Net.SCGuildLeaveEssageResp = Net.SCGuildLeaveEssageResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildLeaveEssageResp:__init()
        self:AutoConstruct({
            "time__I",      --留言时间
            "name__s",      --留言者名称
            "pos__C",       --留言者职位
            "text__s",      --留言内容
        })
    end
    return Net.SCGuildLeaveEssageResp
end

-- 消息说明: 搜索工会 消息Id: 22121
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_SEARCH_REQ] = function()
    Net.CSGuildSearchReq = Net.CSGuildSearchReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildSearchReq:__init()
        self:AutoConstruct({
            "guild_id__I",      --搜索工会的Id
            "guild_name__s",    --搜索工会的名称
        })
    end
    return Net.CSGuildSearchReq
end

-- 消息说明: 搜索工会响应 消息Id: 22122
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_SEARCH_RESP] = function()
    Net.SCGuildSearchResp = Net.SCGuildSearchResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildSearchResp:__init()
        self:AutoConstruct({
            "guild_list__T__guild_id@I##guild_name@s##icon_id@H##guild_lv@H##member_num@C##leader_name@s##is_full@C##is_apply@C##declaration@s",
        })
    end
    return Net.SCGuildSearchResp
end

-- 消息说明: 泉涌 消息Id: 22123
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_QY_REQ] = function()
    Net.CSGuildQyReq = Net.CSGuildQyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildQyReq:__init()
        self:AutoConstruct({
            "type__C",      --类型
        })
    end
    return Net.CSGuildQyReq
end

-- 消息说明: 泉涌响应 消息Id: 22124
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_QY_RESP] = function()
    Net.SCGuildQyResp = Net.SCGuildQyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildQyResp:__init()
        self:AutoConstruct({
            "type__C",      --类型
            "exp__I",       --获得工会经验
            "contribution__I",  --获得工会贡献值
        })
    end
    return Net.SCGuildQyResp
end

-- 消息说明: 领取泉水奖励 消息Id: 22125
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_QS_AWARD_REQ] = function()
    Net.CSGuildQsAwardReq = Net.CSGuildQsAwardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildQsAwardReq:__init()
        self:AutoConstruct({
            "qs__H",      --要领取的泉水奖励
        })
    end
    return Net.CSGuildQsAwardReq
end

-- 消息说明: 领取泉水奖励响应 消息Id: 22126
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_QS_AWARD_RESP] = function()
    Net.SCGuildQsAwardResp = Net.SCGuildQsAwardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildQsAwardResp:__init()
        self:AutoConstruct({
            "qs__H",      --要领取的泉水奖励
        })
    end
    return Net.SCGuildQsAwardResp
end

-- 消息说明: 任命 消息Id: 22117
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_APPOINT_REQ] = function()
    Net.CSGuildAppointReq = Net.CSGuildAppointReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildAppointReq:__init()
        self:AutoConstruct({
            "pos__C",       --任命的职位
            "target_id__I",     --任命目标ID
        })
    end
    return Net.CSGuildAppointReq
end

-- 消息说明: 任命响应 消息Id: 22118
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_APPOINT_RESP] = function()
    Net.SCGuildAppointResp = Net.SCGuildAppointResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildAppointResp:__init()
        self:AutoConstruct({
            "pos__C",       --任命的职位
            "target_id__I",     --任命目标ID
        })
    end
    return Net.SCGuildAppointResp
end

-- 消息说明: 修改工会信息请求 消息Id: 22135
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_INFO_MODIFY_REQ] = function()
    Net.CSGuildInfoModifyReq = Net.CSGuildInfoModifyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildInfoModifyReq:__init()
        self:AutoConstruct({
            "type__C",      --类型(1=图标,2=公告,3=宣言)
            "icon_id__H",   --图标ID
            "text__s",      --文本
        })
    end
    return Net.CSGuildInfoModifyReq
end

-- 消息说明: 修改工会信息响应 消息Id: 22136
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_INFO_MODIFY_RESP] = function()
    Net.SCGuildInfoModifyResp = Net.SCGuildInfoModifyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildInfoModifyResp:__init()
        self:AutoConstruct({
            "type__C",      --类型(1=图标,2=公告,3=宣言)
            "icon_id__H",       --图标ID
            "text__s",      --文本
        })
    end
    return Net.SCGuildInfoModifyResp
end

-- 消息说明: 工会事件列表请求 消息Id: 22137
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_EVENT_LIST_REQ] = function()
    Net.CSGuildEventListReq = Net.CSGuildEventListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildEventListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildEventListReq
end

-- 消息说明: 工会事件列表响应 消息Id: 22138
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_EVENT_LIST_RESP] = function()
    Net.SCGuildEventListResp = Net.SCGuildEventListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildEventListResp:__init()
        self:AutoConstruct({
            "event_list__T__event_id@H##target_name@s##value@I##time@I",
        })
    end
    return Net.SCGuildEventListResp
end

-- 消息说明: 技能升级请求 消息Id: 22141
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_SKILL_LEVEL_UP_REQ] = function()
    Net.CSGuildSkillLevelUpReq = Net.CSGuildSkillLevelUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildSkillLevelUpReq:__init()
        self:AutoConstruct({
            "type__C",      --升级的类型(1=工会,2=个人)
            "skill_id__H",      --技能ID
            "skill_level__H",       --技能等级
        })
    end
    return Net.CSGuildSkillLevelUpReq
end

-- 消息说明: 技能升级响应 消息Id: 22142
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_SKILL_LEVEL_UP_RESP] = function()
    Net.SCGuildSkillLevelUpResp = Net.SCGuildSkillLevelUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildSkillLevelUpResp:__init()
        self:AutoConstruct({
            "type__C",      --升级的类型(1=工会,2=个人)
            "skill_id__H",      --技能ID
            "skill_level__H",       --技能等级
        })
    end
    return Net.SCGuildSkillLevelUpResp
end

-- 消息说明: 工会技能列表请求 消息Id: 22139
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_SKILL_LIST_REQ] = function()
    Net.CSGuildSkillListReq = Net.CSGuildSkillListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildSkillListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildSkillListReq
end

-- 消息说明: 工会技能列表响应 消息Id: 22140
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_SKILL_LIST_RESP] = function()
    Net.SCGuildSkillListResp = Net.SCGuildSkillListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildSkillListResp:__init()
        self:AutoConstruct({
            "my_skill_list__T__skill_id@H##skill_level@H",
            "guild_skill_list__T__skill_id@H##skill_level@H",
        })
    end
    return Net.SCGuildSkillListResp
end

-- 消息说明: 军团副本数据请求 消息Id: 22143
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_DATA_REQ] = function()
    Net.CSGuildFbDataReq = Net.CSGuildFbDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbDataReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSGuildFbDataReq
end

-- 消息说明: 军团副本数据响应 消息Id: 22144
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_DATA_RESP] = function()
    Net.SCGuildFbDataResp = Net.SCGuildFbDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbDataResp:__init()
        self:AutoConstruct({
            "total_hp__I", --总的HP
            "damage__I", --伤害
            "curr_pass_chapter__H",  --当前通关的章节(0=表示还没有通关)
            "today_start_chapter__H",    --今日开始章节
            "is_from_history_chapter__C",    --是否从历史章节开始
            "battle_times__C",   --已挑战的次数
            "buy_battle_times__C",   --已购买的挑战次数
            "battle_times_last_recover_time__I", --挑战次数上一次恢复时间
            "take_chapter_reward_record__T__chapter@H", --已经领取的奖励章节记录列表
        })
    end
    return Net.SCGuildFbDataResp
end

-- 消息说明: 军团副本章节奖励请求 消息Id: 22145
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_CHAPTER_REWARD_REQ] = function()
    Net.CSGuildFbChapterRewardReq = Net.CSGuildFbChapterRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbChapterRewardReq:__init()
        self:AutoConstruct({
            "chapter__H",       --领取的章节
        })
    end
    return Net.CSGuildFbChapterRewardReq
end

-- 消息说明: 军团副本章节奖励响应 消息Id: 22146
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_CHAPTER_REWARD_RESP] = function()
    Net.SCGuildFbChapterRewardResp = Net.SCGuildFbChapterRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbChapterRewardResp:__init()
        self:AutoConstruct({
            "chapter__H",       --领取的章节
        })
    end
    return Net.SCGuildFbChapterRewardResp
end

-- 消息说明: 军团副本关卡奖励请求 消息Id: 22147
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_MAP_REWARD_REQ] = function()
    Net.CSGuildFbMapRewardReq = Net.CSGuildFbMapRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbMapRewardReq:__init()
        self:AutoConstruct({
            "chapter__H",       --请求的章节
            "camp__C",      --请求的阵营
            "reward_idx__C",    --领取的奖励索引(从0开始)
        })
    end
    return Net.CSGuildFbMapRewardReq
end

-- 消息说明: 军团副本关卡奖励响应 消息Id: 22148
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_MAP_REWARD_RESP] = function()
    Net.SCGuildFbMapRewardResp = Net.SCGuildFbMapRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbMapRewardResp:__init()
        self:AutoConstruct({
            "chapter__H",       --请求的章节
            "camp__C",      --请求的阵营
            "box_idx__C",       --领取的宝箱索引(从0开始,在界面的位置)
            "reward_idx__C",    --领取的奖励索引(从0开始,在表里的位置)
        })
    end
    return Net.SCGuildFbMapRewardResp
end

-- 消息说明: 军团副本关卡奖励记录请求 消息Id: 22149
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_MAP_REWARD_RECORD_REQ] = function()
    Net.CSGuildFbMapRewardRecordReq = Net.CSGuildFbMapRewardRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbMapRewardRecordReq:__init()
        self:AutoConstruct({
            "chapter__H",       --请求的章节
            "camp__C",      --请求的阵营
        })
    end
    return Net.CSGuildFbMapRewardRecordReq
end

-- 消息说明: 军团副本关卡奖励记录响应 消息Id: 22150
-- box_idx@H: 宝箱索引(在界面上的位置)
-- reward_idx@H : 奖励索引(在配置表中的位置)
-- ##role_name@s : 领取者名称
-- 奖励列表在配置表中的索引
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_MAP_REWARD_RECORD_RESP] = function()
    Net.SCGuildFbMapRewardRecordResp = Net.SCGuildFbMapRewardRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbMapRewardRecordResp:__init()
        self:AutoConstruct({
            "chapter__H",       --请求的章节
            "camp__C",      --请求的阵营
            "take_box_record_list__T__box_idx@H##reward_idx@H##role_name@s", -- 已领取的宝箱记录列表
        })
    end
    return Net.SCGuildFbMapRewardRecordResp
end













-- 消息说明: 军团副本挑战次数购买 消息Id: 22153 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ] = function()
    Net.CSGuildFbBattleTimesBuyReq = Net.CSGuildFbBattleTimesBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbBattleTimesBuyReq:__init()
        self:AutoConstruct({
            "buy_times__C",     --购买次数
        })
    end
    return Net.CSGuildFbBattleTimesBuyReq
end

-- 消息说明: 军团副本挑战次数购买响应 消息Id: 22154 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP] = function()
    Net.SCGuildFbBattleTimesBuyResp = Net.SCGuildFbBattleTimesBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbBattleTimesBuyResp:__init()
        self:AutoConstruct({
            "buy_times__C",     --购买次数
        })
    end
    return Net.SCGuildFbBattleTimesBuyResp
end

-- 消息说明: 工会弹劾请求 消息Id: 22155 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_IMPEACH_REQ] = function()
    Net.CSGuildImpeachReq = Net.CSGuildImpeachReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildImpeachReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildImpeachReq
end

-- 消息说明: 工会弹劾响应 消息Id: 22156 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_IMPEACH_RESP] = function()
    Net.SCGuildImpeachResp = Net.SCGuildImpeachResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildImpeachResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCGuildImpeachResp
end

-- 消息说明: 工会关卡怪物数据请求 消息Id: 22157
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_MONSTER_DATA_REQ] = function()
    Net.CSGuildMonsterDataReq = Net.CSGuildMonsterDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildMonsterDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildMonsterDataReq
end

-- 列表的顺序代表阵营
-- killer_name: 击杀者名称
-- MonsterList说明:
-- "index__C",     --槽位索引
-- "damage__I",    --伤害
-- "total_hp__I",  --总的HP
-- 消息说明: 工会关卡怪物数据请求 消息Id: 22158
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_MONSTER_DATA_RESP] = function()
    Net.SCGuildMonsterDataResp = Net.SCGuildMonsterDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildMonsterDataResp:__init()
        self:AutoConstruct({
            "camp_list__T__killer_name@s##monster_list@D|MonsterList|",  --怪物列表
        })
    end
    return Net.SCGuildMonsterDataResp
end

-- 消息说明: 军团副本伤害记录请求 消息Id: 22151
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_DAMAGE_RECORD_REQ] = function()
    Net.CSGuildFbDamageRecordReq = Net.CSGuildFbDamageRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbDamageRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildFbDamageRecordReq
end

-- 消息说明: 军团副本伤害记录响应 消息Id: 22152
-- 列表字段说明:
-- head_icon@H: 头像
-- damage@I: 伤害
-- battle_times@C: 战斗次数
-- role_name@s: 角色名称
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_DAMAGE_RECORD_RESP] = function()
    Net.SCGuildFbDamageRecordResp = Net.SCGuildFbDamageRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbDamageRecordResp:__init()
        self:AutoConstruct({
            "damage_list__T__head_icon@H##damage@I##battle_times@C##role_name@s", -- 伤害列表
        })
    end
    return Net.SCGuildFbDamageRecordResp
end

-- 消息说明: 工会副本章节重置 消息Id: 22159
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_FB_CHAPTER_RESET_REQ] = function()
    Net.CSGuildFbChapterResetReq = Net.CSGuildFbChapterResetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildFbChapterResetReq:__init()
        self:AutoConstruct({
            "is_history_record__C",     --是否重置到历史记
        })
    end
    return Net.CSGuildFbChapterResetReq
end

-- 消息说明: 工会副本章节重置响应 消息Id: 22160
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_FB_CHAPTER_RESET_RESP] = function()
    Net.SCGuildFbChapterResetResp = Net.SCGuildFbChapterResetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildFbChapterResetResp:__init()
        self:AutoConstruct({
            "is_history_record__C",     --是否重置到历史记录
        })
    end
    return Net.SCGuildFbChapterResetResp
end

-- 消息说明: 工会怪物伤害通知 消息Id: 22161
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_MONSNTER_DAMAGE_NOTIFY] = function()
    Net.SCGuildMonsnterDamageNotify = Net.SCGuildMonsnterDamageNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildMonsnterDamageNotify:__init()
        self:AutoConstruct({
            "chapter__H",       --所属的章节
            "camp__C",      --所属的阵营
            "damage__I",        --伤害
            "cur_total_hp__I",  --当前总HP
            "attack_name__s",   --攻击者名称
        })
    end
    return Net.SCGuildMonsnterDamageNotify
end

-- 消息说明: 我的关卡奖励记录请求 消息Id: 22162
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUILD_MY_MAP_REWARD_RECORD_REQ] = function()
    Net.CSGuildMyMapRewardRecordReq = Net.CSGuildMyMapRewardRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuildMyMapRewardRecordReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGuildMyMapRewardRecordReq
end

-- 消息说明: 我的关卡奖励记录响应 消息Id: 22163.
-- CampList说明:
-- "camp__C",      --阵营(0=超能,1=传奇(传说),2=魔兽(幻想))
-- "is_take_reward__C",    --是否已领取奖励(1=已领取,0=还木有)
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_MY_MAP_REWARD_RECORD_RESP] = function()
    Net.SCGuildMyMapRewardRecordResp = Net.SCGuildMyMapRewardRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildMyMapRewardRecordResp:__init()
        self:AutoConstruct({
            "reward_record_list__T__chapter@H##camp_list@D|CampList|", -- 记录列表
        })
    end
    return Net.SCGuildMyMapRewardRecordResp
end

-- 消息说明: 工会弹劾通知 消息Id: 22164
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUILD_IMPEACH_NOTICE] = function()
    Net.SCGuildImpeachNotice = Net.SCGuildImpeachNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuildImpeachNotice:__init()
        self:AutoConstruct({
            "new_captain_id__I" --新队长ID
        })
    end
    return Net.SCGuildImpeachNotice
end