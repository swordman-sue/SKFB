
--[[
%% @module: 怒气/合击技能特殊逻辑
%% @author: swordman sue
%% @created: 2016-11-25
--]]

Skill = Skill or {}

Skill.SpecialSkillAngerJoint = Skill.SpecialSkillAngerJoint or BaseClass(Skill.SpecialSkillBase)

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillAngerJoint:DoAttack(skill_id, skill_lev)
	--处理怒气改变
	self.__s_attack_state.has_handle_anger_change = false	
	self:HandleAngerChange()

	--处理怒气/合击技能
	self:__HandleAngerOrJointSkill(function()
		self.__battle_obj:ChangeState("attack", true)
	end)
end

function Skill.SpecialSkillAngerJoint:EnterStateAttack(state_name)
	Skill.SpecialSkillBase.EnterStateAttack(self, state_name)

	--怒气/合击技能受击浮空
	self:__HandleAngerOrJointSkillTargetFloat()

	--处理合击副将相关
	self:__HandleJointAttachedObjs()

	self.__battle_obj:SetSkipBoundTest(true)
end

function Skill.SpecialSkillAngerJoint:UpdateStateAttack(state_name, elapse_time)
	Skill.SpecialSkillBase.UpdateStateAttack(self, state_name, elapse_time)
	
	if not self.__s_attack_state.is_anim_end then
		--怒气/合击技能瞬移检测
		self:__CheckAngerTeleporting(elapse_time)
	end
end

function Skill.SpecialSkillAngerJoint:QuitStateAttack(state_name)
	--处理怒气/合击技能结束
	self:__HandleAngerOrJointSkillEnd()

	self.__battle_obj:SetSkipBoundTest(false)
end

--------------------------------------------------------------------------------------------------------------------------------
--受击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillAngerJoint:DoBeAttacked(attacker_obj_id, skill_id, skill_lev, damage_time_index)
	self.__s_beattacked_state.attacker_obj_id = attacker_obj_id
	self.__s_beattacked_state.skill_id = skill_id
	self.__s_beattacked_state.damage_time_index = damage_time_index or 1

	--受击音效
	self.__battle_obj:PlayBeAttackedSound(skill_id)

	--受击浮空
	self.__battle_obj:DoBeAttackedFloat(self.__s_beattacked_state.skill_id, true)

	--播放受击特效
	self.__battle_obj:PlayBeAttackedEffect()

	--播放怪物受击特效
	if self.__battle_obj:IsMonster() then
		self.__battle_obj:PlayBeAttackedEffectEx(self.__vo.beattacked_effect_id)
	end

	--检测精灵祝福BUFF
	local data = {}
	data.skill_id = skill_id
	data.skill_lev = skill_lev	
	Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.BE_ATTACKED_BY_SKILL, data, self.__vo)

    --攻击中且不能中断
    if self.__battle_obj:IsInUnInterruptableAttack() then
    	return
    end

	if self.__battle_obj:IsInState("beattacked") then
		--处理受击动作
		self:__HandleBeAttackedAnim()
	else
		self.__battle_obj:ChangeState("beattacked", true)
	end
end

function Skill.SpecialSkillAngerJoint:EnterStateBeAttacked(state_name)
	--处理受击动作
	self:__HandleBeAttackedAnim()
end

function Skill.SpecialSkillAngerJoint:UpdateStateBeAttacked(state_name, elapse_time)
	if self.__s_beattacked_state and self.__s_beattacked_state.is_anim_end then
		--恢复受击者状态
		if self.__battle_obj.is_real_dead then
			self.__battle_obj:DoDead()
		else
			self.__battle_obj:DoIdle()
		end
	end
end

function Skill.SpecialSkillAngerJoint:QuitStateBeAttacked(state_name)
	self.__s_beattacked_state.skill_id = nil
	self.__s_beattacked_state.damage_time_index = 1

	--中断浮空
	ActionManager.GetInstance():StopFloatAnim(self.__s_beattacked_state.float_anim_id)
	self.__s_beattacked_state.float_anim_id = nil
