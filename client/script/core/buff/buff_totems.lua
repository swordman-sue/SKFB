
--[[
%% @module: 图腾BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.TotemsBuff = Buff.TotemsBuff or BaseClass(Buff.BaseBuff)

function Buff.TotemsBuff:__MyInit(config_buff)
	local sp_params = config_buff.params_totems	
	if not sp_params then
		return false
	end

	--图腾过程中，释放者可能被移除，因此重新构造新的VO
	local obj_id = self.__src_obj_vo.obj_id
	local battle_attr = self:__GetSrcBattleAttrs()
	self.__src_obj_vo = Scene.BattleObjVO.New()
	self.__src_obj_vo.obj_id = obj_id
	self.__src_obj_vo.battle_attr = battle_attr:Clone()

	-- BUFF ID
	self.__totems_buff_id = sp_params[1]
	-- 生效范围
	self.__totems_range = sp_params[2] * 0.5
	-- 生效目标(0敌人 1队友)
	self.__totems_target_type = sp_params[3]

	return true
end

function Buff.TotemsBuff:Update(now_time, elapse_time)
	if self.__totems_target_list then
		local hit_tgt
		local totems_tgt, totems_tgt_pos, totems_tgt_id
		for _, vo in ipairs(self.__totems_target_list) do
			totems_tgt_id = vo.obj_id
			totems_tgt = GlobalScene:GetObj(totems_tgt_id)
			if totems_tgt then
				totems_tgt_pos = totems_tgt:GetWorldPosVec()
				hit_tgt = totems_tgt_pos:IsNearHorz(self.__totems_pos, self.__totems_range)
				if hit_tgt and not self.__totems_target_enter_map[totems_tgt_id] then
					--进入图腾范围，触发BUFF
					local buff_id = GlobalBuffMgr:CreateBuff(self.__totems_buff_id, self.__src_obj_vo, totems_tgt:GetVO())
					if buff_id then
						self.__totems_target_enter_map[totems_tgt_id] = buff_id
					end

				elseif not hit_tgt and self.__totems_target_enter_map[totems_tgt_id] then
					--离开图腾范围，移除BUFF
					GlobalBuffMgr:DeleteBuff(self.__totems_target_enter_map[totems_tgt_id])
					self.__totems_target_enter_map[totems_tgt_id] = nil
				end
			end
		end
	end

	Buff.BaseBuff.Update(self, now_time, elapse_time)
end

function Buff.TotemsBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)
	
	--记录图腾位置
	if self.__external_data then
		self.__totems_pos = Vector2D.New(self.__external_data.x, self.__external_data.y)
	else
		local tgt = self:__GetTgtObj()
		self.__totems_pos = Vector2D.New(tgt:GetWorldPos())
	end

	--进入图腾范围的目标列表
	self.__totems_target_enter_map = {}

	--图腾目标列表
	self.__totems_target_list = self.__totems_target_type == 0 and
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", self.__src_obj_vo.obj_id) or
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", self.__src_obj_vo.obj_id)
end

--移除回调
function Buff.TotemsBuff:BuffRemoved()
	Buff.BaseBuff.BuffRemoved(self)

	for _, buff_id in pairs(self.__totems_target_enter_map) do
		GlobalBuffMgr:DeleteBuff(buff_id)		
	end
	self.__totems_target_enter_map = nil
end

--检测目标对象
function Buff.TotemsBuff:__CheckTgtObj()
end

--创建特效
function Buff.TotemsBuff:__CreateEffect()
	local effect_x, effect_y
	local tgt = self:__GetTgtObj()
	local tgt_dir

	if self.__external_data then
		effect_x, effect_y = GlobalScene:WorldToPixel(self.__external_data.x, self.__external_data.y)

	elseif tgt then
		effect_x, effect_y = GlobalScene:WorldToPixel(tgt:GetWorldPos())
		tgt_dir = tgt:GetDir()
	end

	if self.__effect_id and not self.__effect_obj_id then
		self.__effect_obj_id = GlobalEffectMgr:CreateScenePosEffect(
			{id = self.__effect_id, pos_x = effect_x, pos_y = effect_y, loop = true, dir = tgt_dir}, Scene.RenderQueue.GRQ_AIR)
	end

	if self.__down_effect_id and not self.__down_effect_obj_id then
		self.__down_effect_obj_id = GlobalEffectMgr:CreateScenePosEffect(
			{id = self.__down_effect_id, pos_x = effect_x, pos_y = effect_y, loop = true, dir = tgt_dir}, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
	end
end

function Buff.TotemsBuff:PauseByScreen()
	Buff.BaseBuff.PauseByScreen(self)

	self:SetEffectVisible(false)
end

function Buff.TotemsBuff:ResumeByScreen()
	Buff.BaseBuff.ResumeByScreen(self)

	self:SetEffectVisible(true)
end

