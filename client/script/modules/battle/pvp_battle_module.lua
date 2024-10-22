
--[[
%% @module: PVP战斗模块
%% @author: swordman sue
%% @created: 2017-06-15
--]]

Modules = Modules or {}

Modules.BattleModule = Modules.BattleModule or BaseClass(BaseModule)

Modules.BattleModule.ResultType = 
{
	--平局
	TIE = 0, 
	--胜利
	WIN = 1,
	--失败
	LOSE = 2,
}

--role_id + power + change_hp + "HelloWorld"
local function generate_damage_fuck(role_id, ass_hole, change_hp)
    return CPPGameLib.Md5(string.format("%d%d%dHelloWorld", role_id, ass_hole, change_hp))
end

--查找目标
function Modules.BattleModule:SearchTargetReq_PVP(spirit_skill_id)
	spirit_skill_id = spirit_skill_id or 0

	LogTool.LogInfo("[Modules.BattleModule:SearchTargetReq_PVP] 查找目标请求 spirit_skill_id:%d", spirit_skill_id)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_SEARCH_TARGET_REQ)
	protocol.spirit_skill_id = spirit_skill_id
	GlobalNetAdapter:OnSend(protocol)	
end

--查找目标响应
function Modules.BattleModule:SearchTargetResp_PVP(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:SearchTargetResp_PVP] 查找目标响应 spirit_skill_id:%d", protocol_data.spirit_skill_id)

	self:FireNextFrame(Macros.Event.BattleModule.PVP_SEARCH_TARGET_RESP)
end

--战斗准备通知
function Modules.BattleModule:ReadyNotify_PVP(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:ReadyNotify_PVP] 战斗准备通知")

	self.__cur_info = self.__cur_info or {}
	self.__cur_info.is_pve = protocol_data.is_robot == Macros.Global.TRUE
	self.__cur_info.target_data = self:__BuildTargetData_PVP(protocol_data)

	--延时进入战斗
	if self.__delay_start_timer_pvp then
		GlobalTimerQuest:CancelQuest(self.__delay_start_timer_pvp)
	end
	self.__delay_start_timer_pvp = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function() 
		self.__delay_start_timer_pvp = nil
		self:__HandleReadyNotify__PVP(protocol_data)
	end), 3 )
end

--战斗准备完成请求
function Modules.BattleModule:ReadyFinishReq_PVP()
	LogTool.LogInfo("[Modules.BattleModule:ReadyFinishReq_PVP] 战斗准备完成请求")

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_READY_FINISH_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--战斗准备完成响应
function Modules.BattleModule:ReadyFinishResp_PVP(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:ReadyFinishResp_PVP] 战斗准备完成响应")
end

--战斗开始通知
function Modules.BattleModule:BattleStartNotify_PVP(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:BattleStartNotify_PVP] 战斗开始通知")

	if self.__cur_info then
		self.__cur_info.is_started = true
	end
end

--战斗结束通知
function Modules.BattleModule:BattleOverNotify_PVP(protocol_data)
	local ret_type
	if protocol_data.winner_id == 0 then
		ret_type = Modules.BattleModule.ResultType.TIE
	else
		ret_type = (protocol_data.winner_id == CallModuleFunc(ModuleType.ROLE, "GetRoleID")) and Modules.BattleModule.ResultType.WIN or Modules.BattleModule.ResultType.LOSE
	end

	LogTool.LogInfo("[Modules.BattleModule:BattleOverNotify_PVP] 战斗结束通知 ret_type:%d", ret_type)

	GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
    function()
		self:__HandleBattleEnd_PVP(ret_type, protocol_data)
    end), 1)
end

--释放技能请求
function Modules.BattleModule:UseSkillReq_PVP(obj_id, skill_id, x, y)
	x = x or 0
	y = y or 0

	if self:__CheckUseSkillLock_PVP(obj_id) then
		return
	end

	LogTool.LogInfo("[Modules.BattleModule:UseSkillReq_PVP] 释放技能请求 obj_id:%d skill_id:%d x:%d y:%d", self:__ConvertCID2SID_PVP(obj_id), skill_id, x, y)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_USE_SKILL_REQ)
	protocol.obj_id = self:__ConvertCID2SID_PVP(obj_id)
	protocol.skill_id = skill_id
	protocol.x = x
	protocol.y = y
	GlobalNetAdapter:OnSend(protocol)	
