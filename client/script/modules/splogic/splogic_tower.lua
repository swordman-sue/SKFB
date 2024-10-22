
--[[
%% @module: 爬塔副本
%% @author: swordman sue
%% @created: 2016-10-15
--]]

Modules = Modules or {}

Modules.SpTowerModule = Modules.SpTowerModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpTowerModule:__init()
end

function Modules.SpTowerModule:__delete()
end

--初始化
function Modules.SpTowerModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpTowerModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)

	self:__ClearCondition()
end

--更新剩余时间
function Modules.SpTowerModule:SetRemainTime(var)
	Modules.SpMainDungeonModule.SetRemainTime(self, var)

	self:__CheckEnd()
end

--战斗结束处理
function Modules.SpTowerModule:BattleEndHandle(var)
	Modules.SpMainDungeonModule.BattleEndHandle(self, var)
	
	self:__ClearCondition()

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.TOWER_SHOP)
end

--打开胜利界面
function Modules.SpTowerModule:OpenBattleSuccessView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	if battle_info.type == Macros.Game.FBType.TOWER_COMMON then

	    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TOWER_COMMON_RESULT_WIN )
	    
	elseif battle_info.type == Macros.Game.FBType.TOWER_ELITE then

		CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TOWER_ELITE_RESULT_WIN )
		
	end
end

--战斗结束，从结算界面返回对应界面
function Modules.SpTowerModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	local info = { 
			[1] = battle_info.param1,
			[2] = self.__splogic_status == Modules.SplogicStatus.WIN,
			[3] = battle_info,

	}

	if battle_info.type == Macros.Game.FBType.TOWER_ELITE then
		CallModuleFunc(ModuleType.TOWER,"SetShowAwardType" , nil )
	elseif self.__splogic_status ~= Modules.SplogicStatus.WIN then
		CallModuleFunc(ModuleType.TOWER,"SetShowAwardType" , true )
	end

	CallModuleFunc(ModuleType.TOWER,"SetbattleData",info)

	CallModuleFunc(ModuleType.TOWER, "OpenView", BaseViewType.TOWER )

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end	

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpTowerModule:__CreateHeros()
	Modules.SpMainDungeonModule.__CreateHeros(self)

	self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()
end

function Modules.SpTowerModule:__StartBattle()
	Modules.SpMainDungeonModule.__StartBattle(self)

	self:__UpdateCondition()
end

--血量变化
function Modules.SpTowerModule:__ObjHPChange(obj_id, cur_hp, change_hp)
	Modules.SpMainDungeonModule.__ObjHPChange(self, obj_id, cur_hp, change_hp)

	local obj = GlobalScene:GetObj(obj_id)
	if obj and obj:IsHero() then
		self:__UpdateCondition()
	end
end

