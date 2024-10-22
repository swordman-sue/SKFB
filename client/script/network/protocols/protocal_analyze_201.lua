
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--游戏错误提示: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GAME_ERROR_NOTIFY] = function()
    Net.SCGameErrorNotify = Net.SCGameErrorNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGameErrorNotify:__init()
        self:AutoConstruct({
            "error_code__H", --错误码
            "from_msg_id__H", --引起错误的消息ID            
        })
    end
    return Net.SCGameErrorNotify
end

--角色基本信息同步: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_INFO_NOTIFY] = function()
    Net.SCRoleInfoNotify = Net.SCRoleInfoNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRoleInfoNotify:__init()
        self:AutoConstruct({
            "role_id__I",       -- 角色ID
            "role_name__s",     -- 角色名称
            "account__s",       -- 角色帐号
            "stamina__I",       -- 体力
            "level__H",     -- 等级
            "exp__I",       -- 经验
            "sex__C",       -- 性别
            "gold__I",      -- 金币   
            "head_icon__I",     -- 头像图标
            "diamond__I",       -- 钻石
            "energy__I",        -- 精力
            "arena_prestige__I",-- 竞技场声望
            "hero_soul__I",     -- 雄魂(魂晶)
            "god_soul__I",      -- 神魂
            "wanted__I",        -- 通缉令
            "battle_value__I",  -- 战力
            "vip_level__C",     -- VIP等级
            "vip_exp__I",       -- VIP经验
            "click_like_times__C",  -- 点赞次数
            "rebel_value__I",       -- 叛军值(战功)
            "tower_prestige__I",    -- 爬塔声望(威名)
            "guild_id__I",          -- 工会ID(0=表示木有)
            "guild_contribution__I",-- 工会贡献值
            "daily_init_level__H",  -- 角色每日初始等级
            "stamina_recover_time__I",  -- 体力恢复时间(最近一次)
            "energy_recover_time__I",   -- 精力恢复时间(最近一次)
            "wanted_recover_time__I",   -- 通缉令恢复时间(最近一次)
            "guide_list__T__key@I##value@I", -- 引导数据列表
            "open_server_time__I",      -- 开服时间
			"server_id__I",      		-- 服务器ID
			"honor__I",      			-- 荣誉
			"total_friendship_point__H",	-- 总的友情点
			"today_friendship_point__H",	-- 今日友情点		
            "endless_store__I", -- 无尽石	
            "role_create_time__I",  -- 角色创建时间
            "nobility_level__H",    -- 爵位等级
            "gem_coin__I",  -- 宝石币
            "destiny_coin__I",  -- 命运硬币
            "star_soul__I", -- 星魂
            "crystal__I", -- 星魂
			"score_match_honor__I", -- 积分赛荣誉     
            "ycoin__I", -- 越南币       
            "resource_war_score__I",   --资源战积分
            "total_recharge_amount__I",    --充值总额度
        })
    end
    return Net.SCRoleInfoNotify
end

--角色属性改变通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_PROPERTY_NOTIFY] = function()
    Net.SCRolePropertyNotify = Net.SCRolePropertyNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRolePropertyNotify:__init()
        self:AutoConstruct({
            "type__C",      -- 属性类型
            "value__I",     -- 属性值            
        })
    end
    return Net.SCRolePropertyNotify
end

--角色升级
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_LEVEL_UPGRADE] = function()
    Net.SCRoleLevelUpgrade = Net.SCRoleLevelUpgrade or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRoleLevelUpgrade:__init()
        self:AutoConstruct({
            "level__H", --当前升级到的等级
        })
    end
    return Net.SCRoleLevelUpgrade
end

--心跳包: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HEART_BEAT] = function()
    Net.CSHeartBeat = Net.CSHeartBeat or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeartBeat:__init()
        self:AutoConstruct({
            "value__I",
        })
    end
    return Net.CSHeartBeat
end

--心跳包响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HEART_BEAT] = function()
    Net.SCHeartBeat = Net.SCHeartBeat or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeartBeat:__init()
    self:AutoConstruct({
        "value__I",
    })
    end
    return Net.SCHeartBeat
end