end

--释放技能反馈
function Modules.BattleModule:UseSkillResp_PVP(protocol_data)	
	LogTool.LogInfo("[Modules.BattleModule:UseSkillResp_PVP] 释放技能反馈 obj_id:%d skill_id:%d x:%d y:%d", protocol_data.obj_id, protocol_data.skill_id, protocol_data.x, protocol_data.y)

	self:__HandleUseSkill_PVP(protocol_data)
end

--释放技能通知
function Modules.BattleModule:UseSkillNotify_PVP(protocol_data)	
	LogTool.LogInfo("[Modules.BattleModule:UseSkillNotify_PVP] 释放技能通知 obj_id:%d skill_id:%d x:%d y:%d", protocol_data.obj_id, protocol_data.skill_id, protocol_data.x, protocol_data.y)

	self:__HandleUseSkill_PVP(protocol_data)
end

--血量变化同步请求
function Modules.BattleModule:HPChangeSyncReq_PVP(src_obj_id, tgt_obj_id, change_hp, damage_type)
	change_hp = change_hp or 0
	damage_type = damage_type or Modules.BattleFormula.DamageType.NORMAL

	--血量变化数值纠正
	local tgt_obj = GlobalScene:GetObj(tgt_obj_id)
	if tgt_obj then
		local tgt_obj_vo = tgt_obj:GetVO()
		local result_hp = tgt_obj_vo.hp + change_hp
		result_hp = math.max(0, result_hp)
		result_hp = math.min(result_hp, tgt_obj_vo.max_hp)
		change_hp = result_hp - tgt_obj_vo.hp
	end
	if change_hp == 0 then
		return
	end

	-- LogTool.LogInfo("[Modules.BattleModule:HPChangeSyncReq_PVP] 血量变化同步请求 src_obj_id:%d tgt_obj_id:%d change_hp:%d damage_type:%d", self:__ConvertCID2SID_PVP(src_obj_id), self:__ConvertCID2SID_PVP(tgt_obj_id), change_hp, damage_type)

    local ass_hole = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroPower") or 0
    local fuck = generate_damage_fuck(CallModuleFunc(ModuleType.ROLE, "GetRoleID"), ass_hole, change_hp) or ""

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_HP_CHANGE_REQ)
	protocol.src_obj_id = self:__ConvertCID2SID_PVP(src_obj_id)
	protocol.tgt_obj_id = self:__ConvertCID2SID_PVP(tgt_obj_id)
	protocol.change_hp = change_hp
	protocol.damage_type = damage_type 
	protocol.ass_hole = ass_hole
	protocol.fuck = fuck
	GlobalNetAdapter:OnSend(protocol)	
end

--血量变化同步反馈
function Modules.BattleModule:HPChangeSyncResp_PVP(protocol_data)
	-- LogTool.LogInfo("[Modules.BattleModule:HPChangeSyncResp_PVP] 血量变化同步反馈 src_obj_id:%d tgt_obj_id:%d change_hp:%d damage_type:%d", protocol_data.src_obj_id, protocol_data.tgt_obj_id, protocol_data.change_hp, protocol_data.damage_type)

	self:__HandleHPChangeSync_PVP(protocol_data)
end

--血量变化同步通知
function Modules.BattleModule:HPChangeSyncNotify_PVP(protocol_data)
	-- LogTool.LogInfo("[Modules.BattleModule:HPChangeSyncNotify_PVP] 血量变化同步通知 src_obj_id:%d tgt_obj_id:%d change_hp:%d damage_type:%d", protocol_data.src_obj_id, protocol_data.tgt_obj_id, protocol_data.change_hp, protocol_data.damage_type)

	self:__HandleHPChangeSync_PVP(protocol_data)
end

