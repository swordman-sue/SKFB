
--[[
%% @module: BUFF管理器
%% @author: swordman sue
%% @created: 2016-09-18
%% @description: 创建和管理BUFF对象
--]]

Buff = Buff or {}

Buff.BuffMgr = Buff.BuffMgr or BaseClass()

Buff.BuffMgr.identify_count = Buff.BuffMgr.identify_count or 0

Buff.BuffType = 
{
	--治疗
	TREATMENT = 1,
	--眩晕
	VERTIGO = 2,
	--冰冻
	FROZEN = 3,
	--沉默
	SILENT = 4,
	--吸收护盾
	ABSORPTION_SHIELD = 5,
	--放逐
	EXILE = 6,
	--图腾
	TOTEMS = 7,
	--吸血
	SUCK_BLOOD = 8,
	--石化
	PETRIFACTION = 9,
	--属性
	ATTRIBUTE = 10,
	--回怒
	ANGER_REPLY = 11,
	--净化
	PURGE = 12,
}

function Buff.BuffMgr.CreateInstance()
	if not GlobalBuffMgr then
		GlobalBuffMgr = Buff.BuffMgr.New()
	end
end

function Buff.BuffMgr.DelInstance()
	if GlobalBuffMgr then
		GlobalBuffMgr:DeleteMe()
		GlobalBuffMgr = nil
	end
end

function Buff.BuffMgr:__init()
	self.__buff_map = {}
	self.__tgt_buff_lookup_map = {}

	self.__buff_class_map = 
	{
		[Buff.BuffType.TREATMENT] = Buff.TreatmentBuff,
		[Buff.BuffType.VERTIGO] = Buff.VertigoBuff,
		[Buff.BuffType.FROZEN] = Buff.FrozenBuff,
		[Buff.BuffType.SILENT] = Buff.SilentBuff,
		[Buff.BuffType.ABSORPTION_SHIELD] = Buff.AbsorptionShieldBuff,
		[Buff.BuffType.EXILE] = Buff.ExileBuff,
		[Buff.BuffType.TOTEMS] = Buff.TotemsBuff,
		[Buff.BuffType.SUCK_BLOOD] = Buff.SuckBloodBuff,
		[Buff.BuffType.PETRIFACTION] = Buff.FrozenBuff,
		[Buff.BuffType.ATTRIBUTE] = Buff.AttributeBuff,
		[Buff.BuffType.ANGER_REPLY] = Buff.AngerReplyBuff,
		[Buff.BuffType.PURGE] = Buff.PurgeBuff,
	}

    GlobalRunner:AddRunObj(self, Runner.Priority.RP_BUFF_MGR)
end

function Buff.BuffMgr:__delete()
	self:__ClearAllBuff(true)

    GlobalRunner:RemoveRunObj(self, Runner.Priority.RP_BUFF_MGR)	
end

function Buff.BuffMgr:Update(now_time, elapse_time)
	if self.__is_pause then
		return
	end

	if self.__is_pause_screen then
		return
	end

	self.__is_in_update = true
	for _, buff in pairs(self.__buff_map) do
		if not buff.is_delete then
			buff:Update(now_time, elapse_time)
		end
	end
	self.__is_in_update = false

	if self.__to_delete_buff_list then
		for _, buff_id in ipairs(self.__to_delete_buff_list) do
			self:DeleteBuff(buff_id)
		end
		self.__to_delete_buff_list = nil
	end
end

