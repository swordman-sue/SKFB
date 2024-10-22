
--[[
%% @module: 积分赛
%% @author: ldx
%% @created: 2018-07-11
--]]

Modules = Modules or {}

Modules.SpIntegralRaceModule = Modules.SpIntegralRaceModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpIntegralRaceModule:__init()
end

function Modules.SpIntegralRaceModule:__delete()
end

--初始化
function Modules.SpIntegralRaceModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpIntegralRaceModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--战斗退出处理
function Modules.SpIntegralRaceModule:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--战斗成功处理
function Modules.SpIntegralRaceModule:BattleSuccessHandle(callback)	
	Modules.SpMainDungeonModule.BattleSuccessHandle(self, callback)
end

--打开胜利界面
function Modules.SpIntegralRaceModule:OpenBattleSuccessView()
	CallModuleFunc(ModuleType.INTEGRAL_RACE, "SetISWin" , true )
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.INTEGRAL_RACE_RESULT_WIN)
end

--战斗失败处理
function Modules.SpIntegralRaceModule:BattleFailedHandle(callback)
	Modules.SpMainDungeonModule.BattleFailedHandle(self, callback)
end

--打开失败界面
function Modules.SpIntegralRaceModule:OpenBattleFailedView()
	CallModuleFunc(ModuleType.INTEGRAL_RACE, "SetISWin" , false )
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.INTEGRAL_RACE_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpIntegralRaceModule:BattleEndReturnRelevantView()
	local is_win = self.__splogic_status == Modules.SplogicStatus.WIN
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	local function open_integralrace()
		CallModuleFunc(ModuleType.INTEGRAL_RACE, "OpenView", BaseViewType.INTEGRAL_RACE)		

		--派发结束事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW)				
	end 
	open_integralrace()
	Modules.SplogicModule.BattleEndReturnRelevantView(self, false)	
end

--是否自动战斗
function Modules.SpIntegralRaceModule:IsAutoFight()
	return CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")
end

--进入GamePlaying状态
function Modules.SpIntegralRaceModule:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)

	self.__auto_fight_record = CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--离开GamePlaying状态
function Modules.SpIntegralRaceModule:GamePlayingStopHandle()
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--是否所有英雄阵亡
function Modules.SpIntegralRaceModule:IsAllHeroDead()
	return Modules.SpMainDungeonModule.IsAllHeroDead(self)
end

--是否所有怪物阵亡
function Modules.SpIntegralRaceModule:IsAllMonsterDead()
	return Modules.SpMainDungeonModule.IsAllMonsterDead(self)
end    

--计算战斗结算星数
function Modules.SpIntegralRaceModule:__CalcResultStar()
	return 0
end

--检测慢镜
function Modules.SpIntegralRaceModule:__CheckSlowMirror()
	Modules.SpMainDungeonModule.__CheckSlowMirror(self)
end
