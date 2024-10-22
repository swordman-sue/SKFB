
--[[
%% @module: 怪物AI
%% @author: swordman sue
%% @created: 2017-05-13
--]]

AI = AI or {}

AI.MonsterAI = AI.MonsterAI or BaseClass(AI.HeroAI)

--触发条件
local Condition = 
{
	--怪物x血量首次低于x%时（参数：怪物ID|血量百分数）
	MONSTER_HP_LOW_THAN = 1,

	--当前战役开始x秒后（参数：X秒）
	BATTLE_START_AFTER = 2,

	--死亡
	DEAD = 3,

	--任意单位死亡
	ANY_ONE_DEAD = 4,

	--公共怒气达到X（参数：X）
	ANGER_ACHIEVE = 5,

	--任意己方死亡
	ANY_TEAMMATE_DEAD = 6,

	--任意敌方死亡
	ANY_ENEMY_DEAD = 7,

	--任意单位释放技能X（参数：X）
	ANY_OBJ_RELEASE_SKILL = 8,

	--任意怪物出生
	MONSTER_DO_BIRTH = 9,
}

--检测条件
local function compare_condition(condition, condition_params, ...)
	local arg_list = {...}

	local ret = CPPGameLib.Switch(condition)
	{
		--怪物x血量首次低于x%时（参数：怪物ID|血量百分数）
		[Condition.MONSTER_HP_LOW_THAN] = function()
			local old_value = arg_list[2]
			local new_value = arg_list[3]
			if condition_params[1] == arg_list[1] and (old_value >= condition_params[2] and new_value < condition_params[2]) then
				return true
			end
		end,

		--当前战役开始x秒后（参数：X秒）
		[Condition.BATTLE_START_AFTER] = function()
			local old_time = arg_list[1]
			local new_time = arg_list[2]
			if old_time < condition_params[1] and new_time >= condition_params[1] then
				return true
			end
		end,

		--死亡
		[Condition.DEAD] = function()
			return true
		end,

		--任意单位死亡
		[Condition.ANY_ONE_DEAD] = function()
			return true
		end,

		--公共怒气达到X
		[Condition.ANGER_ACHIEVE] = function()
			local old_value = arg_list[1]
			local new_value = arg_list[2]
			if old_value < condition_params[1] and new_value >= condition_params[1] then
				return true
			end
		end,

		--任意己方死亡
		[Condition.ANY_TEAMMATE_DEAD] = function()
			return true
		end,

		--任意敌方死亡
		[Condition.ANY_ENEMY_DEAD] = function()
			return true
		end,

		--任意单位释放技能X
		[Condition.ANY_OBJ_RELEASE_SKILL] = function()
			return condition_params[1] == arg_list[1]
		end,

		--怪物出生
		[Condition.MONSTER_DO_BIRTH] = function()
			return true
		end,
	}
	return ret
end

--动作类型
--[[
注：特殊配置（0.正常技能、1.暂停非怒气技能、2.非暂停不可打断技能）
--]]
local Action = 
{
	--将x技能加入技能预备队列。（参数：技能ID|特殊配置）
	RELEASE_SKILL = 1,

	--在原地创建一个x单位并将x技能加入该单位的技能预备队列。（参数：怪物ID|技能ID|特殊配置）
	CREATE_X_OBJ = 2,

	--做一次冒泡喊话，喊话内容可配置。（参数：内容|持续时间|喊话英雄的初始战斗位置(左起第一个为1，不填则是本怪物喊话)）
	SAY = 3,

	--延时x秒后触发下一个动作。（参数：X秒）
	DELAY = 4,

	--取消某怪物的隐藏状态。(参数：怪物ID)
	CANCEL_HIDE_MODE = 5,

	--召唤一个怪物作为己方助战英雄。(参数：怪物ID)
	CALL_ASSIST_HERO = 6,
}

function AI.MonsterAI:__init(params)
	self.__type = AI.Type.MONSTER
end

function AI.MonsterAI:__delete()
end

function AI.MonsterAI:Update(now_time, elapse_time)
	AI.HeroAI.Update(self, now_time, elapse_time)

	self:__CheckDoActions(elapse_time)
end

--血量变化
function AI.MonsterAI:HpChange(info_id, old_hp, new_hp)
	local me = self:__GetMe()
	if not me or not me:GetEnteredBattleRegion() then
		return
	end

	self:__CheckCondition(Condition.MONSTER_HP_LOW_THAN, info_id, old_hp, new_hp)
end

--倒计时
function AI.MonsterAI:BattleTimeStep(old_time, new_time)
	local me = self:__GetMe()
	if not me then
		return
	end

	self:__CheckCondition(Condition.BATTLE_START_AFTER, old_time, new_time)
end

--英雄死亡
function AI.MonsterAI:HeroDead(info_id)
	local me = self:__GetMe()
	if not me or not me:GetEnteredBattleRegion() then
		return
	end

	self:__CheckCondition(Condition.ANY_ONE_DEAD)
	self:__CheckCondition(Condition.ANY_ENEMY_DEAD)
