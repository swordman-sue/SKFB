
--[[
%% @module: 副本模块
%% @author: swordman sue
%% @created: 2016-08-11
--]]

Modules = Modules or {}

Modules.DungeonModule = Modules.DungeonModule or BaseClass(BaseModule)

--宝箱状态
Modules.DungeonModule.BoxStatus = 
{
	NONE = 0,
	CAN_RECEIVE = 1,
	ALREADY_RECEIVE = 2,	
}

--副本类型列表
local dungeon_type_list = {Macros.Game.FBType.MAIN_DUNGEON,Macros.Game.FBType.HERO_DUNGEON,Macros.Game.FBType.ELITE_DUNGEON}
	
function Modules.DungeonModule:__init()
	--章节数据
	self.__chapter_data_map = {}
	--额外数据
	self.__ext_data_map = {}
	--红点数据
	self.__red_dot_map = {}

	self.everyday = {}
	self.everyday.combat = false

	--各个副本当前打开章索引
	self.__open_chapter_data = {}
	--是否弹窗
	self.__is_sureframe_popup_again = true
	--是否弹添加好友弹窗
	self.__is_show_add_friend_tips = true
end

function Modules.DungeonModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理战斗结束返回相关界面
function Modules.DungeonModule:HandleBattleEndReturnRelevantView()
	self.everyday.combat = true
end

--处理返回主界面
function Modules.DungeonModule:HandleMaincityVisibleEvent()
	self.__is_on_cleaning = false

	if self.__clean_view then
		self.__clean_view:DeleteMe()
		self.__clean_view = nil
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--主线副本数据请求
function Modules.DungeonModule:FBMainDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_FB_MAIN_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--主线副本数据响应
function Modules.DungeonModule:FBMainDataResp(protocol_data)
	self.__dungeonbox_reward_level = protocol_data.take_chapter_reward_record
	self:__SetChapterList(protocol_data.chapter_list, Macros.Game.FBType.MAIN_DUNGEON)
	--统计红点
	self:__CountDungeonRedDot(Macros.Game.FBType.MAIN_DUNGEON,Macros.Game.SystemID.DUNGEON_GAMELEVEL)

	self:FireNextFrame(Macros.Event.DungeonModule.MAIN_DUNGEON_DATA_RESP)	
end

--英雄副本数据请求
function Modules.DungeonModule:FBHeroDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_HERO_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--英雄副本数据响应
function Modules.DungeonModule:FBHeroDataResp(protocol_data)
	self:__SetChapterList(protocol_data.chapter_list, Macros.Game.FBType.HERO_DUNGEON)
	self:__SetExtraData({battle_times = protocol_data.battle_times}, Macros.Game.FBType.HERO_DUNGEON)
	--统计红点
	self:__CountDungeonRedDot(Macros.Game.FBType.HERO_DUNGEON,Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL)

	self:FireNextFrame(Macros.Event.DungeonModule.HERO_DUNGEON_DATA_RESP)
end

--日常副本数据请求
function Modules.DungeonModule:FBDailyDataReq( info )
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_DAILY_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--日常副本数据响应
function Modules.DungeonModule:FBDailyDataResp(protocol_data)
	self:__SetChapterList(protocol_data.chapter_list, Macros.Game.FBType.DAILY_DUNGEON)
	self:FireNextFrame(Macros.Event.DungeonModule.DAILY_DUNGEON_DATA_RESP)
end

--精英副本数据请求
function Modules.DungeonModule:FBEliteDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_ELITE_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--精英副本数据响应
function Modules.DungeonModule:FBEliteDataResp(protocol_data)
	self:__SetChapterList(protocol_data.chapter_list, Macros.Game.FBType.ELITE_DUNGEON)
	--统计红点
	self:__CountDungeonRedDot(Macros.Game.FBType.ELITE_DUNGEON,Macros.Game.SystemID.DUNGEON_ELITE)

	self:FireNextFrame(Macros.Event.DungeonModule.ELITE_DUNGEON_DATA_RESP)

	--重置相应区域入口
	CallModuleFunc(ModuleType.MAIN_CITY, "ResetRegionEntrysBySystemID", Macros.Game.SystemID.ELITE_DUNGEON_BREAK)
end

--领取宝箱请求
function Modules.DungeonModule:FBAwardGetReq(award_type, chapter_index, index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_AWARD_GET_REQ)
	protocol.award_type = award_type
	protocol.fb_type = fb_type
	protocol.chapter_index = chapter_index
	protocol.index = index
	GlobalNetAdapter:OnSend(protocol)	
end

--领取宝箱响应
function Modules.DungeonModule:FBAwardGetResp(protocol_data)
	local chapter_info = self:GetChapterInfo(protocol_data.chapter_index, protocol_data.fb_type)

	local reward_list

	--关卡奖励
	if protocol_data.award_type == Macros.Game.FBRewardType.GAMELEVEL then
		local gamelevel_info = chapter_info.gamelevel_list[protocol_data.index]
		if gamelevel_info then
			gamelevel_info.box_award_flag = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
		end

		--获取奖励列表
		local gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(protocol_data.chapter_index, protocol_data.index, protocol_data.fb_type)
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id, false)
		if config_gamelevel then
			reward_list = ConfigAdapter.Item.GetGiftPackRewardListII(config_gamelevel.box_id_list, config_gamelevel.box_num_list) 
		end

	--星级奖励
	elseif protocol_data.award_type == Macros.Game.FBRewardType.STAR then
		chapter_info.star_award_list[protocol_data.index] = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE 

		--获取奖励列表
		local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(protocol_data.chapter_index, protocol_data.fb_type)
		if config_chapter then
			local box_id_list = config_chapter[string.format("box_%d_id_list", protocol_data.index)]
			local box_num_list = config_chapter[string.format("box_%d_num_list", protocol_data.index)]
			reward_list = ConfigAdapter.Item.GetGiftPackRewardListII(box_id_list, box_num_list)
		end		
	end

	--更新红点
	if protocol_data.fb_type == Macros.Game.FBType.MAIN_DUNGEON then
		self:__UpdateDungeonRedDot(Macros.Game.SystemID.DUNGEON_GAMELEVEL, protocol_data.chapter_index)
	
	elseif protocol_data.fb_type == Macros.Game.FBType.HERO_DUNGEON then
		self:__UpdateDungeonRedDot(Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL, protocol_data.chapter_index)	
	
	elseif protocol_data.fb_type == Macros.Game.FBType.ELITE_DUNGEON then
		self:__UpdateDungeonRedDot(Macros.Game.SystemID.DUNGEON_ELITE, protocol_data.chapter_index)	
	end
	
	--奖励提示
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )

	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.RECEIVE_BOX_REWARD)

	self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_AWARD_GET_RESP, protocol_data.award_type, protocol_data.index)
