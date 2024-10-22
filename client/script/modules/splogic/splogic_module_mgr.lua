
--[[
%% @module: 特殊逻辑管理器
%% @author: swordman sue
%% @created: 2016-07-14
%% @description: 负责创建管理特殊逻辑
--]]

Modules = Modules or {}

--战斗状态
Modules.SplogicStatus =
{
    UNKNOWN = 0,
    RUN = 1,
    WIN = 2,
    LOSE = 3,
    TIE = 4,
    EXIT = 5,
}

--特殊逻辑注册表
Modules.SplogicModulesRegistry =
{
	--主城
	[Macros.Game.FBType.MAIN_CITY] = 
	{
		class_name = "SpMaincityModule", file_list = {"modules/splogic/splogic_maincity"}
	},

	--[[
		副本
	--]]
	--新手副本
	[Macros.Game.FBType.LEAD] = 
	{
		class_name = "SpLeadDungeonModule", file_list = {"modules/splogic/splogic_lead_dungeon"}
	},
	--主线副本
	[Macros.Game.FBType.MAIN_DUNGEON] = 
	{
		class_name = "SpMainDungeonModule", file_list = {"modules/splogic/splogic_main_dungeon"}
	},
	--英雄副本
	[Macros.Game.FBType.HERO_DUNGEON] = 
	{
		class_name = "SpHeroDungeonModule", file_list = {"modules/splogic/splogic_hero_dungeon"}
	},
	--精英副本
	[Macros.Game.FBType.ELITE_DUNGEON] = 
	{
		class_name = "SpEliteDungeonModule", file_list = {"modules/splogic/splogic_elite_dungeon"}
	},
	--日常副本
	[Macros.Game.FBType.DAILY_DUNGEON] = 
	{
		class_name = "SpDailyDungeonModule", file_list = {"modules/splogic/splogic_tower", "modules/splogic/splogic_daily_dungeon"}
	},	
	--普通爬塔
	[Macros.Game.FBType.TOWER_COMMON] = 
	{
		class_name = "SpTowerModule", file_list = {"modules/splogic/splogic_tower"}
	},
	--精英爬塔
	[Macros.Game.FBType.TOWER_ELITE] = 
	{
		class_name = "SpTowerModule", file_list = {"modules/splogic/splogic_tower"}
	},
	--叛军副本
	[Macros.Game.FBType.REBEL] = 
	{
		class_name = "SpRebelModule", file_list = {"modules/splogic/splogic_rebel"}
	},
	--领地副本
	[Macros.Game.FBType.MANOR_DARE] = 
	{
		class_name = "SpManorDareModule", file_list = {"modules/splogic/splogic_manor_dare"}
	},
	--夺宝副本
	[Macros.Game.FBType.PLUNDERED_TREASURES] = 
	{
		class_name = "SpPlunderedTreasuresModule", file_list = {"modules/splogic/splogic_plundered_treasures"}
	},
	--竞技场
	[Macros.Game.FBType.ARENA] = 
	{
		class_name = "SpArenaModule", file_list = {"modules/splogic/splogic_arena"}
	},
	--世界BOSS
	[Macros.Game.FBType.WORLD_BOSS] = 
	{
		class_name = "SpWorldBossModule", file_list = {"modules/splogic/splogic_world_boss"}
	},
	--公会副本
	[Macros.Game.FBType.UNION] = 
	{
		class_name = "SpUnionModule", file_list = {"modules/splogic/splogic_union"}
	},
	--怪物攻城
	[Macros.Game.FBType.ATTACK_CITY] = 
	{
		class_name = "SpAttackCityModule", file_list = {"modules/splogic/splogic_attack_city"}
	},
	--1V1
	[Macros.Game.FBType.PVP] = 
	{
		class_name = "SpPVPModule", file_list = {"modules/splogic/splogic_pvp"}
	},
	--深渊
	[Macros.Game.FBType.CHASM] = 
	{
		class_name = "SpChasmModule", file_list = {"modules/splogic/splogic_chasm"}
	},
	--组队普通战斗
	[Macros.Game.FBType.TEAM_NORMAL_BATTLE] = 
	{
		class_name = "SpTeamNormalBattleModule", file_list = {"modules/splogic/splogic_team_normal_battle"}
	},
	--组队BOSS战斗
	[Macros.Game.FBType.TEAM_BOSS_BATTLE] = 
	{
		class_name = "SpTeamBossBattleModule", file_list = {"modules/splogic/splogic_team_boss_battle"}
	},
	--天梯赛
	[Macros.Game.FBType.LADDER] = 
	{
		class_name = "SpLadderModule", file_list = {"modules/splogic/splogic_ladder"}
	},
	--占卜战斗
	[Macros.Game.FBType.DIVINATION] = 
	{
		class_name = "SpDivination", file_list = {"modules/splogic/splogic_divination"}
	},
	--积分战斗战斗
	[Macros.Game.FBType.INTEGRAL_RACE] = 
	{
		class_name = "SpIntegralRaceModule", file_list = {"modules/splogic/splogic_integralrace"}
	},
	--资源战战斗
	[Macros.Game.FBType.RESOURCE_WAR] = 
	{
		class_name = "SpResourceWars", file_list = {"modules/splogic/splogic_resource_wars"}
	},
	--工会战低级星球  
	[Macros.Game.FBType.UNION_LOW] = 
	{
		class_name = "SpUnionWars", file_list = {"modules/splogic/splogic_union_wars"}
	},
	--工会战高级星球
	[Macros.Game.FBType.UNION_TALL] = 
	{
		class_name = "SpUnionWars", file_list = {"modules/splogic/splogic_union_wars"}
	},
	


	--[[
		场景
	--]]
	--组队副本大厅
	[Macros.Game.SceneType.TEAM_HALL] = 
	{
		class_name = "SpTeamHallModule", file_list = {"modules/splogic/splogic_team_hall"}
	},
	--组队副本大地图
	[Macros.Game.SceneType.TEAM_WORLD_MAP] = 
	{
		class_name = "SpTeamWorldMapModule", file_list = {"modules/splogic/splogic_team_world_map"}
	},
	--组队采集场景
	[Macros.Game.SceneType.TEAM_GATHER] = 
	{
		class_name = "SpTeamGatherModule", file_list = {"modules/splogic/splogic_team_gather"}
	},
	--组队普通怪遭遇场景
	[Macros.Game.SceneType.TEAM_NORMAL_ENCOUNTER] = 
	{
		class_name = "SpTeamNormalEncounterModule", file_list = {"modules/splogic/splogic_team_normal_encounter"}
	},
	--组队BOSS怪遭遇场景
	[Macros.Game.SceneType.TEAM_BOSS_ENCOUNTER] = 
	{
		class_name = "SpTeamBossEncounterModule", file_list = {"modules/splogic/splogic_team_boss_encounter"}
	},
}

