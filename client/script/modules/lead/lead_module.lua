--[[
%% @module: 新手指引模块
%% @author: swordman sue
%% @created: 2016-12-23
%% @description: 负责新手指引相关
--]]

Modules = Modules or {}

Modules.LeadModule = Modules.LeadModule or BaseClass(BaseModule)

--指引完成标签(此值不能修改)
local LeadFinishedTag = 88888888

--特殊指引表
local SpIDMap = 
{
	[Macros.Game.LeadSpecialID.BATTLE_ACC] = true,
	[Macros.Game.LeadSpecialID.RELEASE_ANGER_OR_JOINT_PLUS_SKILL] = true,
	[Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_1] = true,
	[Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_2] = true,
	[Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_3] = true,
	[Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_4] = true,
	[Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_5] = true,
	[Macros.Game.LeadSpecialID.FRIEND_ASSIST_LEAD] = true,
	[Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_ONE] = true,
	[Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_TWO] = true,
	[Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_THREE] = true,
	[Macros.Game.LeadSpecialID.RELEASE_ANGER_OR_JOINT_PLUS_SKILL_WEAK] = true,
	[Macros.Game.LeadSpecialID.RELEASE_ANGER_OR_JOINT_PLUS_SKILL_WEAK2] = true,
}

--特殊操作类型表
local SpOperIDMap = 
{
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_CHAPTER] = true,
	--选择主线关卡（关卡ID）
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_GAMELEVEL] = true,
	--选择主线星级宝箱（宝箱索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_STAR_BOX] = true,
	--选择主线关卡宝箱（关卡ID）
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_GAMELEVEL_BOX] = true,
	--英雄释放怒气技能
	[Macros.Game.LeadOperateSpecialID.HERO_RELEASE_ANGER_SKILL] = true,
	--英雄释放合击技能
	[Macros.Game.LeadOperateSpecialID.HERO_RELEASE_JOINT_SKILL] = true,
	--英雄出战按钮（英雄ID）
	[Macros.Game.LeadOperateSpecialID.HERO_BATTLE_BTN] = true,
	--阵容切换英雄
	[Macros.Game.LeadOperateSpecialID.LINEUP_CHANGE_HERO] = true,
	--背包使用物品（物品ID）
	[Macros.Game.LeadOperateSpecialID.PACK_USE_ITEM] = true,
	--背包使用礼包（物品ID）
	[Macros.Game.LeadOperateSpecialID.PACK_USE_GIFT] = true,
	--日常成就前往（成就ID）
	[Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GOTO] = true,
	--日常成就领奖（成就ID）
	[Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GET_REWARD] = true,
	--阵容装备格子（装备类型）
	[Macros.Game.LeadOperateSpecialID.LINEUP_EQUIP_GRID] = true,
	--阵容圣器格子（圣器类型）
	[Macros.Game.LeadOperateSpecialID.LINEUP_TREASURE_GRID] = true,
	--装备穿戴（格子索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.EQUIP_WEAR] = true,
	--圣器穿戴（格子索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.TREASURE_WEAR] = true,
	--圣器争夺-选择圣器（圣器ID）
	[Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_CHOOSE_TREASURE] = true,
	--圣器争夺-选择碎片（碎片ID）
	[Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_CHOOSE_FRAGMENT] = true,
	--挑战界面-选择入口（系统ID）
	[Macros.Game.LeadOperateSpecialID.CHALLENGE_CHOOSE_ENTRY] = true,
	--爬塔界面-选择关卡（爬塔关卡索引）
	[Macros.Game.LeadOperateSpecialID.TOWER_CHOOSE_GAMELEVEL] = true,
	--爬塔商店-购买（商品ID）
	[Macros.Game.LeadOperateSpecialID.TOWER_SHOP_BUY] = true,
	--装备界面-装备（格子索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.EQUIP_CHOOSE] = true,
	--装备界面-碎片-合成（格子索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.EQUIP_FRAGMENT_COMPOSE] = true,
	--选择英雄章节（章节索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.HERO_DUNGEON_SELECT_CHAPTER] = true,
	--选择英雄副本关卡（关卡ID）
	[Macros.Game.LeadOperateSpecialID.HERO_DUNGEON_SELECT_GAMELEVEL] = true,
	--阵容-援军-阵位（阵位索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.LEINUP_REINFORCEMENTS_POS] = true,
	--英雄商店-购买商品（商品索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.HERO_SHOP_BUY] = true,
	--英雄提升-选择英雄
	[Macros.Game.LeadOperateSpecialID.HERO_PROMOTE_SELECT_HERO] = true,
	--主界面-英雄阵位（索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.CITY_HERO_LINEUP_BTN] = true,
	--装备精炼，精炼石品质（索引，从1开始）
	[Macros.Game.LeadOperateSpecialID.EQUIP_REFINE] = true,
	--战队升级-继续
	[Macros.Game.LeadOperateSpecialID.TEAM_UPDATE_CONTINUE] = true,
	--声望商店购买（商品ID）
	[Macros.Game.LeadOperateSpecialID.ARENA_REPUTATION_SHOP] = true,
	--英雄获得展示界面-继续
	[Macros.Game.LeadOperateSpecialID.HERO_OBTAIN_VIEW_CONTINUE] = true,
	--竞技场排名奖励点击继续
	[Macros.Game.LeadOperateSpecialID.ARENA_RANK_BREAK_CONTINUE] = true,
	--召唤英雄展示点击继续
	[Macros.Game.LeadOperateSpecialID.HERO_RECRUIT_HERO_OBTAIN_VIEW_CONTINUE] = true,
	--阵容-援军
	[Macros.Game.LeadOperateSpecialID.LEINUP_REINFORCEMENTS_ENTRY] = true,
	--主线关卡界面-阵容
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_GAMELEVEL_HERO_LINEUP] = true,
	--主线关卡界面-排行榜
	[Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_GAMELEVEL_RANK] = true,
	--深渊商店-购买（商品ID）
	[Macros.Game.LeadOperateSpecialID.CHASM_SHOP_BUY] = true,
	--爵位系统-激活徽章
	[Macros.Game.LeadOperateSpecialID.KNIGHTHOOD_LIGHT_UP] = true,
	--结算界面-点击继续
	[Macros.Game.LeadOperateSpecialID.BATTLE_RESULT_CONTINUE] = true,
	--好友助战-点击继续
	[Macros.Game.LeadOperateSpecialID.FRIEND_ASSIST_CONTINUE] = true,
	--夺宝界面-退出按钮
	[Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_EXIT_BTN] = true,
	--主界面点击建筑
	[Macros.Game.LeadOperateSpecialID.MAINCITY_CLICK_BUILDING] = true,
}

--关键点协议请求超时
local KeyPointReqTimeout = 3

--指引步骤超时
local SectionTimeout = 5
 
function Modules.LeadModule:__init()
	self.__can_update = true
	if not StartConfig.SkipLead then
		StartConfig.SkipLead = CPPSdkTools.IsInExamine()
	end
end

function Modules.LeadModule:__delete()
	self:Stop(true)
end

--初始化进度记录
function Modules.LeadModule:SetupProgressTagRecord()
	self:__ReadLocalProgressTagRecord()
	self:__SyncLocalProgressTagRecord()
end

