
--[[
%% @module: 世界boss
%% @author: swordman sue
%% @created: 2016-10-20
--]]

Modules = Modules or {}

Modules.SpWorldBossModule = Modules.SpWorldBossModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpWorldBossModule:__init()
end

function Modules.SpWorldBossModule:__delete()
end

--初始化
function Modules.SpWorldBossModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpWorldBossModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

function Modules.SpWorldBossModule:Update(now_time, elapse_time)
    Modules.SpMainDungeonModule.Update(self, now_time, elapse_time)

    self:__CheckACC()    
end

--打开胜利界面
function Modules.SpWorldBossModule:OpenBattleSuccessView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.WORLD_BOSS_RESULT_WIN )
end

--打开失败界面
function Modules.SpWorldBossModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.WORLD_BOSS_RESULT_WIN )
end

--战斗结束，从结算界面返回对应界面
function Modules.SpWorldBossModule:BattleEndReturnRelevantView()
	CallModuleFunc(ModuleType.WORLDBOSS, "OpenView", BaseViewType.WORLDBOSS)
	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

