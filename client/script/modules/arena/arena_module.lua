Modules = Modules or {}

Modules.ArenaModule = Modules.ArenaModule or BaseClass(BaseModule)

function Modules.ArenaModule:__init()
	self:__RegistProtocolForOpenView(BaseViewType.ARENA, Net.ProtocolNo.C2S_ARENA_DATA_REQ, false)
	self:__RegistProtocolForOpenView(BaseViewType.ARENA_REPUTATIONSHOP, Net.ProtocolNo.C2S_ARENA_DATA_REQ, true)
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--请求竞技场挑战列表
function Modules.ArenaModule:ArenaTargetListReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ARENA_DATA_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--请求竞技场挑战列表响应
function Modules.ArenaModule:ArenaTargetListResp(data)
	local last_data = self.__data

	--记录当前数据
	self.__data = data
	self.__data.rank = self.__data.rank == 0 and (#CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) + 1) or self.__data.rank 
	self.__data.history_rank = self.__data.history_rank == 0 and (#CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) + 1) or self.__data.history_rank 

	--记录上一次的排名
	self.__data.last_rank = last_data and last_data.rank or self.__data.rank
	self.__data.last_history_rank = last_data and last_data.history_rank or self.__data.history_rank			
end

--请求竞技场战报
function Modules.ArenaModule:ArenaFightReportReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ARENA_FIGHTREPORTDATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

--请求竞技场战报响应
function Modules.ArenaModule:ArenaFightReportResp(data)
	LogicGUI.TemplArenaFightReportView.New(data)
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--获取挑战列表
function Modules.ArenaModule:GetData()
	return self.__data
end

--获取排名
function Modules.ArenaModule:GetRank()
	return self.__data.rank
end

--获取历史排名
function Modules.ArenaModule:GetHistoryRank()
	return self.__data.history_rank
end

--获取之前排名
function Modules.ArenaModule:GetLastRank()
	return self.__data.last_rank 
end

--获取之前的历史排名
function Modules.ArenaModule:GetLastHistoryRank()
	return self.__data.last_history_rank 
end

--设置挑战对手信息
function Modules.ArenaModule:SetArenaEnemyInfo(rank,index,name,icon,battle_value)
	self.__enemyindex = index
	self.__enemyrank = rank
	self.__enemyname = name
	self.__enemyicon = icon
	self.__enemy_battlevalue = battle_value
end

--设置敌人的战力
function  Modules.ArenaModule:SetEnemyBattleValue(battle_value)
	self.__enemy_battle_value = battle_value
end

--获取敌人的战力
function  Modules.ArenaModule:GetEnemyBattleValue()
	return self.__enemy_battle_value
end

--获取挑战对手信息
function Modules.ArenaModule:GetArenaEnemyInfo()
	return self.__enemyname,self.__enemyicon,self.__enemy_battlevalue	
end

--挑战对手位置
function Modules.ArenaModule:GetArenaEnemyIndex()
	return self.__enemyindex
end

--获取对手排名
function Modules.ArenaModule:GetArenaEnemyRank()
	return self.__enemyrank 
end

--判断是否前进了
function Modules.ArenaModule:GetIsMovetoFront()
	return self.__enemyrank < self.__data.last_rank
end

--获取结算面板所需信息
function Modules.ArenaModule:GetFightResultViewMessage()
	return self.__data.rank, self.__enemyrank, self.__enemyname, self.__enemyicon, 
	self.__data.rank == (#CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) + 1)
end

--打开排行榜
function Modules.ArenaModule:OpenRankView(data)
	local view = self:GetViewByType(BaseViewType.ARENA)	
	if view and view:IsOpen() then
		LogicGUI.TemplArenaRankView.New(data)
	end
end