--更新
function Modules.LeadModule:Update(now_time, elapse_time)
	--保存指引相关记录(本地)
	self:__SaveLocalProgressTagRecord(now_time)

	if self.__oper_layer then
		self.__oper_layer:SetTouchSwallow(false)
	end

	--强制指引步骤执行超时
	if self.__todo_next_section_timeout and self.__todo_next_section_timeout <= Macros.Global.NowTime then
		local cur_lead_id = self.__cur_lead_info and self.__cur_lead_info.index or nil
		local cur_section_index = self.__cur_section_index

		--强制中断当前指引
		self:Stop()

		--若当前指引属于必须指引，则提示网络繁忙，让用户重新登录并进入指引恢复逻辑
		local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
		local max_necessary_lead_id = table.maxn(config_nessary_leads)
		if cur_lead_id and cur_lead_id <= max_necessary_lead_id then
		    LogicGUI.TemplAffirmTips.New({
		    	type = LogicGUI.TemplAffirmTipsType.YES,
		        content = CPPGameLib.GetString("common_network_busy2", 0, cur_lead_id, cur_section_index),
		        confirm_func = function()
					CallModuleFunc(ModuleType.LOGIN, "Logout")
		        end,
		       	show_close_btn = false,
		        render_queue = Scene.RenderQueue.GRQ_UI_MAX,
		    })			
		end
		return
	end

	--强制指引中的关键点协议请求超时
	if self.__key_point_req_info then
		self.__key_point_req_info.elapse_time = self.__key_point_req_info.elapse_time + elapse_time
		if self.__key_point_req_info.elapse_time >= KeyPointReqTimeout then
			self.__key_point_req_info = nil

			--强制完成当前指引步骤
			self:SetProgressTag(self.__cur_lead_info.index, self.__cur_section_index, true)

		    LogicGUI.TemplAffirmTips.New({
		    	type = LogicGUI.TemplAffirmTipsType.YES,
		        content = CPPGameLib.GetString("common_network_busy2", 1, self.__cur_lead_info.index, self.__cur_section_index),
		        confirm_func = function()
					CallModuleFunc(ModuleType.LOGIN, "Logout")
		        end,
		       	show_close_btn = false,
		        render_queue = Scene.RenderQueue.GRQ_UI_MAX,
		    })			
		end
		return
	end

	if GlobalViewMgr:IsInViewOpenEffect() or GUITemplateMgr:IsInTemplateOpenEffect() then
		--界面/模板打开效果中，过滤
		return
	end

	--指引下一个步骤
	if self.__todo_next_section_time and self.__todo_next_section_time <= Macros.Global.NowTime then
		self.__todo_next_section_time = nil

		local pre_section_info = self:GetSectionInfo(self.__cur_section_index - 1)
		if not pre_section_info or pre_section_info.auto_check_next ~= Macros.Global.FALSE then
			self:__DoSectionOperate(self.__todo_next_section_widget)

			--派发切换到下一个步骤的事件
			self:FireNextFrame(Macros.Event.LeadModule.CHECK_NEXT_SECTION, pre_section_info)
		end
		self.__todo_next_section_widget = nil
	end

	--延后一帧检测特殊操作
	if self.__section_operate_sp_list then
		for _, sp_info in ipairs(self.__section_operate_sp_list) do
			self:__CheckSectionOperateSP(sp_info.sp_oper_id, sp_info.widget, unpack(sp_info.arg_list))
		end
		self.__section_operate_sp_list = nil
	end

	--怒气变化锁
	if self.__lock_common_anger_change and not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPause") then
		self.__lock_common_anger_elapse_time = self.__lock_common_anger_elapse_time + elapse_time
		if self.__lock_common_anger_elapse_time >= 3 then
			self.__lock_common_anger_change = false
		end
	end
end

--是否指引中
function Modules.LeadModule:IsInLead()
	return self.__cur_lead_info ~= nil
end

--指引是否开启过
function Modules.LeadModule:IsLeadStarted(lead_id)
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)

	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	if lead_id <= max_necessary_lead_id then
		--若必须指引由副本通关触发，则根据副本是否通关判断开启情况
		local condition
		local config_lead = CPPGameLib.GetConfig("Lead", lead_id)
		if config_lead then
			condition = config_lead.condition
			if not condition and config_lead.condition_story_id then
				local config_story = CPPGameLib.GetConfig("Story", config_lead.condition_story_id)
				condition = config_story and config_story.condition or nil
			end
		end
		if condition and condition[1] == Macros.Game.LeadConditionType.PASS_GAME_LEVEL then
			local var = CallModuleFunc(ModuleType.DUNGEON, "IsPassMainDungeonGameLevel", condition[2])
			return var
		end
	end

	local prog_tag = self:GetProgressTag(lead_id)
	return prog_tag >= 0
end

--指引是否中断
function Modules.LeadModule:IsLeadAbort(lead_id)
	if self:IsLeadStarted(lead_id) and not self:IsLeadFinished(lead_id) then
		return true
	end
end

--指引是否完成过
function Modules.LeadModule:IsLeadFinished(lead_id)
	local prog_tag = self:GetProgressTag(lead_id)
	return prog_tag == LeadFinishedTag
end

--最后的必须指引是否开启过
function Modules.LeadModule:IsMaxNecessaryLeadStarted()
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	return self:IsLeadStarted(max_necessary_lead_id)
end

--最后的必须指引是否完成
function Modules.LeadModule:IsMaxNecessaryLeadFinished()
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	return self:IsLeadFinished(max_necessary_lead_id)
end

--必须指引是否中断
function Modules.LeadModule:IsNecessaryLeadAbort(lead_id)
	if not self:IsLeadAbort(lead_id) then
		return false
	end

	--进一步检查，若后续必须指引已开启，则证明未中断
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	for i = lead_id + 1, max_necessary_lead_id do
		if self:IsLeadStarted(i) then
			return false
		end		
	end	

	--进一步检查，若后续指引已开启，则证明未中断
	if self:IsLeadStarted(max_necessary_lead_id + 1) then
		return false
	end
	
	return true
end

--开始指引
function Modules.LeadModule:Start(lead_id, section_index, end_func, ignore_delay_time)
	if StartConfig.SkipLead then
		return
	end

	if self.__cur_lead_info then
		return
	end	
	if SpIDMap[lead_id] then
		if not self.__cur_sp_lead_id then
			self.__cur_sp_lead_id = lead_id
		else
			self.__cur_sp_lead_id_two = lead_id
		end
		self.__is_cur_sp_leading = true
		self:FireNextFrame(Macros.Event.LeadModule.START_SP, lead_id)		
		return
	end
	local config_lead = CPPGameLib.GetConfig("Lead", lead_id)
	if not config_lead then
		return
	end

	--更新指引进度标记
	self:SetProgressTag(lead_id, 0, true)

	--仅用于更新进度
	if self.__just_update_progress_tag then
		return
	end
	LogTool.LogInfo("[Modules.LeadModule:Start] lead_id:%d section:%d", lead_id, section_index)

	self.__cur_lead_info = config_lead
	self.__cur_section_index = section_index - 1
	self.__cur_lead_end_func = end_func
	self:__NextSection(ignore_delay_time)

	if self.__cur_lead_info then
		--重置已触发标记
		for _, section_info in ipairs(self.__cur_lead_info.section_list) do
			section_info.is_triggered = false
		end

		self:FireNextFrame(Macros.Event.LeadModule.START, self.__cur_lead_info.index)
	end
end

