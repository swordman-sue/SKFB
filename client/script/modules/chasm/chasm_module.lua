--[[
%% @module: 深渊
%% @author: yjg
%% @created: 2017年7月11日21:59:47
--]]


Modules = Modules or {}

Modules.ChasmModule = Modules.ChasmModule or BaseClass(BaseModule)

function Modules.ChasmModule:__init()
	--服务端与客户端对象ID映射表
    self.__sc_obj_id_map = {}

    --刷怪进度表(key：槽 value：进度)
    self.__create_monster_prog_map = {}
end

function Modules.ChasmModule:__delete()
end

-- 消息说明: 无尽之地数据请求 消息Id: 24701
function Modules.ChasmModule:EndlessLandDataReq()
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽之地数据请求 消息Id: 24701")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_DATA_REQ)
	GlobalNetAdapter:OnSend( data )
end

-- 消息说明: 无尽之地数据响应 消息Id: 24702
-- "today_battle_times__H",	--今日挑战次数
-- "cur_wave__H",	--当前挑战的波数(正在打的)
-- "today_wave_record__H",	--今日波数记录(已通关的)
-- "cur_wave_kill_num__H",	--当前波击杀怪的数量
-- "history_wave_record__H",--历史波数记录(已通关的)
-- "cur_keep_kill_num__H",--当前连杀数量
-- "keep_kill_num_record__H",--连杀数量记录
-- "today_collision_reward_times__H",--碰撞奖励次数(今日已经获得的次数)
-- "keep_kill_reward_record__T__kill_num@H", -- 连杀奖励记录
-- "is_first_battle__C",--是否首次挑战(1=是,0=否)
function Modules.ChasmModule:EndlessLandDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽之地数据响应 消息Id: 24702")
	self.endlessLandData = info
	self.is_first_battle = info.is_first_battle
	-- local t1 = {}
	-- for i , v in ipairs( info.keep_kill_reward_record ) do
	-- 	t1[v.kill_num] = v
	-- end
	-- self.endlessLandData.keep_kill_reward_record = t1
	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_DATA)
end

-- 消息说明: 无尽之地进入请求 消息Id: 24703
function Modules.ChasmModule:EndlessLandEnterReq()
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽之地进入请求 消息Id: 24703")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_ENTER_REQ)
	GlobalNetAdapter:OnSend( data )
end

-- 消息说明: 无尽之地进入响应 消息Id: 24704
function Modules.ChasmModule:EndlessLandEnterResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽之地进入响应 消息Id: 24704")

	if self.endlessLandData.cur_wave > 1 then
		CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM_SWEEP )
	else
		local checkBattle = self:CheckBattleCondition()
		if checkBattle then
			CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.CHASM)
		end
	end

	self:ChangeWave(true)
end

-- 消息说明: 杀怪请求 消息Id: 24705
function Modules.ChasmModule:EndlessLandKillMonsterReq( obj_id )
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return
	end
	local obj_vo = obj:GetVO()

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_KILL_MONSTER_REQ)
	data.index = obj_vo.index
	data.monster_id = obj_vo.info_id
	GlobalNetAdapter:OnSend( data )
end

-- 消息说明: 杀怪响应 消息Id: 24706
function Modules.ChasmModule:EndlessLandKillMonsterResp( protocol_data )	
	--更新连杀记录
	self:SetCurKeepKillNum()

	local obj_id = self:__ConvertSID2CID(protocol_data.index, protocol_data.monster_id)
	if not obj_id then
		return 
	end

	--处理刷怪
	self:__HandleCreateMonster(protocol_data.index, obj_id)

	--处理怪物死亡
	self.__drop_map = self.__drop_map or {}
	self.__drop_map[obj_id] = {}
	self.__drop_map[obj_id].item_list = protocol_data.drop_item_list

	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ObjDeadNotify", obj_id, true)
end

-- 消息说明: 碰撞请求 消息Id: 24707
function Modules.ChasmModule:EndlessLandCollisionReq( info )
	if not self.collisionData or self.collisionData.today_collision_reward_times < ConfigAdapter.Common.GetCollisionRewardTimes() then
		CPPGameLib.PrintTable("xy" , info , "消息说明: 碰撞请求 消息Id: 24707")
		local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_COLLISION_REQ)
		GlobalNetAdapter:OnSend( data )
	end
end

