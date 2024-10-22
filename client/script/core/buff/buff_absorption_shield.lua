
--[[
%% @module: 吸收护盾BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.AbsorptionShieldBuff = Buff.AbsorptionShieldBuff or BaseClass(Buff.BaseBuff)

local ASDamageType = 
{
	--物理
	PHYSICAL = 0,
	--魔法
	MAGIC = 1,
	--全部
	ALL = 2,	
}

function Buff.AbsorptionShieldBuff:__MyInit(config_buff)
	local sp_params = config_buff.params_absorption_shield	
	if not sp_params then
		return false
	end
	
	-- 吸收伤害类型
	self.__as_damage_type = sp_params[1]
	-- 护盾总吸收量
	self.__as_total_absorption = sp_params[2]

	--影响护盾血量属性类型
	self.__as_affect_attr_type = sp_params[3]
	--影响护盾血量属性万分比
	self.__as_affect_attr_prob_coeff = sp_params[4]

	if self.__as_affect_attr_type and self.__as_affect_attr_prob_coeff and 
		self.__as_affect_attr_type > 0 and self.__as_affect_attr_prob_coeff > 0 then
		local attr_value = self:__GetSrcBattleAttr(self.__as_affect_attr_type)	
		self.__as_total_absorption = self.__as_total_absorption + attr_value * MathTool.TransProbCoeff2Decimal(self.__as_affect_attr_prob_coeff)
	end

	return true
end

function Buff.AbsorptionShieldBuff:Update(now_time, elapse_time)
	if self.__as_total_absorption <= 0 then
		--护盾总吸收量<=0，则BUFF结束
		self.__total_time = 0
	end
	Buff.BaseBuff.Update(self, now_time, elapse_time)
end

--作用回调(条件调用)
function Buff.AbsorptionShieldBuff:BuffTakeEffectExternal(attacker_vo, damage)
	local real_damage = damage

	if self.__as_damage_type ~= ASDamageType.ALL then
		local damage_type = Macros.Game.IsProfessionPhysical(attacker_vo.profession) and ASDamageType.PHYSICAL or ASDamageType.MAGIC
		if damage_type ~= self.__as_damage_type then
			return math.ceil(real_damage)
		end
	end

	--抵消伤害
	real_damage = real_damage - self.__as_total_absorption
	real_damage = math.max(0, real_damage)

	--更新护盾总吸收量
	self.__as_total_absorption = self.__as_total_absorption - damage

	return math.ceil(real_damage)
end