end

--副本扫荡请求
function Modules.DungeonModule:FBCleanReq(clean_times, param1, param2, fb_type)
	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		return
	end

	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	--请求扫荡的次数
	self.__clean_times_req = clean_times or self.__clean_times_req
	--是否重新扫荡
	self.__is_reset_clean_up = clean_times and true or false
	--刷新标记
	if not self.__clean_view then
		self.__is_on_cleaning = true
	end
	--请求扫荡前的角色等级
	self.__role_level_before_clean = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)

	--请求协议
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_CLEAN_REQ)
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.fb_type = fb_type
	GlobalNetAdapter:OnSend(protocol)
end

--副本扫荡反馈
function Modules.DungeonModule:FBCleanResp(protocol_data)	
	if not self.__clean_view then
		self:OpenCleanView(protocol_data)			
	else
		self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_CLEAN_RESP, protocol_data, self.__clean_times_req, self.__is_reset_clean_up)
	end
end

--副本通关一键领取
function Modules.DungeonModule:FBPassedAllReq(chapter_index,fb_type)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_ONE_KEY_TAKE_REWARD_REQ)
	protocol.chapter = chapter_index
	protocol.fb_type = fb_type
	GlobalNetAdapter:OnSend(protocol)
end

--副本通关一键领取
function Modules.DungeonModule:FBPassedAllResp(protocol_data)	
	self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_ONEKEY_AWARD_GET_RESP,true)
	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.ONE_KEY_RECEIVE_BOX_REWARD)
end

--英雄降临宝箱奖励
function Modules.DungeonModule:FBPassedBoxRewardReq(reward_level)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_MAIN_FB_CHAPTER_REWARD_REQ)
	protocol.reward_level = reward_level
	GlobalNetAdapter:OnSend(protocol)
end

--英雄降临宝箱奖励回调
function Modules.DungeonModule:FBPassedBoxRewardResp(protocol_data)	
	self.__dungeonbox_reward_level = protocol_data.reward_level
	self:FireNextFrame(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD, protocol_data)
end

--关卡评论列表请求
function Modules.DungeonModule:MapCommentListReq(map_id,is_message_req)
	self.__is_message_req = is_message_req
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MAP_COMMENT_LIST_REQ)
	protocol.map_id = map_id
	GlobalNetAdapter:OnSend(protocol)
end

--关卡评论列表响应
function Modules.DungeonModule:MapCommentListResp(protocol_data)		
	self.__tmp_comment_list = {}
	if protocol_data.comment_list then
		--根据点赞次数排序
		table.sort(protocol_data.comment_list,function (data1,data2)
			if data1.like_times ~= data2.like_times then
				return data1.like_times > data2.like_times
			else
				return data1.comment_id < data2.comment_id
			end
		end)
		for i,commend_info in ipairs(protocol_data.comment_list) do
			commend_info.like_player_list = commend_info.like_player_list:GetData()		
			commend_info.__index = i
			commend_info.map_id = protocol_data.map_id
			commend_info.clean_up_list = {}
			for i,role_info in ipairs(commend_info.like_player_list) do
				commend_info.clean_up_list[role_info.role_id] = role_info.role_id
			end
		end
		self.__tmp_comment_list = protocol_data.comment_list
	end	

	self:FireNextFrame(Macros.Event.DungeonModule.GAMELEVEL_COMMENT,self.__tmp_comment_list,self.__is_message_req)
end

--关卡评论请求
function Modules.DungeonModule:MapCommentReq(info)
	local level_limit = GlobalCustomizedLogicAdapterMgr:Call("GetLevelLimitOfMapComment")
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	if level_limit and level_limit > role_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", level_limit))		
		return
	end
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MAP_COMMENT_REQ)
	protocol.map_id = info.map_id
	protocol.text = LanguageFilter.ChatFilter(info.text)
	GlobalNetAdapter:OnSend(protocol)
end

--关卡评论响应
function Modules.DungeonModule:MapCommentResp(protocol_data)
	local info = {}
	info.map_id = protocol_data.map_id
	info.comment_id = protocol_data.comment_id
	info.time = protocol_data.time
	info.text = protocol_data.text
	info.like_player_list = {}
	info.clean_up_list = {}
	info.like_times = 0
	self:FireNextFrame(Macros.Event.DungeonModule.GAMELEVEL_COMMENT_RESP,info)	
end

--关卡评论点赞
function Modules.DungeonModule:MapCommentLikeReq(info)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_MAP_COMMENT_LIKE_REQ)
	protocol.map_id = info.map_id
	protocol.comment_id = info.comment_id
	GlobalNetAdapter:OnSend(protocol)
end

