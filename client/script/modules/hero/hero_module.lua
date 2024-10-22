
--[[
%% @module: 英雄模块
%% @author: swordman sue
%% @created: 2016-08-19
--]]

Modules = Modules or {}

Modules.HeroModule = Modules.HeroModule or BaseClass(BaseModule)

--升级材料数量上限
Modules.HeroModule.MaxUpgradeMaterialNum = 5

--进阶材料数量上限
Modules.HeroModule.MaxBreakMaterialNum = 2

local sort_hero_info_list

function Modules.HeroModule:__init()
	self.__can_update = true
	self.__update_interval = 1
    self:__RegistProtocolForOpenView(BaseViewType.HERO_RECRUIT,Net.ProtocolNo.C2S_HERO_RECRUITDATA_REQ, false)
    self:__RegistProtocolForOpenView(BaseViewType.HERO_HANDBOOK,Net.ProtocolNo.C2S_HERO_COLLECTION_LIST_REQ, false)

	self.__red_dot_info_map = {} 
	self.__maincity_lineup_hero_reddot_list = {}
	self.__mark_divination_strengthen_list = {}
	self.__mark_divination_break_list = {}
	self.__mark_is_divination_could_strengthen_list = {}
	self.__mark_is_divination_could_break_list = {}	
end

function Modules.HeroModule:__delete()
	self:DumpViews()
end

function Modules.HeroModule:DumpViews()
	GlobalViewMgr:CloseViews({[BaseViewType.HERO_LINEUP] = true}, BaseView.CloseMode.DESTROY, true, true)
end

function Modules.HeroModule:Update(now_time, elapse_time)
	--保存新增记录表
	if self.__is_need_save_new_record_map then
		self.__is_need_save_new_record_map = false

	    local text = ""
	    for k , v in pairs( self.__new_record_map ) do
	        text = text .. k .. "<GH>"
	    end
	    Cookies.Set( Cookies.LevelType.ROLE , Macros.Setting.NewHero , text)		

	    self:FireNextFrame(Macros.Event.NewModule.PACK_EXTEND, Macros.Game.SystemID.HERO_BAG)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--英雄基础信息列表
function Modules.HeroModule:HeroBasicListResp(protocol_data)
    self.__hero_info_map = {}
    for _, hero_info in ipairs(protocol_data.hero_list) do
    	hero_info.train_pro_list = hero_info.train_pro_list:GetData()
    	hero_info.awaken_item_list = hero_info.awaken_item_list:GetData()
    	local hero_config = CPPGameLib.GetConfig("Hero",hero_info.info_id)
		hero_info.type = hero_config.type
    	self.__hero_info_map[hero_info.uid] = hero_info
    end

    --初始化新增记录表
    local lst = CPPGameLib.SplitEx( Cookies.Get( Cookies.LevelType.ROLE  ,Macros.Setting.NewHero) , "<GH>")
    for k , v in pairs( lst ) do
        self:AddToNewRecordMap( tonumber(v) , true )
    end
end

--英雄基础信息改变
function Modules.HeroModule:HeroBasicInfoResp(protocol_data)
	if not self.__hero_info_map then
		return
	end
	
	for _, hero_info in ipairs(protocol_data.hero_base_data_list) do
		hero_info.train_pro_list = hero_info.train_pro_list:GetData()
    	hero_info.awaken_item_list = hero_info.awaken_item_list:GetData()		

		local old_info = self.__hero_info_map[hero_info.uid]
		if protocol_data.flag == Macros.Global.TRUE then	
			local is_update = old_info ~= nil
			self.__hero_info_map[hero_info.uid] = hero_info

			--增加英雄
			if not is_update then
				self:AddToNewRecordMap(hero_info.uid)
				self:FireNextFrame(Macros.Event.HeroModule.HERO_ADD, hero_info)								

			--更新英雄
			else
				self:FireNextFrame(Macros.Event.HeroModule.HERO_UPDATE, hero_info)
			end		

		--删除英雄
		else
			self.__hero_info_map[hero_info.uid] = nil	
			self:DelFrmNewRecordMap(hero_info.uid)	
			self:FireNextFrame(Macros.Event.HeroModule.HERO_REMOVE, old_info)	
		end

		--改变英雄
		self:FireNextFrame(Macros.Event.HeroModule.HERO_CHANGE, hero_info)			
	end
end

--添加到新增记录表
function Modules.HeroModule:AddToNewRecordMap( id , ignore_save )   
    self.__new_record_map = self.__new_record_map or {}

    local item_config = self:GetHeroInfo( id )
    if item_config then
	    item_config = CPPGameLib.GetConfig("Hero", item_config.info_id , false)
	    if item_config.new then
	        self.__new_record_map[id] = self.__new_record_map[id] or {}
	        self.__new_record_map[id].type = true

	        --保存
	        if not ignore_save then
	            self:SaveNewRecordMap()
	        end
	    end
	end
end

--获得新增记录
function Modules.HeroModule:GetNewRecord( id )   
	if not self.__new_record_map then
		return
	end

	if id then
        return self.__new_record_map[id]
    end
end

--获取新增记录表
function Modules.HeroModule:GetNewRecordMap()
    return self.__new_record_map
end

--从新增记录表中删除
function Modules.HeroModule:DelFrmNewRecordMap( id , ignore_save )  
    if not self.__new_record_map then
    	return
    end

    if self.__new_record_map[id] then
	    self.__new_record_map[id] = nil

        --保存
        if not ignore_save then 
            self:SaveNewRecordMap()
        end
	end
end

--保存新增记录表到文件
function Modules.HeroModule:SaveNewRecordMap()   
    if not self.__new_record_map then
    	return
    end

    self.__is_need_save_new_record_map = true
end

--请求英雄详细信息
function Modules.HeroModule:HeroDetailInfoReq(uid)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_DETAIL_INFO_REQ)
	protocol.uid = uid
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.HeroModule:HeroDetailInfoResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_DETAIL_INFO_RESP, protocol_data)
end

--英雄阵位列表
function Modules.HeroModule:HeroLineupListResp(protocol_data)
	--英雄阵容列表
	self.__hero_lineup_map = {}
	self.__hero_lineup_list = protocol_data.formation_list
	for _, lineup_info in ipairs(self.__hero_lineup_list) do
		local hero_info = self:GetHeroInfo(lineup_info.uid)
		lineup_info.divination_list = lineup_info.card_list:GetData()
		if hero_info then
			self.__hero_lineup_map[lineup_info.uid] = lineup_info
		end
	end

	--援军阵容列表
	self.__hero_reinforcement_list = protocol_data.reinforcement_list
	for _, lineup_info in ipairs(self.__hero_reinforcement_list) do
		local hero_info = self:GetHeroInfo(lineup_info.uid)
		if hero_info then
			hero_info.is_reinforcement = true
			self.__hero_lineup_map[lineup_info.uid] = lineup_info
		end
	end

	--计算队伍属性加成
	self:__CalcTeamPlusProps()	

	--发送事件
	self:FireNextFrame(Macros.Event.HeroModule.HERO_LINEUP_UPDATE)
end

--请求英雄上阵
function Modules.HeroModule:HeroGotoBattleReq(uid, pos)	
	local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(pos)
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	if not pos_info or pos_info.need_level > role_info.level then
		--等级不足
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lineup_invalid_level"))
		return
	end

	self:SaveHeroPropertyBeforeChange(uid,pos)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_GOTO_BATTLE_REQ)
	protocol.uid = uid
	protocol.pos = pos
	GlobalNetAdapter:OnSend(protocol)
end

--英雄上阵响应
function Modules.HeroModule:HeroGotoBattleResp(protocol_data)
	local view = self:GetViewByType(BaseViewType.HERO_LINEUP)
	if view and view:IsOpen() then
		view:HeroGotoBattleResp()
	end			

	self:SetIsHaveChangePropertyNeedWindWords(true)
	self:SetMessageTypeAndTranslateInfo(Macros.Game.WIND_WORDS_LINEUP_TYPE.LINEUP_MESSGAE) 

	self:FireNextFrame(Macros.Event.HeroModule.HERO_ON_OR_OFF_FIGHT, true)  

	--指引下一步
	CallModuleFunc(ModuleType.LEAD, "NextSectionByLogic", Macros.Game.NecessaryLeadKeyPointFilterType.HERO_ON_FIGHTING)
end

--升级请求
function Modules.HeroModule:HeroUpgradeReq(uid, cost_hero_list)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_UPGRADE_REQ)
	protocol.uid = uid
	protocol.cost_hero_list = cost_hero_list
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.HeroModule:HeroUpgradeResp(protocol_data)	
	self:FireNextFrame(Macros.Event.HeroModule.HERO_UPGRADE_SUCC, protocol_data.uid)	

	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.HERO_UPGRADE)
end

