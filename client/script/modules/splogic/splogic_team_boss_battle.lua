
--[[
%% @module: 怪物攻城
%% @author: swordman sue
%% @created: 2017-7-6
--]]

Modules = Modules or {}

Modules.SpTeamBossBattleModule = Modules.SpTeamBossBattleModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpTeamBossBattleModule:__init()
end

function Modules.SpTeamBossBattleModule:__delete()
end

--初始化
function Modules.SpTeamBossBattleModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamBossBattleModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--注册事件
function Modules.SpTeamBossBattleModule:RegistAllEvents()
    Modules.SpMainDungeonModule.RegistAllEvents(self)

    --组队副本BOSS奖励通知
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_RESULT, function(_)
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

--打开胜利界面
function Modules.SpTeamBossBattleModule:OpenBattleSuccessView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TEAM_BOSS_BATTLE_RESULT_WIN )
end

--打开失败界面
function Modules.SpTeamBossBattleModule:OpenBattleFailedView()
    if self.__is_team_break then
        CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    else
        --结算界面
        CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.TEAM_BOSS_BATTLE_RESULT_WIN )
    end
end

--战斗结束，从结算界面返回对应界面
function Modules.SpTeamBossBattleModule:BattleEndReturnRelevantView()
    Modules.SplogicModule.BattleEndReturnRelevantView(self)

	--返回BOSS怪遭遇场景
    CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", nil, nil, true)
end

--请求战斗结束
function Modules.SpTeamBossBattleModule:BattleEndReq(is_win, star, is_manual_exit)
	local context = self.__hero_total_hp and math.ceil(GlobalVOMgr:CalcTotalHeroHP() / self.__hero_total_hp * 100) or nil
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit, context)
end

--处理怪物数据通知
function Modules.SpTeamBossBattleModule:HandleMonsterDataNotice(monster_info)
    if monster_info.damage >= monster_info.total_hp then
		self.__is_checked_end = true
		self:BattleEndReq(false)    	    
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpTeamBossBattleModule:__StartBattle(for_next)
	Modules.SpMainDungeonModule.__StartBattle(self, for_next)

	self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()
end

--执行打怪处理
function Modules.SpTeamBossBattleModule:__AttackMonster(vo)
    CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.TEAM_BOSS_BATTLE, vo.uid, vo.map_id)                  
end
