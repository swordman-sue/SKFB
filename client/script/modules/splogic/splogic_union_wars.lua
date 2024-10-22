
--[[
%% @module: 军团战战斗
%% @author: ludexian
%% @created: 2018-09-19
--]]

Modules = Modules or {}

Modules.SpUnionWars = Modules.SpUnionWars or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpUnionWars:__init()
end

function Modules.SpUnionWars:__delete()
end

--初始化
function Modules.SpUnionWars:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpUnionWars:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--战斗退出处理
function Modules.SpUnionWars:BattleExitHandle()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

    --销毁动作
    ActionManager.GetInstance():Dump()       

	return true
end

--战斗成功处理
function Modules.SpUnionWars:BattleSuccessHandle(callback)	
	Modules.SpMainDungeonModule.BattleSuccessHandle(self, callback)
end

--打开胜利界面
function Modules.SpUnionWars:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.SP_BATTLE_RESULT_WIN)
end

--战斗失败处理
function Modules.SpUnionWars:BattleFailedHandle(callback)
	-- Modules.SpMainDungeonModule.BattleFailedHandle(self, callback)
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.SP_BATTLE_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpUnionWars:BattleEndReturnRelevantView()
	CallModuleFunc(ModuleType.UNION , "GuildWarRoleDataReq")

	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )

	local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , starDetail.star_id )
	CallModuleFunc(ModuleType.UNION , "GuildWarStarDetailReq" , starCnf )
	
	local combatOccupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , starDetail.star_id )
	--低级
	if starCnf.star_type <= 2 or combatOccupyInfo then
		CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT , true )
	else
		local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
		CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT_BATTLEGROUND , battle_info.param1 )
	end
	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--请求战斗结束
function Modules.SpUnionWars:BattleEndReq(is_win, star, is_manual_exit)
	local context = math.ceil(GlobalVOMgr:CalcTotalHeroHP() / self.__hero_total_hp * 100)
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit, context)
end

--是否自动战斗
function Modules.SpUnionWars:IsAutoFight()
	return CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")
end

--进入GamePlaying状态
function Modules.SpUnionWars:GamePlayingStartHandle()
	Modules.SpMainDungeonModule.GamePlayingStartHandle(self)

	self.__auto_fight_record = CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--离开GamePlaying状态
function Modules.SpUnionWars:GamePlayingStopHandle()
	Modules.SpMainDungeonModule.GamePlayingStopHandle(self)

	CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__auto_fight_record)
end

--是否所有英雄阵亡
function Modules.SpUnionWars:IsAllHeroDead()
	return Modules.SpMainDungeonModule.IsAllHeroDead(self)
end

--是否所有怪物阵亡
function Modules.SpUnionWars:IsAllMonsterDead()
	return Modules.SpMainDungeonModule.IsAllMonsterDead(self)
end    

--计算战斗结算星数
function Modules.SpUnionWars:__CalcResultStar()
	return 0
end

--检测慢镜
function Modules.SpUnionWars:__CheckSlowMirror()
	Modules.SpMainDungeonModule.__CheckSlowMirror(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpUnionWars:__StartBattle(for_next)
	Modules.SpMainDungeonModule.__StartBattle(self, for_next)

	if not for_next then
		self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()
	end
end