end

function Skill.SpecialSkillAngerJoint:ResumeExpand()
	if self.__s_attack_state.joint_attached_hero_map then
		local obj
		for obj_id, _ in pairs(self.__s_attack_state.joint_attached_hero_map) do
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				obj:Resume()
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--处理怒气/合击技能
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkill(enter_attack_state_func)	
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)

	--怒气/合击技能(主将)
	if ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id) then
		self.__battle_obj:DoNull(nil, false, true)

		--怒气归零
		if self.__s_attack_state.sp_skill_type ~= Macros.Game.SpSkillType.FALSE_ANGER_OR_JOINT then
			local cost_anger = not ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id, true) and -self.__vo.cost_of_anger_or_joint_skill or -self.__vo.cost_of_anger_or_joint_skill * 2
			self.__battle_obj:DoAngerChange(cost_anger)
		end

		--怒气/合击技能瞬移预处理(为了近战类的怒气技能可以找到目标)
		self:__HandleAngerOrJointSkillTeleportingForSearchTgts()

		--根据技能命中目标逻辑/弹道逻辑，搜索所有目标
		self.__s_attack_state.target_id_map = Skill.SkillLogic:SearchAllTgt(self.__vo.obj_id, self.__s_attack_state.skill_id)

		--怒气/合击技能瞬移预处理
		self:__HandleAngerOrJointSkillTeleporting()

		--怒气/合击技能的受击对象处理
		self:__HandleAngerOrJointSkillTarget()

		--怒气/合击技能吟唱处理
		self:__HandleAngerOrJointSkillSing(enter_attack_state_func)		

		--全屏暂停
		GlobalScene:PauseOtherObjs(self.__vo.obj_id)

		--地图层变黑
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, false)
			ActionManager.GetInstance():StartMapBlackingWithLock(nil, nil, nil, nil, nil, config_skill.bg_res_name)
		end

		--派发怒气/合击技能释放的事件
		GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE, self.__vo.obj_id, self.__s_attack_state.skill_id)		

	--合击技能(副将)
	elseif ConfigAdapter.Skill.IsJointAttachedSkill(self.__s_attack_state.skill_id) then
		--怒气/合击技能瞬移预处理(为了近战类的怒气技能可以找到目标)
		self:__HandleAngerOrJointSkillTeleportingForSearchTgts()

		--根据技能命中目标逻辑/弹道逻辑，搜索所有目标
		self.__s_attack_state.target_id_map = Skill.SkillLogic:SearchAllTgt(self.__vo.obj_id, self.__s_attack_state.skill_id)

		--怒气/合击技能瞬移预处理
		self:__HandleAngerOrJointSkillTeleporting()		

		--进入攻击状态
		enter_attack_state_func()

	else
		--进入攻击状态
		enter_attack_state_func()
	end
end

