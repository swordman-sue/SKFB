
--[[
%% @module: 资源站战斗
%% @author: ludexian
%% @created: 2017-11-29
--]]

Modules = Modules or {}

Modules.SpResourceWars = Modules.SpResourceWars or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpResourceWars:__init()
end

function Modules.SpResourceWars:__delete()
end

--初始化
function Modules.SpResourceWars:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpResourceWars:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--战斗退出处理
function Modules.SpResourceWars:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--战斗成功处理
function Modules.SpResourceWars:BattleSuccessHandle(callback)	
	Modules.SpMainDungeonModule.BattleSuccessHandle(self, callback)
end

--打开胜利界面
function Modules.SpResourceWars:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.RESOURCE_WAR_RESULT_WIN )
end

--战斗失败处理
function Modules.SpResourceWars:BattleFailedHandle(callback)
	Modules.SpMainDungeonModule.BattleFailedHandle(self, callback)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpResourceWars:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	-- if battle_info.is_win == 1 then
	-- 	CallModuleFunc(ModuleType.DIVINATION, "SetIscombat" , true )
	-- end
	CallModuleFunc(ModuleType.RESOURCE_WAR, "OpenView", BaseViewType.RESOURCE_WAR , battle_info.param1 )

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--是否自动战斗
function Modules.SpResourceWars:IsAutoFight()
	return CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")
end

--进入GamePlaying状态
function Modules.SpResourceWars:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)

	self.__auto_fight_record = CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--离开GamePlaying状态
function Modules.SpResourceWars:GamePlayingStopHandle()
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--是否所有英雄阵亡
function Modules.SpResourceWars:IsAllHeroDead()
	return Modules.SpMainDungeonModule.IsAllHeroDead(self)
end

--是否所有怪物阵亡
function Modules.SpResourceWars:IsAllMonsterDead()
	return Modules.SpMainDungeonModule.IsAllMonsterDead(self)
end    

--计算战斗结算星数
function Modules.SpResourceWars:__CalcResultStar()
	return 0
end

--检测慢镜
function Modules.SpResourceWars:__CheckSlowMirror()
	Modules.SpMainDungeonModule.__CheckSlowMirror(self)
end
