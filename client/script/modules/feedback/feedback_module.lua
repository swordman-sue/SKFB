--[[
%% @module: 充值回馈
%% @author: yjg
%% @created: 2018年2月2日11:43:24
--]]


Modules = Modules or {}

Modules.FeedbackModule = Modules.FeedbackModule or BaseClass(BaseModule)

Modules.FeedbackModule.NO_MONEY = 0
Modules.FeedbackModule.RICH = 1
Modules.FeedbackModule.DRAW = 2

function Modules.FeedbackModule:__init()
end

function Modules.FeedbackModule:__delete()
end


-- 消息说明:请求充值回馈数据 消息Id: 261001
function Modules.FeedbackModule:RechargeRewardDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECHARGE_REWARD_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明:充值回馈数据响应 消息Id: 261002
-- "activity_data_id__H", --活动数据ID
-- "is_took_extra_reward__C", --是否已领取额外奖励
-- "daily_data_list__T__data@C", --每日数据列表(0=还没充值,1=已充值,2=已领取) 
function Modules.FeedbackModule:RechargeRewardDataResp( info )
	self.extra_reward = info.is_took_extra_reward
	self.activity_data_id = info.activity_data_id

	if not self.daily_data_list then
		self.daily_data_list = {}
	end

	for i , v in ipairs( info.daily_data_list ) do
		self:SetDailyDataList( i , v.data , true)
	end

	self:SetRechargeReward()

	GlobalEventSystem:FireNextFrame(Macros.Event.FeedbackModule.FEEDBACK_DATA)
end


-- 消息说明:领取充值奖励请求 消息Id: 261003
-- "day__H", --要领取的天数 
function Modules.FeedbackModule:TakeRechargeRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_RECHARGE_REWARD_REQ)
	data.day = info.day
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明:领取充值奖励响应 消息Id: 261004
-- "day__H", --要领取的天数
function Modules.FeedbackModule:TakeRechargeRewardResp( info )
	self:SetDailyDataList( info.day , Modules.FeedbackModule.DRAW )

	--大硕硕说显示全部获得 2-6
	local info = self:GetRechargeReward( info.day )
	if info then
		local t1 = {}
	    table.insert(t1,{id = info.reward_id , num = info.reward_num})
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.FeedbackModule.FEEDBACK_DATA)
end

-- 消息说明:领取额外奖励请求 消息Id: 261005
function Modules.FeedbackModule:TakeRechargeExtraRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_RECHARGE_EXTRA_REWARD_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明:领取额外奖励响应 消息Id: 261006
function Modules.FeedbackModule:TakeRechargeExtraRewardResp( info )
	self.extra_reward = 1

	local info = self:GetRechargeExtraReward( )
	if not info then
		return
	end

	--显示奖励
	local t1 = {}
    for i,v in ipairs( info.award ) do
        table.insert(t1,{id = v.id , num = v.num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	GlobalEventSystem:FireNextFrame(Macros.Event.FeedbackModule.FEEDBACK_DATA)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.FEEDBACK )
end

--设置每天的奖励状态
function Modules.FeedbackModule:SetDailyDataList( day , type_ , for_init )
	local text = ""
	if type_ == 0 then
		--开服天数
		local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")

		local day_ = day + self:GetExtraIncreaseTime()
		if time > day_ then
			-- 过期
			text = "Expired"
		elseif time < day_ then
			-- 未开
			text = "NoOpen"
		else
			-- 还没充值
			text = "NoMoney"
		end
	elseif type_ == 1 then
		-- 已充值
		text = "Rich"
	elseif type_ == 2 then
		-- 已领取
		text = "Draw"
	end

	if not self.daily_data_list then
		self.daily_data_list = {}
	end
	self.daily_data_list[ day ] = text

	if not for_init then
		--统计红点
		CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.FEEDBACK )
	end
end

--获取每天的奖励状态
function Modules.FeedbackModule:GetDailyDataList( day  )
	if not self.daily_data_list then
		self.daily_data_list = {}
	end
	return self.daily_data_list[ day ]
end

--获取特殊奖励
function Modules.FeedbackModule:GetExtraReward(  )
	return self.extra_reward or 0
end

