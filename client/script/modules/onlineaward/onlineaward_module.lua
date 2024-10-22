--[[
%% @module: 在线奖励
%% @author: yjg
%% @created: 2018年1月20日11:03:11
--]]


Modules = Modules or {}

Modules.OnlineawardModule = Modules.OnlineawardModule or BaseClass(BaseModule)

function Modules.OnlineawardModule:__init()
	self:OnlineawardCnf()
end

function Modules.OnlineawardModule:__delete()
	if self.highRenewTime then
		GlobalTimerQuest:CancelQuest( self.highRenewTime )
		self.highRenewTime = nil
	end
end


-- 消息说明:在线奖励数据请求 消息Id: 25901
function Modules.OnlineawardModule:OnlineRewardDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:在线奖励数据请求 消息Id: 25901")  

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ONLINE_REWARD_DATA_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:在线奖励数据响应 消息Id: 25902
-- "online_time__I",   --在线时间(秒)
-- "reward_record_list__T__day@H##record_data@D|RecordData|"
function Modules.OnlineawardModule:OnlineRewardDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:在线奖励数据响应 消息Id: 25902")  
	self.online_time = info.online_time

	for i , v in ipairs( info.reward_record_list ) do
		local t1 = {}
		for ii , vv in ipairs( v.record_data:GetData() ) do
			table.insert(t1 , vv)
		end
		self:SetRewardRecordList( i , t1 )
	end

	self:Countdown()

	GlobalEventSystem:FireNextFrame(Macros.Event.OnlineawardModule.ONLINE_REWARD_DATA)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ONLINE_AWARD )
end

-- 消息说明:领取在线奖励请求 消息Id: 25903
function Modules.OnlineawardModule:TakeOnlineRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取在线奖励请求 消息Id: 25903") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_ONLINE_REWARD_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:领取在线奖励响应 消息Id: 25904
function Modules.OnlineawardModule:TakeOnlineRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取在线奖励响应 消息Id: 25904") 

	local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    local rewardRecordList = self:GetRewardRecordList( openClothing )
    local yetIndex = 0
    for i , v in ipairs( rewardRecordList ) do
        if v.GetType == "yet" then
            yetIndex = i
        end
    end

    local onlineawardCnf = self:GetOnlineawardCnf( openClothing )
    local lst = onlineawardCnf.info[ yetIndex + 1 ]
    local t1 = {}
    for i,v in ipairs(lst.reward) do
        table.insert(t1,{id = v.item , num = v.num})
    end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self:OnlineRewardDataReq()
end

--设置奖励领取列表
function Modules.OnlineawardModule:SetRewardRecordList( day , data )
	if not self.reward_record_list then
		self.reward_record_list = {}
	end

	-- data = {
	-- 	[1] = 0 , 
	-- 	[2] = 0 , 
	-- 	[3] = 0 , 
	-- 	[4] = 0 , 
	-- }

	local cnf = self:GetOnlineawardCnf( day )
	local timeLst = cnf.info
-- info = t1 , 
-- optimum = v.optimum , 
-- action = v.action , 
-- pos = v.pos ,
-- zoom = v.zoom ,
-- additional = v.additional ,
-- day = _ ,
	local t1 = {}
	for i , v in ipairs( data ) do
		local getType = ""
		if v.flag == 1 then
			--已领取
			getType = "yet"
		elseif v.flag == 0 then
			--需要在线的时间
			local time = timeLst[i].time * 60 
			if time <= self:GetOnlineTime() then
				if #t1 == 0 or ( t1[#t1] and t1[#t1].GetType == "yet" ) then
					--可以
					getType = "may"
				else
					--不行
					getType = "nein"
				end
			else
				--不行
				getType = "nein"
			end
		end
		--开服天数
		local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
		if day < openClothing and getType ~= "yet" then
			getType = "overdue"
		end
		table.insert( t1 , { GetType = getType } )
	end
	self.reward_record_list[ day ]  = t1
end

--获得奖励领取列表
function Modules.OnlineawardModule:GetRewardRecordList( day )
	if self.reward_record_list then
		if day then
			return self.reward_record_list[day]
		else
			return self.reward_record_list
		end
	else
		return nil
	end
end


--获得当前在线时间
function Modules.OnlineawardModule:SetOnlineTime( index )
	self.online_time = self.online_time + index
	return self.online_time
end

--获得当前在线时间
function Modules.OnlineawardModule:GetOnlineTime( ... )
	return self.online_time or 0
end

function Modules.OnlineawardModule:GetOnlineawardCnf( day )

	if not self.onlineawardCnf then
		return 
	end

	if day then
		return self.onlineawardCnf[day]
	else
		return self.onlineawardCnf
	end
end

function Modules.OnlineawardModule:OnlineawardCnf()
	
	if not self.onlineawardCnf then
		self.onlineawardCnf = {}
	else
		return
	end

	local cnf = CPPGameLib.GetConfig("Onlineaward", nil, nil , true )
	
	for _ , v in ipairs(cnf) do
		local t1 = {}
		for i , vv in ipairs( v.time ) do
			local t2 = {}
			for ii , vvv in ipairs( v.reward[i] ) do
				table.insert( t2 , { item = vvv , num = v.number[i][ii] } )
			end
			table.insert( t1 , { time = vv , reward = t2 } )
		end
		table.insert( self.onlineawardCnf , { 
												info = t1 , 
												optimum = v.optimum , 
												action = v.action , 
												pos = v.pos ,
												zoom = v.zoom ,
												additional = v.additional ,
												day = _ ,
												title = v.Title
												} )
	end
end

function Modules.OnlineawardModule:Countdown()
	local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
	local rewardRecordList = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetRewardRecordList" , openClothing )

	if not rewardRecordList then
		return 
	end

    local yetIndex = 0
    for i , v in ipairs( rewardRecordList ) do
        if v.GetType == "yet" then
            if yetIndex < i then
                yetIndex = i
            end
        elseif v.getType == "may" then
	        return
        end
    end
    
    local nextIndex = yetIndex + 1
    if not rewardRecordList[ nextIndex ] then
    	return
    end
    --如果他的下一个是未领取的话
    local time = 0
    if rewardRecordList[ nextIndex ].GetType == "nein" then
    	local onlineawardCnf = self:GetOnlineawardCnf( openClothing )
    	local lst = onlineawardCnf.info[ nextIndex ]
    	time = lst.time * 60
    end

	if self.highRenewTime then
		GlobalTimerQuest:CancelQuest( self.highRenewTime )
		self.highRenewTime = nil
	end

	if time - self:GetOnlineTime() <= 0 then
		GlobalEventSystem:FireNextFrame( Macros.Event.OnlineawardModule.ONLINE_COUNTDOWN , 0 )
		return 
	end

	self.highRenewTime = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		if time > self:GetOnlineTime() then
			self:SetOnlineTime( 1 )
		else
			if self.highRenewTime then
				GlobalTimerQuest:CancelQuest( self.highRenewTime )
				self.highRenewTime = nil
				self:OnlineRewardDataReq()
			end
		end
		GlobalEventSystem:FireNextFrame( Macros.Event.OnlineawardModule.ONLINE_COUNTDOWN , time - self:GetOnlineTime() )
	end),1 ,-1)
end

function Modules.OnlineawardModule:CheckOnlineReward(calc_red_num)
	local red_num = 0

    local role_create_time = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    local lst = CallModuleFunc(ModuleType.ONLINE_AWARD , "GetRewardRecordList" , role_create_time )
    if lst then
	    for i , v in ipairs( lst ) do
	        if v.GetType == "may" then
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

	if calc_red_num then
		return red_num
	end
end