
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--请求主线副本数据: 
Net.ProtocolRegistry[Net.ProtocolNo.MSG_C2S_FB_MAIN_DATA_REQ] = function()
    Net.CSFBMainData = Net.CSFBMainData or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBMainData:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFBMainData
end

--主线副本数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.MSG_S2C_FB_MAIN_DATA_RESP] = function()
    Net.SCFBMainData = Net.SCFBMainData or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBMainData:__init()
        self:AutoConstruct({
			"take_chapter_reward_record__H",	--领取章节奖励记录(奖励等级)
            "chapter_list__T__gamelevel_list@D|FBGameLevelList|##star_award_list@D|FBStarAwardList|", -- 章节列表
        })
    end
    return Net.SCFBMainData
end

--请求英雄副本数据: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_HERO_DATA_REQ] = function()
    Net.CSFBHeroData = Net.CSFBHeroData or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBHeroData:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFBHeroData
end

--英雄副本数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_HERO_DATA_RESP] = function()
    Net.SCFBHeroData = Net.SCFBHeroData or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBHeroData:__init()
        self:AutoConstruct({
            "battle_times__C",  --今日已挑战次数
            "chapter_list__T__is_extra_map_pass@C##gamelevel_list@D|HeroFBGameLevelList|", -- 章节列表
        })
    end
    return Net.SCFBHeroData
end

--请求日常副本数据: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_DAILY_DATA_REQ] = function()
    Net.CSFBDailyData = Net.CSFBDailyData or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBDailyData:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFBDailyData
end

--日常副本数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_DAILY_DATA_RESP] = function()
    Net.SCFBDailyData = Net.SCFBDailyData or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBDailyData:__init()
        self:AutoConstruct({
            "chapter_list__T__chapter@C##map_idx_record@C##battle_times@C", -- 已打过的章节列表
        })
    end
    return Net.SCFBDailyData
end

--副本奖励领取请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_AWARD_GET_REQ] = function()
    Net.CSFBAwardGetReq = Net.CSFBAwardGetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBAwardGetReq:__init()
        self:AutoConstruct({
            "award_type__C",    --奖励类型
            "fb_type__C",   --副本类型
            "chapter_index__H", --章节
            "index__C",         --上下文索引
        })
    end
    return Net.CSFBAwardGetReq
end

--副本奖励领取响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_AWARD_GET_RESP] = function()
    Net.SCFBAwardGetResp = Net.SCFBAwardGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBAwardGetResp:__init()
        self:AutoConstruct({
            "award_type__C",    --奖励类型
            "fb_type__C",   --副本类型
            "chapter_index__H", --章节
            "index__C",         --上下文索引
        })
    end
    return Net.SCFBAwardGetResp
end

--副本扫荡请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_CLEAN_REQ] = function()
    Net.CSFBCleanReq = Net.CSFBCleanReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBCleanReq:__init()
        self:AutoConstruct({
            "fb_type__C",       --副本类型
            "param1__I",        --参数1
            "param2__I",        --参数2        
        })
    end
    return Net.CSFBCleanReq
end

--副本扫荡响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_CLEAN_RESP] = function()
    Net.SCFBCleanResp = Net.SCFBCleanResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBCleanResp:__init()
        self:AutoConstruct({
            "fb_type__C",       --副本类型
            "param1__I",        --参数1
            "param2__I",        --参数2
            "result__C",        --结果(0=正常, 1=遇到叛军, 2=英雄背包已满, 3=装备背包已满)
            "drop_item_list__T__item_id@I##item_num@I", -- 掉落物品列表
        })
    end
    return Net.SCFBCleanResp
end


-- 消息说明: 副本重置请求 消息Id: 21013
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_RESET_REQ] = function()
    Net.CSFbResetReq = Net.CSFbResetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFbResetReq:__init()
        self:AutoConstruct({
            "map_type__C",      --关卡类型
            "param1__I",        --参数1
            "param2__I",        --参数2        
        })
    end
    return Net.CSFbResetReq
end

-- 消息说明: 副本重置响应 消息Id: 21014
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_RESET_RESP] = function()
    Net.SCFbResetResp = Net.SCFbResetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFbResetResp:__init()
        self:AutoConstruct({
            "map_type__C",      --关卡类型
            "param1__I",        --参数1
            "param2__I",        --参数2        
        })
    end
    return Net.SCFbResetResp
end

-- 消息说明: 一键领取奖励请求(关卡+星数) 21015
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_ONE_KEY_TAKE_REWARD_REQ] = function()
    Net.CSFbOneKeyTakeRewardReq = Net.CSFbOneKeyTakeRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFbOneKeyTakeRewardReq:__init()
        self:AutoConstruct({
           "fb_type__C",       --副本类型
           "chapter__H",        --章节     
        })
    end
    return Net.CSFbOneKeyTakeRewardReq
end

-- 消息说明: 一键领取奖励响应(关卡+星数) 21016
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_ONE_KEY_TAKE_REWARD_RESP] = function()
    Net.SCFbOneKeyTakeRewardResp = Net.SCFbOneKeyTakeRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFbOneKeyTakeRewardResp:__init()
        self:AutoConstruct({
            "fb_type__C",       --副本类型
            "chapter__H",       --章节     
        })
    end
    return Net.SCFbOneKeyTakeRewardResp
end

-- 消息说明: 领取主线副本章节奖励请求 21017
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ] = function()
    Net.CSTakeMainFbChapterRewardReq = Net.CSTakeMainFbChapterRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeMainFbChapterRewardReq:__init()
        self:AutoConstruct({
           "reward_level__H",       --奖励等级   
        })
    end
    return Net.CSTakeMainFbChapterRewardReq
end

-- 消息说明: 领取主线副本章节奖励响应 21018
Net.ProtocolRegistry[Net.ProtocolNo.S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP] = function()
    Net.SCTakeMainFbChapterRewardResp = Net.SCTakeMainFbChapterRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeMainFbChapterRewardResp:__init()
        self:AutoConstruct({
            "reward_level__H",      --奖励等级
            "item_list__T__item_id@I##item_num@I", -- 物品列表(只会是英雄,装备,圣器,杨经理已经承若!!!!)      
        })
    end
    return Net.SCTakeMainFbChapterRewardResp
end

--精英副本数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_FB_ELITE_DATA_REQ] = function()
    Net.CSFBEliteDataReq = Net.CSFBEliteDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSFBEliteDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSFBEliteDataReq
end

--精英副本数据响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_FB_ELITE_DATA_RESP] = function()
    Net.SCFBEliteDataResp = Net.SCFBEliteDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCFBEliteDataResp:__init()
        self:AutoConstruct({
            "chapter_list__T__gamelevel_list@D|EliteFBGameLevelList|##star_award_list@D|EliteFBStarAwardList|##enemy_map_id@H##is_enemy_dead@C", -- 章节列表
            -- MapList说明:
            -- "star__C",     --获得的星数
            -- "used_battle_times__C", --已使用的挑战次数
            -- "used_reset_times__C",  --已使用的重置次数
            -- "is_get_reward__C", --是否已领取奖励(1=是,0=否)

            -- StarRewardList说明:
            -- "is_get_reward__C", --是否已领取奖励(1=是,0=否)
        })
    end
    return Net.SCFBEliteDataResp
end