--关卡评论点赞响应
function Modules.DungeonModule:MapCommentLikeResp(protocol_data)
	self:FireNextFrame(Macros.Event.DungeonModule.GAMELEVEL_COMMENT_PARISE_RESP,protocol_data.comment_id)			
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--获取章节列表
function Modules.DungeonModule:GetChapterList(fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	return self.__chapter_data_map[fb_type]
end

--获取章节信息
function Modules.DungeonModule:GetChapterInfo(chapter_index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local chapter_info = self:__ParseChapterInfo(chapter_index, fb_type)
	return chapter_info
end

--获取当前已通关的章节数
function Modules.DungeonModule:GetPassedAllChapterNum(fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	local chapter_list = self:GetChapterList(fb_type)
	local chapter_info = self:GetChapterInfo(#chapter_list,fb_type)
	if not chapter_info then
		return 0
	end
	local passed_chapter_num = chapter_info.is_history_passed_all and #chapter_list or (#chapter_list - 1)
	return passed_chapter_num
end

--获取当前已通关的章节星数
function Modules.DungeonModule:GetChapterTotalStar()
	local chapter_list = self:GetChapterList()
	local chapter_info 
	local cur_star
	for chapter_index=1,#chapter_list do
		chapter_info = self:GetChapterInfo(chapter_index)
		cur_star = cur_star or 0
		cur_star = cur_star + chapter_info.cur_star
	end
	return cur_star
end

--获取关卡信息
function Modules.DungeonModule:GetGameLevelInfo(chapter_index, gamelevel_index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local chapter_info = self:GetChapterInfo(chapter_index, fb_type)
	return chapter_info and chapter_info.gamelevel_list[gamelevel_index] or nil
end

--获取额外信息
function Modules.DungeonModule:GetExtraInfo(fb_type)
	return self.__ext_data_map[fb_type] or {}
end

--获取当前已经领取到的章节宝箱等级
function Modules.DungeonModule:GetFBPassedBoxRewardLevel()
	return self.__dungeonbox_reward_level
end

--获取当前副本打开章数
function Modules.DungeonModule:GetOpenChapterIndex(fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	self.__open_chapter_data[fb_type] = self.__open_chapter_data[fb_type] or #self.__chapter_data_map[fb_type]
	return self.__open_chapter_data[fb_type]
end

--设置当前副本打开章数
function Modules.DungeonModule:SetOpenChapterIndex(fb_type,chapter_index)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	self.__open_chapter_data[fb_type] = chapter_index
end

--设置当前助战角色Id
function Modules.DungeonModule:SetAssistRoleID(assist_target_id,target_type,friend_name)
	self.__assist_target_id = target_type and target_type == Macros.Global.FALSE and assist_target_id or nil	
	self.__assist_target_name = friend_name
end

--设置当前助战角色Id
function Modules.DungeonModule:GetAssistRoleID()
	return self.__assist_target_id,self.__assist_target_name
end

--设置日常副本各个副本的难度进度
function Modules.DungeonModule:SetDaliyDungeonDifficultRecord()
	self.__daliy_dungeon_fb_type_record_info = self.__daliy_dungeon_fb_type_record_info or {}
	for fb_type,daliy_info in ipairs(CPPGameLib.GetConfig("DailyChapter",nil,nil,true)) do
		local chapter_info = self:GetChapterInfo(fb_type,Macros.Game.FBType.DAILY_DUNGEON)
		self.__daliy_dungeon_fb_type_record_info[fb_type] = chapter_info.__map_idx_record
	end
end

--通过日常副本类型来获取各个副本的难度进度
function Modules.DungeonModule:GetDaliyDungeonDifficultRecordByFbType(fb_type)
	return self.__daliy_dungeon_fb_type_record_info[fb_type] or 0
end

--副本通关一键领取数据更新
function Modules.DungeonModule:UpdateFBPassedAllData(chapter_index,fb_type)
	local chapter_info = self:GetChapterInfo(chapter_index,fb_type)
	local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, fb_type)
	for gamelevel_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
		--初始化关卡宝箱状态
		if gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
			gamelevel_info.box_award_flag = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
		end
	end

	--星级宝箱
	chapter_info.has_star_award = false
	if config_chapter then
		for index, tgt_star in ipairs(config_chapter.star_list) do
			--初始化星级宝箱状态
			if chapter_info.star_award_list[index] == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
				chapter_info.star_award_list[index] = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
			end
		end
	end

	--更新红点
	local system_id = fb_type == Macros.Game.FBType.ELITE_DUNGEON and 
		Macros.Game.SystemID.DUNGEON_ELITE or Macros.Game.SystemID.DUNGEON_GAMELEVEL
	self:__UpdateDungeonRedDot(system_id, chapter_index, 0)
end

--获取章节显示列表
function Modules.DungeonModule:GetChapterShowList(fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local chapter_info_list = self:GetChapterList(fb_type)
	if not chapter_info_list then
		return
	end

	local chapter_show_list = {}
	for index, _ in ipairs(chapter_info_list) do
		local index_info = {}
		index_info.index = index
		table.insert(chapter_show_list, index_info)
	end

	--插入即将开启的章节
	if fb_type == Macros.Game.FBType.MAIN_DUNGEON or
		fb_type == Macros.Game.FBType.HERO_DUNGEON or
		fb_type == Macros.Game.FBType.ELITE_DUNGEON then
		for i = 1, 2 do
			local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(i + #chapter_info_list, fb_type, false)
			if config_chapter then
				table.insert(chapter_show_list, {index = i + #chapter_info_list})
			end
		end
	end

	return chapter_show_list
end

--请求战斗前的条件检测
function Modules.DungeonModule:CheckBattleCondition(type, param1, param2, param3, param4)
	fb_type = type
	chapter_index = param1
	gamelevel_index = param2	
	local extra_data = self:GetExtraInfo(fb_type)
	local chapter_info = self:GetChapterInfo(chapter_index, fb_type)

	local gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index, fb_type)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id, false)
	if config_gamelevel then
		--检测关卡挑战次数
		if fb_type == Macros.Game.FBType.MAIN_DUNGEON or fb_type == Macros.Game.FBType.HERO_DUNGEON 
			or fb_type == Macros.Game.FBType.ELITE_DUNGEON then
			local gamelevel_info = self:GetGameLevelInfo(chapter_index, gamelevel_index, fb_type)
			if config_gamelevel.battle_times_max > 0 then
				if gamelevel_info and gamelevel_info.battle_times >= config_gamelevel.battle_times_max then 
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips3"))
					return false
				end
			end
		end

		--检测体力消耗
		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(gamelevel_id)
		local cost_stamina = cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0
		if cost_stamina > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.STAMINA) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_stamina_not_enough"))
			LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.STAMINA )
			return false
		end
	end

	--检测英雄副本挑战次数
	if fb_type == Macros.Game.FBType.HERO_DUNGEON then
		if gamelevel_index > 0 then
			--非额外关卡
			if extra_data.battle_times >= CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_TRANSCRIPT) then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips3"))				
				return false
			end
		else
			--额外关卡(只能挑战1次)
			if chapter_info.is_extra_map_pass == Macros.Global.TRUE then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_extra_gamelevel_challenged"))
				return false
			end			
		end

	--检测日常副本挑战次数
	elseif fb_type == Macros.Game.FBType.DAILY_DUNGEON then
		--未解锁
		if not chapter_info.is_unlocked then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_daily_chapter_locked"))
			return false
		end
		--未开放
		if not chapter_info.is_opened then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_daily_chapter_closed"))
			return false
		end
	elseif fb_type == Macros.Game.FBType.ELITE_DUNGEON and gamelevel_index == 0 then
		--精英外敌
		--检测体力消耗
		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(self.__elite_external_dungeon_mapid)
		local cost_stamina = cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0
		if cost_stamina > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.STAMINA) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_stamina_not_enough"))
			LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.STAMINA )
			return false
		end
	end

	--检测背包空间
    local gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index, fb_type)
    local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id, false)
    if config_gamelevel then
        local test = ""
        -- 需要的英雄背包数量
        if config_gamelevel.need_hero_bag_num then
            GlobalModulesMgr:Call(
                                    ModuleType.SUNDRY, 
                                    "PackVacancy", Macros.Game.ObjType.HERO , 
                                    config_gamelevel.need_hero_bag_num , false , 
                                    function ( sum , text )
                                        if sum == 0 then
                                            test = text
                                        end
                                    end 
                                )
        end

        -- 需要的装备背包数量
        if config_gamelevel.need_equipment_bag_num then
            GlobalModulesMgr:Call(
                                    ModuleType.SUNDRY, 
                                    "PackVacancy", Macros.Game.ObjType.EQUIP , 
                                    config_gamelevel.need_equipment_bag_num , false , 
                                    function ( sum , text )
                                        if sum == 0 then
                                            test = text
                                        end
                                    end 
                                )
        end

        -- 需要的圣器背包数量
        if config_gamelevel.need_treasure_bag_num then
            GlobalModulesMgr:Call(
                                    ModuleType.SUNDRY, 
                                    "PackVacancy", Macros.Game.ObjType.TREASURE , 
                                    config_gamelevel.need_treasure_bag_num , false , 
                                    function ( sum , text )
                                        if sum == 0 then
                                            test = text
                                        end
                                    end 
                                )
        end

        if test ~= "" then
            GlobalTipMsg.GetInstance():Show( test )
            return false
        end
    end

	return true