--更新条件信息
function Modules.SpTowerModule:__UpdateCondition()
	if not self:__IsCheckWinCondition() then
		return
	end

	if not self.__layer_condition then
		self.__layer_condition = UILayer.New()
		self.__layer_condition:SetAnchorPoint(0, 1)
		self.__layer_condition:SetContentSize(160 , 66)
		self.__layer_condition:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_battle_damage"))
		GlobalScene:AddToRenderGroup(self.__layer_condition, Scene.RenderQueue.GRQ_UI_UP)
		PosTool.LeftTop(self.__layer_condition, 0, -160)

		self.__lab_condition = Util:RichText(nil, Macros.TypefaceSize.popup )
		self.__lab_condition:SetAnchorPoint(0, 0.5)
		self.__lab_condition:SetFontSpacing(6)
		self.__layer_condition:AddChild(self.__lab_condition)

		self.condition = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE , "condition", true) )
		self.condition:SetAnchorPoint(0.5,0)
		self.condition:SetPosition( self.__layer_condition:GetNode():GetContentWidth()/2 - 10 , self.__layer_condition:GetNode():GetContentHeight() + 2 )
		self.__layer_condition:AddChild(self.condition)
	end

	local str_condition = CPPGameLib.GetString("battle_tower_cond_tip4")

	local win_condition = self:__GetWinCondition()
	if win_condition then
		local c_type = win_condition[1]
		local c_param = win_condition[2]

		--我方剩余血量不低于x%
		if c_type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
			local hp = GlobalVOMgr:CalcTotalHeroHP()
			local hp_percentage = math.floor(hp / self.__hero_total_hp * 100)
			str_condition = CPPGameLib.GetString(
													"battle_tower_cond_tip"..c_type, c_param,
													GUI.RichText.GetColorWord(string.format("%d%%", hp_percentage), 
													hp_percentage >= c_param and 
													Macros.Color.white_hex or 
													Macros.Color.red_hex,
													hp_percentage >= c_param and 
													Macros.Color.white_stroke_hex or 
													Macros.Color.red_stroke_hex
													)
												)			
			
		--我方死亡人数不超过x
		elseif c_type == Macros.Game.BattleWinCondition.MY_DEAD_NUM_LESS_THAN then
			local now_hero_num = CPPGameLib.GetTableLength(GlobalVOMgr:GetHeroMap())
			local now_hero_dead_num = #self.__birth_hero_obj_ids - now_hero_num
			str_condition = CPPGameLib.GetString(
													"battle_tower_cond_tip"..c_type, c_param, 
													GUI.RichText.GetColorWord(	
																				now_hero_dead_num, 
																				now_hero_dead_num <= c_param and 
																				Macros.Color.white_hex or 
																				Macros.Color.red_hex,
																				
																				now_hero_dead_num >= c_param and 
																				Macros.Color.white_stroke_hex or 
																				Macros.Color.red_stroke_hex
																				)
																			)

		--x秒内获胜
		elseif c_type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then
			str_condition = CPPGameLib.GetString("battle_tower_cond_tip"..c_type, c_param)
		end	
	end	

	self.__lab_condition:SetText(str_condition)
	PosTool.LeftCenter(self.__lab_condition, 6)
end

function Modules.SpTowerModule:__ClearCondition()
	if self.__layer_condition then
		self.__layer_condition:DeleteMe()
		self.__layer_condition = nil
	end
end

--检测结束
function Modules.SpTowerModule:__CheckEnd()	
	if not self.__is_started then
		return
	end	

	if self.__is_checked_end then
		return
	end

	if self.__splogic_status ~= Modules.SplogicStatus.RUN then
		return
	end

	if self:__IsCheckWinCondition() then
		self:__CheckEndCondition()
	end
	
	Modules.SpMainDungeonModule.__CheckEnd(self)
end

--检测结束条件
function Modules.SpTowerModule:__CheckEndCondition()
	--普通爬塔胜利条件检测
	self:__UpdateCondition()

	local is_succ = true

	local win_condition = self:__GetWinCondition()
	if win_condition then
		local c_type = win_condition[1]
		local c_param = win_condition[2]

		--我方死亡人数不超过x
		if c_type == Macros.Game.BattleWinCondition.MY_DEAD_NUM_LESS_THAN then
			local now_hero_num = CPPGameLib.GetTableLength(GlobalVOMgr:GetHeroMap())
			if #self.__birth_hero_obj_ids - now_hero_num > c_param then
				is_succ = false
			end

		--x秒内获胜
		elseif c_type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then
			local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
			local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
			local cost_time = config_gamelevel.time - self.__remain_time
			if cost_time > c_param then
				is_succ = false
			end
		end
	end

	if not is_succ then
		self.__is_checked_end = true
		self:__StopSlowMirror()
		self:BattleEndReq(false, 0)
		return
	end
end

--处理结束(击杀所有怪物)
function Modules.SpTowerModule:__HandleEndForAllMonsterDead()
	--普通爬塔胜利条件检测
	if self:__IsCheckWinCondition() then
		local win_condition = self:__GetWinCondition()
		if win_condition then
			local c_type = win_condition[1]
			local c_param = win_condition[2]

			--我方剩余血量不低于x%
			if c_type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
				local hp = GlobalVOMgr:CalcTotalHeroHP()
				if hp / self.__hero_total_hp * 100 < c_param then
					self:BattleEndReq(false, 0)
					return
				end
			end
		end
	end	

	Modules.SpMainDungeonModule.__HandleEndForAllMonsterDead(self)
end

--是否检测结算条件
function Modules.SpTowerModule:__IsCheckWinCondition()
	return self.__type == Macros.Game.FBType.TOWER_COMMON
end

--获取战斗结算条件
function Modules.SpTowerModule:__GetWinCondition()
	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetBattleMapLevel")
	local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level)
	if config_tower_game_level then
		return config_tower_game_level.win_condition
	end
end
