
--[[
%% @module: 精英副本
%% @author: swordman sue
%% @created: 2016-10-28
--]]

Modules = Modules or {}

Modules.SpEliteDungeonModule = Modules.SpEliteDungeonModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpEliteDungeonModule:__init()
end

function Modules.SpEliteDungeonModule:__delete()
end

--初始化
function Modules.SpEliteDungeonModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpEliteDungeonModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--打开胜利界面
function Modules.SpEliteDungeonModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.ELITE_DUNGEON_RESULT_WIN )
end

--战斗结束，从结算界面返回对应界面
function Modules.SpEliteDungeonModule:BattleEndReturnRelevantView(...)	
	local params = {...}
	local delete_func = function ()
		Modules.SplogicModule.BattleEndReturnRelevantView(self)
	end
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", battle_info.chapter_index,Macros.Game.FBType.ELITE_DUNGEON)
	local is_pass
	if battle_info.gamelevel_index == Macros.Global.FALSE then
		--打开外域入侵界面
		CallModuleFunc(ModuleType.DUNGEON,"OpenView", BaseViewType.DUNGEON,BaseViewSubType.DUNGEON.ELITE,false,true)	
	elseif chapter_info and chapter_info.is_history_passed_all and (battle_info.is_first_win ~= Macros.Global.FALSE) then
		is_pass = true
		--当前章节已通关，播放通关特效
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.ELITE_DUNGEON_GAMELEVEL, battle_info.chapter_index or 1,unpack(params) and battle_info.gamelevel_index or nil,true,nil,nil,unpack(params),delete_func)
	else
		--当前章节未通关，返回关卡界面
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.ELITE_DUNGEON_GAMELEVEL, battle_info.chapter_index or 1,unpack(params) and battle_info.gamelevel_index or nil,nil,nil,nil,unpack(params),unpack(params) and delete_func or nil)
	end
	if not unpack(params) and not is_pass then
		Modules.SplogicModule.BattleEndReturnRelevantView(self)
	end
end

--计算战斗结算星数
function Modules.SpEliteDungeonModule:__CalcResultStar( index )
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local t1 = {}
	for k,v in pairs(CPPGameLib.GetConfig("ElitenFBGetStar",nil,nil,true)) do
		table.insert( t1 , { time = v.time , star = v.star } )
	end

	local star = 1
	for i,v in ipairs( t1 ) do
		if battle_info.cost_time <= v.time then
			star = v.star
			break
		end
	end
	return star
end
