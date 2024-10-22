
--[[
%% @module: AI基类
%% @author: swordman sue
%% @created: 2016-07-13
--]]

AI = AI or {}

AI.BaseAI = AI.BaseAI or BaseClass(EventAndTimerWrapper)

AI.Type = 
{
	UNKNOW = 0,
	HERO = 1,
	MONSTER = 2,
	SPIRIT = 3,
	HERO_PVP = 4,
	MT_MONSTER = 5,
	MT_SPIRIT = 6,
}

--[[
@ params：
	obj_id 			[number]场景对象ID
--]]

function AI.BaseAI:__init(params)
	self.__type = AI.Type.UNKNOW
    self.__obj_id = params.obj_id

	if not self:__MyInit() then
		return
	end

	self.__state_machine = CPPGameStateMachine()
	self.__state_machine:CreateSink("main")

	--待机状态
	self.__s_idle_state = {}
	local function idle_enter_func(state_name)
		self["__EnterStateIdle"](self, state_name)
	end
	local function idle_update_func(state_name, elapse_time)
		self["__UpdateStateIdle"](self, state_name, elapse_time)
	end
	local function idle_quit_func(state_name)
		self["__QuitStateIdle"](self, state_name)
	end
	local tmp_state = self.__state_machine:CreateNormalState("idle", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(idle_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(idle_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(idle_quit_func))

	--巡敌状态
	self.__s_chase_state = {}
	local function chase_enter_func(state_name)
		self["__EnterStateChase"](self, state_name)
	end
	local function chase_update_func(state_name, elapse_time)
		self["__UpdateStateChase"](self, state_name, elapse_time)
	end
	local function chase_quit_func(state_name)
		self["__QuitStateChase"](self, state_name)
	end
	local tmp_state = self.__state_machine:CreateNormalState("chase", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(chase_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(chase_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(chase_quit_func))

	--攻击状态
	self.__s_attack_state = {}
	local function attack_enter_func(state_name)
		self["__EnterStateAttack"](self, state_name)
	end
	local function attack_update_func(state_name, elapse_time)
		self["__UpdateStateAttack"](self, state_name, elapse_time)
	end
	local function attack_quit_func(state_name)
		self["__QuitStateAttack"](self, state_name)
	end
	local tmp_state = self.__state_machine:CreateNormalState("attack", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(attack_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(attack_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(attack_quit_func))

	--初始化完毕
    self.__is_run = true
	self.__init_success = true
   	self:__ChangeToIdle()
end

function AI.BaseAI:__delete()
	self.__state_machine = nil
	self.__skill_map = nil
	self.__skill_list = nil
	self.__common_cd_info = nil
end

function AI.BaseAI:IsInitSuccess()
	return self.__init_success
end

function AI.BaseAI:StartAI()
	self.__is_run = true
end

function AI.BaseAI:StopAI()
	self.__is_run = false
end

function AI.BaseAI:GetType()
	return self.__type
end

function AI.BaseAI:GetObjID()
	return self.__obj_id
end

function AI.BaseAI:Update(now_time, elapse_time)	
	self:__UpdateSkillCD(elapse_time)

    if not self.__is_run then
        return
    end

	local me = self:__GetMe()
	if not me or me:IsDead() then
		self:StopAI()
		return
	end

	if not me:GetEnteredBattleRegion() then
		return
	end

	if not me:IsInState("null") and
		not me:IsInState("idle") and 
		not me:IsInState("move") and
		not me:IsInState("sp_move") then		
		return
	end

	--移除待攻击近战碰撞对记录
	local my_vo = self:__GetMyVO()
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DelToAttackPNObjsRecord", my_vo.obj_id)

	if me:IsForbidMove() or me:IsForbidAttack() then
		return
	end

	--选择技能
	self:__SelectSkill(elapse_time)

	--搜索目标
	self:__SearchTarget()

	--更新状态机
	self.__state_machine:Update(elapse_time)
end

--每场战役开启时的技能重置
function AI.BaseAI:ResetSkillForBattleStart()
	self.__next_skill_id = nil
	self.__common_cd_info.in_cd = false

	for _, skill_info in pairs(self.__skill_map) do
		if skill_info.type == Macros.Game.SkillType.SPECIAL then
			skill_info.in_cd = true
		else
			skill_info.in_cd = false
		end
	end	
end

--触发怒气/合击技能
function AI.BaseAI:TriggerAngerOrJointSkill(is_plus)
end

function AI.BaseAI:ShutdownAngerOrJointSkill(is_plus)
end

--检测怒气/合击技能开关
function AI.BaseAI:IsAngerOrJointSkillTriggered()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function AI.BaseAI:__MyInit()
   	local me = self:__GetMe()
	if not me then
   		LogTool.LogError("[AI.BaseAI:__Init] 找不到对应的场景对象(self.__obj_id:%d)", self.__obj_id)
   		self.__init_success = false
   		return false
	end
	local my_vo = self:__GetMyVO()

	--远程职业的技能间CD信息
	self.__common_cd_info = {}
	self.__common_cd_info.in_cd = false
	self.__common_cd_info.cd_time = (my_vo.common_cd_time or 0) * 0.001
	self.__common_cd_info.cd_elapse_time = 0

	--技能表
	self.__skill_map = {}

	--技能释放表
	self.__skill_list = {}

	--初始化技能相关
	local config_skill
	if my_vo.skill_list then
		for _, skill_id in ipairs(my_vo.skill_list) do
			config_skill = CPPGameLib.GetConfig("Skill", skill_id)
			if config_skill then
				--记录普攻、特殊技能
				if config_skill.type == Macros.Game.SkillType.NORMAL or config_skill.type == Macros.Game.SkillType.SPECIAL then
					local skill_info = {}
					skill_info.id = skill_id
					skill_info.type = config_skill.type
					skill_info.in_cd = config_skill.type == Macros.Game.SkillType.SPECIAL
					skill_info.cd_time = config_skill.cd_time
					skill_info.cd_elapse_time = 0

					self.__skill_map[skill_id] = skill_info
					table.insert(self.__skill_list, skill_info)
				end
			end
		end
		--优先级(特殊 > 普攻)
		table.sort(self.__skill_list, SortTools.KeyUpperSorter("type"))		
	end
	
   	return true
end

--更新技能CD
function AI.BaseAI:__UpdateSkillCD(elapse_time)
	--技能CD检测
	for skill_id, skill_info in pairs(self.__skill_map) do
		if skill_info.in_cd then
			skill_info.cd_elapse_time = skill_info.cd_elapse_time + elapse_time
			if skill_info.cd_elapse_time >= skill_info.cd_time then
				skill_info.in_cd = false
			end
		end
	end

	--远程职业的技能间CD检测
	if self.__common_cd_info.in_cd then
		self.__common_cd_info.cd_elapse_time = self.__common_cd_info.cd_elapse_time + elapse_time
		if self.__common_cd_info.cd_elapse_time >= self.__common_cd_info.cd_time then
			self.__common_cd_info.in_cd = false
		end
	end
end

--选择技能
function AI.BaseAI:__SelectSkill()
	local my_vo = self:__GetMyVO()

	--远程职业
	if not my_vo:IsProfessionNear() then
		local me = self:__GetMe()
		local my_world_pos = me:GetWorldPosVec()
		local enemy = GlobalScene:GetObj(self:__GetFirstEnemy())
		local enemy_world_pos = enemy and enemy:GetWorldPosVec() or nil

		local tgt_logic_info = my_vo.skill_id_of_near_skill and ConfigAdapter.Skill.GetSkillTargetLogicInfo(my_vo.skill_id_of_near_skill) or nil
		local trigger_range = tgt_logic_info and tgt_logic_info[3] or ConfigAdapter.Common.GetTriggerRangeOfNearSkill()
		if enemy_world_pos and my_world_pos:IsNearHorz(enemy_world_pos, trigger_range) then
			--被近身，优先选择近身技能
			self.__next_skill_id = my_vo.skill_id_of_near_skill
			return
		else
			--已选择近身技能，一旦远离敌人则切换远程技能
			if self.__next_skill_id == my_vo.skill_id_of_near_skill then
				self.__next_skill_id = nil
			end

			--技能间CD检测
			if self.__common_cd_info.in_cd then
				return
			end
		end

	--近战职业
	else
		--先清空，再实时检测
		self.__next_skill_id = nil
	end

	--选择技能
	if not self.__next_skill_id then
		for _, skill_info in ipairs(self.__skill_list) do
			if not skill_info.in_cd then
				self.__next_skill_id = skill_info.id
				break
			end
		end
	end
end

--释放技能
function AI.BaseAI:__ReleaseSkill(sp_skill_type)
	if not self.__next_skill_id then
		return false
	end

	--重置CD
	self:__SetSkillCD()

	--触发攻击
	local me = self:__GetMe()
	local my_vo = self:__GetMyVO()
	me:DoAttack(self.__next_skill_id, my_vo.skill_level, sp_skill_type)
	
	--重置当前技能
	self.__next_skill_id = nil
	
	return true
end

--设置技能CD
function AI.BaseAI:__SetSkillCD()
	if not self.__next_skill_id then
		return
	end

	local skill_info = self.__skill_map[self.__next_skill_id]
	if skill_info then
		--技能CD
		if skill_info.cd_time > 0 then
			skill_info.in_cd = true
			skill_info.cd_elapse_time = 0
		end

		--远程职业技能间CD
		local my_vo = self:__GetMyVO()
		if not my_vo:IsProfessionNear() then
			self.__common_cd_info.in_cd = true
			self.__common_cd_info.cd_elapse_time = 0
		end
	end	
end

--根据技能搜索目标
function AI.BaseAI:__SearchTarget()
	self.__tgt_obj_id = nil
	self.__chase_world_pos = nil

	local is_need_tgt = true
	local is_need_chase = true
	if self.__next_skill_id then
		--根据技能命中目标逻辑，搜索目标和追击点
		is_need_tgt, is_need_chase, self.__tgt_obj_id, self.__chase_world_pos = 
		Skill.SkillLogic:SearchTgtAndChaseForAI(self.__obj_id, self.__next_skill_id)
	end
end

--获取首个敌人
function AI.BaseAI:__GetFirstEnemy()
	return CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstEnemy", self.__obj_id)
end

--切换状态
function AI.BaseAI:__ChangeState(state_name, force)
    if self.__state_machine == nil then
        return
    end
    if not force then
        self.__state_machine:ChangeState(state_name, false)
    else
        self.__state_machine:ChangeStateForce(state_name)
    end
end

--[[
	待机状态
--]]

function AI.BaseAI:__ChangeToIdle()
    self:__ChangeState("idle", true)
end

function AI.BaseAI:__EnterStateIdle(state_name)
    local me = self:__GetMe()
	me:DoIdle()
end

function AI.BaseAI:__UpdateStateIdle(state_name, elapse_time)
	if not self:__CheckToAttack() then
		local me = self:__GetMe()
		if not me:IsInAngerOrJointSkillSing() then
			self:__ChangeToChase()
		end
	end
end

function AI.BaseAI:__QuitStateIdle(state_name)
end

--[[
	巡逻状态
--]]

function AI.BaseAI:__ChangeToChase()
	self:__ChangeState("chase", true)
end

function AI.BaseAI:__EnterStateChase(state_name)
    local me = self:__GetMe()
    if self.__chase_world_pos then
    	local tgt = GlobalScene:GetObj(self.__tgt_obj_id)
    	if tgt and tgt:CanDoBeAttacked() then
	    	me:DoMove(self.__chase_world_pos, true)
	    end
	end
end

function AI.BaseAI:__UpdateStateChase(state_name, elapse_time)
	if not self:__CheckToAttack() then
		local me = self:__GetMe()
		if self.__chase_world_pos and not me:IsInState("move") then
	    	local tgt = GlobalScene:GetObj(self.__tgt_obj_id)
	    	if tgt and tgt:CanDoBeAttacked() then
		    	me:DoMove(self.__chase_world_pos, true)
	    	end
		end
	end
end

function AI.BaseAI:__QuitStateChase(state_name)
end

--[[
	攻击状态
--]]

function AI.BaseAI:__ChangeToAttack()
	self:__ChangeState("attack", true)
end

function AI.BaseAI:__EnterStateAttack()
	self:__ReleaseSkill()
end

function AI.BaseAI:__UpdateStateAttack()
	local me = self:__GetMe()
	if not me:IsInAngerOrJointSkillSing() then
		self:__ChangeToChase()
	end
end

function AI.BaseAI:__QuitStateAttack()
end

--攻击检测
function AI.BaseAI:__CheckToAttack()
	if not self.__next_skill_id then
		return false
	end

	--根据命中目标逻辑，检测能否攻击
	local my_vo = self:__GetMyVO()
	if not Skill.SkillLogic:CheckToAttackForAI(self.__obj_id, self.__tgt_obj_id, self.__next_skill_id, my_vo.skill_level) then
		return false
	end

	self:__ChangeToAttack()
	return true		
end

function AI.BaseAI:__GetMe()
	return GlobalScene:GetObj(self.__obj_id)
end

function AI.BaseAI:__GetMyVO()
	local me = self:__GetMe()
	if me then
		return me:GetVO()
	end
end

