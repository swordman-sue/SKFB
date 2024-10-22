--[[
%% @module: 转盘
%% @author: yjg
%% @created: 2018年3月16日15:18:25
--]]

Modules = Modules or {}

Modules.TurntableModule = Modules.TurntableModule or BaseClass(BaseModule)

function Modules.TurntableModule:__init()
end

function Modules.TurntableModule:__delete()
	if self.countdown then
		GlobalTimerQuest:CancelQuest( self.countdown )
		self.countdown = nil
	end
end


-- 消息说明:转盘数据请求 消息Id: 26601
function Modules.TurntableModule:TurntableDataReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明:转盘数据请求 消息Id: 26601")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TURNTABLE_DATA_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:转盘数据响应 消息Id: 26602
-- "cur_rank_score__I",    --当前排行积分
-- "cur_activity_data_id__H",  --当前活动数据ID
-- "diamond__I",   --钻石
-- "lottery_record_list__T__time@I##index@H##context@I##role_name@s", --抽奖记录列表(抽奖时间戳, 数据下标,上下文[物品数量,钻石],角色名称)
function Modules.TurntableModule:TurntableDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:转盘数据响应 消息Id: 26602")
	-- 当前活动数据ID
	self.cur_activity_data_id = info.cur_activity_data_id
	--钻石
	self.diamond = info.diamond
	--抽奖记录列表
	self.lottery_record_list = info.lottery_record_list
	--当前排行积分
	self.cur_rank_score = info.cur_rank_score

	GlobalEventSystem:FireNextFrame(Macros.Event.TurntableModule.TURNTABLE_DATA)
end

-- 消息说明:转盘抽奖请求 消息Id: 26603
-- lottery_type__H
function Modules.TurntableModule:TurntableLotteryReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明:转盘抽奖请求 消息Id: 26603")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TURNTABLE_LOTTERY_REQ)
	data.lottery_type = info.lottery_type
	data.is_cost_item = info.is_cost_item or 1
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:转盘抽奖响应 消息Id: 26604
-- "lottery_type__H",  --抽奖类型(0=普通抽奖,1=一键抽奖)
-- "lottery_result_list__T__index@H##context@I", -- 抽奖结果列表(数据下标,上下文[物品数量,钻石])
function Modules.TurntableModule:TurntableLotteryResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明:转盘抽奖响应 消息Id: 26604")
	self.lottery_result_list = info.lottery_result_list

	GlobalEventSystem:FireNextFrame(Macros.Event.TurntableModule.WHIRL_DATA)
end

-- 消息说明:转盘排行榜请求 消息Id: 
function Modules.TurntableModule:TurntableRankReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明:转盘排行榜请求 消息Id: ")

	local info = {}
	info.type = Macros.Game.RankType.RANK_TURNTABLE_LOTTERY
	CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
end

-- 消息说明:转盘排行榜 消息Id: 
-- "type__C",   --排行榜类型
-- "sub_type__C",  --排行榜子类型(目标只有世界BOSS排行用上)
-- "rank__H",  --排名(自己的)
-- "rank_value__I",--排名值(自己的)
function Modules.TurntableModule:TurntableRankResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明:转盘排行榜响应 消息Id: ")
	--排名(自己的)
	self.rank = info.rank
	--排名列表
	self.rank_list = info.rank_list 
end


--获得活动ID
function Modules.TurntableModule:GetActivityId()
	return self.cur_activity_data_id or 0
end

--获得活动时间
function Modules.TurntableModule:GetActivityTime()
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.TURNTABLE )
	if info and info.reward then
		if not self.countdown then
			self.countdown = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
				if info.reward - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") <= 0 then
					--活动结束
					if self.countdown then
						GlobalTimerQuest:CancelQuest( self.countdown )
						self.countdown = nil
					end
					CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.TURNTABLE)
				end
			end),1 ,-1)
		end
	end
	return info
end

--获得活动结算时间
function Modules.TurntableModule:GetActivityEndTime()
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.TURNTABLE )
	if info and info.reward then
		return info.reward
	end
end

--获得我的排名
function Modules.TurntableModule:GetMyRank()
	return self.rank or 0
end

--获得我的排名奖励
function Modules.TurntableModule:GetMyRankAward()
	local info = nil
	return info
end

--获得当前积分
function Modules.TurntableModule:GetNowIntegral()
	return self.cur_rank_score
end

--获得转盘奖池数量
function Modules.TurntableModule:SeturntableQuantity( num )
	self.diamond = num