--进阶请求
function Modules.HeroModule:HeroBreakReq(uid, cost_same_name_hero_list, cost_same_quality_hero_list, cost_same_profession_hero_list)
	--战力变化 --进阶
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged",Macros.Game.COMBAT_CHANGE.HERO_BREAK)
	
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_BREACH_REQ)
	protocol.uid = uid
	protocol.same_name_hero_list = cost_same_name_hero_list or {}
	protocol.same_quality_hero_list = cost_same_quality_hero_list or {}
	protocol.same_profession_hero_list = cost_same_profession_hero_list or {}
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.HeroModule:HeroBreakResp(protocol_data)
	self:__CalcTeamPlusProps()
	self:FireNextFrame(Macros.Event.HeroModule.HERO_BREAK_SUCC, protocol_data.uid)

	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.HERO_BREAK)
end

--培养请求
function Modules.HeroModule:HeroTrainReq(uid, cost_type, train_times)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_TRAIN_REQ)
	protocol.hero_uid = uid
	protocol.cost_type = cost_type
	protocol.train_times = train_times
	GlobalNetAdapter:OnSend(protocol)	
end

function Modules.HeroModule:HeroTrainResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_TRAIN_RESP, protocol_data)
end		

--潜能请求
function Modules.HeroModule:HeroPotentialReq(uid)	
	--战力变化 --潜能
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged",Macros.Game.COMBAT_CHANGE.HERO_POTENTIAL)
	
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_DESTINY_REQ)
	protocol.uid = uid
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.HeroModule:HeroPotentialResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_POTENTIAL_RESP, protocol_data)
end

--合成请求
function Modules.HeroModule:HeroComposeReq(info_id)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_COMPOSE_REQ)
	protocol.info_id = info_id
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.HeroModule:HeroComposeResp(protocol_data)
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_COMPOUND_TIPS ,false , protocol_data.info_id )
end

-- "hero_uid__I",      --请求觉醒的英雄uid
-- "cost_hero_list__T__cost_hero_uid@I", --作为消耗的英雄uid（0=表示不需要)
--觉醒请求
function Modules.HeroModule:HeroAwakenReq(hero_uid, cost_hero_list)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_AWAKEN_REQ)
	protocol.hero_uid = hero_uid
	protocol.cost_hero_list = cost_hero_list or {}
	GlobalNetAdapter:OnSend(protocol)	
end

--觉醒响应
function Modules.HeroModule:HeroAwakenResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_AWAKEN_RESP, protocol_data)
end

--装备觉醒道具请求
function Modules.HeroModule:HeroEquipAwakenItemReq(hero_uid,awaken_item_id)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_EQUIP_AWAKEN_ITEM_REQ)
	protocol.hero_uid = hero_uid
	protocol.awaken_item_id = awaken_item_id
	GlobalNetAdapter:OnSend(protocol)
end

--装备觉醒道具响应
function Modules.HeroModule:HeroEquipAwakenItemResp(protocol_data)
	local hero_info = self.__hero_info_map[protocol_data.hero_uid]
	if hero_info then
		hero_info.prop_constructed = false
		table.insert(hero_info.awaken_item_list, protocol_data.awaken_item_id)
	end

	self:FireNextFrame(Macros.Event.HeroModule.HERO_EQUIP_AWAKEN_PROP_RESP, protocol_data)
end

--请求图鉴列表
function Modules.HeroModule:HeroCollectionReq()
	GlobalNetAdapter:OnSend(Net.ProtocolNo.C2S_HERO_COLLECTION_LIST_REQ)
end

--英雄培养替换请求
function Modules.HeroModule:HeroTrainReplaceReq(hero_uid)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_TRAIN_REPLACE_REQ)
	protocol.hero_uid = hero_uid
	GlobalNetAdapter:OnSend(protocol)
end

--英雄培养替换响应
function Modules.HeroModule:HeroTrainReplaceResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_TRAIN_REPLACE_RESP)
end

--培养奖励领取请求
function Modules.HeroModule:HeroTrainRewardGetReq(hero_uid,pro_type)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_TRAIN_REWARD_GET_REQ)
	protocol.hero_uid = hero_uid
	protocol.pro_type = pro_type
	GlobalNetAdapter:OnSend(protocol)
end

--培养奖励领取响应
function Modules.HeroModule:HeroTrainRewardGetResp(protocol_data)
	self:FireNextFrame(Macros.Event.HeroModule.HERO_TRAIN_GET_REWARD_RESP,protocol_data.pro_type)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--角色等级改变
function Modules.HeroModule:HandleRoleLevelChange(old_var, new_var)
	--英雄阵位开启
	local config_hero_lineup_pos = CPPGameLib.GetConfig("HeroLineup", Macros.Game.HeroLineup.HERO_LINEUP)
	for lineup_index, lineup_info in ipairs(config_hero_lineup_pos.pos_list) do
		if lineup_info.need_level > old_var and lineup_info.need_level <= new_var then
			self:FireNextFrame(Macros.Event.HeroModule.HERO_LINEUP_POS_OPENED, lineup_index)
			--统计红点
			CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.HERO_LINEUP)
			break
		end
	end

	--援军阵位开启
	local config_reinforcement_lineup_pos = CPPGameLib.GetConfig("HeroLineup", Macros.Game.HeroLineup.REINFORCEMENT)
	for lineup_index, lineup_info in ipairs(config_reinforcement_lineup_pos.pos_list) do
		if lineup_info.need_level > old_var and lineup_info.need_level <= new_var then
			self:FireNextFrame(Macros.Event.HeroModule.REINFORCEMENT_LINEUP_POS_OPENED, lineup_index)
			--统计红点
			CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.REINFORCEMENT)
			break
		end
	end

	--装备开启
	local module_info1 = CPPGameLib.GetConfig("SystemEntry", Macros.Game.SystemID.TREASURE_DRESS)
	local module_info2 = CPPGameLib.GetConfig("SystemEntry", Macros.Game.SystemID.EQUIP_DRESS)
	if module_info1.level > old_var and module_info1.level <= new_var or module_info2.level > old_var and module_info2.level <= new_var then
		--统计红点
		CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.EQUIP_DRESS)
	end

	--卡牌开启
	for pos=1,8 do
		--穿卡牌等级
		local config = CPPGameLib.GetConfig("DivinationHole", pos)			
		if config and old_var < config.role_level and new_var >= config.role_level then
			self:FireNextFrame(Macros.Event.HeroModule.HERO_UPDATE_DIVINATION_NOTICE)			
			--统计红点
			CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.DIVINATION_DRESS)
			break	
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--获取英雄作为升级材料对应的经验
function Modules.HeroModule.GetHeroExp(hero_info)
	local exp = GlobalCustomizedLogicAdapterMgr:Call("GetHeroExp", hero_info)
	if exp then
		return exp
	end
	exp = 0

	if not hero_info then
		return exp
	end

	--英雄作为材料对应的经验
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	exp = exp + (config_hero and config_hero.to_exp or 0)

	--英雄当前等级对应的经验
	local config_hero_level = ConfigAdapter.Hero:GetUpgradeInfo(config_hero.init_quality, hero_info.level)
	exp = exp + (config_hero_level and config_hero_level.total_exp or 0)

	--英雄当前等级已获得的经验
	exp = exp + hero_info.exp

	return exp
end

--获取英雄基础信息
--[[
	"uid__I",           	--英雄uid
	"info_id__H",       	--英雄配置id
	"level__C",         	--英雄等级
	"quality_level__C",     --品质
	"potential_level__C",   --潜能
	"awaken_star__C",   --觉醒星数
    "awaken_level__C",  --觉醒等级  
	"break_level__C",   	--进阶等级
	"is_to_battle__C",  	--是否上阵            
	"exp__I",       		--当前经验   
	"quality_stage__C" 		--品质等阶
--]]
function Modules.HeroModule:GetHeroInfo(uid)
	if not uid then
		return
	end
	return self.__hero_info_map[uid]
end

