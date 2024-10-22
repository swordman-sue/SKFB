--
-- @brief PVP模块 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

Modules = Modules or {}

Modules.PVPModule = Modules.PVPModule or BaseClass(BaseModule)

function Modules.PVPModule:__init()
	self.iconlst = {}
	for _ , v in pairs( CPPGameLib.GetConfig("HeroHandBook",nil,nil,true) ) do
		for __,vv in pairs(v.hero_list) do
			for ___,vvv in pairs(vv.hero_list ) do
				if vv.quality > 2 then
					local cnf = CPPGameLib.GetConfig("Hero", vvv )
					table.insert( self.iconlst , cnf.head_icon )
				end
			end
		end
	end
end

function Modules.PVPModule:__delete()
end

-- 消息说明: 领取挑战奖励 消息Id: 24419
function Modules.PVPModule:PVPTakeBattleRewardReq(info)
	-- CPPGameLib.PrintTable("xy", info ,"消息说明: 领取挑战奖励 消息Id: 24419")  
	local date = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_TAKE_BATTLE_REWARD_REQ)
	date.type = info.type
	date.take_reward_list = info.take_reward_list
	GlobalNetAdapter:OnSend(date) 
end

-- 消息说明: 领取挑战奖励响应 消息Id: 24420
function Modules.PVPModule:PVPTakeBattleRewardResp(info)
	CPPGameLib.PrintTable("xy", info ,"消息说明: 领取挑战奖励响应 消息Id: 24420")  
	if info.type == Macros.Game.PVP_REWARD_TYPE.DAY then
		for i , v in ipairs( info.take_reward_list ) do
			self.playerPVPData.daily_battle_reward_record_list[v.times] = v.times
		end
	elseif info.type == Macros.Game.PVP_REWARD_TYPE.WEEK then
		for i , v in ipairs( info.take_reward_list ) do
			self.playerPVPData.week_battle_reward_record_list[v.times] = v.times
		end
	end

	local t1 = {}
	for i,v in ipairs( info.take_reward_list ) do
		CPPGameLib.PrintTable("xy", v ,"物品合成响应")  
		local cnf =  CPPGameLib.GetConfig("PVP_BattleTimesReward", v.times )
		for i , v in ipairs( cnf.item_id_list ) do
			table.insert(t1,{id = v , num = cnf.item_num_list[i] })
		end
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.PVPModule.PVP_AWARD)
end

-- 消息说明: 玩家PVP数据请求 消息Id: 24421
function Modules.PVPModule:PlayerPVPDataReq(info)
	-- CPPGameLib.PrintTable("xy", info ,"消息说明: 玩家PVP数据请求 消息Id: 24421")  
	local date = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLAYER_PVP_DATA_REQ)
	GlobalNetAdapter:OnSend(date) 
end

-- 消息说明: 玩家PVP数据响应 消息Id: 24422
function Modules.PVPModule:PlayerPVPDataResp(info)
	CPPGameLib.PrintTable("xy", info ,"消息说明: 玩家PVP数据响应 消息Id: 24422")  
	self.playerPVPData = info

	local t1 = {}
	for i , v in ipairs( info.daily_battle_reward_record_list ) do
		t1[v.times] = v.times
	end
	self.playerPVPData.daily_battle_reward_record_list = t1

	local t1 = {}
	for i , v in ipairs( info.week_battle_reward_record_list ) do
		t1[v.times] = v.times
	end
	self.playerPVPData.week_battle_reward_record_list = t1

	self:FireNextFrame(Macros.Event.PVPModule.PVP_DATA)
end

-- 消息说明: 取消查找目标请求 消息Id: 24425
function Modules.PVPModule:PVPCancelSearchTargetReq( info )
	CPPGameLib.PrintTable("xy", info ,"消息说明: 取消查找目标请求 消息Id: 24425")  
	local date = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_CANCEL_SEARCH_TARGET_REQ)
	GlobalNetAdapter:OnSend(date) 
end

-- 消息说明: 取消查找目标响应 消息Id: 24426
function Modules.PVPModule:PVPCancelSearchTargetResp( info )
	CPPGameLib.PrintTable("xy", info ,"消息说明: 取消查找目标请求 消息Id: 24425")  
	if info.is_in_battle == 0 then
		self:FireNextFrame(Macros.Event.PVPModule.PVP_CANCEL)
	end
end


--排行榜数据响应
function Modules.PVPModule:PVPRankResp( info )
	CPPGameLib.PrintTable("xy", info ,"排行榜数据响应")  
	self:FireNextFrame(Macros.Event.PVPModule.PVP_RANK,info)
end

--上周排名
function Modules.PVPModule:GetLastWeek( )
	return self.playerPVPData.last_week_rank
end

--获得排行榜数据
function Modules.PVPModule:GetRankList( )
	local t1 = self.playerPVPData.rank_list
	for i = 1 , 3 - #t1  do
		table.insert( self.playerPVPData.rank_list , { role_name = CPPGameLib.GetString("pvp_noOne") , score = 0 } )
	end
	return self.playerPVPData.rank_list , self.playerPVPData.my_rank , self.playerPVPData.my_score
end

--本日挑战次数
function Modules.PVPModule:GetDayBattleTimes( )  
	local lst = self:GetBattleTimesReward( Macros.Game.PVP_REWARD_TYPE.DAY )
	local max = 0
	for i , v in ipairs( lst ) do
		max = v.times
		if v.times >= self.playerPVPData.today_battle_times then
			break
		end
	end
	return self.playerPVPData.today_battle_times , max