--获取特殊奖励领取状态
function Modules.FeedbackModule:GetExtraRewardDrawType(  )
	local index = self:GetAccumulativePayDay()
	local cnf = self:GetRechargeExtraReward()
	if not cnf then
		return ""
	end
	local text = ""
	if index >= cnf.day then
		local type_ = self:GetExtraReward()
		if type_ == 0 then
			--可以领取
			text = "May"
		else
			--已经领取
			text = "Draw"
		end
	else
		--不能领取
		text = "Unable"
	end
	return text
end

--获得默认打开第几天
function Modules.FeedbackModule:GetSelectedDefault( )
	local index = 1
	local dap = false
	local lst = self:GetRechargeReward()
	if lst then
		for i , v in ipairs( lst ) do
			local type_ = self:GetDailyDataList( v.day )
			if type_ == "Rich" then
				index = v.day 
				dap = true
				break
			end
		end
	end

	if dap == false then
		local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
		local time_ = lst[#lst].day + self:GetExtraIncreaseTime()
		if time >= time_ then
			index = lst[#lst].day
		else
			index = time - self:GetExtraIncreaseTime()
		end
	end
	return index
end


--获得累计充值天数
function Modules.FeedbackModule:GetAccumulativePayDay()
	local index = 0
	local lst = self:GetRechargeReward()
	if lst then
		for i , v in ipairs( lst ) do
			local type_ = self:GetDailyDataList( v.day )
			if type_ == "Rich" or type_ == "Draw" then
				index = index + 1
			end
		end
	end
	return index
end

function Modules.FeedbackModule:GetFeedbackTime()
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.FEEDBACK )
	return info
end

function Modules.FeedbackModule:SetRechargeReward()
	local cnf = CPPGameLib.GetConfig("RechargeReward", nil , nil , true )
	if not cnf then
		return
	end
	if not cnf[self.activity_data_id] then
		return
	end
	cnf = cnf[self.activity_data_id].day_lst

	if not self.rechargeReward then
		self.rechargeReward = {}
	end
	for i , v in ipairs( cnf ) do
		self.rechargeReward[ v.day ] = {}
		self.rechargeReward[ v.day ].award = {}
		self.rechargeReward[ v.day ].recharge_id = v.recharge_id
		self.rechargeReward[ v.day ].day = v.day

		self.rechargeReward[ v.day ].reward_id = v.reward_id_list[1]
		self.rechargeReward[ v.day ].reward_num = v.reward_num_list[1]

		for ii , vv in ipairs( v.reward_id_list ) do
			local _ , cnf = ObjIDTool.GetInfo( vv )
			for iii , vvv in ipairs( cnf.item_id_list ) do
				table.insert( self.rechargeReward[ v.day ].award , { id = vvv , num = cnf.item_min_num_list[iii] } )
			end
		end
	end

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.FEEDBACK )
end

function Modules.FeedbackModule:GetRechargeReward( day )
	if self.rechargeReward then
		if day then
			return self.rechargeReward[day]
		else
			return self.rechargeReward
		end
	end
end

function Modules.FeedbackModule:GetRechargeExtraReward( day )
	day = day or 1
	local cnf = CPPGameLib.GetConfig("RechargeExtraReward", self.activity_data_id, false)
	if not cnf then
		return
	end
	cnf = cnf.id_lst[ day ]
	local t1 = {}
	t1.day = cnf.need_recharge_day
	t1.award = { }
	for i , v in ipairs( cnf.reward_id_list ) do
		table.insert( t1.award , { id = v , num = cnf.reward_num_list[i] } )
	end
	return t1
end

--获得额外增加时间
function Modules.FeedbackModule:GetExtraIncreaseTime( )
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.FEEDBACK )
	if not info or not info.star then
		return 0
	end

	local _ , time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
	return  math.ceil( ( info.star - time ) / 60 / 60 / 24 )
end

--检测奖励
function Modules.FeedbackModule:CheckReward(calc_red_num)
	local red_num = 0

    --充值奖励
   	local lst = self:GetRechargeReward()
   	if lst then
		for i , v in ipairs( lst ) do
			local type_ = self:GetDailyDataList(v.day )
			if type_ == "Rich" then
				if calc_red_num then
					red_num = red_num + 1
					if not Modules.Red.BaseRed.IS_SHOW_NUM then
						break
					end
				else
					return true
				end
			end
		end
	end

	--特殊奖励
	local reward = self:GetExtraRewardDrawType()
	if reward == "May" then
		if calc_red_num then
			red_num = red_num + 1
		else
			return true
		end
	end

	if calc_red_num then
		return red_num
	end
end