--结束指引
function Modules.LeadModule:Stop(for_delete)
	if not self.__cur_lead_info then
		return
	end

	LogTool.LogInfo("[Modules.LeadModule:Stop] lead_id:%d", self.__cur_lead_info.index)

	local last_lead_id = self.__cur_lead_info.index

	--更新指引进度标记
	if not for_delete then
		self:SetProgressTag(self.__cur_lead_info.index, LeadFinishedTag, true)
	end

	--指引结束回调
	local end_call_func = self.__cur_lead_info.section_list[#self.__cur_lead_info.section_list].end_call_func
	if end_call_func then
		CallModuleFunc(ModuleType[end_call_func[1]],end_call_func[2],select(3,unpack(end_call_func)))
	end 

	self.__cur_lead_info = nil
	self.__cur_section_index = nil
	self.__section_operate_sp_list = nil
	self.__skip_section_info = nil
	self.__recovery_open_info = nil
	self.__todo_next_section_time = nil
	self.__todo_next_section_timeout = nil
	self.__todo_next_section_widget = nil
	self.__key_point_req_info = nil

	--指引结束回调
	if self.__cur_lead_end_func then
		self.__cur_lead_end_func()
		self.__cur_lead_end_func = nil
	end	

	self:__Resume()
	self:__ClearAllLayers()
	self:__ClearArrowAndTips()
	self:__RecoveryOperWidget(for_delete)

	self:FireNextFrame(Macros.Event.LeadModule.STOP, last_lead_id)
end

--打断指引
function Modules.LeadModule:Break()
    local section_info = CallModuleFunc(ModuleType.LEAD, "GetSectionInfo")
    if section_info and section_info.break_story_id then
	    CallModuleFunc(ModuleType.STORY, "Start", section_info.break_story_id)
	end
	self:Stop()
end

--[[
触发条件参考Macros.Game.LeadConditionType
]]
--检测指引开启
function Modules.LeadModule:CheckStart(condition_type, ...)
	if StartConfig.SkipLead then
		return
	end

	if self.__cur_lead_info then
		return
	end	

	if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
		return
	end

	local arg_list = {...}
	local ret_arg_list = {}

	local config_leads = CPPGameLib.GetConfig("Lead", nil, nil, true)
	for lead_id, lead_info in pairs(config_leads) do
		if lead_info.condition and lead_info.condition[1] == condition_type then
			local is_triggered = true
			for i = 2, #lead_info.condition do
				if not self:__CompareCondition(arg_list, ret_arg_list, lead_info.condition, i) then
					is_triggered = false
					break
				end
			end
			if is_triggered then
				--触发新手指引			
				self:Start(lead_id, 1, select(#lead_info.condition, ...))
				break
			end
		end
	end

	if self.__cur_lead_info or self.__is_cur_sp_leading then
		self.__is_cur_sp_leading = false
		return true, ret_arg_list	
	end
end

--检测特殊操作
function Modules.LeadModule:CheckSectionOperateSP(sp_oper_id, widget, ...)
	if not sp_oper_id then
		LogTool.LogError("[Modules.LeadModule:CheckSectionOperateSP] 参数sp_oper_id为空")
		return
	end

	if not widget then
		LogTool.LogError("[Modules.LeadModule:CheckSectionOperateSP] 参数widget为空")
		return
	end

	local section_info = self:GetSectionInfo()
	if not section_info or section_info.is_triggered then
		--当前步骤无效或当前步骤已触发
		return
	end

	if section_info.operate_params[1] ~= sp_oper_id then
		return
	end

	local sp_info = {}
	sp_info.sp_oper_id = sp_oper_id
	sp_info.widget = widget
	sp_info.arg_list = {...}
	self.__section_operate_sp_list = self.__section_operate_sp_list or {}
	table.insert(self.__section_operate_sp_list, sp_info)
end

--恢复检测
function Modules.LeadModule:CheckRecovery()
	if StartConfig.SkipLead then
		return
	end
	
	if self.__has_check_recovery then
		return
	end
	self.__has_check_recovery = true

	if self.__cur_lead_info then
		return
	end	

	local function do_recovery(lead_id, recovery_pre_handle, recovery_id, recovery_section)
		if recovery_pre_handle and recovery_pre_handle[1] ~= -1 then
			--恢复预处理
			GlobalModulesMgr:OpenSystemModule(recovery_pre_handle[1], {select(2, unpack(recovery_pre_handle))})
		end
		self:Start(recovery_id, recovery_section, false)
	end

	local config_recovery, recovery_index_info
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	for lead_id = 1, max_necessary_lead_id do
		config_recovery = CPPGameLib.GetConfig("NecessaryLead", lead_id, false)
		if config_recovery then
			--必须指引未开启，根据前置指引恢复信息进行恢复
			if not self:IsLeadStarted(lead_id) and not self:IsMaxNecessaryLeadStarted() then
				if config_recovery.prefix_recovery_index_info then

					--恢复处理
					do_recovery(lead_id, config_recovery.prefix_recovery_pre_handle, config_recovery.prefix_recovery_index_info[1], config_recovery.prefix_recovery_index_info[2])
					return				
				end

			--必须指引中断，根据中断指引恢复信息进行恢复
			elseif self:IsNecessaryLeadAbort(lead_id) then
				recovery_index_info = config_recovery.recovery_index_info
				if recovery_index_info then
					local last_section_index = self:GetProgressTag(lead_id)
					for index, sub_recovery_index_info in ipairs(recovery_index_info) do
						if sub_recovery_index_info[1] == -1 or last_section_index < sub_recovery_index_info[1] then
							if #sub_recovery_index_info >= 5 then
								--跳过步骤
								if sub_recovery_index_info[4] >= 1 and sub_recovery_index_info[5] >= 1 then
									self.__skip_section_info = {}
									self.__skip_section_info.min = sub_recovery_index_info[4]
									self.__skip_section_info.max = sub_recovery_index_info[5]
								end

								--恢复性跳转
								if sub_recovery_index_info[6] then
									self.__recovery_open_info = {}
									self.__recovery_open_info.section_index = sub_recovery_index_info[6]
									self.__recovery_open_info.sys_id = sub_recovery_index_info[7]
									self.__recovery_open_info.sys_params = {select(8, unpack(sub_recovery_index_info))}
								end
							end

							--恢复处理
							do_recovery(lead_id, config_recovery.recovery_pre_handle and config_recovery.recovery_pre_handle[index], sub_recovery_index_info[2], sub_recovery_index_info[3])
							return
						end
					end
				end
			end
		end
	end
end

--检测圣器首次满足合成条件
function Modules.LeadModule:CheckTreasureComposeFirstTime(treasure_list)
	if not treasure_list then
		return
	end

	local flag = self:GetProgressTag(Macros.Game.LeadProgressTagType.LEAD_TREASURE_COMPOSABLE_FIRST_TIME)
	if flag == Macros.Global.TRUE then
		return
	end

	local config_treasure_compose, can_compose
	for _, treasure_data in pairs(treasure_list) do
		can_compose = true

		config_treasure_compose = CPPGameLib.GetConfig("TreasureCompose", treasure_data.treasureId, false)
		if config_treasure_compose then
			for _, item_id in ipairs(config_treasure_compose.cost_item_list) do
				if ObjIDTool.GetOwnNum(item_id) <= 0 then
					can_compose = false
					break
				end
			end				
		else
			can_compose = false
		end

		if can_compose then
			self:SetProgressTag(Macros.Game.LeadProgressTagType.LEAD_TREASURE_COMPOSABLE_FIRST_TIME, Macros.Global.TRUE)

			--指引检测
			self:CheckStart(Macros.Game.LeadConditionType.TREASURE_COMPOSE_FIRST_TIME)

			--剧情检测
			CallModuleFunc(ModuleType.STORY, "CheckStart", Macros.Game.LeadConditionType.TREASURE_COMPOSE_FIRST_TIME)
			return
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--事件回调
--------------------------------------------------------------------------------------------------------------------------------
--战役开始
function Modules.LeadModule:HandleBattleStart(gamelevel_id, battle_index)
	--检测精灵系统开启后第一次进入副本
	local flag = self:GetProgressTag(Macros.Game.LeadProgressTagType.LEAD_BATTLE_START_AFTER_SPIRIT_OPENED)
	if flag ~= Macros.Global.TRUE and GlobalModulesMgr:IsOpen(Macros.Game.SystemID.WORLD_HEART) then
		self:SetProgressTag(Macros.Game.LeadProgressTagType.LEAD_BATTLE_START_AFTER_SPIRIT_OPENED, Macros.Global.TRUE)

		--指引检测
		self:CheckStart(Macros.Game.LeadConditionType.BATTLE_START_AFTER_SPIRIT_OPENED)		

		--剧情检测
		CallModuleFunc(ModuleType.STORY, "CheckStart", Macros.Game.LeadConditionType.BATTLE_START_AFTER_SPIRIT_OPENED)		
	end

	if CallModuleFunc(ModuleType.DUNGEON, "IsPassGameLevel", gamelevel_id) then
		return
	end
	self:CheckStart(Macros.Game.LeadConditionType.BATTLE_START, gamelevel_id, battle_index)
end

--击杀怪物
function Modules.LeadModule:HandleMonsterDead(obj_id)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		return
	end
	local obj_vo = obj:GetVO()

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if CallModuleFunc(ModuleType.DUNGEON, "IsPassGameLevel", battle_info.gamelevel_id) then
		return
	end

	self:CheckStart(Macros.Game.LeadConditionType.MONSTER_DEAD, battle_info.gamelevel_id, battle_info.battle_index, obj_vo.info_id)
end

--所有怪物被击杀
function Modules.LeadModule:HandleAllMonsterDead(gamelevel_id, battle_index, end_func)
	if CallModuleFunc(ModuleType.DUNGEON, "IsPassGameLevel", gamelevel_id) then
		return
	end

	return self:CheckStart(Macros.Game.LeadConditionType.ALL_MONSTER_DEAD, gamelevel_id, battle_index, end_func)
end

--公共怒气变化
function Modules.LeadModule:HandleCommonAngerChange(old_anger, new_anger)
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if CallModuleFunc(ModuleType.DUNGEON, "IsPassGameLevel", battle_info.gamelevel_id) then
		return
	end

	if self.__lock_common_anger_change then
		return
	end

	--检测怒气达到X（13|关卡ID|万分比）
	self.__common_anger_achieve_percent_record = self.__common_anger_achieve_percent_record or {}
	local ret, ret_arg_list = self:CheckStart(Macros.Game.LeadConditionType.ANGER_ACHIEVE_PERCENT, battle_info.gamelevel_id, new_anger / GlobalVOMgr:GetMaxAnger())
	if ret and ret_arg_list then
		--TODO：完成剧场模式后，去掉以下处理
		local to_anger = battle_info.gamelevel_id == 10103 and 200 or 100
		if new_anger < to_anger then
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoAngerChange", to_anger - new_anger)
		end		
		
		local fixed_index = ret_arg_list[1]
		if fixed_index then
			self.__lock_common_anger_change = true
			self.__lock_common_anger_elapse_time = 0
			self.__common_anger_achieve_percent_index = fixed_index
			self.__common_anger_achieve_percent_record[fixed_index] = true
		end
	end

	--检测怒气达到X（4|关卡ID|固定值|索引）
	self.__common_anger_achieve_fixed_record = self.__common_anger_achieve_fixed_record or {}
	local ret, ret_arg_list = self:CheckStart(Macros.Game.LeadConditionType.ANGER_ACHIEVE_FIXED, battle_info.gamelevel_id, new_anger)
	local is_sp_lead = false
	if ret and ret_arg_list and not is_sp_lead then
		local fixed_index = ret_arg_list[1]
		if fixed_index then
			self.__lock_common_anger_change = true
			self.__lock_common_anger_elapse_time = 0
			self.__common_anger_achieve_fixed_index = fixed_index
			self.__common_anger_achieve_fixed_record[fixed_index] = true
		end
	end
end

--战斗结束
function Modules.LeadModule:HandleBattleEnd(gamelevel_id, is_win)
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.type ~= Macros.Game.FBType.MAIN_DUNGEON then
		return
	end

    local flag = self:GetProgressTag(Macros.Game.LeadProgressTagType.LEAD_BATTLE_FAILED_FIRST_TIME)
    if flag ~= Macros.Global.TRUE and is_win == Macros.Global.FALSE then
    	
    	--首次战斗失败，触发指引
		local ret = self:CheckStart(Macros.Game.LeadConditionType.BATTLE_FAILED_FIRST_TIME)
		if ret then
			self:SetProgressTag(Macros.Game.LeadProgressTagType.LEAD_BATTLE_FAILED_FIRST_TIME, Macros.Global.TRUE)
		end
    end
end

--角色名改变
function Modules.LeadModule:HandleRoleNameChanged()
	self:CheckStart(Macros.Game.LeadConditionType.ROLE_NAME_CHANGED)
end

--通关一个副本
function Modules.LeadModule:HandlePassGameLevel(gamelevel_id, for_story)
	local function handle_func()		
		self:CheckStart(Macros.Game.LeadConditionType.PASS_GAME_LEVEL, gamelevel_id)		
	end	

	--结算界面有指引
	local gamelevel_info = CPPGameLib.GetConfig("LeadBattleResultGL", gamelevel_id, false)
	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") or (not for_story and gamelevel_info) then
		handle_func()	
	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			handle_func()

			if close_callback then
				close_callback()
			end
		end, Macros.Game.SpDialogPriority.LEAD, nil, true)
	end	

	--仅用于更新进度
	self.__just_update_progress_tag = true
	self:CheckStart(Macros.Game.LeadConditionType.PASS_GAME_LEVEL, gamelevel_id)		
	self.__just_update_progress_tag = false
end

--首次通关爬塔副本
function Modules.LeadModule:HandlePassTowerGameLevelFirstTime()
	local function handle_func()
		self:CheckStart(Macros.Game.LeadConditionType.TOWER_GAMELEVEL_PASSED_FIRST_TIME)					
	end	

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		handle_func()
	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			handle_func()

			if close_callback then
				close_callback()
			end
		end, Macros.Game.SpDialogPriority.LEAD, nil, true)
	end

	--仅用于更新进度
	self.__just_update_progress_tag = true
	self:CheckStart(Macros.Game.LeadConditionType.TOWER_GAMELEVEL_PASSED_FIRST_TIME, gamelevel_id)		
	self.__just_update_progress_tag = false
end

--某个系统开启
function Modules.LeadModule:HandleSystemOpened(open_list)
	if not open_list then
		return
	end

	local function handle_func()		
		for _, sys_id in ipairs(open_list) do
			self:CheckStart(Macros.Game.LeadConditionType.SYSTEM_OPENED, sys_id)
		end
	end

	handle_func()
end

--英雄阵位开启
function Modules.LeadModule:HandleHeroLineupPosOpened(lineup_index)
	local function handle_func()
		self:CheckStart(Macros.Game.LeadConditionType.HERO_LINEUP_POS_OPENED, lineup_index)	
	end

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		handle_func()
	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			handle_func()
		
			if close_callback then
				close_callback()
			end
		end, Macros.Game.SpDialogPriority.LEAD, nil, true)
	end
