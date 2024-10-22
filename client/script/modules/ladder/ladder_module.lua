--[[
%% @module: 天梯赛
%% @author: yjg
%% @created: 2017年10月28日18:35:08
--]]


Modules = Modules or {}

Modules.LadderModule = Modules.LadderModule or BaseClass(BaseModule)

function Modules.LadderModule:__init()
	self.kingMatchData = {}
end

function Modules.LadderModule:__delete()
end


-- 消息说明:请求玩家王者赛数据 消息Id: 25601
function Modules.LadderModule:KingMatchDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求玩家王者赛数据 消息Id: 25601") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_KING_MATCH_DATA_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:玩家王者赛数据响应 消息Id: 25602
function Modules.LadderModule:KingMatchDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:玩家王者赛数据响应 消息Id: 25602") 
	-- S2C_KING_MATCH_DATA_RESP
	self.kingMatchData = info

	if not self.taskRecord then
		self.taskRecord = {}
	end

	for i , v in ipairs( info.season_task_reward_record ) do
		self.taskRecord[ v.win_times ] = true
	end

	GlobalEventSystem:FireNextFrame( Macros.Event.LadderModule.LADDER )
end



-- 消息说明:查找目标请求 消息Id: 25603
function Modules.LadderModule:KingMatchSearchTargetReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:查找目标请求 消息Id: 25603") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_KING_MATCH_SEARCH_TARGET_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:查找目标响应 消息Id: 25604
function Modules.LadderModule:KingMatchSearchTargetResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:查找目标响应 消息Id: 25604") 
	self.matchSearchTarget = info

	GlobalEventSystem:FireNextFrame( Macros.Event.LadderModule.LADDER_VS )
end

-- 消息说明:领取赛季任务奖励请求 消息Id: 25605
function Modules.LadderModule:TakeKingMatchTaskRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取赛季任务奖励请求 消息Id: 25605") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_KING_MATCH_TASK_REWARD_REQ)
	data.win_times = info.win_times
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:领取赛季任务奖励响应 消息Id: 25606
function Modules.LadderModule:TakeKingMatchTaskRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取赛季任务奖励响应 消息Id: 25606")

	local cnf = CPPGameLib.GetConfig("LadderTask", info.win_times )
	local t1 = {}
    for i,v in ipairs(cnf.award_id) do
        table.insert(t1,{id = v , num = cnf.award_num[i] })
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self.taskRecord[info.win_times] = true

    GlobalEventSystem:FireNextFrame( Macros.Event.LadderModule.LADDER_TASK )
end

-- 消息说明:赛季排行榜请求 消息Id: 25607
function Modules.LadderModule:KingMatchSeasonRankReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:赛季排行榜请求 消息Id: 25607")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_KING_MATCH_SEASON_RANK_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:赛季排行榜响应(待完善...) 消息Id: 25608
function Modules.LadderModule:KingMatchSeasonRankResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:赛季排行榜响应(待完善...) 消息Id: 25608")
	self.matchSeason = {}

	for i , v in ipairs(info.season_rank) do
		local data = v.rank_list:GetData( )
		table.insert( self.matchSeason , { lst = data , season = v.season } )
	end

	table.sort( self.matchSeason, function ( a , b )
		return a.season < b.season
	end )

	GlobalEventSystem:FireNextFrame( Macros.Event.LadderModule.LADDER_HONOR )	
end 

-- 消息说明:天梯排行榜
function Modules.LadderModule:LadderRankReq( info )
	local info = {}
	info.type = Macros.Game.RankType.LADDER
	CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
end

-- 消息说明:天梯排行榜响应
function Modules.LadderModule:LadderRankResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:天梯排行榜响应")
	self.rank_list = info.rank_list
	GlobalEventSystem:FireNextFrame( Macros.Event.LadderModule.LADDER_RANK )
end

--获得当前赛季
function Modules.LadderModule:GetNowSeason()
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(serverTime)

	return self.kingMatchData.cur_season or 0 , n_nian
end

--获得我的场次和胜率
function Modules.LadderModule:GetMyWinning()
	local max = self.kingMatchData.total_battle_times or 0
	local odds = 0
	if max ~= 0 then
		local win = self.kingMatchData.win_times or 0
		odds = win/max*100
	end
	return max , math.floor( odds + 0.5 )
end

--获得我的排名和积分
function Modules.LadderModule:GetMyGrid()
	return self.kingMatchData.my_rank or 0 , self.kingMatchData.my_rank_value or 0
end

--获得我的段位和星数
function Modules.LadderModule:GetMyRank()
	return self.kingMatchData.level or 0 , self.kingMatchData.star or 0
end

--获取我的历史段位
function Modules.LadderModule:GetHistoryRank()
	return self.kingMatchData.history_level or 0
