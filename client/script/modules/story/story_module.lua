--[[
%% @module: 剧情模块
%% @author: swordman sue
%% @created: 2017-01-04
%% @description: 负责剧情对话相关
--]]

Modules = Modules or {}

Modules.StoryModule = Modules.StoryModule or BaseClass(BaseModule)

--剧情结束标签(此值不能修改)
local StoryFinishedTag = 88

function Modules.StoryModule:__init()
	self.__can_update = true
	if not StartConfig.SkipStory then
		StartConfig.SkipStory = CPPSdkTools.IsInExamine()
	end
end

function Modules.StoryModule:__delete()
	self:Stop(true, true, true)
end

function Modules.StoryModule:Update(now_time, elapse_time)
	--延时开始剧情
	if self.__tostart_time and self.__tostart_time <= Macros.Global.NowTime then
		self.__tostart_time = nil
		self:__Start()
	end

	--X秒不操作，自动切换到下一步
	if self.__auto_skip_time and self.__auto_skip_time <= Macros.Global.NowTime then
		self:NextSection()
	end

	--怒气变化锁
	if self.__lock_common_anger_change and not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPause") then
		self.__lock_common_anger_elapse_time = self.__lock_common_anger_elapse_time + elapse_time
		if self.__lock_common_anger_elapse_time >= 3 then
			self.__lock_common_anger_change = false
		end
	end
end

--是否剧情中
function Modules.StoryModule:IsInStory(story_id)
	if story_id then
		return self.__cur_story_info and self.__cur_story_info.index == story_id
	end
	return self.__cur_story_info ~= nil
end

--开始剧情
function Modules.StoryModule:Start(story_id, section_index, end_func)
	section_index = section_index or 1

	if StartConfig.SkipStory then
		return
	end

	if self.__cur_story_info then
		return
	end	

	local config_story = CPPGameLib.GetConfig("Story", story_id)
	if not config_story then
		return
	end

	LogTool.LogInfo("[Modules.StoryModule:Start] story_id:%d section:%d", story_id, section_index)

	--是否在线推送奖励
	if config_story.section_list[1].is_online_push then
		GlobalEventSystem:FireNextFrame(Macros.Event.MaincityModule.ONLINE_REWARD_PUSH,true,config_story.section_list[1].online_push_params)
		self.__is_online_push_mark = true
	end

	--是否为阵容介绍特殊处理
	if story_id == 44 then
		GlobalEventSystem:FireNextFrame(Macros.Event.StoryModule.RECOMMEND_LINEUP,true)
		self.__is_recommendlineup_mark = true
	end

	--更新剧情进度标记
	self:SetProgressTag(story_id, 0)

	self.__cur_story_info = config_story
	self.__cur_section_index = section_index - 1
	self.__cur_story_end_func = end_func

	if self.__cur_story_info.delay_time then
		--延时触发
		self.__tostart_time = Macros.Global.NowTime + self.__cur_story_info.delay_time
		self:__CreateMaskLayer()
	else
		--立即触发
		self:__Start()
	end
end

--结束剧情
function Modules.StoryModule:Stop(ignore_end_lead_info, ignore_end_story_info, for_delete)
	if not self.__cur_story_info then
		return
	end
	
	LogTool.LogInfo("[Modules.StoryModule:Stop] story_id:%d", self.__cur_story_info.index)

	--更新剧情进度标记
	if not for_delete then
		self:SetProgressTag(self.__cur_story_info.index, StoryFinishedTag)
	end

	self:__ClearMaskLayer()

	--关闭界面
	local view = self:GetView()
	if view and view:IsOpen() then
		view:Stop(function()
			--剧情结束后触发的指引信息
			local end_lead_info = not ignore_end_lead_info and self.__cur_story_info.end_lead_info or nil
			--剧情结束后触发的剧情信息
			local end_story_info = not ignore_end_story_info and self.__cur_story_info.end_story_info or nil

			self.__cur_story_info = nil
			self.__cur_section_index = nil
			self.__auto_skip_time = nil
			self.__freezon_end_time = nil

			--剧情结束回调
			if self.__cur_story_end_func then
				self.__cur_story_end_func()
				self.__cur_story_end_func = nil
				if self.__is_online_push_mark then
					GlobalEventSystem:FireNextFrame(Macros.Event.MaincityModule.ONLINE_REWARD_PUSH)
				end
				if self.__is_recommendlineup_mark then
					GlobalEventSystem:FireNextFrame(Macros.Event.StoryModule.RECOMMEND_LINEUP)
				end				
			else
				if self.__is_online_push_mark then
					GlobalEventSystem:FireNextFrame(Macros.Event.MaincityModule.ONLINE_REWARD_PUSH)
				end
				if self.__is_recommendlineup_mark then
					GlobalEventSystem:FireNextFrame(Macros.Event.StoryModule.RECOMMEND_LINEUP)
				end	
			end	
			self.__is_online_push_mark = false
			self.__is_recommendlineup_mark = false
			--战斗恢复
			self:__Resume()

			--剧情结束后触发的指引
			if end_lead_info then
				CallModuleFunc(ModuleType.LEAD, "Start", end_lead_info[1], end_lead_info[2])
			end
			--剧情结束后触发的剧情
			if end_story_info then
				CallModuleFunc(ModuleType.STORY, "Start", end_story_info[1], end_story_info[2])
			end
		end)
	end	