end

--请求挑战关卡
function Modules.DungeonModule:HandleGameLevelChallenge(chapter_index, gamelevel_index, fb_type, assist_target_id, assist_spiritid, common_skillid)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local checkBattle = CallModuleFunc(ModuleType.DUNGEON, "CheckBattleCondition", fb_type, chapter_index, gamelevel_index, assist_target_id, assist_spiritid )
	if checkBattle then
		--请求挑战关卡
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", fb_type, chapter_index, gamelevel_index, assist_target_id, assist_spiritid,common_skillid)
	end
end

--处理扫荡关卡
function Modules.DungeonModule:HandleGameLevelClean(chapter_index, gamelevel_index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local clean_times, times_by_stamina, times_by_battle_times = self:GetGameLevelCanCleanTimes(chapter_index, gamelevel_index, fb_type)
	if clean_times <= 0 then
		if times_by_battle_times <= 0 then
			-- print("挑战次数不足，TODO：重置处理")		
		elseif times_by_stamina <= 0 then
			-- print("体力不足，TODO：弹出体力丹使用界面")
			local staminaMedicine_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetStaminaMedicineID())
			if staminaMedicine_num > 0 then
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , ConfigAdapter.Common.GetStaminaMedicineID())   
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_stamina_not_enough"))
				LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.STAMINA )
			end
		end
		return false
	end
	local _gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index,fb_type)
	if _gamelevel_id then
		--判断背包是否已满
		local gamelevel_info = CPPGameLib.GetConfig("GameLevel",_gamelevel_id)
		local is_herobag_no_full,is_equipbag_no_full,is_treasurebag_no_full = true,true,true
		if gamelevel_info.need_hero_bag_num then
			CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO, gamelevel_info.need_hero_bag_num,true,function ( sum )
            	is_herobag_no_full = sum >= gamelevel_info.need_hero_bag_num
        	end)
		end	
		if gamelevel_info.need_equipment_bag_num then
			CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.EQUIP, gamelevel_info.need_equipment_bag_num,true,function ( sum )
            	is_equipbag_no_full = sum >= gamelevel_info.need_equipment_bag_num
        	end)
		end		
		if gamelevel_info.need_treasure_bag_num then
			CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.TREASURE, gamelevel_info.need_treasure_bag_num,true,function ( sum )
            	is_treasurebag_no_full = sum >= gamelevel_info.need_treasure_bag_num
        	end)
		end
		local is_no_full = is_herobag_no_full and is_equipbag_no_full and is_treasurebag_no_full or false
		if not is_no_full then
			return
		end
	else
		return false
	end
	self:FBCleanReq(clean_times, chapter_index, gamelevel_index, fb_type)
	return true
end

--处理扫荡关卡
function Modules.DungeonModule:HandleGameLevelCleanByTimes(chapter_index, gamelevel_index, clean_times, fb_type)
	self:FBCleanReq(clean_times, chapter_index, gamelevel_index, fb_type)	
end

--检测扫荡开启条件
function Modules.DungeonModule:CheckGameLevelCleanOpenCondition(chapter_index, gamelevel_index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local gamelevel_info = self:GetGameLevelInfo(chapter_index, gamelevel_index, fb_type)

	if fb_type == Macros.Game.FBType.MAIN_DUNGEON or fb_type == Macros.Game.FBType.ELITE_DUNGEON then		
		--未通关
		if gamelevel_info.star < ConfigAdapter.Common.GetMainDungeonCleanOpenCondition_Star() then
			return false
		end
		--角色等级
		local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.DUNGEON_CLEAN_UP)
		if is_open then
			return true
		end
		--VIP等级
		if CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.OPEN_MAIN_DUNGEON_CLEAN) == Macros.Global.TRUE then
			return true
		end
	end
	return false
end

--获取可扫荡次数
function Modules.DungeonModule:GetGameLevelCanCleanTimes(chapter_index, gamelevel_index, fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON

	local times = 0
	local times_by_stamina = 0
	local times_by_battle_times  = 0
	local gamelevel_info = self:GetGameLevelInfo(chapter_index, gamelevel_index, fb_type)
	local gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index, fb_type)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not gamelevel_info or not config_gamelevel then
		return times, 0, 0
	end

	if fb_type == Macros.Game.FBType.MAIN_DUNGEON or fb_type == Macros.Game.FBType.ELITE_DUNGEON then		
		--默认次数
		local def_times = ConfigAdapter.Common.GetMainDungeonCleanDefaultTimes()
		
		--根据体力计算的可扫荡次数
		times_by_stamina = MathTool.MAX_NUM
		local stamina = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.STAMINA)
		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(gamelevel_id)
		local cost_stamina = cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0		
		if cost_stamina > 0 then
			times_by_stamina = math.floor(stamina / cost_stamina)
		end

		--根据挑战次数计算的可扫荡次数
		times_by_battle_times = config_gamelevel.battle_times_max - gamelevel_info.battle_times
		times = math.min(def_times, times_by_stamina)
		times = math.min(times, times_by_battle_times)
	elseif fb_type == Macros.Game.FBType.DAILY_DUNGEON then
		--日常副本
		--跳转到当前选中副本可以挑战到的难度
    	local chapter_info = self:GetChapterInfo(chapter_index,fb_type)
		times = chapter_info.__remanent_battle_times
		times_by_battle_times = chapter_info.__remanent_battle_times
	end

	return times, times_by_stamina, times_by_battle_times
