--
-- @brief 叛军模块 
-- @author: yjg
-- @date:2016年9月19日20:50:00
--

Modules = Modules or {}

Modules.RebelModule = Modules.RebelModule or BaseClass(BaseModule)

function Modules.RebelModule:__init()
	self:__RegistProtocolForOpenView(BaseViewType.REBEL, Net.ProtocolNo.C2S_REBEL_DATA_REQ, false)

	self.rebelLst = {}
	self.rebelMerit = {}
	self.rebelSum = {}
	self.rebelRank = {}
	self.awardRecord = {}
end

function Modules.RebelModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.RebelModule:HandleNewDayNotify()
	self:RebelDataReq()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
-- 进入界面时和战斗结束时请求叛军数据
function Modules.RebelModule:RebelDataReq()
	-- print("进入界面时和战斗结束时请求叛军数据")
    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REBEL_DATA_REQ))	
end

-- 请求叛军数据响应
function Modules.RebelModule:RebelDataResp(info)
	self.rebel = info
	--叛军列表
	self.rebelLst = info.rebel_list
	--战功
	self.rebelMerit.damage_rank = info.damage_rank
	self.rebelMerit.exploit_rank = info.exploit_rank
	--累计
	self.rebelSum.today_exploit = info.today_exploit
	self.rebelSum.today_damage = info.today_damage

	for i,v in ipairs(self.rebelLst) do
		if v.quality == 1 then
			v.quality = 2 
		end
		local info = CPPGameLib.GetConfig("RebelQuality", v.quality)
		v.name = info.name

		local battle_id = ConfigAdapter.GameLevel.GetFirstBattleID(v.map_id)
		local monster_list = ConfigAdapter.Battle.GetMonsterList(battle_id)


		local rebelBattleCnf = CPPGameLib.GetConfig("RebelBattle", v.level)
		local hp = 0
		for i,vv in ipairs(rebelBattleCnf.add_pro_type_list) do
			if vv == Macros.Game.BattleProp.HP_RATE then
				hp = rebelBattleCnf.add_pro_value_list[i]
			end
		end
		local sumHp = 0
		local config_monster
		for i, monster_id in ipairs(monster_list) do
			config_monster = CPPGameLib.GetConfig("Monster", monster_id)
			sumHp = sumHp +  math.ceil( config_monster.blood_max * ( 1 + MathTool.TransProbCoeff2Decimal(hp) ) )
		end
		v.sumHp = sumHp

		--模型
		local config_monster = CPPGameLib.GetConfig("Monster", monster_list[1])
		v.monster_id = monster_list[1]

		local model_id = nil
		local model_type = nil
		if config_monster.hero_id then
			model_id = config_monster.hero_id
			model_type = LogicGUI.ModelType.HERO
		else
			model_id = config_monster.model_id
			model_type = LogicGUI.ModelType.MONSTER
			
			if config_monster.model_type then
				if config_monster.model_type == "hero" then
					model_type = LogicGUI.ModelType.HERO
				end
			end
		end
		v.modes_id = model_id
		v.model_type = model_type
	end

	--更新发现叛军列表
	self.rebelFindList = {}
	for _, v in ipairs(self.rebelLst) do
		local info = {}
		info.quality = v.quality
		info.level = v.level
		info.rebel_id = v.uid
		info.map_id = v.map_id
		info.discoverer_id = v.discoverer_id
		table.insert(self.rebelFindList, info)
	end

	self:FireNextFrame(Macros.Event.RebelModule.REBEL_DATA_RESP)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.REBEL )	
end

-- 领取功勋奖励
function Modules.RebelModule:RebelExploitAwardReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REBEL_EXPLOIT_AWARD_REQ)
	data.award_list = info.award_list
	GlobalNetAdapter:OnSend(data)	
end

