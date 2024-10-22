
--[[
%% @module: 战斗公式
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.BattleFormula = Modules.BattleFormula or {}

--血量改变来源
Modules.BattleFormula.HPChangeSrc = 
{
	SKILL = 0,
	BUFF = 1,
}

--血量改变类型
Modules.BattleFormula.HPChangeType = 
{
	--伤害
	DAMAGE = 1,
	--治疗
	TREATMENT = 2,
}

--伤害类型
Modules.BattleFormula.DamageType = 
{
	--闪避
	DODGE = 0,
	--格挡
	BLOCK = 1,
	--暴击
	CRIT = 2,
	--普通
	NORMAL = 3,
}

--战斗加成类属性表
Modules.BattleFormula.BattleAdditionalPropMap = 
{
	[Macros.Game.BattleProp.ATTACK_RATE] = Macros.Game.BattleProp.ATTACK,
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE] = Macros.Game.BattleProp.PHYSICAL_DEFENCE,
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE] = Macros.Game.BattleProp.MAGIC_DEFENCE,
	[Macros.Game.BattleProp.HP_RATE] = Macros.Game.BattleProp.HP,
}

--战斗最终加成类属性表
Modules.BattleFormula.BattleFinalAdditionalPropMap = 
{
	[Macros.Game.BattleProp.ATTACK_RATE_FINAL] = Macros.Game.BattleProp.ATTACK,
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE_FINAL] = Macros.Game.BattleProp.PHYSICAL_DEFENCE,
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE_FINAL] = Macros.Game.BattleProp.MAGIC_DEFENCE,
	[Macros.Game.BattleProp.HP_RATE_FINAL] = Macros.Game.BattleProp.HP,
}

--[[
伤害/治疗计算
--]]
function Modules.BattleFormula:Calc(attacker_id, defender_id, skill_id, skill_lev, damage_time_index)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return
	end

	if config_skill.camp == Macros.Game.SkillTgtCamp.ENEMY then
		--伤害计算
		local damage_type, damage = self:CalcDamage(attacker_id, defender_id, skill_id, skill_lev, damage_time_index)

		--处理吸血
		self:__HandleSuckBlood(attacker_id, damage, skill_id, skill_lev)

		--处理吸血BUFF
		GlobalBuffMgr:HandleSuckBloodBuff(attacker_id, damage)

		--处理吸收护盾BUFF
		damage = GlobalBuffMgr:HandleAbsorptionShieldBuff(attacker_id, defender_id, damage) or 0

		return Modules.BattleFormula.HPChangeType.DAMAGE, -damage, damage_type
	else
		--治疗计算
		local treatment_type, treatment = self:CalcTreatment(attacker_id, defender_id, skill_id, skill_lev)

		return Modules.BattleFormula.HPChangeType.TREATMENT, treatment, treatment_type
	end
end