end

--援军阵位开启
function Modules.LeadModule:HandleReinforcementLineupPosOpened(lineup_index)	
	local function handle_func()
		self:CheckStart(Macros.Game.LeadConditionType.REINFORCEMENT_LINEUP_POS_OPENED, lineup_index)	
	end

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		handle_func()
	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			handle_func()
		
			if close_callback then
				close_callback()
			end
		end, Macros.Game.SpDialogPriority.LEAD, nil, true)
	end
end

--英雄降临界面弹出
function Modules.LeadModule:HandleHeroAdventViewOpened(chapter_index)	
	if not chapter_index then
		return
	end

	local flag = self:GetProgressTag(Macros.Game.LeadProgressTagType.LEAD_HERO_ADVENT_FIRST_TIME + chapter_index)
	if flag == Macros.Global.TRUE then
		GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.AFTER_HERO_ADVENT_POPUP)
		return
	end
	local can_start,ret_arg_list = self:CheckStart(Macros.Game.LeadConditionType.HERO_ADVENT_FIRST_TIME_OPENED, chapter_index)
	if can_start and ret_arg_list then
		self:SetProgressTag(Macros.Game.LeadProgressTagType.LEAD_HERO_ADVENT_FIRST_TIME + chapter_index, Macros.Global.TRUE)		
	else
		GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.AFTER_HERO_ADVENT_POPUP)
	end
end

--首次深渊失败并且杀敌数量大于某个数量
function Modules.LeadModule:HandleChasmFirstChallengeLose(keep_kill_num)	
	if not keep_kill_num then
		return
	end	
	local flag = self:GetProgressTag(Macros.Game.LeadProgressTagType.CHASM_PASSED_FIRST_TIME)
	if flag == Macros.Global.TRUE then
		return
	end
	local can_start,ret_arg_list = self:CheckStart(Macros.Game.LeadConditionType.CHASM_PASSED_FIRST_TIME, keep_kill_num)
	self:SetProgressTag(Macros.Game.LeadProgressTagType.CHASM_PASSED_FIRST_TIME, Macros.Global.TRUE)
end

--界面、选项卡界面、模板打开、主UI显示
function Modules.LeadModule:HandleEventForViewOpen(view_id, view_type)
	if CallModuleFunc(ModuleType.LOADING, "IsLoadingViewType", view_type) then
		return
	end

	if self:__FilterViewOrTemplateOpenEffect(Modules.LeadModule.HandleEventForViewOpen) then
		return
	end

	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.VIEW_CTRL)
	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.VIEW_CLOSE_BTN)
	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.VIEW_TAB)
end
function Modules.LeadModule:HandleEventForTabViewOpen()
	if self:__FilterViewOrTemplateOpenEffect(Modules.LeadModule.HandleEventForTabViewOpen) then
		return
	end
	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.TAB_VIEW_CTRL)
end
function Modules.LeadModule:HandleEventForTemplateOpen()
	if self:__FilterViewOrTemplateOpenEffect(Modules.LeadModule.HandleEventForTemplateOpen) then
		return
	end
	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.TEMPLATE_CTRL)
end
function Modules.LeadModule:HandleMaincityVisibleEvent()
	if self:__FilterViewOrTemplateOpenEffect(Modules.LeadModule.HandleMaincityVisibleEvent) then
		return
	end
	self:__DoSectionOperate(nil, Macros.Game.LeadOperateType.CITY_ENTRY)
end

--界面/模板打开效果完成
function Modules.LeadModule:HandleEventForViewOrTemplateEffectFinished()
	if GlobalViewMgr:IsInViewOpenEffect() or GUITemplateMgr:IsInTemplateOpenEffect() then
		return
	end
	if self.__event_callback_cache_list then
		for _, callback in ipairs(self.__event_callback_cache_list) do
			callback(self)
		end
		self.__event_callback_cache_list = nil
	end
end

