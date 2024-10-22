--[[
%% @module: 英雄模块
%% @author: swordman sue
%% @created: 2016-08-19
--]]

Modules = Modules or {}

Modules.HeroModule = Modules.HeroModule or BaseClass(BaseModule)

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--召唤请求
function Modules.HeroModule:HeroRecruitReq(callType)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_RECRUIT_REQ)
	protocol.type = callType
	GlobalNetAdapter:OnSend(protocol)
	if callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.LOWMOREPROP_RECRUIT or 
		callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.FRIENDPOINT_MORERECRUIT or 
		 callType == Macros.Game.HERO_RECRUIT_DETAIL_TYPE.HIGHMOREMONEY_RECRUIT then
		self.__is_on_tencall = true
	end
end

--召唤回调
function Modules.HeroModule:HeroRecruitResp(protocol_data)	
	self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE,protocol_data.type,protocol_data.hero_list)
	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.HERO_RECRUIT)
end

--召唤数据请求
function Modules.HeroModule:HeroRecruitDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_RECRUITDATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--召唤数据回调
function Modules.HeroModule:HeroRecruitDataResp(protocol_data)
	local old_advanced_summon_time = self._recruitData and self._recruitData.advanced_summon_time or nil

	self._recruitData = {}
	self._recruitData.normal_summon_times = protocol_data.normal_summon_times
	self._recruitData.normal_summon_time = protocol_data.normal_summon_time
	self._recruitData.advanced_summon_times = protocol_data.advanced_summon_times
	self._recruitData.advanced_summon_time = protocol_data.advanced_summon_time
	self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE,Macros.Game.HERO_RECRUIT_DETAIL_TYPE.UPDATE_RECRUIT_TIME,protocol_data)

	if old_advanced_summon_time ~= protocol_data.advanced_summon_time then
		--更新系统推送(高级召唤的免费次数恢复)
		MsgPushHandle.UpdateLocalPushFun(MsgPushHandle.PushKey.HIGH_RECURIT_TIMES_RECOVERY)
	end
end

--收集请求
function Modules.HeroModule:HeroCollectionReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_COLLECTION_LIST_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--收集回调
function Modules.HeroModule:HeroCollectionResp(protocol_data)
	self.CollectionList = {}
	for k,v in pairs(protocol_data.hero_collection_list) do
		self.CollectionList[v.hero_info_id] = v.hero_info_id
	end

	self:FaceliftSettle()
	self:FireNextFrame(Macros.Event.HeroModule.HERO_GATHER)
end

function Modules.HeroModule:FaceliftSettle()
	self.facelift = {}
	self.facelift.get = {}
	self.facelift.no = {}

	local lst = {}
	for _ , v in pairs(CPPGameLib.GetConfig("HeroHandBook",nil,nil,true)) do
		for __,vv in pairs(v.hero_list) do
			for ___,vvv in pairs(vv.hero_list ) do
				table.insert( lst , { id = vvv , init_quality = vv.quality } )
			end
		end
	end

	for i , v in pairs(lst) do
		if v.init_quality >= 3 then
			local type_ = false
			if self.CollectionList[v.id] then
				type_ = true
				local cnf = ObjIDTool.GetInfo(v.id)
				table.insert(self.facelift.get , {id = v.id , type_ = type_ ,quality = v.init_quality } )
			else
				table.insert(self.facelift.no , {id = v.id , type_ = type_ ,quality = v.init_quality } )
			end
		end
	end

	-- 2016年12月13日11:41:17 除了获得的放前面 其他的不需要排序 ————大硕硕
	-- 可是我不信怎么办？

	table.sort( self.facelift.get, function ( a , b )
		if a.quality > b.quality then
			return a.quality > b.quality 
		elseif a.quality == b.quality then
			return a.id < b.id
		end
	end )

	table.sort( self.facelift.no, function ( a , b )
		if a.quality < b.quality then
			return a.quality < b.quality 
		elseif a.quality == b.quality then
			return a.id < b.id
		end
	end )
end

--援军上阵/下阵请求
function Modules.HeroModule:RecinforcementToBattleReq(hero_uid, pos, is_lineup)	
	self.__is_lineup_view = is_lineup
	self.__is_on_changing_property = true

	if hero_uid > 0 then
		self:SaveHeroPropertyBeforeChange(hero_uid, pos, true)
	end

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REINFORCEMENT_TO_BATTLE_REQ)
	protocol.hero_uid = hero_uid
	protocol.pos = pos
	GlobalNetAdapter:OnSend(protocol)