end

--[[
触发条件参考Macros.Game.LeadConditionType
]]
--检测剧情开启
function Modules.StoryModule:CheckStart(condition_type, ...)
	if StartConfig.SkipStory then
		return
	end

	if self.__cur_story_info then
		return
	end	

	if self.__just_update_progress_tag then
		return
	end

	if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
		return
	end

	local arg_list = {...}
	local ret_arg_list = {}

	local config_storys = CPPGameLib.GetConfig("Story", nil, nil, true)
	for story_id, story_info in pairs(config_storys) do
		if story_info.condition and story_info.condition[1] == condition_type then
			local is_triggered = true
			for i = 2, #story_info.condition do
				if not self:__CompareCondition(arg_list, ret_arg_list, story_info.condition, i) then
					is_triggered = false
					break
				end
			end
			if is_triggered then
				--触发剧情
				self:Start(story_id, 1, arg_list[#story_info.condition])
				break
			end
		end
	end

	if self.__cur_story_info then
		return true, ret_arg_list
	end
end

function Modules.StoryModule:Dump()
	Modules.LeadModule.Dump(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--事件回调
--------------------------------------------------------------------------------------------------------------------------------
--战役开始
function Modules.StoryModule:HandleBattleStart(gamelevel_id, battle_index)
	Modules.LeadModule.HandleBattleStart(self, gamelevel_id, battle_index)
end

--击杀怪物
function Modules.StoryModule:HandleMonsterDead(obj_id)
	Modules.LeadModule.HandleMonsterDead(self, obj_id)
end

function Modules.StoryModule:HandleAllMonsterDead(gamelevel_id, battle_index, end_func)
	return Modules.LeadModule.HandleAllMonsterDead(self, gamelevel_id, battle_index, end_func)
end

--怒气变化
function Modules.StoryModule:HandleCommonAngerChange(old_anger, new_anger)
	Modules.LeadModule.HandleCommonAngerChange(self, old_anger, new_anger)
end

--角色名改变
function Modules.StoryModule:HandleRoleNameChanged()
	Modules.LeadModule.HandleRoleNameChanged(self)
end

--通关一个副本
function Modules.StoryModule:HandlePassGameLevel(gamelevel_id)
	Modules.LeadModule.HandlePassGameLevel(self, gamelevel_id, true)
end

--首次通关爬塔副本
function Modules.StoryModule:HandlePassTowerGameLevelFirstTime()
	Modules.LeadModule.HandlePassTowerGameLevelFirstTime(self)
end

--某个系统开启
function Modules.StoryModule:HandleSystemOpened(open_list)
	Modules.LeadModule.HandleSystemOpened(self, open_list)	
end

--英雄阵位开启
function Modules.StoryModule:HandleHeroLineupPosOpened(lineup_index)
	Modules.LeadModule.HandleHeroLineupPosOpened(self, lineup_index)
end

--援军阵位开启
function Modules.StoryModule:HandleReinforcementLineupPosOpened(lineup_index)	
	Modules.LeadModule.HandleReinforcementLineupPosOpened(self, lineup_index)		
end

--英雄降临弹出
function Modules.StoryModule:HandleHeroAdventViewOpened(chapter_index)
	Modules.LeadModule.HandleHeroAdventViewOpened(self, chapter_index)
end

--首次深渊失败并且杀敌数量大于某个数量
function Modules.StoryModule:HandleChasmFirstChallengeLose(keep_kill_num)
	Modules.LeadModule.HandleChasmFirstChallengeLose(self, keep_kill_num)
end

--首次战斗失败
function Modules.StoryModule:HandleBattleEnd(gamelevel_id, is_win)
	Modules.LeadModule.HandleBattleEnd(self, gamelevel_id, is_win)
end


--下一个步骤
function Modules.StoryModule:NextSection()
	if not self.__cur_story_info then
		return
	end

	if self.__freezon_end_time and self.__freezon_end_time > Macros.Global.NowTime then
		--子步骤禁止操作的时间
		return
	end

	--更新剧情进度标记
	self:SetProgressTag(self.__cur_story_info.index, self.__cur_section_index)

	--更新步骤索引
	self.__cur_section_index = self.__cur_section_index + 1

	--结束剧情
	if self.__cur_section_index > #self.__cur_story_info.section_list then
		self:Stop()
		return
	end

	local section_info = self:GetSectionInfo()
	if not section_info then
		LogTool.LogError("[Modules.StoryModule:NextSection] invalid section_info story_id:%d section:%d", self.__cur_story_info.index, self.__cur_section_index)
		return		
	end	

	LogTool.LogInfo("[Modules.StoryModule:NextSection] story_id:%d section:%d", self.__cur_story_info.index, self.__cur_section_index)

	--X秒不操作，自动切换到下一步
	self.__auto_skip_time = section_info.skip_second and (Macros.Global.NowTime + section_info.skip_second) or nil
	self.__freezon_end_time = section_info.freezon_time and (Macros.Global.NowTime + section_info.freezon_time) or nil

	--剧情步骤
	local view = self:GetView()
	if view and view:IsOpen() then
		view:DoSectionOperate()
	end
end

--获取剧情信息
function Modules.StoryModule:GetStoryInfo()
	return self.__cur_story_info
end

--获取字幕数量
function Modules.StoryModule:GetStoryCaptionsNum()
	local captions_num = 0
	for i,story_info in ipairs(self.__cur_story_info.section_list) do
		if story_info.auto_skip_second then
			captions_num = captions_num + 1
		end
	end
	return captions_num
end

--设置步骤索引
function Modules.StoryModule:SetCurSectionIndex(cur_section_index)
	self.__cur_section_index = cur_section_index
end


--获取步骤索引
function Modules.StoryModule:GetSectionIndex()
	return self.__cur_section_index
end

--获取步骤信息
function Modules.StoryModule:GetSectionInfo(section_index)
	if not self.__cur_story_info then
		return
	end

	return self.__cur_story_info.section_list[section_index or self.__cur_section_index]
end

--设置进度标记
function Modules.StoryModule:SetProgressTag(story_id, section_index)
	if story_id == ConfigAdapter.Common.GetStoryIDForLaunchExamine() then
		return
	end

	if story_id == Macros.Game.LeadProgressTagType.LEAD_HERO_ADVENT_FIRST_TIME then
		CallModuleFunc(ModuleType.ROLE, "SetProgressTag", story_id, section_index)
	end

	--记录剧情进度
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	if not CallModuleFunc(ModuleType.LEAD, "IsMaxNecessaryLeadFinished") and (role_level and role_level < 4) then
		CallModuleFunc(ModuleType.ROLE, "UpdateStoryTagReq", story_id, section_index)
	end
end

--获取进度标记
function Modules.StoryModule:GetProgressTag(story_id)
	return CallModuleFunc(ModuleType.ROLE, "GetProgressTag", story_id, -1)
end

--获取禁止操作的结束时间
function Modules.StoryModule:GetFreezonEndTime()
	return self.__freezon_end_time
end

function Modules.StoryModule:GetOnlineMark()
	return self.__is_online_push_mark
end
--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.StoryModule:__Start()
	self:__ClearMaskLayer()

	--打开界面
	self:OpenView(BaseViewType.STORY, CPPSdkTools.IsInExamine() and Scene.RenderQueue.GRQ_UI_MAX_II or nil)

	--战斗暂停
	self:__Pause()
end

--战斗暂停
function Modules.StoryModule:__Pause()
	Modules.LeadModule.__Pause(self)
end

--战斗恢复
function Modules.StoryModule:__Resume()
	Modules.LeadModule.__Resume(self)
end

--创建屏蔽层
function Modules.StoryModule:__CreateMaskLayer()
	if self.__mask_layer then
		return
	end
	self.__mask_layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height, click_mask_layer)
	self.__mask_layer:SetAnchorPoint(0.5, 0.5)
	self.__mask_layer:SetTouchEnabled(true)
	self.__mask_layer:SetTouchSwallow(true)
	GlobalScene:AddToRenderGroup(self.__mask_layer, Scene.RenderQueue.GRQ_UI_UP_II)
	PosTool.Center(self.__mask_layer)
end

function Modules.StoryModule:__ClearMaskLayer()
	if self.__mask_layer then
		self.__mask_layer:RemoveSelf(true)
		self.__mask_layer = nil
	end
end

function Modules.StoryModule:__CompareCondition(...)
	return Modules.LeadModule.__CompareCondition(self, ...)
end

function Modules.StoryModule:IsInShield()
	return self.__mask_layer
end