-- 消息说明: 碰撞响应 消息Id: 24708
function Modules.ChasmModule:EndlessLandCollisionResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 碰撞响应 消息Id: 24708")
	self.collisionData = info 
end

-- 消息说明: 领取连杀奖励请求 消息Id: 24709
function Modules.ChasmModule:EndlessLandKeepKillRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取连杀奖励请求 消息Id: 24709")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ)
	data.keep_kill_num = info.keep_kill_num
	GlobalNetAdapter:OnSend( data )
end

-- 消息说明: 领取连杀奖励响应 消息Id: 24710
function Modules.ChasmModule:EndlessLandKeepKillRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取连杀奖励响应 消息Id: 24710")
	self.endlessLandData.last_take_continue_kill_reward = info.keep_kill_num

	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_AWARD , info.keep_kill_num )
end

-- 消息说明: 无尽排行榜 消息Id: 
function Modules.ChasmModule:EndlessRankReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽排行榜 消息Id:")
	local info = {}
	info.type = Macros.Game.RankType.CHASM
	CallModuleFunc( ModuleType.RANK, "RankListRequest" , info )
end

-- 消息说明: 无尽排行榜响应 消息Id: 
function Modules.ChasmModule:EndlessRankResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 无尽排行榜响应 消息Id:")
	self.rankData = info
	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_RANK)
end

-- 消息说明: 波战斗结束请求
function Modules.ChasmModule:EndlessLandBattleOverReq(is_win, is_manual_exit)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ENDLESS_LAND_BATTLE_OVER_REQ)
	data.is_win = is_win and Macros.Global.TRUE or Macros.Global.FALSE
	data.wave = self:GetCurWave()
	GlobalNetAdapter:OnSend(data)

	CallModuleFunc(ModuleType.BATTLE, "HandleBattleEndReq", is_manual_exit)
end

-- 消息说明: 波战斗结束反馈
function Modules.ChasmModule:EndlessLandBattleOverResp(protocol_data)
	self.reward_wave = protocol_data.reward_wave
	local is_win = protocol_data.is_win == Macros.Global.TRUE
	if is_win then
		local cur_wave = self.endlessLandData.cur_wave
		if cur_wave > self.endlessLandData.today_wave_record then
			--更新今日波数记录
			self.endlessLandData.today_wave_record = cur_wave
		end
		if cur_wave > self.endlessLandData.history_wave_record then
			--更新历史波数记录
			self.endlessLandData.history_wave_record = cur_wave
		end
	end

	CallModuleFunc(ModuleType.BATTLE, "HandleBattleEndResp", protocol_data, not is_win, not is_win)
end

--深渊购买
function Modules.ChasmModule:ShopBuyRequest( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 深渊购买 消息Id:")
	-- data.type =  info.type or 1
	-- data.param1 =  info.param1 or 0
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest", info )
end

--深渊响应
function Modules.ChasmModule:ChasmBuyResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 深渊响应 消息Id:")
	if self.chasmRequest[info.param1] then
		self.chasmRequest[info.param1] = { goods_id = info.param1 , buy_num = self.chasmRequest[info.param1].buy_num +  info.param2 }
	else
		self.chasmRequest[info.param1] = { goods_id = info.param1 , buy_num = info.param2 }
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_SHOP)

	local cnf = CPPGameLib.GetConfig("EndlessLandShop", info.param1 )
	local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = cnf.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

--深渊商店记录
function Modules.ChasmModule:ChasmBuyNotesRequest( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 深渊商店记录 消息Id:")
	if not self.chasmRequest then
		self.chasmRequest = {}
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest" , Macros.Game.SHOP.CHASM_SHOP)
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest" , Macros.Game.SHOP.CHASM_AWARD)
	end
end

--深渊商店记录响应
function Modules.ChasmModule:ChasmBuyNotesResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 深渊商店记录响应 消息Id:")
	local t1 = {}
	for i,v in ipairs(info.buy_record_list) do
		self.chasmRequest[v.goods_id] = v 
	end

	CPPGameLib.PrintTable("xy", self.chasmRequest ,"ChasmBuyNotesResp") 
	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_SHOP)
end

function Modules.ChasmModule:GetChasmRequest( id )
	return self.chasmRequest[ id ]
end

--获得连杀数量
function Modules.ChasmModule:GetCurKeepKillNum()
	if self.endlessLandData then
		return self.endlessLandData.cur_keep_kill_num
	else
		return 0
	end
end

