--[[
%% @module: 直购特惠主界面
%% @author: swordmansue
%% @created: 2017-08-31
--]]

Modules = Modules or {} 

Modules.DirectBuyShopModule = Modules.DirectBuyShopModule or BaseClass(BaseModule)

function Modules.DirectBuyShopModule:__init()
end

function Modules.DirectBuyShopModule:__delete()  
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.DirectBuyShopModule:HandleNewDayNotify()
	self:DirectNotesRequest()
end

------------------------------------------------------------------------------------------------------------------------
--直购商店
------------------------------------------------------------------------------------------------------------------------
--直购商店数据请求
function Modules.DirectBuyShopModule:DirectShopDataReq( type )
	if type then
		self.__is_direct_shop_req = false
	else
		self.__is_direct_shop_req = true
	end

    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_DIRECT_SHOP_DATA_REQ)
    GlobalNetAdapter:OnSend(protocol)
end

--直购商店数据响应
function Modules.DirectBuyShopModule:DirectShopDataResp(protocol_data)
    self.directShopgoodsList = protocol_data.goods_list

	if self.isDiamond ~= true then
	    for i , v in ipairs( protocol_data.can_take_goods_list ) do
	    	if not self.can_take_goods_list then
	    		self.can_take_goods_list = {}
	    	end
	    	self.can_take_goods_list[ v.goods_id ] = true
	    end
	    self.isDiamond = false
	end

    self:GetDesenoTime()

    --服务器主动响应才取消标记
    if not self.__is_direct_shop_req then
    	self:SetDirectBuyBuyTag( false )
    end
    self.__is_direct_shop_req = false
    
    self:FireNextFrame(Macros.Event.DirectBuyModule.DIRECT_SHOP_DATA_RESP)

end

--领取直购商品奖励请求
function Modules.DirectBuyShopModule:TakeDirectBuyGoodsReq(goods_id)
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_DIRECT_BUY_GOODS_REQ)
    protocol.goods_id = goods_id 
    GlobalNetAdapter:OnSend(protocol)    
end

--领取直购商品奖励响应
function Modules.DirectBuyShopModule:TakeDirectBuyGoodsResp(protocol_data)
	self.can_take_goods_list[ protocol_data.goods_id ] = nil

	self:__ShowRewardList(protocol_data.goods_id)

    self:FireNextFrame(Macros.Event.DirectBuyModule.TAKE_DIRECT_BUY_GOODS_RESP, protocol_data.goods_id)
end

--钻石购买
function Modules.DirectBuyShopModule:ShopBuyRequest( type_ , goods_id )
	self.isDiamond = true

	local info = {}
	info.type = type_
	info.param1 = goods_id
	info.param2 = 1
	info.param3 = 0
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
end

--钻石购买回调
function Modules.DirectBuyShopModule:DirectBuyResponse( protocol_data )

	if protocol_data.type == Macros.Game.SHOP.DIRECT_BUY then
		self:__ShowRewardList(protocol_data.param1)
	else
		self:__ShowHotRewardList(protocol_data.param1)
	end
end


--购买列表
function Modules.DirectBuyShopModule:GetBuyGoodsList()
	return self.directShopgoodsList
end

--领取列表
function Modules.DirectBuyShopModule:GetBuyCanTakeGoodsList( id )
	if self.can_take_goods_list then
		return self.can_take_goods_list[ id ]
	end
end

--购买记录请求
function Modules.DirectBuyShopModule:DirectNotesRequest( info )
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",Macros.Game.SHOP.CONCERT_BUY)
end

--购买记录回调
function Modules.DirectBuyShopModule:DirectNotesResponse( info )
	if not self.buy_record_list then
		self.buy_record_list = {}
	end

	for i,v in ipairs(info.buy_record_list) do
		self.buy_record_list[ v.goods_id ] = v.buy_num
	end

    self:FireNextFrame(Macros.Event.DirectBuyModule.CONVERT_DATA_RESP)
end

--购买列表
function Modules.DirectBuyShopModule:GetConvertGoodsList()
	local Cnf = CPPGameLib.GetConfig("ExchangeShop", nil , nil , true )

	local t1 = {}
	local t2 = {}
	for i , v in ipairs( Cnf ) do
		local num = self:GetConvertRecordLst(v.goods_id) 
		if num and ( v.buy_num_max - num <= 0) then
			table.insert( t2 , { goods_id = v.goods_id , buy_num = num or 0 } )	
		else
			table.insert( t1 , { goods_id = v.goods_id , buy_num = num or 0 } )
		end
	end

	for i , v in ipairs( t2 ) do
		table.insert( t1 , v )
	end
	return t1