Modules.SplogicModulesMgr = Modules.SplogicModulesMgr or BaseClass(BaseModule)

local get_module_class

function Modules.SplogicModulesMgr:__init()
	self.__can_update = true

	self.__default_module = Modules.SplogicModule.New(Macros.Game.FBType.DEFAULT)
	self.__current_module = self.__default_module
	self.__modules_class_map = {}

	self.__is_in_background = false
	self.__just_enter_foreground = false

    --所有将要弹出的窗体信息
    self.__cur_dialog_info = nil	
    self.__dialog_info_list = {}
    self.__oc_dialog_info_list = {}
    self.__exit_battle_dialog_info_list = {}
end

function Modules.SplogicModulesMgr:__delete()
	self:Stop()
	self:UnloadRes()
	self:__ClearEnterForeGroundTimer()

	self.__modules_class_map = nil
end

function Modules.SplogicModulesMgr:Update(now_time, elapse_time)
	if not self.__is_in_pause then
		self.__current_module:Update(now_time, elapse_time)
	end
end

--副本开始
function Modules.SplogicModulesMgr:Start()
	self:__InitBattleTimeout()
	self:Resume()
	self:__InitCountDownTimer()

	local scene_info = GlobalVOMgr:GetSceneInfo()
	if not scene_info then
		LogTool.LogError("[Modules.SplogicModulesMgr:Start] 找不到场景信息")
		return
	end

	LogTool.LogInfo("[Modules.SplogicModulesMgr:Start] 特殊逻辑开启 type:%d", scene_info.type)

	--加载资源
	self:LoadRes(nil, nil, function()
		self.__is_res_loaded = true
	end)

	local module_class = get_module_class(self, scene_info.type)
	if module_class then
		self.__current_module = module_class.New(scene_info.type, scene_info.id)
		self.__current_module:Initialize()
		self.__current_module:RegistAllProtocols()
		self.__current_module:LoadRes()
	else
		LogTool.LogError("[Modules.SplogicModulesMgr:Start] 找不到对应的特殊逻辑类(type:%d)", scene_info.type)
		self.__current_module = self.__default_module
	end
end

--副本结束
function Modules.SplogicModulesMgr:Stop(cleanup)
	self:__StopCountDownTimer(true)
	self:DumpDialogs()

	if cleanup ~= false then
		--将要进入副本，卸载公共资源
		local scene_info = GlobalVOMgr:GetSceneInfo()
		if not scene_info or scene_info.type ~= Macros.Game.FBType.MAIN_CITY then
			self:UnloadRes()
		end

		--重置暂停引用
		if self.__ref_in_pause then
			self.__ref_in_pause:Reset()
		end
	end

	--卸载当前
	if self.__current_module then
		self.__current_module:UnloadRes()
		self.__current_module:GamePlayingStopHandle()
		self.__current_module:UnRegistAllProtocols()
		self.__current_module:UnRegistAllEvents()
		self.__current_module:Dispose()
		
		if cleanup ~= false then
			self.__current_module:DeleteMe()
			self.__current_module = self.__default_module
		end
	end

	if cleanup ~= false then
		LogTool.LogInfo("[Modules.SplogicModulesMgr:Stop] 特殊逻辑结束")
	end
end

--获取当前类型
function Modules.SplogicModulesMgr:GetCurType()
	return self.__current_module:GetType()
end

--获取当前状态
function Modules.SplogicModulesMgr:GetCurStatus()
	return self.__current_module:GetStatus()
end

--处理界面关闭逻辑
function Modules.SplogicModulesMgr:HandleNoViewOpen()
	if self:IsInMainCity() then
		return
	end

	--战斗结束且所有界面都关闭了，则返回主城
	local ret = self.__current_module:GetStatus()
	if ret == Modules.SplogicStatus.EXIT then
		CallModuleFunc(ModuleType.BATTLE, "BattleEndReturnCity")
	end
end

--设置结束时间，并开启倒计时
function Modules.SplogicModulesMgr:SetEndTime(delta_second)
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	self.__remain_time = delta_second
	self.__current_module:SetRemainTime(self.__remain_time)
	self:__StartCountDownTimer()