--[[
伤害计算
	最终伤害=max((攻击*攻击/(攻击+max(防御*(1-百分比穿透)-固定值穿透,0))*(1+伤害加成-百分比减伤)+(伤害附加-固定值减伤)),0)*技能伤害系数*伤害类型修正值*QTE连击加成伤害系数			
--]]
function Modules.BattleFormula:CalcDamage(attacker_id, defender_id, skill_id, skill_lev, damage_time_index)		
	local attacker = GlobalScene:GetObj(attacker_id)
	local defender = GlobalScene:GetObj(defender_id)
	if not attacker or not defender then
		return 0
	end

	local attacker_vo = attacker:GetVO()
	local defender_vo = defender:GetVO()
	local attacker_attr = self:__HandleEffectOfLowLifeTarget(skill_id, attacker_vo, defender_vo)
	local defender_attr = defender_vo.battle_attr
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)	

	--攻击
	local attack = attacker_attr.attack
	--防御
	local defence = Macros.Game.IsProfessionPhysical(attacker_vo.profession) and defender_attr.physical_defence or defender_attr.magic_defence
	--百分比穿透
	local penetration_rate = MathTool.TransProbCoeff2Decimal(attacker_attr.penetration_rate)
	--固定值穿透
	local penetration_value = attacker_attr.penetration_value
	--伤害加成
	local damage_increase_rate = MathTool.TransProbCoeff2Decimal(attacker_attr.damage_increase_rate)
	--百分比减伤
	local damage_decrease_rate = MathTool.TransProbCoeff2Decimal(defender_attr.damage_decrease_rate)
	--伤害附加
	local damage_increase_value = attacker_attr.damage_increase_value
	--固定值减伤
	local damage_decrease_value = defender_attr.damage_decrease_value

	--技能伤害系数
	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	local skill_damage_rate = config_skill_lev and MathTool.TransProbCoeff2Decimal(config_skill_lev.damage_rate) or 1
	local skill_damage_rate_plus = config_skill.damage_rate_list and config_skill.damage_rate_list[damage_time_index] or 1
	skill_damage_rate = skill_damage_rate * skill_damage_rate_plus

	--QTE连击加成伤害系数
	local qte_combo_rate = MathTool.TransProbCoeffPercentage(ConfigAdapter.Battle.GetBattleQTEComboAddHarmCoef()) or 1

	--伤害类型
	local damage_type = Modules.BattleFormula:CalcDamageType(attacker_attr, defender_attr)

	--伤害类型修正值
	local damage_correct = Modules.BattleFormula:CalcDamageCorrect(damage_type, attacker_attr, defender_attr)	

	local damage = math.max((attack * attack / (attack + math.max(defence * (1 - penetration_rate) - penetration_value, 0)) * (1 + damage_increase_rate - damage_decrease_rate) + (damage_increase_value - damage_decrease_value)), 0) * skill_damage_rate * damage_correct * qte_combo_rate

	--叛军全力一击，双倍伤害处理
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.type == Macros.Game.FBType.REBEL and 
		battle_info.param2 == Macros.Game.RebelAttackType.FULL and
		attacker:IsHero() then
		damage = damage * 2.5
	end

	--怪物受伤调整
	local config_monster = CPPGameLib.GetConfig("Monster", defender_vo.info_id, false)
	if config_monster and config_monster.damage_adjust_factor then
		damage = damage * MathTool.TransProbCoeff2Decimal(config_monster.damage_adjust_factor or 10000)
	end
	return damage_type, math.ceil(damage)
end

--[[
伤害类型计算
	闪避伤害区间为：(0,闪避几率]；			
	格挡伤害区间为：(闪避几率,闪避几率+格挡几率]；			
	暴击伤害区间为：(闪避几率+格挡几率,闪避几率+格挡几率+暴击几率]；			
	普通伤害区间为：(闪避几率+格挡几率+暴击几率,∞)；			
	在(0,1]区间中随机出本次伤害类型			
	注：	
		闪避几率=max(防御者闪避率-攻击者命中率,0)		
		格挡几率=防御者格挡率		
		暴击几率=max(攻击者暴击率-防御者抗爆率,0)		
--]]
function Modules.BattleFormula:CalcDamageType(attacker_attr, defender_attr)
	local dodge_rate = MathTool.TransProbCoeff2Decimal(math.max(0, defender_attr.dodge_rate - attacker_attr.hit_rate))
	local block_rate = MathTool.TransProbCoeff2Decimal(defender_attr.block_rate)
	local crit_rate = MathTool.TransProbCoeff2Decimal(math.max(0, attacker_attr.crit_rate - defender_attr.crit_defence_rate))

	--闪避区间
	local dodge_section = Section.New(0, dodge_rate, nil, true)
	--格挡区间
	local block_section = Section.New(dodge_rate, dodge_rate + block_rate, nil, true)
	--暴击区间
	local crit_section = Section.New(dodge_rate + block_rate, dodge_rate + block_rate + crit_rate, nil, true)
	--普通区间
	local normal_section = Section.New(dodge_rate + block_rate + crit_rate)

	local rand = MathTool.GetRandom()
	if dodge_section:InSection(rand) then
		--闪避
		return Modules.BattleFormula.DamageType.DODGE
	end
	if block_section:InSection(rand) then
		--格挡
		return Modules.BattleFormula.DamageType.BLOCK
	end
	if crit_section:InSection(rand) then
		--暴击
		return Modules.BattleFormula.DamageType.CRIT
	end
	if normal_section:InSection(rand) then
		--普通
		return Modules.BattleFormula.DamageType.NORMAL
	end
	return Modules.BattleFormula.DamageType.NORMAL