-- 领取功勋奖励响应
function Modules.RebelModule:RebelExploitAwardResp(info)
	--角色叛军
	local lv = CallModuleFunc(ModuleType.REBEL , "RebelAwardLv")
	local roleRebel = CPPGameLib.GetConfig("RoleRebel", lv)

	--更新记录
    for i,v in ipairs(info.award_record_list) do
    	self.awardRecord[v.award_id] = v.award_id
    end
	
	--奖励
	local num = 0
    local t1 = {}
	for i,v in pairs(CPPGameLib.GetConfig("RebelExploit", nil, nil , true )) do
	    if roleRebel.award_list_id == v.list_id then
	   		t1[v.award_id] = v
	    end
    end
	local lst = {}
	for i,v in ipairs(info.award_record_list) do
		if t1[v.award_id] then
			table.insert( lst,{id = t1[v.award_id].award_item_id , num = t1[v.award_id].award_item_num} )
		end
	end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , lst )

	GlobalEventSystem:FireNextFrame(Macros.Event.RebelModule.REBEL_AWARD_RECORD,info)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.REBEL )	
end

-- 功勋奖励记录请求
function Modules.RebelModule:RebelExploitAwardRecordReq(info)
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REBEL_EXPLOIT_AWARD_RECORD_REQ))	
end

-- 功勋奖励记录响应
function Modules.RebelModule:RebelExploitAwardRecordResp(info)
	for i,v in ipairs(info.award_record_list) do
		self.awardRecord[v.award_id] = v.award_id
	end
	GlobalEventSystem:FireNextFrame(Macros.Event.RebelModule.REBEL_AWARD_RECORD,info)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.REBEL )
end

-- 叛军分享请求
function Modules.RebelModule:RebelShaReReq(info)
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REBEL_SHARE_REQ))	
end

-- 叛军分享响应
function Modules.RebelModule:RebelShaReResp(info)
end

-- 发现叛军通知
function Modules.RebelModule:RebelDiscoverNotify(info)
	self.rebelFindList = self.rebelFindList or {}
	info.discoverer_id = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo").role_id
	table.insert( self.rebelFindList, info )
	self:FireNextFrame(Macros.Event.RebelModule.REBEL_DATA_RESP)
	
	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.REBEL )
end

--请求叛军排行榜
function Modules.RebelModule:RebelRankReq(type)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LIST_REQ)
	data.type = type
	data.sub_type = 0
	GlobalNetAdapter:OnSend(data)
end

--响应叛军排行榜
function Modules.RebelModule:RebelRankResp( info )
	CPPGameLib.PrintTable(info,"响应叛军排行榜")
	if info.type == Macros.Game.RankType.REBEL_HARM then
		self.rebelRank.HARM = info.rank_list
	elseif info.type == Macros.Game.RankType.REBEL_FEATS then
		self.rebelRank.FEATS = info.rank_list
	end
	GlobalEventSystem:FireNextFrame(Macros.Event.RebelModule.REBEL_RANK)
end

--叛军购买响应
function Modules.RebelModule:RebelBuyResp( info )
	-- 2926 战功商店购买物品成功后，添加购买成功的弹出窗口
	local cnf = CPPGameLib.GetConfig("RebelShop", info.param1 )
	local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = cnf.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	-- 1951 战功商店买东西弹恭喜获得？？？
	-- GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_success"))
end

--------------------------------------------------------------------------------------------------------------------------------
--接口
--------------------------------------------------------------------------------------------------------------------------------
--获得伤害排行
function Modules.RebelModule:GetRebelRankHarm( ... )
	if not self.rebelRank.HARM then return end
	for i,v in ipairs(self.rebelRank.HARM) do
		v.rank = i
		v.type = Macros.Game.RankType.REBEL_HARM
	end
	return self.rebelRank.HARM
end

--获得功勋排行
function Modules.RebelModule:GetRebelRankFarm( ... )
	if not self.rebelRank.FEATS then return end
	for i,v in ipairs(self.rebelRank.FEATS) do
		v.rank = i
		v.type = Macros.Game.RankType.REBEL_FEATS
	end
	return self.rebelRank.FEATS
end

--叛军列表
function Modules.RebelModule:RebelLst( ... )
	if not self.rebelLst then
		return
	end
	return self.rebelLst
end

--战功显示&伤害显示
function Modules.RebelModule:RebelMerit( ... )
	local info = {}
	info.damage_rank = self.rebel.damage_rank
	info.exploit_rank = self.rebel.exploit_rank
	return info 