end

--设置剩余时间
function Modules.SplogicModulesMgr:SetRemainTime(var)
	self.__current_module:SetRemainTime(var)
end

--获取剩余时间
function Modules.SplogicModulesMgr:GetRemainTime()
	return self.__remain_time or 0
end

--游戏暂停
function Modules.SplogicModulesMgr:Pause(for_end, for_world_star)
	if self.__is_in_pause then
		return
	end
	self.__is_in_pause = true

	if self:__IsShowPauseDialog() then
		self:__ShowPauseDialog()
	end
	self:__PauseTimer()
	self:__CheckBattleTimeout(true)

	self.__current_module:Pause(for_end, for_world_star)	

	self:FireNextFrame(Macros.Event.SpLogicModule.PAUSE)
end

--游戏恢复
function Modules.SplogicModulesMgr:Resume()
	if not self.__is_in_pause then
		return
	end
	self.__is_in_pause = false

	self:__ResumeTimer()
	self:__CheckBattleTimeout()

	self.__current_module:Resume()

	self:FireNextFrame(Macros.Event.SpLogicModule.RESUME)	
end

--游戏暂停(用于外部可能出现冲突的情况)
function Modules.SplogicModulesMgr:PauseEx(...)
	self.__ref_in_pause = self.__ref_in_pause or RefObj.New()
	self.__ref_in_pause:Retain()
	if self.__ref_in_pause:GetRefCount() == 1 then
		self:Pause(...)
	end
end

--游戏恢复
function Modules.SplogicModulesMgr:ResumeEx()
	if not self.__ref_in_pause or not self.__ref_in_pause:IsValid() then
		return
	end	
	self.__ref_in_pause:Release()
	if not self.__ref_in_pause:IsValid() then
		self:Resume()
	end
end

--是否暂停中(用于外部可能出现冲突的情况)
function Modules.SplogicModulesMgr:IsPause()
	return self.__is_in_pause
end

--怒气/合击技能暂停相关
function Modules.SplogicModulesMgr:PauseByScreen()
	if self.__ref_count_down_timer_active then
		self.__ref_count_down_timer_active:Release()
	end
end

--怒气/合击技能恢复相关
function Modules.SplogicModulesMgr:ResumeByScreen()
	if self.__ref_count_down_timer_active then
		self.__ref_count_down_timer_active:Retain()
	end
end

--设置战斗加速
function Modules.SplogicModulesMgr:SetDeltaSpeedTime(factor, reset_timer)
    CPPSceneManager:SetDeltaSpeedTime(factor)

    if reset_timer and self.__count_down_timer then
		self:__StartCountDownTimer(factor)
	end
end

--加载资源
function Modules.SplogicModulesMgr:LoadRes(texture_list, template_list, finished_callback)
	local org_texture_list = texture_list
	local org_template_list = template_list
	texture_list = {}
	template_list = {}

	--过滤已加载资源
	if org_texture_list then
		for _, tex_atlas_info in ipairs(org_texture_list) do
			if not self.__splogic_loaded_texinfo_list or not self.__splogic_loaded_texinfo_list[tex_atlas_info] then
				table.insert(texture_list, tex_atlas_info)
			end
		end
	end
	if org_template_list then
		for _, templ_registry_info in ipairs(org_template_list) do
			if not self.__splogic_loaded_templinfo_list or not self.__splogic_loaded_templinfo_list[templ_registry_info] then
				table.insert(template_list, templ_registry_info)
			end
		end	
	end

    self.__loaded_tex_count = 0
    self.__loaded_template_count = 0
    self.__need_load_tex_count = texture_list and #texture_list or 0
    self.__need_load_template_count = template_list and #template_list or 0
    
    local function load_tex_callback(texture_path)
        self.__loaded_tex_count = self.__loaded_tex_count + 1
        
        if self.__loaded_tex_count == self.__need_load_tex_count then
           
            --加载模板
            local function load_template_callback()
                self.__loaded_template_count = self.__loaded_template_count + 1
                if self.__loaded_template_count == self.__need_load_template_count then
                    finished_callback()
                end
            end                            
            if self.__need_load_template_count > 0 then
                for _, templ_registry_info in ipairs(template_list) do
                    GUITemplateMgr:AddAndLoadModuleLayout(templ_registry_info[1], 
                    	Resource.PathTool.GetUILayoutPath(templ_registry_info[2]), templ_registry_info[3], load_template_callback)

                    self.__splogic_loaded_templinfo_list = self.__splogic_loaded_templinfo_list or {}
                    self.__splogic_loaded_templinfo_list[templ_registry_info] = true
                end
            else
                self.__loaded_template_count = -1
                load_template_callback()
            end
        end
    end

    --加载贴图集
    if self.__need_load_tex_count > 0 then
        for _, tex_atlas_info in ipairs(texture_list) do
            Resource.LoadTool.LoadUITextureAtlas(tex_atlas_info, load_tex_callback)

            self.__splogic_loaded_texinfo_list = self.__splogic_loaded_texinfo_list or {}
            self.__splogic_loaded_texinfo_list[tex_atlas_info] = true
        end
    else
        self.__loaded_tex_count = -1
        load_tex_callback()
    end
end