end

--援军上阵/下阵响应
function Modules.HeroModule:RecinforcementToBattleResp(protocol_data)
	self.__is_on_changing_property = false
	local view	 
	if self.__is_lineup_view then
		view = self:GetViewByType(BaseViewType.HERO_LINEUP)	
	else
		view = self:GetView()	
	end

	--援军上阵
	if protocol_data.hero_uid > 0 then
		self:SetIsHaveChangePropertyNeedWindWords(true)
		self:SetMessageTypeAndTranslateInfo(Macros.Game.WIND_WORDS_LINEUP_TYPE.REINFORCEMENT_MESSAGE)
		self:SetReinforcementCheerLevel()

		if view and view:IsOpen() then
			if self.__is_lineup_view then
				view:HeroGotoReinforcementView()
			else
				view:CloseManual()
			end			
		end	

		self:FireNextFrame(Macros.Event.HeroModule.HERO_REINFORCEMENTS_ON_BATTLE, protocol_data.pos, protocol_data.hero_uid)		

	--援军下阵
	else
		if view and view:IsOpen() then
			view:CloseManual()	
		end	

		self:FireNextFrame(Macros.Event.HeroModule.HERO_REINFORCEMENTS_OFF_BATTLE, protocol_data.pos)					
	end	

	self:FireNextFrame(Macros.Event.HeroModule.HERO_ON_OR_OFF_FIGHT, protocol_data.hero_uid > 0) 			
end

--"hero_uid__I",      --英雄uid
--英雄进化请求
function Modules.HeroModule:HeroEvolutionReq(hero_uid)
	local hero_info = self:GetHeroInfo(hero_uid)
	if not hero_info then
		return
	end
	self.__before_heroevolution_req_quality = hero_info.quality_level
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_EVOLUTION_REQ)
	protocol.hero_uid = hero_uid
	GlobalNetAdapter:OnSend(protocol)
end

-- "hero_uid__I",  --英雄uid
-- "quality__C",   --当前品质
-- "stage__C", 	   --当前品阶
--英雄进化响应
function Modules.HeroModule:HeroEvolutionResp(protocol_data)
	CPPGameLib.PrintTable("skfb",protocol_data,"HeroEvolutionRespHeroEvolutionRespHeroEvolutionResp ")
	self:FireNextFrame(Macros.Event.HeroModule.HERO_EVOLUTION_NOTICE,protocol_data,self.__before_heroevolution_req_quality)		
end
-- "camp__H",      --阵营
--VIP召唤请求
function Modules.HeroModule:HeroVipSummonReq(camp)
	self.__is_on_tencall = true
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_VIP_SUMMON_REQ)
	protocol.camp = camp
	GlobalNetAdapter:OnSend(protocol)
end

-- "camp__H",      --阵营
-- "hero_list__T__hero_id@H",  -- 召唤获得的英雄列表
--VIP召唤响应
function Modules.HeroModule:HeroVipSummonResp(protocol_data)
	self:Fire(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE, Macros.Game.HERO_RECRUIT_DETAIL_TYPE.VIP_RECRUIT, protocol_data.hero_list)	
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--获得英雄召唤数据
function Modules.HeroModule:GetHeroRecruitInfo()
	return self._recruitData
end

--设置初级英雄招募免费状态
function Modules.HeroModule:SetHeroLowRecruitFreeState(isFree)
	self.LowRecruitState = isFree
end

--获得初级英雄招募免费状态
function Modules.HeroModule:GetHeroLowRecruitFreeState()
	return self.LowRecruitState
end

--设置高级英雄招募免费状态
function Modules.HeroModule:SetHeroHighRecruitFreeState(isFree)
	self.HighRecruitState = isFree
end

--获得高级英雄招募免费状态
function Modules.HeroModule:GetHeroHighRecruitFreeState()
	return self.HighRecruitState
end


--获得援军列表
function Modules.HeroModule:GetHeroReinforcementList()
	return self.__hero_reinforcement_list
end

--获得收集列表
function Modules.HeroModule:GetCollectionList()
	return self.CollectionList
end

--获得该英雄是否进化了
function Modules.HeroModule:GetIsHeroEvolution(uid)
	if not uid then
		return
	end
	local hero_info = self:GetHeroInfo(uid)
	if hero_info then
		local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
		if config_hero then
			return hero_info.quality_level > config_hero.init_quality
		end
	end
	return false
end