end

--本周挑战次数
function Modules.PVPModule:GetWeekBattleTimes( )  
	local lst = self:GetBattleTimesReward( Macros.Game.PVP_REWARD_TYPE.WEEK )
	local max = 0
	for i , v in ipairs( lst ) do
		max = v.times
		if v.times >= self.playerPVPData.week_battle_times then
			break
		end
	end
	return self.playerPVPData.week_battle_times , max
end

--获得今天还可获得的奖励次数
function Modules.PVPModule:GetPVPAwardTime( )  
	local num = ConfigAdapter.Common.GetPVPAwardTime() - self.playerPVPData.today_battle_times
	if ConfigAdapter.Common.GetPVPAwardTime() - self.playerPVPData.today_battle_times <= 0 then
		num = 0  
	end
	return num
end

--已经领取
function Modules.PVPModule:GetRecord( type , id )  
	if type == Macros.Game.PVP_REWARD_TYPE.DAY then
		return self.playerPVPData.daily_battle_reward_record_list[id]
	elseif type == Macros.Game.PVP_REWARD_TYPE.WEEK then
		return self.playerPVPData.week_battle_reward_record_list[id]
	end
end

--获得挑战奖励
function Modules.PVPModule:GetBattleTimesReward( id )  
	local t1 = {}
	local t3 = {}
	for k , v in pairs( CPPGameLib.GetConfig("PVP_BattleTimesReward", nil, nil , true ) ) do
		if id == v.type then
			local t2 = {}
			for i , vv in ipairs( v.item_id_list ) do
				table.insert( t2 , { id = vv , num = v.item_num_list[i] } )
			end
			local record = self:GetRecord( v.type , v.times )
			if record then
				table.insert( t3 , { times = v.times , type = v.type , item = t2 } )
			else
				table.insert( t1 , { times = v.times , type = v.type , item = t2 } )
			end
		end
	end

	table.sort( t1 , function ( a , b )
		return a.times < b.times
	end )

	table.sort( t3 , function ( a , b )
		return a.times < b.times
	end )

	for i,v in ipairs(t3) do
		table.insert( t1 , v )
	end

	return t1	
end

--获得排名奖励
function Modules.PVPModule:GetRankReward(  )  
	local t1 = {}
	for k , v in pairs( CPPGameLib.GetConfig("PVP_RankReward", nil, nil , true ) ) do

		local text = v.rank_min 
		if v.rank_min ~= v.rank_max then
			text = v.rank_min .."~" .. v.rank_max
		end

		local t2 = {}
		for i , vv in ipairs( v.item_id_list ) do
			table.insert( t2 , { id = vv , num = v.item_num_list[i] } )
		end

		table.insert( t1 , { text = text , item = t2 , title_id = title_id , sort = k , title_id = v.title_id } )
	end

	table.sort( t1, function ( a , b )
		return a.sort < b.sort
	end )

    for i , v in ipairs( t1 ) do
		local show = true
		if i%2 == 0 then
			show = false
		end 
		v.show = show
    end

    if t1[#t1].show == false then
    	table.insert( t1 , { show = true } )
    end

	return t1	
end


--随机名字
function Modules.PVPModule:NameRandom()
	--表
	local lst = CPPGameLib.GetConfig("RobotNameList", nil, nil , true )

	local function Group( ... )
		local key = {}
		local part = MathTool.GetRandom( 1 , #lst )
		if lst[part].first_name then
			key[1] = lst[part].first_name
		else
			key[1] = ""
		end

		local part = MathTool.GetRandom( 1 , #lst )
		if lst[part].middle_name then
			key[2] = lst[part].middle_name
		else
			key[2] = ""
		end

		local part = MathTool.GetRandom( 1 , #lst )
		if lst[part].last_name then
			key[3] = lst[part].last_name
		else
			key[3] = ""
		end
		return key
	end

	local num = 2
	local text
	while num >= 2 do
		num = 0
		text = ""
		local lst = Group()
		for __ , v in ipairs(lst) do
			if v == "" then
				num = num + 1
			else
				text = text .. v
			end
		end

		local _, is_pass = LanguageFilter.NameFilter(text)
		if not is_pass then
			num = 2
		end
	end

	return text
end

--获得随机名字和随机图标
function Modules.PVPModule:GetRandomInfo(  )  
	local i = MathTool.GetRandom( 1 , #self.iconlst )
	return self.iconlst[i] , self:NameRandom()
end

--获得一键的列表
function Modules.PVPModule:GetOneKeyLst( type )
	local lst = self:GetBattleTimesReward( type )
	local t1 = {}
	for i , v in ipairs( lst ) do
		local record = CallModuleFunc( ModuleType.PVP , "GetRecord" , v.type , v.times )

		local num = nil
	    if v.type == Macros.Game.PVP_REWARD_TYPE.DAY then
	    	num = CallModuleFunc( ModuleType.PVP , "GetDayBattleTimes" )
	    elseif v.type == Macros.Game.PVP_REWARD_TYPE.WEEK then
	    	num = CallModuleFunc( ModuleType.PVP , "GetWeekBattleTimes" )
	    end
	    if num > v.times then
	    	num = v.times
	    end

		--可领取
		if num >= v.times and not record then
			table.insert( t1 , { times = v.times } )
		end
	end
	return t1
end


--保存随机名字
function Modules.PVPModule:SaveRandomInfo( name )
	self.saveRandomInfo = name
end

--获取随机名字
function Modules.PVPModule:GetSaveRandomInfo( )
	return self.saveRandomInfo
end