--BUFF同步请求
function Modules.BattleModule:BuffSyncReq_PVP(config_buff_id, src_obj_id, tgt_obj_id, is_sprite_bless_buff)
	-- LogTool.LogInfo("[Modules.BattleModule:BuffSyncReq_PVP] BUFF同步请求 config_buff_id:%d src_obj_id:%d tgt_obj_id:%d", config_buff_id, self:__ConvertCID2SID_PVP(src_obj_id), self:__ConvertCID2SID_PVP(tgt_obj_id))

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_CREATE_BUFF_REQ)
	protocol.buff_id = config_buff_id
	protocol.src_obj_id = self:__ConvertCID2SID_PVP(src_obj_id)
	protocol.tgt_obj_id = self:__ConvertCID2SID_PVP(tgt_obj_id)
	protocol.is_sprite_bless_buff = is_sprite_bless_buff or 0
	GlobalNetAdapter:OnSend(protocol)		
end

--BUFF同步反馈
function Modules.BattleModule:BuffSyncResp_PVP(protocol_data)
	-- LogTool.LogInfo("[Modules.BattleModule:BuffSyncResp_PVP] BUFF同步反馈 config_buff_id:%d src_obj_id:%d tgt_obj_id:%d", protocol_data.buff_id, protocol_data.src_obj_id, protocol_data.tgt_obj_id)

	self:__HandleBuffSync_PVP(protocol_data)
end

--BUFF同步通知
function Modules.BattleModule:BuffSyncNotify_PVP(protocol_data)
	-- LogTool.LogInfo("[Modules.BattleModule:BuffSyncNotify_PVP] BUFF同步通知 config_buff_id:%d src_obj_id:%d tgt_obj_id:%d", protocol_data.buff_id, protocol_data.src_obj_id, protocol_data.tgt_obj_id)
	
	self:__HandleBuffSync_PVP(protocol_data)
end

--战斗结束请求(打机器人)
function Modules.BattleModule:BattleEndReq_PVP(var)	
	LogTool.LogInfo("[Modules.BattleModule:BattleEndReq_PVP] 战斗结束请求(打机器人) result_type:%d", var)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PVP_BATTLE_RESULT_REQ)
	protocol.result = var
	GlobalNetAdapter:OnSend(protocol)		
end

--战斗结束反馈(打机器人)
function Modules.BattleModule:BattleEndResp_PVP(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:BattleEndResp_PVP] 战斗结束反馈(打机器人) result_type:%d", protocol_data.result)

	self:__HandleBattleEnd_PVP(protocol_data.result, protocol_data)
end

--获取红方英雄列表
function Modules.BattleModule:GetRedHeroList_PVP()
	if self.__cur_info and #self.__cur_info.red_hero_list > 0 then
		for _, hero_info in ipairs(self.__cur_info.red_hero_list) do
			--预生成本地对象ID
			hero_info.obj_id = GlobalScene:GenerateObjID()
			--记录双端对象ID映射关系
			self:__RecordObjID_PVP(hero_info.obj_id, hero_info.info_id * 10 + 0)

			Modules.BattleFormula:ParseHeroMirrorData(hero_info)

            --构造祝福精灵信息
            Modules.BattleFormula:BuildBattleBlessSpiritInfo({spirit_list = self.__cur_info.red_sprite_list}, hero_info)
		end

		Modules.BattleFormula:SortBattleHeroList(self.__cur_info.red_hero_list)

		return self.__cur_info.red_hero_list		
	end
end

--获取红方精灵
function Modules.BattleModule:GetRedSpriteInfo_PVP()
	if self.__cur_info and self.__cur_info.red_spirit_id > 0 then
		local sprite_info = {}
		sprite_info.id = self.__cur_info.red_spirit_id
		sprite_info.enhance_level = self.__cur_info.red_spirit_enhance_level
		sprite_info.train_stage = self.__cur_info.red_sprite_train_stage
		sprite_info.train_times = self.__cur_info.red_sprite_train_times
		sprite_info.skill_id = self.__cur_info.red_spirit_skill_id
		--预生成本地对象ID
		sprite_info.obj_id = GlobalScene:GenerateObjID()
		--记录双端对象ID映射关系
		self:__RecordObjID_PVP(sprite_info.obj_id, sprite_info.id * 10 + 0)

		return sprite_info
	end
