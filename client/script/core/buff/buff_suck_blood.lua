
--[[
%% @module: 吸血BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.SuckBloodBuff = Buff.SuckBloodBuff or BaseClass(Buff.BaseBuff)

function Buff.SuckBloodBuff:__MyInit(config_buff)
	local sp_params = config_buff.params_suck_blood
	if not sp_params then
		return false
	end

	-- 吸血概率
	self.__suck_blood_prob = sp_params[1]
	-- 吸血比例
	self.__suck_blood_factor = MathTool.TransProbCoeff2Decimal(sp_params[2])

	return true
end

--作用回调(条件调用)
function Buff.SuckBloodBuff:BuffTakeEffectExternal(damage)
	local tgt = self:__GetTgtObj()
	if tgt and MathTool.CheckProbability(self.__suck_blood_prob) then
		local change_hp = math.ceil(damage * self.__suck_blood_factor)
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", self.__tgt_obj_id, change_hp)
	    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HPChangeSyncReq", Modules.BattleFormula.HPChangeSrc.BUFF, 0, self.__tgt_obj_id, change_hp)
	end
end
