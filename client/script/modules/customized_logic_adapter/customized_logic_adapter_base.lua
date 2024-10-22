--[[
%% @module: 996BT适配器
%% @author: swordman sue
%% @created: 2019-3-12
--]]

CustomizedLogicAdapter = CustomizedLogicAdapter or {}

CustomizedLogicAdapter.Base = CustomizedLogicAdapter.Base or BaseClass()

function CustomizedLogicAdapter.Base:__init()
	self.__cp_channel_type = 0
end

function CustomizedLogicAdapter.Base:__delete()
end

function CustomizedLogicAdapter.Base:SetCPChannelType(var)
	self.__cp_channel_type = var
end

function CustomizedLogicAdapter.Base:GetCPChannelType()
	return self.__cp_channel_type
end

--是否隐藏主界面VIP
function CustomizedLogicAdapter.Base:IsHideMaincityVIP()
	return false
end

--检测是否开启世界聊天
function CustomizedLogicAdapter.Base:CheckWorldChatEnabled()
	return true
end

--检测能否创建军团
function CustomizedLogicAdapter.Base:CheckCreateGuildEnabled()
	return true
end

--检测是否开启GM商店
function CustomizedLogicAdapter.Base:CheckGMShopEnabled()
end

--获取英雄作为升级材料的经验
function CustomizedLogicAdapter.Base:GetHeroExp(heor_info)
end

--获取圣器作为升级材料的经验
function CustomizedLogicAdapter.Base:GetTreasureExp(treasure_info)
end

--检测卡牌回收是否开启
function CustomizedLogicAdapter.Base:CheckCardRecycleEnabled()
	return true
end

--获取卡牌强化材料类型
function CustomizedLogicAdapter.Base:GetMaterialTypeOfCardStrengthen()
end

--检测VIP尊享礼包是否开启
function CustomizedLogicAdapter.Base:CheckPrivilegedGiftOfVIP()
	return true
end

--获取英雄回收tips
function CustomizedLogicAdapter.Base:GetHeroRecycleTips()
end

--获取装备回收tips
function CustomizedLogicAdapter.Base:GetEquipRecycleTips()
end

--检测月卡的充值档位是否开启
function CustomizedLogicAdapter.Base:CheckChargeMonthCardEnabled()
	return true
end

--检测GM等级
function CustomizedLogicAdapter.Base:CheckGMLevel()
	return true
end

--是否对商城的商品进行排序
function CustomizedLogicAdapter.Base:SortGoodsOfStore()
end

--获取每日签到双倍tips
function CustomizedLogicAdapter.Base:GetDailySignDoubleRewardTipsKey()
end

--检测每日签到双倍
function CustomizedLogicAdapter.Base:CheckDailySignDoubleReward(need_level)
	return need_level and CallModuleFunc(ModuleType.VIP , "GetVipLV") >= need_level
end

--是否隐藏高级召唤鼓舞提示
function CustomizedLogicAdapter.Base:IsHideRecruitEncourageTips()
end

--获取关卡评论开启等级
function CustomizedLogicAdapter.Base:GetLevelLimitOfMapComment()
end

--获取聊天玩家VIP/GM等级 
function CustomizedLogicAdapter.Base:GetChatVIPOrGMLevOfPlayer(data)
	return data and data.vip_level or 0
end
