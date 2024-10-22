
--[[
%% @module: BUFF基类
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff = Buff or {}

Buff.BaseBuff = Buff.BaseBuff or BaseClass()

function Buff.BaseBuff:__init(config_buff, src_obj_vo, tgt_obj_vo, is_bless_sprite_buff, external_data)
	if not config_buff then
		return
	end
	local tgt_obj = GlobalScene:GetObj(tgt_obj_vo.obj_id)
	if not tgt_obj then
		LogTool.LogError("[Buff.BaseBuff:__init] 找不到tgt_obj_vo.obj_id为%d的对象", tgt_obj_vo.obj_id)
		return
	end

	self.__config_buff = config_buff

	--释放BUFF的对象VO
	self.__src_obj_vo = src_obj_vo

	--持有BUFF的对象ID
	self.__tgt_obj_id = tgt_obj_vo.obj_id

	--是否精灵祝福BUFF
	self.__is_bless_sprite_buff = is_bless_sprite_buff

	--额外数据
	self.__external_data = external_data

	--类型
	self.__buff_type = config_buff.type
	--作用次数
	self.__total_count = config_buff.times or 1
	--单次作用时间
	self.__unit_time = config_buff.time or 0
	--持续时间
	self.__total_time = self.__unit_time * self.__total_count

	--特效ID
	self.__effect_id = config_buff.effect_id

	--下层特效ID
	self.__down_effect_id = config_buff.down_effect_id

	--特效是否循环播放
	self.__is_loop_play_effect = config_buff.is_loop_play_effect == Macros.Global.TRUE

	--音效ID
	self.__sound_id = config_buff.sound_id

	if not self:__MyInit(config_buff) then
		return
	end

	self:__CreateEffect()	

	self.__init_success = true	

	--TEMP：临时打印
	if self.__is_bless_sprite_buff then
		print("skfb", string.format("祝福精灵BUFF 攻击者ID：%d，受击者ID：%d", src_obj_vo.info_id, tgt_obj_vo.info_id))
	end
end

function Buff.BaseBuff:__delete()
	self:__DeleteEffect()

	self:BuffRemoved()
end

function Buff.BaseBuff:Update(now_time, elapse_time)
	self:__CheckTgtObj()

	--BUFF结束检测
	self.__total_elapse_time = self.__total_elapse_time or 0
	self.__total_elapse_time = self.__total_elapse_time + elapse_time
	if self.__total_elapse_time >= self.__total_time and not self.__is_end then
		self.__is_end = true
		GlobalBuffMgr:DeleteBuff(self.__buff_id)
	end

	if not self.__is_end then
		self.__cur_count = self.__cur_count or 0
		
		if not self.__unit_elapse_time then
			self.__unit_elapse_time = self.__unit_time
		else
			self.__unit_elapse_time = self.__unit_elapse_time + elapse_time
		end				
		if self.__unit_elapse_time >= self.__unit_time and self.__cur_count < self.__total_count then
			self:BuffTakeEffect()
			self.__unit_elapse_time = self.__unit_elapse_time - self.__unit_time
			self.__cur_count = self.__cur_count + 1			
		end
	end
end

function Buff.BaseBuff:IsInitSuccess()
	return self.__init_success
end

function Buff.BaseBuff:SetConfigID(config_id)
	self.__config_id = config_id
end

function Buff.BaseBuff:SetID(buff_id)
	self.__buff_id = buff_id
end

function Buff.BaseBuff:GetType()
	return self.__buff_type
end

--获取释放BUFF的对象ID(只保证在创建BUFF时有效)
function Buff.BaseBuff:GetSrcObjID()
	return self.__src_obj_vo.obj_id
end

--获取持有BUFF的对象ID
function Buff.BaseBuff:GetTgtObjID()
	return self.__tgt_obj_id
end

--创建回调
function Buff.BaseBuff:BuffCreated()
	self:__PlaySound()

	--显示BUFF名字特效
	local tgt_obj = self:__GetTgtObj()
	if tgt_obj then
		tgt_obj:ShowBuffNameEffect(self.__config_buff.name)
	end

	if self.__is_bless_sprite_buff then
		--如果是精灵祝福BUFF则显出精灵虚影(1秒渐隐消失),同时冒出对应的技能名美术字
		local src_obj_id = self:GetSrcObjID()
		local obj = GlobalScene:GetObj(src_obj_id)
		if obj and obj.__ShowSpriteBlessHalfHead then
			obj:__ShowSpriteBlessHalfHead()
		end
	end
end

--作用回调(定时调用)
function Buff.BaseBuff:BuffTakeEffect()
	if self.__cur_count >= 1 and self.__is_loop_play_effect then
		self:__DeleteEffect()
		self:__CreateEffect()
	end
end

--作用回调(条件调用)
function Buff.BaseBuff:BuffTakeEffectExternal()
	--override me
end

--移除回调
function Buff.BaseBuff:BuffRemoved()
	self:__StopSound()
end

function Buff.BaseBuff:PauseByScreen()
	self:__PauseSound()
end

function Buff.BaseBuff:ResumeByScreen()
	self:__ResumeSound()
end

function Buff.BaseBuff:SetEffectVisible(var)
	GlobalEffectMgr:SetEffectVisible(self.__effect_obj_id, var)
	GlobalEffectMgr:SetEffectVisible(self.__down_effect_obj_id, var)	
end

function Buff.BaseBuff:GetConfig()
	return self.__config_buff
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Buff.BaseBuff:__MyInit()
	return true
end

--获取释放BUFF的对象
function Buff.BaseBuff:__GetSrcOj()
	return GlobalScene:GetObj(self.__src_obj_vo.obj_id)
end

function Buff.BaseBuff:__GetSrcBattleAttr(type)
	if self.__is_bless_sprite_buff then
		--若是祝福技能触发的BUFF，则使用祝福精灵的属性
		if self.__src_obj_vo.bless_sprite_battle_attr then
			return self.__src_obj_vo.bless_sprite_battle_attr:GetAttr(type, false)
		end
	else		
		local src = self:__GetSrcOj()
		if src then
			return src:GetAttr(type, false)
		end
	end
end

function Buff.BaseBuff:__GetSrcBattleAttrs()
	if self.__is_bless_sprite_buff then
		--若是祝福技能触发的BUFF，则使用祝福精灵的属性
		return self.__src_obj_vo.bless_sprite_battle_attr
	else
		return self.__src_obj_vo.battle_attr
	end
end

--获取持有BUFF的对象
function Buff.BaseBuff:__GetTgtObj()
	return GlobalScene:GetObj(self.__tgt_obj_id)
end

--检测持有BUFF的对象
function Buff.BaseBuff:__CheckTgtObj()
	local tgt = self:__GetTgtObj()
	if not tgt then
		self.__total_time = 0
	end
end

--创建特效
function Buff.BaseBuff:__CreateEffect()	
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	local tgt = self:__GetTgtObj()	
	local dir = tgt and tgt:GetDir()

	if self.__effect_id and not self.__effect_obj_id then
		self.__effect_obj_id = GlobalEffectMgr:CreateSceneTargetEffect({id = self.__effect_id, loop = true, dir = dir}, 
			self.__tgt_obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)		
	end

	if self.__down_effect_id and not self.__down_effect_obj_id then
		self.__down_effect_obj_id = GlobalEffectMgr:CreateSceneTargetEffect({id = self.__down_effect_id, loop = true, dir = dir}, 
			self.__tgt_obj_id, RenderObj.InnerLayer.BODY_EFFECT_DOWN)		
	end
end

--移除特效
function Buff.BaseBuff:__DeleteEffect()
	if self.__effect_obj_id then
		GlobalEffectMgr:DelEffect(self.__effect_obj_id)
		self.__effect_obj_id = nil
	end	
	if self.__down_effect_obj_id then
		GlobalEffectMgr:DelEffect(self.__down_effect_obj_id)
		self.__down_effect_obj_id = nil
	end
end

--播放音效
function Buff.BaseBuff:__PlaySound()
    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return
    end

	if self.__sound_id then
		self.__sound_bind_id = GlobalSoundMgr:PlayEffectByConfigID(self.__sound_id)
	end
end

--暂停音效
function Buff.BaseBuff:__PauseSound()
	if self.__sound_bind_id then
		GlobalSoundMgr:PauseEffect(self.__sound_bind_id)
	end
end

--恢复音效
function Buff.BaseBuff:__ResumeSound()
	if self.__sound_bind_id then
		GlobalSoundMgr:ResumeEffect(self.__sound_bind_id)
	end
end

--停止音效
function Buff.BaseBuff:__StopSound()
	if self.__sound_bind_id then
		GlobalSoundMgr:StopEffect(self.__sound_bind_id)
		self.__sound_bind_id = nil
	end
end