--请求服务器时间: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SERVER_TIME_REQ] = function()
    Net.CSServerTimeReq = Net.CSServerTimeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSServerTimeReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSServerTimeReq
end

--请求服务器时间响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SERVER_TIME_RESP] = function()
    Net.SCServerTimeResp = Net.SCServerTimeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCServerTimeResp:__init()
        self:AutoConstruct({
            "server_time__I",   --服务器时间
            "year__H",      --年份
            "month__C",     --月份
            "day__C",       --日
            "hour__C",      --小时
            "minute__C",        --分钟
            "second__C",        --秒
            "make_time__I",
        })
    end
    return Net.SCServerTimeResp
end

-- 角色详细请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ROLE_DETAILS_REQ] = function()
    Net.CSRoleDetailsReq = Net.CSRoleDetailsReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRoleDetailsReq:__init()
        self:AutoConstruct({
            "type__C",  --请求的类型
            "role_id__I",   --请求的角色ID
        })
    end
    return Net.CSRoleDetailsReq
end

-- 角色详细响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_DETAILS_RESP] = function()
    Net.SCRoleDetailsResp = Net.SCRoleDetailsResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRoleDetailsResp:__init()
        self:AutoConstruct({
            "type__C",      --请求的类型
            "role_id__I",       --角色ID
            "level__C",     --角色等级
            "vip_level__C",     --vip等级
            "head_icon__H",     --角色头像
            "battle_value__I",  --战力
            "role_name__s",     --角色名称
            "guild_name__s",    --工会名称
            "is_friend__C",     --是否好友
            "is_black__C",      --是否黑名单
            "formation_list__T__slot@C##hero_info_id@H##awaken_star@C##break_level@H##quality@C##equipment_list@D|EquipmentList|##treasure_list@D|TreasureList|", -- 阵型列表
        })
    end
    return Net.SCRoleDetailsResp
end



-- 角色详细响应: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ROLE_PRO_RECOVER_REQ] = function()
    Net.CSRoleProRecoverReq = Net.CSRoleProRecoverReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRoleProRecoverReq:__init()
        self:AutoConstruct({
            "type__C",  --属性类型     
        })
    end
    return Net.CSRoleProRecoverReq
end

-- 角色属性恢复请求: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ROLE_PRO_RECOVER_RESP] = function()
    Net.CSHeadImageReplaceReq = Net.CSHeadImageReplaceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeadImageReplaceReq:__init()
        self:AutoConstruct({
            "type__C",      --属性类型
            "recover_time__I",  --上次恢复时间
        })
    end
    return Net.CSHeadImageReplaceReq
end


-- 消息说明: 角色头像替换请求 消息Id: 20115
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HEAD_IMAGE_REPLACE_REQ] = function()
    Net.CSHeadImageReplaceReq = Net.CSHeadImageReplaceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeadImageReplaceReq:__init()
        self:AutoConstruct({
            "head_image_id__I", --头像ID
        })
    end
    return Net.CSHeadImageReplaceReq
end

-- 消息说明: 头像替换响应 消息Id: 20116
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HEAD_IMAGE_REPLACE_RESP] = function()
    Net.SCHeadImageReplaceResp = Net.SCHeadImageReplaceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeadImageReplaceResp:__init()
        self:AutoConstruct({
            "head_image_id__I", --头像ID
        })
    end
    return Net.SCHeadImageReplaceResp
end

-- 消息说明: 系统公告 消息Id: 20119
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SYSTEM_NOTICE] = function()
    Net.SCSystemNotice = Net.SCSystemNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSystemNotice:__init()
        self:AutoConstruct({
            "server_id__H",     --公告的服务器ID(客户端无视它)
            "notice_id__H",     --公告ID(0=表示text有效)
            "text__s",      --公告内容
            "param_list__T__param@s", -- 参数列表
        }) 
    end
    return Net.SCSystemNotice
end

--引导更新请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GUIDE_UPDATE_REQ] = function()
    Net.CSGuideUpdateReq = Net.CSGuideUpdateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGuideUpdateReq:__init()
        self:AutoConstruct({
            "key__I",       
            "value__I",   
        })
    end
    return Net.CSGuideUpdateReq
end