end

--打开扫荡战报
function Modules.DungeonModule:OpenCleanView(data)
	if not self.__clean_view then
		local level_change = self.__role_level_before_clean ~= CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
		self.__clean_view = Modules.TemplDungeonCleanResult.New(data, self.__clean_times_req, level_change)
	end
end

--关闭扫荡战报
function Modules.DungeonModule:CloseCleanView()
	self.__is_on_cleaning = false

	if self.__clean_view then
		self.__clean_view:DeleteMe()
		self.__clean_view = nil
	end
end

--获取扫荡的状态
function Modules.DungeonModule:IsOnCleaning()
	return self.__is_on_cleaning
end

--设置是否再次弹出确认框
function Modules.DungeonModule:IsSureFramePopUp(is_popup)
	self.__is_sureframe_popup_again = is_popup
end

--设置是否再次弹出确认框
function Modules.DungeonModule:GetIsSureFramePopUp()
	return self.__is_sureframe_popup_again
end

--获取当前玩家打到副本的最新章节和最新关卡
function Modules.DungeonModule:GetCurDungeonLatestChapterAndGameLevelByType(fb_type)
	local chapter_list = self:GetChapterList(fb_type)
	local chapter_info = self:GetChapterInfo(#chapter_list,fb_type)
	return #chapter_list,#chapter_info.gamelevel_list
end

--检测通关记录
function Modules.DungeonModule:CheckPassGameLevel()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if not battle_info then
		return
	end

	local sp_result = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetCurStatus")
	local is_lead_associated_fb = CPPGameLib.GetConfig("LeadAssociatedFB", battle_info.type, false) ~= nil
	if is_lead_associated_fb and sp_result == Modules.SplogicStatus.WIN then
		--检测主线副本、英雄副本通关
		if self:__CheckPassMainDungeonGameLevel() then
			return
		end
		
		--检测首次通关爬塔副本
		if self:__CheckPassTowerGameLevel() then
			return
		end
		
		--检测竞技场、新手副本...
		local key = Macros.Game.LeadProgressTagType.LEAD_PASS_GAMELEVEL_BEG + battle_info.gamelevel_id
		local flag = CallModuleFunc(ModuleType.ROLE, "GetProgressTag", key)
		if flag ~= Macros.Global.TRUE then
			--更新记录
			CallModuleFunc(ModuleType.ROLE, "SetProgressTag", key, Macros.Global.TRUE)

			--派发通关事件
			self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_PASS_GAME_LEVEL, battle_info.gamelevel_id)
		end
	end
end

--是否通关
function Modules.DungeonModule:IsPassGameLevel(gamelevel_id)
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info then
		--主线副本、英雄副本、精英副本、日常副本
		if battle_info.type == Macros.Game.FBType.MAIN_DUNGEON or battle_info.type == Macros.Game.FBType.HERO_DUNGEON then
			local gamelevel_info = self:GetGameLevelInfo(battle_info.chapter_index, battle_info.gamelevel_index, battle_info.type)
			return gamelevel_info and not gamelevel_info.no_passed_flag
		end

		--爬塔副本
		if battle_info.type == Macros.Game.FBType.TOWER_COMMON or battle_info.type == Macros.Game.FBType.TOWER_ELITE then
			return CallModuleFunc(ModuleType.ROLE, "GetProgressTag", Macros.Game.LeadProgressTagType.TOWER_GAMELEVEL_PASSED_FIRST_TIME) == Macros.Global.TRUE
		end
	end

	--竞技场、新手副本...
	local key = Macros.Game.LeadProgressTagType.LEAD_PASS_GAMELEVEL_BEG + gamelevel_id
	return CallModuleFunc(ModuleType.ROLE, "GetProgressTag", key) == Macros.Global.TRUE
end

--是否通关
function Modules.DungeonModule:IsPassMainDungeonGameLevel(gamelevel_id)
	local chapter_list = self:GetChapterList()
	for chapter_index, chapter_info in ipairs(chapter_list) do
		for gamelevel_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
			local id = ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index)
			if id == gamelevel_id then
				local info = self:GetGameLevelInfo(chapter_index, gamelevel_index)
				return not info.no_passed_flag
			end
		end
	end
end

--获取日常副本章节信息
function Modules.DungeonModule:GetAllDailyChapterType()	
	local t1 = {}
    for i=1 ,#CPPGameLib.GetConfig("DailyChapter",nil,nil,true) do
    	local key = true
        local chapter_info = self:GetChapterInfo( i, Macros.Game.FBType.DAILY_DUNGEON)       
        --如果等级没达到
        if not chapter_info.is_unlocked and key then
        	table.insert(t1 ,"notLevel" )
        	key = false
        end

        --如果时间没达到
        if not chapter_info.is_opened and key then
        	table.insert( t1 , "notTime" )
        	key = false
        end

        --如果已经消耗完次数
        if (chapter_info.__remanent_battle_times <= 0) and key then
        	table.insert( t1 , "has_battled" )
        	key = false
        end

        if key then
        	table.insert(t1 ,"ok")
        end
    end
    return t1
end

-- 消息说明: 副本重置请求 消息Id: 21013
function Modules.DungeonModule:FBResetReq(info)
	-- CPPGameLib.PrintTable(info, "消息说明: 副本重置请求 消息Id: 21013")
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_RESET_REQ)
    protocol.map_type = info.map_type
    protocol.param1 = info.param1
    protocol.param2 = info.param2 - 1
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明: 副本重置响应 消息Id: 21014
function Modules.DungeonModule:FBResetResp(info)
	-- CPPGameLib.PrintTable(info, "消息说明: 副本重置响应 消息Id: 21014")
	local lst = self:GetGameLevelInfo(info.param1 , info.param2+1 , info.map_type)
	lst.reset_times =  lst.reset_times + 1 
	lst.battle_times =  0
	
	self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_MAINLINE_RESET)