function Modules.SplogicModulesMgr:UnloadRes()
    self.__is_res_loaded = false

    --模板
    if self.__splogic_loaded_templinfo_list then
        for templ_registry_info, _ in pairs(self.__splogic_loaded_templinfo_list) do
            GUITemplateMgr:UnLoadAndRemoveModuleLayout(templ_registry_info[1])
        end
        self.__splogic_loaded_templinfo_list = nil
    end

    --贴图集
    if self.__splogic_loaded_texinfo_list then
        for tex_atlas_info, _ in pairs(self.__splogic_loaded_texinfo_list) do
            Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
        end
        self.__splogic_loaded_texinfo_list = nil
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--特殊逻辑
--------------------------------------------------------------------------------------------------------------------------------
--是否主城
function Modules.SplogicModulesMgr:IsInMainCity()
	return self.__current_module:GetType() == Macros.Game.FBType.MAIN_CITY
end

--是否新手副本
function Modules.SplogicModulesMgr:IsInLeadDungeon()
	return self.__current_module:GetType() == Macros.Game.FBType.LEAD
end

--是否PVP副本
function Modules.SplogicModulesMgr:IsPVPDungeon()
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	return self.__current_module:GetType() == Macros.Game.FBType.PVP and (battle_info and not battle_info.is_pve)
end

--是否PVP副本(包括机器人)
function Modules.SplogicModulesMgr:IsPVPDungeonIncludeRobot()
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	return self.__current_module:GetType() == Macros.Game.FBType.PVP
end

--是否无尽之地
function Modules.SplogicModulesMgr:IsChasmDungeon()
	return self.__current_module:GetType() == Macros.Game.FBType.CHASM
end

--是否组队战斗副本(小怪/BOSS战斗副本)
function Modules.SplogicModulesMgr:IsTeamBattleDungeon()
	return self.__current_module:GetType() == Macros.Game.FBType.TEAM_NORMAL_BATTLE or 
	self.__current_module:GetType() == Macros.Game.FBType.TEAM_BOSS_BATTLE
end

function Modules.SplogicModulesMgr:IsResLoaded()
	return self.__is_res_loaded and self.__current_module:IsResLoaded()
end

function Modules.SplogicModulesMgr:IsResLoadedEx()
	return self.__current_module:IsResLoadedEx()
end

function Modules.SplogicModulesMgr:ResLoadedNotify()
	self.__current_module:ResLoadedNotify()
end

function Modules.SplogicModulesMgr:GetResLoadInfo()
	local res_load_count, res_need_load_count = self.__current_module:GetResLoadInfo()
	return res_load_count + self.__loaded_tex_count + self.__loaded_template_count, 
	res_need_load_count + self.__need_load_tex_count + self.__need_load_template_count
end

function Modules.SplogicModulesMgr:GetKeepRes()
	local keep_list = self.__current_module:GetKeepRes()
    if self.__splogic_loaded_texinfo_list then
        for tex_atlas_info, _ in pairs(self.__splogic_loaded_texinfo_list) do
            keep_list = keep_list or {}
            table.insert(keep_list, tex_atlas_info.texture_path)
        end
    end
    return keep_list
end

function Modules.SplogicModulesMgr:IsNeedLoadMap()
	return self.__current_module:IsNeedLoadMap()
end

function Modules.SplogicModulesMgr:IsInBackground()
	return self.__is_in_background
end

function Modules.SplogicModulesMgr:IsJustEnterForeground()
	return self.__just_enter_foreground
end

--进入前台处理
function Modules.SplogicModulesMgr:DoAfterEnterForeground()
	self.__is_in_background = false
	self.__just_enter_foreground = true

	self:__ClearEnterForeGroundTimer()
	self.__enter_foreground_timer = GlobalTimerQuest:AddDelayQuest(
		GlobalCallbackMgr:AddCallBackFunc(function()
				self.__enter_foreground_timer = nil
				self.__just_enter_foreground = false
			end), 4.0)

	if self:__IsPauseWhenDeActive() and not self:__IsShowPauseDialog() then
		--进入前台，自动恢复
		self:Resume()
	end
end

--进入后台处理
function Modules.SplogicModulesMgr:DoAfterEnterBackground()
	self.__is_in_background = true
	self.__just_enter_foreground = false
	self:__ClearEnterForeGroundTimer()

	if self:__IsPauseWhenDeActive() then
		--进入后台，自动暂停
		self:Pause()
	end
end

--进入GamePlaying状态
function Modules.SplogicModulesMgr:GamePlayingStartHandle()
	self.__current_module:GamePlayingStartHandle()
end

--离开GamePlaying状态
function Modules.SplogicModulesMgr:GamePlayingStopHandle()
	self.__current_module:GamePlayingStopHandle()
end

--战斗成功处理
function Modules.SplogicModulesMgr:BattleSuccessHandle()
	self.__current_module:BattleEndHandle(Modules.SplogicStatus.WIN)
	self:Pause(true)
	
	self.__current_module:BattleSuccessHandle(function()
		self.__current_module:StopBGM()
		self.__current_module:OpenBattleSuccessView()
	end)
end

--战斗平局处理
function Modules.SplogicModulesMgr:BattleTieHandle()
	self.__current_module:BattleEndHandle(Modules.SplogicStatus.TIE)
	self:Pause(true)
	
	self.__current_module:BattleTieHandle(function()
		self.__current_module:StopBGM()
		self.__current_module:OpenBattleTieView()
	end)
end

