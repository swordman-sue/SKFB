
--[[
%% @module: 治疗BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.TreatmentBuff = Buff.TreatmentBuff or BaseClass(Buff.BaseBuff)

function Buff.TreatmentBuff:__MyInit(config_buff, ...)
	local sp_params = config_buff.params_treatment
	if not sp_params then
		return false
	end

	--施法者属性类型、施法者属性参数
	local buff_src_attr_type = sp_params[1] or 0
	local buff_src_attr_rate = MathTool.TransProbCoeff2Decimal(sp_params[2]) or 0
	local buff_src_attr_val = self:__GetSrcBattleAttr(buff_src_attr_type)

	--受益者属性类型、受益者属性参数
	local buff_tgt_attr_type = sp_params[3] or 0
	local buff_tgt_attr_rate = MathTool.TransProbCoeff2Decimal(sp_params[4]) or 0
	local buff_tgt_attr_val = 0
	local tgt = self:__GetTgtObj()
	local tgt_vo
	if tgt then
		tgt_vo = tgt:GetVO()
		buff_tgt_attr_val = tgt:GetAttr(buff_tgt_attr_type, false)
	end

	--治疗量
	self.__treatment_type, self.__treatment_var = Modules.BattleFormula:CalcTreatmentByBuff(
		self:__GetSrcBattleAttrs(), buff_src_attr_val, buff_src_attr_rate, buff_tgt_attr_val, buff_tgt_attr_rate, tgt_vo)
	self.__treatment_var = math.ceil(self.__treatment_var)

	return true
end

--作用回调(定时调用)
function Buff.TreatmentBuff:BuffTakeEffect()
	Buff.BaseBuff.BuffTakeEffect(self)

	local tgt = self:__GetTgtObj()
	if tgt then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", self.__tgt_obj_id, self.__treatment_var, self.__treatment_type)
	    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HPChangeSyncReq", Modules.BattleFormula.HPChangeSrc.BUFF, 0, self.__tgt_obj_id, self.__treatment_var, self.__treatment_type)
	end
end
