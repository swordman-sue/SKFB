--[[
%% @module: 世界boss模块
%% @author: yjg
%% @created: 2016年11月17日15:33:19
--]]


Modules = Modules or {}

Modules.WorldBossModule = Modules.WorldBossModule or BaseClass(BaseModule)

Modules.WorldBossModule.REWARD_TYPE = {}
Modules.WorldBossModule.REWARD_TYPE.HONOR = 1
Modules.WorldBossModule.REWARD_TYPE.KILL = 2

function Modules.WorldBossModule:__init()
	self.bossInfo = {}
	self.recordDraw = {}
	self.rankList = {}

	self.rankAward = {}
	self.rankAward[Macros.Game.RankType.WORLDBOSS_HONOR] = {}
	self.rankAward[Macros.Game.RankType.WORLDBOSS_HARM] = {}

	self:SettleRankAward()
end

-- 消息说明: 世界BOSS数据请求 消息Id: 22501
function Modules.WorldBossModule:WorldBossDataReq(info)
	-- CPPGameLib.PrintTable("xy" , info,"世界BOSS数据请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_DATA_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- "boss_level__H",    --BOSS等级
-- "camp__C",      --选择的阵营(99=表示还木有选择)
-- "battle_times__C",  --挑战次数(已经使用的)
-- "last_recovery_time__I",--上次恢复时间(挑战次数)
-- "buy_battle_times__C",  --已经购买的挑战次数
-- "honor__I",     --我的荣誉
-- "damage__I",        --我的伤害
-- "relive_time__I",   --BOSS复活时间(0=表示还木有挂掉)
-- "remain_hp__I",     --BOSS剩余血量
-- "rank_list__T__honor@I##role_name@s", -- 排行列表

-- 消息说明: 世界BOSS数据响应 消息Id: 22502
function Modules.WorldBossModule:WorldBossDataResp(info)
	CPPGameLib.PrintTable("xy" , info,"世界BOSS数据响应")
	self.bossInfo = info
	--宣告全世界boss出世
	GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_INFO)
end

