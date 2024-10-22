
--[[
%% @module: 日常副本
%% @author: swordman sue
%% @created: 2016-10-31
--]]

Modules = Modules or {}

Modules.SpDailyDungeonModule = Modules.SpDailyDungeonModule or BaseClass(Modules.SpTowerModule)

function Modules.SpDailyDungeonModule:__init()
end

function Modules.SpDailyDungeonModule:__delete()
end

--初始化
function Modules.SpDailyDungeonModule:Initialize()
	Modules.SpTowerModule.Initialize(self)
end

--释放
function Modules.SpDailyDungeonModule:Dispose()
	Modules.SpTowerModule.Dispose(self)
end

--战斗退出处理
function Modules.SpDailyDungeonModule:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--打开胜利界面
function Modules.SpDailyDungeonModule:OpenBattleSuccessView()
	local win_condition = self:__GetWinCondition()
	local info = {}
	info.result = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetCurStatus")
	info.data = self:__GetBattleEndContext(true)
	info.type = win_condition[1]

    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.DAILY_DUNGEON_RESULT_WIN , info )
end

--打开失败界面
function Modules.SpDailyDungeonModule:OpenBattleFailedView()
	local win_condition = self:__GetWinCondition()
	if win_condition and win_condition[1] == Macros.Game.BattleWinCondition.NORMAL then
		Modules.SpMainDungeonModule.OpenBattleFailedView(self)
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_chapter_info = ConfigAdapter.Dungeon.GetChapterInfo(battle_info.chapter_index, self.__type)
	if config_chapter_info and config_chapter_info.is_lose_also_have_reward == Macros.Global.FALSE then		
	    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.BATTLE_RESULT_LOSE)
	else
		local win_condition = self:__GetWinCondition()
		local info = {}
		info.result = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetCurStatus")
		info.data = self:__GetBattleEndContext(true)
		info.type = win_condition[1]
	    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.SP_BATTLE_RESULT_LOSE, info)
	end
end

--战斗结束，从结算界面返回对应界面
function Modules.SpDailyDungeonModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.DUNGEON_DAILY, battle_info.chapter_index)	

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--请求战斗结束
function Modules.SpDailyDungeonModule:BattleEndReq(is_win, star, is_manual_exit)
	local context1 = self:__GetBattleEndContext()
	local context2 = is_win and self:__GetBattleEndContextEx() or nil
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit, context1, context2)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpDailyDungeonModule:__CreateMonsters()
	Modules.SpTowerModule.__CreateMonsters(self)

	self.__monster_total_hp = GlobalVOMgr:CalcTotalMonsterMaxHP()	
end

--是否检测结算条件
function Modules.SpDailyDungeonModule:__IsCheckWinCondition()
	return true
end

--检测结束条件
function Modules.SpDailyDungeonModule:__CheckEndCondition()
end

--获取战斗结算条件
function Modules.SpDailyDungeonModule:__GetWinCondition()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_chapter_info = ConfigAdapter.Dungeon.GetChapterInfo(battle_info.chapter_index, self.__type)
	if config_chapter_info then
		return config_chapter_info.win_condition
	end
end

--获取战斗结算上下文(服务端计算奖励使用)
function Modules.SpDailyDungeonModule:__GetBattleEndContext( ignore_calc_context )
	if not self:__IsCheckWinCondition() then
		return
	end

	local win_condition = self:__GetWinCondition()
	if not win_condition then
		return
	end

	local context
	local c_type = win_condition[1]
	local c_param = win_condition[2]
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local sort_func, compare_func

	--我方剩余血量不低于x%
	if c_type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
		local hp = GlobalVOMgr:CalcTotalHeroHP()
		context = math.floor( hp / self.__hero_total_hp * 100, 100) 

	--x秒内获胜
	elseif c_type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then		
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
		context = config_gamelevel.time - self.__remain_time
		sort_func = SortTools.ItemLowerSorter()
		compare_func = function(context, context_var) 
			return context <= context_var
		end

	--时间结束之后根据伤害算奖励
	elseif c_type == Macros.Game.BattleWinCondition.KILL_ALL_ENEMY then
		local hp = GlobalVOMgr:CalcTotalMonsterHP()
		context = math.min((self.__monster_total_hp - hp) / self.__monster_total_hp * 100, 100)		

	--常规战斗
	elseif c_type == Macros.Game.BattleWinCondition.NORMAL then
		context = ignore_calc_context and CPPGameLib.GetString("battle_tower_daily_rule_name") or nil
	end

	if not ignore_calc_context and c_type ~= Macros.Game.BattleWinCondition.NORMAL then
		context = ConfigAdapter.Dungeon.GetDailyDungeonRewardContext(
			battle_info.chapter_index, battle_info.gamelevel_index, context, sort_func, compare_func)
	end

	return context
end

--获取战斗结算上下文(服务端判断是否通关)
function Modules.SpDailyDungeonModule:__GetBattleEndContextEx()
	local is_pass = 1
	local win_condition = self:__GetWinCondition()
	if win_condition then
		local c_type = win_condition[1]
		local c_param = win_condition[2]

		--我方死亡人数不超过x
		if c_type == Macros.Game.BattleWinCondition.MY_DEAD_NUM_LESS_THAN then
			local now_hero_num = CPPGameLib.GetTableLength(GlobalVOMgr:GetHeroMap())
			if #self.__birth_hero_obj_ids - now_hero_num > c_param then
				is_pass = 0
			end

		--x秒内获胜
		elseif c_type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then
			local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
			local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
			local cost_time = config_gamelevel.time - self.__remain_time
			if cost_time > c_param then
				is_pass = 0
			end

		--我方剩余血量不低于x%
		elseif c_type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
			local hp = GlobalVOMgr:CalcTotalHeroHP()
			if hp / self.__hero_total_hp * 100 < c_param then
				is_pass = 0
			end
		end
	end
	return is_pass