end

--领取列表
function Modules.DirectBuyShopModule:GetConvertRecordLst( id )
	if id then
		return self.buy_record_list[ id ]
	else
		return self.buy_record_list
	end
end

------------------------------------------------------------------------------------------------------------------------
--物品兑换
------------------------------------------------------------------------------------------------------------------------
--物品兑换请求
function Modules.DirectBuyShopModule:ConvertShopBuyRequest( goods_id )
	local info = {}
	info.type = Macros.Game.SHOP.CONCERT_BUY
	info.param1 = goods_id
	info.param2 = 1
	info.param3 = 0
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
end


--物品兑换响应
function Modules.DirectBuyShopModule:ConvertShopBuyResponse( protocol_data )
	if self.buy_record_list[ protocol_data.param1 ] then
		self.buy_record_list[ protocol_data.param1 ] = self.buy_record_list[ protocol_data.param1 ] + 1
	else
		self.buy_record_list[ protocol_data.param1 ] = 1
	end

	local config_shop_goods = CPPGameLib.GetConfig("ExchangeShop", protocol_data.param1)
	local t1 = {}
    table.insert(t1,{id = config_shop_goods.item_id , num = config_shop_goods.item_num})
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.DirectBuyModule.CONVERT_DATA_RESP)
end

------------------------------------------------------------------------------------------------------------------------
--精准推送
------------------------------------------------------------------------------------------------------------------------
--精准推送商店数据请求
function Modules.DirectBuyShopModule:AccurateRecommendDataReq()	
	self.__is_req = true
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ACCURATE_RECOMMEND_DATA_REQ)
    GlobalNetAdapter:OnSend(protocol)
end

--精准推送商店数据响应
function Modules.DirectBuyShopModule:AccurateRecommendDataResp(protocol_data)
    self.__accurate_recommend_goods_list = {}
    for i,good_info in ipairs(protocol_data.goods_list) do
    	self.__accurate_recommend_goods_list[good_info.goods_id] = good_info 
    end   
    self:FireNextFrame(Macros.Event.DirectBuyModule.DIRECT_HOT_RECOMMENT_SHOP_DATA_RESP,self.__accurate_recommend_goods_list,self.__is_req)
    --服务器主动响应才取消标记
    if not self.__is_req then
    	self.__hot_recomment_buy_tag = false
    end
    self.__is_req = false
end

--领取精准推送商品奖励请求
function Modules.DirectBuyShopModule:TakeAccurateRecommendGiftReq(goods_id)
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_ACCURATE_RECOMMEND_GIFT_REQ)
    protocol.goods_id = goods_id 
    GlobalNetAdapter:OnSend(protocol)    
end

--领取精准推送商品奖励响应
function Modules.DirectBuyShopModule:TakeAccurateRecommendGiftResp(protocol_data)
	local good_info = self.__accurate_recommend_goods_list[protocol_data.goods_id]
	if good_info then
		good_info.can_take_times = good_info.can_take_times - 1
	end
	self:__ShowRewardList(protocol_data.goods_id,true)

    self:FireNextFrame(Macros.Event.DirectBuyModule.TAKE_ACCURATE_RECOMMEND_GIFT_RESP, protocol_data.goods_id)
end

------------------------------------------------------------------------------------------------------------------------
--活动热卖
------------------------------------------------------------------------------------------------------------------------
-- 消息说明:热卖商品数据请求 消息Id: 26501
function Modules.DirectBuyShopModule:HotSaleGoodsDataReq( info )
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HOT_SALE_GOODS_DATA_REQ)
    GlobalNetAdapter:OnSend(protocol)   
end

-- 消息说明:热卖商品数据响应 消息Id: 26502
-- goods_id@H      // 商品ID
-- cur_data_idx@H  // 当前数据索引
-- buy_times@H     // 购买次数
-- take_times@H    // 领取次数
function Modules.DirectBuyShopModule:HotSaleGoodsDataResp( info )
	self.hotSalegoods_id = {}
	for i , v in ipairs( info.goods_list ) do
		self.hotSalegoods_id[ v.goods_id ] = v
	end

	self:FireNextFrame(Macros.Event.DirectBuyModule.HOT_SALE_GOODS_DATA_RESP)