--获取英雄详细信息
--[[
	基础信息 + 战斗属性(不含其他系统的属性加成)
--]]
function Modules.HeroModule:GetDetailHeroInfo(uid, is_reset)
	local hero_info = self.__hero_info_map[uid]
	if not hero_info then
		return
	end	

	--重置属性
	if is_reset then
		for _, prop_key in pairs(Macros.Game.BattlePropKey) do
			hero_info[prop_key] = 0
		end
		hero_info.prop_constructed = false
	end

	--构造属性
	if not hero_info.prop_constructed then
		local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
		if not config_hero then
			return hero_info
		end

		local prop_key

		--基础属性、升级加成
		local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
		for _, prop_type in pairs(Macros.Game.BattleProp) do
			prop_key = Macros.Game.BattlePropKey[prop_type]
			if not basic_prop_map or not basic_prop_map[prop_type] then
				hero_info[prop_key] = 0
			else
				hero_info[prop_key] = basic_prop_map[prop_type]
			end
		end
		
		--进阶加成
		local break_plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusPropMap(hero_info.info_id, hero_info.break_level)
		Modules.BattleFormula:HandlePlusPropMap(hero_info, break_plus_prop_map)
		-- CPPGameLib.PrintTable("xyz", break_plus_prop_map, "英雄进阶加成")
	
		--培养加成
		local train_plus_prop_map = ConfigAdapter.Hero.GetTrainPlusPropMap(hero_info.train_pro_list)
		Modules.BattleFormula:HandlePlusPropMap(hero_info, train_plus_prop_map)
		-- CPPGameLib.PrintTable("xyz", train_plus_prop_map, "英雄培养加成")

		--潜能加成
		local potential_plus_prop_map = ConfigAdapter.Hero.GetPotentialPlusPropMap(hero_info.potential_level , hero_info.info_id )
		Modules.BattleFormula:HandlePlusPropMap(hero_info, potential_plus_prop_map)
		-- CPPGameLib.PrintTable("xyz", potential_plus_prop_map, "英雄潜能加成")

		--觉醒加成
		local awaken_plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id, hero_info.awaken_star, hero_info.awaken_level, hero_info.awaken_item_list)
		Modules.BattleFormula:HandlePlusPropMap(hero_info, awaken_plus_prop_map)
		-- CPPGameLib.PrintTable("xyz", awaken_plus_prop_map, "英雄觉醒加成")

		--进化加成
		local evolution_plus_prop_map = ConfigAdapter.Hero.GetHeroEvolutionPlusPropMap(hero_info.quality_level,hero_info.quality_stage,hero_info.info_id)
		Modules.BattleFormula:HandlePlusPropMap(hero_info, evolution_plus_prop_map)
		-- CPPGameLib.PrintTable("xyz", evolution_plus_prop_map, "英雄进化加成")

		hero_info.prop_constructed = true
	end

	return hero_info
end

--获取英雄信息表
function Modules.HeroModule:GetHeroInfoMap()
	return self.__hero_info_map
end

--获取基础信息列表
function Modules.HeroModule:GetHeroInfoList(is_exclude_exphero)
	local list = {}
	if not is_exclude_exphero then
		for _, hero_info in pairs(self.__hero_info_map) do
			table.insert(list, hero_info)
		end
	else
		for _, hero_info in pairs(self.__hero_info_map) do
			local hero_config = CPPGameLib.GetConfig("Hero",hero_info.info_id)
			if hero_config.type ~= Macros.Game.HeroType.EXP then
				table.insert(list, hero_info)
			end
		end
	end
	table.sort(list, sort_hero_info_list)
	return list
end

--获得英雄总数量和上限
function Modules.HeroModule:GetHeroSumAndMax()
	local num = self:GetHeroInfoList()
	return #num , CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_PACK_MAX)
end

--获取英雄数量(根据配置ID)
function Modules.HeroModule:GetNumByInfoID(info_id, filter_func)
	local count = 0
	for _, hero_info in pairs(self.__hero_info_map) do
		if not filter_func then
			if hero_info.info_id == info_id then
				count = count + 1
			end
		else
			if hero_info.info_id == info_id and filter_func(hero_info) then
				count = count + 1
			end
		end
	end	
	return count
end

--获取英雄升级材料列表
function Modules.HeroModule:GetUpgradeMaterialList(filter_hero_uid, filter_func, for_auto_add_materials)	
	local list = {}
	local config_hero
	for _, hero_info in pairs(self.__hero_info_map) do
		config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
		
		--过滤已上阵英雄
		if not self.__hero_lineup_map[hero_info.uid] and 
	
			--过滤指定英雄
			(not filter_hero_uid or filter_hero_uid ~= hero_info.uid) and
			(not filter_func or filter_func(hero_info)) and
	
			--自动添加材料，过滤紫品或以上的非经验英雄
			(not for_auto_add_materials or (config_hero.type == Macros.Game.HeroType.EXP or hero_info.quality_level < Macros.Game.QualityType.PURPLE)) then

			table.insert(list, hero_info)
		end
	end

	table.sort(list, function(info1, info2)
		if not info1 then
			return false
		end
		if not info2 then
			return true
		end

		--品质从低到高
		if info1.quality_level < info2.quality_level then
			return true
		elseif info1.quality_level > info2.quality_level then
			return false
		end

		--经验加成从低到高
		local exp1 = Modules.HeroModule.GetHeroExp(info1)
		local exp2 = Modules.HeroModule.GetHeroExp(info2)
		if exp1 < exp2 then
			return true
		elseif exp1 > exp2 then
			return false
		end

		--英雄ID从大到小
		return info1.info_id > info2.info_id
	end)
	return list
end

--英雄进阶的英雄消耗过滤器
function Modules.HeroModule.BreakCostHeroFilter(hero_info)
	if not hero_info then
		return
	end

	local is_in_lineup = CallModuleFunc(ModuleType.HERO, "IsInLineup", hero_info.uid)
	if hero_info.break_level <= 0 and hero_info.potential_level <= 0 and not is_in_lineup then
		return true
	end
end

--获取英雄进阶的英雄消耗列表
function Modules.HeroModule:GetBreakCostHeroList(target_hero_uid, same_name_hero_num, same_quality_hero_num, same_profession_hero_num, is_greedy)
	local tgt_hero_info = self.__hero_info_map[target_hero_uid]
	if not tgt_hero_info then
		return
	end

	local config_hero
	local tgt_config_hero = CPPGameLib.GetConfig("Hero", tgt_hero_info.info_id)
	if not tgt_config_hero then
		return
	end

	local need_same_name_hero = same_name_hero_num > 0
	local need_same_quality_hero = same_quality_hero_num > 0
	local need_same_profession_hero = same_profession_hero_num > 0
	local cost_same_name_hero_list, cost_same_quality_hero_list, cost_same_profession_hero_list

	--搜索同品质英雄
	local function SearchSameQualityHero(hero_info)
		if need_same_quality_hero and 
			hero_info.quality_level == tgt_hero_info.quality_level and (same_quality_hero_num > 0 or is_greedy) then
			
			same_quality_hero_num = same_quality_hero_num - 1
			cost_same_quality_hero_list = cost_same_quality_hero_list or {}
			table.insert(cost_same_quality_hero_list, {uid = hero_info.uid})
			return true
		end
	end

	--搜索同品质且职业英雄
	local function SearchSameProfessionHero(hero_info)
		local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

		if need_same_profession_hero and 
			hero_info.quality_level == tgt_hero_info.quality_level and config_hero.profession == tgt_config_hero.profession and (same_profession_hero_num > 0 or is_greedy) then
			
			same_profession_hero_num = same_profession_hero_num - 1
			cost_same_profession_hero_list = cost_same_profession_hero_list or {}
			table.insert(cost_same_profession_hero_list, {uid = hero_info.uid})
			return true
		end
	end

	for _, hero_info in pairs(self.__hero_info_map) do
		if not is_greedy and same_name_hero_num <= 0 and same_quality_hero_num <= 0 and same_profession_hero_num <= 0 then
			--非贪心法下，搜索够材料则跳出循环
			break
		end

		if hero_info.uid ~= target_hero_uid and Modules.HeroModule.BreakCostHeroFilter(hero_info) then
			--同名英雄
			if need_same_name_hero and hero_info.info_id == tgt_hero_info.info_id and (is_greedy or same_name_hero_num > 0) then
				same_name_hero_num = same_name_hero_num - 1
				cost_same_name_hero_list = cost_same_name_hero_list or {}
				table.insert(cost_same_name_hero_list, {uid = hero_info.uid})
			end

			--先搜索同品质且同职业英雄
			if not SearchSameProfessionHero(hero_info) or is_greedy then
				--再搜索同品质英雄
				SearchSameQualityHero(hero_info)
			end
		end
	end	

	return cost_same_name_hero_list, cost_same_quality_hero_list, cost_same_profession_hero_list
end

--获取阵位信息
function Modules.HeroModule:GetLineupInfo(index)
	return self.__hero_lineup_list[index]
end

--获取是否在阵位上
function Modules.HeroModule:IsInLineup(uid)
	if not uid then
		return
	end
	return self.__hero_lineup_map and self.__hero_lineup_map[uid] 
end

--获取援军阵位信息
function Modules.HeroModule:GetReinforcementLineupInfo(index)
	return self.__hero_reinforcement_list[index]
end

--获取阵位上的英雄信息
function Modules.HeroModule:GetLineupHeroInfo(index)
	local lineup_info = self:GetLineupInfo(index)
	if lineup_info then
		return self:GetHeroInfo(lineup_info.uid)
	end	
end

--获取阵位信息(根据英雄uid)
function Modules.HeroModule:GetLineupInfoByUID(uid, lineup_type)
	lineup_type = lineup_type or Macros.Game.HeroLineup.HERO_LINEUP

	local lineup_list = lineup_type == Macros.Game.HeroLineup.HERO_LINEUP and self.__hero_lineup_list or self.__hero_reinforcement_list
	for index, lineup_info in ipairs(lineup_list) do
		if lineup_info.uid == uid then
			return index, lineup_info
		end
	end
end