--怒气/合击技能吟唱
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillSing(enter_attack_state_func)
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	if not config_skill then
		--进入攻击状态
		enter_attack_state_func()
		return
	end

	--攻击动作定格第一帧
	local is_attack_anim_locked
	local function lock_attack_anim()
		if is_attack_anim_locked then
			return
		end
		is_attack_anim_locked = true

		--怒气/合击技能，攻击动作并定格在第一帧
		self.__battle_obj:PushAnimTimeScale(0)
		self.__battle_obj:__PlayAttackAnim()
	end
	local function unlock_attack_anim()
		if is_attack_anim_locked then
			self.__battle_obj:PopAnimTimeScale()
			is_attack_anim_locked = false			
		end
	end

	--进入攻击状态
	local function handle_enter_attack_state_func()
		unlock_attack_anim()

		--吟唱标记
		self.__s_attack_state.is_in_anger_or_joint_skill_singing = false

		enter_attack_state_func()
	end
	--吟唱特效
	local function handle_sing_effect()
		if self.is_deleted then
			return
		end

		if config_skill.anger_sing_effect_id and not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			--攻击动作定格
			lock_attack_anim()

			--播放吟唱特效
			GlobalEffectMgr:CreateSceneTargetEffect({
				id = config_skill.anger_sing_effect_id, dir = self.__battle_obj:GetDir(), end_callback = function()
					handle_enter_attack_state_func()
				end}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)
		else
			handle_enter_attack_state_func()			
		end
	end
	--头像动画
	local function handle_head_anim(joint_effect_info)
		--吟唱标记
		self.__s_attack_state.is_in_anger_or_joint_skill_singing = true
		
		if not joint_effect_info or CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			handle_sing_effect()
		else
			--攻击动作定格
			lock_attack_anim()

			--头像动画
			local node_head1 = Util:Sprite(Resource.PathTool.GetJointHeadPath(joint_effect_info[2]))				
			local node_head2 = Util:Sprite(Resource.PathTool.GetJointHeadPath(joint_effect_info[3]))
			local node_title = Util:Sprite(Resource.PathTool.GetJointSkilNamePath(joint_effect_info[5] or self.__s_attack_state.skill_id))
			ActionManager.GetInstance():HeadAnim(joint_effect_info[1], node_head1, node_head2, node_title, joint_effect_info[4], handle_sing_effect)
		end
	end
	handle_head_anim(config_skill.joint_effect_info)
end

--怒气/合击技能瞬移检测
function Skill.SpecialSkillAngerJoint:__CheckAngerTeleporting(elapse_time)
	if not self.__s_attack_state.anger_teleporting_info then
		return
	end

	--瞬移检测
	if not self.__s_attack_state.anger_teleporting_info.toggle then
		self.__s_attack_state.anger_teleporting_info.pass_time = self.__s_attack_state.anger_teleporting_info.pass_time + elapse_time
		if self.__s_attack_state.anger_teleporting_info.pass_time >= self.__s_attack_state.anger_teleporting_info.delay_time then
			--瞬移
			local tgt_pos = self.__s_attack_state.anger_teleporting_info.tgt_pos
			self.__battle_obj:SetWorldPos(tgt_pos.x, tgt_pos.y)

			self.__s_attack_state.anger_teleporting_info.toggle = true
			if not self.__s_attack_state.anger_teleporting_info.recovery_time then
				self.__s_attack_state.anger_teleporting_info = nil
			end
		end

	--瞬移还原检测
	else
		self.__s_attack_state.anger_teleporting_info.pass_time = self.__s_attack_state.anger_teleporting_info.pass_time + elapse_time
		if self.__s_attack_state.anger_teleporting_info.pass_time >= self.__s_attack_state.anger_teleporting_info.recovery_time then
			--还原
			self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)

			self.__s_attack_state.anger_teleporting_info = nil
		end
	end
end

--怒气/合击技能瞬移预处理(为了近战类的怒气技能可以找到目标)
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillTeleportingForSearchTgts()
	self.__s_attack_state.org_pos = Vector2D.New(self.__battle_obj:GetWorldPos())

	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)	
	if config_skill and config_skill.anger_teleporting then
		--已配置anger_teleporting且技能目标逻辑为NEAR、RANGE，则先瞬移到最近的敌人处
		local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(self.__s_attack_state.skill_id)
		if tgt_logic_info and (tgt_logic_info[1] == Macros.Game.SkillTgtLogic.NEAR or tgt_logic_info[1] == Macros.Game.SkillTgtLogic.RANGE) then
			local first_tgt_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", self.__vo.obj_id, self.__s_attack_state.skill_id)
			local first_tgt = GlobalScene:GetObj(first_tgt_id)
			if first_tgt then
				self.__battle_obj:SetWorldPos(first_tgt:GetWorldPos())
			end
		end
	end
end