end

-- 消息说明:领取热卖商品充值奖励请求 消息Id: 26503
-- "goods_id__H",      --要领取的商品ID
function Modules.DirectBuyShopModule:TakeHotSaleGoodsRechargeRewardReq( info )
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ)
   	protocol.goods_id = info.goods_id
    GlobalNetAdapter:OnSend(protocol) 
end

-- 消息说明:领取热卖商品充值奖励响应 消息Id: 26504
-- "goods_id__H",      --要领取的商品ID
function Modules.DirectBuyShopModule:TakeHotSaleGoodsRechargeRewardResp( info )
	local cnf = self:GetHotSaleGoodsCnf( info.goods_id )
	local t1 = {}
	table.insert( t1 , { id = cnf.item_id , num = cnf.item_num } )
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.DirectBuyModule.HOT_SALE_GOODS_DATA_RESP)
end

-- 获得热卖商品表
function Modules.DirectBuyShopModule:GetHotSaleGoodsCnf( goods_id )
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.HOT_SALE )
    if not info.subtype then
    	return {}
    end

	local cnf = CPPGameLib.GetConfig("HotSaleGoods", info.subtype )
	if not self.hotSaleGoodslst then
		self.hotSaleGoodslst = {}
		for i , v in pairs( cnf.goods_list ) do
			local pick = self:GetHotSaleGoodsPick( i )

			local take = self:GetHotSaleTakeTimes( i )

			local curIdx = self:GetHotSaleCurIdx( i ) + 1
			
			local listSum = #v.item_id_list

			local item_id = v.item_id_list and v.item_id_list[ curIdx ]
			if not item_id then
				item_id = v.item_id_list[ #v.item_id_list ]
			end

			local item_num = v.item_num_list and v.item_num_list[ curIdx ] 
			if not item_num then
				item_num = v.item_num_list[ #v.item_num_list ]
			end

			local buy_num_max = v.buy_num_max_list and v.buy_num_max_list[ curIdx ]
			if not buy_num_max then
				buy_num_max = v.buy_num_max_list[ #v.buy_num_max_list ]
			end

			local buy_sum_max = buy_num_max * listSum

			local need_level = v.need_level_list and v.need_level_list[ curIdx ]
			if not need_level then
				need_level = v.need_level_list[ #v.need_level_list ]
			end

			local good_info = { 
						goods_id = i , pick = pick , take = take , listSum = listSum ,
						item_id = item_id , item_num = item_num , 
						buy_num_max = buy_num_max , buy_sum_max = buy_sum_max ,
						need_level = need_level , curIdx = curIdx , sort = v.sort , 
						cost_type = v.cost_type_list and v.cost_type_list[ curIdx ] , 
						cost_value = v.cost_value_list and v.cost_value_list[ curIdx ] , 
						diamond = v.diamond_discount and v.diamond_discount[ curIdx ] , 
						recharge_id = v.recharge_id_list and v.recharge_id_list[ curIdx ] , 
						recharge = v.recharge_discount and v.recharge_discount[ curIdx ] , 
						reset_day = v.reset_day_list , 
				 	}
			table.insert( self.hotSaleGoodslst , good_info )
		end
		table.sort( self.hotSaleGoodslst , function( a , b )
			return a.sort < b.sort
		end )
	end

	for i , v in ipairs( self.hotSaleGoodslst ) do
		v.pick = self:GetHotSaleGoodsPick( v.goods_id )
		v.take = self:GetHotSaleTakeTimes( v.goods_id )
		v.curIdx = self:GetHotSaleCurIdx( v.goods_id ) + 1
	end

	if goods_id then
		for i,v in ipairs( self.hotSaleGoodslst ) do
			if v.goods_id == goods_id then
				return v
			end
		end
	else
	    return self.hotSaleGoodslst
   	end
end

-- 获得热卖商品购买状态
function Modules.DirectBuyShopModule:GetHotSaleGoodsPick( id )
	return self.hotSalegoods_id[ id ] and self.hotSalegoods_id[ id ].buy_times or 0
end

-- 获得热卖商品领取状态
function Modules.DirectBuyShopModule:GetHotSaleTakeTimes( id )
	return self.hotSalegoods_id[ id ] and self.hotSalegoods_id[ id ].take_times or 0
end

--获得热卖商品顺位
function Modules.DirectBuyShopModule:GetHotSaleCurIdx( id )
	return self.hotSalegoods_id[ id ] and self.hotSalegoods_id[ id ].cur_data_idx or 0
end

-- 获得热卖商品活动开放时间
function Modules.DirectBuyShopModule:GetHotSaleActivityTime(  )
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.HOT_SALE )
	return info
end

-- 获得热卖商品活动开放了几天
function Modules.DirectBuyShopModule:GetHotSalePassTime(  )
    --归零点的开始时间
	local info = self:GetHotSaleActivityTime()
    local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( info.star )
    local star = info.star - ( hour * 60 * 60  ) - ( minute * 60 ) - second

    --现在的时间
	local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( n_Time )
	local nowTime = n_Time - ( hour * 60 * 60  ) - ( minute * 60 ) - second

	return ( nowTime - star ) / 60 / 60 / 24  + 1
end

--获得还有没有已购买未领取的东西
function Modules.DirectBuyShopModule:GetHotSaleUnclaimed(  )
	local t1 = 0
	if not self.hotSalegoods_id then
		return 0
	end
	for k , v in pairs( self.hotSalegoods_id ) do
		if v.buy_times > v.take_times then
			t1 = t1 + 1
		end
	end
	return t1
end


--刷新时间
function Modules.DirectBuyShopModule:GetDirectBuyUpTime( )
	local t1 = ""
	for i , v in ipairs( ConfigAdapter.Common.GetDirectBuyUpTime() ) do
		local time = string.format( "%04d" , v )
		local shi = string.sub( time , 0 , 2 )
		local fen = string.sub( time , 3 , 4 )
		local t2 = ""
		if i ~= #ConfigAdapter.Common.GetDirectBuyUpTime() then
			t2 = CPPGameLib.GetString("common_Comma")
		end
		t1 = t1 .. shi ..":"..fen .. t2
	end
	return t1 
end

--获得限时倒计时
function Modules.DirectBuyShopModule:GetDesenoTime()
	if self.time then
		GlobalTimerQuest:CancelQuest( self.time )
		self.time = nil
	end

	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	-- --当前服务器时间
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(serverTime)

	local time = nil
	local lst = ConfigAdapter.Common.GetDirectBuyUpTime()
	for i , v in ipairs( lst ) do
		local nTime = hour..minute
		if v >= tonumber(nTime) then
			v = string.format( "%04d" , v )

			local h = string.sub( v , 0 , 2 )
			local m = string.sub( v , 3 , 4 )

			h = h * 60 * 60
			m = m * 60
			time = h + m
			break
		end
	end

	if not time then
		local h = string.sub( string.format( "%04d" , lst[1] ) , 0 , 2 )
		local m = string.sub( string.format( "%04d" , lst[1] )  , 3 , 4 )
		nexTime = ( 24 * 60 * 60 )

		nexTime = nexTime - ( ( hour * 60 * 60 ) + (minute*60) + second )
		time = ( nexTime + ( ( h * 60 * 60 ) + (m * 60) ) ) 
		time = time + ( ( hour * 60 * 60 ) + (minute*60) + second )
	end

	self.time = GlobalTimerQuest:AddPeriodQuest(
	    GlobalCallbackMgr:AddCallBackFunc(function()
		    local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		    local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(serverTime)
		    local endTime = time - ( ( hour * 60 * 60 ) + (minute*60) + second )
	        if endTime == 0 then
	        	self:DirectShopDataReq()
	        else
	        end
	end),1,-1)
end

--展示获得物品
function Modules.DirectBuyShopModule:__ShowRewardList(goods_id,is_hot_recomment)
	local config_shop_goods 
	if is_hot_recomment then
		config_shop_goods = CPPGameLib.GetConfig("AccurateRecommendGift", goods_id)
	else
		config_shop_goods = CPPGameLib.GetConfig("DirectBuyShop", goods_id)
	end	
	if not config_shop_goods then
		return
	end

	local config_item = CPPGameLib.GetConfig("Item", config_shop_goods.item_id)
	if config_item then
		local reward_list
		if config_item.type == Macros.Game.ItemBigType.GIFT then
			--礼包
		    reward_list = ConfigAdapter.Item.GetGiftPackRewardList(config_shop_goods.item_id)
		else
			--其他
			reward_list = {{id = config_shop_goods.item_id, num = config_shop_goods.item_num}}
		end
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )
	end
end

--展示获得物品（热卖
function Modules.DirectBuyShopModule:__ShowHotRewardList(goods_id)

	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.HOT_SALE )
    if not info.subtype then
    	return {}
    end

	local config_shop_goods = CPPGameLib.GetConfig("HotSaleGoods" , info.subtype )
	if not config_shop_goods then
		return
	end
	config_shop_goods = config_shop_goods.goods_list[ goods_id ]

	local t1 = {}
	for i , v in ipairs( config_shop_goods.item_id_list ) do
		table.insert( t1 , { id = v , num = config_shop_goods.item_num_list[i] } )
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

--动作开关
function Modules.DirectBuyShopModule:SetActionType( var )
	self.actionType = var
end

--动作开关
function Modules.DirectBuyShopModule:GetActionType( var )
	return self.actionType
end

--直购结束1
function Modules.DirectBuyShopModule:GetDirectBuyShopOpen1( var )
	local cnf = CPPGameLib.GetConfig("Activity", Macros.Game.SystemID.DIRECT_BUY  )
	local activityCnf = CallModuleFunc(ModuleType.ACTIVITY , "ToTypeSettleActivityType" , cnf.sort_list[1].time_type , cnf.sort_list[1] )

	local info = {}
	info.isOpen = "no"
	if activityCnf.judge == "open" then
		info.isOpen = "yes"
	end

	local text = ""
	if activityCnf.star then
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.star )
		text = CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 

		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.over )
		text = text .. "~" ..  CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 
	end
	return text , info
