
--[[
%% @module: 组队普通战斗
%% @author: swordman sue
%% @created: 2017-7-6
--]]

Modules = Modules or {}

Modules.SpTeamNormalBattleModule = Modules.SpTeamNormalBattleModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpTeamNormalBattleModule:__init()
end

function Modules.SpTeamNormalBattleModule:__delete()
end

--初始化
function Modules.SpTeamNormalBattleModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamNormalBattleModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)

    self:__StopAttackBossTimer()
end

--进入GamePlaying状态
function Modules.SpTeamNormalBattleModule:GamePlayingStartHandle()   
    Modules.SpMainDungeonModule.GamePlayingStartHandle(self)
end

function Modules.SpTeamNormalBattleModule:GamePlayingStopHandle()	
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)
end

--打开胜利界面
function Modules.SpTeamNormalBattleModule:OpenBattleSuccessView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TEAM_NORMAL_BATTLE_RESULT_WIN )

    self:__StartAttackBossTimer()
end

--打开失败界面
function Modules.SpTeamNormalBattleModule:OpenBattleFailedView()
    if self.__is_team_break then
        CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    else
        --结算界面
    	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TEAM_NORMAL_BATTLE_RESULT_WIN )
    end

    self:__StartAttackBossTimer()
end

--战斗结束，从结算界面返回对应界面
function Modules.SpTeamNormalBattleModule:BattleEndReturnRelevantView()
    Modules.SplogicModule.BattleEndReturnRelevantView(self)

    self:__StopAttackBossTimer()    

    if self.__is_time_to_boss then
        --任务时间到了要去打BOSS
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
    else
    	--返回普通怪遭遇场景
        CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", nil, nil, true)
    end
end

--请求战斗结束
function Modules.SpTeamNormalBattleModule:BattleEndReq(is_win, star, is_manual_exit)
	local context = math.ceil(GlobalVOMgr:CalcTotalHeroHP() / self.__hero_total_hp * 100)
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit, context)
end

--注册事件
function Modules.SpTeamNormalBattleModule:RegistAllEvents()
	Modules.SpMainDungeonModule.RegistAllEvents(self)

    --任务时间到了要去打BOSS
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_TASK_TIME_TO_BOSS, function(_)
        self.__is_time_to_boss = true
        self.__is_checked_end = true
        self:BattleEndReq(false)     
    end)    

    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        self.__is_team_break = true
        self.__is_checked_end = true
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")
    end)            
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpTeamNormalBattleModule:__StartBattle(for_next)
	Modules.SpMainDungeonModule.__StartBattle(self, for_next)

	if not for_next then
		self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()
	end
end

function Modules.SpTeamNormalBattleModule:__StartAttackBossTimer()
    if not self.__is_time_to_boss then
        return
    end

    if self.__attack_boss_timer then
        GlobalTimerQuest:CancelQuest(self.__attack_boss_timer)
    end

    self.__attack_boss_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
        self.__attack_boss_timer = nil

        --任务时间到了要去打BOSS
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")        
    end), 4)        
end

function Modules.SpTeamNormalBattleModule:__StopAttackBossTimer()
    if self.__attack_boss_timer then
        GlobalTimerQuest:CancelQuest(self.__attack_boss_timer)
        self.__attack_boss_timer = nil
    end
end

--处理场景离开通知
function Modules.SpTeamNormalBattleModule:HandleLeaveSceneNotice(protocol_data)
    if CallModuleFunc(ModuleType.TEAM, "GetTeamDungeonTeamLeaderRoleId") == protocol_data.role_id then
        --任务完成，队长回到世界地图，队员无条件返回世界地图
        CallModuleFunc(ModuleType.TEAM, "LeaveReturnWorldMap")
    end
end
