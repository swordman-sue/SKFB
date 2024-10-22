--[[
%% @module: 自定义超值购
%% @author: yjg
%% @created: 2018年2月24日17:02:25
--]]


Modules = Modules or {}

Modules.CustomPurchaseModule = Modules.CustomPurchaseModule or BaseClass(BaseModule)

function Modules.CustomPurchaseModule:__init()
end

function Modules.CustomPurchaseModule:__delete()
end

-- 消息说明:超值折扣数据请求 消息Id: 26301
function Modules.CustomPurchaseModule:SuperDiscountDataReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 超值折扣数据请求: 消息Id: 26301")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_DATA_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:超值折扣数据响应 消息Id: 26302
-- "status__H",                --购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
-- "take_accumulate_reward_times__H",  --已领取累积奖励的次数
-- "cur_buy_time__H",          --当前购买次数(领取奖励时会重置)
function Modules.CustomPurchaseModule:SuperDiscountDataResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 超值折扣数据响应: 消息Id: 26302")
	self.status = info.status
	self.take_accumulate_reward_times = info.take_accumulate_reward_times
	self.cur_buy_time = info.cur_buy_time

	GlobalEventSystem:FireNextFrame(Macros.Event.CustomPurchaseModule.CUSTOM_PURCHASE_DATA)
end

-- 消息说明:领取超值折扣购买奖励请求 消息Id: 26303
function Modules.CustomPurchaseModule:SuperDiscountBuyRewardReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 领取超值折扣购买奖励请求: 消息Id: 26303")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_BUY_REWARD_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:领取超值折扣购买奖励响应 消息Id: 26304
function Modules.CustomPurchaseModule:SuperDiscountBuyRewardResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 领取超值折扣购买奖励响应: 消息Id: 26304")

	local cnf = self:GetNewSuperDiscountBuyRewardCnf( self:GetOpenType() )
	local info = cnf[ self:GetActivityHowManyDays() ].lst
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , info )

	self.status = 2

	GlobalEventSystem:FireNextFrame(Macros.Event.CustomPurchaseModule.DISCOUNT_BUY_REWARD)
end

-- 消息说明:领取超值折扣累积奖励请求 消息Id: 26305
function Modules.CustomPurchaseModule:SuperDiscountAccumulateRewardReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 领取超值折扣购买奖励响应: 消息Id: 26305")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:领取超值折扣累积奖励响应 消息Id: 26306
function Modules.CustomPurchaseModule:SuperDiscountAccumulateRewardResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 领取超值折扣累积奖励响应: 消息Id: 26306")

	local info = self:GetNewSuperDiscountAccumulateRewardCnf( self:GetOpenType() , self:GetTakeAccumulateRewardTimes() )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取超值折扣累积奖励响应: 消息Id: 26306")
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , info.lst )

	self.take_accumulate_reward_times = self.take_accumulate_reward_times + 1
	
	GlobalEventSystem:FireNextFrame(Macros.Event.CustomPurchaseModule.DISCOUNT_BUY_REWARD)
end




--购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
function Modules.CustomPurchaseModule:GetStatus( )
	print("xy" , "==============---=-=？" , self.status )

	local type_ = nil 
	if self.status == 0 then
		type_ = "weiGouMai"
	elseif self.status == 1 then
		type_ = "keLingQu"
	elseif self.status == 2 then
		type_ = "yiLingQu"
	end

	return type_
end

--已领取累积奖励的次数
function Modules.CustomPurchaseModule:GetTakeAccumulateRewardTimes( )
	return self.take_accumulate_reward_times
end

--当前购买次数(领取奖励时会重置)
function Modules.CustomPurchaseModule:GetCurBuyTime( )
	return self.cur_buy_time
end

--获得自定义超值购表
function Modules.CustomPurchaseModule:GetCustomPurchaseCnf( id )
	local cnf = CPPGameLib.GetConfig("Custompurchase", id )
	return cnf
end

--获得购买奖励(新)表
function Modules.CustomPurchaseModule:GetNewSuperDiscountBuyRewardCnf( id )
	local cnf = CPPGameLib.GetConfig("NewSuperDiscountBuyReward", id )

	local t1 = {}
	for _ , v in ipairs( cnf.day_list ) do
		local t2 = {}
		for ii , vv in ipairs( v.item_id_list ) do
			table.insert( t2 , { id  = vv , num = v.item_num_list[ii] } )
		end
		table.insert( t1 , { lst = t2 , pay_id = v.pay_id  , day = v.day } )
	end

	return t1
end

--累积奖励(新)表
function Modules.CustomPurchaseModule:GetNewSuperDiscountAccumulateRewardCnf( id , time )
	CPPGameLib.PrintTable("skfb" , id , "NewSuperDiscountAccumulateReward--------") 
	local cnf = CPPGameLib.GetConfig("NewSuperDiscountAccumulateReward", id )
	CPPGameLib.PrintTable("skfb" , cnf , "NewSuperDiscountAccumulateReward--------")  

	local t2 = {}
	for i , v in ipairs( cnf.times_list ) do
		local t1 = {}
		for ii , vv in ipairs( v.item_id_list ) do
			table.insert( t1 , { id = vv , num = v.item_num_list[ii] } )
		end
		t2[ v.times ] = { lst = t1 , need_buy_times = v.need_buy_times }
	end

	return t2[ time ] , t2
end

--获得今天是活动第几天
function Modules.CustomPurchaseModule:GetActivityHowManyDays( )
	    CPPGameLib.PrintTable("xy" , self.openType , "GetActivityHowManyDays----------") 
	local type_ = Macros.Game.SystemID.CUSTOM_PURCHASE
	local info  = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , type_ )
	--开始时间
	local star = info.star
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(star)
	--第一天
	star = star - ( hour * 60 * 60 ) - ( minute * 60 ) - second
	local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

	print("xy" , "math.ceil( ( n_Time - star ) / 24 / 60 / 60 )" , math.ceil( ( n_Time - star ) / 24 / 60 / 60 ) )
	return math.ceil( ( n_Time - star ) / 24 / 60 / 60 )
end

--设置现在打开的界面类型
function Modules.CustomPurchaseModule:SetOpenType( type_ )
	self.openType = type_
end

--获得现在打开的界面类型
function Modules.CustomPurchaseModule:GetOpenType( )
	CPPGameLib.PrintTable("xy" , self.openType , "GetOpenType")  
	return self.openType
end