--协议发送响应
function Modules.LeadModule:HandleOnSendProtocol(protocol_no)
	if self.__key_point_req_info and self.__key_point_req_info.protocol_no == protocol_no then
		self.__key_point_req_info = nil
	end
end

--获取指引信息
function Modules.LeadModule:GetLeadInfo()
	return self.__cur_lead_info
end

--获取步骤信息
function Modules.LeadModule:GetSectionInfo(section_index)
	if not self.__cur_lead_info then
		return
	end

	return self.__cur_lead_info.section_list[section_index or self.__cur_section_index]
end

--获取步骤信息
function Modules.LeadModule:GetCurLeadSpLeadId()
	return self.__cur_sp_lead_id,self.__cur_sp_lead_id_two
end

--获取步骤操作信息
function Modules.LeadModule:GetOperateInfo()
	local section_info = self:GetSectionInfo()
	if not section_info then
		return
	end

	local operate_id = section_info.operate_params[1]
	return operate_id, select(2, unpack(section_info.operate_params))
end

function Modules.LeadModule:Dump()
	self.__lock_common_anger_change = false
	self.__common_anger_achieve_fixed_index = nil
	self.__common_anger_achieve_fixed_record = nil
	self.__common_anger_achieve_percent_index = nil
	self.__common_anger_achieve_percent_record = nil
end

--设置指引相关记录
function Modules.LeadModule:SetProgressTag(lead_id, section_index, update_local)
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	if lead_id > 16 and 
		lead_id <= Macros.Game.LeadProgressTagType.LEAD_MAX_LEAD then
		return
	end

	local prog_tag = self:GetProgressTag(lead_id)
	if prog_tag >= section_index then
		--已记录更高进度，过滤
		return
	end

	--记录到本地
	if update_local then
		self:__SetLocalProgressTag(lead_id, section_index)
	end

	--记录到服务端
	CallModuleFunc(ModuleType.ROLE, "SetProgressTag", lead_id, section_index)
end

--获取指引相关记录
function Modules.LeadModule:GetProgressTag(lead_id)
	local local_prog_tag = self:__GetLocalProgressTag(lead_id)
	local server_prog_tag = CallModuleFunc(ModuleType.ROLE, "GetProgressTag", lead_id, -1)
	if server_prog_tag > local_prog_tag then
		return server_prog_tag
	end
	return local_prog_tag
end

--切换到下一个步骤
function Modules.LeadModule:NextSection(update_local)
	local section_info = self:GetSectionInfo()

	if not update_local and section_info.key_point_filter_type then
		if not self.__key_point_req_info then
			self.__key_point_req_info = {}
			self.__key_point_req_info.elapse_time = 0
			self.__key_point_req_info.protocol_no = Macros.Game.NecessaryLeadKeyPointFilterTypeProtocol[section_info.key_point_filter_type]
		end
		return
	end

	--清空关键点信息
	self.__key_point_req_info = nil	

	--清空超时信息
	self.__todo_next_section_timeout = nil

	--恢复操作控件
	self:__RecoveryOperWidget()
	
	if section_info.end_story_id then
		self:__ClearArrowAndTips()

		--更新指引进度标记
		self:SetProgressTag(self.__cur_lead_info.index, self.__cur_section_index, update_local)

		--子步骤完成后触发的剧情
		CallModuleFunc(ModuleType.STORY, "Start", section_info.end_story_id, nil, function()
			--派发切换到下一个步骤的事件
			self:FireNextFrame(Macros.Event.LeadModule.CHECK_NEXT_SECTION)			
			--下一个步骤
			if self.__cur_lead_info then
				self:__NextSection(nil, update_local)
			end
		end)
	else
		--下一个步骤
		self:__NextSection(nil, update_local)
	end
end

--切换到下一个步骤(关键点对应的业务逻辑响应后，调用此接口)
function Modules.LeadModule:NextSectionByLogic(key_point_filter_type)
	local section_info = self:GetSectionInfo()
	if not section_info then
		return
	end

	if self:IsInLead() and section_info.key_point_filter_type and section_info.key_point_filter_type == key_point_filter_type then
		--关键点对应的业务逻辑响应，切换到下一个步骤
		self:NextSection(true)
	end
end

function Modules.LeadModule:FriendAssistSpHandle()
	self.__cur_sp_lead_id = Macros.Game.LeadSpecialID.FRIEND_ASSIST_LEAD
	self.__is_cur_sp_leading = true
	CallModuleFunc(ModuleType.BATTLE,"HandleLeadTag")
end
--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取步骤操作信息
function Modules.LeadModule:__GetOperateInfo()
	local section_info = self:GetSectionInfo()
	if not section_info then
		return
	end

	local operate_id = section_info.operate_params[1]
	return CPPGameLib.GetConfig("LeadOperation", operate_id)
end

--获取步骤操作控件
function Modules.LeadModule:__GetOperateWidget(oper_type)
	local view, widget

	local operate_info = self:__GetOperateInfo()
	if not operate_info then
		return widget
	end

	if oper_type and oper_type ~= operate_info.type then
		return widget
	end

	--主UI入口
	if operate_info.type == Macros.Game.LeadOperateType.CITY_ENTRY then
		if GlobalMainCityEntryMgr then
			widget = GlobalMainCityEntryMgr:GetEntryBtnById(operate_info.params[1])
		end

	--界面选项卡
	elseif operate_info.type == Macros.Game.LeadOperateType.VIEW_TAB then
		view = GlobalViewMgr:GetViewByType(BaseViewType[operate_info.params[1]])
		if view and view:IsOpen() then
			widget = view:GetTabBtn(operate_info.params[2])
		end
                                                          
	--界面关闭按钮
	elseif operate_info.type == Macros.Game.LeadOperateType.VIEW_CLOSE_BTN then
		view = GlobalViewMgr:GetViewByType(BaseViewType[operate_info.params[1]])
		if view and view:IsOpen() then
			local title_bar = view:GetTitleBar()
			widget = title_bar:GetCloseBtn()
		end

	--界面控件
	elseif operate_info.type == Macros.Game.LeadOperateType.VIEW_CTRL then
		view = GlobalViewMgr:GetViewByType(BaseViewType[operate_info.params[1]])
		if view and view:IsOpen() then
			widget = view:GetWidget(operate_info.params[2])
		end

	--子界面控件
	elseif operate_info.type == Macros.Game.LeadOperateType.TAB_VIEW_CTRL then
		view = GlobalViewMgr:GetViewByType(BaseViewType[operate_info.params[1]])
		if view and view:IsOpen() then
			local tab_view = view:GetTabView(operate_info.params[2])
			if tab_view then
				widget = tab_view:GetWidget(operate_info.params[3])
			end
		end

	--模板控件
	elseif operate_info.type == Macros.Game.LeadOperateType.TEMPLATE_CTRL then
		local template = GUITemplateMgr:GetTemplateByType(operate_info.params[1])
		if template then
			widget = template:GetWidget(operate_info.params[2])
		end

	--特殊
	elseif operate_info.type == Macros.Game.LeadOperateType.SPECIAL then
	end

	return widget
end

--创建遮罩层
function Modules.LeadModule:__CreateMaskLayer()
	self:__ClearMaskLayer()

	self.__mask_layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	self.__mask_layer:SetAnchorPoint(0.5, 0.5)
	self.__mask_layer:SetTouchEnabled(true)
	self.__mask_layer:SetTouchSwallow(true)
	GlobalScene:AddToRenderGroup(self.__mask_layer, Scene.RenderQueue.GRQ_UI_UP_II)
	PosTool.Center(self.__mask_layer)
end

--清空遮罩层
function Modules.LeadModule:__ClearMaskLayer()
	if self.__mask_layer then
		self.__mask_layer:RemoveSelf(true)
		self.__mask_layer = nil
	end
end