end

--获得赛季奖励
function Modules.LadderModule:GetSeasonAward()
	local cnf = CPPGameLib.GetConfig("LadderDan", self:GetMyRank(), false )

	if cnf and cnf.type ~= Macros.Game.LADDER_LEVEL.LEVEL_7 then
		print("xy" , "===================1")
		local t1 = {}
		t1.lst = {}
		for i , v in ipairs( cnf.close_award_id ) do
			table.insert( t1.lst , { info_id = v , item_num = cnf.close_award_num[i] , sc = 0.8 } )
		end
		t1.season = true
		return t1
	elseif cnf and cnf.type == Macros.Game.LADDER_LEVEL.LEVEL_7 then

		local rank_ , value = self:GetMyGrid()
		local lst = self:GetKingAward(rank_)

		local t1 = {}
		t1.lst = {}
		for i , v in ipairs( lst.award_id ) do
			table.insert( t1.lst , { info_id = v , item_num = lst.award_num[i] , sc = 0.8 } )
		end
		t1.season = true

		return t1
	else
		local cnf = CPPGameLib.GetConfig("LadderDan", 1 , false )
		local t1 = {}
		t1.lst = {}
		for i , v in ipairs( cnf.close_award_id ) do
			table.insert( t1.lst , { info_id = v , item_num = cnf.close_award_num[i] , sc = 0.8 } )
		end
		t1.noSeason = true
		return t1
	end
end

--是否首胜
function Modules.LadderModule:GetIsWinning()
	if self.kingMatchData.today_win_times and self.kingMatchData.today_win_times ~= 0 then
		return false
	else
		return true
	end
end

--是否首胜 返回晋级奖励和首胜奖励
function Modules.LadderModule:GetIsHaveWinning()
	--如果有首胜的话
	if self:GetIsWinning() then
		local cnf = CPPGameLib.GetConfig("LadderDan", self:GetMyRank() , false )
		if cnf then
			local t1 = {}
			t1.lst = {}
			for i , v in ipairs( cnf.first_award_id ) do
				table.insert( t1.lst , { info_id = v , item_num = cnf.first_award_num[i] , sc = 0.8 } )
			end
			t1.winning = true
			return t1
		else
			print("xy" , "这个段位没有首胜？" , self:GetMyRank() )
			if self:GetMyRank() <= 0 then
				local cnf = CPPGameLib.GetConfig("LadderDan", self:GetMyRank() , false )
				local t1 = {}
				t1.lst = {}
				for i , v in ipairs( cnf.first_award_id ) do
					if v ~= 0 then
						table.insert( t1.lst , { info_id = v , item_num = cnf.first_award_num[i] , sc = 0.8 } )
					end
				end
				t1.winning = true
				return t1
			else
				print("xy" , "这个是什么段位？" , self:GetMyRank() )
			end
		end
	end

	--不然只有晋级
	local cnf = CPPGameLib.GetConfig("LadderDan", self:GetMyRank() , false )
	local t1 = {}
	t1.lst = {}
	if cnf and cnf.type ~= Macros.Game.LADDER_LEVEL.LEVEL_7 then
		for i , v in ipairs( cnf.promotion_award_id ) do
			if v ~= 0 then
				table.insert( t1.lst , { info_id = v , item_num = cnf.promotion_award_num[i] , sc = 0.8 } )
			end
		end
		t1.promotion = true
	elseif cnf and cnf.type == Macros.Game.LADDER_LEVEL.LEVEL_7 then
		local cnf = CPPGameLib.GetConfig("LadderDan", self:GetMyRank() - 1 , false )
		for i , v in ipairs( cnf.promotion_award_id ) do
			if v ~= 0 then
				table.insert( t1.lst , { info_id = v , item_num = cnf.promotion_award_num[i] , sc = 0.8 } )
			end
		end
		t1.promotion = true
		t1.max = true
	else
		print("xy" , "这个段位没有晋级？" , self:GetMyRank() )
	end	

	return t1
end

--获得历史最强
function Modules.LadderModule:GetHistoryMost()
	local t1 = {}

	if not self.matchSeason then
		self.matchSeason = {}
	end

	for i , v in ipairs( self.matchSeason ) do
		local t2 = {}
		for ii , vv in ipairs( v.lst ) do
			table.insert( t2 , { name = vv.role_name , icon = vv.head_icon , server = vv.server_id , titleId = vv.title_id } )			
		end
		table.insert( t1 , { lst = t2  , sort = v.season } )
	end

	if #t1 <= 0 then
		table.insert( t1 , { typeNil = true } )
	end

	return t1
end

--获得历史排行榜列表
function Modules.LadderModule:GetMatchSeason()
	return self.matchSeason
