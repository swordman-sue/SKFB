
--[[
%% @module: 助战英雄对象状态机
%% @author: swordman sue
%% @created: 2017-05-11
--]]

Scene = Scene or {}

Scene.HeroAssist = Scene.HeroAssist or BaseClass(Scene.BattleObj)

function Scene.HeroAssist:__InitStateMachine()
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

function Scene.HeroAssist:CanDoNull()
	return true
end

function Scene.HeroAssist:CanDoIdle()
	return false
end

function Scene.HeroAssist:CanDoRepel(skill_id)
	return false
end

function Scene.HeroAssist:CanDoBeAttacked()
	return false
end

function Scene.HeroAssist:CanDoBeAttackedFloat()
	return false
end

function Scene.HeroAssist:CanDoSpecialMove()
	return false
end

function Scene.HeroAssist:CanDoDead()
	return false
end

--------------------------------------------------------------------------------------------------------------------------------
--出生状态
--------------------------------------------------------------------------------------------------------------------------------
local function handle_objs_visible(obj_id, skill_id, var)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return
	end
	
	if config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND then
		--隐藏敌方
		local enemy
		local enemy_vo_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyMap", obj_id)
		for obj_id, _ in pairs(enemy_vo_map) do
			enemy = GlobalScene:GetObj(obj_id)
			if enemy then
				enemy:SetVisible(var)
			end
		end
	else
		--隐藏己方
		local teammate
		local teammate_vo_map = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateMap", obj_id)
		for obj_id, _ in pairs(teammate_vo_map) do
			teammate = GlobalScene:GetObj(obj_id)
			if teammate then
				teammate:SetVisible(var)
			end
		end
	end
end

function Scene.HeroAssist:CanDoBirth()
	return true
end

function Scene.HeroAssist:DoBirth()
	if not self:CanDoBirth() then
		return
	end

	self.__s_birth_state = {}
    self:ChangeState("birth", true)	
end

function Scene.HeroAssist:__EnterStateBirth(state_name)
	self:SetInBirth(true)
	self:SetSkipBoundTest(true)
    self:PlayAnimation(RenderObj.ActionIndex.IDLE)

	--[[
		全屏暂停、隐藏己方英雄、黑屏特效
	--]]
	--全屏暂停
	GlobalScene:PauseOtherObjs(self.__vo.obj_id)    

	--隐藏精灵
	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if sprite then
		sprite:SetVisible(false)
	end

	--隐藏己方/敌方
	handle_objs_visible(self.__vo.obj_id, self.__vo.skill_id_of_anger_skill, false)

	--黑屏特效
	local config_skill = CPPGameLib.GetConfig("Skill", self.__vo.skill_id_of_anger_skill)
	if config_skill and config_skill.bg_res_name then
		ActionManager.GetInstance():StartMapBlackingWithLock(nil, nil, nil, nil, nil, config_skill.bg_res_name)
	end

	--添加烟尘特效
	GlobalEffectMgr:CreateSceneTargetEffect({id = 100056, dir = self.__dir}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)

	--延时释放技能
	self:InsertDelayAction(function()
		self:__QuitStateBirthHelper()
	end, 0.5)
end

function Scene.HeroAssist:__UpdateStateBirth(state_name, elapse_time)
end

function Scene.HeroAssist:__QuitStateBirth(state_name)
	self.__s_birth_state = nil

	self:SetInBirth(false)
	self:SetEnteredBattleRegion(true)
	self:SetDir(MathTool.HORZ_RIGHT)

	--显示精灵
	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if sprite then
		sprite:SetVisible(true)	
	end
end

function Scene.HeroAssist:__QuitStateBirthHelper()
	--显示己方/敌方
	handle_objs_visible(self.__vo.obj_id, self.__vo.skill_id_of_anger_skill, true)

	self:InsertDelayAction(function()
	    self:DoAttack(self.__vo.skill_id_of_anger_skill)
	end)
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.HeroAssist:CanDoAttack(skill_id)
	return true
end

function Scene.HeroAssist:__QuitStateAttackHelper()
	self:DoNull()
end

--------------------------------------------------------------------------------------------------------------------------------
--空状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.HeroAssist:__UpdateStateNull(state_name, elapse_time)
	if not self.__s_null_state.is_delete then
		self.__s_null_state.is_delete = true
		GlobalScene:DeleteObj(self.__vo.obj_id)
	end
end