end

--获取蓝方英雄列表
function Modules.BattleModule:GetBlueHeroList_PVP()
	if self.__cur_info and #self.__cur_info.blue_hero_list > 0 then
		for _, hero_info in ipairs(self.__cur_info.blue_hero_list) do
			--预生成本地对象ID
			hero_info.obj_id = GlobalScene:GenerateObjID()
			--记录双端对象ID映射关系
			self:__RecordObjID_PVP(hero_info.obj_id, hero_info.info_id * 10 + 1)

			Modules.BattleFormula:ParseHeroMirrorData(hero_info)

            --构造祝福精灵信息
            Modules.BattleFormula:BuildBattleBlessSpiritInfo({spirit_list = self.__cur_info.blue_sprite_list}, hero_info)
		end

		Modules.BattleFormula:SortBattleHeroList(self.__cur_info.blue_hero_list)

		return self.__cur_info.blue_hero_list		
	end
end

--获取蓝方精灵
function Modules.BattleModule:GetBlueSpriteInfo_PVP()
	if self.__cur_info and self.__cur_info.blue_spirit_id > 0 then
		local sprite_info = {}
		sprite_info.id = self.__cur_info.blue_spirit_id
		sprite_info.enhance_level = self.__cur_info.blue_spirit_enhance_level
		sprite_info.train_stage = self.__cur_info.blue_sprite_train_stage
		sprite_info.train_times = self.__cur_info.blue_sprite_train_times
		sprite_info.skill_id = self.__cur_info.blue_spirit_skill_id
		--预生成本地对象ID
		sprite_info.obj_id = GlobalScene:GenerateObjID()
		--记录双端对象ID映射关系
		self:__RecordObjID_PVP(sprite_info.obj_id, sprite_info.id * 10 + 1)

		return sprite_info
	end
end

--销毁释放技能锁
function Modules.BattleModule:ClearUseSkillLock_PVP(obj_id)
	if self.__cur_info.use_skill_lock_info and self.__cur_info.use_skill_lock_info.obj_id == obj_id then
		self.__cur_info.use_skill_lock_info = nil
	end
end

--获取己方战斗结算数据
function Modules.BattleModule:GetMyResultData_PVP()
	if not self.__cur_info then
		return
	end

	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	if role_id == self.__cur_info.red_role_id then
		return self.__cur_info.red_result_data
	end
	return self.__cur_info.blue_result_data
end

--获取敌方战斗结算数据
function Modules.BattleModule:GetEnemyResultData_PVP()
	if not self.__cur_info then
		return
	end

	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	if role_id == self.__cur_info.red_role_id then
		return self.__cur_info.blue_result_data
	end
	return self.__cur_info.red_result_data
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--将本地对象ID转换为服务器对象ID
function Modules.BattleModule:__ConvertCID2SID_PVP(c_obj_id)
	if self.__cur_info and self.__cur_info.__cs_obj_id_map then
		return self.__cur_info.__cs_obj_id_map[c_obj_id] or 0
	end
	return 0
end

--将服务器对象ID转换为本地对象ID
function Modules.BattleModule:__ConvertSID2CID_PVP(s_obj_id)
	if self.__cur_info and self.__cur_info.__sc_obj_id_map then
		return self.__cur_info.__sc_obj_id_map[s_obj_id] or 0
	end
	return 0
end

--记录双端对象ID映射关系
function Modules.BattleModule:__RecordObjID_PVP(c_obj_id, s_obj_id)
	self.__cur_info.__cs_obj_id_map = self.__cur_info.__cs_obj_id_map or {}
	self.__cur_info.__sc_obj_id_map = self.__cur_info.__sc_obj_id_map or {}
	self.__cur_info.__cs_obj_id_map[c_obj_id] = s_obj_id
	self.__cur_info.__sc_obj_id_map[s_obj_id] = c_obj_id
end

