
--[[
%% @module: 战力公式
%% @author: swordman sue
%% @created: 2016-10-24
--]]

Modules = Modules or {}

Modules.PowerFormula = Modules.PowerFormula or BaseClass(BaseModule)

--[[
计算怪物战力
血量战力=血量上限*血量战力系数*闪避战力系数*格挡战力系数*抗暴战力系数*减伤战力系数
攻击战力=攻击*攻击战力系数*命中战力系数*暴击战力系数*伤害加成战力系数*百分比穿透战力系数+伤害附加*伤害附加战力系数+固定值穿透*固定值穿透战力系数
防御战力=物防*物防战力系数+法防*法防战力系数+固定值减伤*固定值减伤战力系数

血量战力系数=0.5
闪避战力系数=1+闪避率
格挡战力系数=1/MAX(1-格挡率*(0.2+格挡强度),0.1)
抗暴战力系数=4/(MAX(2-抗爆率*0.5,0.01)*MAX(2-抗暴击强度*0.5,0.01))
减伤战力系数=1+伤害减免

攻击战力系数=5
命中战力系数=1+命中率
暴击战力系数=(1.05+暴击率)/1.05*(2+暴击强度)/2
伤害加成战力系数=1+伤害加成
百分比穿透战力系数=1+百分比穿透
伤害附加战力系数=10
固定值穿透战力系数=5

物防战力系数=5
法防战力系数=5
伤害抵挡战力系数=10
--]]
local function calc_obj_battle_power(monster_info)
	local monster_info = CPPGameLib.CopyTblShallow(monster_info)
	local battle_value = 0
	monster_info.attack = math.floor(monster_info.attack *(1+MathTool.TransProbCoeff2Decimal(monster_info.attack_rate))*(1+MathTool.TransProbCoeff2Decimal(monster_info.attack_rate_final)))
	monster_info.physical_defence = math.floor(monster_info.physical_defence *(1+MathTool.TransProbCoeff2Decimal(monster_info.physical_defence_rate))*(1+MathTool.TransProbCoeff2Decimal(monster_info.physical_defence_rate_final)))
	monster_info.magic_defence = math.floor(monster_info.magic_defence *(1+MathTool.TransProbCoeff2Decimal(monster_info.magic_defence_rate))*(1+MathTool.TransProbCoeff2Decimal(monster_info.magic_defence_rate_final)))
	monster_info.hp = math.floor(monster_info.hp *(1+MathTool.TransProbCoeff2Decimal(monster_info.hp_rate))*(1+MathTool.TransProbCoeff2Decimal(monster_info.hp_rate_final)))
	
	--血量战力
	battle_value = monster_info.hp *0.5*
	(1+MathTool.TransProbCoeff2Decimal(monster_info.dodge_rate))*
	(1/math.max(1-MathTool.TransProbCoeff2Decimal(monster_info.block_rate)*(0.2+MathTool.TransProbCoeff2Decimal(monster_info.block_rate_correct)),0.1))*
	(4/(math.max(2-MathTool.TransProbCoeff2Decimal(monster_info.crit_defence_rate*0.5),0.01)*math.max(2-MathTool.TransProbCoeff2Decimal(monster_info.crit_defence_rate_correct*0.5),0.01)))*
	(1+MathTool.TransProbCoeff2Decimal(monster_info.damage_decrease_rate))

	--攻击战力
	battle_value = math.floor(battle_value) + 
	monster_info.attack*5*(1+MathTool.TransProbCoeff2Decimal(monster_info.hit_rate))*
	((1.05+MathTool.TransProbCoeff2Decimal(monster_info.crit_rate))/1.05*(2+MathTool.TransProbCoeff2Decimal(monster_info.crit_rate_correct))/2)*
	(1+MathTool.TransProbCoeff2Decimal(monster_info.damage_increase_rate))*(1+MathTool.TransProbCoeff2Decimal(monster_info.penetration_rate))+
	monster_info.damage_increase_value*10+monster_info.penetration_value*5
	
	--防御战力
	battle_value = math.floor(battle_value) + 
	monster_info.physical_defence*5+
	monster_info.magic_defence*5+
	monster_info.damage_decrease_value*10
	
	return math.floor(battle_value)
end

--[[
怪物战力
--]]
function Modules.PowerFormula:CalcMonsterPower(monster_id, add_pro_type_list, add_pro_value_list)
	local monster_config = CPPGameLib.GetConfig("Monster",monster_id)
	if not monster_config then
		return 0
	end
	local monster_info = {}
	for pro_type,value in pairs(Macros.Game.BattlePropKey) do
		monster_info[value] = monster_config[value] or 0
	end	
	monster_info.hp = monster_config.blood_max

	if add_pro_type_list then
		for i,pro_type in ipairs(add_pro_type_list) do
			local prop_key = Macros.Game.BattlePropKey[pro_type]
			monster_info[prop_key] = (monster_info[prop_key] or 0) + add_pro_value_list[i]
		end
	end

	return calc_obj_battle_power(monster_info)
end

--[[
怪物战力
--]]
function Modules.PowerFormula:CalcMonsterPowerEx(monster_info)
	return calc_obj_battle_power(monster_info)
end

--[[
英雄战力
--]]
function Modules.PowerFormula:CalcHeroPower(hero_info)
	if not hero_info then
		return 0
	end

	return calc_obj_battle_power(hero_info)
end
