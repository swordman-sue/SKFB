
--[[
%% @module: 英雄副本
%% @author: swordman sue
%% @created: 2016-10-28
--]]

Modules = Modules or {}

Modules.SpHeroDungeonModule = Modules.SpHeroDungeonModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpHeroDungeonModule:__init()
end

function Modules.SpHeroDungeonModule:__delete()
end

--初始化
function Modules.SpHeroDungeonModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpHeroDungeonModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--打开胜利界面
function Modules.SpHeroDungeonModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.HERO_DUNGEON_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpHeroDungeonModule:BattleEndReturnRelevantView(...)
	local params = {...}
	local delete_func = function ()
		Modules.SplogicModule.BattleEndReturnRelevantView(self)
	end
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", battle_info.chapter_index, Macros.Game.FBType.HERO_DUNGEON)
	local config_chapter = CPPGameLib.GetConfig("HeroFBChapter",battle_info.chapter_index)
	local is_pass
	if config_chapter and (config_chapter.extra_map_id == battle_info.gamelevel_id) then
		--英雄副本特殊关卡回到章节界面
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.DUNGEON, BaseViewSubType.DUNGEON.HERO)
	elseif chapter_info and chapter_info.is_history_passed_all and (battle_info.is_first_win ~= Macros.Global.FALSE) then
		is_pass = true
		--当前章节已通关
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.HERO_DUNGEON_GAMELEVEL,battle_info.chapter_index,true,true,battle_info.gamelevel_index,unpack(params),delete_func)	
	elseif chapter_info and (battle_info.is_first_win ~= Macros.Global.FALSE) then
		--首次通关该关卡
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.HERO_DUNGEON_GAMELEVEL,battle_info.chapter_index,false,true,battle_info.gamelevel_index,unpack(params),unpack(params) and delete_func or nil)
	else
		--当前章节未通关，返回关卡界面
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.HERO_DUNGEON_GAMELEVEL, battle_info.chapter_index,nil,nil,battle_info.gamelevel_index,unpack(params),unpack(params) and delete_func or nil)	
	end

	if not unpack(params) and not is_pass then
		Modules.SplogicModule.BattleEndReturnRelevantView(self)
	end
end

