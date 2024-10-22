
--[[
%% @module: 主线副本(单人)
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.SpMainDungeonModule = Modules.SpMainDungeonModule or BaseClass(Modules.SplogicModule)

function Modules.SpMainDungeonModule:__init()
end

function Modules.SpMainDungeonModule:__delete()
end

--初始化
function Modules.SpMainDungeonModule:Initialize()
	Modules.SplogicModule.Initialize(self)
end

--释放
function Modules.SpMainDungeonModule:Dispose()
	Modules.SplogicModule.Dispose(self)

	self:__StopSlowMirror()

	if self.__do_battle_success_anim_timer then
		GlobalTimerQuest:CancelQuest(self.__do_battle_success_anim_timer)
		self.__do_battle_success_anim_timer = nil
	end
end

--注册事件
function Modules.SpMainDungeonModule:RegistAllEvents()
	--英雄移除
	self:BindGlobalEvent(Macros.Event.SceneObj.HERO_VO_REMOVE, function(_, obj_id)
		self:__CheckAllHeroDead()
		self:__CheckEnd()
	end)

	--怪物移除
	self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_VO_REMOVE, function(_, obj_id, vo)
		self:__HandleMonsterVORemove(vo)
	end)

	--英雄死亡
	self:BindGlobalEvent(Macros.Event.SceneObj.HERO_DEAD, function(_, obj_id)
		self:__CheckAllHeroDead()
	end)

	--怪物死亡
	self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_DEAD, function(_, obj_id)
		self:__CheckMonsterDead(obj_id)
	end)

	--对象血量变化
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_HP_CHANGE, function(_, obj_id, cur_hp, change_hp)
		self:__ObjHPChange(obj_id, cur_hp, change_hp)		
	end)	

	--怒气/合击技能释放
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE, function(_, obj_id)		
		self.__is_anger_or_joint_skill_releasing = true
		self:__ObjAngerOrJointSkillReleased(obj_id)
	end)

	--怒气/合击技能释放结束
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE_END, function(_, obj_id)		
		self.__is_anger_or_joint_skill_releasing = false
		self:__ObjAngerOrJointSkillReleasedEnd(obj_id)
	end)	

	--监听特殊指引开启事件
	self:BindGlobalEvent(Macros.Event.LeadModule.START_SP, function(_, lead_id)
		--战斗加速
		if lead_id == Macros.Game.LeadSpecialID.BATTLE_ACC then
			CallModuleFunc(ModuleType.BATTLE, "ChangeACCIndex")
		end		
	end)

	--精灵技能释放
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE, function(_, obj_id)		
		self.__is_sprite_skill_releasing = true
	end)

	--精灵技能释放结束
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE_END, function(_, obj_id)		
		self.__is_sprite_skill_releasing = false
	end)

	--QTE
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE, function(_, obj_id)		
		self.__is_anger_or_joint_qte = true
		self:__ObjAngerOrJointQTE(obj_id)
	end)

	--QTE结束
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE_END, function(_, obj_id)		
		self.__is_anger_or_joint_qte = false
		self:__ObjAngerOrJointQTEEnd(obj_id)
	end)
end

--注册协议
function Modules.SpMainDungeonModule:RegistAllProtocols()
end

--进入GamePlaying状态
function Modules.SpMainDungeonModule:GamePlayingStartHandle()	
	Modules.SplogicModule.GamePlayingStartHandle(self)

	--初始化镜头
	self:__SetupCamera(true)

	--播放BGM
	self:PlayBGM()

	--开始战役
	self:__StartBattle()

	if not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		--战斗界面
		self:__OpenBattleView()

		--战斗准备特效
		self:__PlayStartBattleEffect()
	end

    --设置运行帧率
    -- CPPSceneManager:SetGameFrame(Macros.Global.FPS_Dungeon)
end

--离开GamePlaying状态
function Modules.SpMainDungeonModule:GamePlayingStopHandle()
	Modules.SplogicModule.GamePlayingStopHandle(self)

	--停止BGM
	self:StopBGM()
end

--加载资源
function Modules.SpMainDungeonModule:LoadRes()
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		self.__is_res_loaded = true
		return
	end

	self.__is_res_loaded = false

	--加载特效
	local function load_effects()
		self.__is_res_loaded = true
	end

	--加载骨骼动画
	local skeleton_path_map = {}
	local function record_skeleton_map(list, is_monster)
		if list then
			local skeleton_path
			for _, obj_info in ipairs(list) do
				skeleton_path = Resource.PathTool.GetModelPathEx(obj_info.info_id, is_monster)
				if skeleton_path then
					skeleton_path_map[skeleton_path] = true
				end
			end
		end
	end
	local hero_list = self:__GetBattleHeroList()
	local monster_list = self:__GetBattleMonsterList()
	local hero_mirror_list = self:__GetBattleHeroMirrorList()
	record_skeleton_map(hero_list)
	record_skeleton_map(hero_mirror_list)
	record_skeleton_map(monster_list, self.__type ~= Macros.Game.FBType.PVP)

	self.__loaded_skeleton_count = 0
	self.__need_load_skeleton_count = CPPGameLib.GetTableLength(skeleton_path_map)	
	local function load_skeletons()
		for skeleton_path, _ in pairs(skeleton_path_map) do
			Resource.LoadTool.LoadSkeletonAsync(skeleton_path, function(path)
				self.__loaded_skeleton_count = self.__loaded_skeleton_count + 1
				if self.__loaded_skeleton_count == self.__need_load_skeleton_count then
					load_effects()
				end
			end)		
		end		
	end

	--加载贴图集
	local texture_list = 
	{
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.BATTLE),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.BATTLE, true),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.BATTLE_COMMON),		
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.BATTLE_COMMON, true),		
	}
	Modules.SplogicModulesMgr.LoadRes(self, texture_list, nil, function()
		load_skeletons()
	end)
end

function Modules.SpMainDungeonModule:GetResLoadInfo()
	local res_load_count, res_need_load_count = Modules.SplogicModule.GetResLoadInfo(self)
	return res_load_count + self.__loaded_skeleton_count, res_need_load_count + self.__need_load_skeleton_count
end

function Modules.SpMainDungeonModule:Update(now_time, elapse_time)
	self:__CalcBoundaryObj()
	self:__HandleAttack()
	self:__HandleCollision()
	self:__HandleCheckBuff()
	self:__HandleBeAttacked()
	self:__HandleHPBarPosition(now_time)

	self:__DoExitFrmLastBattle()
	self:__CheckEnterRegionInBirth()
	self:__CheckEndDelay(elapse_time)

	self:__CheckBossWarning(elapse_time)
	self:__CheckBossMessageShow(elapse_time)

	self:__DoCameraAnim(elapse_time)
end

function Modules.SpMainDungeonModule:Pause(for_end, for_world_star)
	Modules.SplogicModule.Pause(self, for_end, for_world_star)
end

function Modules.SpMainDungeonModule:Resume()
	Modules.SplogicModule.Resume(self)
end

--更新剩余时间
function Modules.SpMainDungeonModule:SetRemainTime(var)
	Modules.SplogicModule.SetRemainTime(self, var)

	if not self.__is_started then
		return
	end

	if self.__slow_mirror_timer then
		return
	end

	if self.__is_checked_end then
		return
	end

	if self.__splogic_status ~= Modules.SplogicStatus.RUN then
		return
	end

	if self.__remain_time <= 0 then
		self.__is_checked_end = true
		self:__StopSlowMirror()
		self:BattleEndReq(false, 0)
		return
	end
end

--战斗成功处理
function Modules.SpMainDungeonModule:BattleSuccessHandle(callback)
	local handle_callback

    local delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()    	
    	handle_callback()
    end), 2)  	

    handle_callback = function(clear_timer)
    	if delay_timer then
    		if clear_timer then
    			GlobalTimerQuest:CancelQuest(delay_timer)
				LogTool.LogInfo("[Modules.SpMainDungeonModule:BattleSuccessHandle] handle_callback by sp_move finished")
    		else
				LogTool.LogInfo("[Modules.SpMainDungeonModule:BattleSuccessHandle] handle_callback by delay_timer")
    		end
	    	delay_timer = nil
	        if callback then
	            callback()
	        end
	    end
    end

    --按战斗排序移动到胜利位置并播放胜利动作
    local win_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetWinHeroList")

    local hero
    local move_finished_num = 0
    local hero_move_end_callback = function(obj_id)
        move_finished_num = move_finished_num + 1

        --设置胜利朝向
        hero = GlobalScene:GetObj(obj_id)
        if hero then
	        hero:PauseAI()
	        hero:SetDir(MathTool.HORZ_RIGHT)
	    end

        if move_finished_num >= #win_hero_list then
        	handle_callback(true)    	

		    --所有英雄播放胜利动作
        	self:__DoBattleSuccessAnim()
        end
    end

	local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

    --移动到胜利位置
    local hero_win_pos, hero_win_move_speed
    local win_space_x = battle_region.size.width / (#win_hero_list + 1)
    local win_pos_x = battle_region.org.x
    for _, vo in ipairs(win_hero_list) do
        hero = GlobalScene:GetObj(vo.obj_id)
    	
        win_pos_x = win_pos_x + win_space_x
        hero_win_pos = Vector2D.New(win_pos_x, hero:GetWorldPosY())
        --加速
        hero_win_move_speed = hero:GetAttr(Macros.Game.BattleProp.SPEED) * ConfigAdapter.Common.GetBattleSuccessSpeedFactor()

        --特殊移动
        hero:DoSpecialMove(hero_win_pos, true, hero_win_move_speed, true, hero_move_end_callback)
    end

    --镜头处理
	local config_camera = ConfigAdapter.Common.GetBattleCameraInfo() 
	local force_data = {}
	force_data.scale = config_camera.init_scale
	force_data.pos = Vector2D.New(battle_region.org.x + battle_region.size.width / 2, battle_region.org.y + battle_region.size.height / 2)
    self:__DoCameraAnim(nil, force_data)    
end

--打开胜利界面
function Modules.SpMainDungeonModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.BATTLE_RESULT_WIN)
end

