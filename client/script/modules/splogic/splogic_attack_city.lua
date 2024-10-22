
--[[
%% @module: 怪物攻城
%% @author: swordman sue
%% @created: 2017-7-6
--]]

Modules = Modules or {}

Modules.SpAttackCityModule = Modules.SpAttackCityModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpAttackCityModule:__init()
end

function Modules.SpAttackCityModule:__delete()
end

--初始化
function Modules.SpAttackCityModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpAttackCityModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

function Modules.SpAttackCityModule:Update(now_time, elapse_time)
    Modules.SpMainDungeonModule.Update(self, now_time, elapse_time)

    self:__CheckACC()    
end

--打开胜利界面
function Modules.SpAttackCityModule:OpenBattleSuccessView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.ATTACK_CITY_RESULT_WIN)
end

--打开失败界面
function Modules.SpAttackCityModule:OpenBattleFailedView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.ATTACK_CITY_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpAttackCityModule:BattleEndReturnRelevantView()
	CallModuleFunc(ModuleType.MONSTER_STORMCASTLE, "OpenView", BaseViewType.MONSTER_STORMCASTLE)

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--请求战斗结束
function Modules.SpAttackCityModule:BattleEndReq(is_win, star, is_manual_exit)
	local context = math.ceil(GlobalVOMgr:CalcTotalHeroHP() / self.__hero_total_hp * 100)
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit, context)
end

--注册事件
function Modules.SpAttackCityModule:RegistAllEvents()
	Modules.SpMainDungeonModule.RegistAllEvents(self)

	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_BOSS_DEAD, function(_)
		self.__is_checked_end = true
		self:BattleEndReq(false)		
	end)	
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpAttackCityModule:__StartBattle(for_next)
	Modules.SpMainDungeonModule.__StartBattle(self, for_next)

	if not for_next then
		self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()
	end
end

