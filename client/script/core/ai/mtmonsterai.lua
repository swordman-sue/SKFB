
--[[
%% @module: 怪物AI(用于多人场景)
%% @author: swordman sue
%% @created: 2017-11-15
--]]

AI = AI or {}

AI.MtMonsterAI = AI.MtMonsterAI or BaseClass(AI.BaseAI)

function AI.MtMonsterAI:__init(params)
	self.__type = AI.Type.MT_MONSTER
end

function AI.MtMonsterAI:__delete()
end

function AI.MtMonsterAI:Update(now_time, elapse_time)
	local my_vo = self:__GetMyVO()
	if not my_vo or my_vo:IsInBattle() then
		return
	end

    if not self.__is_run then
        return
    end

	local me = self:__GetMe()
	if not me or me:IsDead() then
		self:StopAI()
		return
	end

	if not me:IsInState("null") and
		not me:IsInState("idle") and 
		not me:IsInState("move") and
		not me:IsInState("sp_move") then		
		return
	end

	--更新状态机
	self.__state_machine:Update(elapse_time)
end

--每场战役开启时的技能重置
function AI.MtMonsterAI:ResetSkillForBattleStart()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--更新技能CD
function AI.MtMonsterAI:__UpdateSkillCD(elapse_time)
end

--选择技能
function AI.MtMonsterAI:__SelectSkill()
end

--释放技能
function AI.MtMonsterAI:__ReleaseSkill()
end

--根据技能搜索目标
function AI.MtMonsterAI:__SearchTarget()
end

--攻击检测
function AI.MtMonsterAI:__CheckToAttack()
	return false
end

--[[
	待机状态
--]]

function AI.MtMonsterAI:__EnterStateIdle(state_name)
    local me = self:__GetMe()
    if me then
		me:DoIdle()
	end

	self.__s_idle_state.pass_time = 0
	self.__s_idle_state.total_time = 1 + MathTool.GetRandom() * 4
end

function AI.MtMonsterAI:__UpdateStateIdle(state_name, elapse_time)
	self.__s_idle_state.pass_time = self.__s_idle_state.pass_time + elapse_time
	if self.__s_idle_state.pass_time >= self.__s_idle_state.total_time then
		self:__ChangeToChase()
	end
end

--[[
	巡逻状态
--]]

function AI.MtMonsterAI:__EnterStateChase(state_name)
	self.__s_chase_state.area_size = MathTool.GetRandom(-2.5, 2.5)

	local my_vo = self:__GetMyVO()
    if my_vo and not my_vo:IsInBattle() then
	    local me = self:__GetMe()
    	local dst_pos = Vector2D.New(my_vo.org_pos_x + self.__s_chase_state.area_size, my_vo.org_pos_y)
		me:DoMove(dst_pos)				
	end
end

function AI.MtMonsterAI:__UpdateStateChase(state_name, elapse_time)
	local my_vo = self:__GetMyVO()
    if my_vo and not my_vo:IsInBattle() then
	    local me = self:__GetMe()
	    if not me:IsInState("move") then
	    	self:__ChangeToIdle()
	    end
    end
end

