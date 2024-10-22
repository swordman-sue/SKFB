
--[[
%% @module: 夺宝副本
%% @author: swordman sue
%% @created: 2016-10-15
--]]

Modules = Modules or {}

Modules.SpPlunderedTreasuresModule = Modules.SpPlunderedTreasuresModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpPlunderedTreasuresModule:__init()

end

function Modules.SpPlunderedTreasuresModule:__delete()
end

--初始化
function Modules.SpPlunderedTreasuresModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpPlunderedTreasuresModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--打开胜利界面
function Modules.SpPlunderedTreasuresModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.PLUNDERED_TREASURES_RESULT_WIN)
end

--打开失败界面
function Modules.SpPlunderedTreasuresModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.PLUNDERED_TREASURES_RESULT_LOSE)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpPlunderedTreasuresModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	local function open_plundered_treasures(is_getpiece)
		local has_fragment = battle_info.end_params_list[1]
		if has_fragment == Macros.Global.FALSE then
			has_fragment = is_getpiece and Macros.Global.TRUE or Macros.Global.FALSE
		end
		if has_fragment == Macros.Global.TRUE then
			--有碎片
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.PLUNDEREDTREASURES, BaseViewSubType.PlunderedTreasures.MAIN_VIEW, true)		
		else
			--无碎片
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.PLUNDEREDTREASURES, BaseViewSubType.PlunderedTreasures.CHOOSE_ENEMY)		
		end
		--派发结束事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW)		
	end 

	if self.__splogic_status == Modules.SplogicStatus.WIN then
		LogicGUI.TemplChooseCard.New(self.__type, function(is_getpiece)
			open_plundered_treasures(is_getpiece)
		end)
	else
		open_plundered_treasures()
	end

	Modules.SplogicModule.BattleEndReturnRelevantView(self, false)	
end
