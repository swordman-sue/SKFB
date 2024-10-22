
--[[
%% @module: 协议自定义结构
%% @author: swordman sue
%% @created: 2016-07-26
--]]

Net = Net or {}

Net.CSStruct = Net.CSStruct or {}

--[[
-- 毫秒时间戳
Net.CSStruct.MsecTimestamp = Net.CSStruct.MsecTimestamp or BaseClass()

-- 重载操作符
Net.CSStruct.MsecTimestamp.METATABLE_FUNC_EQ = function(a, b)
    return (a.second == b.second) and (a.millisecond == b.millisecond)
end
Net.CSStruct.MsecTimestamp.METATABLE_FUNC_LT = function(a, b)
    return a.second < b.second or (a.second == b.second and a.millisecond < b.millisecond)
end

function Net.CSStruct.MsecTimestamp:__init(second, millisecond)
    self.second = second or 0
    self.millisecond = millisecond or 0

    -- 原有metatable
    local orig_metatable = getmetatable(self) or {}
    orig_metatable.__eq = Net.CSStruct.MsecTimestamp.METATABLE_FUNC_EQ
    orig_metatable.__lt = Net.CSStruct.MsecTimestamp.METATABLE_FUNC_LT

    setmetatable(self, orig_metatable)
end

function Net.CSStruct.MsecTimestamp:Encode(msg_adapter)
    return msg_adapter.WriteFMT("IH", self.second, self.millisecond)
end

function Net.CSStruct.MsecTimestamp:Decode(msg_adapter)
    self.second,
    self.millisecond = msg_adapter.ReadFMT("IH")

    return msg_adapter.ReadResult()
end
--]]

--副本关卡列表
Net.CSStruct.FBGameLevelList = Net.CSStruct.FBGameLevelList or BaseClass()

function Net.CSStruct.FBGameLevelList:__init()
    self.__list = {}    
end

function Net.CSStruct.FBGameLevelList:Encode(msg_adapter)
end

function Net.CSStruct.FBGameLevelList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local gamelevel_info = {}
        gamelevel_info.star,
        gamelevel_info.battle_times,
        gamelevel_info.reset_times,
        gamelevel_info.box_award_flag = msg_adapter.ReadFMT("CCCC")
        table.insert(self.__list, gamelevel_info)
    end

    return msg_adapter.ReadResult()
end

function Net.CSStruct.FBGameLevelList:GetData()
    return self.__list
end

--英雄副本关卡列表
Net.CSStruct.HeroFBGameLevelList = Net.CSStruct.HeroFBGameLevelList or BaseClass()

function Net.CSStruct.HeroFBGameLevelList:__init()
    self.__list = {}
end

function Net.CSStruct.HeroFBGameLevelList:Encode(msg_adapter)
end

function Net.CSStruct.HeroFBGameLevelList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local gamelevel_info = {}
        gamelevel_info.battle_times,
        gamelevel_info.box_award_flag = msg_adapter.ReadFMT("CC")
        table.insert(self.__list, gamelevel_info)
    end

    return msg_adapter.ReadResult()
end

function Net.CSStruct.HeroFBGameLevelList:GetData()
    return self.__list
end

--副本星级奖励列表
Net.CSStruct.FBStarAwardList = Net.CSStruct.FBStarAwardList or BaseClass()

function Net.CSStruct.FBStarAwardList:__init()
    self.__list = {}
end

function Net.CSStruct.FBStarAwardList:Encode(msg_adapter)
end

function Net.CSStruct.FBStarAwardList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local status = msg_adapter.ReadUChar()
        table.insert(self.__list, status)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.FBStarAwardList:GetData()
    return self.__list
end

--邮件上下文列表
Net.CSStruct.MailContextList = Net.CSStruct.MailContextList or BaseClass()

function Net.CSStruct.MailContextList:__init()
    self.__list = {}
end

function Net.CSStruct.MailContextList:Encode(msg_adapter)
end

function Net.CSStruct.MailContextList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.type = msg_adapter.ReadFMT("C")   --根据类型一个一个读 
        data.text = msg_adapter.ReadFMT("s")
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.MailContextList:GetData()
    return self.__list
end

--夺宝对手英雄列表
Net.CSStruct.HeroList = Net.CSStruct.HeroList or BaseClass()

function Net.CSStruct.HeroList:__init()
    self.__list = {}
end

function Net.CSStruct.HeroList:Encode(msg_adapter)
end

