Modules = Modules or {}

Modules.RankModule = Modules.RankModule or BaseClass(BaseModule)

function Modules.RankModule:__init()
	--false  是每次打开都请求
    --self:__RegistProtocolForOpenView(BaseViewType.RANK,Net.ProtocolNo.C2S_RANK_LIST_REQ,false)
    self.__rank_data_list = {}
end

function Modules.RankModule:__delete()
	
end

--请求排行榜列表
function Modules.RankModule:RankListRequest( info)
	-- CPPGameLib.PrintTable(info,"请求排行榜列表")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LIST_REQ)
	data.type = info.type or Macros.Game.RankType.BATTLE
	data.sub_type = info.sub_type or 0
	GlobalNetAdapter:OnSend(data)
end

--请求排行榜响应
function Modules.RankModule:RankListResponse( info )
	self.__rank_data_list[info.type] = info
	-- CPPGameLib.PrintTable("xy"  ,info,"请求排行榜响应")
	if info.type == Macros.Game.RankType.REBEL_HARM or info.type == Macros.Game.RankType.REBEL_FEATS then 	--叛军
		CallModuleFunc(ModuleType.REBEL, "RebelRankResp",info)
	elseif info.type == Macros.Game.RankType.KRUUNU then				--爬塔
		CallModuleFunc(ModuleType.TOWER, "TowerRankResp",info)
	elseif info.type == Macros.Game.RankType.BATTLE then				--战斗力	
	elseif info.type == Macros.Game.RankType.LEVEL then 				--等级
		
	elseif info.type == Macros.Game.RankType.WORLDBOSS_HONOR or info.type == Macros.Game.RankType.WORLDBOSS_HARM then 		--荣誉排行榜
		CallModuleFunc(ModuleType.WORLDBOSS, "RankResp",info)
	elseif info.type == Macros.Game.RankType.ARENA then	 --竞技场
		CallModuleFunc(ModuleType.ARENA, "OpenRankView",info)
		-- CPPGameLib.PrintTable(info,"uuuuuuuuuuuuuuuuuuuuuuuuuuu")
	elseif info.type == Macros.Game.RankType.MAIN_DUNGEON_STAR then	 --星级排行榜

	elseif info.type == Macros.Game.RankType.ONESELF_SERVER then --本服排行榜
		CallModuleFunc(ModuleType.PVP, "PVPRankResp",info)
	elseif info.type == Macros.Game.RankType.ALL_SERVER then --全服排行榜
		CallModuleFunc(ModuleType.PVP, "PVPRankResp",info)
	elseif info.type == Macros.Game.RankType.MONSTER_STORMCITY then --怪物攻城
		CallModuleFunc(ModuleType.MONSTER_STORMCASTLE, "MonsterStormCityResp",info)
	elseif info.type == Macros.Game.RankType.CHASM then --无尽
		CallModuleFunc(ModuleType.CHASM, "EndlessRankResp",info)
	elseif info.type == Macros.Game.RankType.LADDER then --天梯
		CallModuleFunc(ModuleType.LADDER, "LadderRankResp",info)
	elseif info.type == Macros.Game.RankType.CARD then --占星
		CallModuleFunc(ModuleType.DIVINATION, "DivinationRankListResponse",info)
	elseif info.type == Macros.Game.RankType.RANK_TURNTABLE_LOTTERY then
		CallModuleFunc(ModuleType.TURNTABLE, "TurntableRankResp",info) --转盘
	elseif info.type == Macros.Game.RankType.RECHARGE then
		CallModuleFunc(ModuleType.RECHARGE_RANK, "RechargeRankResp" , info ) --充值
	elseif info.type == Macros.Game.RankType.CONSUME then
		CallModuleFunc(ModuleType.RECHARGE_RANK, "ConsumeRankResp" , info ) --消费
	end
	self:FireNextFrame(Macros.Event.RankModule.RANK_LIST_REQ,info)
end

--当前排行榜的数据列表
function Modules.RankModule:GetRankDataList(type)
	return self.__rank_data_list[type]
end

--点赞
function Modules.RankModule:RankLikeRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LIKE_REQ)
	data.target_id = info.target_id
	data.type = info.type
	GlobalNetAdapter:OnSend(data)
end

--留言
function Modules.RankModule:RankLeaveMessageRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LEAVE_MESSAGE_REQ)
	data.message = info.message 
	GlobalNetAdapter:OnSend(data)
end

--留言响应
function Modules.RankModule:RankLeaveMessageResponse( info )


	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rank_leave"))
	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	local data = {}
	data.role_id = role_id
	data.message = info.message
	self:NewLeaveMessage(data)
	self:FireNextFrame(Macros.Event.RankModule.LEAVE_SUCCESS,role_id)
end



--请求留言列表
function Modules.RankModule:RankLeaveMessageListRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LEAVE_MESSAGE_LIST_REQ)

	GlobalNetAdapter:OnSend(data)
end

--请求留言列表响应
function Modules.RankModule:RankLeaveMessageListResponse( info )
	self:GetLeaveMessageList(info)	
end



--获得排名
function Modules.RankModule:GetRanking(type)
	local personId = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	if self.__data_map[type] then
		for i,v in ipairs(self.__data_map[type]) do
			if personId ==v.role_id then
				return i
			end
		end
		return 0
	end
	return nil
end


--退出之前要置空
function Modules.RankModule:DeleteData(  )
	if self.__data_map then
		self.__data_map = {}
	end
end


function Modules.RankModule:GetDataRank(type)
	if self.__data_map[type] then
		return self.__data_map[type]
	end 
end



function Modules.RankModule:GetLeaveMessageList(data)
	self.__data_leave = data
end




function Modules.RankModule:ReturnLeaveMessage(id)
	if self.__data_leave.leave_message_list then
     	for i,v in pairs(self.__data_leave.leave_message_list) do
     		if v.role_id == id then
     			return v.message
     		end
    	end
 	end
 	return nil
end


function Modules.RankModule:NewLeaveMessage(info)
	-- if self.__data_leave.leave_message_list then
		for i,v in pairs(self.__data_leave.leave_message_list) do
			if v.role_id == info.role_id then
				v.message = info.message
			else
				table.insert(self.__data_leave.leave_message_list,info)
			end
		end
	-- else
	-- 	table.insert(self.__data_leave.leave_message_list,info)
	-- end
end

-- 消息说明: 请求我的排名 消息Id: 20811
function Modules.RankModule:MYRankReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MY_RANK_REQ)
	data.rank_type = info.rank_type
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 请求我的排名响应 消息Id: 20812
function Modules.RankModule:MYRankResp( info )
	--竞技场
	if info.rank_type == Macros.Game.RankType.ARENA then
		CallModuleFunc(ModuleType.RANK_ACTIVITY, "SetArenaRank" , info.my_rank )
	--主线副本
	elseif info.rank_type == Macros.Game.RankType.MAIN_DUNGEON_STAR then
		CallModuleFunc(ModuleType.RANK_ACTIVITY, "SetManinDungeonStarRank", info.my_rank )
	--战斗力
	elseif info.rank_type == Macros.Game.RankType.BATTLE then
		CallModuleFunc(ModuleType.RANK_ACTIVITY, "SetBattleRank", info.my_rank )
	end

end
