--[[
%% @module: 996BT适配器
%% @author: swordman sue
%% @created: 2019-3-12
--]]

CustomizedLogicAdapter = CustomizedLogicAdapter or {}

CustomizedLogicAdapter.NNS_BT = CustomizedLogicAdapter.NNS_BT or BaseClass(CustomizedLogicAdapter.Base)

function CustomizedLogicAdapter.NNS_BT:__init()
end

function CustomizedLogicAdapter.NNS_BT:__delete()
end

--是否隐藏主界面VIP
function CustomizedLogicAdapter.NNS_BT:IsHideMaincityVIP()
	local config_gm_level = CPPGameLib.GetConfig("VipGM", 0)
    local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
    return total_recharge_amount >= config_gm_level.need_recharge_amount
end

--初始化主界面GM等级图标
function CustomizedLogicAdapter.NNS_BT:InitMaincityGMLevel(img_vip)
	if not self:IsHideMaincityVIP() then
		return
	end

    local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	local gm_level = ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)
	local img_gm_level = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_gm_lev"..gm_level))
	if img_gm_level then
		local parent_node = img_vip:GetParent()
		img_gm_level:SetAnchorPoint(img_vip:GetAnchorPoint())
		local posx, posy = img_vip:GetPosition()
		img_gm_level:SetPosition(posx, posy - 26)
		parent_node:AddChild(img_gm_level)
	end
	return img_gm_level
end

--更新主界面GM等级图标
function CustomizedLogicAdapter.NNS_BT:UpdateMaincityGMLevel(img_gm_level)
	if not img_gm_level then
		return
	end

	local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	local gm_level = ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)	
	img_gm_level:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_gm_lev"..gm_level))
end

--检测是否开启世界聊天
function CustomizedLogicAdapter.NNS_BT:CheckWorldChatEnabled()
	local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	local gm_level = ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)
	return gm_level >= 1	
end

--检测能否创建军团
function CustomizedLogicAdapter.NNS_BT:CheckCreateGuildEnabled()
	local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	if total_recharge_amount < 50 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("union_create_failed_tips", 50))
		return 
	end
	return true
end

--检测是否开启GM商店
function CustomizedLogicAdapter.NNS_BT:CheckGMShopEnabled()
	return true
end

--初始化GM商店选项卡
function CustomizedLogicAdapter.NNS_BT:CreateGMShopTabDataList()
	local tab_str_list = {	
		{name = CPPGameLib.GetString("shop_tools")}
	}

	--插入GM1~GM6分页
	for i = 1, 6 do
		table.insert(tab_str_list, {name = CPPGameLib.GetString("shop_gm")..i})
	end
	
	return tab_str_list
end

--获取英雄作为升级材料的经验
function CustomizedLogicAdapter.NNS_BT:GetHeroExp(hero_info)
	if not hero_info then
		return 0
	end
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	if not config_hero then
		return 0
	end

	if config_hero.type ~= Macros.Game.HeroType.EXP then
		return 1
	end
end

--获取圣器作为升级材料的经验
function CustomizedLogicAdapter.NNS_BT:GetTreasureExp(treasure_info)
	if not treasure_info then
		return 0
	end
	local config_treasure = CPPGameLib.GetConfig("Treasure", treasure_info.info_id)
	if not config_treasure then
		return 0
	end

	--经验英雄获得经验为1
	if config_treasure.type == Macros.Game.TreasureType.EXP then
		return 1
	end
end

--检测卡牌回收是否开启
function CustomizedLogicAdapter.NNS_BT:CheckCardRecycleEnabled()
	return false
end

--获取卡牌强化材料类型
function CustomizedLogicAdapter.NNS_BT:GetMaterialTypeOfCardStrengthen()
	return Macros.Game.CARD_QUALITY_TYPE.EXP
end


--检测VIP尊享礼包是否开启
function CustomizedLogicAdapter.NNS_BT:CheckPrivilegedGiftOfVIP()
	return false
end


--获取英雄回收tips
function CustomizedLogicAdapter.NNS_BT:GetHeroRecycleTips()
	return CPPGameLib.GetString("recycle_heroTipsEX")
end

--获取装备回收tips
function CustomizedLogicAdapter.NNS_BT:GetEquipRecycleTips()
	return CPPGameLib.GetString("recycle_equipTipsEX")
end

--检测月卡的充值档位是否开启
function CustomizedLogicAdapter.NNS_BT:CheckChargeMonthCardEnabled()
	return false
end

--检测GM等级
function CustomizedLogicAdapter.NNS_BT:CheckGMLevel(need_gm_level)
	if not need_gm_level then
		return true
	end
	local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	local gm_level = ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)	
	return gm_level >= need_gm_level	
end

--是否对商城的商品进行排序
function CustomizedLogicAdapter.NNS_BT:SortGoodsOfStore(good_list)
	if not good_list then
		return
	end
	table.sort(good_list, SortTools.KeyLowerSorter("cost_value"))
end

--获取每日签到双倍tips
function CustomizedLogicAdapter.NNS_BT:GetDailySignDoubleRewardTipsKey()
	return "activity_VipTips_996"
end

--检测每日签到双倍
function CustomizedLogicAdapter.NNS_BT:CheckDailySignDoubleReward(need_level)
	local total_recharge_amount = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT)
	local gm_level = ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)	
	return need_level and gm_level >= need_level
end

--是否隐藏高级召唤鼓舞提示
function CustomizedLogicAdapter.NNS_BT:IsHideRecruitEncourageTips()
	return true
end

--获取关卡评论开启等级
function CustomizedLogicAdapter.NNS_BT:GetLevelLimitOfMapComment()
	return 20
end

--获取聊天玩家VIP/GM等级 
function CustomizedLogicAdapter.NNS_BT:GetChatVIPOrGMLevOfPlayer(data)
	return data and data.gm_level or 0
end