end

--[[
伤害类型修正值
	闪避伤害类型修正值=0；		
	格挡伤害类型修正值=max(0.8-格挡强度,0.1)；	解释：基础格挡能挡掉20%伤害，最多能挡掉90%伤害	
	暴击伤害类型修正值=max(2+暴击强度-抗暴击强度,1.1)；	解释：基础暴击为200%伤害，抗暴击强度最多能让暴击只打出110%伤害	
	普通伤害类型修正值=1；		
--]]
function Modules.BattleFormula:CalcDamageCorrect(damage_type, attacker_attr, defender_attr)
	--闪避
	if damage_type == Modules.BattleFormula.DamageType.DODGE then
		return 0

	--格挡
	elseif damage_type == Modules.BattleFormula.DamageType.BLOCK then
		return math.max(0.8 - MathTool.TransProbCoeff2Decimal(defender_attr.block_rate_correct), 0.1)

	--暴击
	elseif damage_type == Modules.BattleFormula.DamageType.CRIT then
		return math.max(2 + MathTool.TransProbCoeff2Decimal(attacker_attr.crit_rate_correct - defender_attr.crit_defence_rate_correct), 1.1)

	--普通
	elseif damage_type == Modules.BattleFormula.DamageType.NORMAL then
		return 1
	end
end

--[[
治疗计算				
	治疗量=max((攻击*(1+伤害加成)+伤害附加)*技能治疗系数*治疗类型修正值,0)			
--]]
function Modules.BattleFormula:CalcTreatment(attacker_id, defender_id, skill_id, skill_lev)
	local attacker = GlobalScene:GetObj(attacker_id)
	local defender = GlobalScene:GetObj(defender_id)
	if not attacker or not defender then
		return 0
	end

	local attacker_vo = attacker:GetVO()
	local defender_vo = defender:GetVO()
	local attacker_attr = self:__HandleEffectOfLowLifeTarget(skill_id, attacker_vo, defender_vo)

	--攻击
	local attack = attacker_attr.attack
	--伤害加成
	local damage_increase_rate = MathTool.TransProbCoeff2Decimal(attacker_attr.damage_increase_rate)
	--伤害附加
	local damage_increase_value = attacker_attr.damage_increase_value

	--技能治疗系数
	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	local skill_treatment_rate = config_skill_lev and MathTool.TransProbCoeff2Decimal(config_skill_lev.treatment_rate) or 1

	--治疗类型
	local treatment_type = Modules.BattleFormula:CalcTreatmentType(attacker_attr)

	--治疗类型修正值
	local treatment_correct = Modules.BattleFormula:CalcTreatmentCorrect(treatment_type, attacker_attr)	

	local treatment = (attack * (1 + damage_increase_rate) + damage_increase_value) * skill_treatment_rate * treatment_correct		
	return treatment_type, math.max(0, math.ceil(treatment))
end

