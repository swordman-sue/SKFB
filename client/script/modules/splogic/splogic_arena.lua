
--[[
%% @module: 竞技场
%% @author: swordman sue
%% @created: 2016-10-20
--]]

Modules = Modules or {}

Modules.SpArenaModule = Modules.SpArenaModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpArenaModule:__init()
end

function Modules.SpArenaModule:__delete()
end

--初始化
function Modules.SpArenaModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpArenaModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--战斗退出处理
function Modules.SpArenaModule:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--战斗成功处理
function Modules.SpArenaModule:BattleSuccessHandle(callback)
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Stop", false)
		CallModuleFunc(ModuleType.Pack, "ChooseCardReq", self.__type)	
		self:BindGlobalEvent(Macros.Event.PackModule.GET_CHOOSECARD_DATA, function(_, data)		
			local card_data
	        if data.create_item_id ~= 0 then
	        	card_data = card_data or {}
	        	card_data.card_id = data.create_item_id
	        	card_data.card_num = data.create_item_num
	        else
	        	local config_money = CPPGameLib.GetConfig("HuoBi", data.currency_type)
	        	if config_money then
		        	card_data = card_data or {}
					card_data.card_id = config_money.big_icon
		        	card_data.card_num = data.currency_value
		        end
	        end  
	        self:__StopQuickBattle(card_data)
		end)							
		return
	end	

	Modules.SpMainDungeonModule.BattleSuccessHandle(self, callback)
end

--打开胜利界面
function Modules.SpArenaModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.ARENA_RESULT_WIN )
end

--战斗失败处理
function Modules.SpArenaModule:BattleFailedHandle(callback)
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Stop", false)
		self:__StopQuickBattle()
		return
	end	

	Modules.SpMainDungeonModule.BattleFailedHandle(self, callback)
end

--打开失败界面
function Modules.SpArenaModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.ARENA_RESULT_LOSE)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpArenaModule:BattleEndReturnRelevantView()
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		Modules.SplogicModule.BattleEndReturnRelevantView(self)	
		return
	end

	local is_win = self.__splogic_status == Modules.SplogicStatus.WIN
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	local function open_arena()
		CallModuleFunc(ModuleType.ARENA, "OpenView", BaseViewType.ARENA, is_win)		

		--派发结束事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW)				
	end 

	if is_win then
		LogicGUI.TemplChooseCard.New(self.__type, function()
			open_arena()
		end)
	else
		open_arena()
	end

	Modules.SplogicModule.BattleEndReturnRelevantView(self, false)	
end

--是否自动战斗
function Modules.SpArenaModule:IsAutoFight()
	return true
end

--进入GamePlaying状态
function Modules.SpArenaModule:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)

	self.__auto_fight_record = CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", true)
end

--离开GamePlaying状态
function Modules.SpArenaModule:GamePlayingStopHandle()
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--处理快速战斗逻辑
function Modules.SpArenaModule:HandleQuickBattle()
	--获取敌人战力	    	
	local target_battle_value = CallModuleFunc(ModuleType.ARENA, "GetEnemyBattleValue")
	if not target_battle_value then
		self.__is_all_monster_dead_by_formula = true
    	CallModuleFunc(ModuleType.BATTLE, "OneKeyWin")
		return
	end

	--获取当前战力
	local battle_values = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.BATTLE_VALUE)
	--比率
	local rate = target_battle_value / battle_values
	--找比率
	local cur_rate_info
	for _, rank_info in pairs(CPPGameLib.GetConfig("ArenaFightFiveTimes", nil, nil , true )) do
		local limit_max = rank_info.limit_max == -1 and MathTool.MAX_NUM or rank_info.limit_max
		if rate >= MathTool.TransProbCoeff2Decimal(rank_info.limit_min) and rate < MathTool.TransProbCoeff2Decimal(limit_max) then
			cur_rate_info = rank_info
			break
		end
	end	
	if not cur_rate_info then
		local cur_table = CPPGameLib.GetConfig("ArenaFightFiveTimes", nil, nil , true )
		cur_rate_info = CPPGameLib.GetConfig("ArenaFightFiveTimes",table.maxn(cur_table))
	end

	--胜利
	local rank_num = MathTool.GetRandom(1, 100)
	if rank_num <= MathTool.TransProbCoeffPercentage(cur_rate_info.win_rate) then
		self.__is_all_monster_dead_by_formula = true
    	CallModuleFunc(ModuleType.BATTLE, "OneKeyWin")

	--失败
	else
		self.__is_all_hero_dead_by_formula = true
    	CallModuleFunc(ModuleType.BATTLE, "OneKeyLose")
	end
end

--是否所有英雄阵亡
function Modules.SpArenaModule:IsAllHeroDead()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if (battle_info and battle_info.in_skip) or CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		return self.__is_all_hero_dead_by_formula
	end

	if GlobalVOMgr:CalcTotalHeroHP() <= 0 then
		return true
	end
end

--是否所有怪物阵亡
function Modules.SpArenaModule:IsAllMonsterDead()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if (battle_info and battle_info.in_skip) or CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		return self.__is_all_monster_dead_by_formula
	end

	if GlobalVOMgr:CalcTotalMonsterHP() <= 0 then
		return true
	end
end    

--快速挑战结束
function Modules.SpArenaModule:__StopQuickBattle(card_data)
	local is_win = self.__splogic_status == Modules.SplogicStatus.WIN
	local reward_map
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_arena_award = CPPGameLib.GetConfig("ArenaAward", battle_info.pre_role_info.level)
	if config_arena_award then
		reward_map = {}
		reward_map[Macros.Game.RolePropType.EXP] = config_arena_award.exp

		if is_win then	
			reward_map[Macros.Game.RolePropType.GOLD] = config_arena_award.win_gold
			reward_map[Macros.Game.RolePropType.ARENA_PRESTIGE] = config_arena_award.win_arena_prestige
		else
			reward_map[Macros.Game.RolePropType.GOLD] = config_arena_award.fail_gold
			reward_map[Macros.Game.RolePropType.ARENA_PRESTIGE] = config_arena_award.fail_arena_prestige
		end
	end

	self:FireNextFrame(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_RESP, is_win, reward_map, card_data)
end

--计算战斗结算星数
function Modules.SpArenaModule:__CalcResultStar()
	return 0
end