--引导更新响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GUIDE_UPDATE_RESP] = function()
    Net.SCGuideUpdateResp = Net.SCGuideUpdateResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGuideUpdateResp:__init()
        self:AutoConstruct({
            "key__I",
            "value__I",
        })
    end
    return Net.SCGuideUpdateResp
end

--场景进入请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCENE_ENTER_REQ] = function()
    Net.CSSceneEnterReq = Net.CSSceneEnterReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSceneEnterReq:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "context__I", --上下文值
        })
    end
    return Net.CSSceneEnterReq
end

--场景进入响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ENTER_RESP] = function()
    Net.SCSceneEnterResp = Net.SCSceneEnterResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneEnterResp:__init()
        self:AutoConstruct({
            "type__H", --场景类型(与副本类型一样)
            "context__I", --上下文值
            "map_x__H", --地图坐标x
            "map_y__H", --地图坐标y
            "player_list__T__role_id@I##head_img_id@H##role_name@s##sprite_id@I##map_x@H##map_y@H##title_id@I##nickname_id@I##server_id@H", -- 玩家列表            
        })
    end
    return Net.SCSceneEnterResp
end

--场景进入通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ENTER_NOTICE] = function()
    Net.SCSceneEnterNoticeResp = Net.SCSceneEnterNoticeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneEnterNoticeResp:__init()
        self:AutoConstruct({
            "type__H", --场景类型
            "role_id__I", --角色ID
            "head_img_id__H", --头像ID
            "role_name__s", --角色名称
            "sprite_id__I", --精灵ID
            "map_x__H", --地图坐标x
            "map_y__H", --地图坐标y
            "title_id__I", --称号Id
            "nickname_id__I", --昵称Id
            "server_id__H", --服务器ID
        })
    end
    return Net.SCSceneEnterNoticeResp
end

--场景离开请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCENE_LEAVE_REQ] = function()
    Net.CSSceneLeaveReq = Net.CSSceneLeaveReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSceneLeaveReq:__init()
        self:AutoConstruct({
            "type__H", --场景类型
        })
    end
    return Net.CSSceneLeaveReq
end

--场景离开响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_LEAVE_RESP] = function()
    Net.SCSceneLeaveResp = Net.SCSceneLeaveResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneLeaveResp:__init()
        self:AutoConstruct({
            "type__H", --场景类型
        })
    end
    return Net.SCSceneLeaveResp
end

--场景离开通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_LEAVE_NOTICE] = function()
    Net.SCSceneLeaveNoticeResp = Net.SCSceneLeaveNoticeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneLeaveNoticeResp:__init()
        self:AutoConstruct({
            "type__H", --场景类型
            "role_id__I", --角色ID            
        })
    end
    return Net.SCSceneLeaveNoticeResp
end

--英雄行走请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_GOTO_REQ] = function()
    Net.CSHeroGotoReq = Net.CSHeroGotoReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroGotoReq:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.CSHeroGotoReq
end

--英雄行走响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_GOTO_RESP] = function()
    Net.SCHeroGotoResp = Net.SCHeroGotoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroGotoResp:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.SCHeroGotoResp
end

--英雄行走通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_GOTO_NOTICE] = function()
    Net.SCHeroGotoNoticeResp = Net.SCHeroGotoNoticeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroGotoNoticeResp:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "role_id__I", --角色ID
            "src_x__H", --源点x
            "src_y__H", --源点y
            "dest_x__H", --终点标x
            "dest_y__H", --终点标y
        })
    end
    return Net.SCHeroGotoNoticeResp
end

-- 消息说明: 怪物列表通知 消息Id: 20129
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_MONSTER_LIST_NOTICE] = function()
    Net.SCSceneMonsterListNotice = Net.SCSceneMonsterListNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneMonsterListNotice:__init()
        self:AutoConstruct({
            "monster_list__T__uid@I##map_id@I##damage@I##total_hp@I##start_battle_time@I##map_x@H##map_y@H", -- 怪物列表            
        })
    end
    return Net.SCSceneMonsterListNotice
end