--更新连杀记录
function Modules.ChasmModule:SetCurKeepKillNum(var)
	var = var or 1
	self.endlessLandData.cur_wave_kill_num = self.endlessLandData.cur_wave_kill_num + var

	self.endlessLandData.cur_keep_kill_num = self.endlessLandData.cur_keep_kill_num + var

	if self.endlessLandData.cur_keep_kill_num > self.endlessLandData.keep_kill_num_record then
		self.endlessLandData.keep_kill_num_record = self.endlessLandData.cur_keep_kill_num
	end
end

--获得最高击杀数量
function Modules.ChasmModule:GetKeepKillNum( ... )
	if self.endlessLandData then
		return self.endlessLandData.keep_kill_num_record
	else
		return 0
	end
end

--获得今日波数记录
function Modules.ChasmModule:GetTodayWave( ... )
	if self.endlessLandData then
		return self.endlessLandData.today_wave_record
	else
		return 0
	end
end

--获得历史波数记录
function Modules.ChasmModule:GetHistoryWaveRecord()
	return self.endlessLandData.history_wave_record
end

--获得碰撞奖励
function Modules.ChasmModule:GetCrashReward()
	local text = ""
	if self.collisionData.reward_id ~= 0 then
		local cnf = CPPGameLib.GetConfig("EndlessLandCollision", self.collisionData.reward_id )
		for i , v in ipairs( cnf.item_id_list ) do
			local itemCnf = CPPGameLib.GetConfig("Item", v )
			text = text .. itemCnf.name .." X ".. cnf.item_num_list[i]
		end
	end
	return text
end

--获得是否免费
function Modules.ChasmModule:GetTodayFirstBattle()
	if not self.endlessLandData then 
		return true
	end

	if not self.endlessLandData.today_battle_times or self.endlessLandData.today_battle_times <= 0 then
		return true
	end

	local cnf = CPPGameLib.GetConfig("EndlessLandBattleCost", (self.endlessLandData.today_battle_times or 0) + 1 , false )
	if not cnf.cost_item_id and not cnf.cost_currency_type then
		return true
	end
end

--获得消耗
function Modules.ChasmModule:GetConsume( ... )
	local cnf = CPPGameLib.GetConfig("EndlessLandBattleCost", (self.endlessLandData.today_battle_times or 0) + 1 , false )
	local t1 = {}
	if cnf then
		if cnf.cost_item_id  then
			local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", cnf.cost_item_id )

			if item_num >= cnf.cost_item_num then
				return item_num , cnf.cost_item_num , cnf.cost_item_id
			elseif cnf.cost_currency_type then
				local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", cnf.cost_currency_type )
				return item_num , cnf.cost_currency_value , cnf.cost_currency_type
			end
		elseif cnf.cost_currency_type then
			local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", cnf.cost_currency_type )
			return item_num , cnf.cost_currency_value , cnf.cost_currency_type
		else
			return
		end
	end 
end

--获得当前波数奖励
function Modules.ChasmModule:GetCurWaveAward( ... )
	local wave = self:GetCurWave()
	local cnf = CPPGameLib.GetConfig("EndlessLandWave", wave )
	local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.ENDLESS_STORE )

	local t1 = {}
	t1[1] = { id = config.big_icon , num = cnf.endless_stone }

	if cnf.reward_item_id_list then
		for i , v in ipairs( cnf.reward_item_id_list ) do
			table.insert( t1 , { id = v , num = cnf.reward_item_num_list[i] } )
		end
	end
	return t1 , cnf.endless_stone
end

--获得当前波数
function Modules.ChasmModule:GetCurWave( ... )
	return self.endlessLandData.cur_wave 
end

--获得最高波数
function Modules.ChasmModule:GetMaxWave( ... )
	local cnf = CPPGameLib.GetConfig("EndlessLandWave", nil , nil , true)
	return #cnf
end

--当前波的怪物是否都刷出来了
function Modules.ChasmModule:IsAllMonsterCreatedOfCurWave(force)
	local wave = self:GetCurWave()
	local config_wave = CPPGameLib.GetConfig("EndlessLandWave", wave)
	if not config_wave then
		return true
	end

	local key, pos_monster_list, prog_index
	for index, _ in ipairs(config_wave.monster_id_list) do
		key = string.format("pos_%d_monster_list", index)
		pos_monster_list = config_wave[key]
		prog_index = self.__create_monster_prog_map[index - 1] or 0
		if pos_monster_list and #pos_monster_list > prog_index then
			return false
		end
	end

	return true