--获取阵位上的装备信息(key：info_id，value：equip_info)
function Modules.HeroModule:GetLineupEquipInfo(index)
	local tbl_all = {}
	local tbl_equip = {}
	local tbl_treasure = {}
	local tbl_special_equip = {}
	local tbl_special_treasure = {}

	if not index then
		return tbl_all, tbl_equip, tbl_treasure,tbl_special_equip,tbl_special_treasure
	end

	--攻击圣器
	local treasure_info = CallModuleFunc(ModuleType.TREASURE,"GetTreasure",self.__hero_lineup_list[index].attack_treasure_uid)	
	if treasure_info then	
		tbl_all[treasure_info.info_id] = treasure_info
		tbl_treasure[treasure_info.info_id] = treasure_info
		tbl_special_treasure[Macros.Game.TreasureType.EMBLEM] = treasure_info
	end
	
	--防御圣器
	treasure_info = CallModuleFunc(ModuleType.TREASURE,"GetTreasure",self.__hero_lineup_list[index].defence_treasure_uid)
	if treasure_info then
		tbl_all[treasure_info.info_id] = treasure_info
		tbl_treasure[treasure_info.info_id] = treasure_info
		tbl_special_treasure[Macros.Game.TreasureType.GAD] = treasure_info
	end
	
	--武器
	local equip_info = CallModuleFunc(ModuleType.EQUIP,"GetEquip",self.__hero_lineup_list[index].weapon_equipment_uid)
	if equip_info then
		tbl_all[equip_info.info_id] = equip_info
		tbl_equip[equip_info.info_id] = equip_info
		tbl_special_equip[Macros.Game.EquipType.WEAPON] = equip_info
	end

	--头盔
	equip_info = CallModuleFunc(ModuleType.EQUIP,"GetEquip",self.__hero_lineup_list[index].hat_equipment_uid)
	if equip_info then
		tbl_all[equip_info.info_id] = equip_info
		tbl_equip[equip_info.info_id] = equip_info
		tbl_special_equip[Macros.Game.EquipType.HELMET] = equip_info
	end

	--腰带
	equip_info = CallModuleFunc(ModuleType.EQUIP,"GetEquip",self.__hero_lineup_list[index].belt_equipment_uid)
	if equip_info then
		tbl_all[equip_info.info_id] = equip_info
		tbl_equip[equip_info.info_id] = equip_info
		tbl_special_equip[Macros.Game.EquipType.BELT] = equip_info
	end

	--战甲
	equip_info = CallModuleFunc(ModuleType.EQUIP,"GetEquip",self.__hero_lineup_list[index].clothes_equipment_uid)
	if equip_info then
		tbl_all[equip_info.info_id] = equip_info
		tbl_equip[equip_info.info_id] = equip_info
		tbl_special_equip[Macros.Game.EquipType.CLOTH] = equip_info
	end

	return tbl_all, tbl_equip, tbl_treasure,tbl_special_equip,tbl_special_treasure
end

--获取阵位上的卡牌信息(key：card_type，value：divination_info)
function Modules.HeroModule:GetLineupDivinationInfo(index)
	local divination_type_list = {}

	if not index then
		return divination_type_list
	end
	--遍历该阵位英雄的
	for pos=1,8 do
		local obj_info = CallModuleFunc(ModuleType.DIVINATION,"GetDivination",self.__hero_lineup_list[index].divination_list[pos].card_id)
		if obj_info then
			divination_type_list[obj_info.type] = obj_info
		end
	end
	return divination_type_list	
end

--获取阵位上有没有这个装备信息(key：info_id，value：equip_info)
function Modules.HeroModule:GetLineupEquipDress( index , type , system )
	local tbl_all , tbl_equip , tbl_treasure = self:GetLineupEquipInfo( index )

	local bool = false

	if system == Macros.Game.ObjType.EQUIP then
		for k,v in pairs(tbl_equip) do
			local type_ , info_ = ObjIDTool.GetInfo( k )
			if info_.item_type == type then
				bool = true
				break
			end
		end

	elseif system == Macros.Game.ObjType.TREASURE then
		for k,v in pairs(tbl_treasure) do
			local type_ , info_ = ObjIDTool.GetInfo( k )
			if info_.type == type then
				bool = true
				break
			end
		end
		
	end

	return bool
end

--获取阵位列表
function Modules.HeroModule:GetLineupList()
	return self.__hero_lineup_list
end

--获取首个上阵位置
function Modules.HeroModule:GetFirstLineupPos()
	for i, lineup_info in ipairs(self.__hero_lineup_list) do
		if lineup_info.uid > 0 and self.__hero_info_map[lineup_info.uid] then
			return i
		end
	end
	return nil
end

--获取首个上阵的英雄ID
function Modules.HeroModule:GetFirstLineupHeroUID()
	for i, lineup_info in ipairs(self.__hero_lineup_list) do
		if lineup_info.uid > 0 then
			return lineup_info.uid
		end
	end
	return 0
end

--获取上阵选择列表
function Modules.HeroModule:GetLineupSelectList(info_id, lineup_type, lineup_index,no_sort)
	no_sort = no_sort or false
	local hero_info
	local lineup_info_id_map = {}
	local no_battle_map = {}
	for _, lineup_info in ipairs(self.__hero_lineup_list) do
		hero_info = self:GetHeroInfo(lineup_info.uid)
		--记录已上阵英雄的ID，过滤待切换英雄
		if hero_info and hero_info.info_id ~= info_id then
			lineup_info_id_map[hero_info.info_id] = true			
		end
	end
	for _, lineup_info in ipairs(self.__hero_reinforcement_list) do
		if lineup_info.uid ~= 0 then
			hero_info = self:GetHeroInfo(lineup_info.uid)
			--记录已上阵援军英雄的ID，过滤待切换英雄
			if hero_info and hero_info.info_id ~= info_id then
				lineup_info_id_map[hero_info.info_id] = true
			end
		end
	end
	local list = {}
	local hero_config
	for _, hero_info in pairs(self.__hero_info_map) do
		--过滤已上阵英雄、与已上阵英雄相同类型的英雄
		if hero_info.is_to_battle == Macros.Global.TRUE or (hero_info.is_to_battle == Macros.Global.FALSE and not lineup_info_id_map[hero_info.info_id]) then
			hero_config = CPPGameLib.GetConfig("Hero", hero_info.info_id)

			--过滤经验类英雄
			if hero_config and hero_config.type ~= Macros.Game.HeroType.EXP then
				table.insert(list, hero_info)
			end
		end
		--获取未上阵英雄
		if hero_info.is_to_battle == Macros.Global.FALSE and not lineup_info_id_map[hero_info.info_id] then
			table.insert(no_battle_map, hero_info)
		end
	end
	--不需要排序
	if no_sort then
		return no_battle_map
	end
	table.sort(list,function (info1,info2)
		if not info1 then
			return false
		end
		if not info2 then
			return true
		end

		--上阵优先
		if info1.is_to_battle == Macros.Global.TRUE and info2.is_to_battle == Macros.Global.FALSE then
			return true
		elseif info1.is_to_battle == Macros.Global.FALSE and info2.is_to_battle == Macros.Global.TRUE then
			return false
		end

		if lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then
			--合击数量
			local hero_name_list_1 = self:GetCanActiavteTogetherAttackName(info1.info_id,lineup_index)
			local hero_name_list_2 = self:GetCanActiavteTogetherAttackName(info2.info_id,lineup_index)
			if #hero_name_list_1 > #hero_name_list_2 then
				return true
			elseif #hero_name_list_1 < #hero_name_list_2 then
				return false
			end
		end
		
		--宿命数量
		local fate_num_1 = self:GetCanActiavteReinforcementsNum(info1.uid, lineup_type,lineup_index)
		local fate_num_2 = self:GetCanActiavteReinforcementsNum(info2.uid, lineup_type,lineup_index)
		if fate_num_1 > fate_num_2 then
			return true
		elseif fate_num_1 < fate_num_2 then
			return false
		end

		--品质从高到低
		if info1.quality_level > info2.quality_level then
			return true
		elseif info1.quality_level < info2.quality_level then
			return false
		end

		--等级从高到低
		if info1.level > info2.level then
			return true
		elseif info1.level < info2.level then
			return false
		end

		--进阶等级从高到低
		if info1.break_level > info2.break_level then
			return true
		elseif info1.break_level < info2.break_level then
			return false
		end

		--潜能等级从高到低
		if info1.potential_level > info2.potential_level then
			return true
		elseif info1.potential_level < info2.potential_level then
			return false
		end

		--觉醒星级从高到低
		if info1.awaken_star > info2.awaken_star then
			return true
		elseif info1.awaken_star < info2.awaken_star then
			return false
		end

		--觉醒等级从高到低
		if info1.awaken_level > info2.awaken_level then
			return true
		elseif info1.awaken_level < info2.awaken_level then
			return false
		end

		--英雄ID从大到小
		return info1.info_id > info2.info_id
	end)

	local ret_list = {}
	for i, hero_info in ipairs(list) do
		local data = {}
		data.lineup_type = lineup_type
		data.lineup_index = lineup_index
		data.hero_info = hero_info
		table.insert(ret_list,data)
	end
	return ret_list
end

