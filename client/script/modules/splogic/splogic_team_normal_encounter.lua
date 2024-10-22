
--[[
%% @module: 组队普通怪遭遇场景
%% @author: swordman sue
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.SpTeamNormalEncounterModule = Modules.SpTeamNormalEncounterModule or BaseClass(Modules.SpMultiDungeonModule)

function Modules.SpTeamNormalEncounterModule:__init()
    self.__monster_notice_toggle = true
end

function Modules.SpTeamNormalEncounterModule:__delete()
end

--初始化
function Modules.SpTeamNormalEncounterModule:Initialize()
	Modules.SpMultiDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamNormalEncounterModule:Dispose()
	Modules.SpMultiDungeonModule.Dispose(self)
end

--加载资源
function Modules.SpTeamNormalEncounterModule:LoadRes()
    --加载贴图集
    local texture_list = 
    {
        Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.TEAM_GATHER_AND_BATTLE),
    }
    Modules.SplogicModulesMgr.LoadRes(self, texture_list, nil, function()
        self.__is_res_loaded = true
    end)
end

function Modules.SpTeamNormalEncounterModule:Update(now_time, elapse_time)
	Modules.SpMultiDungeonModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpTeamNormalEncounterModule:GamePlayingStartHandle()   
    Modules.SpMultiDungeonModule.GamePlayingStartHandle(self)

    --打开采集、打怪界面
    CallModuleFunc(ModuleType.TEAM, "OpenView", BaseViewType.TEAM_GATHER_AND_BATTLE)

    --组队副本任务奖励弹窗
    CallModuleFunc(ModuleType.TEAM, "ShowTaskRewardView")

    CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")    
end

function Modules.SpTeamNormalEncounterModule:GamePlayingStopHandle()
    CallModuleFunc(ModuleType.TEAM, "CloseView")
    
    Modules.SpMultiDungeonModule.GamePlayingStopHandle(self)
end

function Modules.SpTeamNormalEncounterModule:RegistAllEvents()
    Modules.SpMultiDungeonModule.RegistAllEvents(self)

    --任务时间到了要去打BOSS
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_TASK_TIME_TO_BOSS, function(_)
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
    end)

    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    end)    
end

--处理进入切场景状态
function Modules.SpTeamNormalEncounterModule:HandleEnterSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleEnterSceneChangeState(self, callback)
end

--处理离开切场景状态
function Modules.SpTeamNormalEncounterModule:HandleQuitSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleQuitSceneChangeState(self, callback)
end

--处理组队副本任务奖励界面关闭
function Modules.SpTeamNormalEncounterModule:HandleTeamDungeonTaskRewardViewClosed()
    CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
end

--执行打怪处理
function Modules.SpTeamNormalEncounterModule:__AttackMonster(vo)
    CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.TEAM_NORMAL_BATTLE, vo.uid, vo.map_id)                  
end

--处理场景离开通知
function Modules.SpTeamNormalEncounterModule:HandleLeaveSceneNotice(protocol_data)
    Modules.SpMultiDungeonModule.HandleLeaveSceneNotice(self, protocol_data)

    if CallModuleFunc(ModuleType.TEAM, "GetTeamDungeonTeamLeaderRoleId") == protocol_data.role_id then
        --任务完成，队长回到世界地图，队员无条件返回世界地图
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
    end
end