-- 消息说明: 选择阵营请求 消息Id: 22503
function Modules.WorldBossModule:WorldBossSelectCampReq(info)
	CPPGameLib.PrintTable("xy" , info,"选择阵营请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_SELECT_CAMP_REQ)
	data.camp = info.camp
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 选择阵营响应 消息Id: 22504
function Modules.WorldBossModule:WorldBossSelectCampResp(info)
	CPPGameLib.PrintTable("xy" , info,"选择阵营响应")
	--阵营修复
	self.bossInfo.camp = info.camp
end

-- 消息说明: 领取奖励请求 消息Id: 22505
function Modules.WorldBossModule:WorldBossRewardReq(info)
	CPPGameLib.PrintTable("xy",info,"领取奖励请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_REWARD_REQ)
	data.reward_type = info.reward_type
	data.reward_list = info.reward_list
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取奖励响应 消息Id: 22506
function Modules.WorldBossModule:WorldBossRewardResp(info)
	CPPGameLib.PrintTable("xy",info,"领取奖励响应")
	local cnf = nil
	if info.reward_type == Modules.WorldBossModule.REWARD_TYPE.HONOR then
		cnf = self:GetHonor( )
	elseif info.reward_type == Modules.WorldBossModule.REWARD_TYPE.KILL then
		cnf = self:GetKill( )
	end

	local t2 = {}
	for k,v in pairs(cnf) do
		t2[v.id] = v
	end

    local t1 = {}
    for i,v in ipairs(info.reward_list) do
    	local lst = t2[v.reward_id]
	    table.insert(t1,{id = lst.reward_item_id , num = lst.reward_item_num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:UpRecordDraw(info)
end

-- 消息说明: 领取奖励记录请求 消息Id: 22507
function Modules.WorldBossModule:WorldBossRewardRecordReq(info)
	CPPGameLib.PrintTable(info,"领取奖励记录请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_REWARD_RECORD_REQ)
	data.reward_type = info.reward_type
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 领取奖励记录响应 消息Id: 22508
function Modules.WorldBossModule:WorldBossRewardRecordResp(info)
	CPPGameLib.PrintTable("xy" , info , "领取奖励记录响应")
	self:UpRecordDraw(info)
end

-- 消息说明: 世界BOSS战报请求 消息Id: 22509
function Modules.WorldBossModule:WorldBossBattleRecordReq(info)
	CPPGameLib.PrintTable("xy" , info,"世界BOSS战报请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_BATTLE_RECORD_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 世界BOSS战报响应 消息Id: 22510
function Modules.WorldBossModule:WorldBossBattleRecordResp(info)
	CPPGameLib.PrintTable("xy" , info , "世界BOSS战报响应")
	self.score = info.record_list
	GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_SCORE)
end

-- 消息说明: 世界BOSS挑战次数购买 消息Id: 22511
function Modules.WorldBossModule:WorldBossBattleTimesBuyReq(info)
	CPPGameLib.PrintTable(info,"世界BOSS挑战次数购买")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ)
	data.buy_times = info.buy_times
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 世界BOSS挑战次数购买响应 消息Id: 22512
function Modules.WorldBossModule:WorldBossBattleTimesBuyResp(info)
	CPPGameLib.PrintTable(info,"世界BOSS挑战次数购买响应")
end

-- 消息说明: 世界BOSS伤害公告 消息Id: 22513
function Modules.WorldBossModule:WorldBossDamageNotify(info)
	CPPGameLib.PrintTable("xy",info,"世界BOSS伤害公告")
	self.bossInfo.remain_hp = info.cur_total_hp
	GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_NOTICE , info )
	if info.cur_total_hp <= 0 then
		self:WorldBossDataReq()
	end
end

-- 消息说明: 世界BOSS活动开始 消息Id: 22514
function Modules.WorldBossModule:WorldBossActivityStartNotify()
	CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.WORLDBOSS)
end

-- 消息说明: 世界BOSS活动结束 消息Id: 22515
function Modules.WorldBossModule:WorldBossActivityOverNotify()
	CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.WORLDBOSS)
end


--请求排行榜
function Modules.WorldBossModule:RankReq(info)
	CPPGameLib.PrintTable("xy" , info,"请求排行榜")
	if self.rankList[info.type] and self.rankList[info.type][info.sub_type] then
		GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_RANK)
	else
		local data = {}
		data.type = info.type
		data.sub_type = info.sub_type
		CallModuleFunc(ModuleType.RANK, "RankListRequest",info)
	end
end

--排行榜回调
function Modules.WorldBossModule:RankResp(info)
	CPPGameLib.PrintTable("xy" , info,"排行榜回调")
	self:LocalRankRecord(info)
end

--获得阵营选择
function Modules.WorldBossModule:GetCamp()
	return self.bossInfo.camp
end

--获得Boss等级
function Modules.WorldBossModule:GetBossLevel()
	return self.bossInfo.boss_level
end

--获得boss生存状态
function Modules.WorldBossModule:GetBossHP()

	local bossCnf = CPPGameLib.GetConfig("WorldBoss", self.bossInfo.boss_level)
	--关卡信息
	local level = CPPGameLib.GetConfig("GameLevel", tonumber(bossCnf.map_id))
	local hp = ConfigAdapter.Battle.GetMonsterTotalHP(level.battle_list[1])

	return self.bossInfo.remain_hp  , hp
end

--获得我的boss荣誉
function Modules.WorldBossModule:GetBossHonor()
	return self.bossInfo.honor , self.bossInfo.honor_rank 
end

--获得我的boss伤害
function Modules.WorldBossModule:GetBossDamage()
	return self.bossInfo.damage , self.bossInfo.damage_rank 
end

--当前可挑战的次数
function Modules.WorldBossModule:GetNowChallengeTime()
	--默认次数
	local default = ConfigAdapter.Common.GetWorldBossTime()
	--加上附加的次数
	default = (default - self.bossInfo.battle_times) + self.bossInfo.buy_battle_times
	return default
end