end

--获得重置相关信息
function Modules.DungeonModule:GetRestTimes(data,fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	local vip_type = fb_type == Macros.Game.FBType.MAIN_DUNGEON and Macros.Game.VIPType.MAINLINE_RESET 
						or Macros.Game.VIPType.ELITELINE_RESET
	--拥有的次数
	local time = CallModuleFunc(ModuleType.VIP, "GetPrivilege" ,vip_type)
    --花费
    local resetLst = fb_type == Macros.Game.FBType.MAIN_DUNGEON and ConfigAdapter.Common.GetMainDungeonCleanReset()
						or ConfigAdapter.Common.GetEliteDungeonCleanReset()

    local gold = resetLst[data.reset_times + 1] and resetLst[data.reset_times + 1] or resetLst[#resetLst]

    --可以的次数
    local may = time - data.reset_times

    return gold , may , time
end

--获得下级VIP重置效果
function Modules.DungeonModule:GetRestNewVipInfo(fb_type)
	fb_type = fb_type or Macros.Game.FBType.MAIN_DUNGEON
	local vip_type = fb_type == Macros.Game.FBType.MAIN_DUNGEON and Macros.Game.VIPType.MAINLINE_RESET 
						or Macros.Game.VIPType.ELITELINE_RESET
	--当前vip等级
	local userVip = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL )
	--当前持有次数
	local now = CallModuleFunc(ModuleType.VIP, "GetPrivilege" , vip_type )
	local nex = 0
	local viplv = userVip
	local text = ""
	for i=1 , 50 do
		viplv = viplv + 1
		nex = CallModuleFunc(ModuleType.VIP, "GetPrivilege" , vip_type , viplv)
		if nex and now ~= nex and nex ~= 0 then
			text = CPPGameLib.GetString("dungeon_resetVipTips", viplv , nex)
			break
		elseif not nex then
			text = CPPGameLib.GetString("dungeon_resetVipMaxTips")
			break
		end
	end

	return text
end

--获取副本章节红点数
function Modules.DungeonModule:GetChapterRedDotNum(dungeon_type_systemid,chapter_index)
	return self.__red_dot_map[dungeon_type_systemid][chapter_index] or 0
end

--获取副本红点数
function Modules.DungeonModule:GetDungeonRedDotMap()
	return self.__red_dot_map
end

--计算某章之前和之后的红点数量
function Modules.DungeonModule:GetDungeonRedDotByChapterIndexAndSystemId(dungeon_type_systemid,chapter_index)	
	if not dungeon_type_systemid or not chapter_index or not self.__red_dot_map[dungeon_type_systemid] then
		return 0,0
	end
	--计算红点数量
	local before_reddot_num,after_reddot_num
	for cur_chapter_index,single_chapter_num in ipairs(self.__red_dot_map[dungeon_type_systemid]) do
		before_reddot_num = before_reddot_num or 0
		after_reddot_num = after_reddot_num or 0
		if cur_chapter_index < chapter_index then
			before_reddot_num = before_reddot_num + single_chapter_num
		elseif cur_chapter_index > chapter_index then
			after_reddot_num = after_reddot_num + single_chapter_num
		end
	end
	return before_reddot_num,after_reddot_num
end

--设置精英外敌关卡地图id
function Modules.DungeonModule:SetEliteExternalDungeonMapId(map_id)	
	self.__elite_external_dungeon_mapid = map_id
end

--设置是否打副本导致升级
function Modules.DungeonModule:SetIsFightDungeonFBMakeLevelUp(is_make)	
	self.__is_fightdungeon_make_levelup = is_make
end

--获取是否打副本导致升级
function Modules.DungeonModule:GetIsFightDungeonFBMakeLevelUp(is_make)	
	return self.__is_fightdungeon_make_levelup
end

--设置是否为主线副本战斗
function Modules.DungeonModule:SetIsMainDungeonFight(is_maindungeon)	
	self.__is_maindungeon_fight = is_maindungeon
end

--获取是否为主线副本战斗
function Modules.DungeonModule:GetIsMainDungeonFight()	
	return self.__is_maindungeon_fight
end

--获取英雄降临是否有效
function Modules.DungeonModule:GetIsHeroAdventVaild()	
	local dungeonbox_reward_level = self:GetFBPassedBoxRewardLevel()
	return dungeonbox_reward_level < #CPPGameLib.GetConfig("MainFBChapterReward",nil,nil,true)
end

--显示添加好友提示
function Modules.DungeonModule:SetIsShowAddFriendTips(is_hide)
	self.__is_show_add_friend_tips = not is_hide and true or false
end

--显示添加好友提示
function Modules.DungeonModule:GetIsShowAddFriendTips()
	return self.__is_show_add_friend_tips
end

--是不是精英副本的外遇
function Modules.DungeonModule:GetEliteTranscriptTidy( id )
	local t1 = {}
	for k,v in pairs(CPPGameLib.GetConfig("EliteFBChapter",nil,nil,true)) do
		for id , vv in ipairs( v.enemy_map_list ) do
			t1[vv] = true 
		end
	end
	return t1[ id ]
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--设置章节列表
function Modules.DungeonModule:__SetChapterList(chapter_list, fb_type)
	if not chapter_list then
		return
	end

	self.__chapter_data_map[fb_type] = self:__ParseChapterList(chapter_list, fb_type)
end

--设置额外数据
function Modules.DungeonModule:__SetExtraData(extra_data, fb_type)
	self.__ext_data_map[fb_type] = extra_data
end

--解析章节列表
function Modules.DungeonModule:__ParseChapterList(chapter_list, fb_type)
	if not chapter_list then
		return
	end

	--解析日常副本章节列表
	if fb_type == Macros.Game.FBType.DAILY_DUNGEON then
		local battled_chapter_map = {}
		for _, v in pairs(chapter_list) do
			battled_chapter_map[v.chapter] = {}
			battled_chapter_map[v.chapter].__map_idx_record = v.map_idx_record
			battled_chapter_map[v.chapter].__battle_times = v.battle_times
		end
		chapter_list = {}		
		for chapter_index, config_chapter in ipairs(CPPGameLib.GetConfig("DailyChapter",nil,nil,true)) do
			local chapter_info = {}
			chapter_info.__map_idx_record = battled_chapter_map[chapter_index] and battled_chapter_map[chapter_index].__map_idx_record or 0
			chapter_info.__remanent_battle_times = config_chapter.daily_battle_times - (battled_chapter_map[chapter_index] and battled_chapter_map[chapter_index].__battle_times or 0)

			--关卡列表
			chapter_info.gamelevel_list = {}
			for gamelevel_index, gamelevel_id in ipairs(config_chapter.gamelevel_list) do
				local gamelevel_info = {}
				if chapter_info.__map_idx_record == 0 then
					gamelevel_info.no_passed_flag = true
				else
					gamelevel_info.no_passed_flag = chapter_info.__map_idx_record < gamelevel_index
				end
				table.insert(chapter_info.gamelevel_list, gamelevel_info)
			end

			table.insert(chapter_list, chapter_info)
		end
		return chapter_list
	end	

	--检测历史全部通关、星级奖励列表
	local config_chapter
	local extra_map_list = {}
	for chapter_index, chapter_info in ipairs(chapter_list) do
		config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, fb_type)
		if config_chapter then
			if chapter_info.gamelevel_list then
				chapter_info.gamelevel_list = chapter_info.gamelevel_list:GetData()
				if #chapter_info.gamelevel_list >= #config_chapter.gamelevel_list then
					--历史全部通关
					chapter_info.is_history_passed_all = true
				end
			end
			if chapter_info.star_award_list then
				chapter_info.star_award_list = chapter_info.star_award_list:GetData()
			end
			--精英副本外域入侵
			if chapter_info.enemy_map_id and chapter_info.enemy_map_id ~= 0 then
				table.insert(extra_map_list,{map_id = chapter_info.enemy_map_id,chapter_index = chapter_index,is_dead = chapter_info.is_enemy_dead})
			end		
		end
	end

	--精英副本外域入侵数据
	if fb_type == Macros.Game.FBType.ELITE_DUNGEON then
		self:__SetExtraData(extra_map_list,Macros.Game.FBType.ELITE_DUNGEON)
	end

	--检测下一个章节的开启
	local cur_chapter_index = #chapter_list
	if cur_chapter_index >= 0 and cur_chapter_index < ConfigAdapter.Dungeon.GetChapterNum(fb_type) then
		local cur_chapter_info = chapter_list[cur_chapter_index]
		if cur_chapter_info then
			config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(cur_chapter_index, fb_type)
		end

		local flag
		if fb_type == Macros.Game.FBType.MAIN_DUNGEON then
			--(当前没打过任何章节) 或 (当前章节的关卡已全部通关)，则构造将要打的下一个章节
			flag = not cur_chapter_info or #cur_chapter_info.gamelevel_list >= #config_chapter.gamelevel_list

		elseif fb_type == Macros.Game.FBType.HERO_DUNGEON then
			--(当前没打过任何章节) 或 (当前章节的关卡已全部通关 且 下一个章节的前置主线关卡已通关)，则构造将要打的下一个章节
			local next_chapter_index = cur_chapter_index + 1
			local next_config_chapter = CPPGameLib.GetConfig("HeroFBChapter", next_chapter_index)
			local pre_main_chapter_info = self:GetChapterInfo(next_config_chapter.pre_chapter_id)
			flag = not cur_chapter_info or (#cur_chapter_info.gamelevel_list >= #config_chapter.gamelevel_list and pre_main_chapter_info and pre_main_chapter_info.is_history_passed_all)
		elseif fb_type == Macros.Game.FBType.ELITE_DUNGEON then
			--(当前没打过任何章节) 或 (当前章节的关卡已全部通关)，则构造将要打的下一个章节
			flag = not cur_chapter_info or #cur_chapter_info.gamelevel_list >= #config_chapter.gamelevel_list
		end

		if flag then
			local next_chapter_info = {}
			next_chapter_info.gamelevel_list = {}
			next_chapter_info.star_award_list = {}

			--插入将要打的下一个章节
			table.insert(chapter_list, next_chapter_info)

			--检测英雄副本的新章节开启
			if fb_type == Macros.Game.FBType.MAIN_DUNGEON then
				local hero_chapter_list = self:GetChapterList(Macros.Game.FBType.HERO_DUNGEON)
				if hero_chapter_list then
					local next_chapter_index = #hero_chapter_list + 1
					local cur_chapter_info = self:GetChapterInfo(next_chapter_index - 1,Macros.Game.FBType.HERO_DUNGEON)
					local next_config_chapter = CPPGameLib.GetConfig("HeroFBChapter", next_chapter_index,false)
					if cur_chapter_info and cur_chapter_info.is_history_passed_all and next_config_chapter then														
						local config_chapter = CPPGameLib.GetConfig("HeroFBChapter", next_chapter_index - 1)
						local flag = #cur_chapter_info.gamelevel_list >= #config_chapter.gamelevel_list and (#chapter_list > next_config_chapter.pre_chapter_id) and true or false
						if flag then
							local next_chapter_info = {}
							next_chapter_info.gamelevel_list = {}
							next_chapter_info.star_award_list = {}

							--插入将要打的下一个章节
							table.insert(hero_chapter_list, next_chapter_info)
						end
					end
				end
			end
		end
	end	

	return chapter_list
end

--解析章节数据
function Modules.DungeonModule:__ParseChapterInfo(chapter_index, fb_type)
	if not chapter_index then
		return
	end

	local chapter_info = self.__chapter_data_map[fb_type][chapter_index]
	if not chapter_info then
		return
	end
	if chapter_info.constructed then
		return chapter_info
	end

	local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, fb_type)
	if not config_chapter then
		return
	end

	--下一个将要打的关卡
	local next_gamelevel

	--主线副本
	if fb_type == Macros.Game.FBType.MAIN_DUNGEON or fb_type == Macros.Game.FBType.ELITE_DUNGEON then
		--章节星级
		chapter_info.cur_star = 0
		chapter_info.max_star = Macros.Game.FBMaxStarPerMap * #config_chapter.gamelevel_list
		for gamelevel_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
			if config_chapter.gamelevel_list[gamelevel_index] then
				--初始化关卡宝箱状态
				if gamelevel_info.box_award_flag == Macros.Global.TRUE then
					gamelevel_info.box_award_flag = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
				else
					local gamelevel_detail_info = CPPGameLib.GetConfig("GameLevel",config_chapter.gamelevel_list[gamelevel_index])
					gamelevel_info.box_award_flag = gamelevel_detail_info.box_id_list and Modules.DungeonModule.BoxStatus.CAN_RECEIVE or Modules.DungeonModule.BoxStatus.NONE				
				end
				chapter_info.cur_star = chapter_info.cur_star + gamelevel_info.star
			end
		end

		--星级宝箱
		chapter_info.has_star_award = false
		local star_award_list = chapter_info.star_award_list
		if config_chapter then
			for index, tgt_star in ipairs(config_chapter.star_list) do
				--初始化星级宝箱状态
				if star_award_list[index] == Macros.Global.TRUE then
					star_award_list[index] = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
				else
					star_award_list[index] = chapter_info.cur_star >= tgt_star and Modules.DungeonModule.BoxStatus.CAN_RECEIVE or Modules.DungeonModule.BoxStatus.NONE
					if star_award_list[index] == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
						chapter_info.has_star_award = true
					end
				end
			end
		end
		chapter_info.star_award_list = star_award_list

		--构造下一个将要打的关卡
		if not chapter_info.is_history_passed_all then
			next_gamelevel = {}
			next_gamelevel.star = 0
			next_gamelevel.battle_times = 0
			next_gamelevel.reset_times = 0
			next_gamelevel.box_award_flag = Modules.DungeonModule.BoxStatus.NONE
			next_gamelevel.no_passed_flag = true
		end

	--英雄副本
	elseif fb_type == Macros.Game.FBType.HERO_DUNGEON then
		--章节星级
		for gamelevel_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
			--初始化关卡宝箱状态
			if gamelevel_info.box_award_flag == Macros.Global.TRUE then
				gamelevel_info.box_award_flag = Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE
			else
				local gamelevel_detail_info = CPPGameLib.GetConfig("GameLevel",config_chapter.gamelevel_list[gamelevel_index])
				gamelevel_info.box_award_flag = gamelevel_detail_info.box_id_list and Modules.DungeonModule.BoxStatus.CAN_RECEIVE or Modules.DungeonModule.BoxStatus.NONE				
			end
		end

		--历史通关记录
		chapter_info.history_passed_gamelevel_num = #chapter_info.gamelevel_list
		chapter_info.history_total_gamelevel_num = #config_chapter.gamelevel_list

		--构造下一个将要打的关卡
		if not chapter_info.is_history_passed_all then
			next_gamelevel = {}
			next_gamelevel.battle_times = 0
			next_gamelevel.box_award_flag = Modules.DungeonModule.BoxStatus.NONE
			next_gamelevel.no_passed_flag = true
		end		

	--日常副本
	elseif fb_type == Macros.Game.FBType.DAILY_DUNGEON then		
		chapter_info.is_unlocked = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL) >= config_chapter.gamelevel_limit_list[1]
		chapter_info.is_opened = false
		local date_info = TimeTool.GetDateFromTimeStamp(CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"), nil, true)
		local cur_week = date_info.wday
		for _, week in ipairs(config_chapter.week_day_list) do
			if week == cur_week then
				chapter_info.is_opened = true
				break
			end
		end
	end

	--插入下一个将要打的关卡
	if next_gamelevel then
		table.insert(chapter_info.gamelevel_list, next_gamelevel)
	end

	chapter_info.constructed = true	

	return chapter_info
end

--检测主线副本、英雄副本通关
function Modules.DungeonModule:__CheckPassMainDungeonGameLevel()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.type ~= Macros.Game.FBType.MAIN_DUNGEON and battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON then
		return
	end

	if battle_info.no_passed_flag then
		--派发通关事件
		self:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_PASS_GAME_LEVEL, battle_info.gamelevel_id)		
	end

	return true