end

--切换到下一波
function Modules.ChasmModule:ChangeWave(for_init)
	--清空掉落物表
	self.__drop_map = nil

	--重置对象ID映射表
	self.__sc_obj_id_map = {}

	--重置刷怪进度表
	self.__create_monster_prog_map = {}

	--重置当前波击杀数
	self.endlessLandData.cur_wave_kill_num = 0

	if not for_init then
		self.endlessLandData.cur_wave = math.min(self.endlessLandData.cur_wave + 1, self:GetMaxWave())
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_WAVE)
end

--获得击杀奖励
function Modules.ChasmModule:GetKillRecord( )
	local cnf = CPPGameLib.GetConfig("EndlessLandKeepKillReward", nil , nil , true)
	local t1 = {}
	for i , v in pairs(cnf) do
		table.insert( t1 , v )
	end

	table.sort( t1, function ( a , b )
		return a.keep_kill_num < b.keep_kill_num
	end )

	local mubiao = 0
	local diamond = 0
	for i , v in ipairs( t1 ) do
		if v.keep_kill_num > self.endlessLandData.last_take_continue_kill_reward then
			mubiao = v.keep_kill_num
			diamond = v.diamond
			break
		end
	end
	print("xy" , "mubiao , diamond " , mubiao , diamond  )
	return mubiao , diamond 
end

--获得当前能不能领取
function Modules.ChasmModule:GetRewardWave(  )
	return self.reward_wave
end

--获得排行榜列表
function Modules.ChasmModule:GetRankLst( )
	for i , v in ipairs( self.rankData.rank_list ) do
		v.rank = i
	end
	return self.rankData.rank_list
end

--获得我的信息
function Modules.ChasmModule:GetMyRank( )
	return self.rankData.rank
end

--获取当前波的怪物(第一批)
function Modules.ChasmModule:GetBattleMonsterList()
	local wave = self:GetCurWave()
	local config_wave = CPPGameLib.GetConfig("EndlessLandWave", wave)
	if config_wave then
		local monster_list = {}
		for i, id in ipairs(config_wave.monster_id_list) do
		    local monster_info = Modules.BattleFormula:BuildBattleMonsterInfo(id, nil, nil, config_wave.add_pro_type_list, config_wave.add_pro_value_list)
		    monster_info.index = i - 1
			table.insert(monster_list, monster_info)
		end		
		Modules.BattleFormula:SortBattleMonsterList(monster_list)		
		return monster_list
	end
end

--获取当前波的怪物血量上限(所有)
function Modules.ChasmModule:GetBattleMonsterTotalHP()
	local total_hp = 0
	local wave = self:GetCurWave()
	local config_wave = CPPGameLib.GetConfig("EndlessLandWave", wave)
	if config_wave then
		local key, monstr_info
		for i, monster_id in ipairs(config_wave.monster_id_list) do
			monstr_info = Modules.BattleFormula:BuildBattleMonsterInfo(monster_id, nil, nil, config_wave.add_pro_type_list, config_wave.add_pro_value_list)
			total_hp = total_hp + monstr_info.hp

			key = string.format("pos_%d_monster_list", i)
			if config_wave[key] then
				for _, sub_monster_id in ipairs(config_wave[key]) do
					monstr_info = Modules.BattleFormula:BuildBattleMonsterInfo(sub_monster_id, nil, nil, config_wave.add_pro_type_list, config_wave.add_pro_value_list)
					total_hp = total_hp + monstr_info.hp
				end
			end
		end		
	end	
	return total_hp
end

--深渊商店
function Modules.ChasmModule:ShopCnf()
	if self.shopCnf then
		return 
	end
	self.shopCnf = {}
	self.awardCnf = {}
	local cnf = CPPGameLib.GetConfig("EndlessLandShop", nil , nil , true)
	for i , v in pairs( cnf ) do
		if v.goods_type == Macros.Game.CHASM_SHOP.SHOP then
			table.insert( self.shopCnf , v )
		else
			table.insert( self.awardCnf , v )
		end
	end

	table.sort( self.shopCnf, function ( a , b )
		return a.sort_id < b.sort_id
	end )

	table.sort( self.awardCnf, function ( a , b )
		return a.sort_id < b.sort_id
	end )
end

