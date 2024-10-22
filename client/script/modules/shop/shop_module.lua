Modules = Modules or {}

Modules.ShopModule = Modules.ShopModule or BaseClass(BaseModule)

function Modules.ShopModule:__init()
	self.__buy_record_map = {}
	self.__goods_list_map = {}
end

function Modules.ShopModule:__delete()	
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.ShopModule:HandleNewDayNotify()
	self.__buy_record_map = {}
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--购买发送
function Modules.ShopModule:ShopBuyRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_REQ)  
	data.type =  info.type or 1
	data.param1 =  info.param1 or 0
	data.param2 =  info.param2 or 0
	data.param3 =  info.param3 or 0

	--发送请求消息
	GlobalNetAdapter:OnSend(data) 
end

--购买响应
function Modules.ShopModule:ShopBuyResponse( info )
	local cnf
    --普通商店、礼包商店、GM商店、竞技场商店
	if info.type == Macros.Game.SHOP.STOREPROP or info.type == Macros.Game.SHOP.STOREOEM or 
		info.type == Macros.Game.SHOP.GM_SHOP or info.type == Macros.Game.SHOP.ARENA then
		self:__HandleBuyResponse(info)
	
	elseif info.type == Macros.Game.SHOP.TOWER then
		CallModuleFunc(ModuleType.TOWER, "TowerBuyResponse" , info)

		cnf = CPPGameLib.GetConfig("TowerShop",info.param1,false)
		local t1 = {}
        table.insert(t1,{id = cnf.object_id , num = cnf.object_num * info.param2 })
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	elseif info.type == Macros.Game.SHOP.ACTIVITY_DISCOUNT then --活动打折
		CallModuleFunc(ModuleType.SEVENDAYS, "HandleBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.LEVEL_GIFTBAG then --等级礼包
		CallModuleFunc(ModuleType.ACTIVITY, "LevelShopBuyResp" , info)

	elseif info.type == Macros.Game.SHOP.REBEL then --叛军购买
		CallModuleFunc(ModuleType.REBEL , "RebelBuyResp" , info)

	--军团商店
	elseif info.type == Macros.Game.SHOP.UNIONPROP or 
		   info.type == Macros.Game.SHOP.UNIONTIMEPROP or 
		   info.type == Macros.Game.SHOP.UNIONAWARDPROP then
		   CallModuleFunc(ModuleType.UNION , "UnionBuyResp" , info)

	elseif info.type == Macros.Game.SHOP.SOUL or info.type == Macros.Game.SHOP.AWAKEN_SHOP then
		CallModuleFunc(ModuleType.HERO_SHOP , "HeroShopBuyResp" , info)

	elseif info.type == Macros.Game.SHOP.CHASM_SHOP or info.type == Macros.Game.SHOP.CHASM_AWARD then
		--无尽深渊
		CallModuleFunc(ModuleType.CHASM , "ChasmBuyResp" , info)

	elseif info.type == Macros.Game.SHOP.DIRECT_BUY or info.type == Macros.Game.SHOP.DIRECT_HOT_BUY then
		--直购特惠
		CallModuleFunc(ModuleType.DIRECT_BUY_SHOP , "DirectBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.CONCERT_BUY then
		--兑换特惠
		CallModuleFunc(ModuleType.DIRECT_BUY_SHOP , "ConvertShopBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.GEM_PROP --原石道具
		or info.type == Macros.Game.SHOP.GEM_REWARD  then 	--原石奖励
		--兑换特惠
		CallModuleFunc(ModuleType.TEAM , "TeamShopBuyResp" ,info.type,info)

	elseif info.type == Macros.Game.SHOP.DESTINY then --命运商店

		CallModuleFunc(ModuleType.DIVINATION , "DestinyBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.STAR_SOUL then --星魂商店
		CallModuleFunc(ModuleType.DIVINATION , "DestinyStarBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.HIDE then --隐藏商店
		CallModuleFunc(ModuleType.DIVINATION , "DestinyExchangeShopResp" , info)

	elseif info.type == Macros.Game.SHOP.CHRISTMAS_INTEGRAL --圣诞积分兑换
		or info.type == Macros.Game.SHOP.CHRISTMAS_PROP  then 	--圣诞道具兑换
		CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"ClearUpShopDataByShopType",info.type,info,true)

	elseif info.type == Macros.Game.SHOP.CRYSTAL then --水晶商店
		CallModuleFunc(ModuleType.CRYSTAL_SHOP,"CrystalShopBuyResp", info )

	elseif info.type == Macros.Game.SHOP.SHOP_SCORE_MATCH then
		--积分赛商店
		self:FireNextFrame(Macros.Event.IntegralModule.GLORY_SHOP_BUY_ITEM_RESP)
		local cnf = CPPGameLib.GetConfig("ScoreMatchShop",info.param1,false)
		local t1 = {}
        table.insert(t1,{id = cnf.item_id , num = ( cnf.item_num or 1 ) * info.param2 })
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    elseif info.type == Macros.Game.SHOP.RESOURCE_WAR_SHOP then --资源商店
    	CallModuleFunc( ModuleType.RESOURCE_WAR , "ResourceWarBuyResp" , info )
	end

	self:FireNextFrame(Macros.Event.ShopModule.BUY_SUCCESS, info)
end

--购买记录请求
function Modules.ShopModule:ShopBuyRecordRequest(type, force_req, sub_type)
	type =  type or Macros.Game.SHOP.STOREOEM

	if not force_req and self:__GetBuyRecordMapEx(type, sub_type) then
		--请求普通商店购买记录，但之前缓存的vip等级与实时vip等级不一致时，重新初始化商品列表
		if type == Macros.Game.SHOP.STOREPROP and
			self.__viplev_record_for_buy_limit ~= CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL) then
			self:__InitCommonGoodsList()
		end

		--派发购买记录下发事件
		self:FireNextFrame(Macros.Event.ShopModule.RECORD_LIST, type, sub_type)
		return
	end

	self.__sub_type_for_buy_record_req = sub_type
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ)
	data.type = type 
	GlobalNetAdapter:OnSend(data)
end

--购买记录响应
function Modules.ShopModule:ShopBuyRecordResponse( info )
	--道具
	if info.type == Macros.Game.SHOP.STOREPROP then 
		self:__SetBuyRecord(info.type, info.buy_record_list)
		self:__InitCommonGoodsList()

	--礼包
	elseif info.type == Macros.Game.SHOP.STOREOEM then 
		self:__SetBuyRecord(info.type, info.buy_record_list)
		self:__InitGiftGoodsList()
		self:FireNextFrame(Macros.Event.VIPModule.VIP_BUY, info.buy_record_list)

	--竞技场商店
	elseif info.type == Macros.Game.SHOP.ARENA then		
		self:__SetBuyRecord(info.type, info.buy_record_list)
		self:__InitArenaGoodsList()		

	--GM商店
	elseif info.type == Macros.Game.SHOP.GM_SHOP then
		self:__SetBuyRecord(info.type, info.buy_record_list, self.__sub_type_for_buy_record_req)
		self:__InitGMGoodsList(self.__sub_type_for_buy_record_req, ConfigAdapter.Sundry.GetGMShopGoodsList(self.__sub_type_for_buy_record_req))

	elseif info.type == Macros.Game.SHOP.TOWER then			--爬塔
		CallModuleFunc(ModuleType.TOWER, "TowerBuyNotesResponse" , info)
															
	elseif info.type == Macros.Game.SHOP.UNIONPROP or info.type == Macros.Game.SHOP.UNIONTIMEPROP or info.type == Macros.Game.SHOP.UNIONAWARDPROP then --公会
		CallModuleFunc(ModuleType.UNION, "HandleBuyResponse" , info)

	elseif info.type == Macros.Game.SHOP.ACTIVITY_DISCOUNT then 	--活动
		CallModuleFunc(ModuleType.SEVENDAYS, "ShopBuyRecordResponse" , info)

	elseif info.type == Macros.Game.SHOP.LEVEL_GIFTBAG then 	--等级礼包
		CallModuleFunc(ModuleType.ACTIVITY, "LevelShopBuyNotesResp" , info)

	elseif info.type == Macros.Game.SHOP.CHASM_SHOP or info.type == Macros.Game.SHOP.CHASM_AWARD then 	--无尽
		CallModuleFunc(ModuleType.CHASM, "ChasmBuyNotesResp" , info)

	elseif info.type == Macros.Game.SHOP.CONCERT_BUY then 	--兑换
		CallModuleFunc(ModuleType.DIRECT_BUY_SHOP, "DirectNotesResponse" , info)

	elseif info.type == Macros.Game.SHOP.GEM_PROP --原石道具
		or info.type == Macros.Game.SHOP.GEM_REWARD  then 	--原石奖励
		CallModuleFunc(ModuleType.TEAM, "HandleTeamShopBuyRecord" , info.type,info.buy_record_list)

	--圣诞积分兑换
	elseif info.type == Macros.Game.SHOP.CHRISTMAS_INTEGRAL or 
		--圣诞道具兑换
		info.type == Macros.Game.SHOP.CHRISTMAS_PROP  then 	

		CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"ClearUpShopDataByShopType",info.type,info.buy_record_list)
	
	elseif info.type == Macros.Game.SHOP.SHOP_SCORE_MATCH then
		--积分赛商店
		CallModuleFunc(ModuleType.INTEGRAL_RACE,"ClearUpShopData",info.buy_record_list)		

	elseif info.type == Macros.Game.SHOP.RESOURCE_WAR_SHOP then
		--资源战商店
		CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarBuyNotesResp",info.buy_record_list)		
	end

	--派发购买记录下发事件
	self:FireNextFrame(Macros.Event.ShopModule.RECORD_LIST, info.type, self.__sub_type_for_buy_record_req)
end

--出售
function Modules.ShopModule:ShopGoodsSellRep( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SHOP_GOODS_SELL_REQ)

	data.type = info.type
	data.goods_list = info.goods_list
	GlobalNetAdapter:OnSend(data)
end

--出售响应
function Modules.ShopModule:ShopGoodsSellResp( info )
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pack_congratsGet") .. info.currency_list[1].value .. CPPGameLib.GetString("currency_get_"..info.currency_list[1].type) )
end

-- 消息说明: 限时商品列表请求 消息Id: 20907
function Modules.ShopModule:ShopLimitGoodsListReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SHOP_LIMIT_GOODS_LIST_REQ)  
	data.type =  info.type
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 随时商品列表响应 消息Id: 20908
function Modules.ShopModule:ShopLimitGoodsListResp(info)
	CallModuleFunc(ModuleType.UNION,"ShopLimitGoodsListResp",info)
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--解析商品配置
function Modules.ShopModule:ParseGood(type, config_good)
	local good_data = CPPGameLib.CopyTblShallow(config_good)

	good_data.shop_type = type

	--限购数量
	local limit_num_by_vip = -1
	local limit_num_by_max = -1
	if good_data.vip_idx and good_data.vip_idx ~= 0 then
		local viplev = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)
		local config_vip = CPPGameLib.GetConfig("Vip", viplev)
		limit_num_by_vip = config_vip["value"..good_data.vip_idx] or -1
	end
	if good_data.buy_num_max ~= 0 then		
		limit_num_by_max = good_data.buy_num_max
	end
	good_data.canBuyNum = math.max(limit_num_by_vip, limit_num_by_max)
	good_data.canBuyNum = math.max(0, good_data.canBuyNum)

	--已购数量
	local buy_record_map = self.__buy_record_map[type]
	if buy_record_map then
		good_data.haveBuyNum = buy_record_map[good_data.goods_id] or 0						
	else			
		good_data.haveBuyNum = 0
	end

	--消耗类型、消耗数量
	good_data.cost_type = good_data.cost_type or (good_data.cost_type_list and good_data.cost_type_list[1]) or Macros.Game.RolePropType.GOLD
	good_data.cost_value = good_data.cost_value or (good_data.cost_value_list and good_data.cost_value_list[1]) or Macros.Game.RolePropType.GOLD
	if type == Macros.Game.SHOP.STOREPROP then
		good_data.cost_value = good_data.price_list[good_data.haveBuyNum + 1] or good_data.price_list[#good_data.price_list]
	end

	return good_data
end

--获取商品数据
function Modules.ShopModule:GetGoodData(type, sub_type, good_id)
	local goods_list = self:GetGoodsListByType(type, sub_type)
	if goods_list then
		for _, good_data in ipairs(goods_list) do
			if good_data.goods_id == good_id then
				return good_data
			end
		end		
	end
end

--获取商品数据
function Modules.ShopModule:GetGoodDataByItemID(type, sub_type, item_id)
	local goods_list = self:GetGoodsListByType(type, sub_type)
	if goods_list then
		for _, good_data in ipairs(goods_list) do
			if good_data.item_id == item_id then
				return good_data
			end
		end		
	end
end

--获取普通商店或礼包商店列表
function Modules.ShopModule:GetGoodsListByType(type, sub_type)
	local goods_list = self.__goods_list_map[type]

	if type == Macros.Game.SHOP.STOREOEM then
		if goods_list then
			local vip_lev = CallModuleFunc(ModuleType.VIP , "GetVipLV")
			local show_goods_list = {}
			for i , v in ipairs( goods_list ) do
				if not v.show_vip_level or v.show_vip_level <= vip_lev then 
					table.insert( show_goods_list , v )
				end
			end	
			goods_list = show_goods_list
		end
	
	elseif type == Macros.Game.SHOP.GM_SHOP then
		if goods_list then
			goods_list = goods_list[sub_type]
		end

	elseif type == Macros.Game.SHOP.ARENA then
		if goods_list then
			goods_list = goods_list[sub_type]
		end
	end

	return goods_list
end

--获取可购买的礼包列表
function Modules.ShopModule:GetPurchasableGiftList(info)
	local vipLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)

	local t1 = {}
	if self.__goods_list_map[Macros.Game.SHOP.STOREOEM] then
		for i , v in ipairs( self.__goods_list_map[Macros.Game.SHOP.STOREOEM] ) do
			if vipLevel >= v.need_vip_level then
				table.insert(t1 , v)
			end
		end
	end
	return t1 
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--初始化普通商店列表
function Modules.ShopModule:__InitCommonGoodsList()
	self.__goods_list_map[Macros.Game.SHOP.STOREPROP] = {}
	self.__viplev_record_for_buy_limit = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)

	for id, config_good in ipairs(CPPGameLib.GetConfig("ItemShop",nil,nil,true)) do
		local good_data = self:ParseGood(Macros.Game.SHOP.STOREPROP, config_good)
		table.insert(self.__goods_list_map[Macros.Game.SHOP.STOREPROP], good_data)
	end

	GlobalCustomizedLogicAdapterMgr:Call("SortGoodsOfStore", self.__goods_list_map[Macros.Game.SHOP.STOREPROP])