--获取所有已经上阵的英雄列表(包括援军)
function Modules.HeroModule:GetAllLineupHeroList(lineup_type, lineup_index)
	local all_lineup_hero_map = {}
	local only_lineup_hero_map = {}
	local function handle_func(i, lineup_info, to_lineup_type)
		if lineup_type == to_lineup_type then
			--不把待切换英雄位置对应的英雄宿命算进去
			if i ~= lineup_index then
				if lineup_info.uid ~= 0 then
					local info = self:GetHeroInfo(lineup_info.uid)
					all_lineup_hero_map[info.info_id] = info.info_id					
				end
			end
		else
			if lineup_info.uid ~= 0 then
				local info = self:GetHeroInfo(lineup_info.uid)
				all_lineup_hero_map[info.info_id] = info.info_id				
			end
		end
	end

	local lineupList = self:GetLineupList()
	for i, lineup_info in ipairs(lineupList) do
		handle_func(i, lineup_info, Macros.Game.HeroLineup.HERO_LINEUP)
	end
	--上阵英雄列表
	for info_id,hero_id in pairs(all_lineup_hero_map) do
		only_lineup_hero_map[info_id] = hero_id
	end
	local reinforcement_list = self:GetHeroReinforcementList()
	for i, lineup_info in ipairs(reinforcement_list) do
		local info = self:GetHeroInfo(lineup_info.uid)
		if info then
			handle_func(i, lineup_info, Macros.Game.HeroLineup.REINFORCEMENT)
		end
	end

	return all_lineup_hero_map,only_lineup_hero_map
end