--战斗失败处理
function Modules.SpMainDungeonModule:BattleFailedHandle(callback)
	Modules.SplogicModule.BattleFailedHandle(self, callback)
end

--打开失败界面
function Modules.SpMainDungeonModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.BATTLE_RESULT_LOSE)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpMainDungeonModule:BattleEndReturnRelevantView(...)
	local params = {...}

	local open_func, real_open_func
	local delete_func = function ()
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", battle_info.chapter_index)
	local is_pass
	if chapter_info and chapter_info.is_history_passed_all and (battle_info.is_first_win ~= Macros.Global.FALSE) then
		is_pass = true

		open_func = function()
		end

		--当前章节已通关，播放通关特效
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.DUNGEON_GAMELEVEL, battle_info.chapter_index or 1,unpack(params) and battle_info.gamelevel_index or nil,true,open_func,nil,unpack(params),delete_func,open_func)	

	else
		open_func = function()
		end

		--当前章节未通关，返回关卡界面
		CallModuleFunc(ModuleType.DUNGEON, "OpenView", BaseViewType.DUNGEON_GAMELEVEL, battle_info.chapter_index or 1,unpack(params) and battle_info.gamelevel_index or nil,nil,open_func,nil,unpack(params),unpack(params) and delete_func or nil,open_func)	
	end

	Modules.SplogicModule.BattleEndReturnRelevantView(self)	
end

