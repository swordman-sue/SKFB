
--[[
%% @module: 组队世界地图
%% @author: swordman sue
%% @created: 2017-09-29
--]]

Modules = Modules or {}

Modules.SpTeamWorldMapModule = Modules.SpTeamWorldMapModule or BaseClass(Modules.SplogicModule)

function Modules.SpTeamWorldMapModule:__init()
end

function Modules.SpTeamWorldMapModule:__delete()
end

--初始化
function Modules.SpTeamWorldMapModule:Initialize()
	Modules.SplogicModule.Initialize(self)
end

--释放
function Modules.SpTeamWorldMapModule:Dispose()
	Modules.SplogicModule.Dispose(self)
end

function Modules.SpTeamWorldMapModule:Update(now_time, elapse_time)
	Modules.SplogicModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpTeamWorldMapModule:GamePlayingStartHandle()
    Modules.SplogicModule.GamePlayingStartHandle(self)

    --打开组队世界地图
    CallModuleFunc(ModuleType.TEAM, "OpenView", BaseViewType.TEAM_WORLD_MAP)

    --检测是否显示队伍解散弹窗
    CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    
    --设置运行帧率
    -- CPPSceneManager:SetGameFrame(Macros.Global.FPS_MainCity)
end

function Modules.SpTeamWorldMapModule:GamePlayingStopHandle()
    Modules.SplogicModule.GamePlayingStopHandle(self)
end

--加载资源
function Modules.SpTeamWorldMapModule:LoadRes()
    self.__is_res_loaded = true
end

function Modules.SpTeamWorldMapModule:IsNeedLoadMap()
    return false
end

function Modules.SpTeamWorldMapModule:IsPauseWhenDeActive()
    return false
end