--创建屏蔽层
function Modules.LeadModule:__CreateShieldLayer(widget)
	self:__ClearShieldLayer()

	local widget_size = Util:GetWidgetSizeWithScale(widget)
	local widget_pos = NodeUnity.ConvertToWorldPosition(widget, COCOPoint(0, 0))
	local scale_time = 0.45
	local fadeout_time = 0.5
	local fadeout_time2 = 0.1

	--效果
	--遮罩
	local stencil_node = Node.Create()
	stencil_node:SetContentSize(Macros.Global.DesignSize.width,Macros.Global.DesignSize.height)
	stencil_node:SetAnchorPoint(0.5, 0.5)

	--裁剪
	self.__clipping_node = ClippingNodeEx.Node()
	self.__clipping_node:SetInverted(true)
	self.__clipping_node:SetAlphaTestValue(0)
	self.__clipping_node:SetStencilMask(stencil_node)
	GlobalScene:AddToRenderGroup(self.__clipping_node, Scene.RenderQueue.GRQ_UI_UP_II)	
	PosTool.Center(self.__clipping_node)

	--带色底
	self.__color_shield_layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	Util:SetLayoutBGColor(self.__color_shield_layer, Macros.Color.lilyBlack, 150)
	self.__color_shield_layer:SetAnchorPoint(0.5,0.5)				
	self.__clipping_node:AddChild(self.__color_shield_layer)
	PosTool.Center(self.__color_shield_layer)

	self.__show_layer_list = self.__show_layer_list or {}
	table.insert(self.__show_layer_list, {org_opacity = 255, do_fadeout = true, node = self.__color_shield_layer})

	local function show_effect()
		if self.__show_layer_list then
			for i, layer_info in ipairs(self.__show_layer_list) do
				CPPActionManager.StopAllActions(layer_info.node)
				layer_info.node:SetVisible(true)
				if layer_info.org_opacity then
					layer_info.node:SetOpacity(layer_info.org_opacity)
				end

				--缩放到指引控件
				if layer_info.do_scale then
					if layer_info.org_scale then
						layer_info.node:SetScaleFactor(layer_info.org_scale)
					end

					CPPActionManager.ScaleTo(layer_info.node, scale_time, layer_info.tgt_scale, i == 2 and
						GlobalCallbackMgr:AddCallBackFunc(function()
							--圆形遮罩缩放完成回调
							for si, slayer_info in ipairs(self.__show_layer_list) do
								if slayer_info.tgt_opacity then
									slayer_info.node:SetOpacity(slayer_info.tgt_opacity)
								end

								--淡出color_shield_layer和羽化效果图
								if slayer_info.do_fadeout then
									if slayer_info.multi_fadeout_time then
										ActionManager.GetInstance():RunFadeTo(
											slayer_info.node, 
											slayer_info.multi_fadeout_opacity, 
											fadeout_time - slayer_info.multi_fadeout_time, function()
											ActionManager.GetInstance():RunFadeOut(slayer_info.node, slayer_info.multi_fadeout_time, 
												function()
													slayer_info.node:SetVisible(false)
												end)
										end)
									else
										ActionManager.GetInstance():RunFadeOut(slayer_info.node, fadeout_time2,
											function()
												slayer_info.node:SetVisible(false)
											end)
									end
								end
							end
						end) or nil)				
				end
			end
		end
	end

	--创建屏蔽层(1上 2下 3左 4右)
	local function click_shield_layer()
		local section_info = self:GetSectionInfo()
		if section_info and section_info.is_force == Macros.Global.TRUE then
			--播放强制指引特效
			local effect = GlobalEffectMgr:GetEffect(self.__arrow_effect_id)
			if effect and effect:IsVisible() then	
				show_effect()
			end
		else
			self.__click_shield_layer_count = self.__click_shield_layer_count + 1
			if self.__click_shield_layer_count >= 2 then
				--非强制指引，点击屏蔽层达到2次，中断
				self:Stop()
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("lead_weak_tips"))
			end
		end
	end

	local function create_shield_layer(type)		
		local layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height, click_shield_layer)
		layer:SetTouchEnabled(true)
		layer:SetTouchSwallow(true)
		GlobalScene:AddToRenderGroup(layer, Scene.RenderQueue.GRQ_UI_UP_II)	
		if type == 1 then
			layer:SetAnchorPoint(0.5, 1)
			layer:SetContentHeight(Macros.Global.DesignSize.height - (widget_pos.y + widget_size.height*0.95))
			PosTool.CenterTop(layer)
		elseif type == 2 then
			layer:SetAnchorPoint(0.5,0)
			layer:SetContentHeight(widget_pos.y+widget_size.height*0.05)
			PosTool.CenterBottom(layer)
		elseif type == 3 then
			layer:SetAnchorPoint(0,0.5)
			layer:SetContentWidth(widget_pos.x+widget_size.width*0.05)
			layer:SetPosition(0,widget_pos.y+widget_size.height*0.5)		
		elseif type == 4 then
			layer:SetAnchorPoint(1,0.5)	
			layer:SetContentWidth(Macros.Global.DesignSize.width - (widget_pos.x + widget_size.width*0.95))
			PosTool.RightBottom(layer,0,widget_pos.y+widget_size.height*0.5)			
		end

		self.__shield_layer_list = self.__shield_layer_list or {}
		table.insert(self.__shield_layer_list, layer)
	end

	local function create_circle_show_layer()
		--创建圆形遮罩
		local pos_x = widget_pos.x+widget_size.width/2
		local pos_y = widget_pos.y+widget_size.height/2
		local f_p = math.max(pos_x,pos_y)
		local s_p = math.max(Macros.Global.DesignSize.width - pos_x,Macros.Global.DesignSize.height - pos_y)
		local radius = math.max(f_p,s_p)
		local real_radius = math.min(widget_size.width/2,widget_size.height/2)
		local circle_node = CPPGraphicLabel.Node()		
		circle_node:DrawSolidCircle(0, 0, radius, 44, 44, 44, 0xff)
		circle_node:SetPosition(pos_x,pos_y)
		stencil_node:AddChild(circle_node)

		table.insert(self.__show_layer_list, {org_scale = 1, tgt_scale = real_radius / radius, do_scale = true, node = circle_node})

		--创建羽化效果贴图
		local feather_sprite = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_bg2"))
	    feather_sprite:SetAnchorPoint(0.5,0.5)
		feather_sprite:SetPosition(pos_x, pos_y)
		feather_sprite:SetFeather(true)
		feather_sprite:SetFeatherType(1)
		feather_sprite:SetFeatherFocus(0.5, 0.5)
		feather_sprite:SetFeatherFocusFactor(1.5)
		feather_sprite:SetFeatherAttenuationThreshold(0.9)
		feather_sprite:SetColor(0xff, 0xff, 0xff)
		GlobalScene:AddToRenderGroup(feather_sprite, Scene.RenderQueue.GRQ_UI_UP_II)	

		table.insert(self.__show_layer_list, 
			{
				org_scale = radius / 54, tgt_scale = real_radius / 54, org_opacity = 220, tgt_opacity = 240, 
				do_scale = true, do_fadeout = true, multi_fadeout_opacity = 150, multi_fadeout_time = 0.3, node = feather_sprite, manual_release = true
			})
	end

	create_circle_show_layer()
	for i = 1, 4 do
		create_shield_layer(i)
	end		
	
	show_effect()
end

--清空屏蔽层
function Modules.LeadModule:__ClearShieldLayer()
	if self.__show_layer_list then
		for _, layer_info in ipairs(self.__show_layer_list) do
			if layer_info.manual_release then
				layer_info.node:RemoveSelf(true)
			end
		end
		self.__show_layer_list = nil
	end

	if self.__shield_layer_list then
		for _,layer in ipairs(self.__shield_layer_list) do
			layer:RemoveSelf(true)
		end
		self.__shield_layer_list = nil
	end

	if self.__clipping_node then
		self.__clipping_node:RemoveSelf(true)
		self.__clipping_node = nil
	end
end

--创建操作层
function Modules.LeadModule:__CreateOperateLayer(widget)
	self:__ClearOperateLayer()

	local widget_size = Util:GetWidgetSizeWithScale(widget)
	local widget_pos = NodeUnity.ConvertToWorldPosition(widget, COCOPoint(0, 0))	

	self.__oper_layer = Util:Layout(widget_size.width*0.9, widget_size.height*0.9)
	self.__oper_layer:SetTouchEnabled(true)
	self.__oper_layer:SetTouchSwallow(false)
	self.__oper_layer:SetAnchorPoint(0, 0)
	self.__oper_layer:SetPosition(widget_pos.x+0.05*widget_size.width, widget_pos.y+0.05*widget_size.height)
	WidgetUnity.SetWidgetListener(self.__oper_layer, nil, nil, 
		function()
			--成功触碰当前步骤的操作控件
			--切换到下一个步骤
			self:NextSection()
		end,
		function()
			self.__oper_layer:SetTouchSwallow(true)			
		end)

	GlobalScene:AddToRenderGroup(self.__oper_layer, Scene.RenderQueue.GRQ_UI_UP_II)
end