--[[
治疗计算(BUFF)
	治疗量=(buff施法者属性值*(1+伤害加成)+伤害附加)*buff施法者属性参数*治疗类型修正值 + buff受益者对应属性值*buff受益者属性影响系数
--]]
function Modules.BattleFormula:CalcTreatmentByBuff(attacker_attr, buff_src_attr_val, buff_src_attr_rate, buff_tgt_attr_val, buff_tgt_attr_rate, tgt_vo)
	--伤害加成
	local damage_increase_rate = MathTool.TransProbCoeff2Decimal(attacker_attr.damage_increase_rate)
	--伤害附加
	local damage_increase_value = attacker_attr.damage_increase_value

	--治疗类型
	local treatment_type = Modules.BattleFormula:CalcTreatmentType(attacker_attr)

	--治疗类型修正值
	local treatment_correct = Modules.BattleFormula:CalcTreatmentCorrect(treatment_type, attacker_attr)	

	local treatment = (buff_src_attr_val * (1 + damage_increase_rate) + damage_increase_value) * buff_src_attr_rate * treatment_correct + buff_tgt_attr_val * buff_tgt_attr_rate		

	--怪物受伤调整
	if tgt_vo then
		local config_monster = CPPGameLib.GetConfig("Monster", tgt_vo.info_id, false)
		if config_monster and config_monster.damage_adjust_factor then
			treatment = treatment * MathTool.TransProbCoeff2Decimal(config_monster.damage_adjust_factor or 10000)
		end
	end

	return treatment_type, math.ceil(treatment)
end

--[[
治疗类型计算
	暴击治疗区间为：(0,治疗暴击几率]；			
	普通治疗区间为：(治疗暴击几率,1]；			
	在(0,1]区间中随机出本次治疗类型			
	注：	
		治疗暴击几率=施法者暴击率/2；	解释：因为治疗暴击不计算抗暴此对抗属性，所以暴击率收益减半	
--]]
function Modules.BattleFormula:CalcTreatmentType(attacker_attr)
	local crit_rate = MathTool.TransProbCoeff2Decimal(attacker_attr.crit_rate / 2)

	--暴击区间
	local crit_section = Section.New(0, crit_rate, nil, true)
	--普通区间
	local normal_section = Section.New(crit_rate, 1, nil, true)

	local rand = MathTool.GetRandom()
	if crit_section:InSection(rand) then
		--暴击
		return Modules.BattleFormula.DamageType.CRIT
	end
	if normal_section:InSection(rand) then
		--普通
		return Modules.BattleFormula.DamageType.NORMAL
	end
	return Modules.BattleFormula.DamageType.NORMAL
end

--[[
治疗类型修正值
	暴击治疗类型修正值=1.5+暴击强度/2；	解释：因为治疗暴击不计算抗暴强度此对抗属性，所以暴击效果收益减半	
--]]
function Modules.BattleFormula:CalcTreatmentCorrect(treatment_type, attacker_attr)
	--暴击
	if treatment_type == Modules.BattleFormula.DamageType.CRIT then
		return 1.5 + MathTool.TransProbCoeff2Decimal(attacker_attr.crit_rate_correct / 2)

	--普通
	elseif treatment_type == Modules.BattleFormula.DamageType.NORMAL then
		return 1
	end
end

--[[
技能击退距离
	技能击退力度*(1+攻击者击退强度-防御者抗击退强度)
--]]
function Modules.BattleFormula:CalcSkillBeatback(beat_back, attacker_id, defender_id)
	local attacker = GlobalScene:GetObj(attacker_id)
	if not attacker then
		return 0
	end
	local defender = GlobalScene:GetObj(defender_id)
	if not defender then
		return 0 
	end

	local attacker_vo = attacker:GetVO()
	local defender_vo = defender:GetVO()
	beat_back = beat_back * (1 + MathTool.TransProbCoeff2Decimal(attacker_vo.battle_attr.beat_back_correct - defender_vo.battle_attr.beat_back_defence_correct))
	return math.max(0, math.floor(beat_back))
end

--[[
战斗结算星数
--]]
function Modules.BattleFormula:CalcResultStar(hero_dead_num)
	if not hero_dead_num or hero_dead_num == 0 then
		return 3
	elseif hero_dead_num == 1 then
		return 2
	else 
		return 1
	end
end