--战斗失败处理
function Modules.SplogicModulesMgr:BattleFailedHandle()	
	self.__current_module:BattleEndHandle(Modules.SplogicStatus.LOSE)
	self:Pause(true)

	self.__current_module:BattleFailedHandle(function()
		self.__current_module:StopBGM()
		self.__current_module:OpenBattleFailedView()
	end)
end

--战斗退出处理
function Modules.SplogicModulesMgr:BattleExitHandle()
	if not self.__current_module:BattleExitHandle() then
		self.__current_module:BattleEndHandle(Modules.SplogicStatus.LOSE)
		self:BattleEndReturnRelevantView()
	end
end

--战斗结束，从结算界面返回对应界面
function Modules.SplogicModulesMgr:BattleEndReturnRelevantView(callback,...)
	local params = {...}
	--处理返回逻辑
	self.__current_module:BattleEndReturnRelevantViewPreload(function()
		self:Stop(false)
		self.__current_module:BattleEndReturnRelevantView(unpack(params))
		if callback then
			callback()
		end
	end)
end

--边界处理(以屏幕可视区域为边界)
function Modules.SplogicModulesMgr:BoundTest(pos, is_world_pos, ...)
	self.__current_module:BoundTest(pos, is_world_pos, ...)
end

--血量处理
function Modules.SplogicModulesMgr:DoHpChange(obj_id, change_hp, damage_type, frm_protocol)
	self.__current_module:DoHpChange(obj_id, change_hp, damage_type, frm_protocol)
end

--血量变化同步请求
function Modules.SplogicModulesMgr:HPChangeSyncReq(type, src_obj_id, tgt_obj_id, change_hp, damage_type)
	self.__current_module:HPChangeSyncReq(type, src_obj_id, tgt_obj_id, change_hp, damage_type)
end

--对象零血通知(客户端)
function Modules.SplogicModulesMgr:ObjDeadNotify(obj_id, frm_protocol)
	self.__current_module:ObjDeadNotify(obj_id, frm_protocol)
end

--怒气处理
function Modules.SplogicModulesMgr:DoAngerChange(change_anger, is_enemy)
	self.__current_module:DoAngerChange(change_anger, is_enemy)
end

--获取首个敌人ID
function Modules.SplogicModulesMgr:GetFirstEnemy(obj_id)
	return self.__current_module:GetFirstEnemy(obj_id)
end

--获取最后一个敌人ID
function Modules.SplogicModulesMgr:GetLastEnemy(obj_id)
	return self.__current_module:GetLastEnemy(obj_id) 
end

--获取敌人VO列表
function Modules.SplogicModulesMgr:GetEnemyList(obj_id, check_can_dobeattacked, skill_id)
	return self.__current_module:GetEnemyList(obj_id, check_can_dobeattacked, skill_id)
end

--获取敌人VO表
function Modules.SplogicModulesMgr:GetEnemyMap(obj_id)
	return self.__current_module:GetEnemyMap(obj_id)
end

--获取首个己方对象ID
function Modules.SplogicModulesMgr:GetFirstTeammate(obj_id)
	return self.__current_module:GetFirstTeammate(obj_id)
end

--获取最后己方对象ID
function Modules.SplogicModulesMgr:GetLastTeammate(obj_id)
	return self.__current_module:GetLastTeammate(obj_id)
end

--获取己方VO列表
function Modules.SplogicModulesMgr:GetTeammateList(obj_id, check_can_dobeattacked, skill_id)
	return self.__current_module:GetTeammateList(obj_id, check_can_dobeattacked, skill_id)
end

--获取己方VO表
function Modules.SplogicModulesMgr:GetTeammateMap(obj_id)
	return self.__current_module:GetTeammateMap(obj_id)
end

--获取首个目标
function Modules.SplogicModulesMgr:GetFirstBattleTgt(obj_id, skill_id)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND then
		return self:GetFirstTeammate(obj_id)
	end	
	return self:GetFirstEnemy(obj_id)
end

--获取最后一个目标
function Modules.SplogicModulesMgr:GetLastBattleTgt(obj_id, skill_id)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND then
		return self:GetLastTeammate(obj_id)
	end	
	return self:GetLastEnemy(obj_id)
end

--获取目标列表
function Modules.SplogicModulesMgr:GetBattleTgtList(obj_id, skill_id, check_can_dobeattacked)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND then
		return self:GetTeammateList(obj_id, check_can_dobeattacked, skill_id)
	end
	return self:GetEnemyList(obj_id, check_can_dobeattacked, skill_id)
end

--获取首个可受击近战目标
function Modules.SplogicModulesMgr:GetFirstPNTgt(obj_id, skill_id)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND then
		return self.__current_module:GetFirstPNTeammate(obj_id)
	end
	return self.__current_module:GetFirstPNEnemy(obj_id)
end

--获取己方精灵
function Modules.SplogicModulesMgr:GetTeammateSprite()
	return self.__current_module:GetTeammateSprite()
end

--添加待攻击近战碰撞对记录
function Modules.SplogicModulesMgr:AddToAttackPNObjsRecord(obj_id, attacker_obj_id, skill_id, skill_lev)
	self.__current_module:AddToAttackPNObjsRecord(obj_id, attacker_obj_id, skill_id, skill_lev)
end

--移除待攻击近战碰撞对记录
function Modules.SplogicModulesMgr:DelToAttackPNObjsRecord(obj_id, force)
	self.__current_module:DelToAttackPNObjsRecord(obj_id, force)
end