--战斗准备通知处理
function Modules.BattleModule:__HandleReadyNotify__PVP(protocol_data)
	self.__cur_info.type = Macros.Game.FBType.PVP	
	self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetPVPGameLevelID()
	self.__cur_info.drop_list = {}

	--战役索引
	self.__cur_info.battle_index = 1
	--战役ID
	self.__cur_info.battle_id = ConfigAdapter.GameLevel.GetFirstBattleID(self.__cur_info.gamelevel_id)
	--战役数量
	self.__cur_info.battle_num = ConfigAdapter.GameLevel.GetBattleNum(self.__cur_info.gamelevel_id)

	--战斗前的角色信息
	self.__cur_info.pre_role_info = CPPGameLib.CopyTbl(CallModuleFunc(ModuleType.ROLE, "GetRoleInfo"))

	--奖励列表
	self.__cur_info.reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(self.__cur_info.type, self.__cur_info.gamelevel_id)

	--红蓝双方信息
	self.__cur_info.red_role_id = protocol_data.red_role_id
	self.__cur_info.red_role_name = protocol_data.red_role_name
	self.__cur_info.red_hero_list = protocol_data.red_hero_list
	self.__cur_info.red_spirit_id = protocol_data.red_spirit_id
	self.__cur_info.red_spirit_skill_id = protocol_data.red_spirit_skill_id
	self.__cur_info.red_spirit_enhance_level = protocol_data.red_spirit_enhance_level
	self.__cur_info.red_sprite_train_stage = protocol_data.red_sprite_train_stage
	self.__cur_info.red_sprite_train_times = protocol_data.red_sprite_train_times
	self.__cur_info.red_sprite_list = protocol_data.red_sprite_list
	self.__cur_info.blue_role_id = protocol_data.blue_role_id
	self.__cur_info.blue_role_name = protocol_data.blue_role_name
	self.__cur_info.blue_hero_list = protocol_data.blue_hero_list
	self.__cur_info.blue_spirit_id = protocol_data.blue_spirit_id
	self.__cur_info.blue_spirit_skill_id = protocol_data.blue_spirit_skill_id
	self.__cur_info.blue_spirit_enhance_level = protocol_data.blue_spirit_enhance_level
	self.__cur_info.blue_sprite_train_stage = protocol_data.blue_sprite_train_stage
	self.__cur_info.blue_sprite_train_times = protocol_data.blue_sprite_train_times
	self.__cur_info.blue_sprite_list = protocol_data.blue_sprite_list

	--重置公共怒气
    GlobalVOMgr:SetAnger(0)
   	GlobalVOMgr:SetAnger(0, true)        

	--进入副本场景
	if not GlobalScene:ChangeScene(self.__cur_info.type, self.__cur_info.gamelevel_id) then
		return
	end

	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", ConfigAdapter.Common.GetPVPACCFactor())
end