--次数回复时间
function Modules.WorldBossModule:GetReplyTime()
	if self.bossInfo.battle_times == 0 then
		return 0
	end
	local server_time =  self.bossInfo.last_recovery_time + ( ConfigAdapter.Common.GetWorldBossChallengeCD() )
	return server_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
end

--复活时间
function Modules.WorldBossModule:GetReliveTime()
	if self.bossInfo.relive_time ~= 0 then
		return self.bossInfo.relive_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	else
		return self.bossInfo.relive_time
	end
end

--获得界面上的榜单
function Modules.WorldBossModule:GetInterfaceRank()
	return self.bossInfo.rank_list
end


--本地保存一份排行榜的列表
function Modules.WorldBossModule:LocalRankRecord( info )
	if not self.rankList[info.type] then
		self.rankList[info.type] = {}
	end
	self.rankList[info.type][info.sub_type] = {}

	table.sort( info.rank_list, function ( a , b )
		return a.context > b.context
	end )

	for i,v in ipairs(info.rank_list) do
		v.rank = i
		table.insert(self.rankList[info.type][info.sub_type] , v )
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_RANK)
end

function Modules.WorldBossModule:GetLocalRankRecord( info )
	return self.rankList[info.type][info.sub_type]
end

--销毁排行榜的列表
function Modules.WorldBossModule:LocalRankRemove( info )
	self.rankList = {}
end

--获得战报
function Modules.WorldBossModule:GetBossScore(  )
	return self.score
end

-- --获得等级信息
-- function Modules.WorldBossModule:SetShowUI()
-- 	--boss信息
-- 	local bossCnf = CPPGameLib.GetConfig("WorldBoss", self.bossInfo.boss_level)
-- 	--关卡信息
-- 	local level = CPPGameLib.GetConfig("GameLevel", tonumber(bossCnf.map_id))
-- 	local info = {}
-- 	info.name = level.name
-- 	info.hp = ConfigAdapter.Battle.GetMonsterTotalHP(level.battle_list[1])
-- 	info.maxHp = ConfigAdapter.Battle.GetMonsterTotalHP(level.battle_list[1])
-- 	info.lv = self.bossInfo.boss_level
-- 	info.isOpen = self:isOpen()
-- 	info.icon = bossCnf.icon
-- 	info.honor = self.bossInfo.honor
-- 	info.damage = self.bossInfo.damage
-- 	local view = self:GetView()
-- 	if view and view:IsOpen() then
-- 		-- view:SetShowUI(info)
-- 	end
-- end

--是否开启
function Modules.WorldBossModule:isOpen()
	local nowTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(nowTime)
	--获得有没有这个boss
	local cnf = CPPGameLib.GetConfig("WorldBossOpenTime", tonumber(week) , false )
	--当前时间组合
	local nowGroupTime = hour..minute
	if cnf and tonumber(nowGroupTime) >= tonumber(cnf.start_time) and tonumber(nowGroupTime) < tonumber(cnf.over_time) then
		local bH = string.sub( string.format( "%04d" , cnf.start_time ) , 0 , 2 )
		local bM = string.sub( string.format( "%04d" , cnf.start_time ) , 3 , 4 )
		local eH = string.sub( string.format( "%04d" , cnf.over_time ) , 0 , 2 )
		local eM = string.sub( string.format( "%04d" , cnf.over_time ) , 3 , 4 )
		return ((eH * 60 * 60) + (eM *60)-(bH * 60 * 60) + (bM *60))-( ( (hour * 60 * 60 ) + (minute * 60) + second )- (bH * 60 * 60) + (bM *60))
	else
		return false 
	end
end