--清空操作层
function Modules.LeadModule:__ClearOperateLayer()
	if self.__oper_layer then
		self.__oper_layer:RemoveSelf(true)
		self.__oper_layer = nil
	end
end

--创建屏蔽层、遮罩层、操作层
function Modules.LeadModule:__CreateAllLayers(widget)
	if not widget then
		return
	end

	--清空屏蔽层、遮罩层、操作层
	self:__ClearAllLayers()

	--创建操作层
	self:__CreateOperateLayer(widget)

	--创建屏蔽层
	self:__CreateShieldLayer(widget)
end

--清空屏蔽层、遮罩层、操作层
function Modules.LeadModule:__ClearAllLayers()
	self.__click_shield_layer_count = 0

	self:__ClearOperateLayer()
	self:__ClearShieldLayer()
	self:__ClearMaskLayer()
end

--创建指引内容框、手指
function Modules.LeadModule:__CreateArrowAndTips(widget)
	if not widget then
		return
	end

	local section_info = self:GetSectionInfo()
	if not section_info then
		return
	end

	--清空指引内容框、手指
	self:__ClearArrowAndTips()

	local widget_size = Util:GetWidgetSizeWithScale(widget)
	local widget_pos = NodeUnity.ConvertToWorldPosition(widget, COCOPoint(widget_size.width/2, widget_size.height/2))

	--根据方向计算起始坐标(1上 2下 3左 4右)
	local function calc_birth_pos_by_dir(dir, is_arrow)
		local offset_x = 0
		local offset_y = 0
		local tgt_offset_x = 0
		local tgt_offset_y = 0

		if dir == 1 then
			offset_y = 200
		elseif dir == 2 then
			offset_y = -200
		end

		if dir == 3 then
			offset_x = -200
		elseif dir == 4 then
			offset_x = 200
		end

		if not is_arrow and section_info.tips_tgt_pos then
			tgt_offset_x = section_info.tips_tgt_pos[1] or 0
			tgt_offset_y = section_info.tips_tgt_pos[2] or 0
		end

		return widget_pos.x + tgt_offset_x + offset_x, widget_pos.y + tgt_offset_y + offset_y, widget_pos.x + tgt_offset_x, widget_pos.y + tgt_offset_y
	end

	--创建指引内容框
	if section_info.tips then
		local birth_pos_x, birth_pos_y, tgt_pos_x, tgt_pos_y = calc_birth_pos_by_dir(section_info.tips_birth_dir)

		--指引内容容器
		self.__panel_tips = Util:Layout()
		self.__panel_tips:SetAnchorPoint(0.5, 0.5)
		self.__panel_tips:SetPosition(birth_pos_x, birth_pos_y)
		GlobalScene:AddToRenderGroup(self.__panel_tips, Scene.RenderQueue.GRQ_UI_UP_II)

		--指引背景
		self.__bg_tips = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "lead_tips_bg"))
		self.__bg_tips:SetAnchorPoint(0.5, 0.5)
		self.__bg_tips:SetContentSize(310, 142)
		self.__panel_tips:AddChild(self.__bg_tips)
		self.__panel_tips:SetContentSize(self.__bg_tips:GetContentWidth(), self.__bg_tips:GetContentHeight())
		PosTool.Center(self.__bg_tips)

		local is_filp = (section_info.tips_tgt_pos and section_info.tips_tgt_pos[3] == Macros.Global.TRUE) and true or false
		--模型
		self.__spirit_model = LogicGUI.Model.New({info_id = 50001,
							parent = self.__panel_tips,model_type = LogicGUI.ModelType.SPRITE,dir = is_filp and MathTool.HORZ_LEFT or MathTool.HORZ_RIGHT})	
		self.__spirit_model:SetScaleFactor(2.5)
		
		if is_filp then
			self.__bg_tips:SetFlipX(true)		
			self.__spirit_model:SetPosition(380,0)
		else
			self.__spirit_model:SetPosition(-80,0)
		end

		--指引文本
		local lab_width = 234
		local lab_str = GUI.RichText.GetColorWord(section_info.tips, Macros.Color.lead_story_def_hex)
		local lab_tips = Util:RichText(lab_str, Macros.TypefaceSize.largish)
		if lab_tips:GetContentWidth() > lab_width then
			--TODO：富文本支持文本水平对齐
			lab_tips:SetTextWidth(lab_width)
			lab_tips:SetText("")
			lab_tips:SetText(lab_str)
		end
		lab_tips:SetAnchorPoint(0.5, 0.5)
		self.__panel_tips:AddChild(lab_tips)
		PosTool.Center(lab_tips)

		--入场动作
		CPPActionManager.JumpTo(self.__panel_tips, 0.5, tgt_pos_x, tgt_pos_y, 0, 1)
	end

	--创建指引手指
	birth_pos_x, birth_pos_y, tgt_pos_x, tgt_pos_y = calc_birth_pos_by_dir(section_info.arrow_birth_dir, true)
	local arrow_effect
	self.__arrow_effect_id, arrow_effect = GlobalEffectMgr:CreateScenePosEffect({res_name = "UI_zhiyinshouzhi", loop = true, pos_x = tgt_pos_x, pos_y = tgt_pos_y}, Scene.RenderQueue.GRQ_UI_UP_II)
end

--清空指引框内容、手指
function Modules.LeadModule:__ClearArrowAndTips()
	if self.__spirit_model then
		self.__spirit_model:DeleteMe()
		self.__spirit_model = nil
	end

	if self.__panel_tips then
		self.__panel_tips:RemoveSelf(true)
		self.__panel_tips = nil
	end	

	if self.__arrow_effect_id then
		GlobalEffectMgr:DelEffect(self.__arrow_effect_id)
		self.__arrow_effect_id = nil
	end
end

--隐藏指引框内容、手指
function Modules.LeadModule:HideArrowAndTips()
	if self.__spirit_model then
		self.__spirit_model:SetVisible(false)
	end

	if self.__panel_tips then
		self.__panel_tips:SetVisible(false)
	end	

	if self.__arrow_effect_id then
		GlobalEffectMgr:SetEffectVisible(self.__arrow_effect_id, false)
	end
end

--设置手指特效显隐
function Modules.LeadModule:SetArrowEffectVisible(is_visible)
	if self.__arrow_effect_id then
		GlobalEffectMgr:SetEffectVisible(self.__arrow_effect_id, is_visible)
	end
end

--指引步骤操作
function Modules.LeadModule:__DoSectionOperate(widget, oper_type)
	local section_info = self:GetSectionInfo()
	if not section_info or section_info.is_triggered then
		--当前步骤无效或当前步骤已触发
		return
	end	

	if self.__todo_next_section_time and self.__todo_next_section_time > Macros.Global.NowTime then
		self.__todo_next_section_widget = widget
		--延时触发
		if section_info.is_sp_handle then
			--特殊处理
			widget = widget or self:__GetOperateWidget(oper_type)
			if not widget then
				return
			end
			--暂停
			self:__Pause()

			--处理操作控件
			self:__HandleOperWidget(widget)	

			--指引框、指引手指
			self:__CreateArrowAndTips(widget)
			self:SetArrowEffectVisible(false)
		end
		return
	end

	widget = widget or self:__GetOperateWidget(oper_type)
	if not widget then
		return
	end

	LogTool.LogInfo("[Modules.LeadModule:__DoSectionOperate] lead_id:%d section:%d", self.__cur_lead_info.index, self.__cur_section_index)

	self.__todo_next_section_timeout = nil

	--已触发标记
	section_info.is_triggered = true

	--暂停
	self:__Pause()

	--处理操作控件
	self:__HandleOperWidget(widget)	

	--创建屏蔽层、遮罩层
	self:__CreateAllLayers(widget)

	--创建指引框、指引手指
	if not section_info.is_sp_handle then
		self:__CreateArrowAndTips(widget)
	end
end