function Net.CSStruct.HeroList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local ID = msg_adapter.ReadFMT("H")   --根据类型一个一个读 
        table.insert(self.__list, ID)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.HeroList:GetData()
    return self.__list
end

--属性列表
Net.CSStruct.ProList = Net.CSStruct.ProList or BaseClass()

function Net.CSStruct.ProList:__init()
    self.__list = {}
end

function Net.CSStruct.ProList:Encode(msg_adapter)
end

function Net.CSStruct.ProList:Decode(msg_adapter)
    -- ProList说明:
    -- pro_type@H  // 属性类型
    -- pro_value@I // 属性值

    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.prop_type,
        data.prop_value = msg_adapter.ReadFMT("HI")
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.ProList:GetData()
    return self.__list
end

-- --掉落物列表
-- Net.CSStruct.DropItemList = Net.CSStruct.DropItemList or BaseClass()

-- function Net.CSStruct.DropItemList:__init()
--     self.__list = {}
-- end

-- function Net.CSStruct.DropItemList:Encode(msg_adapter)
-- end

-- function Net.CSStruct.DropItemList:Decode(msg_adapter)
--     local len = msg_adapter.ReadUShort()
--     for i = 1, len do
--         local data = {}
--         data.id,
--         data.num = msg_adapter.ReadFMT("IH")
--         table.insert(self.__list, data)
--     end

--     return msg_adapter.ReadResult()    
-- end

-- function Net.CSStruct.DropItemList:GetData()
--     return self.__list
-- end

--英雄详细信息装备列表
Net.CSStruct.EquipmentList = Net.CSStruct.EquipmentList or BaseClass()

function Net.CSStruct.EquipmentList:__init()
    self.__list = {}
end

function Net.CSStruct.EquipmentList:Encode(msg_adapter)
end

