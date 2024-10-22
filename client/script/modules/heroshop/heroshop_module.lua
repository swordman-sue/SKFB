--[[
%% @module: 英雄商店界面模块
%% @author: yjg
%% @created: 2016年10月25日22:01:22
--]]


Modules = Modules or {}

Modules.HeroShopModule = Modules.HeroShopModule or BaseClass(BaseModule)

Modules.HeroShopModule.HeroShop = 1
Modules.HeroShopModule.AwakenShop = 2

function Modules.HeroShopModule:__init()
	self.heroShopLst = {}
	self.awakenShopList = {}
end

function Modules.HeroShopModule:__delete()
    if self.screen then
        GlobalTimerQuest:CancelQuest(self.screen)
        self.screen = nil
    end 
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.HeroShopModule:HandleNewDayNotify()
	self:HeroShopDataReq()
	self:AwakenShopDataReq()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 刷新商店 消息Id: 20909
function Modules.HeroShopModule:ShopRefreshReq(info)
	CPPGameLib.PrintTable("xy" , info , "消息说明: 刷新商店: 消息Id: 20909")
	-- "shop_type__C",		--商店类型
	-- "refresh_type__C",	--刷新类型(1=使用免费次数，2=使用物品,3=使用雄魂)
	-- self.OpenIndex = info.openIndex
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SHOP_REFRESH_REQ)
	data.shop_type = info.shop_type
	data.refresh_type = info.refresh_type
	GlobalNetAdapter:OnSend(data) 

end

-- 消息说明: 刷新商店响应 消息Id: 20910
function Modules.HeroShopModule:ShopRefreshResp(info)
   	CPPGameLib.PrintTable("xy" , info,"消息说明: 刷新商店响应: 消息Id: 20910") 
   	if info.shop_type == Macros.Game.SHOP.CRYSTAL then
   		CallModuleFunc(ModuleType.CRYSTAL_SHOP,"CrystalShopRefreshResp" , info )
   	end
end

-- 消息说明: 英雄商店数据请求 消息Id: 20911
function Modules.HeroShopModule:HeroShopDataReq(type_)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 英雄商店数据请求 消息Id: 20911")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_SHOP_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 英雄商店数据响应 消息Id: 20912
function Modules.HeroShopModule:HeroShopDataResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 英雄商店数据响应 消息Id: 20912")
	self.heroShopLst = info
	GlobalEventSystem:FireNextFrame( Macros.Event.HeroShopModule.HERO_SHOP )
end

-- 消息说明: 英雄商店购买请求 消息Id: 
function Modules.HeroShopModule:HeroShopBuyReq(info)
	CPPGameLib.PrintTable("xy" , info , "英雄商店购买请求")	
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
end

-- 消息说明: 英雄商店购买响应 消息Id: 
function Modules.HeroShopModule:HeroShopBuyResp(info)
	CPPGameLib.PrintTable("xy" , info , "英雄商店购买响应")	

	if info.type == Macros.Game.SHOP.SOUL then

		if self.heroShopLst.goods_list then
			for i,v in ipairs(self.heroShopLst.goods_list) do
				if v.goods_id == info.param1 then
					v.buy_num = info.param2
					break
				end
			end
		end

		local cnf = CPPGameLib.GetConfig("HeroShop", info.param1) 
		local type_ , info_ = ObjIDTool.GetInfo(cnf.item_id)
		local lab = Util:LabCenter({
									{ zi = CPPGameLib.GetString("pack_congratsGet")},
									{ zi = info_.name .. "X" .. cnf.item_num, ys = info_.quality, mb = true},
								})
		GlobalTipMsg.GetInstance():ShowLayer( lab )
	-- elseif info.type == Macros.Game.SHOP.AWAKEN_SHOP then
	else
		for i,v in ipairs(self.awakenShopList.goods_list) do
			if v.goods_id == info.param1 then
				v.buy_num = info.param2
				break
			end
		end
	end
end

-- 消息说明: 觉醒商店数据请求 消息Id: 20913
function Modules.HeroShopModule:AwakenShopDataReq(index)
	CPPGameLib.PrintTable("xy" , info , "觉醒商店数据请求")	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_AWAKEN_SHOP_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 觉醒商店数据响应 消息Id: 20914
function Modules.HeroShopModule:AwakenShopDataResp(info)
	CPPGameLib.PrintTable("xy" , info , "觉醒商店数据响应")	
	self.awakenShopList = info   
	GlobalEventSystem:FireNextFrame( Macros.Event.HeroShopModule.AWAKEN_SHOP )
end

--------------------------------------------------------------------------------------------------------------------------------
--接口
--------------------------------------------------------------------------------------------------------------------------------
--获得上阵
function Modules.HeroShopModule:ToIdGetBattle( id )
	local type_ , value_ = ObjIDTool.GetInfo( id )
	local heroId = 0
	if type_ == Macros.Game.ObjType.ITEM then
		if value_.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
			heroId = value_.target_id
		end
	elseif type_ == Macros.Game.ObjType.HERO then
		heroId = id
	end

	if heroId ~= 0 then
		local lst = CallModuleFunc(ModuleType.HERO , "GetLineupList")

		for i,v in ipairs(lst) do
			local info = CallModuleFunc(ModuleType.HERO , "GetHeroInfo" , v.uid)
			if info and info.info_id == heroId then
				return true
			end
		end
	else
		return false
	end