--设置
function Modules.HeroModule:SetBeforeStrengthen(formation_pos,message_type)
	local _, lineup_equip_map, lineup_treasure_map = self:GetLineupEquipInfo(formation_pos)
    local plus_prop_map,equip_strengthen_level_min,equip_refine_level_min
    if message_type == Macros.Game.ObjType.EQUIP then		   
	    plus_prop_map,equip_strengthen_level_min,equip_refine_level_min = ConfigAdapter.Forge.GetEquipPlusPropMap(lineup_equip_map)    
	else
		plus_prop_map,equip_strengthen_level_min,equip_refine_level_min = ConfigAdapter.Forge.GetTreasurePlusPropMap(lineup_treasure_map)
	end	
    equip_strengthen_level_min = equip_strengthen_level_min or 0
    equip_refine_level_min = equip_refine_level_min or 0
    --设置强化等级
	self.__before_strengthen_list = {equip_strengthen_level_min,equip_refine_level_min}
end

--获取需要消耗的英雄数量和Uid
function Modules.HeroModule:GetHeroNumAndUidByInfoId(hero_info_id,cur_hero_uid,cost_hero_num)
	local list = {}
	local hero_num = 0
	for _, hero_info in pairs(self.__hero_info_map) do
		--过滤已上阵英雄
		if not self.__hero_lineup_map[hero_info.uid] and hero_info.info_id == hero_info_id and cur_hero_uid ~= hero_info.uid then
			hero_num = hero_num + 1			
			if hero_num <= cost_hero_num then
				table.insert(list,{cost_hero_uid = hero_info.uid})
			end
		end
	end
	return hero_num,list
end

--判断是否能升级
function Modules.HeroModule:JudgeIsCouldUpgrade(hero_uid)
	local hero_info = self:GetHeroInfo(hero_uid)	
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local role_gold = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
	
	--检测系统是否开启
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_UPGRADE)
	if not is_open then
		return is_open
	end

	--检测英雄等级是否超过角色等级
	if hero_info.level >= role_level then
		return false
	end

    --检测现有材料获得的经验是否满足升级需求
    local data_list = self:GetUpgradeMaterialList(hero_uid)
    if not data_list or #data_list <= 0 then
        return false
    end
    local add_exp = 0
	local upgrade_info = ConfigAdapter.Hero:GetUpgradeInfo(hero_info.quality_level, hero_info.level)		
    for _, hero_info in ipairs(data_list) do
    	add_exp = add_exp + Modules.HeroModule.GetHeroExp(hero_info)
    	if add_exp >= (upgrade_info.exp - hero_info.exp) then
    		break
    	end
    end
	if add_exp < (upgrade_info.exp - hero_info.exp) then
		return false
	end

	--检测现有金币是否满足升级需求
	if role_gold < add_exp then
		return false
	end

	return true
end