--[[
副本奖励表
--]]
function Modules.BattleFormula:CalcRewardCurrencyMap(fb_type, gamelevel_id,cur_role_level)	
	local reward_currency_map = ConfigAdapter.GameLevel.GetRewardCurrencyMap(gamelevel_id)
	local role_level = cur_role_level or CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	--经验、金币公式计算
	local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(gamelevel_id)
	local cost_stamina = cost_currency_map and cost_currency_map[Macros.Game.RolePropType.STAMINA] and cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0
	local cost_energy = cost_currency_map and cost_currency_map[Macros.Game.RolePropType.ENERGY] and cost_currency_map[Macros.Game.RolePropType.ENERGY] or 0

	local exp = 0
	local gold = 0

	if fb_type == Macros.Game.FBType.MAIN_DUNGEON or fb_type == Macros.Game.FBType.HERO_DUNGEON or fb_type == Macros.Game.FBType.ELITE_DUNGEON then 		
		exp =  cost_stamina > 0 and cost_stamina * role_level * 2 or 0
		gold = cost_stamina > 0 and role_level * 5 * cost_stamina + 100 or 0
	elseif fb_type == Macros.Game.FBType.PLUNDERED_TREASURES or fb_type == Macros.Game.FBType.ARENA then
		exp =  cost_energy > 0 and cost_energy * role_level * 2 or 0
		gold = cost_energy > 0 and role_level * 10 * cost_energy + 200 or 0
	end

	--主线副本
	if fb_type == Macros.Game.FBType.MAIN_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.MAIN_DUNGEON )
		gold = gold * num

	--英雄副本
	elseif fb_type == Macros.Game.FBType.HERO_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.HERO_DUNGEON )
		gold = gold * num
	--精英副本
	elseif fb_type == Macros.Game.FBType.ELITE_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.ELITE_DUNGEON )
		gold = gold * num

	--普通爬塔
	elseif fb_type == Macros.Game.FBType.TOWER_COMMON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.TOWER_COMMON )
		reward_currency_map[Macros.Game.RolePropType.GOLD] = reward_currency_map[Macros.Game.RolePropType.GOLD] * num
		reward_currency_map[Macros.Game.RolePropType.TOWER_PRESTIGE] = reward_currency_map[Macros.Game.RolePropType.TOWER_PRESTIGE] * num

	end

	reward_currency_map[Macros.Game.RolePropType.EXP] = (reward_currency_map[Macros.Game.RolePropType.EXP] or 0) + exp
	reward_currency_map[Macros.Game.RolePropType.GOLD] = (reward_currency_map[Macros.Game.RolePropType.GOLD] or 0) + gold

	return reward_currency_map
end

--生成加成属性表
function Modules.BattleFormula:GenPlusPropMap(prop_type_list, prop_value_list)
	if not prop_type_list or not prop_value_list then
		return
	end

	local plus_prop_map
	for i, prop_type in ipairs(prop_type_list) do
		plus_prop_map = plus_prop_map or {}
		self:AddPlusPropMap(plus_prop_map, prop_type, prop_value_list[i])
	end
	return plus_prop_map
end

--添加加成属性
function Modules.BattleFormula:AddPlusPropMap(plus_prop_map, prop_type, prop_value)
	if not plus_prop_map or not prop_type then
		return
	end
	
	plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
	plus_prop_map[prop_type] = plus_prop_map[prop_type] + (prop_value or 0)
end

--处理加成属性
function Modules.BattleFormula:HandlePlusPropMap(obj_info, plus_prop_map)
	if not obj_info or not plus_prop_map then
		return		
	end

	local prop_key
	for prop_type, prop_value in pairs(plus_prop_map) do
		prop_key = Macros.Game.BattlePropKey[prop_type]
		if prop_key then
			obj_info[prop_key] = obj_info[prop_key] or 0
			obj_info[prop_key] = obj_info[prop_key] + prop_value
		end
	end
end

