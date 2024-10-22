
--[[
%% @module: 天梯赛
%% @author: swordman sue
%% @created: 2017年10月31日
--]]

Modules = Modules or {}

Modules.SpLadderModule = Modules.SpLadderModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpLadderModule:__init()
end

function Modules.SpLadderModule:__delete()
end

--初始化
function Modules.SpLadderModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpLadderModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--战斗退出处理
function Modules.SpLadderModule:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--打开胜利界面
function Modules.SpLadderModule:OpenBattleSuccessView()
    local level = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if config_level and config_level.is_the_last_stage == Macros.Global.TRUE then
        --王者段位
        CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.LADDER_SENIOR_RESULT)
    else
        --低段位
        CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.LADDER_LOWER_RESULT)
    end
end

--打开失败界面
function Modules.SpLadderModule:OpenBattleFailedView()
    local level = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if config_level and config_level.is_the_last_stage == Macros.Global.TRUE then
        --王者段位
        CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.LADDER_SENIOR_RESULT)
    else
        --低段位
        CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.LADDER_LOWER_RESULT)
    end
end

--战斗结束，从结算界面返回对应界面
function Modules.SpLadderModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    CallModuleFunc( ModuleType.LADDER , "SetWinningAward" , {is_first_win = battle_info.is_first_win, level = CallModuleFunc(ModuleType.LADDER, "GetMyRank")} )
	CallModuleFunc(ModuleType.LADDER, "OpenView", BaseViewType.LADDER)	

	Modules.SplogicModule.BattleEndReturnRelevantView(self)	
end

--进入GamePlaying状态
function Modules.SpLadderModule:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)
end

--离开GamePlaying状态
function Modules.SpLadderModule:GamePlayingStopHandle()
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)
end

--计算战斗结算星数
function Modules.SpLadderModule:__CalcResultStar()
	return 0
end

