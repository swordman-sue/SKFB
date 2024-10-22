--[[
%% @module: 场景资源
%% @author: swordman sue
%% @created: 2016-07-11
%% @description: 负责创建和管理场景相关资源
--]]

Scene = Scene or {}

Scene.Scene = Scene.Scene or BaseClass(EventAndTimerWrapper)

function Scene.Scene:__NewRes()
end

function Scene.Scene:__DelRes()
    self:__UnloadSceneRes()
end

function Scene.Scene:__LoadSceneRes()
    local is_need_load_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsNeedLoadMap")
    if not is_need_load_map then
        self.__map:SetLoaded(true)
    else       
        local map_id = GlobalVOMgr:GetMapID()
        self.__map:Load(map_id)
    end
end

function Scene.Scene:__UnloadSceneRes()
    self.__map:Unload()
end