--[[
处理加成类属性
--]]
function Modules.BattleFormula:CalcAdditionalProps(info, filter_map)
	local prop_key, add_prop_key

	--加成
	for add_prop_type, prop_type in pairs(Modules.BattleFormula.BattleAdditionalPropMap) do
		if not filter_map or not filter_map[add_prop_type] then
			prop_key = Macros.Game.BattlePropKey[prop_type]
			add_prop_key = Macros.Game.BattlePropKey[add_prop_type]
			info[prop_key] = math.floor(info[prop_key] * (1 + MathTool.TransProbCoeff2Decimal(info[add_prop_key])))
		end
	end
	--最终加成
	for add_prop_type, prop_type in pairs(Modules.BattleFormula.BattleFinalAdditionalPropMap) do
		if not filter_map or not filter_map[add_prop_type] then
			prop_key = Macros.Game.BattlePropKey[prop_type]
			add_prop_key = Macros.Game.BattlePropKey[add_prop_type]
			info[prop_key] = math.floor(info[prop_key] * (1 + MathTool.TransProbCoeff2Decimal(info[add_prop_key])))
		end
	end
end

--获取战力压制属性加成表
function Modules.BattleFormula:CalcCombatSuppressionPlusPropMap(battle_type, my_combat_value, enemy_combat_value)	
	local my_plus_prop_map, enemy_plus_prop_map

	--参数无效
	if not my_combat_value or not enemy_combat_value then
		return my_plus_prop_map, enemy_plus_prop_map
	end

	--双方战力一致，无战力压制
	if my_combat_value == enemy_combat_value then
		return my_plus_prop_map, enemy_plus_prop_map
	end

	--当前副本类型不开放战力压制
	local config_combat_suppression = CPPGameLib.GetConfig("CombatSuppression", battle_type, false)
	if not config_combat_suppression or config_combat_suppression.trigger_type == Macros.Game.CombatSuppressionTriggerType.NONE then
		return my_plus_prop_map, enemy_plus_prop_map
	end	

	--我方战力比敌方低，而且压制类型为只影响我方，无视
	if my_combat_value < enemy_combat_value and config_combat_suppression.trigger_type == Macros.Game.CombatSuppressionTriggerType.EFFECT_ME then
		return my_plus_prop_map, enemy_plus_prop_map
	end

	local function calc_func(low_combat_value, high_combat_value)
		local trigger_ratio = MathTool.TransProbCoeff2Decimal(config_combat_suppression.trigger_ratio)
		local step_ratio = MathTool.TransProbCoeff2Decimal(config_combat_suppression.step_ratio)		
		local delta_combat_ratio = (high_combat_value - low_combat_value) / low_combat_value
		if delta_combat_ratio < trigger_ratio then
			return
		end

		local tmp_plus_prop_map = {}
		local step_num = math.floor((delta_combat_ratio - trigger_ratio) /  step_ratio) + 1
		for i, prop_type in ipairs(config_combat_suppression.add_prop_type_list) do
			tmp_plus_prop_map[prop_type] = tmp_plus_prop_map[prop_type] or 0
			tmp_plus_prop_map[prop_type] = math.min(tmp_plus_prop_map[prop_type] + config_combat_suppression.add_prop_value_list[i] * step_num, config_combat_suppression.add_prop_max_value_list[i])
		end
		return tmp_plus_prop_map
	end

	if my_combat_value > enemy_combat_value then
		--计算我方战力压制属性加成
		my_plus_prop_map = calc_func(enemy_combat_value, my_combat_value)
	else
		--计算敌方战力压制属性加成
		enemy_plus_prop_map = calc_func(my_combat_value, enemy_combat_value)
	end

	return my_plus_prop_map, enemy_plus_prop_map
end

