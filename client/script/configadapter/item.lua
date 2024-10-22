
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Item = ConfigAdapter.Item or {}

--解析礼包
local function parse_gift_pack(gift_pack_id, reward_map)
	local config_item = CPPGameLib.GetConfig("Item", gift_pack_id)
	if not config_item then
		return
	end
	if config_item.type ~= Macros.Game.ItemBigType.GIFT then
		return
	end

	for index, id in ipairs(config_item.item_id_list) do
		local config_sub_item = CPPGameLib.GetConfig("Item", id, false)
		if config_sub_item and config_sub_item.type == Macros.Game.ItemBigType.GIFT and config_sub_item.is_use_in_get == Macros.Global.TRUE then
			--礼包中的礼包
			parse_gift_pack(id, reward_map)
		else
			--礼包中的物品
			reward_map[id] = reward_map[id] or 0
			reward_map[id] = reward_map[id] + config_item.item_max_num_list[index]
		end
	end
end

--获取礼包奖励的物品列表
function ConfigAdapter.Item.GetGiftPackRewardList(gift_pack_id)
	local reward_map = {}

	parse_gift_pack(gift_pack_id, reward_map)

	local reward_list = {}
	for id, num in pairs(reward_map) do
		local obj_info = {}
		obj_info.id = id
		obj_info.num = num
		table.insert(reward_list, obj_info)
	end
	return reward_list
end

--获取礼包奖励的物品列表
function ConfigAdapter.Item.GetGiftPackRewardListII(obj_id_list, obj_num_list)
	local reward_list = {}

	if obj_id_list and obj_num_list then
		for i, obj_id in ipairs(obj_id_list) do
			local obj_info = {}
			obj_info.id = obj_id
			obj_info.num = obj_num_list[i] or 0
			table.insert(reward_list, obj_info)
		end
	end

	return reward_list
end

--获取物品对应的货币信息(返回：货币类型、货币值)
function ConfigAdapter.Item.GetCurrencyInfo(item_id, show_error)
	local currencty_type, currencty_value

	local config_item = CPPGameLib.GetConfig("Item", item_id)
	if config_item and config_item.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
		currencty_type = config_item.param1
		currencty_value = config_item.param2
	else
		if show_error then
			LogTool.LogError("[ConfigAdapter.Item.GetCurrencyInfo] 无效的货币类物品(item_id:%d)", item_id)
		end
	end

	return currencty_type, currencty_value
end