end

--怪物死亡
function AI.MonsterAI:MonsterDead(info_id)
	local me = self:__GetMe()
	if not me or not me:GetEnteredBattleRegion() then
		return
	end

	local my_vo = self:__GetMyVO()
	if my_vo.info_id == info_id then
		self:__CheckCondition(Condition.DEAD)
		return
	end

	self:__CheckCondition(Condition.ANY_ONE_DEAD)
	self:__CheckCondition(Condition.ANY_TEAMMATE_DEAD)
end

--怒气变化
function AI.MonsterAI:AngerChange(old_anger, new_anger)
	local me = self:__GetMe()
	if not me or not me:GetEnteredBattleRegion() then
		return
	end

	self:__CheckCondition(Condition.ANGER_ACHIEVE, old_anger, new_anger)
end

--任意单位释放技能
function AI.MonsterAI:ObjReleaseSkill(skill_id)
	local me = self:__GetMe()
	if not me or not me:GetEnteredBattleRegion() then
		return
	end

	self:__CheckCondition(Condition.ANY_OBJ_RELEASE_SKILL, skill_id)
end

--怪物出生
function AI.MonsterAI:MonsterDoBirth(info_id)
	local me = self:__GetMe()
	if not me then
		return
	end

	local my_vo = self:__GetMyVO()
	if my_vo.info_id == info_id then
		self:__CheckCondition(Condition.MONSTER_DO_BIRTH)
		return
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function AI.MonsterAI:__MyInit()
	if not AI.HeroAI.__MyInit(self) then
		return
	end

	--初始化特殊AI表
	local my_vo = self:__GetMyVO()
	if my_vo.ai_list then
		local config_ai_info
		for _, ai_id in ipairs(my_vo.ai_list) do
			config_ai_info = CPPGameLib.GetConfig("MonsterAI", ai_id)
			if config_ai_info then
				local ea_info = {}
				ea_info.id = ai_id
				ea_info.config_ai_info = config_ai_info
				ea_info.triggered_times = 0
				ea_info.trigger_limit_times = config_ai_info.trigger_limit_times or 1

				self.__ea_info_map = self.__ea_info_map or {}
				self.__ea_info_map[config_ai_info.condition] = self.__ea_info_map[config_ai_info.condition] or {}
				table.insert(self.__ea_info_map[config_ai_info.condition], ea_info)
			end
		end
	end
	return true
end

--选择技能
function AI.MonsterAI:__SelectSkill()
	self.__ea_skill_type = nil

	if not self:__HasEASkill() then
		AI.HeroAI.__SelectSkill(self)
		return
	end

	local ea_skill_info = self.__ea_skill_list[1]
	self.__next_skill_id = ea_skill_info.id
	self.__ea_skill_type = ea_skill_info.type	
end

--释放技能
function AI.MonsterAI:__ReleaseSkill()
	if not self:__HasEASkill() then
		AI.HeroAI.__ReleaseSkill(self)
		return
	end

	table.remove(self.__ea_skill_list, 1)

	AI.BaseAI.__ReleaseSkill(self, self.__ea_skill_type)
	self.__ea_skill_type = nil
end

--根据技能搜索目标
function AI.MonsterAI:__SearchTarget()
	if not self:__HasEASkill() then
		AI.HeroAI.__SearchTarget(self)
	end
end

--攻击检测
function AI.MonsterAI:__CheckToAttack()
	if not self:__HasEASkill() then
		return AI.HeroAI.__CheckToAttack(self)
	end

	if not self.__next_skill_id then
		return false
	end

    local me = self:__GetMe()
    if not me:CanDoAttack(self.__next_skill_id, self.__ea_skill_type) then
    	return false
    end

	if ConfigAdapter.Skill.IsAngerOrJointSkill(self.__next_skill_id) and 
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
		--有怒气/合击技能释放中，过滤
		return false
	end

	if ConfigAdapter.Skill.IsJointSkill(self.__next_skill_id) and 
		not Skill.SkillLogic:CheckJointSkillCondition(self.__obj_id) then
		--合击技能条件不满足，过滤
		self.__next_skill_id = nil
		self.__ea_skill_type = nil
		table.remove(self.__ea_skill_list, 1)		
		return false
	end

	self:__ChangeToAttack()
	return true
end

--检测触发条件
function AI.MonsterAI:__CheckCondition(condition, ...)
	if not self.__ea_info_map then
		return
	end

	local ea_info_list = self.__ea_info_map[condition]
	if not ea_info_list then
		return
	end

	local config_ai_info
	for _, ea_info in ipairs(ea_info_list) do
		if ea_info.trigger_limit_times == 0 or 
			(ea_info.trigger_limit_times > 0 and ea_info.triggered_times < ea_info.trigger_limit_times) then
			config_ai_info = ea_info.config_ai_info
			if compare_condition(condition, config_ai_info.condition_params, ...) then
				ea_info.triggered_times = ea_info.triggered_times + 1

				-- print(8, "trigger condition ", condition)
				-- CPPGameLib.PrintTable(8, config_ai_info.condition_params, "condition_params")

				for i, action in ipairs(config_ai_info.action) do
					self:__InsertAction(ea_info.id, action, config_ai_info.action_params[i])
				end
			end
		end
	end