--开启时间
function Modules.WorldBossModule:OpenTime()
    local lst = {}
    for i,v in pairs(CPPGameLib.GetConfig("WorldBossOpenTime", nil, false , true )) do
        table.insert( lst , {
        						week = v.week_day , 
        						start_time = v.start_time , 
        						over_time = v.over_time , 
        						text = CPPGameLib.GetString("worldBoss_openWeek",i) 
        					})
    end

    table.sort( lst, function ( a , b )
        return a.week < b.week
    end )

    local text = ""
    for i,v in ipairs(lst) do
        if i ~= #lst and i ~= 1 then
            text = text .. CPPGameLib.GetString("common_Comma") .. v.text
		elseif i == 1 then
			text = text .. v.text
		elseif i == #lst then
			text = text.. CPPGameLib.GetString("common_Comma") .. v.text
        end
    end	

	local bH = string.sub( lst[1].start_time , 0 ,-3 )
	local bM = string.sub( lst[1].start_time  , -2)

	local eH = string.sub( lst[1].over_time  , 0 ,-3 )
	local eM = string.sub( lst[1].over_time  , -2)

	return text .." " .. bH..":"..bM.."~"..eH..":"..eM
end

--本地保存一份领取的列表
function Modules.WorldBossModule:UpRecordDraw(info)
	if self.recordDraw[info.reward_type] then
		for i,v in ipairs(info.reward_list) do
			if not self.recordDraw[info.reward_type][v.reward_id] then
				self.recordDraw[info.reward_type][v.reward_id] = true
			end
		end
	else
		self.recordDraw[info.reward_type] = {}
		local t1 = {}
		for i,v in ipairs(info.reward_list) do
			t1[v.reward_id] = true
		end

		self.recordDraw[info.reward_type] = t1
	end
	self:UpShowList(info.reward_type)
end

--刷新奖励显示列表
function Modules.WorldBossModule:UpShowList(type)
	GlobalEventSystem:FireNextFrame(Macros.Event.WorldBossModule.WORLDBOSS_AWARD , type , self.recordDraw[type] )
end

--奖励记录
function Modules.WorldBossModule:GetRecordDraw(type)
	return self.recordDraw[type]
end

--个人荣誉表
function Modules.WorldBossModule:GetHonor()
	local cnf = CPPGameLib.GetConfig("WorldBossNonorReward", nil, nil , true )
	local t1 = {}
	local t2 = {}
	local record = self:GetRecordDraw( Modules.WorldBossModule.REWARD_TYPE.HONOR )

	for i,v in ipairs(cnf) do
		local type_ = false
		if v.need_honor <= self.bossInfo.honor then
			type_ = true
		end

		if record and record[i] then
			table.insert(t2 , { 	
									id = i,
									need_honor = v.need_honor , 
									reward_item_id = v.reward_item_id , 
									reward_item_num = v.reward_item_num ,
									type = type_  , 
									enum = Modules.WorldBossModule.REWARD_TYPE.HONOR,
									my_value = self:GetBossHonor() .."/" .. v.need_honor,
									lab = CPPGameLib.GetString("worldBoss_honorAwardGet" , v.need_honor )})
		else
			table.insert(t1 , { 	
									id = i,
									need_honor = v.need_honor , 
									reward_item_id = v.reward_item_id , 
									reward_item_num = v.reward_item_num ,
									type = type_  , 
									enum = Modules.WorldBossModule.REWARD_TYPE.HONOR,
									my_value = self:GetBossHonor() .."/" .. v.need_honor,
									lab = CPPGameLib.GetString("worldBoss_honorAwardGet" , v.need_honor )})
		end
	end

	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end

	return t1
end

--击杀全服奖励
function Modules.WorldBossModule:GetKill()
	local cnf = CPPGameLib.GetConfig("WorldBossKillReward", nil, nil , true )
	local t1 = {}
	local t2 = {}
	local record = self:GetRecordDraw( Modules.WorldBossModule.REWARD_TYPE.KILL )

	for i,v in ipairs(cnf) do
		local type_ = false

		if v.need_boss_lv <= (self:GetBossLevel() - 1) then
			type_ = true
		end

		if record and record[i] then
			table.insert(t2 , { 	
									id = i,
									need_honor = v.need_boss_lv , 
									reward_item_id = v.reward_item_id , 
									reward_item_num = v.reward_item_num ,
									type = type_ , 
									enum = Modules.WorldBossModule.REWARD_TYPE.KILL,
									my_value = self:GetBossLevel() - 1 .."/" .. v.need_boss_lv,
									lab = CPPGameLib.GetString("worldBoss_bossLvAwardGet" , v.need_boss_lv )})
		else
			table.insert(t1 , { 	
									id = i,
									need_honor = v.need_boss_lv , 
									reward_item_id = v.reward_item_id , 
									reward_item_num = v.reward_item_num ,
									type = type_ , 
									enum = Modules.WorldBossModule.REWARD_TYPE.KILL,
									my_value = self:GetBossLevel() - 1 .."/" .. v.need_boss_lv,
									lab = CPPGameLib.GetString("worldBoss_bossLvAwardGet" , v.need_boss_lv )})
		end
	end

	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end
	return t1
