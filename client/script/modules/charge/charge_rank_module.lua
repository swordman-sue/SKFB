--
-- @brief 充值排行 
-- @author: yjg
-- @date:2018年12月18日11:55:30
--

Modules = Modules or {}

Modules.ChargeRankModule = Modules.ChargeRankModule or BaseClass(BaseModule)

function Modules.ChargeRankModule:__init()
	self.awardLst = {}
end

function Modules.ChargeRankModule:__delete()
end

------------------------------------------------------------------------------------------------------------------------------------------------------------
--充值
------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明:充值排行榜请求 消息Id: 
function Modules.ChargeRankModule:RechargeRankReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:充值排行榜请求 消息Id: ")
	local info = {}
	info.type = Macros.Game.RankType.RECHARGE
	CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
end

-- 消息说明:充值排行榜响应 消息Id: 
-- "type__C",   --排行榜类型
-- "sub_type__C",  --排行榜子类型(目标只有世界BOSS排行用上)
-- "rank__H",  --排名(自己的)
-- "rank_value__I",--排名值(自己的)
function Modules.ChargeRankModule:RechargeRankResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:充值排行榜响应 消息Id: ")
	--排名(自己的)
	self.r_rank = info.rank
	self.r_rank_value = info.rank_value
	--排名列表
	for i , v in ipairs( info.rank_list  ) do
		v.sort = i
		v.type_ = 1
	end
	self.r_rank_list = info.rank_list 

	GlobalEventSystem:Fire(Macros.Event.RankModule.RANK_RECHARGE_REQ , "chongzhi" )
end

--获得我的排名
function Modules.ChargeRankModule:Recharge_GetMyRank()
	return self.r_rank
end

--获得我的排名
function Modules.ChargeRankModule:Recharge_GetMyRankValue()
	return self.r_rank_value
end

--获得排名列表
function Modules.ChargeRankModule:Recharge_GetRankLst()
	return self.r_rank_list
end


--获得排行表
function Modules.ChargeRankModule:Recharge_GetRechargeRankCnf()
	local cnf = CPPGameLib.GetConfig( "RechargeRank", 1 )
	return cnf
end

--获得奖励表
function Modules.ChargeRankModule:Recharge_GetRechargeAwardCnf()
	if not self.awardLst[1] then
		self.awardLst[1] = {}
		local cnf = CPPGameLib.GetConfig( "RechargeAward", nil , false , true )
		for k , v in ipairs( cnf ) do
			if v.rank_mold == 1 then
				local t1 = {}
				if v.rank_min == v.rank_max then
					t1.sort = v.rank_min
				else
					t1.sort = v.rank_min .."~".. v.rank_max
				end
				t1.item_lst = {}
				t1.location = v.rank_min
				for index = 1 , #v.item_id_list do
					table.insert( t1.item_lst , { item_id = v.item_id_list[index] , item_num = v.item_num_list[index] }   )
				end
				table.insert( self.awardLst[1] , t1 )
			end
		end
	end
	return self.awardLst[1]
end





------------------------------------------------------------------------------------------------------------------------------------------------------------
--消费
------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明:消费排行榜请求 消息Id: 
function Modules.ChargeRankModule:ConsumeRankReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:充值排行榜请求 消息Id: ")
	local info = {}
	info.type = Macros.Game.RankType.CONSUME
	CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
end

-- 消息说明:消费排行榜响应 消息Id: 
-- "type__C",   --排行榜类型
-- "sub_type__C",  --排行榜子类型(目标只有世界BOSS排行用上)
-- "rank__H",  --排名(自己的)
-- "rank_value__I",--排名值(自己的)
function Modules.ChargeRankModule:ConsumeRankResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:充值排行榜响应 消息Id: ")
	--排名(自己的)
	self.c_rank = info.rank
	self.c_rank_value = info.rank_value
	--排名列表
	for i , v in ipairs( info.rank_list  ) do
		v.sort = i
		v.type_ = 2
	end
	self.c_rank_list = info.rank_list 
	
	GlobalEventSystem:Fire(Macros.Event.RankModule.RANK_RECHARGE_REQ , "xiaofei" )
end

--获得我的排名
function Modules.ChargeRankModule:Consume_GetMyRank()
	return self.c_rank
end

--获得我的排名
function Modules.ChargeRankModule:Consume_GetMyRankValue()
	return self.c_rank_value
end

--获得排名列表
function Modules.ChargeRankModule:Consume_GetRankLst()
	return self.c_rank_list
end

--获得排行表
function Modules.ChargeRankModule:Consume_GetRechargeRankCnf()
	local cnf = CPPGameLib.GetConfig( "RechargeRank", 2 )
	return cnf
end

--获得奖励表
function Modules.ChargeRankModule:Consume_GetRechargeAwardCnf()
	if not self.awardLst[2] then
		self.awardLst[2] = {}
		local cnf = CPPGameLib.GetConfig( "RechargeAward", nil , false , true )
		for k , v in ipairs( cnf ) do
			if v.rank_mold == 2 then
				local t1 = {}
				if v.rank_min == v.rank_max then
					t1.sort = v.rank_min
				else
					t1.sort = v.rank_min .."~".. v.rank_max
				end
				t1.item_lst = {}
				t1.location = v.rank_min
				for index = 1 , #v.item_id_list do
					table.insert( t1.item_lst , { item_id = v.item_id_list[index] , item_num = v.item_num_list[index] }   )
				end
				table.insert( self.awardLst[2] , t1 )
			end
		end
	end
	return self.awardLst[2]
end