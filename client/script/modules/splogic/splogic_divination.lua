
--[[
%% @module: 占卜战斗
%% @author: swordman sue
%% @created: 2017-11-29
--]]

Modules = Modules or {}

Modules.SpDivination = Modules.SpDivination or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpDivination:__init()
end

function Modules.SpDivination:__delete()
end

--初始化
function Modules.SpDivination:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpDivination:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--打开胜利界面
function Modules.SpDivination:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.DIVINATION_RESULT_WIN )
end

--战斗结束，从结算界面返回对应界面
function Modules.SpDivination:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.is_win == 1 then
		CallModuleFunc(ModuleType.DIVINATION, "SetIscombat" , true )
	end
	CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_SCRYER )

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end