local function check_obj_id(obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or not obj:CanDoBeAttacked() then
		return nil
	end
	return obj_id
end

--获取首个敌人
function Modules.SpMainDungeonModule:GetFirstEnemy(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return check_obj_id(self.__monster_first_obj_id)
	else
		return check_obj_id(self.__hero_first_obj_id)
	end
end

--获取最后一个敌人
function Modules.SpMainDungeonModule:GetLastEnemy(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return check_obj_id(self.__monster_last_obj_id)
	else
		return check_obj_id(self.__hero_last_obj_id)
	end
end

--获取敌方VO列表
function Modules.SpMainDungeonModule:GetEnemyList(obj_id, check_can_dobeattacked, skill_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return GlobalVOMgr:GetMonsterList(nil, check_can_dobeattacked, skill_id)
	else
		return GlobalVOMgr:GetHeroList(nil, check_can_dobeattacked, skill_id)
	end	
end

--获取敌方VO表
function Modules.SpMainDungeonModule:GetEnemyMap(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return GlobalVOMgr:GetMonsterMap()
	else
		return GlobalVOMgr:GetHeroMap()
	end	
end

--获取首个己方对象ID
function Modules.SpMainDungeonModule:GetFirstTeammate(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return check_obj_id(self.__hero_first_obj_id)
	else
		return check_obj_id(self.__monster_first_obj_id)
	end
end

--获取最后己方对象ID
function Modules.SpMainDungeonModule:GetLastTeammate(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return check_obj_id(self.__hero_last_obj_id)
	else
		return check_obj_id(self.__monster_last_obj_id)
	end
end

--获取己方VO列表
function Modules.SpMainDungeonModule:GetTeammateList(obj_id, check_can_dobeattacked, skill_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return GlobalVOMgr:GetHeroList(nil, check_can_dobeattacked, skill_id)
	else
		return GlobalVOMgr:GetMonsterList(nil, check_can_dobeattacked, skill_id)
	end	
end

--获取己方VO表
function Modules.SpMainDungeonModule:GetTeammateMap(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if me and me:IsBullet() then
		me = me:GetCreateObj()
	end
	if not me then
		return nil
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return GlobalVOMgr:GetHeroMap()
	else
		return GlobalVOMgr:GetMonsterMap()
	end	
end

--获取首个近战可治疗己方对象ID
function Modules.SpMainDungeonModule:GetFirstPNTeammate(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if not me then
		return
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return self.__hero_first_pn_obj_id 
	else
		return self.__monster_first_pn_obj_id
	end
end

--获取首个近战可攻击敌方对象ID
function Modules.SpMainDungeonModule:GetFirstPNEnemy(obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if not me then
		return
	end

	if me:IsHero(true) or me:IsHeroPVP(true) then
		return self.__monster_first_pn_obj_id
	else
		return self.__hero_first_pn_obj_id
	end
end

--获取己方精灵
function Modules.SpMainDungeonModule:GetTeammateSprite()
    local spirit_vo_map = GlobalVOMgr:GetSpiritMap()
    if spirit_vo_map then
	    local obj_id = next(spirit_vo_map)
	    return GlobalScene:GetObj(obj_id)	
	end
end

--添加待攻击近战碰撞对记录
function Modules.SpMainDungeonModule:AddToAttackPNObjsRecord(obj_id, attacker_obj_id, skill_id, skill_lev)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return
	end

	if not obj:IsHero() and not obj:IsMonster() and not obj:IsHeroPVP() and not obj:IsMonsterPVP() then
		--只记录英雄和怪物近战碰撞对
		return
	end

	if obj:IsForbidMove() or obj:IsForbidAttack() then
		--不记录处于石化、冰冻、眩晕的对象
		return
	end

	if obj:IsInUnInterruptableAttack() then
		--不记录处于不可打断攻击状态的对象
		return
	end

	local obj_vo = obj:GetVO()
	if obj_vo.immune_ctrl_skill or obj_vo.immune_ctrl_skill2 then
		--不记录BOSS怪(日常副本、世界BOSS...)
		return
	end

	--若只有一个敌人且敌人靠近边界，则过滤
	local teammate_list = self:GetTeammateList(obj_id)
	if #teammate_list <= 1 then
		local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
		local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
		local beat_back_dis = Modules.BattleFormula:CalcSkillBeatback(config_skill_lev and config_skill_lev.beat_back or 0, attacker_obj_id, obj_id)
		local obj_dir = obj:GetDir()
		local obj_pos = obj:GetWorldPosVec()
		if obj_dir == MathTool.HORZ_RIGHT then
			if battle_region.org.x + beat_back_dis > obj_pos.x then
				return
			end

		elseif obj_dir == MathTool.HORZ_LEFT then
			if battle_region.org.x + battle_region.size.width - 1 - beat_back_dis < obj_pos.x then
				return
			end
		end
	end

	self.__to_attack_pn_obj_map = self.__to_attack_pn_obj_map or {}
	self.__to_attack_pn_obj_map[obj_id] = Macros.Global.NowTime
end

--移除待攻击近战碰撞对记录
function Modules.SpMainDungeonModule:DelToAttackPNObjsRecord(obj_id, force)
	if not self.__to_attack_pn_obj_map then
		return
	end

	local record_time = self.__to_attack_pn_obj_map[obj_id]
	if not record_time then
		return
	end
	if force or (record_time + 1 / CPPSceneManager:GetGameFrame() <= Macros.Global.NowTime) then
		self.__to_attack_pn_obj_map[obj_id] = nil
	end
end

--记录本帧中的攻击对象(统一触发攻击逻辑)
function Modules.SpMainDungeonModule:RecordAttackObjCurFrame(obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return
	end

	self.__attack_map = self.__attack_map or {}
	self.__attack_map[obj_id] = obj_id
end

--记录本帧中的受击对象(统一触发受击逻辑)
function Modules.SpMainDungeonModule:RecordBeattackedObjCurFrame(obj_id, attacker_obj_id, skill_id, skill_lev, damage_time_index)	
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return
	end

	local attacker_obj = GlobalScene:GetObj(attacker_obj_id)
	if not attacker_obj then
		return
	end

	local info = {}
	info.obj_id = obj_id
	info.attacker_obj_id = attacker_obj_id
	info.skill_id = skill_id
	info.skill_lev = skill_lev
	info.damage_time_index = damage_time_index

	--受击记录表
	self.__beattacked_record_map = self.__beattacked_record_map or {}
	self.__beattacked_record_map[obj_id] = true

	if self.__handling_beattacked then
		--统一触发受击逻辑中，记录到受击信息缓存表
		self.__beattacked_info_cache_list = self.__beattacked_info_cache_list or {}
		table.insert(self.__beattacked_info_cache_list, info)
	else
		--记录到受击信息表
		self.__beattacked_info_list = self.__beattacked_info_list or {}
		table.insert(self.__beattacked_info_list, info)
	end
end

--记录本帧中的碰撞对象(统一触发碰撞逻辑)
function Modules.SpMainDungeonModule:RecordCollisionObjCurFrame(obj_id, tgt_id)
	if obj_id >= Scene.MaxObjNum then
		LogTool.LogError("[Modules.SpMainDungeonModule:RecordCollisionObjCurFrame] obj_id溢出上限")
		return
	end
	if tgt_id >= Scene.MaxObjNum then
		LogTool.LogError("[Modules.SpMainDungeonModule:RecordCollisionObjCurFrame] tgt_id溢出上限")
		return
	end

	local id_low = math.min(obj_id, tgt_id)
	local id_high = math.max(obj_id, tgt_id)
	local id_key = id_high*Scene.MaxObjNum + id_low
	self.__collision_map = self.__collision_map or {}
	self.__collision_map[id_key] = true	
end

--记录本帧中的BUFF检测(统一触发BUFF)
function Modules.SpMainDungeonModule:RecordCheckBuffCurFrame(obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)	
	if self.__checking_buff then
		--BUFF检测中，先缓存起来
		local info = {}
		info.obj_id = obj_id
		info.skill_id = skill_id
		info.by_skill_tgt_logic = by_skill_tgt_logic
		info.bless_sprite_buff_data = bless_sprite_buff_data
		info.external_data = external_data
		self.__checking_buff_cache_list = self.__checking_buff_cache_list or {}
		table.insert(self.__checking_buff_cache_list, info)
		return
	end

	self.__check_buff_map = self.__check_buff_map or {}
	self.__check_buff_map[obj_id] = self.__check_buff_map[obj_id] or {}

	local check_buff_info = {}
	check_buff_info.skill_id = skill_id
	check_buff_info.by_skill_tgt_logic = by_skill_tgt_logic
	check_buff_info.bless_sprite_buff_data = bless_sprite_buff_data
	check_buff_info.external_data = external_data

	table.insert(self.__check_buff_map[obj_id], check_buff_info)
end

--边界处理(以屏幕可视区域为边界)
function Modules.SpMainDungeonModule:BoundTest(pos, is_world_pos, obj_id)
	local me = GlobalScene:GetObj(obj_id)
	if not me then
		return
	end

	if me:IsBullet() then
		--过滤弹道
		return
	end

	local skip_region_bound_test
	local skip_enemy_bound_test
	if me:GetSkipBoundTest() then
		--过滤战役边界检测
		skip_region_bound_test = true

		--过滤敌方边界检测
		skip_enemy_bound_test = true
	end

    local world_pos_x, world_pos_y
    if is_world_pos then
        world_pos_x = pos.x
        world_pos_y = pos.y
    else
        world_pos_x, world_pos_y = GlobalScene:LogicToWorld(pos.x, pos.y)
    end

    local horz_space = ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin()

    --敌方边界检测
    if not skip_enemy_bound_test then
		local first_enemy = GlobalScene:GetObj(self:GetFirstEnemy(obj_id))
		if first_enemy and not (first_enemy:GetSkipBoundTest() or first_enemy:GetInBirth() or first_enemy:IsInAngerOrJointSkill()) then
			local enemy_world_pos = first_enemy:GetWorldPosVec()
			if me:IsHero(true) or me:IsHeroPVP(true) then
				world_pos_x = math.min(world_pos_x, enemy_world_pos.x - horz_space)
			else
				world_pos_x = math.max(world_pos_x, enemy_world_pos.x + horz_space)
			end	
		end    
	end

    --战役边界检测
    if not skip_region_bound_test then
		local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
	    local min_x = battle_region.org.x + horz_space
	    local max_x = battle_region.org.x + battle_region.size.width - 1 - horz_space
	    local min_y = battle_region.org.y
	    local max_y = battle_region.org.y + battle_region.size.height - 1
	    world_pos_x = math.max(min_x, world_pos_x)
	    world_pos_x = math.min(world_pos_x, max_x)
	    world_pos_y = math.max(min_y, world_pos_y)
	    world_pos_y = math.min(world_pos_y, max_y)
	end

    if is_world_pos then
        pos.x = world_pos_x
        pos.y = world_pos_y
    else
        pos.x, pos.y = GlobalScene:WorldToLogic(world_pos_x, world_pos_y)
    end
end

--播放BGM
function Modules.SpMainDungeonModule:PlayBGM()
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)	
	if config_gamelevel and config_gamelevel.bgm_name then
		GlobalSoundMgr:PlayBGM(config_gamelevel.bgm_name, nil, ConfigAdapter.Common.GetDungeonBGMVolume(), true)
	end
end

--请求战斗结束
function Modules.SpMainDungeonModule:BattleEndReq(is_win, star, is_manual_exit)
	CallModuleFunc(ModuleType.BATTLE, "BattleEndReq", is_win, star, is_manual_exit)
end

--是否全体英雄、怪物进入镜头
function Modules.SpMainDungeonModule:IsAllObjsEnterRegion()
	if self.__all_objs_enter_region then
		return true
	end

	if not self.__birth_hero_obj_ids or not self.__birth_monster_obj_ids then
		return false
	end

	local obj
	for _, obj_id in ipairs(self.__birth_hero_obj_ids) do
		obj = GlobalScene:GetObj(obj_id)
		if obj and not obj:GetEnteredBattleRegion() then
			return false
		end
	end
	for _, obj_id in ipairs(self.__birth_monster_obj_ids) do
		obj = GlobalScene:GetObj(obj_id)
		if obj and not obj:GetEnteredBattleRegion() then
			return false
		end
	end	
	return true
end

--是否怒气/合击技能释放中
function Modules.SpMainDungeonModule:IsInAngerOrJointSkill()
    return self.__is_anger_or_joint_skill_releasing or self.__is_sprite_skill_releasing or self.__is_anger_or_joint_qte
end

--是否所有英雄阵亡
function Modules.SpMainDungeonModule:IsAllHeroDead()
	return GlobalVOMgr:CalcTotalHeroHP() <= 0
end

--是否所有怪物阵亡
function Modules.SpMainDungeonModule:IsAllMonsterDead()
	return GlobalVOMgr:CalcTotalMonsterHP() <= 0
end    

--是否已完成BOSS展示
function Modules.SpMainDungeonModule:IsBossMessageShowFinished()
	return not self.__boss_message_info or self.__boss_message_info.is_finished
end

--触发怒气/合击技能
function Modules.SpMainDungeonModule:TriggerAngerOrJointSkill(obj_id, is_plus)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or obj:IsDead() then
		return
	end

	return obj:TriggerAngerOrJointSkill(is_plus)
end

--触发精灵技能
function Modules.SpMainDungeonModule:TriggerSpriteSkill(obj_id, skill_id, world_pos)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or obj:IsDead() then
		return
	end

	obj:DoNull(nil, nil, true)
	obj:DoAttack(skill_id, nil, nil, world_pos)		
end

--动态创建怪物
function Modules.SpMainDungeonModule:CreateMonsterDynamic(monster_id)
end

--处理跳过战斗逻辑
function Modules.SpMainDungeonModule:HandleSkipBattle()
    local acc_factor = 1
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

    --主线、日常、英雄、精英副本 或者 我方战力是敌方战力1.2倍或以上时，直接胜利
    if battle_info.type == Macros.Game.FBType.MAIN_DUNGEON or battle_info.type == Macros.Game.FBType.DAILY_DUNGEON or
    	battle_info.type == Macros.Game.FBType.HERO_DUNGEON or battle_info.type == Macros.Game.FBType.ELITE_DUNGEON or 
    	battle_info.my_battle_power / battle_info.enemy_battle_power >= 1.2 then
		self.__is_all_monster_dead_by_formula = true    		
    	CallModuleFunc(ModuleType.BATTLE, "OneKeyWin")

    --竞技场
    elseif battle_info.type == Macros.Game.FBType.ARENA then
    	self:HandleQuickBattle()

	--其余副本采用加速处理
    else
    	--设置自动战斗
        self.__is_auto_fight_before_skip = CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")
        CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", true)

        --设置战斗加速
        acc_factor = ConfigAdapter.Common.GetBattleSkipACC()
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", acc_factor, true)      
    end

    --暂停镜头
    GlobalScene:PauseCamera()

    --隐藏非UI、非地图层
    local render_group
    for _, render_queue in pairs(Scene.RenderQueue) do
        if not Scene.Scene.IsUIRenderQueue(render_queue) and not GlobalScene:HasMapGroup(render_queue) then
            render_group = GlobalScene:GetRenderGroup(render_queue, false)
            if render_group then            
                render_group:SetVisible(false)
            end
        end
    end

    --展示战斗跳过特效
    if not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
        ActionManager.GetInstance():StopMapBlackingWithLock()

        local anim_info = {}
        anim_info.type = "anim_battle_skip"
        anim_info.effect_list = 
        {
            {res_name = "UI_zhandoutiaoguo2", time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP},
            {res_name = "UI_battleskip_left", type = Effect.EffectType.SKELETON, time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP, bind_event_callback = true},
            {res_name = "UI_battleskip_right", type = Effect.EffectType.SKELETON, time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP}
        }
        ActionManager.GetInstance():StartMapBlackingWithLock(nil, 127, nil, nil, Scene.RenderQueue.GRQ_UI_UP, anim_info)
    end
end

--处理跳过战斗逻辑结束
function Modules.SpMainDungeonModule:HandleSkipBattleEnd()
	--还原自动战斗
    CallModuleFunc(ModuleType.BATTLE, "SetAutoFight", self.__is_auto_fight_before_skip)	

    --还原战斗加速
    local acc_factor = CallModuleFunc(ModuleType.BATTLE, "GetACCFactor")
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", acc_factor, true)

    --恢复镜头
    GlobalScene:ResumeCamera()

    --恢复非UI、非地图层
    local render_group
    for _, render_queue in pairs(Scene.RenderQueue) do
        if not Scene.Scene.IsUIRenderQueue(render_queue) and not GlobalScene:HasMapGroup(render_queue) then
            render_group = GlobalScene:GetRenderGroup(render_queue, false)
            if render_group then            
                render_group:SetVisible(true)
            end
        end
    end

    --移除战斗跳过特效
    if not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
        ActionManager.GetInstance():StopMapBlackingWithLock()
    end
end

--处理快速战斗逻辑
function Modules.SpMainDungeonModule:HandleQuickBattle()
end

--处理进入切场景状态
function Modules.SpMainDungeonModule:HandleEnterSceneChangeState(callback)
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		if callback then callback(true) end
	else
		--打开loading界面
		CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.SCENE_LOADING, function()
			if callback then callback(true) end
		end)		
	end
end

--处理离开切场景状态
function Modules.SpMainDungeonModule:HandleQuitSceneChangeState(callback)
	if not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		--关闭loading界面
		GlobalViewMgr:CloseViews({[BaseViewType.SCENE_LOADING] = true}, nil, true, true)	
	end

	if callback then callback() end
end

function Modules.SpMainDungeonModule:IsPauseWhenLeadOrStory()
	if self:GetStatus() == Modules.SplogicStatus.RUN then
		return true
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpMainDungeonModule:__StartBattle(for_next)
	self.__is_checked_end = false
	self.__is_started = false

	self.__hero_first_obj_id = nil
	self.__hero_last_obj_id = nil
	self.__monster_first_obj_id = nil
	self.__monster_last_obj_id = nil
	if for_next then
		self.__battle_monster_list = nil
		self.__battle_hero_mirror_list = nil
	end

	self.__attack_map = nil
	self.__collision_map = nil
	self.__beattacked_info_list = nil
	self.__beattacked_info_cache_list = nil
	self.__handling_beattacked = false
	self.__check_buff_map = nil
	self.__checking_buff = false
	self.__checking_buff_cache_list = nil
	self.__to_attack_pn_obj_map = nil

	self.__can_check_enter_region = false
	self.__all_heros_enter_region = false
	self.__all_monsters_enter_region = false
	self.__all_objs_enter_region = false

	self.__can_do_camera_anim = false

	--重置怒气/合击技能释放中标记
	self.__is_anger_or_joint_skill_releasing = false

	--重置精灵技能释放中标记
	self.__is_sprite_skill_releasing = false

	--重置QTE释放中标记
	self.__is_anger_or_joint_qte = false

	--创建敌我双方
	self:__CreateObjs()

	--战斗出生
	self:__DoBirth()

	--开始战斗
	CallModuleFunc(ModuleType.BATTLE, "StartBattle")

	--统计初始怒气
	if not for_next then
		GlobalVOMgr:CalcInitAngerRelevant()
	end

	--战役开始多久
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
    self.__battle_pass_time_beg = config_gamelevel.time - self.__remain_time
end

function Modules.SpMainDungeonModule:__GetStartBattleEffect()
	return 100014
end

function Modules.SpMainDungeonModule:__GetStartBattleEffectTime()
	return 1.6
end

--战斗准备特效
function Modules.SpMainDungeonModule:__PlayStartBattleEffect()
	local ready_effect_id = GlobalEffectMgr:CreateScenePosEffect({id = self:__GetStartBattleEffect()}, Scene.RenderQueue.GRQ_UI_UP)
	local ready_effect_root = GlobalEffectMgr:GetEffect(ready_effect_id)
	if ready_effect_root then
		PosTool.Center(ready_effect_root)
	end

    GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
    	if ready_effect_id then
    		GlobalEffectMgr:DelEffect(ready_effect_id)
    	end

    end), self:__GetStartBattleEffectTime())
end

--获取战斗英雄列表
function Modules.SpMainDungeonModule:__GetBattleHeroList()
	if not self.__battle_hero_list then
		self.__battle_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroList", false)
		if #self.__battle_hero_list < 5 then
			local batlle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
			local assist_hero_list = ConfigAdapter.Dungeon.GetAssistBattleHeroList(batlle_info.gamelevel_id)

			--当上阵英雄不足5人，而且本关卡支持协助英雄，则补充协助英雄
			if assist_hero_list and #assist_hero_list > 0 then
				for i = 1, 5 - #self.__battle_hero_list do
					if assist_hero_list[i] then
						table.insert(self.__battle_hero_list, assist_hero_list[i])
					else
						break
					end
				end
			end
		end
		Modules.BattleFormula:SortBattleHeroList(self.__battle_hero_list)		
	end
	return self.__battle_hero_list
end

--获取战斗怪物列表
function Modules.SpMainDungeonModule:__GetBattleMonsterList()
	if not self.__battle_monster_list then
		self.__battle_monster_list = CallModuleFunc(ModuleType.BATTLE, "GetBattleMonsterList", true)
	end
	return self.__battle_monster_list
end

--获取战斗英雄镜像列表
function Modules.SpMainDungeonModule:__GetBattleHeroMirrorList()
	if not self.__battle_hero_mirror_list then
		self.__battle_hero_mirror_list = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroMirrorList", true)
	end
	return self.__battle_hero_mirror_list
end

--创建敌我双方
function Modules.SpMainDungeonModule:__CreateObjs()
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		return
	end

	self:__CreateHeros()
	self:__CreateMonsters()
	self:__CreateSprites()
end

--创建英雄
function Modules.SpMainDungeonModule:__CreateHeros(vo_class, create_func)
	local vo, obj
	vo_class = vo_class or Scene.HeroVO

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.battle_index <= 1 then
		--首场战役，创建我方
		self.__birth_hero_obj_ids = nil
		local hero_list = self:__GetBattleHeroList()
		for i, hero_info in ipairs(hero_list) do
			vo = vo_class.New()
			vo:ParseInfo(hero_info)
			vo.dir = MathTool.HORZ_RIGHT
			vo.pos_index = i
			if create_func then
				create_func(vo, true)
			else
			 	GlobalScene:CreateHero(vo, true)
			end

			self.__birth_hero_obj_ids = self.__birth_hero_obj_ids or {}
			table.insert(self.__birth_hero_obj_ids, vo.obj_id)
		end		
		self.__hero_total_num = #hero_list

	else
		--非首场战役，重置状态
		if self.__birth_hero_obj_ids then
			local hero_obj_ids = {}
			for _, obj_id in ipairs(self.__birth_hero_obj_ids) do
				obj = GlobalScene:GetObj(obj_id)
				if obj and not obj:IsDead() then
					obj.last_wpos_inbirth = nil
					obj:SetEnteredBattleRegion(false)
					table.insert(hero_obj_ids, obj_id)
				end
			end
			self.__birth_hero_obj_ids = hero_obj_ids
		end
	end
end

--创建怪物
function Modules.SpMainDungeonModule:__CreateMonsters(vo_class, create_func)
	local vo, obj
	vo_class = vo_class or Scene.MonsterVO

	--创建敌方
	self.__birth_monster_obj_ids = nil
	self.__monster_info_map = nil

	GlobalVOMgr:ClearExtraMonsterMaxHP()

	local monster_list = self:__GetBattleMonsterList()
	if monster_list then
		--怪物列表
		for _, monster_info in ipairs(monster_list) do
			vo = vo_class.New()
			vo:ParseInfo(monster_info)
			vo.dir = MathTool.HORZ_LEFT

			if vo.hp > 0 then
				obj = create_func and create_func(vo, true) or GlobalScene:CreateMonster(vo, true)
				obj:PauseAI()
				obj:SetVisible(false)

				self.__birth_monster_obj_ids = self.__birth_monster_obj_ids or {}
				table.insert(self.__birth_monster_obj_ids, vo.obj_id)

				self.__monster_info_map = self.__monster_info_map or {}
				self.__monster_info_map[vo.obj_id] = monster_info
			else
				GlobalVOMgr:CalcExtraMonsterMaxHP(vo.max_hp)
			end
			monster_info.damage = 0
		end
	else
		--英雄镜像列表
		local hero_mirror_list = self:__GetBattleHeroMirrorList()
		if hero_mirror_list then
			for _, hero_mirror_info in ipairs(hero_mirror_list) do
				vo = Scene.HeroMirrorVO.New()
				vo:ParseInfo(hero_mirror_info)
				vo.dir = MathTool.HORZ_LEFT

				if vo.hp > 0 then
					obj = GlobalScene:CreateHeroMirror(vo, true)
					obj:PauseAI()
					obj:SetVisible(false)
					
					self.__birth_monster_obj_ids = self.__birth_monster_obj_ids or {}
					table.insert(self.__birth_monster_obj_ids, vo.obj_id)
				else
					GlobalVOMgr:CalcExtraMonsterMaxHP(vo.max_hp)
				end
			end
		end
	end
end

--创建精灵
function Modules.SpMainDungeonModule:__CreateSprites()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", battle_info.sprite_id)
	if not spirit_info then
		return 
	end

	local hero_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.HERO)		
	local camera_bounding = GlobalScene:GetCameraBounding()
	local spirit_birth_pos = Vector2D.New(camera_bounding.org.x + ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), hero_birth_pos_y + Scene.SpiritOffset.y)

	--精灵出场
	if battle_info and battle_info.battle_index == 1 then
		local battle_spirit_info = Modules.BattleFormula:BuildBattleSpiritInfo(spirit_info)
		local vo = Scene.SpiritVO.New()
		vo:ParseInfo(battle_spirit_info)
		vo:ParseSkillList(battle_info.sprite_common_skill_id)
		vo.org_pos_x, vo.org_pos_y = GlobalScene:WorldToLogic(spirit_birth_pos.x, spirit_birth_pos.y)
		local sprite = GlobalScene:CreateSpirit(vo, true, Scene.ObjType.HERO)
		sprite:SetVisible(false)

	--精灵过场
	else
		local spirit = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
		if spirit then
			spirit:SetEnteredBattleRegion(false)
		end
	end
end

--精灵出场
function Modules.SpMainDungeonModule:__DoBirthForSprite()
	local spirit = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if not spirit then
		return
	end

	local camera_bounding = GlobalScene:GetCameraBounding()
	local spirit_birth_pos = Vector2D.New(camera_bounding.org.x + ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), spirit:GetWorldPosY())
	spirit:SetVisible(true)
	spirit:SetSkipBoundTest(true)

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info and battle_info.battle_index == 1 then
		spirit:SetWorldPos(spirit_birth_pos.x + 156, spirit_birth_pos.y)
		spirit:DoBirth()
	else
		spirit:SetWorldPos(spirit_birth_pos.x - 128, spirit_birth_pos.y)
		spirit:DoSpecialMove(spirit_birth_pos, true, nil, true, function()
			spirit:ResumeAI()
			spirit:SetEnteredBattleRegion(true)
		end)
	end
end

--战斗出生
function Modules.SpMainDungeonModule:__DoBirth()
	--出生处理
	--近战职业靠近中心，远程职业靠近两边
	--双方排头兵与中心的距离一致
	local function handle_birth()
		--镜头包围盒
		local camera_bounding = GlobalScene:GetCameraBounding()
		local camera_middle_x = camera_bounding.org.x + camera_bounding.size.width / 2

		--出生点与边界的最小间隔
		local birth_pos_boundary_margin = ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin()
		--同职业队友出生点间隔
		local same_profession_teammate_space = ConfigAdapter.Common.GetBattleSameProfessionTeammateSpace()
		--不同职业队友出生点间隔加成
		local diff_profession_teammate_space_add = ConfigAdapter.Common.GetBattleDiffProfessionTeammateSpaceAdd()
		--排头兵与边界的间隔
		local first_birth_pos_boundary_margin = birth_pos_boundary_margin + (Modules.BattleModule.MaxTeammateNum - 1) * same_profession_teammate_space + diff_profession_teammate_space_add

		local function handle_birth_real(birth_type, obj_ids, obj_birth_org, factor)
			--obj_ids的顺序是先远程后近战
			local obj, obj_vo, obj_birth_pos, obj_diff_profession_teammate_space_add, y_offset
			for i, obj_id in ipairs(obj_ids) do
				obj = GlobalScene:GetObj(obj_id)
				if obj then
					obj_vo = obj:GetVO()

					--不同职业队友出生点间隔加成
					obj_diff_profession_teammate_space_add = not obj_vo:IsProfessionNear() and diff_profession_teammate_space_add or 0

					--战斗中的英雄间出生点Y坐标偏移
					y_offset = ((#obj_ids - i + 1) % 3) * ConfigAdapter.Common.GetHeroYOffsetForBattle()

					--出生目标点(从边界往中心放置)
					local trick_num
					if obj_vo:IsProfessionNear() then
						trick_num = #obj_ids
					else
						trick_num = Modules.BattleModule.MaxTeammateNum
					end
					obj_birth_pos = Vector2D.New(obj_birth_org.x + ((trick_num - i) * same_profession_teammate_space + obj_diff_profession_teammate_space_add) * factor, obj_birth_org.y - y_offset)

					if factor == -1 then
						--英雄出生点边界处理
						obj_birth_pos.x = math.max(obj_birth_pos.x, camera_bounding.org.x + birth_pos_boundary_margin)
						obj_birth_pos.x = math.min(obj_birth_pos.x, camera_middle_x - birth_pos_boundary_margin)
					else
						--怪物出生点边界处理
						obj_birth_pos.x = math.max(obj_birth_pos.x, camera_middle_x + birth_pos_boundary_margin)
						obj_birth_pos.x = math.min(obj_birth_pos.x, camera_bounding.org.x + camera_bounding.size.width - 1)
					end

					--出生起始点(用于跑步进入)
					obj_birth_src_pos = Vector2D.New(obj_birth_org.x + ((#obj_ids - i) * same_profession_teammate_space + obj_diff_profession_teammate_space_add) * factor, obj_birth_org.y - y_offset)

					--出生
					obj:DoBirth(birth_type, Vector2D.New(obj_birth_src_pos.x + first_birth_pos_boundary_margin * factor, obj_birth_pos.y), obj_birth_pos)
				end
			end
		end

		--我方出生
		if self.__birth_hero_obj_ids then
			local birth_type = CallModuleFunc(ModuleType.BATTLE, "GetBirthType", Scene.ObjType.HERO)

			--英雄出生X坐标
			local hero_birth_pos_x = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosX", Scene.ObjType.HERO)	

			--英雄出生Y坐标
			local hero_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.HERO)	

			--排头英雄出生点
			local hero_birth_org = Vector2D.New(camera_bounding.org.x + first_birth_pos_boundary_margin + hero_birth_pos_x, hero_birth_pos_y)
			
			handle_birth_real(birth_type, self.__birth_hero_obj_ids, hero_birth_org, -1)
		end

		--敌方出生
		if self.__birth_monster_obj_ids then
			birth_type = CallModuleFunc(ModuleType.BATTLE, "GetBirthType", Scene.ObjType.MONSTER)

			--最后排的怪物与边界的间隔
			local last_monster_interval = 0
			local last_monster = GlobalScene:GetObj(self.__birth_monster_obj_ids[#self.__birth_monster_obj_ids])
			if last_monster then
				local last_monster_vo = last_monster:GetVO()
				last_monster_interval = last_monster_vo.boundary_interval or 0
			end

			--怪物出生X坐标
			local monster_birth_pos_x = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosX", Scene.ObjType.MONSTER)	

			--怪物出生Y坐标
			local monster_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.MONSTER)	

			--排头怪物出生点
			local monster_birth_org = Vector2D.New(camera_bounding.org.x + camera_bounding.size.width - 1 - first_birth_pos_boundary_margin - last_monster_interval + monster_birth_pos_x, monster_birth_pos_y)

			handle_birth_real(birth_type, self.__birth_monster_obj_ids, monster_birth_org, 1)
		end

		--精灵出生
		local spirit = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
		if spirit then
			spirit:SetVisible(false)
		end

		--战斗出生完成
		self:__DoBirthFinished()
	end

	--镜头处理
	local function handle_camera()
		self:__SetupCamera()
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	--非首场战役
	if battle_info.battle_index > 1 and not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		--英雄、精灵离开上一个战役区域
	    self.__do_exit_frm_last_battle = true

		--地图层淡出
		ActionManager.GetInstance():StartMapBlackingWithLock(nil, 0xff, 2, function()
			handle_camera()
            handle_birth()

			--地图层淡入
			ActionManager.GetInstance():StopMapBlackingWithLock(2, function()
			end)
		end, Scene.RenderQueue.GRQ_UI_UP)

	--首场战役
	else
		handle_camera()
		handle_birth()
	end
end

--战斗出生完成
function Modules.SpMainDungeonModule:__DoBirthFinished()
	--强制执行场景对象深度排序
    GlobalScene:SetRenderGroupSortInterval()
	GlobalScene:SortRenderGroupChildsForce()

	--BOSS预警
	self:__DoBossWarning()		

	--Boss展示信息面板
	self:__DoBossMessageShow()

	self.__is_started = true

	self.__can_check_enter_region = true		

	--开始战斗事件
	CallModuleFunc(ModuleType.BATTLE, "StartBattleEvent")
end

--计算双方边界兵
function Modules.SpMainDungeonModule:__CalcBoundaryObj()
	self.__hero_first_obj_id = nil
	self.__hero_last_obj_id = nil
	self.__hero_first_pn_obj_id = nil

	self.__monster_first_obj_id = nil
	self.__monster_last_obj_id = nil
	self.__monster_first_pn_obj_id = nil

	local obj, obj_pos_x

	--英雄
	local first_x = MathTool.MIN_NUM
	local first_battle_x = MathTool.MIN_NUM
	local last_x = MathTool.MAX_NUM
	local hero_vo_list = GlobalVOMgr:GetHeroList()
	for _, vo in pairs(hero_vo_list) do
		obj = GlobalScene:GetObj(vo.obj_id)
		if obj and not obj:IsDead() then
			obj_pos_x = obj:GetWorldPos()

			--首个可受击对象
			-- if obj:CanDoBeAttacked() then
				if obj_pos_x > first_x then
					first_x = obj_pos_x
					self.__hero_first_obj_id = vo.obj_id
				end
				if obj_pos_x < last_x then
					last_x = obj_pos_x
					self.__hero_last_obj_id = vo.obj_id
				end
			-- end

			--首个可受击近战对象
			if obj:CanDoBeAttacked() and 
				(not self.__to_attack_pn_obj_map or not self.__to_attack_pn_obj_map[vo.obj_id]) then
				if obj_pos_x > first_battle_x then
					first_battle_x = obj_pos_x
					self.__hero_first_pn_obj_id = vo.obj_id
				end
			end
		end
	end

	--怪物
	first_x = MathTool.MAX_NUM
	first_battle_x = MathTool.MAX_NUM
	last_x = MathTool.MIN_NUM
	local monster_vo_list = GlobalVOMgr:GetMonsterList(true)
	for _, vo in pairs(monster_vo_list) do
		obj = GlobalScene:GetObj(vo.obj_id)
		if obj and not obj:IsDead() then
			obj_pos_x = obj:GetWorldPos()

			--首个可受击对象
			-- if obj:CanDoBeAttacked() then
				if obj_pos_x < first_x then
					first_x = obj_pos_x
					self.__monster_first_obj_id = vo.obj_id
				end
				if obj_pos_x > last_x then
					last_x = obj_pos_x
					self.__monster_last_obj_id = vo.obj_id
				end
			-- end

			--首个可受击近战对象
			if obj:CanDoBeAttacked() and 
				(obj:IsInUnInterruptableAttack() or
				(not self.__to_attack_pn_obj_map or not self.__to_attack_pn_obj_map[vo.obj_id])) then
				if obj_pos_x < first_battle_x then
					first_battle_x = obj_pos_x
					self.__monster_first_pn_obj_id = vo.obj_id
				end
			end
		end
	end
end

--统一处理攻击逻辑
function Modules.SpMainDungeonModule:__HandleAttack()
	if self.__attack_map then
		-- print(8, "__HandleAttack enter")

		local obj
		for _, obj_id in pairs(self.__attack_map) do
			-- print(8, "handle attack ", obj_id)
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				obj:AttackTargets()
			end
		end

		-- print(8, "__HandleAttack exit")

		self.__attack_map = nil
	end
end

--统一处理受击逻辑
function Modules.SpMainDungeonModule:__HandleBeAttacked()
	if self.__beattacked_info_list then
		-- print(8, "__HandleBeAttacked enter ", Macros.Global.NowTimeMilli, #self.__beattacked_info_list)

		--处理受击
		self.__handling_beattacked = true
		Skill.SkillLogic:HandleBeAttacked(self.__beattacked_info_list)
		self.__handling_beattacked = false

		--根据受击信息缓存表，重置受击信息表
		self.__beattacked_info_list = self.__beattacked_info_cache_list
		self.__beattacked_record_map = {}
		if self.__beattacked_info_cache_list then
			for _, info in ipairs(self.__beattacked_info_cache_list) do
				self.__beattacked_record_map[info.obj_id] = true
			end
			self.__beattacked_info_cache_list = nil
		end

		-- print(8, "__HandleBeAttacked exit")		
	end
end

--统一处理碰撞逻辑
function Modules.SpMainDungeonModule:__HandleCollision()
	if self.__collision_map then
		-- print(8, "__HandleCollision enter ", Macros.Global.NowTimeMilli)
		local id1, id2
		local obj1, obj2
		local effect_pos_x, effect_pos_y 
		for id_key, _ in pairs(self.__collision_map) do
			id1 = math.floor(id_key/Scene.MaxObjNum)
			id2 = id_key%Scene.MaxObjNum
			obj1 = GlobalScene:GetObj(id1)
			obj2 = GlobalScene:GetObj(id2)

			--碰撞击退
			if obj1 and (not self.__beattacked_record_map or not self.__beattacked_record_map[id1]) then
				obj1:DoRepel()
			end
			if obj2 and (not self.__beattacked_record_map or not self.__beattacked_record_map[id2]) then
				obj2:DoRepel()
			end

			--碰撞特效
			if obj1 and obj2 then
				effect_pos_x = (obj1:GetPixelX() + obj2:GetPixelX()) / 2
				effect_pos_y = (obj1:GetPixelY() + obj2:GetPixelY()) / 2 + 50
				GlobalEffectMgr:CreateScenePosEffect({id = ConfigAdapter.Common.GetCollisionEffectID(), loop = false, pos_x = effect_pos_x, pos_y = effect_pos_y}, Scene.RenderQueue.GRQ_AIR)		
			end
		end
		-- print(8, "__HandleCollision exit")
		self.__collision_map = nil
	end
end

--统一处理BUFF检测
function Modules.SpMainDungeonModule:__HandleCheckBuff()
	if self.__check_buff_map then
		self.__checking_buff = true

		for obj_id, check_buff_list in pairs(self.__check_buff_map) do
			for _, check_buff_info in ipairs(check_buff_list) do
				Skill.SkillLogic:CheckBuff(obj_id, check_buff_info.skill_id, check_buff_info.by_skill_tgt_logic, check_buff_info.bless_sprite_buff_data, check_buff_info.external_data)
			end
		end

		self.__check_buff_map = nil
		self.__checking_buff = false		
	end

	if self.__checking_buff_cache_list then
		for _, cache_info in ipairs(self.__checking_buff_cache_list) do
			self:RecordCheckBuffCurFrame(
				cache_info.obj_id, cache_info.skill_id, 
				cache_info.by_skill_tgt_logic, cache_info.bless_sprite_buff_data, cache_info.external_data)
		end
		self.__checking_buff_cache_list = nil
	end
end

--处理头顶血条位置
function Modules.SpMainDungeonModule:__HandleHPBarPosition(now_time)
	self.__hp_bar_update_time = self.__hp_bar_update_time or 0
	if now_time - self.__hp_bar_update_time < 0.3 then
		return
	end
	self.__hp_bar_update_time = now_time

	local hero_vo_list = GlobalVOMgr:GetHeroList()
	local monster_vo_list = GlobalVOMgr:GetMonsterList()

	--X轴从小到大排序
	local function sort_func(vo1, vo2)
		local obj1 = GlobalScene:GetObj(vo1.obj_id)
		local obj2 = GlobalScene:GetObj(vo2.obj_id)
		if not obj1 then
			return false
		end
		if not obj2 then
			return true
		end
		return obj1:GetWorldPosX() < obj2:GetWorldPosX()
	end
	table.sort(hero_vo_list, sort_func)
	table.sort(monster_vo_list, sort_func)

	--生成组列表
	local function generate_group_list(list)
		local obj
		local group_objs
		local group_list = {}
		for _, vo in ipairs(list) do
			group_objs = group_list[#group_list]
			if not group_objs then
				--插入到新的组
				table.insert(group_list, {vo.obj_id})
			else
				obj = GlobalScene:GetObj(group_objs[#group_objs])
				new_obj = GlobalScene:GetObj(vo.obj_id)
				if obj and new_obj and (new_obj:GetWorldPosX() - obj:GetWorldPosX() <= 48) then
					--插入到已有组
					table.insert(group_objs, vo.obj_id)
				else
					--插入到新的组
					table.insert(group_list, {vo.obj_id})					
				end
			end
		end
		return group_list
	end

	--处理英雄血条
	local index
	local group_objs, obj
	local hero_group_list = generate_group_list(hero_vo_list)
	for group_index = #hero_group_list, 1, -1 do
		index = 0
		group_objs = hero_group_list[group_index]
		for obj_index = #group_objs, 1, -1 do
			obj = GlobalScene:GetObj(group_objs[obj_index])
			if obj then
				obj:ChangeHpBarPosition(0, index * 20)
				index = index + 1
			end
		end
	end

	--处理怪物血条
	local monster_group_list = generate_group_list(monster_vo_list)
	for group_index, group_objs in ipairs(monster_group_list) do
		index = 0
		for obj_index, obj_id in ipairs(group_objs) do
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				obj:ChangeHpBarPosition(0, index * 20)
				index = index + 1
			end
		end
	end	
end

--生成掉落物信息
function Modules.SpMainDungeonModule:__GenerateDropInfo(obj_id)
	return CallModuleFunc(ModuleType.BATTLE, "GenerateDropInfo", obj_id)
end

--检测掉落物
function Modules.SpMainDungeonModule:__CheckDrop(obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return 
	end
	local obj_vo = obj:GetVO()
	local obj_logic_x, obj_logic_y = obj:GetLogicPos()

	local drop_info = self:__GenerateDropInfo(obj_id)
	if drop_info then
		local vo = Scene.DropBoxVO.New()
		vo.org_pos_x = obj_logic_x
		vo.org_pos_y = drop_info.is_world_star and obj_logic_y - 3 or obj_logic_y + 1
		vo.drop_info = drop_info
		GlobalScene:CreateDropBox(vo)
	end
end

--检测慢镜
function Modules.SpMainDungeonModule:__CheckSlowMirror()
	if not self.__is_started then
		return
	end

	if self.__has_slow_mirror then
		return
	end

	if self.__is_checked_end then
		return
	end

	if self.__splogic_status ~= Modules.SplogicStatus.RUN then
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if not battle_info or battle_info.battle_index < battle_info.battle_num then
		return
	end

	local have_hero = not self:IsAllHeroDead()
	local have_monster = not self:IsAllMonsterDead()	
	if not have_monster or not have_hero then
		if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") or CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
			--检测结束
			self:__CheckEnd()
			return
		end		

		local slow_tgt_factor = CallModuleFunc(ModuleType.BATTLE, "GetACCFactor")
		local slow_time = ConfigAdapter.Common.GetSlowMirrorTime()
		local slow_step_time = 0.1
		local slow_factor = ConfigAdapter.Common.GetSlowMirrorInitFactor()
		local slow_step_factor = (slow_tgt_factor - slow_factor) / (slow_time / slow_step_time)
		local function slow_step_func()
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", slow_factor)
			slow_factor = slow_factor + slow_step_factor
			if slow_factor > slow_tgt_factor then
				--停止慢镜
				self:__StopSlowMirror()

				--检测结束
				self:__CheckEnd()
			end
		end
		self.__has_slow_mirror = true
		self.__slow_mirror_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(slow_step_func), slow_step_time, -1)

		slow_step_func()
	end
end

--停止慢镜
function Modules.SpMainDungeonModule:__StopSlowMirror()
	if self.__slow_mirror_timer then
		GlobalTimerQuest:CancelQuest(self.__slow_mirror_timer)
		self.__slow_mirror_timer = nil

		local factor = CallModuleFunc(ModuleType.BATTLE, "GetACCFactor")
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", factor)
	end
end

--血量变化
function Modules.SpMainDungeonModule:__ObjHPChange(obj_id, cur_hp, change_hp)
	local monster_info = self.__monster_info_map and self.__monster_info_map[obj_id] or nil
	if monster_info then
		monster_info.damage = math.max(0, monster_info.damage - change_hp)
		if change_hp < 0 then
			monster_info.total_damage = monster_info.total_damage or 0
			monster_info.total_damage = monster_info.total_damage - change_hp
		end
	end
end

--检测所有英雄死亡
function Modules.SpMainDungeonModule:__CheckAllHeroDead()
	if not self:IsAllHeroDead() then
		return
	end

	self:__CheckSlowMirror()	

	--所有英雄死亡
	self:FireNextFrame(Macros.Event.BattleModule.BATTLE_ALL_HERO_DEAD)
end

--检测怪物死亡
function Modules.SpMainDungeonModule:__CheckMonsterDead(obj_id)
	self:__CheckAllMonsterDead()
	self:__CheckDrop(obj_id)
end

--检测所有怪物死亡
function Modules.SpMainDungeonModule:__CheckAllMonsterDead()
	if not self:IsAllMonsterDead() then
		return
	end

	self:__CheckSlowMirror()		

	--所有怪物死亡
	self:FireNextFrame(Macros.Event.BattleModule.BATTLE_ALL_MONSTER_DEAD)
end

--处理怪物移除
function Modules.SpMainDungeonModule:__HandleMonsterVORemove(vo)
	self:__CheckAllMonsterDead()
	self:__CheckEnd()
end

--检测结束
function Modules.SpMainDungeonModule:__CheckEnd()
	if not self.__is_started then
		return
	end

	if self.__slow_mirror_timer then
		return
	end

	if self.__is_checked_end then
		return
	end

	if self.__splogic_status ~= Modules.SplogicStatus.RUN then
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	if not battle_info then
		return
	end

	self:__CheckEndReal()
end

--检测结束
function Modules.SpMainDungeonModule:__CheckEndReal()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	local have_hero = not self:IsAllHeroDead()
	local have_monster = not self:IsAllMonsterDead()
	if not have_hero or not have_monster then
		self.__is_checked_end = true

		--计算战斗耗时
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
		battle_info.cost_time = config_gamelevel.time - self.__remain_time

		if not have_hero then
			--英雄全部阵亡，战斗失败
			self:BattleEndReq(false, 0)
		else
			--怪物全部阵亡
			self:__HandleEndForAllMonsterDead()
		end
	end
end

--检测结束
function Modules.SpMainDungeonModule:__CheckEndDelay(elapse_time)
	if not self.__is_started then
		return
	end

	if self.__is_checked_end then
		return
	end

	if self.__slow_mirror_timer then
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if not battle_info or battle_info.battle_index < battle_info.battle_num then
		return
	end

	local have_hero = not self:IsAllHeroDead()
	local have_monster = not self:IsAllMonsterDead()
	if have_hero and have_monster then
		return
	end

	self.__check_end_delay_pass_time = self.__check_end_delay_pass_time or 0
	self.__check_end_delay_pass_time = self.__check_end_delay_pass_time + elapse_time
	self.__check_end_delay_time = self.__check_end_delay_time or 2
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		self.__check_end_delay_time = 0.8
	end
	if self.__check_end_delay_pass_time >= self.__check_end_delay_time then
		self:__CheckSlowMirror()
	end
end

--处理结束(击杀所有怪物)
function Modules.SpMainDungeonModule:__HandleEndForAllMonsterDead()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	--世界之心获得效果完成回调
	local function handle_func()
		--指引或剧情完成回调
		local function handle_funcII()
			self:__HandleEndForAllMonsterDeadReal()
		end

		--检测新手指引
		if CallModuleFunc(ModuleType.LEAD, "HandleAllMonsterDead", battle_info.gamelevel_id, battle_info.battle_index, handle_funcII) then
			return
		end
		--检测剧情
		if CallModuleFunc(ModuleType.STORY, "HandleAllMonsterDead", battle_info.gamelevel_id, battle_info.battle_index, handle_funcII) then
			return
		end
		handle_funcII()
	end

	--结束战斗跳过
	CallModuleFunc(ModuleType.BATTLE, "StopSkip")

	--主线副本每章节最后的关卡首次通关时，展示世界之心获得效果
	if self.__type == Macros.Game.FBType.MAIN_DUNGEON then
		local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(battle_info.chapter_index)
		if config_chapter.show_world_star == Macros.Global.TRUE and 
			battle_info.no_passed_flag and battle_info.gamelevel_index >= #config_chapter.gamelevel_list and battle_info.battle_index >= battle_info.battle_num then

			local config_world_star_story = CPPGameLib.GetConfig("WorldStarStory", battle_info.chapter_index)
			local in_world_star_effect = ActionManager:GetInstance():StartWorldStarEffect(config_world_star_story and config_world_star_story.story_index, function()
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ResumeEx")
				handle_func()
			end) 

			if in_world_star_effect then
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "PauseEx", false, true)
			else
				handle_func()
			end
		else
			handle_func()
		end
	else
		handle_func()
	end
end

--处理结束(击杀所有怪物)
function Modules.SpMainDungeonModule:__HandleEndForAllMonsterDeadReal()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")	
	if battle_info.battle_index >= battle_info.battle_num then
		--最后一场战役，战斗胜利
		local star = self:__CalcResultStar()
		self:BattleEndReq(true, star)
	else
		--切换到下一波
		CallModuleFunc(ModuleType.BATTLE, "ChangeBattle")
		self:__StartBattle(true)
	end
end

--BOSS预警特效
function Modules.SpMainDungeonModule:__DoBossWarning()
	self.__boss_warning_info = nil

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_battle = CPPGameLib.GetConfig("Battle", battle_info.battle_id, false)
	if config_battle and config_battle.boss_warning_time then
		self.__boss_warning_info = {}
		self.__boss_warning_info.elapse_time = 0
		self.__boss_warning_info.time = config_battle.boss_warning_time
	end
end
function Modules.SpMainDungeonModule:__CheckBossWarning(elapse_time)
	if not self.__boss_warning_info then
		return
	end

	self.__boss_warning_info.elapse_time = self.__boss_warning_info.elapse_time + elapse_time
	if self.__boss_warning_info.elapse_time >= self.__boss_warning_info.time then
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			local boss_warning_effect_id = GlobalEffectMgr:CreateScenePosEffect({id = 100001}, Scene.RenderQueue.GRQ_UI_UP)
			local boss_warning_effect_root = GlobalEffectMgr:GetEffect(boss_warning_effect_id)
			if boss_warning_effect_root then
				PosTool.Center(boss_warning_effect_root)
			end
		end
		self.__boss_warning_info = nil
	end
end

--BOSS展示信息面板
function Modules.SpMainDungeonModule:__DoBossMessageShow()
	self.__boss_message_info = nil

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if not battle_info.no_passed_flag then
		return
	end
	local config_battle = CPPGameLib.GetConfig("Battle", battle_info.battle_id, false)
	if config_battle and config_battle.boss_info_time then
		self.__boss_message_info = {}
		self.__boss_message_info.elapse_time = 0
		self.__boss_message_info.time = config_battle.boss_info_time
		self.__boss_message_info.representativeId = config_battle.representativeId
	end
end
function Modules.SpMainDungeonModule:__CheckBossMessageShow(elapse_time)
	if not self.__boss_message_info or self.__boss_message_info.is_finished then
		return
	end

	self.__boss_message_info.elapse_time = self.__boss_message_info.elapse_time + elapse_time	
	if self.__boss_message_info.elapse_time >= self.__boss_message_info.time then
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			self.__in_boss_show = true
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "PauseEx")
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW, false, self.__boss_message_info.representativeId,Macros.Game.HERO_RECRUIT.DUNGEON_SHOW,function ()
				self.__in_boss_show = false
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ResumeEx")
			end)
		end
		self.__boss_message_info.is_finished = true
	end
end

--英雄、精灵离开上一个战役区域
function Modules.SpMainDungeonModule:__DoExitFrmLastBattle()
	if self.__do_exit_frm_last_battle then
		self.__do_exit_frm_last_battle = nil

		local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

	    --英雄离场
		local hero_now_pos, hero_move_speed
		hero_vo_map = GlobalVOMgr:GetHeroMap()
		for obj_id, _ in pairs(hero_vo_map) do
			local hero = GlobalScene:GetObj(obj_id)
			if hero and not hero:IsDead() then
				hero:PauseAI()
				hero_now_pos = hero:GetWorldPosVec()
		        hero_move_speed = hero:GetAttr(Macros.Game.BattleProp.SPEED) * ConfigAdapter.Common.GetBattleChangeSpeedFactor() * 2
				hero:DoSpecialMove(Vector2D.New(battle_region.org.x + battle_region.size.width + 128, hero_now_pos.y), true, hero_move_speed, true)
			end
		end		

		--精灵离场
		local spirit = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
		if spirit then
			spirit:PauseAI()
			local spirit_now_pos = spirit:GetWorldPosVec()
	        local spirit_move_speed = spirit:GetAttr(Macros.Game.BattleProp.SPEED) * ConfigAdapter.Common.GetBattleChangeSpeedFactor() * 2
			spirit:DoSpecialMove(Vector2D.New(battle_region.org.x + battle_region.size.width + 128, spirit_now_pos.y), true, spirit_move_speed, true)
		end
	end
end

--检测对象在出生状态下，进入战斗区域逻辑
function Modules.SpMainDungeonModule:__CheckEnterRegionInBirth()
	if not self.__is_started then
		return		
	end

	if self.__is_checked_end then
		return
	end

	if not self.__can_check_enter_region then
		return
	end

	local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

	local obj, last_wpos_inbirth
	local function all_enter_region_func(for_heros)
		if for_heros then
			self.__all_heros_enter_region = true

			--全体英雄进入镜头后，统一回调
			self:__AllHerosEnterRegionInBirth()			
	
		else
			self.__all_monsters_enter_region = true
		end

		if self.__all_heros_enter_region and self.__all_monsters_enter_region then
			--全体英雄、怪物进入镜头
			self.__all_objs_enter_region = true
		end
	end

    local horz_space = 0
	local region_bounding_left = battle_region.org.x + horz_space
	local region_bounding_right = battle_region.org.x + battle_region.size.width - 1 - horz_space

	--英雄
	if self.__birth_hero_obj_ids then
		for index, obj_id in ipairs(self.__birth_hero_obj_ids) do
			obj = GlobalScene:GetObj(obj_id)
			if obj and obj:GetInBirth() then
				if (not obj.last_wpos_inbirth or obj.last_wpos_inbirth < region_bounding_left) and obj:GetWorldPosX() >= region_bounding_left then
					obj:ResumeAI()
					obj:SetEnteredBattleRegion(true)
					if index == 1 then
						all_enter_region_func(true)
					end
				end
				obj.last_wpos_inbirth = obj:GetWorldPosX()
			end
		end
	end

	--怪物
	if self.__birth_monster_obj_ids then
		for index, obj_id in ipairs(self.__birth_monster_obj_ids) do
			obj = GlobalScene:GetObj(obj_id)
			if obj and obj:GetInBirth() then
				if (not obj.last_wpos_inbirth or obj.last_wpos_inbirth > region_bounding_right) and obj:GetWorldPosX() <= region_bounding_right then
					obj:ResumeAI()
					obj:SetEnteredBattleRegion(true)
					if index == 1 then
						all_enter_region_func()
					end
				end
				obj.last_wpos_inbirth = obj:GetWorldPosX()
			end
		end
	end
end

--全体英雄进入战役区域的回调
function Modules.SpMainDungeonModule:__AllHerosEnterRegionInBirth()	
	--己方
	if self.__birth_hero_obj_ids then
		for _, obj_id in ipairs(self.__birth_hero_obj_ids) do
			obj = GlobalScene:GetObj(obj_id)
			if obj and obj.__PlaySpriteBlessEffect then
				obj:__PlaySpriteBlessEffect()
			end
		end
	end
	--敌方
	if self.__birth_monster_obj_ids then
		for _, obj_id in ipairs(self.__birth_monster_obj_ids) do
			obj = GlobalScene:GetObj(obj_id)
			if obj and obj.__PlaySpriteBlessEffect then
				obj:__PlaySpriteBlessEffect()
			end
		end
	end
end

--战斗胜利动作
function Modules.SpMainDungeonModule:__DoBattleSuccessAnim()
	if not self.__do_battle_success_anim_timer then
		self.__do_battle_success_anim_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			local hero
			local win_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetWinHeroList")
			if win_hero_list then
			    for _, vo in ipairs(win_hero_list) do
			        hero = GlobalScene:GetObj(vo.obj_id)
			        if hero then
			            hero:DoWin()
			        end
			    end		
			end
	    end), 0)
	end
end

--计算战斗结算星数
function Modules.SpMainDungeonModule:__CalcResultStar()
	local hero_dead_num = self.__hero_total_num - GlobalVOMgr:GetHeroNum()	
	return Modules.BattleFormula:CalcResultStar(hero_dead_num)
end

--对象合击、怒气技能释放
function Modules.SpMainDungeonModule:__ObjAngerOrJointSkillReleased(obj_id)
end

--对象合击、怒气技能释放结束
function Modules.SpMainDungeonModule:__ObjAngerOrJointSkillReleasedEnd(obj_id)
end

--打开战斗界面
function Modules.SpMainDungeonModule:__OpenBattleView()
	CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.BATTLE)
end

--QTE
function Modules.SpMainDungeonModule:__ObjAngerOrJointQTE(obj_id)
	local hero = GlobalScene:GetObj(obj_id) 
	if not hero or hero:GetInBirth() then
		return
	end
	--将攻击者的友方、敌方透明化
	local function handle_func(obj_list)
		local obj, cur_obj_id, obj_vo
		for _, obj_vo in ipairs(obj_list) do
			cur_obj_id = obj_vo.obj_id
			obj = GlobalScene:GetObj(cur_obj_id)
			
			if obj and cur_obj_id ~= obj_id then
				--模型透明度
				obj:SetOpacity(10)
				--血条透明度
				obj:SetHpBarOpacity(30)
				--隐藏身上特效
				GlobalEffectMgr:SetEffectsVisibleByTargetID(cur_obj_id, false)										
			end
		end
	end
	local teammate_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", obj_id)
	local enemy_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", obj_id)
	handle_func(teammate_map)
	handle_func(enemy_map)
	--全屏暂停
	GlobalScene:PauseOtherObjs(obj_id,true)
	--地图层变黑
	GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, false)
	ActionManager.GetInstance():StartMapBlackingWithLock(nil, nil, nil, nil, nil, nil)
	--放大背景
	local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()
	--触发QTE时的镜头拉近时长
	local def_time = ConfigAdapter.Common.GetQTEZoomInCameraTime() or 0.3
	local target_scale = 1.7
	local now_camera_scale = GlobalScene:GetCameraScale()
	local scale_speed = (target_scale - now_camera_scale)/def_time
	--镜头缩放
	GlobalScene:SetCameraScale(target_scale, true, function ()
		--QTE特效
		hero:ShowQTEEffect()
	end, scale_speed)	
	--偏移量
	local offset_list = ConfigAdapter.Common.GetQTEZoomInCameraOffsetList()
	--镜头偏移	
	local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
	local hero_x,hero_y = hero:GetModelPos()
	local target_pos = Vector2D.New(hero_x + offset_list[1], battle_region.org.y + battle_region.size.height / 2)
	
	local now_camera_pos = GlobalScene:GetCameraPos()
	local vec = target_pos - now_camera_pos
	local move_speed = math.max(MathTool.EPSINON, vec:Length() / def_time)

	--英雄出生Y坐标
	local hero_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.HERO)
	local _, pixely = GlobalScene:WorldToPixel(0, hero_birth_pos_y)	
	local offset_y = (pixely - hero_y) * target_scale
	GlobalScene:SetCameraPos(target_pos, true, nil, move_speed, offset_y + offset_list[2])
end

--QTE结束
function Modules.SpMainDungeonModule:__ObjAngerOrJointQTEEnd(obj_id)
	--将攻击者的友方、敌方还原透明度
	local function handle_func(obj_list)
		local obj, cur_obj_id, obj_vo
		for _, obj_vo in ipairs(obj_list) do
			cur_obj_id = obj_vo.obj_id
			obj = GlobalScene:GetObj(cur_obj_id)
			
			if obj and cur_obj_id ~= obj_id then
				--模型透明度
				obj:SetOpacity(255)
				--血条透明度
				obj:SetHpBarOpacity(255)
				--显示身上特效
				GlobalEffectMgr:SetEffectsVisibleByTargetID(cur_obj_id, true)										
			end
		end
	end
	local teammate_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", obj_id)
	local enemy_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", obj_id)
	handle_func(teammate_map)
	handle_func(enemy_map)
	-- --全屏暂停(恢复)
	-- GlobalScene:ResumeOtherObjs()

	--地图层恢复
	GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, true)
	ActionManager.GetInstance():StopMapBlackingWithLock(1)

	--镜头缩放
	self:__DoCameraAnim(nil, nil, true)	
end

--初始化镜头
function Modules.SpMainDungeonModule:__SetupCamera(for_init)
	local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()

	--当前战役区域
	local region_bound = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

	--更新镜头缩放
	GlobalScene:SetCameraScale(config_camera.init_scale)

	--更新镜头位置
	local pos = Vector2D.New(region_bound.org.x + region_bound.size.width / 2, region_bound.org.y + region_bound.size.height / 2)
	GlobalScene:SetCameraPos(pos)

	if not for_init then
		GlobalScene:SetCameraScale(1, true, function()
			--开始执行镜头效果
			self.__can_do_camera_anim = true
			self.__camera_effect_elapse_time = nil

			self:__DoBirthForSprite()
		end, config_camera.zoom_out_speed)
	end
end

--执行镜头动画
function Modules.SpMainDungeonModule:__DoCameraAnim(elapse_time, force_data, force_camera)
	if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
		return
	end
	
	--是否AI控制镜头中
	if CallModuleFunc(ModuleType.BATTLE, "IsInAICameraControl") then
		return
	end

	if self:IsInAngerOrJointSkill() and not force_camera then
		return
	end

	local camera_scale = GlobalScene:GetCameraScale()
	local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()

	--强制执行
	if force_data then
		--更新镜头缩放
		GlobalScene:SetCameraScale(force_data.scale, true, nil, force_data.scale > camera_scale and config_camera.zoom_in_speed or config_camera.zoom_out_speed)

		--更新镜头位置
		GlobalScene:SetCameraPos(force_data.pos, true, nil, config_camera.move_speed * 1.5)		
		return
	end

	if not self.__can_do_camera_anim then
		return
	end

	local is_over_camera
	local terrain_camera_bounding =  GlobalScene:GetCameraBounding()
	local camera_bounding = GlobalScene:GetCameraBounding(Scene.RenderQueue.GRQ_BUILD_AND_PLAYER, true)
	local last_hero = GlobalScene:GetObj(self.__hero_last_obj_id)
	local last_monster = GlobalScene:GetObj(self.__monster_last_obj_id)
	if not last_hero or not last_monster or last_hero:GetInBirth() or last_monster:GetInBirth() then
		return
	end	
	local last_hero_x = last_hero:GetModelPos()
	local last_monster_x = last_monster:GetModelPos()
	local camera_pos_x = (last_hero_x + last_monster_x) / 2

	--超出镜头，立即检测
	if (last_hero_x <= camera_bounding.org.x + config_camera.boundary / 2) or (last_monster_x >= camera_bounding.org.x + camera_bounding.size.width - 1 - config_camera.boundary / 2) then
		is_over_camera = true
	end

	--执行检测
	if not force_camera then
		self.__camera_effect_elapse_time = self.__camera_effect_elapse_time or 2
		self.__camera_effect_elapse_time = self.__camera_effect_elapse_time + elapse_time
		if not is_over_camera and self.__camera_effect_elapse_time < 2 then
			return
		end
		self.__camera_effect_elapse_time = 0	
	end

	local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
	local distance = math.abs(last_hero_x - last_monster_x) + config_camera.boundary * 2
	local delta_l = camera_pos_x - terrain_camera_bounding.size.width / 2 - battle_region.org.x
	local delta_r = camera_pos_x + terrain_camera_bounding.size.width / 2 - (battle_region.org.x + battle_region.size.width)
	
	--触发QTE后暂停后镜头恢复时长
	local def_time = ConfigAdapter.Common.GetQTEZoomOutCameraTime() or 0.3

	if delta_l >= 0 and delta_r <= 0 then
		--更新镜头缩放
		local scale = Macros.Global.DesignSize.width / (distance / config_camera.terrain_obj_factor)
		scale = math.max(1, scale)
		scale = math.min(scale, config_camera.max_scale)
		local zoom_speed 
		if not force_camera then
			zoom_speed = scale > camera_scale and config_camera.zoom_in_speed or config_camera.zoom_out_speed
		else
			--触发QTE后暂停后镜头恢复时长
			local now_camera_scale = GlobalScene:GetCameraScale()
			zoom_speed = math.abs(scale - now_camera_scale)/def_time
		end	
		GlobalScene:SetCameraScale(scale, true, nil, zoom_speed)
	end

	--更新镜头位置
	if is_over_camera then	
		local camera_pos = Vector2D.New(camera_pos_x, battle_region.org.y + battle_region.size.height / 2)
		local move_speed
		--镜头偏移	
		if force_camera then			
			local now_camera_pos = GlobalScene:GetCameraPos()
			local vec = camera_pos - now_camera_pos
			move_speed = math.max(MathTool.EPSINON, vec:Length() / def_time)		
		else
			move_speed = config_camera.move_speed
		end
		GlobalScene:SetCameraPos(camera_pos, true, nil, move_speed)
	end
end