-- 消息说明: 怪物数据通知(主要是伤害有变化) 消息Id: 20130
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_MONSTER_DATA_NOTICE] = function()
    Net.SCSceneMonsterDataNotice = Net.SCSceneMonsterDataNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneMonsterDataNotice:__init()
        self:AutoConstruct({
            "uid__I", --怪物UID
            "map_id__I", --关卡地图ID
            "damage__I", --伤害
            "total_hp__I", --总HP
            "start_battle_time__I", --开始战斗时间
        })
    end
    return Net.SCSceneMonsterDataNotice
end

-- 消息说明: 场景物品列表通知 消息Id: 20131
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_LIST_NOTICE] = function()
    Net.SCSceneItemListNotice = Net.SCSceneItemListNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemListNotice:__init()
        self:AutoConstruct({
            "item_list__T__uid@I##info_id@I##map_x@H##map_y@H", -- 场景物品列表            
        })
    end
    return Net.SCSceneItemListNotice
end

-- 消息说明: 场景物品消失通知 消息Id: 20132
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_DISAPPEAR_NOTICE] = function()
    Net.SCSceneItemDisappearNotice = Net.SCSceneItemDisappearNotice or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemDisappearNotice:__init()
        self:AutoConstruct({
            "uid__I", --物品uid            
        })
    end
    return Net.SCSceneItemDisappearNotice
end

-- 消息说明: 场景物品锁住请求 消息Id: 20133
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCENE_ITEM_LOCK_REQ] = function()
    Net.CSSceneItemLockReq = Net.CSSceneItemLockReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSceneItemLockReq:__init()
        self:AutoConstruct({            
            "type__H", --类型(与副本类型一样)
            "uid__I", --物品uid            
        })
    end
    return Net.CSSceneItemLockReq
end

-- 消息说明: 场景物品锁住响应 消息Id: 20134
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_LOCK_RESP] = function()
    Net.SCSceneItemLockResp = Net.SCSceneItemLockResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemLockResp:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "uid__I", --场景物品uid
        })
    end
    return Net.SCSceneItemLockResp
end

-- 消息说明: 场景物品锁住通知 消息Id: 20137
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_LOCK_NOTIFY] = function()
    Net.SCSceneItemLockNotify = Net.SCSceneItemLockNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemLockNotify:__init()
        self:AutoConstruct({
            "type__H", --场景类型
            "uid__I", --物品uid
            "role_id__I", --锁住物品的角色id            
        })
    end
    return Net.SCSceneItemLockNotify
end

-- 消息说明: 场景物品拾取请求 消息Id: 20135
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SCENE_ITEM_PICK_UP_REQ] = function()
    Net.CSSceneItemPickUpReq = Net.CSSceneItemPickUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSceneItemPickUpReq:__init()
        self:AutoConstruct({
            "type__H", --类型(与副本类型一样)
            "uid__I", --场景物品uid
        })
    end
    return Net.CSSceneItemPickUpReq
end

-- 消息说明: 场景物品拾取响应 消息Id: 20136
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_PICK_UP_RESP] = function()
    Net.SCSceneItemPickUpResp = Net.SCSceneItemPickUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemPickUpResp:__init()
        self:AutoConstruct({
            "uid__I", --物品uid
            "info__I", --物品信息ID(物品ID)
            "item_list__T__id@I##num@I", -- 拾取后获得的物品列表            
        })
    end
    return Net.SCSceneItemPickUpResp
end

-- 消息说明: 场景物品拾取通知 消息Id: 20138
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SCENE_ITEM_PICK_UP_NOTIFY] = function()
    Net.SCSceneItemPickUpNotify = Net.SCSceneItemPickUpNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSceneItemPickUpNotify:__init()
        self:AutoConstruct({
            "uid__I", --物品uid
            "info__I", --物品信息ID(物品ID)
            "role_id__I", --拾取物品的角色id
        })
    end
    return Net.SCSceneItemPickUpNotify
end

-- 消息说明: 新手引导更新 消息Id: 20139
Net.ProtocolRegistry[Net.ProtocolNo.C2S_STORY_TAG_UPDATE_REQ] = function()
    Net.CSStoryTagUpdateReq = Net.CSStoryTagUpdateReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSStoryTagUpdateReq:__init()
        self:AutoConstruct({
            "key__I",   --引导X
            "value__I", --引导Y
        })
    end
    return Net.CSStoryTagUpdateReq
end