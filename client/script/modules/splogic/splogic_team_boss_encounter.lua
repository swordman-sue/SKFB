
--[[
%% @module: 组队BOSS怪遭遇场景
%% @author: swordman sue
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.SpTeamBossEncounterModule = Modules.SpTeamBossEncounterModule or BaseClass(Modules.SpMultiDungeonModule)

function Modules.SpTeamBossEncounterModule:__init()
    self.__monster_notice_toggle = true
end

function Modules.SpTeamBossEncounterModule:__delete()
end

--初始化
function Modules.SpTeamBossEncounterModule:Initialize()
	Modules.SpMultiDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamBossEncounterModule:Dispose()
	Modules.SpMultiDungeonModule.Dispose(self)
end

--加载资源
function Modules.SpTeamBossEncounterModule:LoadRes()
    --加载贴图集
    local texture_list = 
    {
        Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.TEAM_GATHER_AND_BATTLE),
    }
    Modules.SplogicModulesMgr.LoadRes(self, texture_list, nil, function()
        self.__is_res_loaded = true
    end)
end

function Modules.SpTeamBossEncounterModule:Update(now_time, elapse_time)
	Modules.SpMultiDungeonModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpTeamBossEncounterModule:GamePlayingStartHandle()   
    Modules.SpMultiDungeonModule.GamePlayingStartHandle(self)

    --打开BOSS界面
    CallModuleFunc(ModuleType.TEAM, "OpenView", BaseViewType.TEAM_BOSS_BATTLE)
    
    CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
end

function Modules.SpTeamBossEncounterModule:GamePlayingStopHandle()    
    Modules.SpMultiDungeonModule.GamePlayingStopHandle(self)
end

--注册事件
function Modules.SpTeamBossEncounterModule:RegistAllEvents()
    Modules.SpMultiDungeonModule.RegistAllEvents(self)

    --组队副本BOSS奖励通知
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_RESULT, function(_)
        CallModuleFunc(ModuleType.TEAM, "ShowBossRewardView")
    end)    

    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    end)        
end

--处理进入切场景状态
function Modules.SpTeamBossEncounterModule:HandleEnterSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleEnterSceneChangeState(self, callback)
end

--处理离开切场景状态
function Modules.SpTeamBossEncounterModule:HandleQuitSceneChangeState(callback)
    Modules.SpMainDungeonModule.HandleQuitSceneChangeState(self, callback)
end

--处理怪物数据通知
function Modules.SpTeamBossEncounterModule:HandleMonsterDataNotice(monster_info)
    Modules.SpMultiDungeonModule.HandleMonsterDataNotice(self, monster_info)

    local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_MONSTER, monster_info.uid)
    if obj then
        local obj_vo = obj:GetVO()
        self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_HP_CHANGE, obj_vo.hp, obj_vo.max_hp)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--执行打怪处理
function Modules.SpTeamBossEncounterModule:__AttackMonster(vo)
    CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.TEAM_BOSS_BATTLE, vo.uid, vo.map_id)                  
end

function Modules.SpTeamBossEncounterModule:__CreateMonster(monster_info)
    local monster_vo,monster
    monster_info.is_boss = true    
    monster_vo,monster = Modules.SpMultiDungeonModule.__CreateMonster(self, monster_info)
    monster:SetDir(MathTool.HORZ_LEFT)
    --BOSS战开始事件
    local data = {}
    data.start_battle_time = monster_info.start_battle_time
    data.vo = monster_vo
    self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_START, data)

    return monster_vo
end