end

--获得最强
function Modules.LadderModule:GetMost()

	local t1 = {}
	for i , v in ipairs( self.rank_list ) do
		t1 = { 
				info_id = v.role_id , 
				rank = i , 
				head_id = v.head_icon , 
				name = v.role_name , 
				power = v.battle_value ,  
				played = v.extend ,  
				integral = v.context , 
				victory = v.extend1,
				titleId = v.title_id,
				server_id = v.server_id
			}
		break
	end

	return t1
end


-- role_id :   角色Id
-- server_id :     服务器Id
-- role_name:  角色名称
-- head_icon:  角色图标
-- level:      角色等级
-- battle_value:   战力
-- context:    上下文 积分
-- extend:     扩展值 胜率
-- guild_name: 工会名称
-- vip_level:    VIP等级
--获得王者表
function Modules.LadderModule:GetKingLst()

	if not self.rank_list then
		self.rank_list = {}
	end

	local t1 = {}
	for i , v in ipairs( self.rank_list ) do
		table.insert( t1 , { 
								info_id = v.role_id , 
								rank = i , 
								head_id = v.head_icon , 
								name = v.role_name , 
								power = v.battle_value ,  
								played = v.extend ,  
								integral = v.context , 
								victory = v.extend1 ,
								server_id = v.server_id ,
					} )
	end

	-- local t1 = {}
	-- for i = 1 , 10 do
	-- 	table.insert( t1 , { server_id = i , name = "啊啊啊啊" , info_id = 39999 , rank = i , head_id = 39999 , power = 1000000 , played = 200000 , integral = 300000 , victory = 400000} )
	-- end

	return t1
end

--获得晋级表
function Modules.LadderModule:GetPromotionCnf()
	local cnf = CPPGameLib.GetConfig("LadderDan", nil , nil , true )
	local t1 = {}
	for i , v in ipairs( cnf ) do
		local t2 = {}
		if cnf[i-1] then
			for ii , vv in ipairs( cnf[i-1].promotion_award_id ) do
				table.insert( t2 , { info_id = vv , item_num = cnf[i-1].promotion_award_num[ii] , sc = 0.85 } )
			end
			table.insert( t1 , { id = i , award = t2 , icon = v.icon , type = v.type , star = v.star } )
		end
	end	
	return t1
end

--获得排行奖励表
function Modules.LadderModule:GetRankAwardCnf()
	--王者
	local cnf = CPPGameLib.GetConfig("LadderKingAward", nil , nil , true )
	local t1 = {}
	for _ , v in ipairs( cnf ) do
		local t2 = {}
		for ii , vv in ipairs( v.award_id ) do
			table.insert( t2 , { info_id = vv , item_num = v.award_num[ii] , sc = 0.85 } )
		end
		table.insert( t1 , { min = v.rank_min , max = v.rank_max , award = t2 , icon = v.icon , title = v.title , icon_star = v.icon_star } )
	end	

	--平民
	local cnf = CPPGameLib.GetConfig("LadderDan", nil , nil , true )
	local t3 = {}
	for i , v in ipairs( cnf ) do
		if v.type ~= Macros.Game.LADDER_LEVEL.LEVEL_7 then
			local t4 = {}
			for ii , vv in ipairs( v.close_award_id ) do
				table.insert( t4 , { info_id = vv , item_num = v.close_award_num[ii] , sc = 0.85 } )
			end
			table.insert( t3 , { sort = i ,  award = t4 , icon = v.icon , icon_star = v.star } )
		end
	end

	table.sort( t3, function ( a , b )
		return a.sort > b.sort
	end )

	for i,v in ipairs(t3) do
		table.insert( t1 , v )
	end

	return t1
end


--获得赛季上场次数
function Modules.LadderModule:GetTaskAppearance( )
	return self.kingMatchData.win_times or 0
end

--获得赛季任务领取记录
function Modules.LadderModule:GetTaskRecordLst( id )
	if not self.taskRecord then
		self.taskRecord = {}
	end

	return self.taskRecord[ id ]
end

--获得天梯任务
function Modules.LadderModule:GetTaskCnf()
	local cnf = CPPGameLib.GetConfig("LadderTask", nil , nil , true )

	local t2 = {}
	local t3 = {}
	for k , v in pairs( cnf ) do
		local t1 = {}
		t1.lst = {}
		for ii , vv in ipairs( v.award_id ) do
			table.insert( t1.lst , { info_id = vv , item_num = v.award_num[ii] } )
		end
		t1.task = v.task

		local type_ = self:GetTaskRecordLst( v.task )
		--已领取
		if type_ then
			table.insert( t2 , t1 )
		else
			table.insert( t3 , t1 )
		end
	end

	table.sort( t2, function ( a , b )
		return a.task < b.task
	end )
	table.sort( t3, function ( a , b )
		return a.task < b.task
	end )

	for i , v in ipairs(t2) do
		table.insert( t3 , v )
	end

	return t3