--怒气/合击技能瞬移预处理
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillTeleporting()
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	if config_skill and config_skill.anger_teleporting then
		--先位置还原，再延时触发瞬移
		self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)

		--根据目标左右边界，计算瞬移目标点
		local mid_world_pos_x, mid_world_pos_y = Skill.SkillLogic:CalcMiddlePosForTargets(self.__s_attack_state.target_id_map)
		if mid_world_pos_x and mid_world_pos_y then
			self.__s_attack_state.anger_teleporting_info = {}
			self.__s_attack_state.anger_teleporting_info.pass_time = 0
			self.__s_attack_state.anger_teleporting_info.delay_time = config_skill.anger_teleporting[1]
			self.__s_attack_state.anger_teleporting_info.recovery_time = config_skill.anger_teleporting[3]
			self.__s_attack_state.anger_teleporting_info.tgt_pos = Vector2D.New(mid_world_pos_x + config_skill.anger_teleporting[2] * self.__battle_obj:GetDir(), mid_world_pos_y)
		else
			self.__s_attack_state.anger_teleporting_info = nil
		end
	end
end

--怒气/合击技能结束
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillEnd()
	--怒气/合击技能(主将)
	if ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id) then
		--瞬移后的位置还原
		if self.__s_attack_state.org_pos then
			self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)
		end
		self.__s_attack_state.anger_teleporting_info = nil

		--恢复合击副将相关
		self:__HandleJointAttachedObjsRecovery()

		--怒气/合击技能的受击对象恢复
		self:__HandleAngerOrJointSkillTargetRecovery()

		--全屏暂停(恢复)
		GlobalScene:ResumeOtherObjs()

		--地图层恢复
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, true)
			ActionManager.GetInstance():StopMapBlackingWithLock(1)
		end

		--派发怒气/合击技能释放的事件
		GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE_END, self.__vo.obj_id, self.__s_attack_state.skill_id)				
	
	--合击技能(副将)
	elseif ConfigAdapter.Skill.IsJointAttachedSkill(self.__s_attack_state.skill_id) then
		--瞬移后的位置还原
		if self.__s_attack_state.org_pos then
			self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)
		end
		self.__s_attack_state.anger_teleporting_info = nil		
	end
end

--怒气/合击技能受击对象处理
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillTarget()
	self.__s_attack_state.org_zorder_map = {}
	self.__s_attack_state.handled_obj_list_for_anger_or_joint_skill = {}

	--合击副将列表
	local joint_attached_hero_map
	if ConfigAdapter.Skill.IsJointSkill(self.__s_attack_state.skill_id) then
		joint_attached_hero_map = Skill.SkillLogic:GetAttachedObjsOfJointSkill(self.__vo.obj_id)
	end
	self.__s_attack_state.joint_attached_hero_map = joint_attached_hero_map

	--将攻击者和目标外的友方、敌方隐藏
	local function handle_func(obj_list)
		local obj, obj_id, obj_vo
		for _, obj_vo in ipairs(obj_list) do
			obj_id = obj_vo.obj_id
			obj = GlobalScene:GetObj(obj_id)
			
			if obj and obj_id ~= self.__vo.obj_id then
				if joint_attached_hero_map and joint_attached_hero_map[obj_id] then
					--隐藏合击副将血条
					obj:SetHpBarVisible(false)		

					--将副将置于次顶层
					self.__s_attack_state.org_zorder_map[obj_id] = obj:GetZOrder()
					obj:SetZOrder(Scene.MaxZOrder - 1)		
				else
					if not self.__s_attack_state.target_id_map or not self.__s_attack_state.target_id_map[obj_id] then
						--隐藏其他人
						obj:SetVisible(false)
						obj:SetHpBarVisible(false)
						obj:SetForbidBeAttacked(true)
					else
						--将受击者置于次顶层
						self.__s_attack_state.org_zorder_map[obj_id] = obj:GetZOrder()
						obj:SetZOrder(Scene.MaxZOrder - 2)
					end
					table.insert(self.__s_attack_state.handled_obj_list_for_anger_or_joint_skill, obj_vo)
				end

				--隐藏身上特效
				GlobalEffectMgr:SetEffectsVisibleByTargetID(obj_id, false)										
			end
		end
	end
	local teammate_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", self.__vo.obj_id)
	local enemy_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", self.__vo.obj_id)
	handle_func(teammate_map)
	handle_func(enemy_map)

	--将攻击者置于顶层
	self.__s_attack_state.org_zorder_map[self.__vo.obj_id] = self.__battle_obj:GetZOrder()
	self.__battle_obj:SetZOrder(Scene.MaxZOrder)

	--隐藏攻击者血条
	self.__battle_obj:SetHpBarVisible(false)

	--隐藏攻击者身上特效
	GlobalEffectMgr:SetEffectsVisibleByTargetID(self.__vo.obj_id, false)