end

--检测首次通关爬塔副本
function Modules.DungeonModule:__CheckPassTowerGameLevel()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info.type ~= Macros.Game.FBType.TOWER_COMMON and battle_info.type ~= Macros.Game.FBType.TOWER_ELITE then
		return
	end

	local flag = CallModuleFunc(ModuleType.ROLE, "GetProgressTag", Macros.Game.LeadProgressTagType.LEAD_TOWER_DUNGEON_PASSED)
	if flag ~= Macros.Global.TRUE then
		--更新记录
		CallModuleFunc(ModuleType.ROLE, "SetProgressTag", Macros.Game.LeadProgressTagType.LEAD_TOWER_DUNGEON_PASSED, Macros.Global.TRUE)

		--派发通关事件
		self:FireNextFrame(Macros.Event.DungeonModule.TOWER_GAMELEVEL_PASSED_FIRST_TIME)
	end

	return true
end

--统计副本红点数量(主线、英雄、精英)
function Modules.DungeonModule:__CountDungeonRedDot(dungeon_type, system_id)	
	self.__red_dot_map[system_id] = {}

	--已开启章节列表
	local chapter_list = self:GetChapterList(dungeon_type)
	--已开启章节数量
	local chapter_num = chapter_list and #chapter_list or 0

	for chapter_index = 1, chapter_num do
		local single_chapter_num = 0

		--章节信息
		local chapter_info = self:GetChapterInfo(chapter_index,dungeon_type)
		if chapter_info then	
			--关卡宝箱红点
			for index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
				if gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
					single_chapter_num = single_chapter_num + 1
				end				
			end

			--星级宝箱红点
			if dungeon_type == Macros.Game.FBType.MAIN_DUNGEON or dungeon_type == Macros.Game.FBType.ELITE_DUNGEON then
				--章节配置表信息
				local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, dungeon_type)
				for index, tgt_star in ipairs(config_chapter.star_list) do
					--初始化星级宝箱状态
					if chapter_info.star_award_list[index] == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
						single_chapter_num = single_chapter_num + 1
					end
				end
			end

			table.insert(self.__red_dot_map[system_id], single_chapter_num)
		end
	end

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.DUNGEON, nil, system_id)	
end

--更新副本红点数量(主线、英雄、精英)
function Modules.DungeonModule:__UpdateDungeonRedDot(system_id, index, change_num)	
	if change_num then
		self.__red_dot_map[system_id][index] = change_num
	else
		self.__red_dot_map[system_id][index] = self.__red_dot_map[system_id][index] - 1
	end				

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.DUNGEON, nil, system_id)		
end