end

--直购结束2
function Modules.DirectBuyShopModule:GetDirectBuyShopOpen2( var )
	local cnf = CPPGameLib.GetConfig("Activity", Macros.Game.SystemID.DIRECT_BUY_SHOW  )
	local activityCnf = CallModuleFunc(ModuleType.ACTIVITY , "ToTypeSettleActivityType" , cnf.sort_list[1].time_type , cnf.sort_list[1] )

	local info = {}
	info.isOpen = "no"
	if activityCnf.judge == "open" then
		info.isOpen = "yes"
	end

	local text = ""
	if activityCnf.star then
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.star )
		text = CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 

		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.over )
		text = text .. "~" ..  CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 
	end
	return text , info
end

--热卖结束3
function Modules.DirectBuyShopModule:GetDirectBuyShopOpen3( var )
	local activityCnf = CallModuleFunc(ModuleType.ACTIVITY , "ToIdGetActivityTime" , Macros.Game.SystemID.HOT_SALE )

	local info = {}
	info.isOpen = "no"
	if activityCnf.judge == "open" then
		info.isOpen = "yes"
	end

	local text = ""
	if activityCnf.star then
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.star )
		text = CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 
		
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityCnf.over )
		text = text .. "~" ..  CPPGameLib.GetString( "time_all" , string.sub( year , 3 , 4 ) , month , day , hour ) 
	end
	return text , info