--下一个步骤
function Modules.LeadModule:__NextSection(ignore_delay_time, update_local)
	--更新指引进度标记
	self:SetProgressTag(self.__cur_lead_info.index, self.__cur_section_index, update_local)

	--更新步骤索引	
	self.__cur_section_index = self.__cur_section_index + 1

	--跳过步骤
	if self.__skip_section_info and
		self.__skip_section_info.min <= self.__cur_section_index and self.__cur_section_index <= self.__skip_section_info.max then
		self:__NextSection(ignore_delay_time)
		return
	end

	--恢复性跳转
	if self.__recovery_open_info and self.__recovery_open_info.section_index == self.__cur_section_index then
		GlobalModulesMgr:OpenSystemModule(self.__recovery_open_info.sys_id, self.__recovery_open_info.sys_params)		
	end

	--结束指引
	if self.__cur_section_index > #self.__cur_lead_info.section_list then
		self:Stop()
		return
	end

	self:__Resume()
	self:__ClearAllLayers()
	self:__ClearArrowAndTips()

	--清空关键点信息
	self.__key_point_req_info = nil	

	local section_info = self:GetSectionInfo()
	if not section_info then
		LogTool.LogError("[Modules.LeadModule:__NextSection] invalid section_info lead_id:%d section:%d", self.__cur_lead_info.index, self.__cur_section_index)
		return		
	end	

	LogTool.LogInfo("[Modules.LeadModule:__NextSection] lead_id:%d section:%d", self.__cur_lead_info.index, self.__cur_section_index)

	--若下一个步骤为强制指引，则在该步骤的指引效果出来前，开启全屏遮罩，避免误点
	if section_info.is_force == Macros.Global.TRUE then
		self:__CreateMaskLayer()
	end

	--延时__DoSectionOperate
	self.__todo_next_section_time = Macros.Global.NowTime + (not ignore_delay_time and section_info.delay_time or 0)

	--指引步骤超时
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	local max_necessary_lead_id = table.maxn(config_nessary_leads)
	local tmp_tag = self:GetOperateInfo() ~= Macros.Game.LeadOperateSpecialID.BATTLE_RESULT_CONTINUE
	if tmp_tag then
		if self.__cur_lead_info and self.__cur_lead_info.index <= max_necessary_lead_id then
			self.__todo_next_section_timeout = self.__todo_next_section_time + SectionTimeout
		else
			self.__todo_next_section_timeout = self.__todo_next_section_time + SectionTimeout
		end
	else
		self.__todo_next_section_timeout = nil
	end
end

--检测特殊操作
local function get_operate_params(operate_info)
	return select(2, unpack(operate_info))
end
function Modules.LeadModule:__CheckSectionOperateSP(sp_oper_id, widget, ...)
	local section_info = self:GetSectionInfo()
	if not section_info then
		return
	end	

	if SpOperIDMap[sp_oper_id] then
		local need_id = get_operate_params(section_info.operate_params)
		local id = select(1, ...)
		if need_id ~= id then
			return
		end		
	end

	self:__DoSectionOperate(widget)

	return true
end

--战斗暂停
function Modules.LeadModule:__Pause()	
	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPauseWhenLeadOrStory") then
		self.__is_pause = true
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "PauseEx")
	end	
end

--战斗恢复
function Modules.LeadModule:__Resume()
	if not self.__is_pause then
		return
	end
	self.__is_pause = false

	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ResumeEx")
end

--处理操作控件
function Modules.LeadModule:__HandleOperWidget(widget)
	if self.__oper_widget then
		return
	end

	if Util:IsWidgetOrInheritWidget(widget) then
		self.__oper_widget_is_propagate_touch_events = widget:IsPropagateTouchEvents()
		widget:SetPropagateTouchEvents(false)
	end

	self.__oper_widget = widget	
	CPPNodeUnity.Retain(self.__oper_widget)
end

--恢复操作控件
function Modules.LeadModule:__RecoveryOperWidget(for_delete)
	if not self.__oper_widget then
		return
	end

	if not for_delete then
		if Util:IsWidgetOrInheritWidget(self.__oper_widget) and self.__oper_widget_is_propagate_touch_events then
			self.__oper_widget:SetPropagateTouchEvents(self.__oper_widget_is_propagate_touch_events)
			self.__oper_widget_is_propagate_touch_events = nil
		end
	end
	CPPNodeUnity.Release(self.__oper_widget)
	self.__oper_widget = nil
end

--过滤界面/模板打开效果处理中的情况
function Modules.LeadModule:__FilterViewOrTemplateOpenEffect(callback)
	if GlobalViewMgr:IsInViewOpenEffect() or GUITemplateMgr:IsInTemplateOpenEffect() then
		self.__event_callback_cache_list = self.__event_callback_cache_list or {}
		table.insert(self.__event_callback_cache_list, callback)
		return true
	end
end

function Modules.LeadModule:__CompareCondition(arg_list, ret_arg_list, condition, i)
	local condition_type = condition[1]	
	if condition_type == Macros.Game.LeadConditionType.ANGER_ACHIEVE_FIXED then
		--特殊处理：怒气达到X（4|关卡ID|固定值|索引）
		if i == 3 then
			--固定值检测
			return condition[i] <= arg_list[i - 1]

		elseif i == 4 then
			--索引检测
			local fixed_index = condition[i]
			if self.__common_anger_achieve_fixed_record[fixed_index] then
				--只触发一次
				return false
			end
			--按顺序触发
			self.__common_anger_achieve_fixed_index = self.__common_anger_achieve_fixed_index or 0
			if self.__common_anger_achieve_fixed_index + 1 == fixed_index then
				table.insert(ret_arg_list, fixed_index)
				return true
			end
		end
	
	elseif condition_type == Macros.Game.LeadConditionType.ANGER_ACHIEVE_PERCENT then
		--特殊处理：怒气达到X（13|关卡ID|万分比）
		if i == 3 then
			--万分比检测
			return MathTool.TransProbCoeff2Decimal(condition[i]) <= arg_list[i - 1]

		elseif i == 4 then
			--索引检测
			local fixed_index = condition[i]
			if self.__common_anger_achieve_percent_record[fixed_index] then
				--只触发一次
				return false
			end

			--按顺序触发
			self.__common_anger_achieve_percent_index = self.__common_anger_achieve_percent_index or 0
			if self.__common_anger_achieve_percent_index + 1 == fixed_index then
				table.insert(ret_arg_list, fixed_index)
				return true
			end
		end
	elseif condition_type == Macros.Game.LeadConditionType.CHASM_PASSED_FIRST_TIME then
		--深渊杀敌数
		return condition[i] <= arg_list[i - 1]
	end 

	return condition[i] == arg_list[i - 1]
end

--设置指引相关记录(本地)
function Modules.LeadModule:__SetLocalProgressTag(lead_id, section_index)
	self.__local_progress_tag_record[lead_id] = section_index
	self.__local_progress_tag_record_dirty = true
end

--获取指引相关记录(本地)
function Modules.LeadModule:__GetLocalProgressTag(lead_id)
	return self.__local_progress_tag_record[lead_id] or -1
end

--读取指引相关记录(本地)
function Modules.LeadModule:__ReadLocalProgressTagRecord()
	self.__local_progress_tag_record_update_time = 0	
	self.__local_progress_tag_record = {}

	local cookies_str = Cookies.Get(Cookies.LevelType.ROLE, Macros.Setting.LeadProgressTag)
	if cookies_str ~= "" then
		local progress_tag_record = CPPGameLib.SplitEx(cookies_str, "#")
		local lead_id, section_index
		for _, var in pairs(progress_tag_record) do
			lead_id = math.floor(var / 10000)
			section_index = var % 10000
			self.__local_progress_tag_record[lead_id] = section_index
		end
	end
end

--保存指引相关记录(本地)
function Modules.LeadModule:__SaveLocalProgressTagRecord(now_time)
	if not self.__local_progress_tag_record_dirty or self.__local_progress_tag_record_update_time + 1 > now_time then
		return
	end

	self.__local_progress_tag_record_dirty = false
	self.__local_progress_tag_record_update_time = now_time

	local cookies_str, var
	for lead_id, section_index in pairs(self.__local_progress_tag_record) do
		var = lead_id * 10000 + section_index
		if not cookies_str then
			cookies_str = tostring(var)
		else				
			cookies_str = string.format("%s#%d", cookies_str, var)
		end
	end
	Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.LeadProgressTag, cookies_str)
end

--同步指引相关记录(本地->服务端)
function Modules.LeadModule:__SyncLocalProgressTagRecord()
	local local_prog_tag, server_prog_tag
	local config_nessary_leads = CPPGameLib.GetConfig("NecessaryLead", nil, nil, true)
	for lead_id, _ in pairs(config_nessary_leads) do
		local_prog_tag = self:__GetLocalProgressTag(lead_id)
		server_prog_tag = CallModuleFunc(ModuleType.ROLE, "GetProgressTag", lead_id, -1)
		if local_prog_tag > server_prog_tag then
			CallModuleFunc(ModuleType.ROLE, "SetProgressTag", lead_id, local_prog_tag)
		end
	end	
end

