
--[[
%% @module: 精灵对象状态机
%% @author: swordman sue
%% @created: 2017-01-19
--]]

Scene = Scene or {}

Scene.Spirit = Scene.Spirit or BaseClass(Scene.BattleObj)

function Scene.Spirit:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)

	--出生状态
	local function birth_enter_func(state_name)
		self["__EnterStateBirth"](self, state_name)
	end
	local function birth_update_func(state_name, elapse_time)
		self["__UpdateStateBirth"](self, state_name, elapse_time)
	end
	local function birth_quit_func(state_name)
		self["__QuitStateBirth"](self, state_name)
	end
	tmp_state = self.__state_machine:CreateNormalState("birth", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(birth_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(birth_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(birth_quit_func))
end

function Scene.Spirit:CanDoRepel(skill_id)
	return false
end

function Scene.Spirit:CanDoBeAttacked()
	return false
end

function Scene.Spirit:CanDoDead()
	return false
end

function Scene.Spirit:CanDoBeAttackedFloat()
	return false
end

--------------------------------------------------------------------------------------------------------------------------------
--待机状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Spirit:__EnterStateIdle(state_name)
	Scene.BattleObj.__EnterStateIdle(self, state_name)

	self:__ShowMountEffect(false)
end

function Scene.Spirit:__QuitStateIdle(state_name)
	Scene.BattleObj.__QuitStateIdle(self, state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--出生状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Spirit:CanDoBirth()
	if self:IsInState("birth") then
		return
	end

	return true
end

function Scene.Spirit:DoBirth()
	if not self:CanDoBirth() then
		return
	end

	self.__s_birth_state = {}
    self:ChangeState("birth", true)	
end

function Scene.Spirit:__EnterStateBirth(state_name)
	self:SetInBirth(true)
	self:SetSkipBoundTest(true)
	self:ResetAISkillForBattleStart()
	self:PauseAI()

	self:__ShowMountEffect(true)
end

function Scene.Spirit:__UpdateStateBirth(state_name, elapse_time)
	self:DoNull(nil, nil, true)
	if self.__vo.appear_skill_id then
		self:DoAttack(self.__vo.appear_skill_id)
	end
end

function Scene.Spirit:__QuitStateBirth(state_name)
	self.__s_birth_state = nil

	self:SetInBirth(false)
	self:SetSkipBoundTest(false)
	self:SetEnteredBattleRegion(true)
	self:ResumeAI()

	local flag = self:CheckBindObjType(Scene.ObjType.HERO, Scene.ObjType.HERO_PVP)
	self:SetDir(flag and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Spirit:__EnterStateMove(state_name)
	Scene.BattleObj.__EnterStateMove(self, state_name)

	local dir = self.__dir
	self:SetDir(MathTool.HORZ_INVALID)
	self:SetDir(dir)

	self:__ShowMountEffect(true)
end

function Scene.Spirit:__QuitStateMove(state_name)
	Scene.BattleObj.__QuitStateMove(self, state_name)

	local flag = self:CheckBindObjType(Scene.ObjType.HERO, Scene.ObjType.HERO_PVP)
	self:SetDir(flag and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT)
end

--------------------------------------------------------------------------------------------------------------------------------
--特殊移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Spirit:__EnterStateSpecialMove(state_name)
	Scene.BattleObj.__EnterStateSpecialMove(self, state_name)

	local dir = self.__dir
	self:SetDir(MathTool.HORZ_INVALID)
	self:SetDir(dir)

	self:__ShowMountEffect(true)
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Spirit:CanDoAttack(skill_id, sp_skill_type, ignore_state)
	if self:GetInBirth() then
		return false
	end
	
	return Scene.BattleObj.CanDoAttack(self, skill_id, sp_skill_type, ignore_state)	
end

function Scene.Spirit:__EnterStateAttack(state_name)
	Scene.BattleObj.__EnterStateAttack(self, state_name)

	self:__ShowMountEffect(false)
end

function Scene.Spirit:__TriggerAttackMove()
end
