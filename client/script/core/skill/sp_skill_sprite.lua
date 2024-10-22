
--[[
%% @module: 精灵技能特殊逻辑
%% @author: swordman sue
%% @created: 2017-05-18
--]]

Skill = Skill or {}

Skill.SpecialSkillSprite = Skill.SpecialSkillSprite or BaseClass(Skill.SpecialSkillBase)

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillSprite:DoAttack(skill_id, skill_lev)
	self:__HandleSkill(function()
		self.__battle_obj:ChangeState("attack", true)
	end)
end

function Skill.SpecialSkillSprite:UpdateStateAttack(state_name, elapse_time)
	Skill.SpecialSkillBase.UpdateStateAttack(self, state_name, elapse_time)
	
	if not self.__s_attack_state.is_anim_end then
		--技能瞬移检测
		self:__CheckAngerTeleporting(elapse_time)
	end
end

function Skill.SpecialSkillSprite:QuitStateAttack(state_name)
	self:__HandleSkillEnd()
end

--------------------------------------------------------------------------------------------------------------------------------
--受击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillSprite:DoBeAttacked(attacker_obj_id, skill_id, skill_lev, damage_time_index)
	Skill.SpecialSkillAngerJoint.DoBeAttacked(self, attacker_obj_id, skill_id, skill_lev, damage_time_index)
end

function Skill.SpecialSkillSprite:EnterStateBeAttacked(state_name)
	Skill.SpecialSkillAngerJoint.EnterStateBeAttacked(self, state_name)
end

function Skill.SpecialSkillSprite:UpdateStateBeAttacked(state_name, elapse_time)
	Skill.SpecialSkillAngerJoint.UpdateStateBeAttacked(self, state_name, elapse_time)
end

function Skill.SpecialSkillSprite:QuitStateBeAttacked(state_name)
	Skill.SpecialSkillAngerJoint.QuitStateBeAttacked(self, state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--处理技能开始
function Skill.SpecialSkillSprite:__HandleSkill(enter_attack_state_func)
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)

	--精灵技能
	if ConfigAdapter.Skill.IsSpriteSkill(self.__s_attack_state.skill_id) then
		self.__battle_obj:DoNull(nil, false, true)

		--处理技能瞬移
		self:__HandleSkillTeleporting()

		enter_attack_state_func()

		--全屏暂停
		GlobalScene:PauseOtherObjs(self.__vo.obj_id)

		--地图层变黑
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, false)
			ActionManager.GetInstance():StartMapBlackingWithLock(nil, nil, nil, nil, nil,config_skill.bg_res_name)
		end

		--派发精灵技能释放的事件
		GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE, self.__vo.obj_id, self.__s_attack_state.skill_id)						
	end
end

--技能瞬移检测
function Skill.SpecialSkillSprite:__CheckAngerTeleporting(elapse_time)
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

--技能瞬移处理
function Skill.SpecialSkillSprite:__HandleSkillTeleporting()	
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

		--根据技能命中目标逻辑/弹道逻辑，搜索所有目标
		local target_id_map = Skill.SkillLogic:SearchAllTgt(self.__vo.obj_id, self.__s_attack_state.skill_id)

		--先位置还原，再延时触发瞬移
		self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)

		--根据目标左右边界，计算瞬移目标点
		local mid_world_pos_x, mid_world_pos_y = Skill.SkillLogic:CalcMiddlePosForTargets(target_id_map)
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

--技能结束
function Skill.SpecialSkillSprite:__HandleSkillEnd()
	if ConfigAdapter.Skill.IsSpriteSkill(self.__s_attack_state.skill_id) then
		--瞬移后的位置还原
		if self.__s_attack_state.org_pos then
			self.__battle_obj:SetWorldPos(self.__s_attack_state.org_pos.x, self.__s_attack_state.org_pos.y)
		end
		self.__s_attack_state.anger_teleporting_info = nil

		--全屏暂停(恢复)
		GlobalScene:ResumeOtherObjs()

		--地图层恢复
		if not CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
			GlobalScene:SetRenderGroupsVisible({Scene.RenderQueue.GRQ_FRONT}, true)
			ActionManager.GetInstance():StopMapBlackingWithLock(1)
		end

		--派发精灵技能释放的事件
		GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE_END)
	end
end

