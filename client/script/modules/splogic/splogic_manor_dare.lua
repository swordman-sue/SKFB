
--[[
%% @module: 领地副本
%% @author: swordman sue
%% @created: 2016-10-15
--]]

Modules = Modules or {}

Modules.SpManorDareModule = Modules.SpManorDareModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpManorDareModule:__init()
end

function Modules.SpManorDareModule:__delete()
end

--初始化
function Modules.SpManorDareModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpManorDareModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--打开胜利界面
function Modules.SpManorDareModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.MANOR_DARE_RESULT_WIN )
end

--战斗结束，从结算界面返回对应界面
function Modules.SpManorDareModule:BattleEndReturnRelevantView()
	CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANOR)

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end