--获得深渊商店表
function Modules.ChasmModule:GetShopCnf()
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( self.shopCnf ) do
		local info = self:GetChasmRequest( v.id ) 
		if info then
			if v.buy_num_max ~= 0 and info.buy_num >= v.buy_num_max then
				table.insert( t2 , v )
			else
				table.insert( t1 , v )
			end
		else
			table.insert( t1 , v )
		end
	end
	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end
	return t1
end

--获得深渊奖励表
function Modules.ChasmModule:GetAwardCnf()
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( self.awardCnf ) do
		local info = self:GetChasmRequest( v.id ) 
		if info then
			if v.buy_num_max ~= 0 and info.buy_num >= v.buy_num_max then
				table.insert( t2 , v )
			else
				table.insert( t1 , v )
			end
		else
			table.insert( t1 , v )
		end
	end
	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end
	return t1
end

--获得排名奖励
function Modules.ChasmModule:GetRankReward(  )  
	local t1 = {}
	for k , v in pairs( CPPGameLib.GetConfig("EndlessLandRankReward", nil, nil , true ) ) do

		local text = v.rank_min 
		if v.rank_min ~= v.rank_max then
			text = v.rank_min .."~" .. v.rank_max
		end

		local t2 = {}
		for i , vv in ipairs( v.item_id_list ) do
			table.insert( t2 , { id = vv , num = v.item_num_list[i] } )
		end

		table.insert( t1 , { text = text , item = t2 , title_id = v.title_id , sort = k } )
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

--正常世界
--tab { 背景 , 背景 } ， 速度 
function Modules.ChasmModule:GetBackground()
	local background = {}
	background[1] = {}
	background[1].bglst = { "chasm_bg_4_1" , "chasm_bg_4_2" , "chasm_bg_4_3"  }
	background[1].speed = 20    --S/像素
	background[1].anchor = { 0.5 , 0.5 } -- 锚点 
	background[1].positionX = 0
	background[1].positionY = Macros.Global.DesignSize.height / 2 + (419/2)   -- Y轴
	background[1].content = { 1280, Macros.Global.DesignSize.height }  -- 大小

	background[2] = {}
	background[2].bglst = { "chasm_bg_3_1" , "chasm_bg_3_2" , "chasm_bg_3_3"  }
	background[2].speed = 40    --S/像素
	background[2].anchor = { 0.5 , 0.5 } -- 锚点 
	background[2].positionX = 0
	background[2].positionY = Macros.Global.DesignSize.height / 2  -- Y轴
	background[2].content = { 1416, Macros.Global.DesignSize.height }  -- 大小

	background[3] = {}
	background[3].bglst = { "chasm_bg_2_1" , "chasm_bg_2_2" }
	background[3].speed = 50    --S/像素
	background[3].anchor = { 0.5 , 0.5 } -- 锚点 
	background[3].positionX = -100
	background[3].positionY = Macros.Global.DesignSize.height / 2  -- Y轴
	background[3].content = { 1537, Macros.Global.DesignSize.height }  -- 大小

	background[4] = {}
	background[4].bglst = { "chasm_bg_1_1" , "chasm_bg_1_2" , "chasm_bg_1_3"  }
	background[4].speed = 110    --S/像素
	background[4].anchor = { 0.5 , 0 } -- 锚点 
	background[4].positionX = -200
	background[4].positionY = 0  -- Y轴
	background[4].content = { 1280, 269 }  -- 大小

	return background
end

--加速世界
--tab { 背景 , 背景 } ， 速度 
function Modules.ChasmModule:GetAccelerateBackground()
	local background = {}
	background[1] = {}
	background[1].bglst = { "speed_bg" , "speed_bg"   }
	background[1].speed = 0   --S/像素
	background[1].anchor = { 0.5 , 0.5 } -- 锚点 
	background[1].positionX = 0
	background[1].positionY = Macros.Global.DesignSize.height / 2  -- Y轴
	background[1].content = { Macros.Global.DesignSize.width, Macros.Global.DesignSize.height }  -- 大小

	return background
end

--处理怪物VO添加
function Modules.ChasmModule:HandleMonsterVOAdd(vo)
	local key = self:__GetMonsterKey(vo.index, vo.info_id)
	self.__sc_obj_id_map[key] = vo.obj_id
end

--处理怪物VO移除
function Modules.ChasmModule:HandleMonsterVORemove(vo)
	local key = self:__GetMonsterKey(vo.index, vo.info_id)

	if self.__sc_obj_id_map[key] == vo.obj_id then
		self.__sc_obj_id_map[key] = nil	
	end