--构造战斗怪物信息
function Modules.BattleFormula:BuildBattleMonsterInfo(id, monster_info, ignore_plus_props, add_pro_type_list, add_pro_value_list)
	if not monster_info then
		monster_info = {info_id = id, damage = 0, blood_max = 0}
	end	

	--读取基础属性
	local config_monster = CPPGameLib.GetConfig("Monster", monster_info.info_id)
	if config_monster then
		for _, prop_key in pairs(Macros.Game.BattlePropKey) do
			if config_monster then
				monster_info[prop_key] = config_monster[prop_key] or 0
			else
				monster_info[prop_key] = 0
			end
		end
	end

    --读取怪物增幅属性
    if add_pro_type_list and add_pro_value_list then
        local prop_key, prop_value
        for i, add_pro_type in ipairs(add_pro_type_list) do
            prop_key = Macros.Game.BattlePropKey[add_pro_type]
            monster_info[prop_key] = monster_info[prop_key] or 0
            monster_info[prop_key] = monster_info[prop_key] + (add_pro_value_list[i] or 0)
        end
    end

	monster_info.hp = config_monster.blood_max

	if not ignore_plus_props then
		--处理加成类属性
		Modules.BattleFormula:CalcAdditionalProps(monster_info)
	end

	return monster_info
end

--构造战斗精灵信息
function Modules.BattleFormula:BuildBattleSpiritInfo(spirit_info)
	if not spirit_info then
		return
	end

	local battle_spirit_info = {info_id = spirit_info.id}
	local enhace_plus_prop_map = ConfigAdapter.Spirit.GetEnhacePlusPropMap(spirit_info.id, spirit_info.enhance_level, spirit_info.star)
	local train_plus_prop_map = ConfigAdapter.Spirit.GetTrainPlusPropMap(spirit_info.id, spirit_info.train_stage, spirit_info.train_times)

	--处理基础属性、强化加成属性
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		battle_spirit_info[prop_key] = enhace_plus_prop_map[prop_type] or 0
	end

	--处理培养加成属性
	Modules.BattleFormula:HandlePlusPropMap(battle_spirit_info, train_plus_prop_map)	

	--处理加成类属性
	Modules.BattleFormula:CalcAdditionalProps(battle_spirit_info)

	return battle_spirit_info	
end

--构造祝福精灵信息
function Modules.BattleFormula:BuildBattleBlessSpiritInfo(spirit_data, hero_info)
	if not spirit_data or not hero_info then
		return
	end
	
    hero_info.bless_sprite_info = 
    {
        id = hero_info.bless_sprite_id, 
        star = hero_info.bless_sprite_star,
        enhance_level = hero_info.bless_sprite_enhance_level,                    
        train_stage = hero_info.bless_sprite_train_stage
    }                
    hero_info.bless_sprite_prop_map = ConfigAdapter.Spirit.GetPropMapForEnemy(spirit_data, hero_info.bless_sprite_info)
end

--对战斗英雄列表进行排序
function Modules.BattleFormula:SortBattleHeroList(list)
    local hero_config1, hero_config2
    table.sort(list, function(vo1, vo2)
        if not vo1 then
            return false
        end
        if not vo2 then
            return true
        end

        hero_config1 = CPPGameLib.GetConfig("Hero", vo1.info_id)
        hero_config2 = CPPGameLib.GetConfig("Hero", vo2.info_id)
        if not hero_config1 then
            return false
        end
        if not hero_config2 then
            return true
        end

        --近战职业ID小，远程职业ID大
        return hero_config1.battle_sort_id > hero_config2.battle_sort_id
    end)
end

--对上阵英雄列表进行排序
function Modules.BattleFormula:SortLineupHeroList(list)
    local hero_config1, hero_config2
    table.sort(list, function(vo1, vo2)
        if not vo1 then
            return false
        elseif not vo2 then
            return true
        end

        if vo1.is_open or vo2.is_open then
            if vo1.is_open and vo2.is_open then
                hero_config1 = CPPGameLib.GetConfig("Hero", vo1.info_id, false)
                hero_config2 = CPPGameLib.GetConfig("Hero", vo2.info_id, false)

                if not hero_config1 and not hero_config2 then
                    return vo1.lineup_index < vo2.lineup_index
                elseif hero_config1 and not hero_config2 then
                	return true
                elseif not hero_config1 and hero_config2 then
                	return false
                else
                    --近战职业ID小，远程职业ID大
                    return hero_config1.battle_sort_id > hero_config2.battle_sort_id
                end
                
            elseif vo1.is_open and not vo2.is_open then
                return true

            elseif not vo1.is_open and vo2.is_open then
                return false
            end

        else
            return vo1.lineup_index < vo2.lineup_index
        end      
    end)