--记录本帧中的攻击对象(统一触发攻击逻辑)
function Modules.SplogicModulesMgr:RecordAttackObjCurFrame(obj_id)
	self.__current_module:RecordAttackObjCurFrame(obj_id)
end

--记录本帧中的受击对象(统一触发受击逻辑)
function Modules.SplogicModulesMgr:RecordBeattackedObjCurFrame(obj_id, attacker_obj_id, skill_id, skill_lev, damage_time_index)
	self.__current_module:RecordBeattackedObjCurFrame(obj_id, attacker_obj_id, skill_id, skill_lev, damage_time_index)
end

--记录本帧中的碰撞对象(统一触发碰撞逻辑)
function Modules.SplogicModulesMgr:RecordCollisionObjCurFrame(obj_id, tgt_id)
	self.__current_module:RecordCollisionObjCurFrame(obj_id, tgt_id)
end

--记录本帧中的BUFF检测(统一触发BUFF)
function Modules.SplogicModulesMgr:RecordCheckBuffCurFrame(obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)
	self.__current_module:RecordCheckBuffCurFrame(obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)
end

--是否自动战斗
function Modules.SplogicModulesMgr:IsAutoFight()
	return self.__current_module:IsAutoFight()
end

--是否全体英雄、怪物进入镜头
function Modules.SplogicModulesMgr:IsAllObjsEnterRegion()
	return self.__current_module:IsAllObjsEnterRegion()
end

--是否怒气/合击技能释放中
function Modules.SplogicModulesMgr:IsInAngerOrJointSkill()
	return self.__current_module:IsInAngerOrJointSkill()
end

--请求战斗结束
function Modules.SplogicModulesMgr:BattleEndReq(...)
	self.__current_module:BattleEndReq(...)
end

--是否所有英雄阵亡
function Modules.SplogicModulesMgr:IsAllHeroDead()
	return self.__current_module:IsAllHeroDead()
end

--是否所有怪物阵亡
function Modules.SplogicModulesMgr:IsAllMonsterDead()
	return self.__current_module:IsAllMonsterDead()
end

--是否已完成BOSS展示
function Modules.SplogicModulesMgr:IsBossMessageShowFinished()
	return self.__current_module:IsBossMessageShowFinished()
end

--触发怒气/合击技能
function Modules.SplogicModulesMgr:TriggerAngerOrJointSkill(obj_id, is_plus)
	return self.__current_module:TriggerAngerOrJointSkill(obj_id, is_plus)
end

--触发精灵技能
function Modules.SplogicModulesMgr:TriggerSpriteSkill(obj_id, skill_id, world_pos)
	return self.__current_module:TriggerSpriteSkill(obj_id, skill_id, world_pos)
end

--动态创建怪物
function Modules.SplogicModulesMgr:CreateMonsterDynamic(data)
self.__current_module:CreateMonsterDynamic(data)
end

--处理跳过战斗逻辑
function Modules.SplogicModulesMgr:HandleSkipBattle()
	self.__current_module:HandleSkipBattle()
end

--处理跳过战斗逻辑结束
function Modules.SplogicModulesMgr:HandleSkipBattleEnd()
	self.__current_module:HandleSkipBattleEnd()
end

--处理快速战斗逻辑
function Modules.SplogicModulesMgr:HandleQuickBattle()
	self.__current_module:HandleQuickBattle()
end

--是否显示战力变化特效
function Modules.SplogicModulesMgr:CanShowBattleValueChangeEffect()
	return self.__current_module:GetType() == Macros.Game.FBType.MAIN_CITY or
	self.__current_module:GetType() == Macros.Game.SceneType.TEAM_HALL or
	self:GetCurStatus() == Modules.SplogicStatus.EXIT or
	self:GetCurStatus() == Modules.SplogicStatus.LOSE
end

--处理进入切场景状态
function Modules.SplogicModulesMgr:HandleEnterSceneChangeState(callback)
	self.__current_module:HandleEnterSceneChangeState(callback)	
end

--处理离开切场景状态
function Modules.SplogicModulesMgr:HandleQuitSceneChangeState(callback)
	self.__current_module:HandleQuitSceneChangeState(callback)	
end

--处理场景进入通知(用于多人同屏)
function Modules.SplogicModulesMgr:HandleEnterSceneNotice(protocol_data)
	self.__current_module:HandleEnterSceneNotice(protocol_data)
end

--处理场景离开通知(用于多人同屏)
function Modules.SplogicModulesMgr:HandleLeaveSceneNotice(protocol_data)
	self.__current_module:HandleLeaveSceneNotice(protocol_data)
end

--处理英雄行走通知(用于多人同屏)
function Modules.SplogicModulesMgr:HandleHeroGotoNotice(protocol_data)
	self.__current_module:HandleHeroGotoNotice(protocol_data)
end

--处理怪物数据通知
function Modules.SplogicModulesMgr:HandleMonsterDataNotice(protocol_data)
	self.__current_module:HandleMonsterDataNotice(protocol_data)
end

--处理怪物新增通知
function Modules.SplogicModulesMgr:HandleMonsterAddNotice(monster_info)
	self.__current_module:HandleMonsterAddNotice(monster_info)
end

--处理采集物新增通知
function Modules.SplogicModulesMgr:HandleItemAddNotice(item_info)
	self.__current_module:HandleItemAddNotice(item_info)
end

--处理采集物锁定响应
function Modules.SplogicModulesMgr:HandleItemLockResp(protocol_data)
	self.__current_module:HandleItemLockResp(protocol_data)
