
--[[
%% @module: 弹道对象状态机
%% @author: swordman sue
%% @created: 2016-07-21
--]]

Scene = Scene or {}

Scene.Bullet = Scene.Bullet or BaseClass(Scene.BattleObj)

function Scene.Bullet:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)

	--抛物追击状态
	local function parabola_chase_enter_func(state_name)
		self["__EnterStateParabolaChase"](self, state_name)
	end
	local function parabola_chase_update_func(state_name, elapse_time)
		self["__UpdateStateParabolaChase"](self, state_name, elapse_time)
	end
	local function parabola_chase_quit_func(state_name)
		self["__QuitStateParabolaChase"](self, state_name)
	end
	local tmp_state = self.__state_machine:CreateNormalState("parabola_chase", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(parabola_chase_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(parabola_chase_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(parabola_chase_quit_func))
end

function Scene.Bullet:CanDoRepel(skill_id)
	return false
end

function Scene.Bullet:CanDoAttack()
	return false
end

function Scene.Bullet:CanDoBeAttacked()
	return false
end

function Scene.Bullet:CanDoDead()
	return false
end

function Scene.Bullet:CanDoSpecialMove()
	return false
end

function Scene.Bullet:CanDoBeAttackedFloat()
	return false
end

--触发攻击逻辑
function Scene.Bullet:AttackTargets(target_id)
	--若由副将的合击技能触发，则不用处理伤害
	local do_damage = not ConfigAdapter.Skill.IsJointAttachedSkill(self.__vo.create_skill_id)

	--单体攻击
	local do_damage_once = self.__vo.can_penetrate == Macros.Global.FALSE and not self.__vo.bomb_effect_id
	if do_damage_once and self.__has_do_damage then
		return
	end

	--攻击指定目标
	local target = GlobalScene:GetObj(target_id)
	if target and target:CanDoBeAttacked() and not self.__attacked_obj_ids[target_id] then
		self:__HandleDamage(target_id, do_damage, do_damage_once)
	end

	--攻击范围目标
	if self.__vo.attack_range > 0 then
		local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", self.__vo.create_obj_id, self.__vo.create_skill_id)
		if target_list then
			local is_hit
			local target, target_world_pos
			for _, vo in ipairs(target_list) do
				target = GlobalScene:GetObj(vo.obj_id)
				if target and target:CanDoBeAttacked() and not self.__attacked_obj_ids[vo.obj_id] then
					target_world_pos = target:GetWorldPosVec()
					--攻击范围检测
					if check_in_range(self.__world_pos, target_world_pos, -self.__vo.attack_range, self.__vo.attack_range) then
						is_hit = true
						self:__HandleDamage(vo.obj_id, do_damage, do_damage_once)
					end
				end
			end

			--爆炸特效(可爆炸的弹道不能穿透)
			if is_hit and self.__vo.bomb_effect_id and self.__vo.can_penetrate == Macros.Global.FALSE then
				self:__PlayBombEffect()				
			end
		end
	end

	--无法穿透，则完成首次攻击后，切换到空状态
	if self.__vo.can_penetrate == Macros.Global.FALSE then
		self:DoNull()
	end		
end

--触发伤害
function Scene.Bullet:__HandleDamage(obj_id, do_damage, do_damage_once)
	if not do_damage then
		return
	end
	if do_damage_once and self.__has_do_damage then
		return
	end

	if not self.__has_do_damage then
		--首次伤害后，触发震动
		self.__shake_list = Skill.SkillLogic:GenShakeInfoList(self.__vo.shake_info, true)
		Skill.SkillLogic:CheckShake(self.__shake_list, 0)

		--记录本帧中的BUFF检测(首次伤害后)
		if self.__vo.can_check_buff and self.__vo.do_once_check_buff then
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", self.__vo.create_obj_id, self.__vo.create_skill_id)	
		end
	end
	self.__has_do_damage = true

	--记录本帧中的BUFF检测(每次伤害后)
	if self.__vo.can_check_buff and not self.__vo.do_once_check_buff then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", self.__vo.create_obj_id, self.__vo.create_skill_id)	
	end	

	self.__attacked_obj_ids[obj_id] = true
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordBeattackedObjCurFrame", obj_id, self.__vo.create_obj_id, self.__vo.create_skill_id, self.__vo.create_skill_lev)			
end

--------------------------------------------------------------------------------------------------------------------------------
--空状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Bullet:__UpdateStateNull(state_name, elapse_time)
	GlobalScene:DeleteObj(self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Bullet:__EnterStateMove(state_name)
	self:__CreateEffect()
end

function Scene.Bullet:__UpdateStateMove(state_name, elapse_time)
	Scene.BattleObj.__UpdateStateMove(self, state_name, elapse_time)

	--与首个目标进行碰撞检测
	local first_target_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", self.__vo.create_obj_id, self.__vo.create_skill_id)
	local first_target = GlobalScene:GetObj(first_target_id)
	if first_target then
		first_target_world_pos = first_target:GetWorldPosVec()
		if (self.__dir == MathTool.HORZ_RIGHT and self.__world_pos.x < first_target_world_pos.x - self.__vo.trigger_range) or 
			(self.__dir == MathTool.HORZ_LEFT and self.__world_pos.x > first_target_world_pos.x + self.__vo.trigger_range) then
			return
		end
	end

	--与所有目标进行碰撞检测
	self.__last_world_pos_x = self.__last_world_pos_x or self.__world_pos.x	
	local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", self.__vo.create_obj_id, self.__vo.create_skill_id)
	if target_list then
		local target, target_id, target_world_pos
		for _, vo in ipairs(target_list) do
			target_id = vo.obj_id
			target = GlobalScene:GetObj(target_id)
			if target and not self.__attacked_obj_ids[target_id] then
				target_world_pos = target:GetWorldPosVec()

				if self.__dir == MathTool.HORZ_RIGHT then
					--朝右
					if self.__world_pos.x >= target_world_pos.x - self.__vo.trigger_range then
						self:AttackTargets(target_id)
						break
					end
				else
					--朝左
					if self.__world_pos.x <= target_world_pos.x + self.__vo.trigger_range then
						self:AttackTargets(target_id)
						break
					end
				end
			end
		end
	end
	self.__last_world_pos_x = self.__world_pos.x
end

function Scene.Bullet:__QuitStateMoveHelper()
    self:DoNull()
end

--------------------------------------------------------------------------------------------------------------------------------
--抛物追击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Bullet:CanDoParabolaChase()
    if self:IsDead() then
        return false
    end

    if not self:IsInState("null") and 
        not self:IsInState("idle") then 
        return false
    end

    return true
end

function Scene.Bullet:DoParabolaChase(end_pos)
	if not self:CanDoParabolaChase() then
		return
	end

    --起止点过近
    if self.__world_pos:IsNear(end_pos, 0.5) then
        self:SetWorldPos(end_pos.x, end_pos.y)
        return
    end

    --边界处理
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", end_pos, nil, self.__vo.obj_id)

    self.__s_parabola_chase_state = self.__s_parabola_chase_state or {}
    self.__s_parabola_chase_state.beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
    self.__s_parabola_chase_state.end_pos = GlobalScene:LogicToWorldVec(end_pos)
    self.__s_parabola_chase_state.move_dir_x = self:__GetParabolaChaseDirX()
    self.__s_parabola_chase_state.pass_time = 0
    self.__s_parabola_chase_state.pass_distance = 0
    self.__s_parabola_chase_state.speed = speed or self:GetMoveSpeed()

    self:ChangeState("parabola_chase", true)    
end

function Scene.Bullet:__EnterStateParabolaChase(state_name)	
	self:__CreateEffect()
end

function Scene.Bullet:__UpdateStateParabolaChase(state_name, elapse_time)
    self.__s_parabola_chase_state.pass_time = self.__s_parabola_chase_state.pass_time + elapse_time

    local fixed_factor = 4
    local parabola_factor = self.__vo.parabola_factor
    local speed = self.__s_parabola_chase_state.speed
    local time = self.__s_parabola_chase_state.pass_time
    local dis_x = self:__GetParabolaChaseDisX()

	--计算X轴、Y轴偏移
	-- y轴偏移公式：
	-- y = math.ceil( -800 * v * v / L / L * t * t  + 800 * v / L * t )
	local move_dis_x = self.__s_parabola_chase_state.pass_time * self.__s_parabola_chase_state.speed * self.__s_parabola_chase_state.move_dir_x
    local move_dis_y = math.ceil(-fixed_factor * parabola_factor * speed * speed / dis_x / dis_x * time * time  + fixed_factor * parabola_factor * speed / dis_x * time)
	local now_pos = Vector2D.New(self.__s_parabola_chase_state.beg_pos.x + move_dis_x, self.__s_parabola_chase_state.beg_pos.y - move_dis_y)

	--计算斜率、旋转角度
	local effect_root = GlobalEffectMgr:GetEffect(self.__effect_id)
	if effect_root then
		local slope = (now_pos.y - self.__world_pos.y) / (now_pos.x - self.__world_pos.x)
		local degree = math.deg(math.atan(slope))
		effect_root:SetRotation(degree)
	end

	--更新位置
	self:SetWorldPos(now_pos.x, now_pos.y)

	--抛物运动结束
	if move_dis_y <= 0 then
	    self:__QuitStateParabolaChaseHelper()
	end
end

function Scene.Bullet:__QuitStateParabolaChase(state_name)
    if self.__s_parabola_chase_state.end_callback then
        self.__s_parabola_chase_state.end_callback(self.__vo.obj_id)
    end
    self.__s_parabola_chase_state = nil
end

function Scene.Bullet:__QuitStateParabolaChaseHelper()
	self:AttackTargets(self.__vo.parabola_tgt_id)
	self:DoNull()
end

--获取抛物追击目标
function Scene.Bullet:__GetParabolaChaseTgt()
	local parabola_tgt = GlobalScene:GetObj(self.__vo.parabola_tgt_id)
	return parabola_tgt
end

--获取抛物追击目标点
function Scene.Bullet:__GetParabolaChaseTgtPos()
	local parabola_tgt = GlobalScene:GetObj(self.__vo.parabola_tgt_id)
	if parabola_tgt then
		self.__parabola_tgt_pos = parabola_tgt:GetWorldPosVec()
	end
	return self.__parabola_tgt_pos
end

--获取抛物追击目标点与弹道发射点之间的横向距离
function Scene.Bullet:__GetParabolaChaseDisX()
	local parabola_tgt_pos = self:__GetParabolaChaseTgtPos()
	return math.abs(parabola_tgt_pos.x - self.__s_parabola_chase_state.beg_pos.x)
end

--获取弹道发射点到抛物追击目标点的向量
function Scene.Bullet:__GetParabolaChaseDirX()
	local parabola_tgt_pos = self:__GetParabolaChaseTgtPos()
	return MathTool.CalcHorzDirBetweenPoints(self.__world_pos, parabola_tgt_pos)
end

--创建弹道特效
function Scene.Bullet:__CreateEffect()
	if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
		return
	end

	if self.__vo.effect_id then
	    local create_obj = self:GetCreateObj()
	    self.__effect_id = GlobalEffectMgr:CreateSceneTargetEffect(
	    	{id = self.__vo.effect_id, dir = create_obj:GetDir(), loop = true}, self.__vo.obj_id, RenderObj.InnerLayer.BODY)
	end
end

--播放爆炸特效
function Scene.Bullet:__PlayBombEffect()
	if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
		return
	end

	local effect_x, effect_y = GlobalScene:WorldToPixel(self:GetWorldPos())
	GlobalEffectMgr:CreateScenePosEffect(
		{id = self.__vo.bomb_effect_id, pos_x = effect_x, pos_y = effect_y, dir = self.__dir}, Scene.RenderQueue.GRQ_AIR)		
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
check_in_range = function(my_world_pos, world_pos, range1, range2)
	local trigger_range_min = math.min(range1, range2)
	local trigger_range_max = math.max(range1, range2)
	if world_pos.x < my_world_pos.x + trigger_range_min or world_pos.x > my_world_pos.x + trigger_range_max then
		return false
	end
	return true
end