end

--获得转盘奖池数量
function Modules.TurntableModule:GeturntableQuantity()
	return self.diamond or 0
end

--获得转盘奖池
function Modules.TurntableModule:GetTurntablePond()
	local t1 = self:GetTurntableLotteryCnf()
	if not t1 then
		return {}
	end
	table.sort( t1, function ( a , b )
 		return a.index < b.index
	end )
	return t1
end

--获得转盘奖池
function Modules.TurntableModule:GetRecordList()
	local t1 = {}
	self.lottery_record_list = self.lottery_record_list or {}
	for i , v in ipairs( self.lottery_record_list ) do
		local _ , cnf = self:GetTurntableLotteryCnf( v.index )
		--有道具id
		local _ , name , info = nil , "" , nil
		info = {}
		if cnf.item_id then
		 	_ , info = ObjIDTool.GetInfo( cnf.item_id )
		 	name = info.name
		else
		    local cnf = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.DIAMOND )
		    name = cnf.name
		end

		table.insert( t1 , { time = v.time , text = name .. "*" .. v.context , role_name = v.role_name , quality = info.quality or nil } )
	end

	table.sort( t1 , function ( a , b )
		return a.time < b.time
	end )
	return t1
end

--获得排行榜
function Modules.TurntableModule:GetRank()
	self.rank_list = self.rank_list or {}
	local t1 = {}
	for i , v in ipairs( self.rank_list ) do
		table.insert( t1 , { icon = v.head_icon , name = v.role_name , rank = i , level = v.level , power = v.battle_value , integral = v.context } )
	end
	return t1
end

--获得选中奖励
-- "lottery_result_list__T__index@H##context@I", -- 抽奖结果列表(数据下标,上下文[物品数量,钻石])
function Modules.TurntableModule:GetSelected()
	local t1 = {}

	self.lottery_result_list = self.lottery_result_list or {}

	for i , v in ipairs( self.lottery_result_list ) do
		table.insert( t1 , { id = v.index , num = v.context } )
	end
	return t1
end

--获得奖励表
function Modules.TurntableModule:GetTurntableRankCnf()
	local id = self:GetActivityId()
	local cnf = CPPGameLib.GetConfig("TurntableRank", id , false )
	if not cnf then
		return
	end

	local t1 = {}
	for i , v in pairs( cnf.rank_list ) do
		local t2 = {}
		for j , vv in ipairs( v.item_id_list ) do
			table.insert( t2 , { id = vv , num = v.item_num_list[j] } )
		end		

		local rank = v.rank_min
		if v.rank_min ~= v.rank_max then
			rank = v.rank_min .."~".. v.rank_max
		end
		table.insert( t1 , { rank = rank , award = t2 , sort = v.rank_min } )
	end

	table.sort( t1 , function ( a , b )
 		return a.sort < b.sort
	end )

	for i , v in ipairs( t1 ) do
		v.show = i%2~=0
	end
	return t1
end

--获得转盘抽奖表
function Modules.TurntableModule:GetTurntableLotteryCnf( index )
	local id = self:GetActivityId()	
    local cnf = CPPGameLib.GetConfig("TurntableLottery", id , false )

	if not cnf then
		return 
	end

    local t1 = {}
    for i , v in ipairs( cnf.data_list ) do

    	if v.item_id then
	    	local text = v.item_num_min
	    	if v.item_num_min ~= v.item_num_max then
	    		text = v.item_num_min .. "~" .. v.item_num_max 
		    end
	    	table.insert( t1 , { item_id = v.item_id , num = text , index = i } ) 
    	else
    		local cnf = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.DIAMOND )
    		table.insert( t1 , { id = cnf.big_icon , num = string.format( "%d" , v.diamon_rate/100 ) , index = i , isCurrency = true } ) 
	    end
	    if v.icon_effects then
		    t1[#t1].icon_effects = v.icon_effects
	    end 
    end

    local t2 = {}
    if index then
    	t2 = cnf.data_list[ index ]
    end

    return t1 , t2
end

--获得转盘配置表
function Modules.TurntableModule:GetTurntableCnf()
	local id = self:GetActivityId()
	local cnf = CPPGameLib.GetConfig("Turntable", id , false )
	if not cnf then
		return
	end
	return cnf
end

--获得转盘配置表
function Modules.TurntableModule:SetNowNotShow( bool )
	self.nowNotShow = bool
end

--获得转盘配置表
function Modules.TurntableModule:GetNowNotShow()
	return self.nowNotShow
end