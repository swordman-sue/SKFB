
--[[
%% @module: 新手副本
%% @author: swordman sue
%% @created: 2016-12-28
--]]

Modules = Modules or {}

Modules.SpLeadDungeonModule = Modules.SpLeadDungeonModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpLeadDungeonModule:__init()
end

function Modules.SpLeadDungeonModule:__delete()
end

--初始化
function Modules.SpLeadDungeonModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpLeadDungeonModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--注册事件
function Modules.SpLeadDungeonModule:RegistAllEvents()
	Modules.SpMainDungeonModule.RegistAllEvents(self)

	self:BindGlobalEvent(Macros.Event.View.VIEW_OPEN, function(_, view_id, view_type)
		if view_type == BaseViewType.DUNGEON or view_type == BaseViewType.DUNGEON_GAMELEVEL then
			ActionManager.GetInstance():StopMapBlackingWithLock()
		end		
	end)
end

function Modules.SpLeadDungeonModule:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)
end

function Modules.SpLeadDungeonModule:BattleEndHandle(ret)
	Modules.SpMainDungeonModule.BattleEndHandle(self, ret)

    self.__splogic_status = Modules.SplogicStatus.WIN
end

--战斗成功处理
function Modules.SpLeadDungeonModule:BattleSuccessHandle(callback)
	self:__BattleEndHandle()
end

--战斗失败处理
function Modules.SpLeadDungeonModule:BattleFailedHandle(callback)
	self:__BattleEndHandle()
end

--请求战斗结束
function Modules.SpLeadDungeonModule:BattleEndReq(is_win, star, is_manual_exit)
    --进入未通关的新手副本关联副本时，将type和gamelevel_id重定向为新手副本
	--所以此处需要将type修正为Macros.Game.FBType.MAIN_DUNGEON
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info then
		battle_info.type = Macros.Game.FBType.MAIN_DUNGEON
		battle_info.gamelevel_id = ConfigAdapter.Dungeon.GetLeadAssociatedGameLevelID()
	end

	Modules.SpMainDungeonModule.BattleEndReq(self, true, 3, is_manual_exit)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取战斗英雄列表
function Modules.SpLeadDungeonModule:__GetBattleHeroList()
	local list = ConfigAdapter.Dungeon.GetLeadBattleHeroList()
	Modules.BattleFormula:SortBattleHeroList(list)
	return list
end

function Modules.SpLeadDungeonModule:__BattleEndHandle()
	ActionManager.GetInstance():StartMapBlackingWithLock(nil, 0xff, 1, function()
		CallModuleFunc(ModuleType.BATTLE, "BattleEndReturnRelevantView")
	end, Scene.RenderQueue.GRQ_UI_UP)
end

--战斗准备特效
function Modules.SpLeadDungeonModule:__PlayStartBattleEffect()
end

