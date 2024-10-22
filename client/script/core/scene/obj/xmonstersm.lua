--[[
%% @module: X怪物对象
%% @author: swordman sue
%% @created: 2017-05-15
%% @description: 在原地创建一个x怪物，释放技能后立即销毁
--]]

Scene = Scene or {}

Scene.XMonster = Scene.XMonster or BaseClass(Scene.BattleObj)

function Scene.XMonster:CanDoNull()
	return true
end

function Scene.XMonster:CanDoIdle()
	return false
end

function Scene.XMonster:CanDoRepel(skill_id)
	return false
end

function Scene.XMonster:CanDoBeAttacked()
	return false
end

function Scene.XMonster:CanDoBeAttackedFloat()
	return false
end

function Scene.XMonster:CanDoSpecialMove()
	return false
end

function Scene.XMonster:CanDoDead()
	return false
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.XMonster:CanDoAttack(skill_id)
	return true
end

function Scene.XMonster:__EnterStateAttack(state_name)
	Scene.BattleObj.__EnterStateAttack(self, state_name)

	self.__model:SetVisible(false, self.__body_anim_play_info.inner_layer)
end

function Scene.XMonster:__QuitStateAttackHelper()
	self:__CheckAttackEx()

	self:DoNull()
end

--------------------------------------------------------------------------------------------------------------------------------
--空状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.XMonster:__UpdateStateNull(state_name, elapse_time)
	if not self.__s_null_state.is_delete then
		self.__s_null_state.is_delete = true
		GlobalScene:DeleteObj(self.__vo.obj_id)
	end
end