--判断是否能进阶
function Modules.HeroModule:JudgeIsCouldBreak(hero_uid, for_break_request)
	local hero_info = self:GetHeroInfo(hero_uid)
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local role_gold = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
	
	local is_sys_open = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.HERO_BREAK )
	if is_sys_open ~= true then
		return false
	end

	--已满级
	if hero_info.break_level >= ConfigAdapter.Hero.GetBreakMaxLevel(hero_info.info_id) then
		if for_break_request then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_break_reached_max"))
		end
		return false
	end

	--等级是否足够
	local need_level, cost_gold, cost_obj_list, cost_same_name_hero_num, cost_same_quality_hero_num, cost_same_profession_hero_num = ConfigAdapter.Hero.GetBreakLevelupInfo(hero_info)
	if hero_info.level < need_level then
		if for_break_request then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_break_tips1"), true)
		end
		return false
	end

	--所需金钱能否够升级
	if role_gold < cost_gold then
		if for_break_request then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
			CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD)
		end
		return false
	end

	local function ShowNotEnoughHeroMaterialTips(info_id, tips)
		if for_break_request then
	    	--材料不足
			GlobalTipMsg.GetInstance():Show(tips or CPPGameLib.GetString("common_material_not_enough"))			

			--弹出获取途径
			if info_id then
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, info_id)
			end
		end		
	end
	
	--检测道具材料是否足够
	for i, cost_obj_info in ipairs(cost_obj_list) do
		local cost_obj_type, cost_obj_config = ObjIDTool.GetInfo(cost_obj_info.id)
		local cost_obj_own_num = ObjIDTool.GetOwnNum(cost_obj_info.id)
		if cost_obj_own_num < cost_obj_info.num then
			ShowNotEnoughHeroMaterialTips()
			return false
		end
	end
	--检测同名英雄、同品质英雄、同品质且职业英雄材料是否足够
	local cost_same_name_hero_list, cost_same_quality_hero_list, cost_same_profession_hero_list = self:GetBreakCostHeroList(hero_uid, cost_same_name_hero_num, cost_same_quality_hero_num, cost_same_profession_hero_num)
	if cost_same_name_hero_num > 0 and (not cost_same_name_hero_list or #cost_same_name_hero_list < cost_same_name_hero_num) then
		ShowNotEnoughHeroMaterialTips(hero_info.info_id, CPPGameLib.GetString("hero_break_tips2"))
		return false
	end	
	if cost_same_quality_hero_num > 0 and (not cost_same_quality_hero_list or #cost_same_quality_hero_list < cost_same_quality_hero_num) then
		ShowNotEnoughHeroMaterialTips(nil, CPPGameLib.GetString("hero_break_tips3"))
		return false
	end
	if cost_same_profession_hero_num > 0 and (not cost_same_profession_hero_list or #cost_same_profession_hero_list < cost_same_profession_hero_num) then
		ShowNotEnoughHeroMaterialTips(nil, CPPGameLib.GetString("hero_break_tips4"))
		return false
	end

	return true
end

--获取对应阵位英雄的卡牌类型
function Modules.HeroModule:GetHeroDivinationTypeByLineUpIndexAndPos(lineup_index,pos)
	if not lineup_index or not pos then
		return
	end
	local obj_info = CallModuleFunc(ModuleType.DIVINATION,"GetDivination",self.__hero_lineup_list[lineup_index] and self.__hero_lineup_list[lineup_index].divination_list[pos].card_id or 0)
	if obj_info then
		return obj_info.type
	else
		return
	end
end

function Modules.HeroModule:IsTreasureGrid(key)
	if key == "attack_treasure_uid" or
		key == "defence_treasure_uid" then
		return true
	end
end

--判断可穿戴的装备列表里面是否有品质大于现在穿着的
function Modules.HeroModule:ParseEquipList(equipList,quality)
	for _,equip_info in ipairs(equipList) do
		if equip_info.quality > quality then
			return true
		end
	end
	return false
end

--判断可穿戴的装备列表里面是否有品质等于现在穿着的并且有宿命
--可穿戴的装备列表 ，当前的装备信息 ， 英雄id ，阵位 
function Modules.HeroModule:ParseEquipListReinforcement(equipList,obj_info,hero_id,lineup_index)
	if not equipList or not obj_info or not hero_id or not lineup_index then
		return
	end
	local tpl_all = self:GetLineupEquipInfo(lineup_index)
	--英雄配置
	local hero_config = CPPGameLib.GetConfig("Hero",hero_id)
	--类型
	local equip_type = ObjIDTool.GetInfo(obj_info.info_id)
	--装备圣器宿命列表
	self.__relation_equip_list = self.__relation_equip_list or {}
	if not self.__relation_equip_list[hero_id] then
		self.__relation_equip_list[hero_id] = {}
		--装备宿命
		if hero_config.relation_equip_list then
			for i,equip_info in ipairs(hero_config.relation_equip_list) do
				for _,equip_id in ipairs(equip_info) do
					self.__relation_equip_list[hero_id][equip_id] = equip_id
				end			
			end
		end
		--圣器宿命
		if hero_config.relation_treasure_list then
			for i,treasure_info in ipairs(hero_config.relation_treasure_list) do
				for _,treasure_id in ipairs(treasure_info) do
					self.__relation_equip_list[hero_id][treasure_id] = treasure_id
				end			
			end
		end
	end
	--判断是否有更高级的装备
	local ParseEquipList = function (equipList,taste)
		for _,equip_info in ipairs(equipList) do
			if equip_type == Macros.Game.ObjType.EQUIP and equip_info.taste > taste then
				return true
			elseif equip_type == Macros.Game.ObjType.TREASURE and equip_info.quality > taste then
				return true
			end
		end
		return false
	end
	--是否还有宿命装备
	local ParseRelationAllList = function (obj_info_id,obj_info_quality)
		local obj_info_id = obj_info_id or obj_info.info_id
		local obj_info_quality = obj_info_quality or (equip_type == Macros.Game.ObjType.EQUIP and obj_info.taste or obj_info.quality)
		local relation_all_list = {}
		--装备宿命
		if hero_config.relation_equip_list then
			for i,equip_info in ipairs(hero_config.relation_equip_list) do
				local is_break = false
				for _,equip_id in ipairs(equip_info) do
					if equip_id == obj_info_id then
						relation_all_list = CPPGameLib.MergeTable2List(relation_all_list, equip_info)
						is_break = true
						break
					end
				end		
				if is_break then
					break
				end	
			end
		end
		--圣器宿命
		if hero_config.relation_treasure_list then
			for i,treasure_info in ipairs(hero_config.relation_treasure_list) do
				local is_break = false
				for _,treasure_id in ipairs(treasure_info) do
					if treasure_id == obj_info_id then
						relation_all_list = CPPGameLib.MergeTable2List(relation_all_list, treasure_info)
						is_break = true
						break
					end
				end	
				if is_break then
					break
				end			
			end
		end
		--是否有其他宿命装备
		if #relation_all_list > 1 then
			for i,equip_id in ipairs(relation_all_list) do
				if equip_type == Macros.Game.ObjType.EQUIP and equip_id ~= obj_info_id and tpl_all[equip_id] and tpl_all[equip_id].taste > obj_info_quality then
					return true
				elseif equip_type == Macros.Game.ObjType.TREASURE and equip_id ~= obj_info_id and tpl_all[equip_id] and tpl_all[equip_id].quality > obj_info_quality then
					return true
				end
			end			
		end
		return false
	end
	--是否还有宿命装备
	local ParseRelationEquipList = function (is_more)
		for _,equip_info in ipairs(equipList) do
			if is_more then
				--是否需要更高级的宿命装备
				if equip_type == Macros.Game.ObjType.EQUIP and equip_info.info_id ~= obj_info.info_id and equip_info.taste > obj_info.taste and self.__relation_equip_list[hero_id][equip_info.info_id] and not tpl_all[equip_info.info_id] then
					return true
				elseif equip_type == Macros.Game.ObjType.TREASURE and equip_info.info_id ~= obj_info.info_id and equip_info.quality > obj_info.quality and self.__relation_equip_list[hero_id][equip_info.info_id] then
					return true
				end
			else
				if equip_info.info_id ~= obj_info.info_id and self.__relation_equip_list[hero_id][equip_info.info_id] then
					--判断是宿命装备并且当前身上没有更高级激活该装备激活宿命的装备
					if not ParseRelationAllList(equip_info.info_id,equip_info.taste) then
						return true
					end
				end
			end
		end
		return false
	end
	--判断是否为宿命装备
	local is_relation_equip = self.__relation_equip_list[hero_id][obj_info.info_id]
	if is_relation_equip then
		--如果是宿命装备
		if ParseRelationEquipList(true) then
			--是否还有品级更高的宿命装备
			return true
		else
			if ParseRelationAllList() then
				if ParseEquipList(equipList,equip_type == Macros.Game.ObjType.EQUIP and obj_info.taste or obj_info.quality) then
					--是否还有更高品质的装备
					return true
				end
			end
		end
	else
		--如果不是宿命装备
		if ParseRelationEquipList() then
			--是否还有宿命装备
			return true
		elseif ParseEquipList(equipList,equip_type == Macros.Game.ObjType.EQUIP and obj_info.taste or obj_info.quality) then
			--是否还有更高品级的装备
			return true
		end
	end

	return false
end

--判断可穿戴的装备列表里面是否有品质等于现在穿着的并且有宿命
--可穿戴的卡牌列表 ，当前的卡牌信息 ， 英雄id ，阵位 
function Modules.HeroModule:ParseDivinationListReinforcement(divination_list,obj_info,hero_id,lineup_index,card_pos)
	if not divination_list or not obj_info or not hero_id or not lineup_index then
		return
	end
	--英雄配置
	local hero_config = CPPGameLib.GetConfig("Hero",hero_id)
	--穿戴卡牌的类型
	local divination_type_list = self:GetLineupDivinationInfo(lineup_index)
	--卡牌宿命列表
	self.__relation_divination_list = self.__relation_divination_list or {}
	if not self.__relation_divination_list[hero_id] then
		self.__relation_divination_list[hero_id] = {}							
		--卡牌宿命
		if hero_config.relation_card_list then
			for i,card_info in ipairs(hero_config.relation_card_list) do
				for _,card_type in ipairs(card_info) do
					self.__relation_divination_list[hero_id][card_type] = card_info
				end			
			end
		end
	end
	--判断是否有激活宿命的卡牌
	local ParseDivinationReinforcementList = function (card_type)
		for _,card_info in ipairs(divination_list) do
			if card_info.type == card_type then
				return true
			end
		end
		return false
	end
	--判断是否有更高级的卡牌以及是否能强化和进阶
	local ParseDivinationList = function ()
		--遍历
		for _,card_info in ipairs(divination_list) do
			if card_info.type == obj_info.type and card_info.quality > obj_info.quality then
				return true
			end
		end
		--判断是否强化
		self.__mark_divination_strengthen_list[lineup_index][card_pos] = true
		if CallModuleFunc(ModuleType.DIVINATION,"ToUidGetCardIsStrengthen",obj_info.uid) == "true" then
			self.__mark_is_divination_could_strengthen_list[lineup_index][card_pos] = true
			return true
		end		
		-- 判断是否进阶
		self.__mark_divination_break_list[lineup_index][card_pos] = true
		if CallModuleFunc(ModuleType.DIVINATION,"ToUidGetCardIsBreak",obj_info.uid) == "true" then
			self.__mark_is_divination_could_break_list[lineup_index][card_pos] = true
			return true
		end				
		return false
	end
	--先判断宿命是否激活
	if hero_config.relation_card_list then
		--是否激活
		local is_activated = true				
		for card_type,_ in pairs(self.__relation_divination_list[hero_id]) do
			if not divination_type_list[card_type] then
				is_activated = false
				break
			end
		end
		--已激活
		if is_activated then
			if ParseDivinationList() then
				return true
			end
		else
			--未激活
			if not self.__relation_divination_list[hero_id][obj_info.type] then
				for card_type,_ in pairs(self.__relation_divination_list[hero_id]) do
					if not divination_type_list[card_type] and ParseDivinationReinforcementList(card_type) then
						return true
					end
				end					
			end
			if ParseDivinationList() then
				return true
			end
		end
	else
		if ParseDivinationList() then
			return true
		end
	end
end

--用装备ID或圣器UID拿到这个阵位的东西
function Modules.HeroModule:ToUidGetPlace(uid)
	
	local index = nil
	for i = 1 , 5 do
		local tbl_all, tbl_equip, tbl_treasure = self:GetLineupEquipInfo(i)
		for k,v in pairs(tbl_all) do
			if v.uid == uid then
				return i , tbl_all, tbl_equip, tbl_treasure
			end
		end
	end
end

--寻找其他类型的卡牌
function Modules.HeroModule:JudgeIsHaveAnotherDivinationType(divination_list,lineup_divination_list)
	local divination_type_list = {}
	for pos=1,8 do
		local obj_info = CallModuleFunc(ModuleType.DIVINATION,"GetDivination",lineup_divination_list[pos].card_id)	
		if obj_info then
			divination_type_list[obj_info.type] = obj_info
		end
	end
	--遍历卡牌背包
	for k,divination_info in pairs(divination_list) do
		if not divination_type_list[divination_info.type] and divination_info.type2 ~= Macros.Game.CARD_QUALITY_TYPE.EXP then
			return true
		end
	end
	return false
end

--寻找其他类型的卡牌
function Modules.HeroModule:JudgeIsActivatedByLineupIndex(lineup_index)
	local lineup_info = self.__hero_lineup_list[lineup_index]
	if lineup_info and lineup_info.uid ~= 0 then
		local hero_info = self:GetHeroInfo(lineup_info.uid)
		local hero_config = CPPGameLib.GetConfig("Hero",hero_info.info_id)
		--卡牌宿命列表
		self.__relation_divination_list = self.__relation_divination_list or {}
		if not self.__relation_divination_list[hero_id] then
			self.__relation_divination_list[hero_id] = {}							
			--卡牌宿命
			if hero_config.relation_card_list then
				for i,card_info in ipairs(hero_config.relation_card_list) do
					for _,card_type in ipairs(card_info) do
						self.__relation_divination_list[hero_id][card_type] = card_info
					end			
				end
			end
		end
		if hero_config.relation_card_list then
			local divination_type_list = self:GetLineupDivinationInfo(lineup_index)
			--是否激活
			local is_activated = true				
			for card_type,_ in pairs(self.__relation_divination_list[hero_id]) do
				if not divination_type_list[card_type] then
					is_activated = false
					break
				end
			end
			--已激活
			self.__is_activate_before = is_activated
		else
			self.__is_activate_before = false
		end
	else
		self.__is_activate_before = false
	end	
end

--通过卡牌UID获取卡牌所在的阵位和孔位
function Modules.HeroModule:GetDivinationFormationPosAndCardSlotByCardUid(card_uid)
	if not card_uid then
		return 0,0
	end
	--遍历寻找对应的阵位和槽位
	for lineup_index, lineup_info in ipairs(self.__hero_lineup_list) do
		if lineup_info.uid > 0 then
			for i,card_info in ipairs(lineup_info.divination_list) do
				if card_info.card_id == card_uid then
					return lineup_index,i
				end
			end
		end
	end
end

--通过阵位获得该阵位上穿戴着的卡牌列表
function Modules.HeroModule:GetDivinationListByLineupIndex(lineup_index)
	local divination_list = {}
	if not lineup_index then
		return divination_list
	end	
	--遍历寻找对应的阵位和槽位
	if self.__hero_lineup_list[lineup_index] then
		for i,card_info in ipairs(self.__hero_lineup_list[lineup_index].divination_list) do
			table.insert(divination_list,card_info.card_id)
		end
	end
	return divination_list
end

--通过阵位和卡牌的位置获得该阵位上穿戴着的卡牌能否强化
function Modules.HeroModule:GetDivinationIsStrengthenByLineupIndexAndCardPos(lineup_index,card_pos)
	if not lineup_index or not card_pos then
		return 
	end
	local is_could_strengthen
	if self.__mark_divination_strengthen_list[lineup_index] and self.__mark_divination_strengthen_list[lineup_index][card_pos] then
		is_could_strengthen = self.__mark_is_divination_could_strengthen_list[lineup_index][card_pos]
	elseif self.__mark_divination_strengthen_list[lineup_index] and not self.__mark_divination_strengthen_list[lineup_index][card_pos] then
		self.__mark_divination_strengthen_list[lineup_index][card_pos] = true
		local is_strengthen_str = CallModuleFunc(ModuleType.DIVINATION,"ToUidGetCardIsStrengthen",self.__hero_lineup_list[lineup_index].divination_list[card_pos].card_id)
		self.__mark_is_divination_could_strengthen_list[lineup_index][card_pos] = is_strengthen_str == "true" and true or false	
		is_could_strengthen = self.__mark_is_divination_could_strengthen_list[lineup_index][card_pos]
	end
	return is_could_strengthen
end

--获取当前阵容已祝福英雄数量
function Modules.HeroModule:GetLineupListBlessHeroNum()
	local bless_num = 0
	for _,lineup_info in ipairs(self.__hero_lineup_list) do
		if lineup_info.uid ~= 0 and lineup_info.bless_sprite_id ~= 0 then
			bless_num = bless_num + 1
		end
	end
	return bless_num
end

--设置援军助威等级
function Modules.HeroModule:SetReinforcementCheerLevel()
	if self.__cur_reinforcement_cheerlevel then
		local cur_cheer_info = self:GetHeroReinforcementCheerInfo()	
		local cur_reinforcement_cheerlevel = cur_cheer_info and cur_cheer_info.encourage_level or 0
		self.__cur_reinforcement_cheerlevel = cur_reinforcement_cheerlevel > self.__cur_reinforcement_cheerlevel and cur_reinforcement_cheerlevel or nil
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--红点相关
--------------------------------------------------------------------------------------------------------------------------------
--[[
1.需要更新的直接调用管理器统计，在统计的时候统一计算
2.不要更新的除了初始化的情况下都不需要重新计算
]]--
--计算英雄阵容红点
function Modules.HeroModule:CountHeroRedDot()	
	local lineup_list = self:GetLineupList()

	--角色信息
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] or {}
	self.__maincity_lineup_hero_reddot_list[1] = 0
	for lineup_index,lineup_info in ipairs(lineup_list) do		
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index] or {}
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] = 0		
		--阵位配置
		local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(lineup_index)
		--是否开启
		local is_open_lev = pos_info and pos_info.need_level <= role_info.level
		--英雄信息
		local hero_info = lineup_info and self:GetHeroInfo(lineup_info.uid)
		--是否显示图标数据
		local is_show_data = (is_open_lev and hero_info) and true or false
		--主界面tips
		if is_open_lev and not hero_info then
			self.__maincity_lineup_hero_reddot_list[1] = self.__maincity_lineup_hero_reddot_list[1] + 1
		end
		if is_show_data then			
			local isCouldUpgrade = self:JudgeIsCouldUpgrade(hero_info.uid)   
			if isCouldUpgrade then
				self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] + 1
			else
				local isCouldBreak = self:JudgeIsCouldBreak(hero_info.uid) 
				if isCouldBreak then
					self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] + 1
				end
			end
		else
			if is_open_lev then
				--红点
				local hero_info_list =self:GetLineupSelectList(nil,Macros.Game.HeroLineup.HERO_LINEUP,lineup_index,true)  
				if #hero_info_list > 0 then
					self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][1] + 1
				end
			end
		end	
	end	