--创建BUFF
function Buff.BuffMgr:CreateBuff(config_buff_id, src_obj_vo, tgt_obj_vo, is_bless_sprite_buff, external_data)
	local config_buff = CPPGameLib.GetConfig("SkillBuff", config_buff_id)
	if not config_buff then
		return
	end
	local buff_class = get_buff_class(self, config_buff.type)
	if not buff_class then
		LogTool.LogError("[Buff.BuffMgr:CreateBuff] 找不到type为%d的BUFF类", config_buff.type)		
		return
	end

	--非图腾类BUFF，仅选中未死亡对象
	--图腾类BUFF，可选中所有对象
	local tgt = GlobalScene:GetObj(tgt_obj_vo.obj_id)
	if not tgt then
		return
	end
	if tgt:IsDead() and config_buff.type ~= Buff.BuffType.TOTEMS then
		return
	end

	--免疫控制类BUFF	
	if (tgt_obj_vo.immune_ctrl_skill or tgt_obj_vo.immune_ctrl_skill2) and self:IsCtrlBuff(config_buff.type) then
		return
	end

	--放逐BUFF命中前，先清除目标身上所有BUFF
	if config_buff.type == Buff.BuffType.EXILE then
		self:DeleteBuffsByTargetID(tgt_obj_vo.obj_id)
	end
	
	local buff = buff_class.New(config_buff, src_obj_vo, tgt_obj_vo, is_bless_sprite_buff, external_data)
	if buff:IsInitSuccess() then
		local buff_id = get_buff_id()
		buff:SetID(buff_id)
		buff:SetConfigID(config_buff_id)
		buff:BuffCreated()

		self.__buff_map[buff_id] = buff

		self:__AddTgtBuffRecord(tgt_obj_vo.obj_id, buff:GetType(), buff_id)

		if not is_bless_sprite_buff then
			--检测精灵祝福BUFF
			local data = {}
			data.buff_id = config_buff_id
			Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.BE_ATTACHED_BY_BUFF, data, tgt_obj_vo)		
		end

		return buff_id, buff
	else
		buff:DeleteMe()
	end
end

--删除BUFF
function Buff.BuffMgr:DeleteBuff(buff_id, cleanup_record)
	local buff = self.__buff_map[buff_id]
	if not buff then
		return
	end	

	if self.__is_in_update then
		buff.is_delete = true
		self.__to_delete_buff_list = self.__to_delete_buff_list or {}
		table.insert(self.__to_delete_buff_list, buff_id)
	else
		self.__buff_map[buff_id] = nil

		if cleanup_record ~= false then
			self:__DelTgtBuffRecord(buff:GetTgtObjID(), buff:GetType(), buff_id)
		end

		buff:DeleteMe()
	end

	return true
end

--移除某目标身上所有BUFF
function Buff.BuffMgr:DeleteBuffsByTargetID(target_id)
	local target_buff_map = self.__tgt_buff_lookup_map[target_id]
	if not target_buff_map then
		return
	end

	for buff_type, buff_id_map in pairs(target_buff_map) do		
		for buff_id, _ in pairs(buff_id_map) do
			self:DeleteBuff(buff_id, false)
		end
	end
	self.__tgt_buff_lookup_map[target_id] = nil
end

--获取BUFF对象
function Buff.BuffMgr:GetBuff(buff_id)
	return self.__buff_map[buff_id]
end

--获取
function Buff.BuffMgr:GetBuffsByTargetID(target_id)
	return self.__tgt_buff_lookup_map[target_id]
end

--设置某目标身上所有BUFF特效的显隐
function Buff.BuffMgr:SetBuffsEffectVisibleByTargetID(target_id, var)
	local target_buff_map = self.__tgt_buff_lookup_map[target_id]
	if not target_buff_map then
		return
	end

	local buff
	for buff_type, buff_id_map in pairs(target_buff_map) do
		for buff_id, _ in pairs(buff_id_map) do
			buff = self.__buff_map[buff_id]
			if buff then
				buff:SetEffectVisible(var)
			end
		end
	end
end

--检测某目标身上是否持有某类BUFF
function Buff.BuffMgr:CheckTargetBuffsByType(target_id, buff_type)
	if not self.__tgt_buff_lookup_map[target_id] then
		return false
	end
	if not self.__tgt_buff_lookup_map[target_id][buff_type] then
		return false
	end
	return next(self.__tgt_buff_lookup_map[target_id][buff_type]) ~= nil
end

