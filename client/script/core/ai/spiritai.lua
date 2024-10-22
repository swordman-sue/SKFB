
--[[
%% @module: 精灵AI
%% @author: swordman sue
%% @created: 2017-01-19
--]]

AI = AI or {}

AI.SpiritAI = AI.SpiritAI or BaseClass(AI.BaseAI)

function AI.SpiritAI:__init(params)
	self.__type = AI.Type.SPIRIT
end

function AI.SpiritAI:__delete()
end

function AI.SpiritAI:Update(now_time, elapse_time)	
	local me = self:__GetMe()
	if not me then
		return
	end
	
	if me:GetInBirth() then
		return
	end

	if me:IsInState("sp_move") then
		return
	end

	AI.BaseAI.Update(self, now_time, elapse_time)
end

--每场战役开启时的技能重置
function AI.SpiritAI:ResetSkillForBattleStart()
	self.__next_skill_id = nil
	self.__common_cd_info.in_cd = false

	for _, skill_info in ipairs(self.__skill_list) do
		skill_info.in_cd = false
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--选择技能
function AI.SpiritAI:__SelectSkill()
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

--根据技能搜索目标
function AI.SpiritAI:__SearchTarget()
end

--[[
	待机状态
--]]
function AI.SpiritAI:__EnterStateIdle(state_name)
end

--[[
	巡逻状态
--]]
function AI.SpiritAI:__EnterStateChase(state_name)
	self.__s_chase_state = {}
	self.__s_chase_state.pass_time = 0
	self.__s_chase_state.next_time = 0
end

function AI.SpiritAI:__UpdateStateChase(state_name, elapse_time)
	if self:__CheckToAttack() then
		return
	end

	local me = self:__GetMe()
	if me:IsInState("move") then
		return
	end

	local last_hero_obj_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetLastTeammate", self.__obj_id)
	local last_hero = GlobalScene:GetObj(last_hero_obj_id)
	if not last_hero then
		return
	end
	local last_hero_wpos = last_hero:GetWorldPosVec()

	if not self.__s_chase_state.toggle then
		--检测坐标变化，判断是否进入跟随
		local my_world_pos_x = me:GetWorldPosX()
		if math.abs(my_world_pos_x - last_hero_wpos.x) > Scene.SpiritFollowSpace then
			self.__s_chase_state.toggle = true
			self.__s_chase_state.pass_time = 0
			self.__s_chase_state.next_time = MathTool.GetRandom(0, 0.5)
		end
	else
		--检测是否到时间进入跟随
		self.__s_chase_state.pass_time = self.__s_chase_state.pass_time + elapse_time
		if self.__s_chase_state.pass_time >= self.__s_chase_state.next_time then
			self.__s_chase_state.toggle = false
			self.__s_chase_state.pass_time = 0
			local tgt_wpos = Vector2D.New(last_hero_wpos.x + Scene.SpiritOffset.x * me:GetDir(), last_hero_wpos.y + Scene.SpiritOffset.y)
			me:DoMove(tgt_wpos, true)
		end
	end
end

--[[
	攻击状态
--]]
function AI.SpiritAI:__CheckToAttack()
	if not self.__next_skill_id then
		return
	end

	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") then
		--有参战对象未进入视野，过滤
		return
	end

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
		--有参战对象释放怒气/合击技能中
		return
	end

	local me = self:__GetMe()
	if not me:CanDoAttack(self.__next_skill_id) then
		return
	end

	self:__ChangeToAttack()
	return true		
end