end

--插入动作
function AI.MonsterAI:__InsertAction(ai_id, action, action_params)
	local action_info = {}
	action_info.action = action
	action_info.params = action_params

	self.__ea_action_map = self.__ea_action_map or {}
	self.__ea_action_map[ai_id] = self.__ea_action_map[ai_id] or {}
	table.insert(self.__ea_action_map[ai_id], action_info)
end

--检测执行动作
function AI.MonsterAI:__CheckDoActions(elapse_time)
	if self.__ea_action_map then
		local del_list

		for ai_id, ea_action_list in pairs(self.__ea_action_map) do
			local is_finished = true
			for _, action_info in ipairs(ea_action_list) do
				if not self:__DoAction(action_info, elapse_time) then					
					is_finished = false
					break
				end
			end
			if is_finished then
				del_list = del_list or {}
				table.insert(del_list, ai_id)
			end
		end

		if del_list then
			for _, ai_id in ipairs(del_list) do
				self.__ea_action_map[ai_id] = nil
			end
		end
	end
end

--执行动作
function AI.MonsterAI:__DoAction(action_info, elapse_time)
	if action_info.is_done then
		return true
	end

	local action_params = action_info.params
	local ret = CPPGameLib.Switch(action_info.action)
	{
		--将x技能加入技能预备队列。（参数：技能ID|特殊配置）
		[Action.RELEASE_SKILL] = function()
			local ea_skill_info = {}
			ea_skill_info.id = action_params[1]
			ea_skill_info.type = action_params[2]
			self.__ea_skill_list = {}
			table.insert(self.__ea_skill_list, ea_skill_info)
			return true
		end,

		--在原地创建一个x单位并将x技能加入该单位的技能预备队列。（参数：怪物ID|技能ID|特殊配置）
		[Action.CREATE_X_OBJ] = function()
			local ea_skill_info = {}
			ea_skill_info.id = action_params[2]
			ea_skill_info.type = action_params[3]

			local monster_info = Modules.BattleFormula:BuildBattleMonsterInfo(action_params[1])
			local vo = Scene.MonsterVO.New()
			vo.is_x_monster = true
			vo:ParseInfo(monster_info)
			vo.dir = MathTool.HORZ_LEFT
			vo.show_hp_bar = false
			vo.show_shadow = false

			local me = self:__GetMe()
			vo.org_pos_x, vo.org_pos_y = me:GetLogicPos()			
		
			GlobalScene:CreateXMonster(vo, ea_skill_info)
			return true
		end,

		--做一次冒泡喊话，喊话内容可配置。（参数：内容|持续时间|喊话英雄的初始战斗位置(左起第一个为1，不填则是本怪物喊话)）
		[Action.SAY] = function()
			if action_params[3] then
				local hero_vo_map = GlobalVOMgr:GetHeroMap()
				for _, hero_vo in pairs(hero_vo_map) do
					if hero_vo.pos_index == action_params[3] then
						local hero = GlobalScene:GetObj(hero_vo.obj_id)
						if hero then
							hero:ShowAISayEffect(action_params[1], action_params[2])
							return true
						end
					end
				end
			else
				local me = self:__GetMe()
				if me then
					me:ShowAISayEffect(action_params[1], action_params[2])
					return true
				end
			end
		end,

		--延时x秒后触发下一个动作。（参数：X秒）
		[Action.DELAY] = function()
			action_info.elapse_time = action_info.elapse_time or 0
			action_info.elapse_time = action_info.elapse_time + elapse_time
			if action_info.elapse_time < action_info.params[1] then
				return false
			end
			return true
		end,

		--取消某怪物的隐藏状态。(参数：怪物ID)
		[Action.CANCEL_HIDE_MODE] = function()
			local monster_vo_map = GlobalVOMgr:GetMonsterMap()
			for _, monster_vo in pairs(monster_vo_map) do
				if monster_vo.info_id == action_params[1] then
					local monster = GlobalScene:GetObj(monster_vo.obj_id)
					if monster then
						monster_vo.is_hide_mode = nil
						monster:SetVisible(true)
						return true
					end
				end
			end
		end,

		--召唤一个怪物作为己方助战英雄。(参数：怪物ID)
		[Action.CALL_ASSIST_HERO] = function()
			return CallModuleFunc(ModuleType.BATTLE, "CreateMonsterAssist", action_params[1])		
		end,
	}
	if not ret then
		return
	end

	-- print(8, "do action ", action_info.action)
	-- CPPGameLib.PrintTable(8, action_info.params, "action_params")

	action_info.is_done = ret
	return ret
end

--是否有特殊AI的技能
function AI.MonsterAI:__HasEASkill()
	return self.__ea_skill_list and #self.__ea_skill_list > 0
end