end

--计算援军阵容红点
function Modules.HeroModule:CountReinforcementsRedDot()
	self.__maincity_lineup_hero_reddot_list[4] = 0
	--获取等级限制
	local reinforcementPosInfo = CPPGameLib.GetConfig("HeroLineup",Macros.Game.HeroLineup.REINFORCEMENT)
	self.__red_dot_info_map[Macros.Game.SystemID.REINFORCEMENT] = self.__red_dot_info_map[Macros.Game.SystemID.REINFORCEMENT] or {}
	--获取援军队列
	local hero_reinforcement_list = self:GetHeroReinforcementList()
	--可选援军队列
	local hero_info_list = self:GetLineupSelectList(nil,Macros.Game.HeroLineup.REINFORCEMENT,1,true)
	--总的红点
	local cur_red_dot_num = 0
	--获取角色等级
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)	
	for i,pos_info in ipairs(reinforcementPosInfo.pos_list) do
		local red_dot_num = 0
		if role_level >= pos_info.need_level then
			if hero_reinforcement_list[i].uid == 0 then	
				self.__maincity_lineup_hero_reddot_list[4] = self.__maincity_lineup_hero_reddot_list[4] + 1	
				if #hero_info_list > 0 then
					red_dot_num = red_dot_num + 1
				end
			end			  			
		end
		cur_red_dot_num = cur_red_dot_num + red_dot_num
		self.__red_dot_info_map[Macros.Game.SystemID.REINFORCEMENT][i] = red_dot_num
	end		
end

--计算装备红点
function Modules.HeroModule:CountEquipRedDot()
	self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] or {}

	local lineup_list = self:GetLineupList()
	local grid_widget_map =
	{
		["weapon_equipment_uid"] = Macros.Game.EquipType.WEAPON,
		["hat_equipment_uid"] = Macros.Game.EquipType.HELMET,
		["belt_equipment_uid"] = Macros.Game.EquipType.BELT,
		["clothes_equipment_uid"] = Macros.Game.EquipType.CLOTH,
		["attack_treasure_uid"] = Macros.Game.TreasureType.EMBLEM,
		["defence_treasure_uid"] = Macros.Game.TreasureType.GAD,
	}

	--获取已穿戴的装备或圣器
	local function GetEquipOrTreasure(lineup_info, key)
		if self:IsTreasureGrid(key) then
			return CallModuleFunc(ModuleType.TREASURE, "GetTreasure", lineup_info[key])
		end
		return CallModuleFunc(ModuleType.EQUIP, "GetEquip", lineup_info[key])
	end

	--主界面tips
	self.__maincity_lineup_hero_reddot_list[2] = 0
	self.__maincity_lineup_hero_reddot_list[3] = 0
	for i,lineup_info in ipairs(lineup_list) do
		--每个英雄装备初始为0
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i] or {}		
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] = 0
		if lineup_info and lineup_info.uid ~= 0 then
			for key,equip_type in pairs(grid_widget_map) do
				local equipList, obj_info

				--检测可穿戴装备或圣器中是否有更好的选择
				if self:IsTreasureGrid(key) then
					equipList = CallModuleFunc(ModuleType.TREASURE, "GetTreasureListByType",equip_type)			
				else
					equipList = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip",equip_type)
				end
				if #equipList > 0 then
					--装备/圣器信息
					obj_info = GetEquipOrTreasure(lineup_info, key)

					if not obj_info then
						--没有穿装备/圣器		
						local is_open = GlobalModulesMgr:IsOpen(self:IsTreasureGrid(key) and Macros.Game.SystemID.TREASURE_DRESS or Macros.Game.SystemID.EQUIP_DRESS)
						if is_open and #equipList > 0 then
							self.__maincity_lineup_hero_reddot_list[self:IsTreasureGrid(key) and 3 or 2] = self.__maincity_lineup_hero_reddot_list[self:IsTreasureGrid(key) and 3 or 2] + 1
							--有更好的装备/圣器
							self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] + 1
						end
					else
						--已穿装备/圣器
						local hero_info = self:GetHeroInfo(lineup_info.uid)

						--检测是否有更好的装备/圣器
						if self:ParseEquipListReinforcement(equipList,obj_info,hero_info.info_id,i) then
							self.__maincity_lineup_hero_reddot_list[self:IsTreasureGrid(key) and 3 or 2] = self.__maincity_lineup_hero_reddot_list[self:IsTreasureGrid(key) and 3 or 2] + 1
							self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] + 1	
						end
					end	
				end		

				--检测已穿戴装备中是否有可以强化或精炼的
				obj_info = GetEquipOrTreasure(lineup_info, key)
				if obj_info and 
					((not self:IsTreasureGrid(key) and (CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsStrengthen", obj_info.uid) == "true" or CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsRefine", obj_info.uid) == "true")) or								
					(self:IsTreasureGrid(key) and (CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsStrengthen", obj_info.uid) == "true" or CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsRefine", obj_info.uid) == "true"))) then								
					self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][2] + 1	
				end
			end
		end	
	end