end

--处理采集物锁定通知
function Modules.SplogicModulesMgr:HandleItemLockNotify(protocol_data)
	self.__current_module:HandleItemLockNotify(protocol_data)
end

--处理采集物拾取响应
function Modules.SplogicModulesMgr:HandleItemPickupResp(protocol_data)
	self.__current_module:HandleItemPickupResp(protocol_data)
end

--处理采集物拾取通知
function Modules.SplogicModulesMgr:HandleItemPickupNotify(protocol_data)
	self.__current_module:HandleItemPickupNotify(protocol_data)
end

--处理采集物消失响应
function Modules.SplogicModulesMgr:HandleItemDisappearNotice(protocol_data)
	self.__current_module:HandleItemDisappearNotice(protocol_data)
end

--处理组队副本任务奖励界面关闭
function Modules.SplogicModulesMgr:HandleTeamDungeonTaskRewardViewClosed()
	self.__current_module:HandleTeamDungeonTaskRewardViewClosed()	
end

--当指引和剧情时，是否暂停
function Modules.SplogicModulesMgr:IsPauseWhenLeadOrStory()
	return self.__current_module:IsPauseWhenLeadOrStory()
end

--------------------------------------------------------------------------------------------------------------------------------
--弹窗管理
--------------------------------------------------------------------------------------------------------------------------------
--[[
@param callback    			弹窗实际操作
@param priority      		弹窗优先级
@param not_only_city 		不只主城打开(默认true)
@param exit_battle  		离开战斗结算界面后打开(默认false)
]]
function Modules.SplogicModulesMgr:InsertDialog(callback, priority, not_only_city, exit_battle)
    not_only_city = not_only_city == nil and true or not_only_city

    local info = {
        callback_func = callback,
        priority      = priority or 1,
        only_city     = not not_only_city,
        exit_battle   = exit_battle or false,
    }

    local index = #self.__dialog_info_list + 1
    for i, v in ipairs(self.__dialog_info_list) do
        --即时弹出的，插到即时列表最后
        --非即时弹出的，插到同优先级的最后
        if info.priority < v.priority then
            index = i
            break
        end
    end
    table.insert(self.__dialog_info_list, index, info)

    if not self.__cur_dialog_info then
        self:ShowDialog()
    end
end

--弹出弹窗
function Modules.SplogicModulesMgr:PopDialog()
    if #self.__dialog_info_list == 0 then
        return nil
    end
    return table.remove(self.__dialog_info_list, 1)
end

--显示弹窗
function Modules.SplogicModulesMgr:ShowDialog()
    --关闭窗口的回调方法
    local function close_callback()
        self.__cur_dialog_info = nil
        self:ShowDialog()
    end

    local only_city_open = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") and GlobalViewMgr:IsOnlyCityViewOpen()
    local is_in_battle = not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") and self.__current_module:GetStatus() ~= Modules.SplogicStatus.EXIT

    --取出第一个窗口
    while #self.__dialog_info_list > 0 do
        if self.__cur_dialog_info then
            break
        end

        local cur_dialog_info = self:PopDialog()

        --当前不在主城，缓存返回主城再打开的弹窗
        if not only_city_open and cur_dialog_info.only_city then
            table.insert(self.__oc_dialog_info_list, cur_dialog_info)

            if not self.only_city_open_handle then
                local function only_city_open_func()
                	if self.only_city_open_handle then
                        self:UnBind(self.only_city_open_handle)
                        self.only_city_open_handle = nil
                    end

                    --优先级排序
                    table.sort(self.__oc_dialog_info_list, SortTools.KeyLowerSorter("priority"))

                    for _, dialog_info in ipairs(self.__oc_dialog_info_list) do
                        self:InsertDialog(dialog_info.callback_func, dialog_info.priority, not dialog_info.only_city, dialog_info.exit_battle)
                    end
                    self.__oc_dialog_info_list = {}
                end                
                self.only_city_open_handle = self:Bind(Macros.Event.View.ONLY_CITY_VIEW_OPEN, only_city_open_func)
            end

        --当前在战斗，缓存离开战斗结算再打开的弹窗
        elseif is_in_battle and cur_dialog_info.exit_battle then
            table.insert(self.__exit_battle_dialog_info_list, cur_dialog_info)

            if not self.exit_battle_handle then
                local function exit_battle_func()
                	if self.exit_battle_handle then
                        self:UnBind(self.exit_battle_handle)
                        self.exit_battle_handle = nil
                    end

                    --优先级排序
                    table.sort(self.__exit_battle_dialog_info_list, SortTools.KeyLowerSorter("priority"))

                    for _, dialog_info in ipairs(self.__exit_battle_dialog_info_list) do
                        self:InsertDialog(dialog_info.callback_func, dialog_info.priority, not dialog_info.only_city, dialog_info.exit_battle)
                    end
                    self.__exit_battle_dialog_info_list = {}
                end                
                self.exit_battle_handle = self:Bind(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW, exit_battle_func)
            end        	

        else
            self.__cur_dialog_info = cur_dialog_info
        end
    end

    if self.__cur_dialog_info then
        self.__cur_dialog_info.callback_func(close_callback)
    end
end

function Modules.SplogicModulesMgr:HasDialog()
    return #self.__dialog_info_list > 0
end

