
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Sundry = ConfigAdapter.Sundry or {}

--GM等级区间列表
local gm_level_section_list

--获取GM等级
function ConfigAdapter.Sundry.GetGMLevel(total_recharge_amount)
	local config_gm_level
	local config_gm_level_list = CPPGameLib.GetConfig("VipGM", level, nil, true)
	local max_index = table.maxn(config_gm_level_list)
	for i = max_index, 0, -1 do
		config_gm_level = config_gm_level_list[i]
		if config_gm_level.need_recharge_amount > 0 and config_gm_level.need_recharge_amount <= total_recharge_amount then
			return i + 1
		end
	end
	return 0
end

--GM商店商品表
local gm_shop_goods_map

--初始化GM商店商品表
local function InitGMShopGoodsMap()
	if not gm_shop_goods_map then
		gm_shop_goods_map = {}

		local sub_type 
		for good_id, config_good in ipairs(CPPGameLib.GetConfig("GMShop", nil, nil, true)) do
			sub_type = config_good.shop_sub_type
			gm_shop_goods_map[sub_type] = gm_shop_goods_map[sub_type] or {}
			table.insert(gm_shop_goods_map[sub_type], config_good)
		end
	end
end

--获取GM商店的商品列表
function ConfigAdapter.Sundry.GetGMShopGoodsList(sub_type)
	InitGMShopGoodsMap()

	return gm_shop_goods_map[sub_type]
end