end

--怒气/合击技能受击对象还原
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillTargetRecovery()
	--将攻击者和目标外的友方、敌方隐藏
	local function handle_func(obj_list)
		if not obj_list then
			return
		end

		local obj, obj_id, obj_vo, org_zorder
		for _, obj_vo in ipairs(obj_list) do
			obj_id = obj_vo.obj_id
			obj = GlobalScene:GetObj(obj_id)
			if obj and obj_id ~= self.__vo.obj_id then
				if not self.__s_attack_state.target_id_map or not self.__s_attack_state.target_id_map[obj_id] then
					--恢复其他人
					obj:SetVisible(true)
					obj:SetHpBarVisible(true)
					obj:SetForbidBeAttacked(false)
				else
					--还原受击者层次
					org_zorder = self.__s_attack_state.org_zorder_map and self.__s_attack_state.org_zorder_map[obj_id]
					if org_zorder then
						obj:SetZOrder(org_zorder)
					end

					--恢复受击者状态
					if obj.is_real_dead then
						obj:DoDead()

					elseif not obj:IsInUnInterruptableAttack() then
						obj:DoIdle()
					end
				end

				--显示身上特效
				GlobalEffectMgr:SetEffectsVisibleByTargetID(obj_id, true)
			end
		end		
	end
	handle_func(self.__s_attack_state.handled_obj_list_for_anger_or_joint_skill)

	--还原攻击者层次
	local org_zorder = self.__s_attack_state.org_zorder_map and self.__s_attack_state.org_zorder_map[self.__vo.obj_id]
	if org_zorder then
		self.__battle_obj:SetZOrder(org_zorder)
	end
	self.__s_attack_state.org_zorder_map = nil

	--显示攻击者血条
	self.__battle_obj:SetHpBarVisible(true)

	--显示攻击者身上特效
	GlobalEffectMgr:SetEffectsVisibleByTargetID(self.__vo.obj_id, true)
end

--怒气/合击技能受击浮空
function Skill.SpecialSkillAngerJoint:__HandleAngerOrJointSkillTargetFloat()
	if not ConfigAdapter.Skill.IsAngerOrJointSkill(self.__s_attack_state.skill_id) then
		return
	end

	if self.__s_attack_state.target_id_map then
		local target
		for target_id, _ in pairs(self.__s_attack_state.target_id_map) do
			target = GlobalScene:GetObj(target_id)
			if target then
				target:DoBeAttackedFloat(self.__s_attack_state.skill_id)
			end
		end
	end
end

--处理合击副将相关
function Skill.SpecialSkillAngerJoint:__HandleJointAttachedObjs()
	if self.__s_attack_state.joint_attached_hero_map then
		local obj
		for obj_id, _ in pairs(self.__s_attack_state.joint_attached_hero_map) do
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				local obj_vo = obj:GetVO()
				obj:Resume()
				obj:PauseAI()
				obj:DoNull()
				obj:DoAttack(obj_vo:GetJointAttachedSkillID())
			end				
		end
	end
end

--恢复合击副将相关
function Skill.SpecialSkillAngerJoint:__HandleJointAttachedObjsRecovery()
	if self.__s_attack_state.joint_attached_hero_map then
		local obj
		for obj_id, _ in pairs(self.__s_attack_state.joint_attached_hero_map) do
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				obj:ResumeAI()
				--显示副将血条
				obj:SetHpBarVisible(true)
				--显示副将身上特效
				GlobalEffectMgr:SetEffectsVisibleByTargetID(obj_id, true)										
			end				
		end
	end
end