end

--活动开启状态
function Modules.DirectBuyShopModule:GetDirectBuyActivity( var )

	local num = 0
	local _ , info = self:GetDirectBuyShopOpen1()
	
	if info.isOpen == "yes" then
		num = num + 1
	end

	local _ , info = self:GetDirectBuyShopOpen2()
	if info.isOpen == "yes" then
		num = num + 1
	end
	return num
end

--获取精准推送购买标记
function Modules.DirectBuyShopModule:GetHotRecommentBuyTag()
	return self.__hot_recomment_buy_tag or 0
end

--设置精准推送购买标记
function Modules.DirectBuyShopModule:SetHotRecommentBuyTag(is_buy_tag)
	self.__hot_recomment_buy_tag = is_buy_tag
end


--获取直购购买标记
function Modules.DirectBuyShopModule:GetDirectBuyBuyTag()
	return self.__direct_buy_buy_tag or 0
end

--设置直购购买标记
function Modules.DirectBuyShopModule:SetDirectBuyBuyTag(is_buy_tag)
	self.__direct_buy_buy_tag = is_buy_tag
end


--获取直购购买标记
function Modules.DirectBuyShopModule:GetHotSaleBuyTag()
	return self.__hot_sale_buy_tag or 0
end

--设置直购购买标记
function Modules.DirectBuyShopModule:SetHotSaleBuyTag(is_buy_tag)
	self.__hot_sale_buy_tag = is_buy_tag
end