end

--初始化礼包商店列表
function Modules.ShopModule:__InitGiftGoodsList()
	local goods_list = {}
	self.__goods_list_map[Macros.Game.SHOP.STOREOEM] = goods_list

	for id, config_good in pairs(CPPGameLib.GetConfig("GiftShop",nil,nil,true)) do
		local good_data = self:ParseGood(Macros.Game.SHOP.STOREOEM, config_good)
		table.insert(goods_list, good_data)
	end
	table.sort(goods_list, function(data1, data2)
		return data1.sort_id < data2.sort_id
	end)
end

--初始化GM商店列表
function Modules.ShopModule:__InitGMGoodsList(sub_type, config_goods_list)
	self.__goods_list_map[Macros.Game.SHOP.GM_SHOP] = self.__goods_list_map[Macros.Game.SHOP.GM_SHOP] or {}
	local goods_list = self.__goods_list_map[Macros.Game.SHOP.GM_SHOP][sub_type]

	if not goods_list then
		goods_list = {}
		self.__goods_list_map[Macros.Game.SHOP.GM_SHOP][sub_type] = goods_list
	
		for id, config_good in ipairs(config_goods_list) do
			local good_data = self:ParseGood(Macros.Game.SHOP.GM_SHOP, config_good)
			good_data.shop_sub_type = config_good.shop_sub_type
			table.insert(goods_list, good_data)
		end

		GlobalCustomizedLogicAdapterMgr:Call("SortGoodsOfStore", goods_list)		
	end

	return goods_list