end

--对战斗怪物列表进行排序
function Modules.BattleFormula:SortBattleMonsterList(list)
    local monster_config1, monster_config2
    table.sort(list, function(vo1, vo2)
        if not vo1 then
            return false
        end
        if not vo2 then
            return true
        end

        monster_config1 = CPPGameLib.GetConfig("Monster", vo1.info_id)
        monster_config2 = CPPGameLib.GetConfig("Monster", vo2.info_id)
        if not monster_config1 then
            return false
        end
        if not monster_config2 then
            return true
        end

        --近战职业ID小，远程职业ID大
        return monster_config1.battle_sort_id > monster_config2.battle_sort_id
    end)
end

--解析服务端返回的英雄镜像数据
function Modules.BattleFormula:ParseHeroMirrorData(hero_info)
	if not hero_info then
		return
	end
	
	local prop_list = hero_info.pro_list:GetData()
	hero_info.pro_list = nil

	local prop_map = {}
	for _, prop_info in ipairs(prop_list) do
		prop_map[prop_info.prop_type] = prop_map[prop_info.prop_type] or 0
		prop_map[prop_info.prop_type] = prop_map[prop_info.prop_type] + prop_info.prop_value
	end
	local prop_value
	for prop_type, prop_key in pairs(Macros.Game.BattlePropKey) do
		prop_value = prop_map[prop_type]
		if prop_value and prop_value ~= 0 then
			hero_info[prop_key] = prop_value
		else
			hero_info[prop_key] = 0
		end
	end		
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--处理吸血
function Modules.BattleFormula:__HandleSuckBlood(attacker_id, damage, skill_id, skill_lev)
	local attacker = GlobalScene:GetObj(attacker_id)
	if not attacker then
		return
	end

	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	if not config_skill_lev or not config_skill_lev.suck_blood then
		return
	end

	if MathTool.CheckProbability(config_skill_lev.suck_blood[1]) then
		local hp_change = math.ceil(damage * MathTool.TransProbCoeff2Decimal(config_skill_lev.suck_blood[2]))
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", attacker_id, hp_change)
	    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HPChangeSyncReq", Modules.BattleFormula.HPChangeSrc.SKILL, attacker_id, attacker_id, hp_change)
	end
end

--临时修改低生命目标的属性(生命值比例|属性类型|属性值)
function Modules.BattleFormula:__HandleEffectOfLowLifeTarget(skill_id, attacker_vo, defender_vo)
	local real_attacker_attr
	local defender_hp_percent = defender_vo.hp / defender_vo.max_hp

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	local effect_of_low_life_target_list = config_skill.effect_of_low_life_target
	if effect_of_low_life_target_list then
		for _, effect_of_low_life_target in ipairs(effect_of_low_life_target_list) do
			
			if defender_hp_percent < MathTool.TransProbCoeff2Decimal(effect_of_low_life_target[1]) then	
				real_attacker_attr = real_attacker_attr or attacker_vo.battle_attr:Clone()

				local add_prop_type = effect_of_low_life_target[2]
				local add_prop_value = effect_of_low_life_target[3]
				local effect_prop_type = Modules.BattleFormula.BattleAdditionalPropMap[add_prop_type]
				if not effect_prop_type then
					--非加成类属性
					real_attacker_attr:SetAttr(add_prop_type, real_attacker_attr:GetAttr(add_prop_type) + add_prop_value)
				else
					--加成类属性
					local effect_prop_value = real_attacker_attr:GetAttr(effect_prop_type)
					effect_prop_value = math.ceil(effect_prop_value * (1 + MathTool.TransProbCoeff2Decimal(add_prop_value)))
					real_attacker_attr:SetAttr(effect_prop_type, effect_prop_value)
				end
			end
			
		end
	end

	return real_attacker_attr or attacker_vo.battle_attr
end