end

--生成掉落物信息
function Modules.ChasmModule:GenerateDropInfo(obj_id)
	local drop_info = self.__drop_map and self.__drop_map[obj_id]
	if drop_info and #drop_info.item_list > 0 then
		drop_info.img_name = "img_dropbox"
		self.__drop_map[obj_id] = nil
		return drop_info
	end
end

--是否首次挑战(1=是,0=否)
function Modules.ChasmModule:GetIsFirstBattle()
	return self.is_first_battle
end

--累计获得的无尽石
function Modules.ChasmModule:SetTotalObtain( index , set )
	if set then
		self.obtain = index or 0
	else
		self.obtain = (self.obtain or 0) + (index or 0)
	end
end

--累计获得的无尽石
function Modules.ChasmModule:GetTotalObtain(  )
	return self.obtain
end

--获得进入次数
function Modules.ChasmModule:GetEntranceTime()
	-- VIP次数
	local vipTimes = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.CHASM_TIMES)
	--免费次数
	local cnf = CPPGameLib.GetConfig("EndlessLandBattleCost", nil , nil , true )
	local freeTimes = 0 
	for k , v in pairs( cnf ) do
		if (not v.cost_item_id) and (not v.cost_currency_type) then
			freeTimes = freeTimes + 1
		end
	end

	--已经进入次数 , 今日可进入次数
	return self.endlessLandData.today_battle_times or 0 , vipTimes , freeTimes
end

--获得升级VIP所能获得次数
function Modules.ChasmModule:GetVipEntranceTips()
	--下级次数
	local nexLevel , nexTimes = CallModuleFunc( ModuleType.VIP, "ExtendPack" , Macros.Game.VIPType.CHASM_TIMES )
	return nexLevel , nexTimes
end
























--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取怪物key
function Modules.ChasmModule:__GetMonsterKey(index, info_id)
	local key = string.format("%d_%d", index, info_id)	
	return key
end

--将服务器对象ID转换为本地对象ID
function Modules.ChasmModule:__ConvertSID2CID(index, info_id)
	local key = self:__GetMonsterKey(index, info_id)
	return self.__sc_obj_id_map[key]
end

--处理刷怪
function Modules.ChasmModule:__HandleCreateMonster(index, killed_obj_id)
	local cur_wave = self:GetCurWave()
	local config_wave = CPPGameLib.GetConfig("EndlessLandWave", cur_wave)
	if not config_wave then
		return
	end

	local monster_id_list = config_wave[string.format("pos_%d_monster_list", index + 1)]
	if not monster_id_list then
		return
	end

	--待恢复怪物槽列表
	local resume_monster_index_list

	if self.__created_senior_monster then
		local killed_obj = GlobalScene:GetObj(killed_obj_id)
		if killed_obj then
			local killed_obj_vo = killed_obj:GetVO()
			if Macros.Game.IsSeniorMonster(killed_obj_vo.info_id) then				
				self.__created_senior_monster = nil

				--存活怪物槽列表
				local alive_monster_index_map = {}
				local monster_vo_map = GlobalVOMgr:GetMonsterMap()
				for obj_id, monster_vo in pairs(monster_vo_map) do
					alive_monster_index_map[monster_vo.index] = true
				end

				--待恢复怪物槽列表
				for index, _ in ipairs(config_wave.monster_id_list) do
					if not alive_monster_index_map[index] then						
						resume_monster_index_list = resume_monster_index_list or {}
						table.insert(resume_monster_index_list, index)
					end
				end							
			else
				--高级怪在场，小怪禁止出场
				return
			end
		end
	end

	local prog_index = self.__create_monster_prog_map[index] or 0
	prog_index = prog_index + 1

	local monster_id = monster_id_list[prog_index]
	if monster_id then
		self.__create_monster_prog_map[index] = prog_index
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "CreateMonsterDynamic", {index = index, info_id = monster_id})

		--检测高级怪出场
		if not self.__created_senior_monster then
			self.__created_senior_monster = Macros.Game.IsSeniorMonster(monster_id)
		end
	end

	--高级怪死亡，小怪恢复出场
	if not self.__created_senior_monster and resume_monster_index_list then
		for _, index in ipairs(resume_monster_index_list) do
			self:__HandleCreateMonster(index)
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.ChasmModule:CheckBattleCondition( type_ )
	return true
end