end

--累计伤害
function Modules.RebelModule:RebelSum( ... )
	local info = {}
	info.today_exploit = self.rebel.today_exploit
	info.today_damage = self.rebel.today_damage
	return info  
end

--获得奖励等级
function Modules.RebelModule:RebelAwardLv( ... )
	return self.rebel.init_role_level
end

--获得叛军发现列表
function Modules.RebelModule:GetRebelFindList()
	return self.rebelFindList
end

--是否分享过叛军
function Modules.RebelModule:IsAlreadyShare( ... )
	local type_ = false
	if self.rebel.is_already_share ~= 0 then
		type_ = true
	end
	return type_
end

--获得可攻打的叛军发现信息
function Modules.RebelModule:GetCanHitRebelFindData( type_ )

	local function RoleInfo( v )
		if v.quality == 1 then
			v.quality = 2
		end

		local info = CPPGameLib.GetConfig("RebelQuality", v.quality)
		v.name = info.name

		local battle_id = ConfigAdapter.GameLevel.GetFirstBattleID(v.map_id)
		local monster_list = ConfigAdapter.Battle.GetMonsterList(battle_id)

		local rebelBattleCnf = CPPGameLib.GetConfig("RebelBattle", v.level)
		local hp = 0
		for i,vv in ipairs(rebelBattleCnf.add_pro_type_list) do
			if vv == Macros.Game.BattleProp.HP_RATE then
				hp = rebelBattleCnf.add_pro_value_list[i]
			end
		end

		local sumHp = 0
		local config_monster
		for i, monster_id in ipairs(monster_list) do
			config_monster = CPPGameLib.GetConfig("Monster", monster_id)
			sumHp = sumHp + math.ceil( config_monster.blood_max * ( 1 + MathTool.TransProbCoeff2Decimal(hp) ) )
		end
		v.sumHp = sumHp
		v.monster_id = monster_list[1]
		v.model_type = LogicGUI.ModelType.MONSTER
		return v
	end

	local role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo").role_id

	for i,v in ipairs(self.rebelFindList) do
		if not v.hit then
			if type_ then
				if v.discoverer_id == role_id then
					return RoleInfo(v)
				end 
			else
				return RoleInfo(v)
			end
		end
	end

end

--显示当前打的谁
function Modules.RebelModule:ShowRebelLayer(callback)
	if self:GetCanHitRebelFindData() then 
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.REBEL_RANK_FIND , false , callback )
	end
end

--当前可领取的奖励数量
function Modules.RebelModule:GetNoeAwardRed()
	--角色叛军
	local lv = self:RebelAwardLv()
	local roleRebel = CPPGameLib.GetConfig("RoleRebel", lv)
	--玩家信息
	local sum = self:RebelSum()
	--奖励
	local num = 0
	for i,v in pairs(CPPGameLib.GetConfig("RebelExploit", nil, nil , true )) do
	    if roleRebel.award_list_id == v.list_id and sum.today_exploit >= v.need_exploit then
    		if not self.awardRecord[v.award_id] then
		    	num = num + 1
		    end
	    end
	end

	return num
end

--当前可领取的奖励数量
function Modules.RebelModule:GetAwardRecord()
	return self.awardRecord
end

--当前可领取的奖励数量
function Modules.RebelModule:GetRebelInfo()
	return self.rebel
end