end

--获得我的对手
function Modules.LadderModule:GetMyOpponent()
	if not self.matchSearchTarget then
		self.matchSearchTarget = {}
	end
	return self.matchSearchTarget
end

--获得赛季信息
function Modules.LadderModule:GetSeasonInfo()
	local cnf = CPPGameLib.GetConfig("LadderCompetitionSeason" , nil , nil , true)

	local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)

	local nowTime = n_nian .. n_yue .. n_ri .. string.format( "%02d" , n_shi ) .. string.format( "%02d" , n_fen ) .. string.format( "%02d" , n_miao )
	
	local o_shi = string.sub( ConfigAdapter.Common.GetLadderOpenTime() , 0 , 2 )
	local o_fen = string.sub( ConfigAdapter.Common.GetLadderOpenTime() , 3 , 4 )

	local e_shi = string.sub( ConfigAdapter.Common.GetLadderCloseTime() , 0 , 2 )
	local e_fen = string.sub( ConfigAdapter.Common.GetLadderCloseTime() , 3 , 4 )


	local t1 = {}
	for i , v in ipairs( cnf ) do
		
		local o_nian = string.sub( v.start_date , 0 , 4 )
		local o_yue = string.sub( v.start_date , 5 , 6 )
		local o_ri = string.sub( v.start_date , 7 , 8 )
		t1 = { 
				--活动开始时间
				openTime = TimeTool.CalcTimeStampFrmDate( v.start_date .. ConfigAdapter.Common.GetLadderOpenTime() ) ,
				--活动结束时间
				closeTime = TimeTool.CalcTimeStampFrmDate( v.over_date .. ConfigAdapter.Common.GetLadderCloseTime() ) ,
				--赛季
				season = v.index ,
				--活动开始时段
				daysOpenTime = o_shi .. o_fen .. "00" ,
				--活动结束时段
				daysCloseTime = e_shi .. e_fen .."00" ,

                openTime_ = o_nian .. "-" .. o_yue .. "-" .. o_ri ,

                nowTime_ = n_Time , 
			}

		if ( tonumber( t1.openTime ) <= tonumber( t1.nowTime_ ) ) and ( tonumber( t1.closeTime ) >= tonumber( t1.nowTime_ ) ) then
			t1.countDown = 0
			break
		elseif ( tonumber( t1.openTime ) >= tonumber( t1.nowTime_ ) ) then
		    local time = TimeTool.GetTimeStampFromDate( o_nian , o_yue , o_ri )
		    t1.countDown = math.ceil( ( time - n_Time ) / ( 24 * 60 * 60 ) )
		    break
		end

	end

	--处于赛季中
	if not t1.countDown or t1.countDown <= 0 then
		local nowTime = n_shi .. n_fen .. n_miao
		if tonumber(t1.daysOpenTime) <= tonumber( nowTime ) and tonumber(t1.daysCloseTime) >= tonumber( nowTime ) then
			t1.proceed = true
		else
			t1.proceed = false
		end
	end

	return t1
end


--获得当前任务的红点
function Modules.LadderModule:GetTaskRed()
	local lst = self:GetTaskCnf()

	local appearance = self:GetTaskAppearance()
	local index = 0
	for i , v in ipairs( lst ) do
		local type_ = CallModuleFunc(ModuleType.LADDER, "GetTaskRecordLst" , v.task )
	    if appearance >= v.task and not type_ then
	        index = index + 1
	    end
	end
	return index
end


--获得赛季的年份
function Modules.LadderModule:GetSeasonVintage( season )
	if not season then return end
	local cnf = CPPGameLib.GetConfig("LadderCompetitionSeason" , season , true)
	
	local text = ""
	if cnf then
		text = string.sub( cnf.start_date, 0 , 4 )
	end
	return text
end


--获得赛季的年份
function Modules.LadderModule:GetKingAward( rank )
	local cnf = CPPGameLib.GetConfig("LadderKingAward" , nil , nil , true)

	local info = {}
	for i , v in ipairs( cnf ) do
		if v.rank_min <= rank and v.rank_max >= rank then
			info = v
			break
		elseif v.rank_max == rank then
			info = v
			break
		end
	end
	return info
end

--设置首胜奖励
function Modules.LadderModule:SetWinningAward( data )
	self.winningAward = data
end


--获得首胜奖励
function Modules.LadderModule:GetWinningAward( data )
	return self.winningAward
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.LadderModule:CheckBattleCondition( type_ )
	return true
end