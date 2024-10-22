--[[
%% @module: 水晶商店
%% @author: yjg
%% @created: 2018年5月9日10:27:38
--]]

Modules = Modules or {} 

Modules.CrystalShopModule = Modules.CrystalShopModule or BaseClass(BaseModule)

function Modules.CrystalShopModule:__init()
end

function Modules.CrystalShopModule:__delete()  
end

--购买是否显示
function Modules.CrystalShopModule:BuyIsShow() 
	return true
end

--充值是否显示
function Modules.CrystalShopModule:RechargeIsShow() 
	return true
end

-- 消息说明: 水晶商店数据请求 消息Id: 20921
function Modules.CrystalShopModule:CrystalShopDataReq( info )  
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店数据请求 消息Id: 20921")  
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CRYSTAL_SHOP_DATA_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明: 水晶商店数据响应 消息Id: 20922
-- "today_refresh_times__H", --今日刷新次数(已经使用的)
-- "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
function Modules.CrystalShopModule:CrystalShopDataResp( info )  
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店数据请求 消息Id: 20922")  
	self.goods_list = info.goods_list
	self.today_refresh_times = info.today_refresh_times
	GlobalEventSystem:FireNextFrame(Macros.Event.CrystalShopModule.CRYSTAL_BUY_DATA)
end

-- 消息说明: 水晶商店购买请求 消息Id: 
function Modules.CrystalShopModule:CrystalShopBuyReq( id )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店购买请求 消息Id: ")
	local info = {}
	info.type = Macros.Game.SHOP.CRYSTAL
    info.param1 = id
    info.param2 = 1
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
end

-- 消息说明: 水晶商店购买响应 消息Id: 
function Modules.CrystalShopModule:CrystalShopBuyResp(info)
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店购买请求 消息Id: ")

	local config = CPPGameLib.GetConfig("CrystalShop", info.param1 )
	local t1 = {}
    table.insert(t1,{id = config.item_id , num = config.item_num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

-- 消息说明: 水晶商店刷新请求 消息Id: 
function Modules.CrystalShopModule:CrystalShopRefreshReq( id ) 
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店刷新请求 消息Id: ")
	local info = {}
	info.shop_type = Macros.Game.SHOP.CRYSTAL
	info.refresh_type = 0
	CallModuleFunc(ModuleType.HERO_SHOP , "ShopRefreshReq" ,info)
end

-- 消息说明: 水晶商店刷新响应 消息Id: 
function Modules.CrystalShopModule:CrystalShopRefreshResp( id ) 
	CPPGameLib.PrintTable("xy" , info , "消息说明: 水晶商店刷新响应 消息Id: ")
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("crystalShop_UpTips") )
end

--获得商品列表
function Modules.CrystalShopModule:ToIdGetGoodsNum( id ) 
	for i , v in ipairs( self.goods_list ) do
		if v.goods_id == id then
			return v
		end
	end
end

--获得商品列表
function Modules.CrystalShopModule:GetGoodsList( ) 
	local t1 = {}
	if not self.goods_list then
		self.goods_list = {}
	end
	for i , v in ipairs( self.goods_list ) do
		local config = CPPGameLib.GetConfig("CrystalShop", v.goods_id )
		local t2 = {}
		for j , vv in ipairs( config.cost_type_list ) do
			table.insert( t2 , { id = vv , value = config.cost_value_list[j] } )
		end
		table.insert( t1 , { 
								id = v.goods_id , 
								item_id = config.item_id , 
								item_num = config.item_num , 
								buy_num_max = config.buy_num_max ,
								buy_num = v.buy_num ,
								cost = t2 , 
								is_recommend = config.is_recommend
							} )
	end

    return t1
end

--获得商品列表
function Modules.CrystalShopModule:GetRefreshTimes()
	return self.today_refresh_times or 0 , ConfigAdapter.Common.GetCrystalShopUpItem() or 0
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--水晶充值（原豪充 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 豪华签到数据请求 消息Id: 22705
function Modules.CrystalShopModule:LuxurySignInDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 豪华签到数据请求 消息Id: 22705")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LUXURY_SIGN_IN_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 豪华签到数据响应 消息Id: 22706
-- sign_in_id@H    // 签到Id
-- status@H    // 奖励状态(0=未达到领取条件,1=可以领取,2=已领取)
-- reward_id@H // 奖励Id
-- "sign_in_data_list__T__sign_in_id@H##status@H##reward_id@H", -- 签到数据列表
function Modules.CrystalShopModule:LuxurySignInDataResp( info )
	self.luxurySignInData = {}
	for i , v in ipairs( info.sign_in_data_list ) do
		self.luxurySignInData[v.sign_in_id] = v
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_LUXURY)
end

-- 消息说明: 领取豪华签到奖励 消息Id: 22707
function Modules.CrystalShopModule:TakeLuxurySignInRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取豪华签到奖励 消息Id: 22707")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_LUXURY_SIGN_IN_REWARD_REQ)
	data.sign_in_id = info.sign_in_id
	GlobalNetAdapter:OnSend(data) 
end

-- 领取豪华签到奖励响应 消息Id: 22708
function Modules.CrystalShopModule:TakeLuxurySignInRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取豪华签到奖励响应 消息Id: 22708")
	if self.luxurySignInData[info.sign_in_id] then
		self.luxurySignInData[info.sign_in_id].status = 2

		local cnf = CPPGameLib.GetConfig("LuxurySignInRewardList", self.luxurySignInData[info.sign_in_id].reward_id )
		if not cnf then return end
		local t1 = {}
		for i , v in ipairs( cnf.item_id_list ) do
			table.insert( t1 , { id = v , num = cnf.item_num_list[i] } )
		end
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_LUXURY)
end

--获得豪充表
function Modules.CrystalShopModule:GetLuxuryechargeCnf( ... )
	if not self.luxurySignInData then
		local t1 = {}
		return t1
	end

	local t1 = {}
	for id , v in pairs( self.luxurySignInData ) do
		local cnf = CPPGameLib.GetConfig("LuxurySignIn", id )
		if cnf then
			v.money = cnf.level_list[1].recharge_id
			table.insert( t1 , v )
		end
	end

	table.sort( t1, function ( a , b )
		return a.money < b.money
	end )

	local t2 = {}
	local t3 = {}
	local t5 = {}
	for _ , v in ipairs( t1 ) do
		local cnf = CPPGameLib.GetConfig("LuxurySignInRewardList", v.reward_id )
		if cnf then
			local t4 = {}
			for i , vv in ipairs( cnf.item_id_list ) do
				table.insert( t4 , { id = vv , num = cnf.item_num_list[i] } )
			end
			v.item = t4
			if v.status == 2 then
				table.insert( t3 , v )
			elseif v.status == 1 then
				table.insert( t2 , v )
			else
				table.insert( t5 , v )
			end
		end
	end

	for i , v in ipairs(t5) do
		table.insert( t2 , v )
	end

	for i , v in ipairs(t3) do
		table.insert( t2 , v )
	end

	return t2
end

--获得豪充表红点
function Modules.CrystalShopModule:GetLuxuryeFundNum( ... )
	local num = 0
	for k , v in pairs( self.luxurySignInData ) do
		if v.status == 1 then
			num = num + 1
		end
	end
	return num
end

--获得请求状态
function Modules.CrystalShopModule:GetLuxurySignInType()
	return self.luxurySignInData
end

