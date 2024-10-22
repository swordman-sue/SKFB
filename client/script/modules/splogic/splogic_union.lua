
--[[
%% @module: 公会副本
%% @author: yjg
%% @created: 2017年5月26日10:32:21
--]]

Modules = Modules or {}

Modules.SpUnionModule = Modules.SpUnionModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpUnionModule:__init()
end

function Modules.SpUnionModule:__delete()
end

--打开胜利界面
function Modules.SpUnionModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.UNION_RESULT_WIN)
end

--打开失败界面
function Modules.SpUnionModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.UNION_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpUnionModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	CPPGameLib.PrintTable("xy", battle_info.is_me_kill ,"--战斗结束，从结算界面返回对应界面")


	local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
	local info = {}
	info.level = battle_info.param1
	info.section = section
	info.is_me_kill = battle_info.is_me_kill

	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_TRANSCRIPT_TWO , info )
	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end