function Net.CSStruct.EquipmentList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.info_id,
        data.strengthen_level,
        data.refine_level = msg_adapter.ReadFMT("HHC")   --根据类型一个一个读 
        table.insert(self.__list,data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.EquipmentList:GetData()
    return self.__list
end

--英雄详细信息圣器列表
Net.CSStruct.TreasureList = Net.CSStruct.TreasureList or BaseClass()

function Net.CSStruct.TreasureList:__init()
    self.__list = {}
end

function Net.CSStruct.TreasureList:Encode(msg_adapter)
end

function Net.CSStruct.TreasureList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.info_id,
        data.strengthen_level,
        data.refine_level = msg_adapter.ReadFMT("HHC")   --根据类型一个一个读 
        table.insert(self.__list,data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.TreasureList:GetData()
    return self.__list
end

--领奖中心奖励物品列表
Net.CSStruct.ItemList = Net.CSStruct.ItemList or BaseClass()

function Net.CSStruct.ItemList:__init()
    self.__list = {}
end

function Net.CSStruct.ItemList:Encode(msg_adapter)
end

function Net.CSStruct.ItemList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.item_id,
        data.item_num = msg_adapter.ReadFMT("II")   --根据类型一个一个读 
        table.insert(self.__list,data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.ItemList:GetData()
    return self.__list
end

--领奖中心奖励货币列表
Net.CSStruct.CurrencyList = Net.CSStruct.CurrencyList or BaseClass()

function Net.CSStruct.CurrencyList:__init()
    self.__list = {}
end

function Net.CSStruct.CurrencyList:Encode(msg_adapter)
end

function Net.CSStruct.CurrencyList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.type,
        data.value = msg_adapter.ReadFMT("CI")   --根据类型一个一个读 
        table.insert(self.__list,data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.CurrencyList:GetData()
    return self.__list
end

--英雄培养属性列表
Net.CSStruct.HeroTrainProList = Net.CSStruct.HeroTrainProList or BaseClass()

function Net.CSStruct.HeroTrainProList:__init()
    self.__list = {}
end

function Net.CSStruct.HeroTrainProList:Encode(msg_adapter)
end

function Net.CSStruct.HeroTrainProList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.pro_type,
        data.pro_level,
        data.pro_value = msg_adapter.ReadFMT("CCI") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.HeroTrainProList:GetData()
    return self.__list
end


--我的技能列表
Net.CSStruct.SelfSkillList = Net.CSStruct.SelfSkillList or BaseClass()

function Net.CSStruct.SelfSkillList:__init()
    self.__list = {}
end

function Net.CSStruct.SelfSkillList:Encode(msg_adapter)
end

function Net.CSStruct.SelfSkillList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    print("xy" , len,"====================1")
    for i = 1, len do
        local data = {}
        data.skill_id,
        data.skill_level = msg_adapter.ReadFMT("HH") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.SelfSkillList:GetData()
    return self.__list
end


--公会技能列表
Net.CSStruct.GuildSkillList = Net.CSStruct.GuildSkillList or BaseClass()

function Net.CSStruct.GuildSkillList:__init()
    self.__list = {}
end

function Net.CSStruct.GuildSkillList:Encode(msg_adapter)
end

function Net.CSStruct.GuildSkillList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.skill_id,
        data.skill_level = msg_adapter.ReadFMT("HH") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.GuildSkillList:GetData()
    return self.__list
end

--精英副本关卡列表
Net.CSStruct.EliteFBGameLevelList = Net.CSStruct.EliteFBGameLevelList or BaseClass()

function Net.CSStruct.EliteFBGameLevelList:__init()
    self.__list = {}    
end

function Net.CSStruct.EliteFBGameLevelList:Encode(msg_adapter)
end

function Net.CSStruct.EliteFBGameLevelList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local gamelevel_info = {}
        gamelevel_info.star,
        gamelevel_info.battle_times,
        gamelevel_info.reset_times,
        gamelevel_info.box_award_flag = msg_adapter.ReadFMT("CCCC")
        table.insert(self.__list, gamelevel_info)
    end

    return msg_adapter.ReadResult()
end

function Net.CSStruct.EliteFBGameLevelList:GetData()
    return self.__list
end

--精英副本星级奖励列表
Net.CSStruct.EliteFBStarAwardList = Net.CSStruct.EliteFBStarAwardList or BaseClass()

function Net.CSStruct.EliteFBStarAwardList:__init()
    self.__list = {}
end

function Net.CSStruct.EliteFBStarAwardList:Encode(msg_adapter)
end

function Net.CSStruct.EliteFBStarAwardList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local status = msg_adapter.ReadFMT("C")
        table.insert(self.__list, status)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.EliteFBStarAwardList:GetData()
    return self.__list
end

--工会关卡怪物数据响应
Net.CSStruct.MonsterList = Net.CSStruct.MonsterList or BaseClass()

function Net.CSStruct.MonsterList:__init()
    self.__list = {}
end

function Net.CSStruct.MonsterList:Encode(msg_adapter)
end

function Net.CSStruct.MonsterList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.index,
        data.damage,
        data.total_hp = msg_adapter.ReadFMT("CII") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.MonsterList:GetData()
    return self.__list
end

--工会关卡我的关卡奖励记录响应
Net.CSStruct.CampList = Net.CSStruct.CampList or BaseClass()

function Net.CSStruct.CampList:__init()
    self.__list = {}
end

function Net.CSStruct.CampList:Encode(msg_adapter)
end

function Net.CSStruct.CampList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.camp,
        data.is_take_reward = msg_adapter.ReadFMT("CC") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.CampList:GetData()
    return self.__list
end

--关卡评论列表响应
Net.CSStruct.LikePlayerList = Net.CSStruct.LikePlayerList or BaseClass()

function Net.CSStruct.LikePlayerList:__init()
    self.__list = {}
end

function Net.CSStruct.LikePlayerList:Encode(msg_adapter)
end

function Net.CSStruct.LikePlayerList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.role_id = msg_adapter.ReadFMT("I") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.LikePlayerList:GetData()
    return self.__list
end

--英雄阵位宝石
Net.CSStruct.GemList = Net.CSStruct.GemList or BaseClass()

function Net.CSStruct.GemList:__init()
    self.__list = {}
end

function Net.CSStruct.GemList:Encode(msg_adapter)
end

function Net.CSStruct.GemList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.gem_id = msg_adapter.ReadFMT("I") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.GemList:GetData()
    return self.__list
end

--英雄阵位卡牌
Net.CSStruct.CardList = Net.CSStruct.CardList or BaseClass()

function Net.CSStruct.CardList:__init()
    self.__list = {}
end

function Net.CSStruct.CardList:Encode(msg_adapter)
end

function Net.CSStruct.CardList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.card_id = msg_adapter.ReadFMT("I") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.CardList:GetData()
    return self.__list
end

--赛季排行榜响应
Net.CSStruct.RankList = Net.CSStruct.RankList or BaseClass()

function Net.CSStruct.RankList:__init()
    self.__list = {}
end

function Net.CSStruct.RankList:Encode(msg_adapter)
end

function Net.CSStruct.RankList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.server_id,
        data.role_name,
        data.head_icon,
        data.level ,
        data.title_id = msg_adapter.ReadFMT("HsHHI") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RankList:GetData()
    return self.__list
end

--组队副本BOSS奖励通知
Net.CSStruct.RerwadList = Net.CSStruct.RerwadList or BaseClass()

function Net.CSStruct.RerwadList:__init()
    self.__list = {}
end

function Net.CSStruct.RerwadList:Encode(msg_adapter)
end

function Net.CSStruct.RerwadList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.Item_Id,
        data.Item_Num = msg_adapter.ReadFMT("II") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RerwadList:GetData()
    return self.__list
end

--组队荣耀排行榜响应
Net.CSStruct.RoleNameList = Net.CSStruct.RoleNameList or BaseClass()

function Net.CSStruct.RoleNameList:__init()
    self.__list = {}
end

function Net.CSStruct.RoleNameList:Encode(msg_adapter)
end

function Net.CSStruct.RoleNameList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.server_id,
        data.role_name = msg_adapter.ReadFMT("Hs") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RoleNameList:GetData()
    return self.__list
end

--英雄觉醒道具列表
Net.CSStruct.AwakenItemList = Net.CSStruct.AwakenItemList or BaseClass()

function Net.CSStruct.AwakenItemList:__init()
    self.__list = {}
end

function Net.CSStruct.AwakenItemList:Encode(msg_adapter)
end

function Net.CSStruct.AwakenItemList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local item_id = msg_adapter.ReadFMT("H")
        table.insert(self.__list, item_id)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.AwakenItemList:GetData()
    return self.__list
end

-- 消息说明: 收藏书列表响应 消息Id: 23125
-- GroupList说明:
--英雄觉醒道具列表
Net.CSStruct.GroupList = Net.CSStruct.GroupList or BaseClass()

function Net.CSStruct.GroupList:__init()
    self.__list = {}
end

function Net.CSStruct.GroupList:Encode(msg_adapter)
end

function Net.CSStruct.GroupList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.group_id,
        data.star = msg_adapter.ReadFMT("HC") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.GroupList:GetData()
    return self.__list
end





-- 消息说明: 在线奖励数据响应 消息Id: 25902
-- record_data:
-- "flag__C", --flag(0=表示还没领取,1=表示已领取)
--英雄觉醒道具列表
Net.CSStruct.RecordData = Net.CSStruct.RecordData or BaseClass()

function Net.CSStruct.RecordData:__init()
    self.__list = {}
end

function Net.CSStruct.RecordData:Encode(msg_adapter)
end

function Net.CSStruct.RecordData:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.flag = msg_adapter.ReadFMT("C") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RecordData:GetData()
    return self.__list
end

-- 消息说明: 阵营招募数据响应 消息Id: 26701
-- RecordList:
-- RecordList说明:
-- reward_idx@H: 奖励位置下标(从0开始)
--英雄觉醒道具列表
Net.CSStruct.RecordList = Net.CSStruct.RecordList or BaseClass()

function Net.CSStruct.RecordList:__init()
    self.__list = {}
end

function Net.CSStruct.RecordList:Encode(msg_adapter)
end

function Net.CSStruct.RecordList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.reward_idx = msg_adapter.ReadFMT("H") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RecordList:GetData()
    return self.__list
end


-- 消息说明: 工会战塔列表响应 消息Id: 27122
-- "role_id__I",           // 角色ID
-- "be_attack_times__H",   // 被攻击次数
-- "head_img_id__H",       // 头像ID
-- "power_I",              // 战力
-- "role_name_s",          // 角色名称
Net.CSStruct.DefenderList = Net.CSStruct.DefenderList or BaseClass()

function Net.CSStruct.DefenderList:__init()
    self.__list = {}
end

function Net.CSStruct.DefenderList:Encode(msg_adapter)
end

function Net.CSStruct.DefenderList:Decode(msg_adapter)
    local len = msg_adapter.ReadUShort()
    for i = 1, len do
        local data = {}
        data.role_id = msg_adapter.ReadFMT("I") 
        data.be_attack_times = msg_adapter.ReadFMT("H") 
        data.head_img_id = msg_adapter.ReadFMT("H") 
        data.power = msg_adapter.ReadFMT("I") 
        data.role_name = msg_adapter.ReadFMT("s") 
        table.insert(self.__list, data)
    end

    return msg_adapter.ReadResult()    
end

function Net.CSStruct.RecordList:GetData()
    return self.__list
end