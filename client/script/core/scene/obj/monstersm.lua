
--[[
%% @module: 怪物对象状态机
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Scene = Scene or {}

Scene.Monster = Scene.Monster or BaseClass(Scene.BattleObj)

function Scene.Monster:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Monster:__QuitStateMove(state_name)
	Scene.BattleObj.__QuitStateMove(self, state_name)

	-- GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.MONSTER_MOVE_END, self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Monster:__EnterStateAttack(state_name)
	Scene.BattleObj.__EnterStateAttack(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_SKILL_RELEASE, self.__s_attack_state.skill_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Monster:__EnterStateDead(state_name)
	Scene.BattleObj.__EnterStateDead(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.MONSTER_DEAD, self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--出生状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Monster:DoBirth(birth_type, birth_src_pos, birth_tgt_pos)
	Scene.BattleObj.DoBirth(self, birth_type, birth_src_pos, birth_tgt_pos)

	--非自动战斗，重置怒气/合击技能触发标记
	if not self.__auto_fight then
		self:ShutdownAngerOrJointSkill()
		self:ShutdownAngerOrJointSkill(true)
	end

	--深渊boss特效
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.type == Macros.Game.FBType.CHASM then
		local config_monster = CPPGameLib.GetConfig("Monster", self.__vo.info_id)	
		if config_monster.quality >= 3 and config_monster.quality < 5  then
		    local fog_effect_id = GlobalEffectMgr:CreateScenePosEffect( { res_name = "UI_wujinguaiwu2", loop = true } , Scene.RenderQueue.GRQ_SHADOW_EFFECT)
		    self.__model:BindFollowNode(fog_effect_id, RenderObj.FollowNodeType.EFFECT_ID, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
		elseif config_monster.quality >= 5 then
		    local fog_effect_id = GlobalEffectMgr:CreateScenePosEffect( { res_name = "UI_wujinguaiwu", loop = true } , Scene.RenderQueue.GRQ_SHADOW_EFFECT)
		    self.__model:BindFollowNode(fog_effect_id, RenderObj.FollowNodeType.EFFECT_ID, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
		end
	end

	--出生后是否隐藏
	if self.__vo.is_hide_mode then
		self:SetVisible(false, true)
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.MONSTER_DO_BIRTH, self.__vo.obj_id)
end
