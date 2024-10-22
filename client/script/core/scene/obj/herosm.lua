
--[[
%% @module: 英雄对象状态机
%% @author: swordman sue
%% @created: 2016-07-13
--]]

Scene = Scene or {}

Scene.Hero = Scene.Hero or BaseClass(Scene.BattleObj)

function Scene.Hero:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Hero:__QuitStateMove(state_name)
	Scene.BattleObj.__QuitStateMove(self, state_name)

	-- GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.HERO_MOVE_END, self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Hero:__EnterStateDead(state_name)
	Scene.BattleObj.__EnterStateDead(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.HERO_DEAD, self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--出生状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Hero:DoBirth(birth_type, birth_src_pos, birth_tgt_pos)
	Scene.BattleObj.DoBirth(self, birth_type, birth_src_pos, birth_tgt_pos)

	--非自动战斗，重置怒气/合击技能触发标记
	if not self.__auto_fight then
		self:ShutdownAngerOrJointSkill()
		self:ShutdownAngerOrJointSkill(true)
	end

	if self.__vo.say_info then
		self:InsertDelayAction(function()
			self:ShowAISayEffect(self.__vo.say_info[2])
		end, self.__vo.say_info[1])
	end
end