end

--获取一键领取列表
function Modules.WorldBossModule:AutoGet(type)
	--荣誉表
	local lst = {}
	if type == Modules.WorldBossModule.REWARD_TYPE.HONOR then
		lst = self:GetHonor()
	elseif type == Modules.WorldBossModule.REWARD_TYPE.KILL then
		lst = self:GetKill()
	end

	local t1 = {}
	for i,v in ipairs(lst) do
		if v.type ~= false then
			table.insert(t1 , {reward_id = v.id})
		end
	end

	local t2 = {}
	for i,v in ipairs( t1 ) do
		if not self.recordDraw[type] then
			table.insert(t2 , {reward_id = v.reward_id} )
		elseif not self.recordDraw[type][v.reward_id] then
			table.insert(t2 , {reward_id = v.reward_id} )
		end
	end

	local info = {}
	info.reward_type = type
	info.reward_list = t2

	return info
end

--整理排行奖励表
function Modules.WorldBossModule:SettleRankAward(type)
	local cnf = CPPGameLib.GetConfig("WorldBossRankReward", nil, nil , true )
	local index = false
	for rank,value in ipairs(cnf) do
		local info = {}
		local info_1 = {}
		for i=1,#value.nonor_reward_id_list do
			table.insert(info, { item = value.nonor_reward_id_list[i] , itemNum = value.nonor_reward_num_list[i]  } )
			
			table.insert(info_1, { item = value.damage_reward_id_list[i] , itemNum = value.damage_reward_num_list[i]  } )
		end

		info.rank = rank
		info_1.rank = rank
		if index == false then
			info.show = true
			info_1.show = true
			index = true
		else 
			info.show = false
			info_1.show = false
			index = false
		end
		table.insert( self.rankAward[Macros.Game.RankType.WORLDBOSS_HONOR] , info )
		table.insert( self.rankAward[Macros.Game.RankType.WORLDBOSS_HARM] , info_1 )
	end

end

--获得排行奖励
function Modules.WorldBossModule:GetRankAward(type)
	return self.rankAward[type]
end

-- --获得主界面显示
-- function Modules.WorldBossModule:GetMainShowData(type)
-- 	return self.bossInfo.rank_list[type]
-- end



-- --当前CD中的次数
-- function Modules.WorldBossModule:GetNowChallengeTimeCD()
-- 	local default = ConfigAdapter.Common.GetWorldBossCDTime()
-- 	local overTime = self.bossInfo.last_recovery + (default*60)
-- 	local nowTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	
-- 	if nowTime - overTime <= 0 then
-- 		return 0
-- 	else
-- 		return nowTime - overTime
-- 	end
-- end

--当前可购买次数
function Modules.WorldBossModule:GetWorldBossBuyTime()
	local default = CallModuleFunc(ModuleType.VIP, "GetPrivilege",Macros.Game.VIPType.WORLDBOSS_BUY_TIME)
	return default - self:GetWorldBossUsedUpBuyTime()
end

function Modules.WorldBossModule:GetWorldBossUsedUpBuyTime()
	return self.bossInfo.buy_battle_times
end

function Modules.WorldBossModule:GetGameLevelID()
	local config_world_boss = CPPGameLib.GetConfig("WorldBoss", self.bossInfo.boss_level)
	return config_world_boss.map_id
end

-------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.WorldBossModule:CheckBattleCondition()
    local challengeTime = CallModuleFunc(ModuleType.WORLDBOSS, "GetNowChallengeTime")
    if challengeTime ~= 0 then
    	return true
    end
end