--判断当前时间是否开启活动
function Modules.RebelModule:GetActivityOpen( start , over )

	start = CPPGameLib.SplitEx(start , ":")

	over = CPPGameLib.SplitEx(over , ":")

	local now = TimeTool.GetTimeFormat(  CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") , TimeTool.defaul_format[15] )
	now = CPPGameLib.SplitEx(now , ":")

	local t1 = ""
	local t2 = ""
	local t3 = ""

	for i,v in ipairs(start) do
		t1 = t1 .. v
		t2 = t2 .. over[i]
		t3 = t3 .. now[i]
	end

	if (tonumber(t1) < tonumber(t3) and tonumber(t2) > tonumber(t3)) or tonumber(t1) == tonumber(t3)  or tonumber(t2) == tonumber(t3) then
		return true
	else
		return false
	end
end

--是否功勋翻倍时间
function Modules.RebelModule:IsExploitDoubleTime()
	local exploit_double_time = ConfigAdapter.Common.GetRebelExploitDouble()
	local start_time = TimeTool.CalcSecondFrmHHMMSS(exploit_double_time[1])
	local over_time = TimeTool.CalcSecondFrmHHMMSS(exploit_double_time[2])

	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")	
	local year, month, day, hour, minute = TimeTool.GetDateFromTimeStamp(server_time)
	local cur_time = hour * 60 * 60 + minute * 60
	return start_time <= cur_time and cur_time <= over_time
end

--战斗对象
function Modules.RebelModule:CombatObject( id )
	if not self.combatObject then
		self.combatObject = id
	end
	self.combatObject = id
end

--获得战斗对象
function Modules.RebelModule:GetCombatObject( id )
	return self.combatObject
end

------------------------------------------------------------------------------------------------------------------------------------------------------------
--排行榜
------------------------------------------------------------------------------------------------------------------------------------------------------------
--获得排行榜的领取状况
function Modules.RebelModule:GetRankDrawType()
	--角色叛军
	local lv = self:RebelAwardLv()
	local roleRebel = CPPGameLib.GetConfig("RoleRebel", lv)
	local record = self:GetAwardRecord()
	--奖励列表
	local t1 = {}
	local t2 = {}
	for i,v in pairs(CPPGameLib.GetConfig("RebelExploit", nil, nil , true )) do
		if roleRebel.award_list_id == v.list_id then
			if record[v.award_id] then
				table.insert(t1,v)
			else
				table.insert(t2,v)
			end
		end
	end
	table.sort( t1, function ( a , b )
		return a.need_exploit < b.need_exploit
	end )
	table.sort( t2, function ( a , b )
		return a.need_exploit < b.need_exploit
	end )

	for i,v in ipairs(t1) do
		table.insert( t2 , v )
	end

	return t2
end

------------------------------------------------------------------------------------------------------------------------------------------------------------
--商店红点
------------------------------------------------------------------------------------------------------------------------------------------------------------

--获得当前可以买的道具数量（红点
function Modules.RebelModule:GetNowBuyNum( ... )
	local lst = {}
	local index = 0
	for i,v in pairs(CPPGameLib.GetConfig("RebelShop", nil, nil , true )) do
		if not lst[v.cost_type_list[1]] then
			lst[v.cost_type_list[1]] = {}
		end
		local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v.cost_type_list[1] )
		if item_num >= v.cost_value_list[1] then
			--预留 每个类型都保留一份可以购买的
			table.insert( lst[v.cost_type_list[1]] , { id = v.item_id , cost_type = v.cost_type_list[1] , cost_value = v.cost_value_list[1] } )
			index = index + 1
		end
	end

	return index
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.RebelModule:CheckBattleCondition( type_ , data )
	--普通
	if type_ == Macros.Game.RebelAttackType.NORMAL then
		local wanted = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").wanted
		if wanted >= ConfigAdapter.Common.GetRebelConsume1() then

			self:CombatObject( data.discoverer_id )
			return true
		end
	--全力
	elseif type_ == Macros.Game.RebelAttackType.FULL then
		local start = TimeTool.TransTimeStamp( "HH:MM" , ( ConfigAdapter.Common.GetRebelConsumeHalve()[1] * 0.01 * 60 * 60) )
		local over = TimeTool.TransTimeStamp( "HH:MM" , ( ConfigAdapter.Common.GetRebelConsumeHalve()[2] * 0.01 * 60 * 60) )

		local bool = CallModuleFunc(ModuleType.REBEL, "GetActivityOpen", start , over )
		local wanted = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").wanted
		
		if bool then
			if wanted >= (ConfigAdapter.Common.GetRebelConsume2()/2) then
				self:CombatObject( data.discoverer_id )
				return true
			end
		else
			if wanted >= ConfigAdapter.Common.GetRebelConsume2() then
				self:CombatObject( data.discoverer_id )
				return true
			end
		end
	end

end