
--[[
%% @module: 战斗对象状态机
%% @author: swordman sue
%% @created: 2016-07-18
--]]

Scene = Scene or {}

Scene.BattleObj = Scene.BattleObj or BaseClass(Scene.Obj)

function Scene.BattleObj:__InitStateMachine()
	Scene.Obj.__InitStateMachine(self)

	--QTE状态
    local function qte_enter_func(state_name)
        self["__EnterStateQTE"](self, state_name)
    end
    local function qte_update_func(state_name, elapse_time)
        self["__UpdateStateQTE"](self, state_name, elapse_time)
    end
    local function qte_quit_func(state_name)
        self["__QuitStateQTE"](self, state_name)
    end
	tmp_state = self.__state_machine:CreateNormalState("qte", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(qte_enter_func),
		GlobalCallbackMgr:AddCallBackFunc(qte_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(qte_quit_func))

	--攻击状态
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

	--受击状态
    self.__s_beattacked_state = {}
	local function beattacked_enter_func(state_name)
		self["__EnterStateBeAttacked"](self, state_name)
	end
	local function beattacked_update_func(state_name, elapse_time)
		self["__UpdateStateBeAttacked"](self, state_name, elapse_time)
	end
	local function beattacked_quit_func(state_name)
		self["__QuitStateBeAttacked"](self, state_name)
    end
	tmp_state = self.__state_machine:CreateNormalState("beattacked", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(beattacked_enter_func),
		GlobalCallbackMgr:AddCallBackFunc(beattacked_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(beattacked_quit_func))
    tmp_state:SetStateCanReEnter(true)

    --击退状态
    local function repel_enter_func(state_name)
        self["__EnterStateRepel"](self, state_name)
    end
    local function repel_update_func(state_name, elapse_time)
        self["__UpdateStateRepel"](self, state_name, elapse_time)
    end
    local function repel_quit_func(state_name)
        self["__QuitStateRepel"](self, state_name)
    end
	tmp_state = self.__state_machine:CreateNormalState("repel", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(repel_enter_func),
		GlobalCallbackMgr:AddCallBackFunc(repel_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(repel_quit_func))

	--死亡状态
	local function dead_enter_func(state_name)
		self["__EnterStateDead"](self, state_name)
	end
	local function dead_update_func(state_name, elapse_time)
		self["__UpdateStateDead"](self, state_name, elapse_time)
	end
	local function dead_quit_func(state_name)
		self["__QuitStateDead"](self, state_name)
	end
	tmp_state = self.__state_machine:CreateNormalState("dead", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(dead_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(dead_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(dead_quit_func))		

	--胜利状态
	local function win_enter_func(state_name)
		self["__EnterStateWin"](self, state_name)
	end
	local function win_update_func(state_name, elapse_time)
		self["__UpdateStateWin"](self, state_name, elapse_time)
	end
	local function win_quit_func(state_name)
		self["__QuitStateWin"](self, state_name)
	end
	tmp_state = self.__state_machine:CreateNormalState("win", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(win_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(win_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(win_quit_func))
end

--------------------------------------------------------------------------------------------------------------------------------
--空闲状态
-----------------------------------------------------------
function Scene.BattleObj:__EnterStateIdle(state_name)
	Scene.Obj.__EnterStateIdle(self, state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:__EnterStateMove(state_name)
	Scene.Obj.__EnterStateMove(self, state_name)

	--添加脚底烟尘特效
	local effect_config_id = ConfigAdapter.Common.GetBattleRunEffect()
	if effect_config_id and effect_config_id ~= 0 then
		self.__s_move_state.fog_follow_id = self:__AddFogEffect(effect_config_id)
	end
end

function Scene.BattleObj:__QuitStateMove(state_name)
	--移除脚底烟尘特效
	self:__DelFogEffect(self.__s_move_state.fog_follow_id)

	Scene.Obj.__QuitStateMove(self, state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--特殊移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:__EnterStateSpecialMove(state_name)
    Scene.Obj.__EnterStateSpecialMove(self, state_name)

	--添加脚底烟尘特效
	local effect_config_id = ConfigAdapter.Common.GetBattleRunEffect()
	if effect_config_id and effect_config_id ~= 0 then
		self.__s_sp_move_state.fog_follow_id = self:__AddFogEffect(effect_config_id)
	end	
end

function Scene.BattleObj:__QuitStateSpecialMove(state_name)
	--移除脚底烟尘特效
	self:__DelFogEffect(self.__s_sp_move_state.fog_follow_id)

    Scene.Obj.__QuitStateSpecialMove(self, state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--QTE状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:DoQTE(skill_id, sp_skill_type)
	if not self:CanDoAttack(skill_id, sp_skill_type) then
		return
	end
	self:ChangeState("qte",true)
end

function Scene.BattleObj:__EnterStateQTE(state_name)
	self.__is_in_turning_qte_camera = true
	--爆点特效(TODO QTE爆点特效)
	local effect_id,effect = GlobalEffectMgr:CreateSceneTargetEffect({id = 100110,loop = false,scale = 1}, self.__vo.obj_id,RenderObj.InnerLayer.HP_EFFECT)  
    PosTool.Center(effect,0,30)   

	--动作(TODO QTE动作)
	local act_index = RenderObj.ActionIndex.IDLE 
	self:PlayAnimation(act_index, true)

	--QTE
	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE,self.__vo.obj_id)
end

function Scene.BattleObj:__UpdateStateQTE(state_name)

end

function Scene.BattleObj:__QuitStateQTE(state_name)
	--QTE
	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE_END,self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:SetForbidAttack(var)
    local delta = var and 1 or -1
    self.__forbid_attack = self.__forbid_attack or 0
    self.__forbid_attack = self.__forbid_attack + delta

    GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_FORBID_ATTACK_CHANGE, self.__vo.obj_id)
end

function Scene.BattleObj:IsForbidAttack()
	return self.__forbid_attack and self.__forbid_attack > 0
end

function Scene.BattleObj:SetForbidSpecialSkill(var)
    local delta = var and 1 or -1
	self.__forbid_sp_skill = self.__forbid_sp_skill or 0
	self.__forbid_sp_skill = self.__forbid_sp_skill + delta

    GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_FORBID_SP_SKILL_CHANGE, self.__vo.obj_id)	
end

function Scene.BattleObj:IsForbidSpecialSkill()
	return self.__forbid_sp_skill and self.__forbid_sp_skill > 0
end

function Scene.BattleObj:SetForbidAngerOrJointSkill(var)
    local delta = var and 1 or -1
	self.__forbid_anger_or_joint_skill = self.__forbid_anger_or_joint_skill or 0
	self.__forbid_anger_or_joint_skill = self.__forbid_anger_or_joint_skill + delta

    GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_FORBID_ANGER_OR_JOINT_SKILL_CHANGE, self.__vo.obj_id)	
end

function Scene.BattleObj:IsForbidAngerOrJointSkill()
	return self.__forbid_anger_or_joint_skill and self.__forbid_anger_or_joint_skill > 0
end

--是否处于怒气/合击技能
function Scene.BattleObj:IsInAngerOrJointSkill()
	if not self.__s_attack_state then
		return
	end

	return ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id)
end

--是否处于怒气/合击技能吟唱
function Scene.BattleObj:IsInAngerOrJointSkillSing()
	return self.__s_attack_state and self.__s_attack_state.is_in_anger_or_joint_skill_singing
end

--攻击逻辑
function Scene.BattleObj:AttackTargets()
	if not self.__s_attack_state then
		return
	end

	--攻击触发标记
	if not self.__s_attack_state.has_attack_targets then
		self.__s_attack_state.has_attack_targets = true

		--检测精灵祝福BUFF
		local data = {}
		data.skill_id = self.__s_attack_state.skill_id
		data.skill_lev = self.__s_attack_state.skill_lev
		Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.ATTACK_WITH_SKILL, data, self.__vo)
	end

	--搜索技能命中目标列表
	if not self.__s_attack_state.target_id_map then
		self.__s_attack_state.target_id_map = Skill.SkillLogic:SearchAllTgtBySkill(self.__vo.obj_id, self.__s_attack_state.skill_id, nil, self.__s_attack_state.external_data)
	end

	if self.__s_attack_state.target_id_map then
		--记录本帧中的受击对象
		for tgt_id, _ in pairs(self.__s_attack_state.target_id_map) do
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordBeattackedObjCurFrame", tgt_id, self.__vo.obj_id, self.__s_attack_state.skill_id, self.__s_attack_state.skill_lev, self.__s_attack_state.damage_time_index - 1)
		end

		--BUFF检测(最后一次攻击触发后，进行检测)
		if self.__s_attack_state.damage_time_index > #self.__s_attack_state.damage_time_list then
			self:__CheckBuffBySkill()
		end

		--播放攻击特效
		self:__PlayAttackEffect()

		--处理怒气改变
		self.__s_attack_state.sp_skill_logic:HandleAngerChange(true)
	end

	--BUFF检测(最后一次攻击触发后，进行检测)
	if self.__s_attack_state.damage_time_index > #self.__s_attack_state.damage_time_list then
		self:__CheckBuff()
	end

	--碰撞触发检测
	self:__CheckCollision()
end

--获取攻击目标
function Scene.BattleObj:GetAttackTargets()
	return self.__s_attack_state and self.__s_attack_state.target_id_map or nil
end

--获取攻击状态数据
function Scene.BattleObj:GetAttackState()
	return self.__s_attack_state
end

--是否处于不可打断攻击状态
function Scene.BattleObj:IsInUnInterruptableAttack()
	return self:IsInState("attack") and self.__s_attack_state.sp_skill_type == Macros.Game.SpSkillType.UN_INTERRUPTABLE
end

function Scene.BattleObj:CanDoAttack(skill_id, sp_skill_type, ignore_state)
    if self:IsDead() then
        return false
    end

    --未进入镜头
    if not self:GetEnteredBattleRegion() then
    	return false
    end

    if not self:IsVisible() then
    	return false
    end    

    if not ignore_state and
    	not self:IsInState("null") and 
    	not self:IsInState("idle") and 
    	not self:IsInState("move") and 
    	not self:IsInState("sp_move") and
    	not self:IsInState("qte") then
        return false
    end

    --特殊移动中且不能中断
    if self:IsInState("sp_move") and self.__s_sp_move_state.uninterruptable then
        return false
    end

    --远程职业出生状态中不能攻击
    if self:GetInBirth() and not self.__vo:IsProfessionNear() then
    	return false
    end

    --禁止攻击
	if self.__forbid_attack and self.__forbid_attack > 0 then
		return false
	end

	--禁止特殊技能
	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id, false)
	if skill_type == Macros.Game.SkillType.SPECIAL and self.__forbid_sp_skill and self.__forbid_sp_skill > 0 then
		return false
	end

	if ConfigAdapter.Skill.IsAngerOrJointSkill(skill_id) then
		--禁止怒气/合击
		if self.__forbid_anger_or_joint_skill and self.__forbid_anger_or_joint_skill > 0 then
			return false
		end
		--公共怒气是否充足
		if not self:IsAngerEnough() and sp_skill_type ~= Macros.Game.SpSkillType.FALSE_ANGER_OR_JOINT then
			return false
		end
		--所有战斗对象是否进入镜头
		if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") then
			return false
		end
		--BOSS展示是否完成
		if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsBossMessageShowFinished") then
			return false
		end
	end
	if ConfigAdapter.Skill.IsAngerOrJointSkill(skill_id, true) then
		--公共怒气是否充足
		if not self:IsAngerEnough(true) and sp_skill_type ~= Macros.Game.SpSkillType.FALSE_ANGER_OR_JOINT then
			return false
		end
	end

	--吟唱中
	if self:IsInAngerOrJointSkillSing() then
		return false
	end

	return true
end

function Scene.BattleObj:DoAttack(skill_id, skill_lev, sp_skill_type, external_data)
	if not self:CanDoAttack(skill_id, sp_skill_type) then
		return
	end

	self.__s_attack_state = self.__s_attack_state or {}
	self.__s_attack_state.skill_id = skill_id
	self.__s_attack_state.skill_lev = skill_lev
	self.__s_attack_state.sp_skill_type = sp_skill_type
	self.__s_attack_state.external_data = external_data

	--特殊逻辑处理
	self.__s_attack_state.sp_skill_logic = Skill.SkillLogic:CreateSpecialSkillLogic(self, skill_id)
	self.__s_attack_state.sp_skill_logic:DoAttack(skill_id, skill_lev)
end

function Scene.BattleObj:__EnterStateAttack(state_name)
	--BUFF
	self.__s_attack_state.has_check_buff = false
	self.__s_attack_state.has_check_buff_by_skill = false

	--伤害触发时间表
	local bullet_id, _, bullet_time_list = ConfigAdapter.Skill.GetSkillBulletInfo(self.__s_attack_state.skill_id)
	if bullet_id or ConfigAdapter.Skill.IsJointAttachedSkill(self.__s_attack_state.skill_id) then
		--已配置弹道/副将合击技能，置空伤害触发时间表
		self.__s_attack_state.damage_time_list = nil

		--设置弹道触发时间表
		self.__s_attack_state.bullet_time_list = bullet_time_list
		self.__s_attack_state.bullet_time_index = 1

		--搜索弹道命中目标列表		
		if not self.__s_attack_state.target_id_map then
			self.__s_attack_state.target_id_map = Skill.SkillLogic:SearchAllTgtBySkillBullet(self.__vo.obj_id, self.__s_attack_state.skill_id)
		end
	else
		--无配置弹道，读取伤害触发时间表
		local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
		self.__s_attack_state.damage_time_list = config_skill and config_skill.damage_time_list or {0}
		self.__s_attack_state.damage_time_index = 1

		--搜索技能命中目标列表
		if not self.__s_attack_state.target_id_map then
			self.__s_attack_state.target_id_map = Skill.SkillLogic:SearchAllTgtBySkill(self.__vo.obj_id, self.__s_attack_state.skill_id, nil, self.__s_attack_state.external_data)
		end

		--是否已触发攻击
		self.__s_attack_state.has_attack_targets = false
	end

	--播放攻击动作
	self:__PlayAttackAnim()

	--播放攻击特效
	self:__PlayAttackEffect()

	--播放攻击音效
	self:__PlayAttackSound()

	--触发攻击前移
	self:__TriggerAttackMove()

	--特殊逻辑处理
	self.__s_attack_state.sp_skill_logic:EnterStateAttack(state_name)
end

function Scene.BattleObj:__UpdateStateAttack(state_name, elapse_time)
	self.__s_attack_state.pass_time = self.__s_attack_state.pass_time or 0
	self.__s_attack_state.pass_time = self.__s_attack_state.pass_time + elapse_time

	--特殊逻辑处理
	self.__s_attack_state.sp_skill_logic:UpdateStateAttack(state_name, elapse_time)

	--攻击结束
	if self.__s_attack_state.is_anim_end then
		self:__QuitStateAttackHelper()

	--攻击更新
	else
		self:__CheckAttack()
		self:__CheckBullet()
		self:__AttackMove(elapse_time)
	end
end

function Scene.BattleObj:__QuitStateAttack(state_name)
	self.__body_anim_play_info.end_callback = nil
	
	--受击导致的离开攻击状态
	if self.__s_beattacked_state.toggle then
		--被非怒气/合击技能击中导致的离开攻击状态
		if not ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_beattacked_state.skill_id) then
			--攻击触发检测(补充)
			self:__CheckAttackEx(true)
		end

		--移除攻击特效
		local attack_effect_id = self.__s_attack_state.attack_effect_id
		local down_attack_effect_id = self.__s_attack_state.down_attack_effect_id
		self:InsertDelayAction(function()
			if attack_effect_id then
				GlobalEffectMgr:DelEffect(attack_effect_id)
			end
			if down_attack_effect_id then
				GlobalEffectMgr:DelEffect(down_attack_effect_id)
			end
		end, 0.3)
	end

	--特殊逻辑处理
	self.__s_attack_state.sp_skill_logic:QuitStateAttack(state_name)
	if self.__s_attack_state then
		self.__s_attack_state.sp_skill_logic:DeleteMe()
	end

	self.__s_attack_state = nil
end

function Scene.BattleObj:__QuitStateAttackHelper()
	--攻击触发检测(补充)
	self:__CheckAttackEx()

	--弹道触发检测(补充)
	self:__CheckBulletEx()

	self:DoIdle()	
end

--记录本帧中的BUFF检测(使用BUFF命中目标逻辑)
function Scene.BattleObj:__CheckBuff()
	if not self.__s_attack_state.has_check_buff then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", self.__vo.obj_id, self.__s_attack_state.skill_id, nil, nil, self.__s_attack_state.external_data)
		self.__s_attack_state.has_check_buff = true
	end
end

--记录本帧中的BUFF检测(使用技能命中目标逻辑)
function Scene.BattleObj:__CheckBuffBySkill()
	if not self.__s_attack_state.has_check_buff_by_skill then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", self.__vo.obj_id, self.__s_attack_state.skill_id, true, nil, self.__s_attack_state.external_data)
		self.__s_attack_state.has_check_buff_by_skill = true
	end
end

--攻击触发检测
function Scene.BattleObj:__CheckAttack()
	if not self.__s_attack_state.damage_time_list then
		return
	end

	local damage_time = self.__s_attack_state.damage_time_list[self.__s_attack_state.damage_time_index]
	if damage_time and self.__s_attack_state.pass_time >= damage_time then
		self.__s_attack_state.damage_time_index = self.__s_attack_state.damage_time_index + 1

		--记录本帧中的攻击对象
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordAttackObjCurFrame", self.__vo.obj_id)
	end
end

--攻击触发检测(补充)
function Scene.BattleObj:__CheckAttackEx(is_abort)
	if not self.__s_attack_state.damage_time_list then
		return
	end

	--若(攻击者因受击中断)或(非战斗跳过中，动作结束导致攻击结束)，且未触发过伤害，则强制触发最后一次伤害	
	if not self.__s_attack_state.has_attack_targets and 
		(is_abort or not CallModuleFunc(ModuleType.BATTLE, "IsSkipping")) then
		self.__s_attack_state.damage_time_index = #self.__s_attack_state.damage_time_list + 1
		self:AttackTargets()
	end

	--若(战斗跳过中，动作结束导致攻击结束)，且未触发完伤害，则强制触发剩余伤害
	if self.__s_attack_state.damage_time_index <= #self.__s_attack_state.damage_time_list and 
		(not is_abort and CallModuleFunc(ModuleType.BATTLE, "IsSkipping")) then
		while self.__s_attack_state.damage_time_index <= #self.__s_attack_state.damage_time_list do
			self.__s_attack_state.damage_time_index = self.__s_attack_state.damage_time_index + 1
			self:AttackTargets()
		end
	end
end

--碰撞触发检测
function Scene.BattleObj:__CheckCollision()
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)	
	local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(self.__s_attack_state.skill_id)
	if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.ENEMY and 
		tgt_logic_info and tgt_logic_info[1] == Macros.Game.SkillTgtLogic.NEAR and self.__s_attack_state.target_id_map then			
		--作用于敌方的技能才产生碰撞效果
		local target_id = next(self.__s_attack_state.target_id_map)
		local target = GlobalScene:GetObj(target_id)
		if target then
			--记录本帧中的碰撞对象
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCollisionObjCurFrame", self.__vo.obj_id, target_id)
		end
	end
end

--弹道触发检测
function Scene.BattleObj:__CheckBullet()
	if not self.__s_attack_state.bullet_time_list then
		return
	end

	local trigger_time = self.__s_attack_state.bullet_time_list[self.__s_attack_state.bullet_time_index]
	if trigger_time and self.__s_attack_state.pass_time >= trigger_time then
		Skill.SkillLogic:TriggerBullet(
			self.__vo.obj_id, self.__s_attack_state.skill_id, self.__s_attack_state.skill_lev, 
			self.__s_attack_state.bullet_time_index, #self.__s_attack_state.bullet_time_list)

		self.__s_attack_state.bullet_time_index = self.__s_attack_state.bullet_time_index + 1
	end
end

--弹道触发检测(补充)
function Scene.BattleObj:__CheckBulletEx()
	if not self.__s_attack_state.bullet_time_list then
		return
	end

    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then	
    	--战斗跳过中，若动作结束，则补充触发剩余弹道
    	while self.__s_attack_state.bullet_time_index <= #self.__s_attack_state.bullet_time_list do
			Skill.SkillLogic:TriggerBullet(
				self.__vo.obj_id, self.__s_attack_state.skill_id, self.__s_attack_state.skill_lev, 
				self.__s_attack_state.bullet_time_index, #self.__s_attack_state.bullet_time_list)

			self.__s_attack_state.bullet_time_index = self.__s_attack_state.bullet_time_index + 1
    	end
    else
    	--非战斗跳过中，若动作结束且未触发过弹道，则补充触发一次
    	if self.__s_attack_state.bullet_time_index == 1 then
			Skill.SkillLogic:TriggerBullet(
				self.__vo.obj_id, self.__s_attack_state.skill_id, self.__s_attack_state.skill_lev, 
				self.__s_attack_state.bullet_time_index, #self.__s_attack_state.bullet_time_list)
    	end
    end
end

--播放攻击动作
function Scene.BattleObj:__PlayAttackAnim(time_scale)
	if self.__s_attack_state.has_play_attack_anim then
		return
	end
	self.__s_attack_state.has_play_attack_anim = true

	local act_index = Skill.SkillLogic:GetAttackActIndex(self.__s_attack_state.skill_id)
	self.__s_attack_state.is_anim_end = false
	self.__body_anim_play_info.end_callback = function()
		if not self.__s_attack_state then
			return
		end
		self.__s_attack_state.is_anim_end = true
	end

	self:PlayAnimation(act_index, false)
end

--播放攻击特效
function Scene.BattleObj:__PlayAttackEffect()
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	if not config_skill then
		return
	end

	--攻击特效(挂载到攻击者身上)
	if (config_skill.attack_effect_id or config_skill.down_attack_effect_id) and not self.__s_attack_state.has_play_attack_effect then		
		--上层
		if config_skill.attack_effect_id then
			self.__s_attack_state.attack_effect_id = GlobalEffectMgr:CreateSceneTargetEffect({id = config_skill.attack_effect_id, dir = self.__dir}, 
				self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)
		end
		--下层
		if config_skill.down_attack_effect_id then
			self.__s_attack_state.down_attack_effect_id = GlobalEffectMgr:CreateSceneTargetEffect({id = config_skill.down_attack_effect_id, dir = self.__dir}, 
				self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_DOWN)
		end

		self.__s_attack_state.has_play_attack_effect = true		
	end

	--攻击场景特效(挂载到场景上)
	if (config_skill.attack_scene_effect_id or config_skill.down_attack_scene_effect_id) and not self.__s_attack_state.has_play_attack_scene_effect then
		local effect_x, effect_y = Skill.SkillLogic:CalcAttackSceneEffectPos(self.__vo.obj_id, self.__s_attack_state.skill_id, self.__s_attack_state.target_id_map, self.__s_attack_state.external_data)		
		if effect_x and effect_y then
			--上层
			if config_skill.attack_scene_effect_id then
				GlobalEffectMgr:CreateScenePosEffect(
					{id = config_skill.attack_scene_effect_id, pos_x = effect_x, pos_y = effect_y, dir = self.__dir}, Scene.RenderQueue.GRQ_AIR)		
			end
			--下层
			if config_skill.down_attack_scene_effect_id then
				GlobalEffectMgr:CreateScenePosEffect(
					{id = config_skill.down_attack_scene_effect_id, pos_x = effect_x, pos_y = effect_y, dir = self.__dir}, Scene.RenderQueue.GRQ_BUILD_AND_PLAYER)		
			end

			self.__s_attack_state.has_play_attack_scene_effect = true			
		end
	end

	--全屏特效
	if (config_skill.full_screen_effect_id or config_skill.down_full_screen_effect_id) and not self.__s_attack_state.has_play_full_screen_effect then
		--上层
		if config_skill.full_screen_effect_id then
			local _, effect = GlobalEffectMgr:CreateScenePosEffect(
				{id = config_skill.full_screen_effect_id, dir = self.__dir}, Scene.RenderQueue.GRQ_UI_DOWN_II)
			PosTool.Center(effect)
		end
		--下层
		if config_skill.down_full_screen_effect_id then
			local _, effect = GlobalEffectMgr:CreateScenePosEffect(
				{id = config_skill.down_full_screen_effect_id, dir = self.__dir, zorder = 2}, Scene.RenderQueue.GRQ_UI_DOWN)		
			PosTool.Center(effect)
		end

		self.__s_attack_state.has_play_full_screen_effect = true
	end
end

--播放攻击音效
function Scene.BattleObj:__PlayAttackSound()
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	if not config_skill then
		return
	end

	GlobalSoundMgr:PlayEffectByConfigID(config_skill.attack_sound_id)	
end

--触发攻击前移
function Scene.BattleObj:__TriggerAttackMove()
	local skill_type = ConfigAdapter.Skill.GetSkillType(self.__s_attack_state.skill_id)

	--近战职业，非怒气/合击技能触发攻击前移
	if self.__vo:IsProfessionNear() and 
		not self:IsInUnInterruptableAttack() and
		not ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id) and 
		not ConfigAdapter.Skill.IsJointAttachedSkill(self.__s_attack_state.skill_id) then
		self.__s_attack_state.move_triggered = true
		self.__s_attack_state.move_beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
		self.__s_attack_state.move_dir = Vector2D.New(self.__dir, 0)
		self.__s_attack_state.move_pass_distance = 0
		self.__s_attack_state.move_speed = self:GetMoveSpeed() * ConfigAdapter.Common.GetAttackMoveSpeedFactorOfNearSkill()
	end
end

--攻击前移
function Scene.BattleObj:__AttackMove(elapse_time)
	if not self.__s_attack_state.move_triggered then
		return
	end

    self.__s_attack_state.move_pass_distance = self.__s_attack_state.pass_time * self.__s_attack_state.move_speed
	local mov_dis = self.__s_attack_state.move_dir:Multi(self.__s_attack_state.move_pass_distance)
	local now_pos = self.__s_attack_state.move_beg_pos + mov_dis        
	self:SetWorldPos(now_pos.x, now_pos.y)
end

--------------------------------------------------------------------------------------------------------------------------------
--受击状态
--------------------------------------------------------------------------------------------------------------------------------
--禁止受击
function Scene.BattleObj:SetForbidBeAttacked(var)
	local delta = var and 1 or -1
	self.__forbid_beattacked = self.__forbid_beattacked or 0
	self.__forbid_beattacked = self.__forbid_beattacked + delta
end

function Scene.BattleObj:CanDoBeAttacked(skill_id)
	if self.__forbid_beattacked and self.__forbid_beattacked > 0 then
		return false
	end

    if self.__is_dead then
    	return false
    end

    if self.__vo.hp <= 0 then
    	return
	end

    if not self:IsVisible() then
    	return false
    end    

    --未进入镜头
    if not self:GetEnteredBattleRegion() then
    	local config_skill = CPPGameLib.GetConfig("Skill", skill_id, false)
    	if not config_skill or config_skill.camp ~= Macros.Game.SkillTgtCamp.FRIEND then
	    	return false
    	end
    	--未进入镜头，可被己方增益技能击中
    end

    --特殊移动中且不能中断
    if self:IsInState("sp_move") and self.__s_sp_move_state.uninterruptable then
        return false
    end

	return true
end

function Scene.BattleObj:DoBeAttacked(attacker_obj_id, skill_id, skill_lev, damage_time_index)	
	if not self:CanDoBeAttacked() then
		return
	end

    --怒气/合击技能中不能进入受击状态
    if self:IsInAngerOrJointSkill() then
    	return
    end

    self.__s_beattacked_state.toggle = true

	--特殊逻辑处理
	if self.__s_beattacked_state.sp_skill_logic then
		self.__s_beattacked_state.sp_skill_logic:DeleteMe()
	end
	self.__s_beattacked_state.sp_skill_logic = Skill.SkillLogic:CreateSpecialSkillLogic(self, skill_id, Skill.SpSkillLogicType.BEATTACKED)
	self.__s_beattacked_state.sp_skill_logic:DoBeAttacked(attacker_obj_id, skill_id, skill_lev, damage_time_index)
end

function Scene.BattleObj:__EnterStateBeAttacked(state_name)
	if self.__is_pause then
		self:ResumeAnimation()
	end

	--特殊逻辑处理
	self.__s_beattacked_state.sp_skill_logic:EnterStateBeAttacked(state_name)
end

function Scene.BattleObj:__UpdateStateBeAttacked(state_name, elapse_time)
	self.__s_beattacked_state.sp_skill_logic:UpdateStateBeAttacked(state_name, elapse_time)
end

function Scene.BattleObj:__QuitStateBeAttacked(state_name)
	if self.__is_pause then
		self:PauseAnimation()
	end

	--特殊逻辑处理
	self.__s_beattacked_state.sp_skill_logic:QuitStateBeAttacked(state_name)
end

--播放受击特效
function Scene.BattleObj:PlayBeAttackedEffect(skill_id)
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	skill_id = skill_id or self.__s_beattacked_state.skill_id

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id, false)
	if config_skill and config_skill.beattacked_effect_id then
		--限制同ID特效的数量
		if GlobalEffectMgr:TestEffectNumByConfigID(self.__vo.obj_id, config_skill.beattacked_effect_id, 3) then
			GlobalEffectMgr:CreateSceneTargetEffect(
				{id = config_skill.beattacked_effect_id, dir = self.__dir}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)
		end
	end
end

function Scene.BattleObj:PlayBeAttackedEffectEx(effect_id)
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	if not effect_id then
		return
	end

	--限制同ID特效的数量
	if GlobalEffectMgr:TestEffectNumByConfigID(self.__vo.obj_id, effect_id, 3) then
		GlobalEffectMgr:CreateSceneTargetEffect(
			{id = effect_id, dir = self.__dir}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)
	end
end

--播放受击音效
function Scene.BattleObj:PlayBeAttackedSound(skill_id)	
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	skill_id = skill_id or self.__s_beattacked_state.skill_id

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)	
	if not config_skill then
		return
	end

	GlobalSoundMgr:PlayEffectByConfigID(config_skill.beattacked_sound_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--受击浮空
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:SetForbidBeAttackedFloat(var)
	local delta = var and 1 or -1
	self.__forbid_beattacked_float = self.__forbid_beattacked_float or 0
	self.__forbid_beattacked_float = self.__forbid_beattacked_float + delta
end

function Scene.BattleObj:CanDoBeAttackedFloat()
	if self.__forbid_beattacked_float and self.__forbid_beattacked_float > 0 then
		return false
	end

    --攻击中且不能中断
    if self:IsInUnInterruptableAttack() then
    	return false
    end

	return self:CanDoBeAttacked()
end

function Scene.BattleObj:DoBeAttackedFloat(skill_id, for_damage)
	if not self:CanDoBeAttackedFloat() then
		return
	end

	if self.__s_beattacked_state.float_anim_id then
		return
	end

	--浮空动画ID|动画完成后是否回归原位置|是否产生伤害时触发
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	local float_info = config_skill and config_skill.float_info or nil
	if float_info then
		if for_damage and float_info[3] ~= Macros.Global.TRUE then
			--是否产生伤害时触发浮空的检测
			return
		end

		if not for_damage and float_info[3] == Macros.Global.TRUE then
			--是否技能开始时触发浮空的检测
			return
		end

		--开始浮空
		self.__s_beattacked_state.float_org_pos = Vector2D.New(self:GetWorldPos())
		self.__s_beattacked_state.float_anim_id = ActionManager.GetInstance():FloatAnim(self.__model:GetNode(), config_skill.float_info, -self.__dir, function()
			--结束浮空
			if self.__s_beattacked_state.float_anim_id then
				self.__s_beattacked_state.float_anim_id = nil

				if not config_skill.float_info[2] or config_skill.float_info[2] == Macros.Global.TRUE then
					--场景对象回归原位置
					self:SetWorldPos(self.__s_beattacked_state.float_org_pos.x, self.__s_beattacked_state.float_org_pos.y)
				else
					--场景对象更新到当前位置
					local world_pos_x, world_pos_y = self.__model:GetWorldPos()
					self:SetWorldPos(world_pos_x, self.__s_beattacked_state.float_org_pos.y)
				end
				self.__s_beattacked_state.float_org_pos = nil
			end
		end)
		return true
	end
end

function Scene.BattleObj:IsInBeattackedFloat()
	return self.__s_beattacked_state.float_anim_id ~= nil
end

--------------------------------------------------------------------------------------------------------------------------------
--击退状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:SetForbidRepel(var)
    local delta = var and 1 or -1
	self.__forbid_repel = self.__forbid_repel or 0
	self.__forbid_repel = self.__forbid_repel + delta
end

function Scene.BattleObj:CanDoRepel(skill_id)    
	if self.__forbid_repel and self.__forbid_repel > 0 then
		return false
	end

    if self.__is_dead then
    	return false
    end

    if not self:IsVisible() then
    	return false
    end

    --未进入镜头
    if not self:GetEnteredBattleRegion() then
    	return false
    end

    --特殊移动中且不能中断
    if self:IsInState("sp_move") and self.__s_sp_move_state.uninterruptable then
        return false
    end

    --攻击中且不能中断
    if self:IsInUnInterruptableAttack() then
    	return false
    end

	--免疫击退
	if self.__vo.immune_ctrl_skill or self.__vo.immune_ctrl_skill2 then
		return false
	end

	return true
end

function Scene.BattleObj:DoRepel(attacker_obj_id, skill_id, skill_lev)
	if not self:CanDoRepel() then
		return
	end

	self.__s_repel_state = self.__s_repel_state or {}
	self.__s_repel_state.total_distance = self.__s_repel_state.total_distance or 0

	--计算击退距离
	local total_distance
	if skill_id then
		local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
		total_distance = Modules.BattleFormula:CalcSkillBeatback(config_skill_lev and config_skill_lev.beat_back or 0, attacker_obj_id, self.__vo.obj_id)
	else
		total_distance = ConfigAdapter.Common.GetBeatBackDistanceOfCollision()
	end	
	if total_distance <= 0 then
		return
	end

	--取较远距离作为最终击退距离
	self.__s_repel_state.total_distance = math.max(self.__s_repel_state.total_distance, total_distance)

    if self:IsInState("repel") then
    	--击退状态中，只刷新最终击退距离，不重入击退状态
    	return
    end

	self.__s_repel_state.skill_id = skill_id
	self:ChangeState("repel", true)
end

function Scene.BattleObj:__EnterStateRepel(state_name)		
	self.__s_repel_state.pass_time = 0
	self.__s_repel_state.pass_distance = 0
	self.__s_repel_state.beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
	self.__s_repel_state.dir = Vector2D.New(-self.__dir, 0)

	--播放受击动作
	self:PlayAnimation(RenderObj.ActionIndex.REPEL, false)

	--添加脚底烟尘特效
	local effect_config_id = ConfigAdapter.Common.GetBattleRepelEffect()
	if effect_config_id and effect_config_id ~= 0 then
		self.__s_repel_state.fog_follow_id = self:__AddFogEffect(effect_config_id, true)
	end
end

function Scene.BattleObj:__UpdateStateRepel(state_name, elapse_time)
    self.__s_repel_state.pass_time = self.__s_repel_state.pass_time + elapse_time

	--击退位移 = v * t * ( 1 - v * t / 4 / l )
	local v = ConfigAdapter.Common.GetBeatbackInitSpeed()
	local l = self.__s_repel_state.total_distance
	local t = self.__s_repel_state.pass_time
    local dis = v * t * (1 - v * t / 4 / l)
	if dis < self.__s_repel_state.pass_distance then
		local mov_dis = self.__s_repel_state.dir:Multi(self.__s_repel_state.total_distance)
		local now_pos = self.__s_repel_state.beg_pos + mov_dis        
		self:SetWorldPos(now_pos.x, now_pos.y)

		if self.is_real_dead then
			self:DoDead()
		else 
			self:DoIdle()
		end
	else
		self.__s_repel_state.pass_distance = dis
		local mov_dis = self.__s_repel_state.dir:Multi(self.__s_repel_state.pass_distance)
		local now_pos = self.__s_repel_state.beg_pos + mov_dis        
		self:SetWorldPos(now_pos.x, now_pos.y)
	end
end

function Scene.BattleObj:__QuitStateRepel(state_name)
	--移除脚底烟尘特效
	self:__DelFogEffect(self.__s_repel_state.fog_follow_id)

	self.__s_repel_state = nil
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:IsDead()
    return self.__is_dead or self.is_real_dead
end

function Scene.BattleObj:CanDoDead()
	if self.__is_dead then
		return false
	end

	--击退完再死亡
	if self:IsInState("repel") then
		return false
	end

	--受击完再死亡
	if self:IsInState("beattacked") then
		return false
	end

	return true
end

--------------------------------------------------------------------------------------------------------------------------------
--出生状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:DoBirth(birth_type, birth_src_pos, birth_tgt_pos)
	local function birth_finished_callback(obj_id)
		if obj_id == self.__vo.obj_id then
			self:ResumeAI()
			self:SetInBirth(false)
            self:SetSkipBoundTest(false)
			self:SetEnteredBattleRegion(true)
		end
	end

    self:SetInBirth(true)
	self:SetVisible(true)
    self:SetSkipBoundTest(true)
	self:DoAngerChange(0)
	self:ResetAISkillForBattleStart()

	if birth_type == Macros.Game.FBBirthType.RUN_INTO then
		--跑步进入
		self:SetWorldPos(birth_src_pos.x, birth_src_pos.y)
		self:DoSpecialMove(birth_tgt_pos, true, nil, nil, birth_finished_callback)
	else
		--静态站位
		self:DoNull()
		self:PauseAI()
		self:SetWorldPos(birth_tgt_pos.x, birth_tgt_pos.y)
		birth_finished_callback(self.__vo.obj_id)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--胜利状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.BattleObj:DoWin()
    self:ChangeState("win", true)
end

function Scene.BattleObj:__EnterStateWin(state_name)
	self.__s_win_state = {}
	self.__s_win_state.is_anim_end = false	

	self.__body_anim_play_info.end_callback = function()
		if not self.__s_win_state then
			return
		end
		self.__s_win_state.is_anim_end = true
	end
	self:PlayAnimation(RenderObj.ActionIndex.WIN, false)
end

function Scene.BattleObj:__UpdateStateWin(state_name, elapse_time)
	if self.__s_win_state.is_anim_end then
		self:DoIdle()
	end
end

function Scene.BattleObj:__QuitStateWin(state_name)
	self.__body_anim_play_info.end_callback = nil
	self.__s_win_state = nil
end