end

--初始化竞技场商店列表
function Modules.ShopModule:__InitArenaGoodsList()
	local goods_list = {}
	self.__goods_list_map[Macros.Game.SHOP.ARENA] = goods_list
	local sub_goods_list
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.LEVEL)

	for id, config_good in ipairs(CPPGameLib.GetConfig("ArenaShop",nil,nil,true)) do
		local good_data = self:ParseGood(Macros.Game.SHOP.ARENA, config_good)
		if not good_data.need_role_level or good_data.need_role_level <= role_level then
			local sub_goods_list = goods_list[good_data.type]
			if not sub_goods_list then
				sub_goods_list = {}
				goods_list[good_data.type] = sub_goods_list
			end
			table.insert(sub_goods_list, good_data)
		end
	end
	for _, sub_goods_list in pairs(goods_list) do
		table.sort(sub_goods_list, function (good_data1, good_data2)
			if not good_data1 then
				return false

			elseif not good_data2 then
				return true
			else
				return good_data1.sort_id < good_data2.sort_id
			end
		end)	
	end
end

--设置购买记录
function Modules.ShopModule:__SetBuyRecord(type, data, sub_type)
	local buy_record_map = {}
	if not sub_type then
		self.__buy_record_map[type] = buy_record_map
	else
		self.__buy_record_map[type] = self.__buy_record_map[type] or {}
		self.__buy_record_map[type][sub_type] = buy_record_map
	end

	for i,shopData in ipairs(data) do
		buy_record_map[shopData.goods_id] = shopData.buy_num
	end