--获取背包中某英雄可激活的宿命数量和宿命名字
function Modules.HeroModule:GetCanActiavteReinforcementsNum(uid, lineup_type, lineup_index)
	local hero_info = self:GetHeroInfo(uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	--print("ldx","hero_info.info_id:ChnagLIU ",hero_info.info_id)
	--还要排除援军之间的相互宿命
	local all_lineup_hero_map,only_lineup_hero_map = self:GetAllLineupHeroList(lineup_type, lineup_index)
	
	local reinforcementsNum = 0
	local reinforcementnamelist = {}	
	--与人的宿命
	if config_hero.relation_hero_list then
		for i,relation_hero_list in ipairs(config_hero.relation_hero_list) do	
			local IsMakeWork = true	
			--是否激活的宿命中没有一个上阵英雄 	
			local is_have_lineuphero = false	
			for _, hero_id in ipairs(relation_hero_list) do	
				if lineup_type == Macros.Game.HeroLineup.REINFORCEMENT then
					if only_lineup_hero_map[hero_id] then
						is_have_lineuphero = true
					end
				else
					is_have_lineuphero = true
				end					
				if not all_lineup_hero_map[hero_id] then
					IsMakeWork = false
					break
				end
			end
			if IsMakeWork and is_have_lineuphero then	
				if lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then
					reinforcementsNum = reinforcementsNum + 1
					local info = {}
					info.reinforcementname = config_hero.relation_hero_reinforcementname_list[i]
					info.hero_name = config_hero.name
					info.pos = "hero_list_"..i
					table.insert(reinforcementnamelist,info)
				end
				for _,hero_id in ipairs(relation_hero_list) do
					if only_lineup_hero_map[hero_id] then
						reinforcementsNum = reinforcementsNum + 1
						local info = {}
						info.reinforcementname = config_hero.relation_hero_reinforcementname_list[i]
						local config_hero = CPPGameLib.GetConfig("Hero",hero_id)
						info.hero_name = config_hero.name
						info.pos = "hero_list_"..i
						table.insert(reinforcementnamelist,info)	
					end
				end								
			end		
		end
	end

	--与人的宿命(单向宿命)
	for _,info_id in pairs(only_lineup_hero_map) do
		local config_hero = CPPGameLib.GetConfig("Hero",info_id)
		if config_hero and config_hero.relation_hero_list then
			for i,relation_hero_list in ipairs(config_hero.relation_hero_list) do
				local IsMakeWork = true	
				local is_have_curhero = false
				for _,hero_id in ipairs(relation_hero_list) do					
					if not is_have_curhero then
						is_have_curhero = hero_id == hero_info.info_id
					end
					if not all_lineup_hero_map[hero_id] and hero_id ~= hero_info.info_id then
						IsMakeWork = false
						break
					end
				end	
				--激活并且含有当前英雄
				if IsMakeWork and is_have_curhero then
					local is_noexist = true
					--检查之前的宿命中是否已经存在了
					for _,reinforcementnameinfo in ipairs(reinforcementnamelist) do
						if reinforcementnameinfo.reinforcementname == config_hero.relation_hero_reinforcementname_list[i] and
							reinforcementnameinfo.hero_name == config_hero.name then
							is_noexist = false
							break
						end
					end
					if is_noexist then
						reinforcementsNum = reinforcementsNum + 1
						local info = {}
						info.reinforcementname = config_hero.relation_hero_reinforcementname_list[i]
						info.hero_name = config_hero.name
						info.pos = "hero_list_"..(10000+i)
						table.insert(reinforcementnamelist,info)
					end
				end			
			end
		end
	end
	
	--上阵英雄才算装备和圣器的
	if lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then
		--获取当前位置的装备列表
		local lineup_equip_map = self:GetLineupEquipInfo(lineup_index)
		--装备的宿命
		if config_hero.relation_equip_list then
			for i, relation_equip_list in ipairs(config_hero.relation_equip_list) do	
				local IsMakeWork = false
				for _, equip_id in ipairs(relation_equip_list) do
					if lineup_equip_map[equip_id] then
						IsMakeWork = true
						break
					end
				end		
				if IsMakeWork then			
					reinforcementsNum = reinforcementsNum + 1
					local info = {}
					info.reinforcementname = config_hero.relation_equip_reinforcementname_list[i]
					info.pos = "equip_list_"..i
					info.hero_name = config_hero.name
					table.insert(reinforcementnamelist,info)	
				end
			end
		end

		--圣器的宿命
		if config_hero.relation_treasure_list then
			for i,relation_treasure_list in ipairs(config_hero.relation_treasure_list) do	
				local IsMakeWork = false
				for _, treasure_id in ipairs(relation_treasure_list) do
					if lineup_equip_map[treasure_id] then
						IsMakeWork = true
						break
					end
				end		
				if IsMakeWork then			
					reinforcementsNum = reinforcementsNum + 1
					local info = {}
					info.reinforcementname = config_hero.relation_treasure_reinforcementname_list[i]
					info.pos = "treasure_list_"..i
					info.hero_name = config_hero.name
					table.insert(reinforcementnamelist,info)
				end
			end
		end

		--卡牌的宿命
		if config_hero.relation_card_list then
			--对应阵位卡牌的穿戴情况
			local divination_type_list = self:GetLineupDivinationInfo(lineup_index)
			for i,relation_card_list in ipairs(config_hero.relation_card_list) do	
				local IsMakeWork = true
				for _, card_type in ipairs(relation_card_list) do
					if not relation_card_list[card_type] then
						IsMakeWork = false
						break
					end
				end		
				if IsMakeWork then			
					reinforcementsNum = reinforcementsNum + 1
					local info = {}
					info.reinforcementname = config_hero.relation_card_reinforcementname_list[i]
					info.pos = "card_list_"..i
					info.hero_name = config_hero.name
					table.insert(reinforcementnamelist,info)
				end
			end
		end
	end
	return reinforcementsNum,reinforcementnamelist
end

--获取当前英雄能否跟队列上除了传进来阵位英雄有合击
function Modules.HeroModule:GetCanActiavteTogetherAttackName(info_id,lineup_index)
	local hero_name_list = {}
	local quality_list = {}
	local lineupList = self:GetLineupList()
	for i, lineup_info in ipairs(lineupList) do
		if lineup_info.uid ~= 0 and i ~= lineup_index then
			local info = self:GetHeroInfo(lineup_info.uid)
			if info then
				local hero_config = CPPGameLib.GetConfig("Hero",info.info_id)
				if hero_config.joint_hero_show_list then
					for i,hero_id in ipairs(hero_config.joint_hero_show_list) do
						if hero_id == info_id then
							table.insert(hero_name_list,hero_config.name)
							table.insert(quality_list,hero_config.init_quality)
							break
						end
					end
				end
			end
		end
	end
	return hero_name_list,quality_list
end

--获取某英雄宿命信息列表(名字、对象列表、是否激活、描述)
function Modules.HeroModule:GetHeroReinforcementInfoList(uid)
	local hero_info = self:GetHeroInfo(uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	local work_list = {}
	local unwork_list = {}

	--获取已上阵英雄
	local all_lineup_hero_map = self:GetAllLineupHeroList()

	--获取当前位置的装备列表
	local lineup_index = self:GetLineupInfoByUID(uid)
	local lineup_equip_map = self:GetLineupEquipInfo(lineup_index)	

	local is_activated

	--与人的宿命
	if config_hero.relation_hero_list then
		for i, relation_hero_list in ipairs(config_hero.relation_hero_list) do
			is_activated = true
			local real_relation_hero_list = {hero_info.info_id}
			for _, hero_id in ipairs(relation_hero_list) do										
				if not all_lineup_hero_map[hero_id] or not lineup_index then
					is_activated = false
				end
				table.insert(real_relation_hero_list, hero_id)
			end		

			local data = {}	
			data = {reinforcementname = config_hero.relation_hero_reinforcementname_list[i], reinforcementdes = config_hero.relation_hero_reinforcementdes_list[i], relation_obj_list = real_relation_hero_list, IsMakeWork = is_activated , pos = "hero_list_"..i}							
			if is_activated then
				table.insert(work_list, data)
			else
				table.insert(unwork_list, data)
			end
		end
	end
	--品阶是否改变
	local is_quality_change = hero_info.quality_level > config_hero.init_quality
	--与装备的宿命
	local cur_relation_equip_list = is_quality_change and config_hero.relation_equip_ex_list or config_hero.relation_equip_list
	local relation_equip_reinforcementname_list = is_quality_change and config_hero.relation_equip_reinforcementname_ex_list or config_hero.relation_equip_reinforcementname_list
	local relation_equip_reinforcementdes_list = is_quality_change and config_hero.relation_equip_reinforcementdes_ex_list or config_hero.relation_equip_reinforcementdes_list
	if cur_relation_equip_list then
		for i, relation_equip_list in ipairs(cur_relation_equip_list) do	
			is_activated = false
			
			local real_relation_equip_list = {hero_info.info_id}
			for _, equip_id in ipairs(relation_equip_list) do
				table.insert(real_relation_equip_list, equip_id)
				if lineup_equip_map[equip_id] and lineup_index then
					is_activated = true					
				end
			end		
			local count_num = 3
			--判断
			while #real_relation_equip_list >= count_num do	
				table.insert(real_relation_equip_list,count_num,"text")	
				count_num = count_num + 2
			end
			local data = {}	
			data = {reinforcementname = relation_equip_reinforcementname_list[i], reinforcementdes = relation_equip_reinforcementdes_list[i], relation_obj_list = real_relation_equip_list, IsMakeWork = is_activated , pos = "equip_list_"..i}							
			if is_activated then
				table.insert(work_list, data)
			else
				table.insert(unwork_list, data)
			end
		end
	end

	--圣器的宿命
	local cur_relation_treasure_list = is_quality_change and config_hero.relation_treasure_ex_list or config_hero.relation_treasure_list
	local relation_treasure_reinforcementname_list = is_quality_change and config_hero.relation_treasure_reinforcementname_ex_list or config_hero.relation_treasure_reinforcementname_list
	local relation_treasure_reinforcementdes_list = is_quality_change and config_hero.relation_treasure_reinforcementdes_ex_list or config_hero.relation_treasure_reinforcementdes_list
	if cur_relation_treasure_list then
		for i,relation_treasure_list in ipairs(cur_relation_treasure_list) do	
			is_activated = false
			
			local real_relation_treasure_list = {hero_info.info_id}
			for _, treasure_id in ipairs(relation_treasure_list) do
				table.insert(real_relation_treasure_list, treasure_id)
				if lineup_equip_map[treasure_id] and lineup_index then
					is_activated = true
				end
			end		
			local count_num = 3
			--判断
			while #real_relation_treasure_list >= count_num do	
				table.insert(real_relation_treasure_list,count_num,"text")	
				count_num = count_num + 2
			end
			local data = {}	
			data = {reinforcementname = relation_treasure_reinforcementname_list[i], reinforcementdes = relation_treasure_reinforcementdes_list[i], relation_obj_list = real_relation_treasure_list, IsMakeWork = is_activated , pos = "treasure_list_"..i}							
			if is_activated then
				table.insert(work_list, data)
			else
				table.insert(unwork_list, data)
			end
		end
	end

	--卡牌的宿命	
	if config_hero.relation_card_list then
		--对应阵位卡牌的穿戴情况
		local divination_type_list = self:GetLineupDivinationInfo(lineup_index)
		--遍历卡牌宿命列表
		for i,relation_card_list in ipairs(config_hero.relation_card_list) do	
			is_activated = true
			
			local real_relation_card_list = {hero_info.info_id}
			for _, card_type in ipairs(relation_card_list) do
				table.insert(real_relation_card_list, card_type)
				if not divination_type_list[card_type] then
					is_activated = false
				end
			end		
			local data = {}	
			data = {reinforcementname = config_hero.relation_card_reinforcementname_list[i], reinforcementdes = config_hero.relation_card_reinforcementdes_list[i], relation_obj_list = real_relation_card_list, IsMakeWork = is_activated , pos = "card_list_"..i,is_card = true,
					recommend_card_list = config_hero.recommend_card_list[i]}							
			if is_activated then
				table.insert(work_list, data)
			else
				table.insert(unwork_list, data)
			end
		end
	end

	return CPPGameLib.MergeTable2List(work_list, unwork_list)
end

--获取某英雄宿命属性加成表(名字、对象列表、是否激活、描述)
function Modules.HeroModule:GetHeroReinforcementPlusPropMap(uid)
	local plus_prop_map = {}

	local hero_info = self:GetHeroInfo(uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	--获取已上阵英雄
	local all_lineup_hero_map = self:GetAllLineupHeroList()

	--获取当前位置的装备列表
	local lineup_index = self:GetLineupInfoByUID(uid)
	local lineup_equip_map = self:GetLineupEquipInfo(lineup_index)

	--统计属性加成
	local function calc_plus_prop(prop_type_list, prop_value_list)
		for prop_index, prop_type in ipairs(prop_type_list) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + (prop_value_list[prop_index] or 0)
		end
	end

	local is_activated

	--与英雄的宿命
	if config_hero.relation_hero_list then
		for i, relation_hero_list in ipairs(config_hero.relation_hero_list) do
			is_activated = true
			for _, hero_id in ipairs(relation_hero_list) do										
				if not all_lineup_hero_map[hero_id] or not lineup_index then
					is_activated = false
					break
				end
			end		

			if is_activated then
				--统计属性加成
				calc_plus_prop(config_hero.relation_hero_pro_type_list[i], config_hero.relation_hero_pro_value_list[i])
			end
		end
	end
	--品阶是否改变
	local is_quality_change = hero_info.quality_level > config_hero.init_quality
	--与装备的宿命
	local cur_relation_equip_list = is_quality_change and config_hero.relation_equip_ex_list or config_hero.relation_equip_list
	local relation_equip_pro_type_list = is_quality_change and config_hero.relation_equip_pro_type_ex_list or config_hero.relation_equip_pro_type_list
	local relation_equip_pro_value_list = is_quality_change and config_hero.relation_equip_pro_value_ex_list or config_hero.relation_equip_pro_value_list
	if cur_relation_equip_list then
		for i, relation_equip_list in ipairs(cur_relation_equip_list) do	
			is_activated = false

			for _, equip_id in ipairs(relation_equip_list) do
				if lineup_equip_map[equip_id] and lineup_index then
					is_activated = true
					break
				end
			end		
			
			if is_activated then
				--统计属性加成
				calc_plus_prop(relation_equip_pro_type_list[i], relation_equip_pro_value_list[i])
			end
		end
	end

	--与圣器的宿命
	local cur_relation_treasure_list = is_quality_change and config_hero.relation_treasure_ex_list or config_hero.relation_treasure_list
	local relation_treasure_pro_type_list = is_quality_change and config_hero.relation_treasure_pro_type_ex_list or config_hero.relation_treasure_pro_type_list
	local relation_treasure_pro_value_list = is_quality_change and config_hero.relation_treasure_pro_value_ex_list or config_hero.relation_treasure_pro_value_list
	if cur_relation_treasure_list then
		for i,relation_treasure_list in ipairs(cur_relation_treasure_list) do	
			is_activated = false

			for _, treasure_id in ipairs(relation_treasure_list) do
				if lineup_equip_map[treasure_id] and lineup_index then
					is_activated = true
					break
				end
			end		

			if is_activated then
				--统计属性加成
				calc_plus_prop(relation_treasure_pro_type_list[i], relation_treasure_pro_value_list[i])
			end
		end
	end

	--与卡牌的宿命
	if config_hero.relation_card_list then
		--对应阵位卡牌的穿戴情况
		local divination_type_list = self:GetLineupDivinationInfo(lineup_index)

		for i,relation_card_list in ipairs(config_hero.relation_card_list) do
			is_activated = true

			for _, card_type in ipairs(relation_card_list) do
				if not divination_type_list[card_type] then
					is_activated = false
					break
				end
			end

			if is_activated then
				--统计属性加成
				calc_plus_prop(config_hero.relation_card_pro_type_list[i], config_hero.relation_card_pro_value_list[i])
			end			
		end
	end

	return plus_prop_map
end

--获取某英雄援军助威属性加成表
function Modules.HeroModule:GetHeroReinforcementCheerPlusPropMap()
	local plus_prop_map = {}
	--判断系统是否开启
	if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.REINFORCEMENTS_CHEER) then
		return plus_prop_map
	end

	local cur_cheer_info = self:GetHeroReinforcementCheerInfo()
  	if not cur_cheer_info then
  		return plus_prop_map
  	end
	--统计属性加成
	for prop_index, prop_type in ipairs(cur_cheer_info.pro_type_list) do
		plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
		plus_prop_map[prop_type] = plus_prop_map[prop_type] + (cur_cheer_info.pro_value_list[prop_index] or 0)
	end

	return plus_prop_map
end

--获取当前援军助威信息
function Modules.HeroModule:GetHeroReinforcementCheerInfo()
	--获取已上阵英雄
	local reinforcement_hero_map = self:GetHeroReinforcementList()
	--是否有空位
  	local is_have_nil = false
	--援军最低等级
  	local least_reinforcement_level
  	--遍历援军
  	for lineup_index, lineup_info in ipairs(reinforcement_hero_map) do
	    local hero_info = self:GetHeroInfo(lineup_info.uid)   
	    if hero_info then
	    	least_reinforcement_level = least_reinforcement_level or hero_info.level
	    	least_reinforcement_level = hero_info.level < least_reinforcement_level and hero_info.level or least_reinforcement_level
	    else
	    	is_have_nil = true
	    	break
	    end
  	end
  	least_reinforcement_level = not is_have_nil and least_reinforcement_level or nil
  	--助威表
  	local config_encourage = CPPGameLib.GetConfig("ReinforcementEncourage",nil,nil,true)
  	--助威增加属性
  	local cur_cheer_info
  	local max_config_encourage = config_encourage[table.maxn(config_encourage)]
  	if least_reinforcement_level then	
  		if least_reinforcement_level > max_config_encourage.reinforcement_level_max then
  			cur_cheer_info = max_config_encourage
  		else
	  		for _,reinforcement_info in pairs(config_encourage) do
	  			if reinforcement_info.reinforcement_level_max >= least_reinforcement_level and reinforcement_info.reinforcement_level_min <= least_reinforcement_level then 
	  				cur_cheer_info = reinforcement_info			
	  				break
	  			end  			
	  		end 		
	  	end
  	end
  	return cur_cheer_info
end

--获取装备或圣器所在的阵位索引
function Modules.HeroModule:GetLineupIndexOfEquipOrTreasure(uid)
	for lineup_index, lineup_info in ipairs(self.__hero_lineup_list) do
		if uid == lineup_info.weapon_equipment_uid or
			uid == lineup_info.hat_equipment_uid or
			uid == lineup_info.belt_equipment_uid or
			uid == lineup_info.clothes_equipment_uid or
			uid == lineup_info.attack_treasure_uid or
			uid == lineup_info.defence_treasure_uid then

			return lineup_index
		end
	end
end

--获取收集列表
function Modules.HeroModule:GetFacelift()
	return self.facelift
end

--------------------------------------------------------------------------------------------------------------------------------
--英雄飘字相关
--------------------------------------------------------------------------------------------------------------------------------
--设置是否有属性变化需要飘字
function Modules.HeroModule:SetIsHaveChangePropertyNeedWindWords(IsNeedWindWords)
	self.__is_need_wind_words = IsNeedWindWords
	if not IsNeedWindWords then
		self.__before_change_hero_info = nil
		self.__reinforcement_name_list = nil
		self.__message_type = nil
		self.__lineup_type = nil
		self.__is_need_show = nil
		self.__before_strengthen_list = nil
		self.__equipment_uid = nil
		self.__cur_reinforcement_cheerlevel= nil
	end
end

--存储当前选择的阵位英雄
function Modules.HeroModule:GetIsHaveChangePropertyNeedWindWords()
	return self.__is_need_wind_words
end

--存储发生变化前的英雄信息(用于英雄飘字)
function Modules.HeroModule:SaveHeroPropertyBeforeChange(uid, pos, is_reinforcement)
	--发送事件
	GlobalEventSystem:Fire(Macros.Event.HeroModule.HERO_WIND_WORDS_PREPROCESS)

	self.__selected_lineup_index = pos or self.__selected_lineup_index	
	self.__before_change_hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", self.__selected_lineup_index)

	if not self.__before_change_hero_info then
		self.__before_change_hero_info = {}
		local heroInfo = self:GetHeroInfo(uid)
		local config_hero = CPPGameLib.GetConfig("Hero", heroInfo.info_id)
		if not config_hero then
			return
		end
		self.__before_change_hero_info.info_id = heroInfo.info_id
		self.__before_change_hero_info.uid = heroInfo.uid
		self.__before_change_hero_info.quality_level = heroInfo.quality_level
		self.__before_change_hero_info.beat_back_correct = config_hero.beat_back_correct or 0
		self.__before_change_hero_info.beat_back_defence_correct = config_hero.beat_back_defence_correct or 0
		local prop_key
		for prop_type=1,4 do			
			prop_key = Macros.Game.BattlePropKey[prop_type]
			self.__before_change_hero_info[prop_key] = 0
		end
		self.__is_need_show = true
	end	
	if is_reinforcement then
		local cur_cheer_info = self:GetHeroReinforcementCheerInfo()	
		self.__cur_reinforcement_cheerlevel = cur_cheer_info and cur_cheer_info.encourage_level or 0
	end
	if pos and uid then
		local heroInfo = self:GetHeroInfo(uid)		
		local config_hero = CPPGameLib.GetConfig("Hero", heroInfo.info_id)
		if not config_hero then
			return
		end
		self.__before_change_hero_info.info_id = heroInfo.info_id
		self.__before_change_hero_info.uid = uid
		self.__before_change_hero_info.pos = pos
		self.__before_change_hero_info.quality_level = heroInfo.quality_level
		self.__before_change_hero_info.beat_back_correct = config_hero.beat_back_correct or 0
		self.__before_change_hero_info.beat_back_defence_correct = config_hero.beat_back_defence_correct or 0
	end
end

--获取之前存储的各种导致属性变化前的英雄属性
function Modules.HeroModule:GetHeroPropertyBeforeChange()		
	return self.__before_change_hero_info,self.__reinforcement_name_list,self.__message_type,self.__lineup_type,self.__is_need_show,self.__before_strengthen_list,self.__equipment_uid,self.__cur_reinforcement_cheerlevel
end

--设置信息类型并转化为对应的缘分
function Modules.HeroModule:SetMessageTypeAndTranslateInfo(type,info)
	if not self.__before_change_hero_info then
		self.__is_need_wind_words = false
		return
	end
	if type == Macros.Game.WIND_WORDS_LINEUP_TYPE.EQUIP_MESSAGE then
		local equip_info = CallModuleFunc(ModuleType.EQUIP,"GetEquip",info.equipment_uid)
		self.__reinforcement_name_list = self:__GetCorrespondingReinforcementName(type,self.__before_change_hero_info.info_id,equip_info.info_id,self.__before_change_hero_info)		
		self.__message_type = Macros.Game.ObjType.EQUIP
		self.__equipment_uid = info.equipment_uid
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.TREASURE_MESSAGE then
		local treasure_info = CallModuleFunc(ModuleType.TREASURE,"GetTreasure",info.treasure_uid)
		self.__reinforcement_name_list = self:__GetCorrespondingReinforcementName(type,self.__before_change_hero_info.info_id,treasure_info.info_id,self.__before_change_hero_info)		
		self.__message_type = Macros.Game.ObjType.TREASURE
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.LINEUP_MESSGAE then
		local num
		num,self.__reinforcement_name_list = self:GetCanActiavteReinforcementsNum(self.__before_change_hero_info.uid,Macros.Game.HeroLineup.HERO_LINEUP,self.__before_change_hero_info.pos)
		self.__lineup_type = Macros.Game.WIND_WORDS_LINEUP_TYPE.LINEUP_MESSGAE
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.EQUIP_ONEKEY_ENHANCE_MESSAGE then
		self.__reinforcement_name_list = {}	
		self.__message_type = Macros.Game.ObjType.EQUIP
	elseif type == Macros.Game.WIND_WORDS_LINEUP_TYPE.DIVINATION_MESSAGE then
		--卡牌信息
		self.__reinforcement_name_list = {}	
		if not self.__is_activate_before then
			self.__reinforcement_name_list = self:__GetCorrespondingReinforcementName(type,self.__before_change_hero_info.info_id,info.formation_pos or 1)		
		end	
		self.__is_activate_before = false		
	else
		local num
		num,self.__reinforcement_name_list = self:GetCanActiavteReinforcementsNum(self.__before_change_hero_info.uid,Macros.Game.HeroLineup.REINFORCEMENT,self.__before_change_hero_info.pos)		
		self.__lineup_type = Macros.Game.WIND_WORDS_LINEUP_TYPE.REINFORCEMENT_MESSAGE		
	end
end