end

--获得宿命
function Modules.HeroShopModule:ToIdGetFate( id )
	local type_ , value_ = ObjIDTool.GetInfo( id )
	local heroId = 0
	if type_ == Macros.Game.ObjType.ITEM then
		if value_.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
			heroId = value_.target_id
		end
	elseif type_ == Macros.Game.ObjType.HERO then
		heroId = id
	end

	if heroId ~= 0 then
		local num = CallModuleFunc(ModuleType.HERO , "GetNumByInfoID" , heroId )
		if num == 0 then
			local type_ , value_ = ObjIDTool.GetInfo( heroId )
			local t1 = {}

			if value_.relation_hero_list then
				for _,v in ipairs(value_.relation_hero_list) do
					for __,vv in ipairs(v) do
						t1[vv] = true
					end
				end

				local lst = CallModuleFunc(ModuleType.HERO , "GetLineupList")
				for i,v in ipairs(lst) do
					local info = CallModuleFunc(ModuleType.HERO , "GetHeroInfo" , v.uid)
					if info and t1[info.info_id] then
						return true
					end
				end
			end
		end
	else
		return false
	end
end

--获得合击
function Modules.HeroShopModule:ToIdGetPunch( id )
	local type_ , value_ = ObjIDTool.GetInfo( id )
	local heroId = 0
	if type_ == Macros.Game.ObjType.ITEM then
		if value_.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
			heroId = value_.target_id
		end
	elseif type_ == Macros.Game.ObjType.HERO then
		heroId = id
	end

	if heroId ~= 0 then
		local num = CallModuleFunc(ModuleType.HERO , "GetNumByInfoID" , heroId )
		if num == 0 then
			local type_ , value_ = ObjIDTool.GetInfo( heroId )
			local t1 = {}
			
			if value_.joint_hero_list then
				for _,v in ipairs(value_.joint_hero_list) do
					t1[v] = true
				end

				local lst = CallModuleFunc(ModuleType.HERO , "GetLineupList")
				for i,v in ipairs(lst) do
					local info = CallModuleFunc(ModuleType.HERO , "GetHeroInfo" , v.uid)
					if info and t1[info.info_id] then
						return true
					end
				end
			end

		end
	else
		return false
	end
end

--获得英雄商店列表
function Modules.HeroShopModule:GetHeroShopList()
	if self.heroShopLst.goods_list then
		for i,v in ipairs(self.heroShopLst.goods_list) do
			v.type = 1
		end
	end
	return self.heroShopLst.goods_list
end

--获得觉醒商店列表
function Modules.HeroShopModule:GetAwakenShopList()
	if self.awakenShopList.goods_list then
		for i,v in ipairs(self.awakenShopList.goods_list) do
			v.type = 2
		end
	end
	return self.awakenShopList.goods_list
end

--获得英雄商店免费次数
function Modules.HeroShopModule:GetHeroShopFreeRefresh()
	return ConfigAdapter.Common.GetHeroShopFreeNum() - ( self.heroShopLst.free_refresh_times  or 0 ) , ConfigAdapter.Common.GetHeroShopFreeNum()
end

--获得物品次数
function Modules.HeroShopModule:GetHeroShopCostRefresh()
	return self.heroShopLst.cost_refresh_times or 0
end

--获得觉醒商店免费次数
function Modules.HeroShopModule:GetAwakenShopFreeRefresh()
	return ConfigAdapter.Common.GetAwakenShopFreeNum() - ( self.awakenShopList.free_refresh_times or 0 ) , ConfigAdapter.Common.GetAwakenShopFreeNum()
end

--获得觉醒商店物品次数
function Modules.HeroShopModule:GetAwakenShopCostRefresh()
	return self.awakenShopList.cost_refresh_times or 0
end

--获得英雄商店CD
function Modules.HeroShopModule:GetHeroShopTime()
	local endTime = ( self.heroShopLst.last_recover_time or 0 )  + (ConfigAdapter.Common.GetHeroShopFreeFlush() * 60)
	endTime = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return endTime
end

--获得觉醒商店CD
function Modules.HeroShopModule:GetAwakenShopTime()
	local endTime = ( self.awakenShopList.last_recover_time or 0 ) + (ConfigAdapter.Common.GetAwakenShopRefrashCD() * 60)
	endTime = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return endTime
end

--获得觉醒商店CD
function Modules.HeroShopModule:GetFreeRefreshTimes()
	return self.heroShopLst.free_refresh_times
end

function Modules.HeroShopModule:SetNowShow( is )
	self.nowShow = is
end

function Modules.HeroShopModule:GetNowShow( )
	return self.nowShow
end