--释放技能处理
function Modules.BattleModule:__HandleUseSkill_PVP(protocol_data)
	local obj_id = self:__ConvertSID2CID_PVP(protocol_data.obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or obj:IsDead() then
		return
	end

	--红蓝方英雄
	if obj:IsHeroPVP() or obj:IsMonsterPVP() then
		self:__ClearUseSkillLockTimer_PVP(obj_id)

		obj:TriggerAngerOrJointSkill(protocol_data.skill_id)

	--精灵
	elseif obj:IsSpirit() then
		local world_pos = Vector2D.New(protocol_data.x, protocol_data.y)

		if not obj:CheckBindObjType(Scene.ObjType.HERO_PVP) then
			--敌方精灵，转换坐标
			local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
			world_pos.x = battle_region.org.x + battle_region.size.width - 1 - (world_pos.x - battle_region.org.x) 
		end

		obj:DoNull(nil, nil, true)
		obj:DoAttack(protocol_data.skill_id, nil, nil, world_pos)		
	end
end

--血量变化同步处理
function Modules.BattleModule:__HandleHPChangeSync_PVP(protocol_data)
	local tgt_obj_id = self:__ConvertSID2CID_PVP(protocol_data.tgt_obj_id)
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", tgt_obj_id, protocol_data.change_hp, protocol_data.damage_type, true)	
end

--BUFF同步处理
function Modules.BattleModule:__HandleBuffSync_PVP(protocol_data)
	local src_obj_id = self:__ConvertSID2CID_PVP(protocol_data.src_obj_id)
	local tgt_obj_id = self:__ConvertSID2CID_PVP(protocol_data.tgt_obj_id)
	local src_obj = GlobalScene:GetObj(src_obj_id)
	local tgt_obj = GlobalScene:GetObj(tgt_obj_id)
	if not src_obj or not tgt_obj then
		return
	end

	GlobalBuffMgr:CreateBuff(protocol_data.buff_id, src_obj:GetVO(), tgt_obj:GetVO(), protocol_data.is_sprite_bless_buff == Macros.Global.TRUE)						
end

--战斗结束处理
function Modules.BattleModule:__HandleBattleEnd_PVP(ret_type, protocol_data)
	local red_result_data = {}
	red_result_data.old_rank = protocol_data.red_old_rank
	red_result_data.new_rank = protocol_data.red_new_rank
	red_result_data.last_week_rank = protocol_data.red_last_week_rank
	red_result_data.score = protocol_data.red_score	
	self.__cur_info.red_result_data = red_result_data

	local blue_result_data = {}
	blue_result_data.old_rank = protocol_data.blue_old_rank
	blue_result_data.new_rank = protocol_data.blue_new_rank
	blue_result_data.last_week_rank = protocol_data.blue_last_week_rank
	blue_result_data.score = protocol_data.blue_score
	self.__cur_info.blue_result_data = blue_result_data
	
	self:CloseView()

	if ret_type == Modules.BattleModule.ResultType.WIN then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleSuccessHandle")

	elseif ret_type == Modules.BattleModule.ResultType.LOSE then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")

	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleTieHandle")
	end

	--战斗结束事件
	self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END, self.__cur_info.gamelevel_id, is_win)	
end

--检测释放技能锁
function Modules.BattleModule:__CheckUseSkillLock_PVP(obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or not obj:IsHeroPVP() then
		return
	end

	if self.__cur_info.use_skill_lock_info then
		return true
	end
	self.__cur_info.use_skill_lock_info = {}
	self.__cur_info.use_skill_lock_info.obj_id = obj_id
	self.__cur_info.use_skill_lock_info.timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
    function()
    	self.__cur_info.use_skill_lock_info = nil
    end), Net.NetAdapter.NetLoadingTimeout)
end

--销毁释放技能锁计时器
function Modules.BattleModule:__ClearUseSkillLockTimer_PVP(obj_id, force)
	local obj = GlobalScene:GetObj(obj_id)
	if not force and (not obj or not obj:IsHeroPVP()) then
		return
	end

	if self.__cur_info.use_skill_lock_info and self.__cur_info.use_skill_lock_info.timer then
		GlobalTimerQuest:CancelQuest(self.__cur_info.use_skill_lock_info.timer)
		self.__cur_info.use_skill_lock_info.timer = nil
	end
end

--构造目标数据
function Modules.BattleModule:__BuildTargetData_PVP(protocol_data)
	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	local is_red = role_id == protocol_data.red_role_id
	local target_data = {}
	target_data.name = is_red and protocol_data.blue_role_name or protocol_data.red_role_name
	target_data.server_id = is_red and protocol_data.blue_server_id or protocol_data.red_server_id
	target_data.fighting_power = is_red and protocol_data.blue_fighting_power or protocol_data.red_fighting_power
	target_data.head_icon = is_red and protocol_data.blue_head_image_id or protocol_data.red_head_image_id

	if self.__cur_info.is_pve == true then
		local icon_ , name_ = CallModuleFunc(ModuleType.PVP, "GetRandomInfo")
		target_data.head_icon = icon_
		target_data.name = name_

		local server_id_list = CallModuleFunc(ModuleType.LOGIN, "GetServerIDList")
		local random_index = MathTool.GetRandom(1, #server_id_list)
		target_data.server_id = server_id_list[random_index]
	end

	self:FireNextFrame(Macros.Event.BattleModule.PVP_READY_NOTIFY, target_data)

	return target_data
end