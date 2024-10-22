--[[
%% @module: 超值购
%% @author: yjg
%% @created: 2017年8月24日21:34:46
--]]


Modules = Modules or {}

Modules.SuperDiscountModule = Modules.SuperDiscountModule or BaseClass(BaseModule)

function Modules.SuperDiscountModule:__init()
end

function Modules.SuperDiscountModule:__delete()
end

-- 消息说明:超值折扣数据请求 消息Id: 25001
function Modules.SuperDiscountModule:SuperDiscountDataReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:超值折扣数据请求 消息Id: 25001")
    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SUPER_DISCOUNT_DATA_REQ))
end

-- 消息说明:超值折扣数据响应 消息Id: 25002
-- "status__H",                --购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
-- "take_accumulate_reward_times__H",  --已领取累积奖励的次数
-- "cur_buy_time__H",          --当前购买次数(领取奖励时会重置)
function Modules.SuperDiscountModule:SuperDiscountDataResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:超值折扣数据响应 消息Id: 25002")
    self.superDiscountData = info
    GlobalEventSystem:FireNextFrame(Macros.Event.SuperDiscountModule.SUPERDISCOUNT)
end

-- 消息说明:领取超值折扣购买奖励请求 消息Id: 25003
function Modules.SuperDiscountModule:SuperDiscountBuyRewardReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取超值折扣购买奖励请求 消息Id: 25003")
    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SUPER_DISCOUNT_BUY_REWARD_REQ))
end

-- 消息说明:领取超值折扣购买奖励响应 消息Id: 25004
function Modules.SuperDiscountModule:SuperDiscountBuyRewardResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取超值折扣购买奖励响应 消息Id: 25004")
    self.superDiscountData.status = 2
    GlobalEventSystem:FireNextFrame(Macros.Event.SuperDiscountModule.SUPERDISCOUNT)
	local lst = self:GetNowTimeAward()
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , lst )
end

-- 消息说明:领取超值折扣累积奖励请求 消息Id: 25005
function Modules.SuperDiscountModule:SuperDiscounitAccumulateRewardReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取超值折扣累积奖励请求 消息Id: 25005")
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
    data.times = info.times
    GlobalNetAdapter:OnSend( data )
end

-- 消息说明:领取超值折扣累积奖励响应 消息Id: 25006
function Modules.SuperDiscountModule:SuperDiscounitAccumulateRewardResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取超值折扣累积奖励响应 消息Id: 25006")

	local lst = self:GetAccumulateRewad( true )
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , lst )

	self:SuperDiscountDataReq()
end

--获得购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
function Modules.SuperDiscountModule:GetStatus()
	if self.superDiscountData then
		return self.superDiscountData.status
	end
end

--获得已领取累积奖励的次数
function Modules.SuperDiscountModule:GetRewardTimes( id , all )
	if self.superDiscountData then
		local lst = self.superDiscountData.took_accumulate_reward_list
		if id then
			for k , v in pairs( lst ) do
				if v.times == id then
					return 1
				end
			end
		elseif all then
			return lst
		end
	end
	return 0
end

--获得购买次数
function Modules.SuperDiscountModule:GetCurBuyTime()
	if self.superDiscountData then
		return self.superDiscountData.cur_buy_time
	end
end

--获得当前天数奖励
function Modules.SuperDiscountModule:GetNowTimeAward()
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , 94 )

	if info and info.show == "open" then
		local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

		local time = n_Time - info.star 
		time = math.ceil( time / 60 / 60 / 24 ) 

		local superDiscountCnf = CPPGameLib.GetConfig("SuperDiscountBuyReward", time )
		
		local t1 = {}
		if superDiscountCnf then
			for k , v in ipairs( superDiscountCnf.item_id_list ) do
				table.insert( t1 , { id = v , num = superDiscountCnf.item_num_list[k] } )
			end

			local rechargeCnf = CPPGameLib.GetConfig("Recharge" , superDiscountCnf.pay_id )

			return t1 , rechargeCnf.money , info.star , info.over , superDiscountCnf.pay_id 
		else
			return t1 , rechargeCnf.money , info.star , info.over , superDiscountCnf.pay_id 
		end
	end
end

--当前显示的累计的奖励
function Modules.SuperDiscountModule:GetAccumulateRewad( before )

	local lv = 1
	if before then
		lv = 0
	end

	local superDiscount = CPPGameLib.GetConfig("SuperDiscountAccumulateReward", ( self:GetCurBuyTime() or 0 ) + lv , false )
	local t1 = {}
	local need_buy_times = nil
	if superDiscount then
		for i , v in ipairs( superDiscount.item_id_list ) do
			table.insert( t1 , { id = v , num = superDiscount.item_num_list[i] } )
		end
		need_buy_times = superDiscount.need_buy_times
	end
	return t1 , need_buy_times
end



--当前显示的累计的奖励
function Modules.SuperDiscountModule:GetSuperDiscountAccumulateReward( )
	local superDiscount = CPPGameLib.GetConfig("SuperDiscountAccumulateReward" , nil , nil , true )
	return superDiscount
end