function Modules.SplogicModulesMgr:DumpDialogs()
    self.__cur_dialog_info = nil
    self.__dialog_info_list = {}
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--进入后台后是否暂停
function Modules.SplogicModulesMgr:__IsPauseWhenDeActive()
	if self.__is_battle_timeout then
		return false
	end
	
	return self.__current_module:IsPauseWhenDeActive()
end

--是否显示暂停弹窗
function Modules.SplogicModulesMgr:__IsShowPauseDialog()
	if self.__is_battle_timeout then
		return false
	end

	return self.__current_module:IsShowPauseDialog()
end

--显示暂停弹窗
function Modules.SplogicModulesMgr:__ShowPauseDialog()
    LogicGUI.TemplAffirmTips.New({
        content = CPPGameLib.GetString("battle_exit_title"),
        confirm_func = function()
        	if self:__CheckBattleTimeout() then
        		return
        	end
            --退出
            self.__current_module:BattleEndReq(false, 0, true)
        end,

        cancel_func = function()
        	local status = self.__current_module:GetStatus()
        	if status == Modules.SplogicStatus.RUN then
		        --恢复
	            self:Resume()
            end
        end,
        show_close_btn = false,
    })
end

--初始计时器
function Modules.SplogicModulesMgr:__InitCountDownTimer()
	self.__remain_time = 0

	if self.__ref_count_down_timer_active then
		self.__ref_count_down_timer_active:DeleteMe()
	end
	self.__ref_count_down_timer_active = RefObj.New()
	self.__ref_count_down_timer_active:Retain()
end

--开启计时器
function Modules.SplogicModulesMgr:__StartCountDownTimer(factor)
	self:__StopCountDownTimer()

	factor = factor or CallModuleFunc(ModuleType.BATTLE, "GetACCFactor")
	self.__count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:__CountDownTimerStep()
		end), 1 / factor, -1)
end

--计时
function Modules.SplogicModulesMgr:__CountDownTimerStep()
	if self.__ref_count_down_timer_active and self.__ref_count_down_timer_active:IsValid() then
		self.__remain_time = self.__remain_time - 1

		--剩余时间
		self:SetRemainTime(self.__remain_time)

		--结束
		if self.__remain_time <= 0 then
			self.__remain_time = 0
			self:__StopCountDownTimer(true)
		end
	end
end

--关闭计时器
function Modules.SplogicModulesMgr:__StopCountDownTimer(cleanup)
	if self.__count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__count_down_timer)
		self.__count_down_timer = nil
	end

	if cleanup then
		if self.__ref_count_down_timer_active then
			self.__ref_count_down_timer_active:DeleteMe()
			self.__ref_count_down_timer_active = nil
		end
	end
end

--暂停计时器
function Modules.SplogicModulesMgr:__PauseTimer()
	if self:IsPVPDungeon() then
		return
	end

	if self.__ref_count_down_timer_active then
		self.__ref_count_down_timer_active:Release()
	end
end

--恢复计时器
function Modules.SplogicModulesMgr:__ResumeTimer()
	if self:IsPVPDungeon() then
		return
	end
	
	if self.__ref_count_down_timer_active then
		self.__ref_count_down_timer_active:Retain()
	end
end

--初始超时参数
function Modules.SplogicModulesMgr:__InitBattleTimeout()
	self.__is_battle_timeout = false
	self.__battle_pause_timepoint = nil
end

--检测战斗超时
function Modules.SplogicModulesMgr:__CheckBattleTimeout(just_record_timepoint)
	if just_record_timepoint then
		--记录战斗暂停时间点
		self.__battle_pause_timepoint = Macros.Global.NowTime
		return
	end

	local ret

	if self.__is_battle_timeout then
		return ret
	end

	if self.__battle_pause_timepoint and (Macros.Global.NowTime - self.__battle_pause_timepoint >= 1800) then
		ret = true
		self.__is_battle_timeout = true

		local function func()
        	self.__is_battle_timeout = false
			CallModuleFunc(ModuleType.BATTLE, "BattleEndReturnCity")
		end
	    LogicGUI.TemplAffirmTips.New({
	    	type = LogicGUI.TemplAffirmTipsType.YES,
	        content = CPPGameLib.GetString("battle_timeout"),
	        confirm_func = func,
	        cancel_func = func,
	        show_close_btn = false,
	    })		

	    self:Pause()
	end

	self.__battle_pause_timepoint = nil

	return ret
end

function Modules.SplogicModulesMgr:__ClearEnterForeGroundTimer()
	if self.__enter_foreground_timer then
		GlobalTimerQuest:CancelQuest(self.__enter_foreground_timer)
		self.__enter_foreground_timer = nil
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_module_class = function(obj_self, module_type)
	if not module_type then
		LogTool.LogError("[get_module_class] 参数module_type无效")
		return
	end

	if obj_self.__modules_class_map[module_type] then
		return obj_self.__modules_class_map[module_type]
	end

	local module_registry_info = Modules.SplogicModulesRegistry[module_type]
	if not module_registry_info then
		LogTool.LogError("[get_module_class] 找不到对应的特殊逻辑注册信息(module_type:%d)", module_type)
		return
	end

	RequireToolBatch:LoadFilesSync({file_list = module_registry_info.file_list})

	local module_class = Modules[module_registry_info.class_name]
	if not module_class then
		LogTool.LogError("[get_module_class] 找不到对应的特殊逻辑类(module_type:%d, class_name:%s)", module_type, module_registry_info.class_name)
		return
	end

	obj_self.__modules_class_map[module_type] = module_class
	return module_class
end