end

--计算占卜卡牌红点
function Modules.HeroModule:CountDivinationRedDot()
	self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP] or {}
	local lineup_list = self:GetLineupList()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	--可穿戴的卡牌队列
	local divination_list = CallModuleFunc(ModuleType.DIVINATION, "GetAllDivinationWithoutWear")
	--主界面tips
	for i,lineup_info in ipairs(lineup_list) do
		--每个英雄装备初始为0
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i] or {}		
		self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3] = {}
		self.__mark_divination_strengthen_list[i] = {}
		self.__mark_divination_break_list[i] = {}
		self.__mark_is_divination_could_strengthen_list[i] = {}
		self.__mark_is_divination_could_break_list[i] = {}
		if lineup_info and lineup_info.uid ~= 0 then			
			--如果可穿卡牌队列大于零
			for pos=1,8 do	
				self.__mark_divination_strengthen_list[i][pos] = false
				self.__mark_divination_break_list[i][pos] = false
				self.__mark_is_divination_could_strengthen_list[i][pos] = false
				self.__mark_is_divination_could_break_list[i][pos] = false
				--重置各个位置红点数量
				self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3][pos] = 0										
				--卡牌信息				
				local obj_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination",lineup_info.divination_list[pos].card_id)
				--根据卡牌信息来进行判断
				if not obj_info and #divination_list > 0 then
					--没有穿卡牌
					local hole_info = CPPGameLib.GetConfig("DivinationHole",pos)			
					if hole_info and role_level >= hole_info.role_level then							
						--可穿其他类型卡牌
						local is_have_another_type = self:JudgeIsHaveAnotherDivinationType(divination_list,lineup_info.divination_list)
						if is_have_another_type then
							self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3][pos] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3][pos] + 1
						end
					end
				else
					--穿了卡牌
					local hero_info = self:GetHeroInfo(lineup_info.uid)
					if self:ParseDivinationListReinforcement(divination_list,obj_info,hero_info.info_id,i,pos) then
						--提高战力
						self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3][pos] = self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][i][3][pos] + 1
					end
				end				
			end
		end	
	end
end

--获取红点数量列表
function Modules.HeroModule:GetRedDotInfoMap()
	return self.__red_dot_info_map
end

--获取主界面上阵英雄红点列表
function Modules.HeroModule:GetMainCityLineupHeroRedDotList()
	return self.__maincity_lineup_hero_reddot_list
end

--获取对应援军阵位英雄的红点
function Modules.HeroModule:GetReinforcementRedDotNumByLineUpIndex(lineup_index)
	return self.__red_dot_info_map[Macros.Game.SystemID.REINFORCEMENT][lineup_index]
end

--获取对应阵位英雄的装备红点
function Modules.HeroModule:GetHeroEquipRedDotNumByLineUpIndex(lineup_index)
	return self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][2]
end

--获取对应阵位英雄的卡牌红点
function Modules.HeroModule:GetHeroDivinationRedDotNumByLineUpIndex(lineup_index)
	return self.__red_dot_info_map[Macros.Game.SystemID.HERO_LINEUP][lineup_index][3] or {}
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--计算队伍属性加成
function Modules.HeroModule:__CalcTeamPlusProps()
	local plus_prop_map = {}

	local hero_info
	for _, lineup_info in ipairs(self.__hero_lineup_list) do
		hero_info = self:GetHeroInfo(lineup_info.uid)
		if hero_info then
			--进阶加成
			local break_plus_prop_map = ConfigAdapter.Hero.GetTeamBreakPlusPropMap(hero_info.info_id, hero_info.break_level)
			if break_plus_prop_map then
				for prop_type, prop_value in pairs(break_plus_prop_map) do
					plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
					plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value
				end
			end
		end
	end

	local prop_key
	for _, lineup_info in ipairs(self.__hero_lineup_list) do
		hero_info = self:GetDetailHeroInfo(lineup_info.uid, true)
		Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
	end	
end

--获得对应的激活缘分
function Modules.HeroModule:__GetCorrespondingReinforcementName(type,hero_info_id,info_id,hero_info)
	local config_hero = CPPGameLib.GetConfig("Hero",hero_info_id)
	local work_list = {}
	if type == Macros.Game.WIND_WORDS_LINEUP_TYPE.EQUIP_MESSAGE then
		--品阶是否改变
		local is_quality_change = hero_info.quality_level > config_hero.init_quality
		--与装备的宿命
		local cur_relation_equip_list = is_quality_change and config_hero.relation_equip_ex_list or config_hero.relation_equip_list
		local relation_equip_reinforcementname_list = is_quality_change and config_hero.relation_equip_reinforcementname_ex_list or config_hero.relation_equip_reinforcementname_list
		if cur_relation_equip_list then
			for i, relation_equip_list in ipairs(cur_relation_equip_list) do	
				is_activated = false
				for _, equip_id in ipairs(relation_equip_list) do
					if equip_id == info_id then
						is_activated = true
						break
					end
				end						
				local data = {}	
				data = {reinforcementname = relation_equip_reinforcementname_list[i], pos = "equip_list_"..i,hero_name = config_hero.name}							
				if is_activated then
					table.insert(work_list, data)
				end
			end
		end
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.TREASURE_MESSAGE then
		--品阶是否改变
		local is_quality_change = hero_info.quality_level > config_hero.init_quality
		--与装备的宿命
		local cur_relation_treasure_list = is_quality_change and config_hero.relation_treasure_ex_list or config_hero.relation_treasure_list
		local relation_treasure_reinforcementname_list = is_quality_change and config_hero.relation_treasure_reinforcementname_ex_list or config_hero.relation_treasure_reinforcementname_list
		--圣器的宿命
		if cur_relation_treasure_list then
			for i,relation_treasure_list in ipairs(cur_relation_treasure_list) do	
				is_activated = false
				for _, treasure_id in ipairs(relation_treasure_list) do
					if treasure_id == info_id then
						is_activated = true
						break
					end
				end		
				local data = {}	
				data = {reinforcementname = relation_treasure_reinforcementname_list[i] , pos = "treasure_list_"..i,hero_name = config_hero.name}							
				if is_activated then
					table.insert(work_list, data)
				end
			end
		end
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.DIVINATION_MESSAGE then
		--卡牌的宿命
		if config_hero.relation_card_list then
			local divination_type_list = self:GetLineupDivinationInfo(info_id)
			for i,relation_card_list in ipairs(config_hero.relation_card_list) do	
				is_activated = true
				for _, card_type in ipairs(relation_card_list) do
					if not divination_type_list[card_type] then
						is_activated = false
						break
					end
				end		
				local data = {}	
				data = {reinforcementname = config_hero.relation_card_reinforcementname_list[i] , pos = "card_list_"..i,hero_name = config_hero.name}							
				if is_activated then
					table.insert(work_list, data)
				end
			end
		end
	else
		
	end
	return work_list
end

--存储当前选择的阵位英雄
function Modules.HeroModule:__SetSelectedLineupIndex(selected_lineup_index)
	self.__selected_lineup_index = selected_lineup_index
end

--存储当前选择的阵位英雄
function Modules.HeroModule:__GetSelectedLineupIndex()
	return self.__selected_lineup_index
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--英雄列表排序函数
sort_hero_info_list = function(info1, info2)
	if not info1 then
		return false
	end
	if not info2 then
		return true
	end

	--上阵优先
	if info1.is_to_battle == Macros.Global.TRUE and info2.is_to_battle == Macros.Global.FALSE then
		return true
	elseif info1.is_to_battle == Macros.Global.FALSE and info2.is_to_battle == Macros.Global.TRUE then
		return false
	end

	--品质从高到低
	if info1.quality_level > info2.quality_level then
		return true
	elseif info1.quality_level < info2.quality_level then
		return false
	end

	--等级从高到低
	if info1.level > info2.level then
		return true
	elseif info1.level < info2.level then
		return false
	end

	--进阶等级从高到低
	if info1.break_level > info2.break_level then
		return true
	elseif info1.break_level < info2.break_level then
		return false
	end

	--潜能等级从高到低
	if info1.potential_level > info2.potential_level then
		return true
	elseif info1.potential_level < info2.potential_level then
		return false
	end

	--觉醒星级从高到低
	if info1.awaken_star > info2.awaken_star then
		return true
	elseif info1.awaken_star < info2.awaken_star then
		return false
	end

	--觉醒等级从高到低
	if info1.awaken_level > info2.awaken_level then
		return true
	elseif info1.awaken_level < info2.awaken_level then
		return false
	end

	--英雄ID从大到小
	return info1.info_id > info2.info_id
end