end

--血量变化
function Modules.SpDailyDungeonModule:__ObjHPChange(obj_id, cur_hp, change_hp)
	Modules.SpMainDungeonModule.__ObjHPChange(self, obj_id, cur_hp, change_hp)
	self:__UpdateCondition()
end

--每次刷新
function Modules.SpDailyDungeonModule:__UpdateCondition()
	if not self:__IsCheckWinCondition() then
		return
	end

	if not self.__layer_condition then
		self.__layer_condition = UILayer.New()
		self.__layer_condition:SetAnchorPoint(0, 1)
		self.__layer_condition:SetBackGroundImageScale9Enabled(true)
		self.__layer_condition:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_difficulty_clear"))
		self.__layer_condition:SetContentSize(266 , 40)
		GlobalScene:AddToRenderGroup(self.__layer_condition, Scene.RenderQueue.GRQ_UI_DOWN_II)
		PosTool.LeftTop(self.__layer_condition, 0, -160)

		self.__layer_condition_bg = UILayer.New()
		self.__layer_condition_bg:SetAnchorPoint(0, 1)
		self.__layer_condition_bg:SetBackGroundImageScale9Enabled(true)
		self.__layer_condition_bg:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_difficulty_clear"))
		self.__layer_condition_bg:SetContentSize(266 , 105)
		GlobalScene:AddToRenderGroup(self.__layer_condition_bg, Scene.RenderQueue.GRQ_UI_DOWN_II)
		PosTool.LeftTop(self.__layer_condition_bg, 0, -202 )

		self.condition = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE , "img_battle_info", true) )
		self.condition:SetAnchorPoint(0 , 0.5 )
		self.__layer_condition:AddChild(self.condition)
		PosTool.LeftCenter( self.condition , 8 )

		self.tips1 = Util:RichText( " " , Macros.TypefaceSize.normal , self.__layer_condition_bg:GetNode():GetContentWidth() - 16 , nil , Macros.Color.white_hex )
		self.tips1:SetAnchorPoint( 0 , 1 )
		self.__layer_condition_bg:AddChild( self.tips1 )
		PosTool.LeftTop(self.tips1 , 8 , -5 )

		self.tips2 = Util:RichText( " " , Macros.TypefaceSize.normal , self.__layer_condition_bg:GetNode():GetContentWidth() - 16 , nil , Macros.Color.white_hex )
		self.tips2:SetAnchorPoint( 0 , 0.5 )
		self.__layer_condition_bg:AddChild( self.tips2 )
		PosTool.LeftCenter(self.tips2 , 8 )

		self.tips3 = Util:RichText( " " , Macros.TypefaceSize.normal , self.__layer_condition_bg:GetNode():GetContentWidth() - 16 , nil , Macros.Color.white_hex )
		self.tips3:SetAnchorPoint( 0 , 0 )
		self.tips3:SetPosition( 8 , 5 )
		self.__layer_condition_bg:AddChild( self.tips3 )

		self.__monster_total_hp = GlobalVOMgr:CalcTotalMonsterMaxHP()

	end

	local win_condition = self:__GetWinCondition()
	if not win_condition then
		return
	end

	local context1 , context2 , context3
	local c_type = win_condition[1]
	local c_param = win_condition[2]
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local sort_func, compare_func

	--我方剩余血量不低于x%
	if c_type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
		local hp = GlobalVOMgr:CalcTotalHeroHP()
		context1 = CPPGameLib.GetString("battle_tower_daily_HP_name" , math.floor( hp / self.__hero_total_hp * 100, 100) )
		context2 = CPPGameLib.GetString("battle_tower_daily_HP_tips")
		context3 = CPPGameLib.GetString("battle_tower_daily_HP_condition" , c_param or 0 )

	--x秒内获胜
	elseif c_type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then	
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
		context1 = CPPGameLib.GetString("battle_tower_daily_time_condition" , c_param or 0 )
		context2 = " "
		context3 = " "

	--时间结束之后根据伤害算奖励
	elseif c_type == Macros.Game.BattleWinCondition.KILL_ALL_ENEMY then
		local hp = GlobalVOMgr:CalcTotalMonsterHP()
		context1 = CPPGameLib.GetString("battle_tower_daily_harm_name" , math.floor( (self.__monster_total_hp - hp) / self.__monster_total_hp * 100, 100) )
		context2 = CPPGameLib.GetString("battle_tower_daily_harm_tips")
		context3 = CPPGameLib.GetString("battle_tower_daily_harm_condition")

	--常规
	elseif c_type == Macros.Game.BattleWinCondition.NORMAL then
		context1 = CPPGameLib.GetString("battle_tower_daily_rule_name")
		context2 = " "
		context3 = " "
	end
	
	self.tips1:SetText(context1)
	self.tips2:SetText(context2)
	self.tips3:SetText(context3)
end

function Modules.SpDailyDungeonModule:__ClearCondition()
	if self.__layer_condition then
		self.__layer_condition:DeleteMe()
		self.__layer_condition = nil
	end
	if self.__layer_condition_bg then
		self.__layer_condition_bg:DeleteMe()
		self.__layer_condition_bg = nil
	end
end

--处理结束(击杀所有怪物)
function Modules.SpDailyDungeonModule:__HandleEndForAllMonsterDead()
	Modules.SpMainDungeonModule.__HandleEndForAllMonsterDead(self)
end