end

--获取购买记录
function Modules.ShopModule:__GetBuyRecordMap(type, good_id)
	local buy_record_map = self.__buy_record_map[type]
	if type == Macros.Game.SHOP.GM_SHOP and buy_record_map then
		local config_good = CPPGameLib.GetConfig("GMShop", good_id, false)
		if config_good then
			return buy_record_map[config_good.shop_sub_type]
		end
	else
		return buy_record_map
	end
end

--获取购买记录
function Modules.ShopModule:__GetBuyRecordMapEx(type, sub_type)
	local buy_record_map = self.__buy_record_map[type]
	if buy_record_map and sub_type then
		return buy_record_map[sub_type]
	end
	return buy_record_map
end

--购买成功相关处理
function Modules.ShopModule:__HandleBuyResponse(info)
	local shop_type = info.type
	local good_id = info.param1

	--更新购买记录
	local buy_record_map = self:__GetBuyRecordMap(shop_type, good_id)
	if buy_record_map then
		local buy_num = info.param2
		if buy_record_map[good_id] then
			buy_record_map[good_id] = buy_record_map[good_id] + buy_num
		else
			buy_record_map[good_id] = buy_num
		end

		local goods_list = self.__goods_list_map[shop_type]
		if shop_type == Macros.Game.SHOP.ARENA or shop_type == Macros.Game.SHOP.GM_SHOP then
			goods_list = goods_list[info.param3]
		end
		if goods_list then
			for i, good_data in ipairs(goods_list) do
				if good_data.goods_id == good_id then
					good_data.haveBuyNum = buy_record_map[good_id]

					--更新消耗数量
					if good_data.price_list then
						good_data.cost_value = good_data.price_list[good_data.haveBuyNum + 1] or good_data.price_list[#good_data.price_list]					
					end
					break
				end
			end
		end
	end

	--获得物品提示
	local cnf
	if shop_type == Macros.Game.SHOP.STOREPROP then
		cnf = CPPGameLib.GetConfig("ItemShop", info.param1, false)
	elseif shop_type == Macros.Game.SHOP.STOREOEM then
		cnf = CPPGameLib.GetConfig("GiftShop", info.param1, false)
	elseif shop_type == Macros.Game.SHOP.GM_SHOP then
		cnf = CPPGameLib.GetConfig("GMShop", info.param1, false)
	elseif shop_type == Macros.Game.SHOP.ARENA then
		cnf = CPPGameLib.GetConfig("ArenaShop", info.param1, false)
	end
	local t1 = {}
    table.insert(t1, {id = cnf.item_id, num = (cnf.item_num or 1) * info.param2})
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS, false, t1)	
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------