--处理吸收护盾BUFF
function Buff.BuffMgr:HandleAbsorptionShieldBuff(attacker_id, defender_id, damage)
	local attacker = GlobalScene:GetObj(attacker_id)
	if not attacker then
		return damage
	end

	if not self.__tgt_buff_lookup_map or not self.__tgt_buff_lookup_map[defender_id] then
		return damage
	end

	local buff_map = self.__tgt_buff_lookup_map[defender_id][Buff.BuffType.ABSORPTION_SHIELD]
	if buff_map then
		local buff
		for buff_id, _ in pairs(buff_map) do
			buff = self.__buff_map[buff_id]
			if buff then
				damage = buff:BuffTakeEffectExternal(attacker:GetVO(), damage)
				if damage <= 0 then
					break
				end
			end
		end
	end

	return damage
end

--处理吸血BUFF
function Buff.BuffMgr:HandleSuckBloodBuff(attacker_id, damage)
	if not self.__tgt_buff_lookup_map or not self.__tgt_buff_lookup_map[attacker_id] then
		return damage
	end

	local buff_map = self.__tgt_buff_lookup_map[attacker_id][Buff.BuffType.SUCK_BLOOD]
	if buff_map then
		local buff
		for buff_id, _ in pairs(buff_map) do
			buff = self.__buff_map[buff_id]
			if buff then
				buff:BuffTakeEffectExternal(damage)
			end
		end
	end
end

function Buff.BuffMgr:Dump()
	self:__ClearAllBuff()

	self.__is_pause_screen = false
end

function Buff.BuffMgr:Pause()
	self.__is_pause = true
end

function Buff.BuffMgr:Resume()
	self.__is_pause = false
end

--怒气/合击技能暂停相关
function Buff.BuffMgr:PauseByScreen(trigger_obj_id)
	if self.__is_pause_screen then
		return
	end
	self.__is_pause_screen = true

	for _, buff in pairs(self.__buff_map) do
		buff:PauseByScreen()
	end	
end

--怒气/合击技能恢复相关
function Buff.BuffMgr:ResumeByScreen(trigger_obj_id)
	if not self.__is_pause_screen then
		return
	end
	self.__is_pause_screen = false

	for _, buff in pairs(self.__buff_map) do
		buff:ResumeByScreen()
	end	
end

--是否控制类BUFF
function Buff.BuffMgr:IsCtrlBuff(buff_type)
	if buff_type == Buff.BuffType.VERTIGO or 
		buff_type == Buff.BuffType.FROZEN or 
		buff_type == Buff.BuffType.EXILE or 
		buff_type == Buff.BuffType.PETRIFACTION or 
		buff_type == Buff.BuffType.SILENT then
		return true
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Buff.BuffMgr:__AddTgtBuffRecord(tgt_obj_id, buff_type, buff_id)
	--图腾类型的buff不记录
	if buff_type == Buff.BuffType.TOTEMS then
		return
	end
	self.__tgt_buff_lookup_map[tgt_obj_id] = self.__tgt_buff_lookup_map[tgt_obj_id] or {}
	self.__tgt_buff_lookup_map[tgt_obj_id][buff_type] = self.__tgt_buff_lookup_map[tgt_obj_id][buff_type] or {}
	self.__tgt_buff_lookup_map[tgt_obj_id][buff_type][buff_id] = true
end

function Buff.BuffMgr:__DelTgtBuffRecord(tgt_obj_id, buff_type, buff_id)
	self.__tgt_buff_lookup_map[tgt_obj_id] = self.__tgt_buff_lookup_map[tgt_obj_id] or {}
	self.__tgt_buff_lookup_map[tgt_obj_id][buff_type] = self.__tgt_buff_lookup_map[tgt_obj_id][buff_type] or {}
	self.__tgt_buff_lookup_map[tgt_obj_id][buff_type][buff_id] = nil
end

function Buff.BuffMgr:__ClearAllBuff(cleanup)
	Buff.BuffMgr.identify_count = 0

	for _, buff in pairs(self.__buff_map) do
		buff:DeleteMe()
	end

	self.__buff_map = not cleanup and {} or nil
	self.__tgt_buff_lookup_map = not cleanup and {} or nil
	self.__to_delete_buff_list = nil
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_buff_id = function()
	Buff.BuffMgr.identify_count = Buff.BuffMgr.identify_count + 1
	return Buff.BuffMgr.identify_count
end

get_buff_class = function(obj_self, buff_type)
	return obj_self.__buff_class_map[buff_type]
end
