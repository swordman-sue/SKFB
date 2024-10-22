
--[[
%% @module: 组队采集场景
%% @author: swordman sue
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.SpTeamGatherModule = Modules.SpTeamGatherModule or BaseClass(Modules.SpMultiDungeonModule)

function Modules.SpTeamGatherModule:__init()
    self.__item_notice_toggle = true
end

function Modules.SpTeamGatherModule:__delete()
end

--初始化
function Modules.SpTeamGatherModule:Initialize()
	Modules.SpMultiDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamGatherModule:Dispose()
	Modules.SpMultiDungeonModule.Dispose(self)
end

--加载资源
function Modules.SpTeamGatherModule:LoadRes()
    --加载贴图集
    local texture_list = 
    {
        Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.TEAM_GATHER_AND_BATTLE),
    }
    Modules.SplogicModulesMgr.LoadRes(self, texture_list, nil, function()
        self.__is_res_loaded = true
    end)
end

function Modules.SpTeamGatherModule:Update(now_time, elapse_time)
	Modules.SpMultiDungeonModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpTeamGatherModule:GamePlayingStartHandle()   
    Modules.SpMultiDungeonModule.GamePlayingStartHandle(self)

    --打开采集、打怪界面
	CallModuleFunc(ModuleType.TEAM, "OpenView", BaseViewType.TEAM_GATHER_AND_BATTLE)
end

function Modules.SpTeamGatherModule:GamePlayingStopHandle()
	CallModuleFunc(ModuleType.TEAM, "CloseView")
	
	Modules.SpMultiDungeonModule.GamePlayingStopHandle(self)
end

function Modules.SpTeamGatherModule:RegistAllEvents()
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
function Modules.SpTeamGatherModule:HandleEnterSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleEnterSceneChangeState(self, callback)
end

--处理离开切场景状态
function Modules.SpTeamGatherModule:HandleQuitSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleQuitSceneChangeState(self, callback)
end

--处理组队副本任务奖励界面关闭
function Modules.SpTeamGatherModule:HandleTeamDungeonTaskRewardViewClosed()
    CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
end

--处理场景离开通知
function Modules.SpTeamGatherModule:HandleLeaveSceneNotice(protocol_data)
    Modules.SpMultiDungeonModule.HandleLeaveSceneNotice(self, protocol_data)

    if CallModuleFunc(ModuleType.TEAM, "GetTeamDungeonTeamLeaderRoleId") == protocol_data.role_id then
        --任务完成，队长回到世界地图，队员无条件返回世界地图
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
    end
end
