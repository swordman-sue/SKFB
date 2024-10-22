
Scene = Scene or {}

--[[
%% @module: 战斗属性
%% @author: swordman sue
%% @created: 2016-07-15
--]]
Scene.ObjBattleAttr = Scene.ObjBattleAttr or BaseClass()

function Scene.ObjBattleAttr:__init()
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		self[prop_key] = 0
	end
end

function Scene.ObjBattleAttr:__delete()
end

function Scene.ObjBattleAttr:ParseData(data)	
	local prop_value
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		prop_value = data[prop_key]
		if prop_value and prop_value ~= 0 then
			self[prop_key] = prop_value
		end
	end
end

--设置属性值
function Scene.ObjBattleAttr:SetAttr(type, val)
    local attr_name = Macros.Game.BattlePropKey[type]
    if not attr_name then
    	LogTool.LogError("[Scene.ObjBattleAttr:SetAttr] 找不到对应的战斗属性类型(type:%d)，请到Macros.Game.BattleProp中定义", type)
    	return
    end

    if type == Macros.Game.BattleProp.SPEED then
    	self[attr_name] = math.max(0, val)
    else
	    self[attr_name] = val
	end
end

--获取属性值
function Scene.ObjBattleAttr:GetAttr(type, show_error)
    local attr_name = Macros.Game.BattlePropKey[type]
    if not attr_name then
    	if show_error ~= false then
	    	LogTool.LogError("[Scene.ObjBattleAttr:GetAttr] 找不到对应的战斗属性类型(type:%d)，请到Macros.Game.BattleProp中定义", type)
	    end
    	return 0 
    end	
    return self[attr_name] or 0
end

--克隆
function Scene.ObjBattleAttr:Clone()
	local new_attr = Scene.ObjBattleAttr.New()
	for prop_type, _ in pairs(Macros.Game.BattlePropKey) do
		new_attr:SetAttr(prop_type, self:GetAttr(prop_type))
	end	
	return new_attr
end

--[[
%% @module: 助战英雄战斗属性
%% @author: swordman sue
%% @created: 2017-05-11
--]]
Scene.HeroAssistBattleAttr = Scene.HeroAssistBattleAttr or BaseClass(Scene.ObjBattleAttr)

function Scene.HeroAssistBattleAttr:__init()
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		self[prop_key] = 0
	end
end

function Scene.HeroAssistBattleAttr:__delete()
end

function Scene.HeroAssistBattleAttr:ParseData(prop_list)
	local prop_map = {}
	for _, prop_info in ipairs(prop_list) do
		prop_map[prop_info.type] = prop_map[prop_info.type] or 0
		prop_map[prop_info.type] = prop_map[prop_info.type] + prop_info.value
	end

	local prop_value
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		prop_value = prop_map[prop_type]
		if prop_value and prop_value ~= 0 then
			self[prop_key] = prop_value
		end
	end
end

--克隆
function Scene.HeroAssistBattleAttr:Clone()
	local new_attr = Scene.HeroAssistBattleAttr.New()
	for prop_type, _ in pairs(Macros.Game.BattlePropKey) do
		new_attr:SetAttr(prop_type, self:GetAttr(prop_type))
	end	
	return new_attr
end

--[[
%% @module: 战斗对象vo
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene.BattleObjVO = Scene.BattleObjVO or BaseClass(Scene.ObjVO)

function Scene.BattleObjVO:__init()
	--职业
	self.profession = 0
	--生命
	self.hp = 0
	--生命上限
	self.max_hp = 0
	--战斗属性
	self.battle_attr = Scene.ObjBattleAttr.New()
	--技能等级
	self.skill_level = 1
	--总技能列表
	self.skill_list = nil
	--模型缩放系数
	self.model_scale = 1
	--释放怒气/合击技能所需消耗
	self.cost_of_anger_or_joint_skill = 0
	--怒气/合击技能间公共CD
	self.anger_or_joint_common_cd_time = 0
	--远程职业公共CD
	self.common_cd_time = ConfigAdapter.Common.GetSkillCommonCD()
end

function Scene.BattleObjVO:__delete()
	if self.battle_attr then
		self.battle_attr:DeleteMe()
		self.battle_attr = nil
	end
end

--是否近战类职业
function Scene.BattleObjVO:IsProfessionNear()
	return Macros.Game.IsProfessionNear(self.profession)
end

--获取合击技能ID(副将)
function Scene.BattleObjVO:GetJointAttachedSkillID()
	if self.skill_list then
		for _, skill_id in ipairs(self.skill_list) do			
			if ConfigAdapter.Skill.IsJointAttachedSkill(skill_id) then
				return skill_id
			end
		end
	end
	return 0
end

--解析技能列表
function Scene.BattleObjVO:ParseSkillList()
	if not self.skill_list then
		return
	end

	local skill_type
	for _, skill_id in ipairs(self.skill_list) do
		skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)

		--记录远程职业的近身技能
		if skill_type == Macros.Game.SkillType.NEAR and not self:IsProfessionNear() then
			self.skill_id_of_near_skill = skill_id
		end

		--记录怒气、真.怒气技能、合击技能、真.合击技能
		if skill_type == Macros.Game.SkillType.ANGER then
			self.skill_id_of_anger_skill = skill_id
		elseif skill_type == Macros.Game.SkillType.ANGER_PLUS then
			self.skill_id_of_anger_plus_skill = skill_id

		elseif skill_type == Macros.Game.SkillType.JOINT then
			self.skill_id_of_joint_skill = skill_id
		elseif skill_type == Macros.Game.SkillType.JOINT_PLUS then
			self.skill_id_of_joint_plus_skill = skill_id
		end
	end
end

function Scene.BattleObjVO:GetAttr(type, show_error)
	if self.battle_attr then
		return self.battle_attr:GetAttr(type, show_error)
	end
	return 0
end
