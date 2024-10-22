
--[[
%% @module: 主线副本关卡界面
%% @author: swordman sue
%% @created: 2016-08-11
--]]

Modules = Modules or {}

Modules.DungeonGameLevelView = Modules.DungeonGameLevelView or BaseClass(BaseView)

local dst_bg_width = 1386
local src_bg_width = 937

function Modules.DungeonGameLevelView:__init()
	self.__layout_name = "dungeon_gamelevel"	
	self.__dungeon_type = Macros.Game.FBType.MAIN_DUNGEON
	self.__is_ignore_iphone_safe_area = true
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("dungeon_main_gamelevel"),
		show_stamina = true,
		show_diamond = true,
		show_power = true,
		show_level = true,
		close_func = function()			
			if self.__is_first_passed_all or self.__is_shield_touching or self.__is_first_open then
				if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
					return
				end
			end
			local top_stack_info = GlobalViewMgr:GetTopStackInfo()
			if not top_stack_info then
				self:CallModuleFunc("OpenView", BaseViewType.DUNGEON, BaseViewSubType.DUNGEON.MAIN, function()
					GlobalViewMgr:ClearStackInfo()
					self:CloseManual(true)
				end)
			else
				self:CloseManual()
			end
		end,
	}

	self.__loaded_callback = function ()
		self.__Image__HeroDungeon_CanChallengeTimes = self:GetWidget("Image__HeroDungeon_CanChallengeTimes")
		--屏蔽层
		self.__panel_shield = self:GetWidget("Panel_shield")
		self.__panel_shield:SetZOrder(3)
		self.__panel_shield:SetVisible(false)
		--跳章按钮
		--后退
		self.__button_left = self:GetWidget("Button_left")	
		self.__button_left:SetAnchorPoint(0,0.5)
		Util:WidgetButton(self.__button_left,"",function ()
			self:__DeleteBulletScreen()
			if self.__btn_message_board then
				self.__btn_message_board:RemoveSelf(true)
				self.__btn_message_board = nil
			end
			if self.__lead_player_attack_effectid then
				self:__DelEffect(self.__lead_player_attack_effectid)
				self.__lead_player_attack_effectid = nil
			end
			self.__weaklead_gamelevel_index = nil
			CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index - 1)
		end)
		
		--前进
		self.__button_right = self:GetWidget("Button_right")
		self.__button_right:SetAnchorPoint(1,0.5)	
		Util:WidgetButton(self.__button_right,"",function ()
			self:__DeleteBulletScreen()
			if self.__btn_message_board then
				self.__btn_message_board:RemoveSelf(true)
				self.__btn_message_board = nil
			end
			if self.__lead_player_attack_effectid then
				self:__DelEffect(self.__lead_player_attack_effectid)
				self.__lead_player_attack_effectid = nil
			end
			self.__weaklead_gamelevel_index = nil
			CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1)
		end)		

		--处理iPhoneX的适配
		self:__HandleIPhoneXAdaptation()

		--闪烁层
		self.__panel_blink = self:GetWidget("Panel_blink")	

		--触摸层
		self.__panel_touch = self:GetWidget("Panel_touch")	
		self.__panel_touch:SetTouchSwallow(false)
		--触摸层
		WidgetUnity.SetWidgetListener(self.__panel_touch,function (x,y)
			self.__move_x = x
			local inner_container_x = self.__scroll_node:GetInnerContainerPosition()
			self.__can_move_left = inner_container_x >= 0
			self.__can_move_right = Macros.Global.DesignSize.width - inner_container_x >= dst_bg_width - 1

		end,nil,function (x,y)
			if (x - self.__move_x) >= 50 and self.__can_move_left then
				if not self.__button_left:IsVisible() then
					return
				end
				CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index - 1)
			elseif (self.__move_x - x) >= 50 and self.__can_move_right then
				if not self.__button_right:IsVisible() then
					return
				end				
				CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1)
			end
		end)
	end

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_GAMELEVEL)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_PASSEDALL_BOXTIP)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL)  
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,true)   
end

function Modules.DungeonGameLevelView:__OpenCallback()

	self.__chapter_index, self.__gamelevel_index, self.__is_first_passed_all,open_callback,self.__is_first_open,self.__open_status,self.__battle_end_func = unpack(self.__open_data)
	local chapter_list = self:CallModuleFunc("GetChapterList",self.__dungeon_type)
	if not self.__chapter_index then		
		self.__chapter_index = #chapter_list
	end
	--闪烁效果
	self.__panel_blink:SetVisible(true)
	CPPActionManager.FadeOut(self.__panel_blink, 0.25, GlobalCallbackMgr:AddCallBackFunc(function ()
		self.__panel_blink:SetVisible(false)
		self.__panel_blink:SetOpacity(255)
	end))
	self.__chapter_info = self:CallModuleFunc("GetChapterInfo", self.__chapter_index,self.__dungeon_type)
	self.__config_chapter = self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON and CPPGameLib.GetConfig("MainFuben", self.__chapter_index)
							or CPPGameLib.GetConfig("EliteFBChapter", self.__chapter_index)
	self:__InitTitleAndBG()
	self:__InitGameLevels()
	self:__InitStarAwards()
	--TEMP：屏蔽弹幕功能
	-- self:__InitMessageBoard()
	self.__Image__HeroDungeon_CanChallengeTimes:SetVisible(false)
	
	--红点数量
	self.__cur_systemid = self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON and Macros.Game.SystemID.DUNGEON_GAMELEVEL or Macros.Game.SystemID.DUNGEON_ELITE
	local before_reddot_num,after_reddot_num = self:CallModuleFunc("GetDungeonRedDotByChapterIndexAndSystemId",self.__cur_systemid,self.__chapter_index)	
	--后退
	self.__button_left:SetVisible(self.__chapter_index > 1)
	if self.__button_left:IsVisible() then
		local image_leftreddot = self:GetWidget("Image_leftRedDot")
		image_leftreddot:SetVisible(before_reddot_num > 0)
	end
	--前进
	self.__button_right:SetVisible(self.__chapter_index < #chapter_list)
	if self.__button_right:IsVisible() then
		local image_rightreddot = self:GetWidget("Image_rightRedDot")
		image_rightreddot:SetVisible(after_reddot_num > 0)
	end

	--记录章节打开记录
	self:CallModuleFunc("SetOpenChapterIndex",self.__dungeon_type,self.__chapter_index)

	--打开回调
	if open_callback and type(open_callback) == 'function' then
		open_callback()
	end		

	--添加助战好友
	local assist_target_id,assist_target_name = self:CallModuleFunc("GetAssistRoleID")
	local is_show_addfriend  = self:CallModuleFunc("GetIsShowAddFriendTips")
	if self.__is_first_passed_all then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			if self:IsOpen() then
				self.__panel_shield:SetVisible(not CallModuleFunc(ModuleType.LEAD, "IsInLead") and true or false)
				--self:SetTitleBarVisible(false)
				self.__close_back = close_callback
				if assist_target_id and not CallModuleFunc(ModuleType.LEAD, "IsInLead") and not CallModuleFunc(ModuleType.STORY, "IsInStory") and is_show_addfriend then
					LogicGUI.TemplDungeonAddAssistRoleView.New(assist_target_id,assist_target_name,function (is_hide)
						self:CallModuleFunc("SetIsShowAddFriendTips",is_hide)
						self:__ChapterPassedAllEffect()
					end)
				else
					self:__ChapterPassedAllEffect()
				end					
			else
				close_callback()
			end
		end, Macros.Game.SpDialogPriority.CHAPTER_PASSED_ALL)
	else
		if assist_target_id and not CallModuleFunc(ModuleType.LEAD, "IsInLead") and not CallModuleFunc(ModuleType.STORY, "IsInStory")  and is_show_addfriend then
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)				
				if self:IsOpen() and assist_target_id and not CallModuleFunc(ModuleType.LEAD, "IsInLead") and not CallModuleFunc(ModuleType.STORY, "IsInStory") then
					LogicGUI.TemplDungeonAddAssistRoleView.New(assist_target_id,assist_target_name,function (is_hide)
						self:CallModuleFunc("SetIsShowAddFriendTips",is_hide)
						close_callback()
						--整合判断
						self:JudgeToAction()
					end)
					return
				end
				close_callback()
				--整合判断
				self:JudgeToAction()
			end, Macros.Game.SpDialogPriority.CHAPTER_PASSED_ALL)		
		else
			--整合判断
			self:JudgeToAction()
		end
	end


	self:CallModuleFunc("SetAssistRoleID")
end

function Modules.DungeonGameLevelView:GetOpenParams()
	return {self.__chapter_index,self.__gamelevel_index}
end

function Modules.DungeonGameLevelView:__Dispose()
	CPPGameLib.ClearObjList(self.__gamelevel_list)
	self.__gamelevel_list = nil	

	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end

	if self.__lighting_effect_id then
		self:__DelEffect(self.__lighting_effect_id)
		self.__lighting_effect_id = nil
	end
	self:__DeleteBulletScreen() 
	--删除控件
	self:TheSameAction()
	--关闭扫荡界面
	self:CallModuleFunc("CloseCleanView")
end

function Modules.DungeonGameLevelView:__RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_DATA_RESP, function()
			self:__InitRebelInfo()
		end)

	self.__init_rebel = true
	local rebel_find_list = CallModuleFunc(ModuleType.REBEL, "GetRebelFindList")
	if not rebel_find_list then						
		--请求叛军数据
		CallModuleFunc(ModuleType.REBEL, "RebelDataReq")
		
	else
		self:__InitRebelInfo()
	end

	--监听宝箱领取响应事件	
	self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_AWARD_GET_RESP, function(_, award_type, index)
		if award_type == Macros.Game.FBRewardType.STAR then
			self:__SetStarboxImg(index)
		end
	end)	

	--监听宝箱领取响应事件	
	self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_ONEKEY_AWARD_GET_RESP, function(_,is_get_award)
		if self.__templ_dungeon_passedall_tip then
			self.__templ_dungeon_passedall_tip:DeleteMe()
			self.__templ_dungeon_passedall_tip = nil			
		end
		if self.__close_back then
			self.__close_back()
			self.__close_back = nil
		end
		if is_get_award then
			self:__OneKeyTakeAward()
		else
			local max_chapter_num = ConfigAdapter.Dungeon.GetChapterNum(self.__dungeon_type)
			if (self.__chapter_index + 1) > max_chapter_num then				
				self:TheSameAction()
			else
				self.__weaklead_gamelevel_index = nil
				CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1,1,nil,nil,true,self.__open_status,self.__battle_end_func)			
			end			
		end		
	end)	

	--战队升级事件	
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_,old_level,level)
		self:__InitLineUpAndRankBtn()
	end)	

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		--新手指引特殊操作检测(选择主线关卡)
		if self.__gamelevel_list then
			for _, templ_gamelevel in ipairs(self.__gamelevel_list) do
				local data = templ_gamelevel:GetData()
				templ_gamelevel:CheckSectionOperateSP()
			end
		end

		--新手指引特殊操作检测(选择主线星级宝箱（宝箱索引，从1开始）)
		for index, _ in ipairs(self.__config_chapter.star_list) do
			local panel_staraward = self:GetWidget("Panel_StarAward"..index)
			CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_STAR_BOX, panel_staraward, index)							
		end
		
		if self.__btn_lineup then
			CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_GAMELEVEL_HERO_LINEUP, self.__btn_lineup)	
		end		
	end)

	--红点更新事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		--更新阵容红点
		if system_id == Macros.Game.SystemID.HERO_LINEUP then
			self:__UpdateLineupBtnRedDot()
		end
	end)

	--更新副本关卡数据
	self:BindGlobalEvent(Macros.Event.DungeonModule.UPDATE_DUNGEON_GAMELEVEL_DATA, function(_, gamelevel_index)
		self:__UpdateGameLevelData(gamelevel_index)
	end)

	--屏蔽关卡界面点击
	self:BindGlobalEvent(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH, function(_, is_shield)
		self.__panel_shield:SetVisible(not CallModuleFunc(ModuleType.LEAD, "IsInLead") and is_shield or false)
		self.__is_shield_touching = is_shield
	end)

	--弹出英雄降临界面
	self:BindGlobalEvent(Macros.Event.LeadModule.START_SP, function(_,lead_id)
		if lead_id == Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_ONE 
			or lead_id == Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_TWO 
			or lead_id == Macros.Game.LeadSpecialID.HERO_ADVENT_POPUP_THREE then
			CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT,true)
		end
	end)

	--英雄降临之后继续其他步骤
	self:BindGlobalEvent(Macros.Event.DungeonModule.AFTER_HERO_ADVENT_POPUP, function()
		if self.__open_status and not self.__first_call_back then
			self.__first_call_back = true
			self:JudgeOpenWhichView(true)
		else
			self:TheSameAction()		
		end
	end)

	--领取主线副本章节奖励
	self:BindGlobalEvent(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD, function(_,data)
		self:__HandleDungeonReward()
	end)

	--TEMP：屏蔽弹幕功能
	--留言列表回调
    -- self:BindGlobalEvent(Macros.Event.DungeonModule.GAMELEVEL_COMMENT,function (__,comment_list,is_message_req)
    -- 	if not is_message_req then
	   --  	self.__messageboard_data = comment_list
	   --  	self:__CreateBulletScreen()
	   --  end
    -- end)

    --弹幕显示
   --  self:BindGlobalEvent(Macros.Event.DungeonModule.REFRASH_MESSAGE_BOARD,function ()
   --  	local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.MessageBoard)) ~= "0"
   --  	if flag then
   --  		--留言数据
			-- CallModuleFunc(ModuleType.DUNGEON,"MapCommentListReq",self.__chapter_index)
   --  	end
   --  end)
	
	self:__InitLevelGiftInfo()
	--阵容和排行榜按钮
	self:__InitLineUpAndRankBtn()
	--英雄降临图标
	self:__InitHeroAdvent()
end

function Modules.DungeonGameLevelView:__InitTitleAndBG()
	--章节名
	local img_chapter_name_bg = self:GetWidget("Image_ChapterNameBG")
	if not self.__rt_chapter_name then
		self.__rt_chapter_name = Util:RichText(string.format(Macros.Color.dungeon_chapter_name_rt_gradient, self.__config_chapter.name), Macros.TypefaceSize.popup)
		self.__rt_chapter_name:SetAnchorPoint(0, 0.5)
		img_chapter_name_bg:AddChild(self.__rt_chapter_name)
		PosTool.LeftCenter(self.__rt_chapter_name, 10)
	end
	local lab = GUI.RichText.GetColorWord(string.format(Macros.Color.dungeon_chapter_name_rt_gradient, self.__config_chapter.name))
	self.__rt_chapter_name:SetText(lab)
	--背景
	if not self.__scroll_node then
		self.__scroll_node = self:GetWidget("ScrollView_Node")
		self.__scroll_node:SetScrollDirection(UIScrollViewEx.ScrollDirHorizontal)
		self.__scroll_node:SetInnerContainerSize(dst_bg_width, 0)
		self.__scroll_node:SetTouchEnabled(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))
		GUI.MultiResolutionAdapter.AdaptForBG(self.__scroll_node, self:GetNode(), self.__is_force_stretch, self.__is_ignore_iphone_safe_area)
	end

	local chapter_bg_id = ConfigAdapter.Dungeon.GetChapterBGID(self.__chapter_index,self.__dungeon_type)
	local sp = Resource.PathTool.GetUIResPath("dungeon_gamelevel_bg/"..chapter_bg_id, nil, "jpg")
	if not self.__img_bg then
		self.__img_bg = Util:Sprite(sp)
		self.__img_bg:SetAnchorPoint(0, 0)
		self.__img_bg:SetScaleFactor(dst_bg_width / src_bg_width)	
		self.__scroll_node:AddChild(self.__img_bg)
	else
		self.__img_bg:SetImage(sp)
	end
	
	--关卡路线
	if self.__config_chapter.gamelevel_line_id then
		sp = Resource.PathTool.GetGameLevelLinePath(self.__config_chapter.gamelevel_line_id)
		if not self.__img_line then
			self.__img_line = Util:Sprite(sp)
			self.__img_line:SetAnchorPoint(0, 0)
			self.__scroll_node:AddChild(self.__img_line)
		else
			self.__img_line:SetImage(sp)
		end
		self.__img_line:SetPosition(self.__config_chapter.gamelevel_line_offset_pos[1],self.__config_chapter.gamelevel_line_offset_pos[2])
	end
end

--初始化关卡
function Modules.DungeonGameLevelView:__InitGameLevels()
	CPPGameLib.ClearObjList(self.__gamelevel_list)
	self.__gamelevel_list = {}
	--缓存特效播放
	local setting_str = self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON and Macros.Setting.MainDungeonGameLevelEffect
						or Macros.Setting.EliteDungeonGameLevelEffect

	--寻找当前已开启到的关卡
	local open_gamelevel_num = 0
	for index=1,#self.__config_chapter.gamelevel_list do
		local gamelevel_info = self.__chapter_info.gamelevel_list[index]
		if gamelevel_info and gamelevel_info.no_passed_flag then
			open_gamelevel_num = index
			break
		end
	end
	
	--新手指引适配相关
	local pos_info = self.__config_chapter.gamelevel_pos_list[open_gamelevel_num]
	if pos_info and pos_info[1] > Macros.Global.DesignSize.width then
		self.__scroll_node:ScrollToRight()
	end	

	--角色等级
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	local chapter_list = self:CallModuleFunc("GetChapterList",self.__dungeon_type)

	local is_first_open = self.__is_first_open
	--查找配置
	local reward_config
	
	CPPActionManager.DelayTo(self.__scroll_node,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
		--关卡
		for index, id in ipairs(self.__config_chapter.gamelevel_list) do	
			--OGL坐标	
			local pos_info = self.__config_chapter.gamelevel_pos_list[index]
			local templ_gamelevel = self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON and Modules.TemplDungeonGameLevel.New(self.__scroll_node)
							or Modules.TemplEliteDungeonGameLevel.New(self.__scroll_node)	
			templ_gamelevel:SetPosition(pos_info[1], pos_info[2])
			templ_gamelevel:SetData({
				chapter_index = self.__chapter_index, gamelevel_index = index, gamelevel_id = id,
				 gamelevel_info = self.__chapter_info.gamelevel_list[index], dungeon_type = self.__dungeon_type
			})
			if self.__chapter_info.gamelevel_list[index] and self.__chapter_info.gamelevel_list[index].no_passed_flag then
				CPPActionManager.DelayTo(self.__scroll_node,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
					templ_gamelevel:__ShowEffect()
				end))			
			end
			-- --关卡点连接
			-- local posInfo_2 = self.__config_chapter.gamelevel_pos_list[index + 1]

			-- if open_gamelevel_num == 1 then
			-- 	--新章节第一关
			-- 	templ_gamelevel:__ShowEffect()
			-- elseif open_gamelevel_num == 0 then
			-- 	--全部通关
			-- 	if index ~= #self.__config_chapter.gamelevel_list then				
			-- 		Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
			-- 	end
			-- else
			-- 	if index < open_gamelevel_num - 1 then
			-- 		Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
			-- 	elseif index == open_gamelevel_num - 1 then		
			-- 		local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d",setting_str, self.__config_chapter.gamelevel_list[index+1])) ~= "1"			
			-- 		if flag then
			-- 			Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list,true,function ()
			-- 				-- self.__gamelevel_list[index+1]:GetNode():SetEnabled(true)
			-- 				self.__gamelevel_list[index+1]:__ShowEffect()
			-- 			end)
			-- 		else
			-- 			Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
			-- 		end
			-- 	elseif index == open_gamelevel_num then	
			-- 		local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", setting_str, self.__config_chapter.gamelevel_list[index])) ~= "1"
			-- 		-- templ_gamelevel:GetNode():SetEnabled(not flag)
			-- 		if not flag then
			-- 			templ_gamelevel:__ShowEffect()
			-- 		end
			-- 	end
			-- end

			if index == #self.__config_chapter.gamelevel_list then
			 	reward_config = CPPGameLib.GetConfig("DungeonStarReward",self.__chapter_index,false)
			end

			--奖励提示
			--创建浮动奖励提示
			if not self.__jump_boss_reward and not self.__chapter_info.gamelevel_list[index] and reward_config and reward_config.is_starbox_reward == Macros.Global.FALSE and self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON then
				local bubble_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"bg_reward_short")
				local bubble_txt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, reward_config.is_reward_hero == 
								Macros.Global.FALSE and "passedall_reward_equip" or "passedall_reward_treasure",true)
				self.__jump_boss_reward = Util:Sprite(bubble_bg_sp)
				templ_gamelevel:GetNode():AddChild(self.__jump_boss_reward)
				self.__jump_boss_reward:SetPosition(100,90)
				local equip_txt = Util:Sprite(bubble_txt_sp)
				self.__jump_boss_reward:AddChild(equip_txt)
				PosTool.Center(equip_txt,0,5)
				self.funId2 = GlobalCallbackMgr:AddCallBackFunc(function ()
					ActionManager.GetInstance():RunJumpByAction(self.__jump_boss_reward,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
						self:RunAddAction(self.__jump_boss_reward,2)
					end))	
				end)
				self:RunAddAction(self.__jump_boss_reward,2)
			end
			--指定等级前的弱指引
			if not self.__weaklead_gamelevel_index and role_level <= ConfigAdapter.Common.GetDungeonWeakLeadNeedLevel() and self.__chapter_index == #chapter_list then
				--特殊处理
				if self.__chapter_index > 1 or (self.__chapter_index <= 1 and #self.__chapter_info.gamelevel_list > 2) then
					self.__weaklead_gamelevel_index = #self.__chapter_info.gamelevel_list
				end			
			end
			--弱指引
			if self.__weaklead_gamelevel_index and self.__weaklead_gamelevel_index == index and not CallModuleFunc(ModuleType.LEAD,"IsInLead") then
				local effect
				self.__lead_player_attack_effectid,effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"},templ_gamelevel:GetNode(),2)
				PosTool.Center(effect)
			end
			--打开关卡
			if self.__gamelevel_index and self.__gamelevel_index == index and not self.__open_status and not is_first_open then
				templ_gamelevel:__HandleGamelevelClick()
			end
			table.insert(self.__gamelevel_list, templ_gamelevel)
		end
	end))		
end

function Modules.DungeonGameLevelView:__OneKeyTakeAward()
	local reward_list = {}
	local chapter_info = self:CallModuleFunc("GetChapterInfo",self.__chapter_index,self.__dungeon_type)
	local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(self.__chapter_index, self.__dungeon_type)
	if config_chapter then
		for index, tgt_star in ipairs(config_chapter.star_list) do
			--初始化星级宝箱状态
			if chapter_info.star_award_list[index] == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
				local box_id_list = config_chapter[string.format("box_%d_id_list", index)]
				local box_num_list = config_chapter[string.format("box_%d_num_list", index)]
				if box_id_list then
					for i, obj_id in ipairs(box_id_list) do
						local obj_info = {}
						obj_info.id = obj_id
						obj_info.num = box_num_list[i] or 0
						table.insert(reward_list, obj_info)
					end
				end
			end
		end
		--精英宝箱
		for game_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do
			if gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
				local gamelevel_id = config_chapter.gamelevel_list[game_index]
				local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id, false)
				if config_gamelevel then
					for i, obj_id in ipairs(config_gamelevel.box_id_list) do
						local obj_info = {}
						obj_info.id = obj_id
						obj_info.num = config_gamelevel.box_num_list[i] or 0
						table.insert(reward_list, obj_info)
					end
				end
			end
		end
	end
	--奖励提示  
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list, function ()
		CallModuleFunc(ModuleType.DUNGEON, "UpdateFBPassedAllData",self.__chapter_index,self.__dungeon_type)
		local max_chapter_num = ConfigAdapter.Dungeon.GetChapterNum(self.__dungeon_type)		
		if self.__chapter_index + 1 > max_chapter_num then
			self:TheSameAction()
		else
			self.__weaklead_gamelevel_index = nil
			CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1,1,nil,nil,true,self.__open_status,self.__battle_end_func)
		end
	end )

end

function Modules.DungeonGameLevelView:__ChapterPassedAllEffect()
	local effectid,effect = self:__CreateEffect({id = 100029,loop = false,end_callback = function ()
		local chapter_info = self:CallModuleFunc("GetChapterInfo",self.__chapter_index,self.__dungeon_type)
		--精英宝箱
		local is_bound_tip = false
		for game_index, gamelevel_info in ipairs(chapter_info.gamelevel_list) do			
			if gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
				is_bound_tip = true
				break			
			end
		end
		if not is_bound_tip then
			is_bound_tip = chapter_info.has_star_award
			if not is_bound_tip then
				if self.__close_back then
					self.__close_back()
					self.__close_back = nil
				end		
				local max_chapter_num = ConfigAdapter.Dungeon.GetChapterNum(self.__dungeon_type)
				if self.__chapter_index + 1 > max_chapter_num then
					self:TheSameAction()
				else
					self.__weaklead_gamelevel_index = nil
					CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1,1,nil,nil,true,self.__open_status,self.__battle_end_func)
				end
			else
				self.__templ_dungeon_passedall_tip = Modules.TemplDungeonPassedAllTip.New(chapter_info,self.__chapter_index,self.__dungeon_type)
			end
		else
			self.__templ_dungeon_passedall_tip = Modules.TemplDungeonPassedAllTip.New(chapter_info,self.__chapter_index,self.__dungeon_type)
		end	
	end},self:GetNode(),3)	
	PosTool.Center(effect)
end

--初始化星级宝箱
function Modules.DungeonGameLevelView:__InitStarAwards()
	--删除跳动
	if self.__jump_reward then
		self.__jump_reward:RemoveSelf(true)
		self.__jump_reward = nil
	end
	local widget, img_box
	
	for index, tgt_star in ipairs(self.__config_chapter.star_list) do
		local str_star = math.min(self.__chapter_info.cur_star, tgt_star).."/"..tgt_star
		local str_star2 = self.__chapter_info.cur_star.."/"..tgt_star

		img_box = self:__SetStarboxImg(index)

		local panel_staraward = self:GetWidget("Panel_StarAward"..index)
		WidgetUnity.SetWidgetListener(panel_staraward, nil, nil, function()
			--打开宝箱信息界面
			local box_id_list = self.__config_chapter[string.format("box_%d_id_list", index)]
			local box_num_list = self.__config_chapter[string.format("box_%d_num_list", index)]
			local params = {}
			params.title = CPPGameLib.GetString("dungeon_star_box_title")
			params.reward_list = ConfigAdapter.Item.GetGiftPackRewardListII(box_id_list, box_num_list)
			params.type = Macros.Game.FBRewardType.STAR
			params.status = self.__chapter_info.star_award_list[index]
			params.condition = str_star2
			params.chapter_index = self.__chapter_index
			params.index = index
			params.fb_type = self.__dungeon_type
			Modules.TemplDungeonBoxInfo.New(params)
		end)

		--当前/目标星数
		widget = Util:WidgetLabel(self:GetWidget("Text_StarAward"..index), "", Macros.TypefaceSize.normal, Macros.Color.btn)
		widget:SetText(str_star)

		--新手指引特殊操作检测(选择主线星级宝箱（宝箱索引，从1开始）)
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_STAR_BOX, panel_staraward, index)				
	end	
end

--初始化叛军通知
function Modules.DungeonGameLevelView:__InitRebelInfo()
	local rebel_find_data = CallModuleFunc(ModuleType.REBEL, "GetCanHitRebelFindData" , true )
		
	local widget = self:GetWidget("Panel_Rebel")	
	widget:SetVisible(rebel_find_data ~= nil)

	-- CPPGameLib.PrintTable("ldx",rebel_find_data,"rebel_find_data: ")		
	if rebel_find_data then
		WidgetUnity.SetWidgetListener(widget, nil, nil, function()
			CallModuleFunc(ModuleType.REBEL, "ShowRebelLayer")
		end)
		--当前不在指引中，且本次入侵没自动给提示过，则自动提示
		local is_sureframe_popup_again = CallModuleFunc(ModuleType.DUNGEON, "GetIsSureFramePopUp")
		if self.__init_rebel and not CallModuleFunc(ModuleType.LEAD, "IsInLead") and is_sureframe_popup_again then			
			CallModuleFunc(ModuleType.REBEL, "ShowRebelLayer",function(is_attack)
				if not is_attack then					
					--延迟攻击
					CallModuleFunc(ModuleType.DUNGEON, "IsSureFramePopUp",false)
				end
			end)
		end
		Util:WidgetLabel(self:GetWidget("Text_Rebel"), CPPGameLib.GetString("dungeon_rebel_find"), Macros.TypefaceSize.slightly, Macros.Color.golden, nil, nil, Macros.Color.common_stroke)

		local role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo").role_id
		if rebel_find_data.discoverer_id == role_id then
			local data = CallModuleFunc(ModuleType.REBEL, "GetCanHitRebelFindData",true)
			local config_monster = CPPGameLib.GetConfig("Monster", rebel_find_data.monster_id)
			--品质框
			local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_"..data.quality)
		    local sp_icon_quality = Util:Sprite(imgdata)
		    self:GetWidget("Panel_RebelIcon"):AddChild(sp_icon_quality)
		    PosTool.Center(sp_icon_quality)

		    --品质底
			imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_"..data.quality)
		    local sp_icon__bg_quality = Util:Sprite(imgdata)
		    self:GetWidget("Panel_RebelIcon"):AddChild(sp_icon__bg_quality)
		    PosTool.Center(sp_icon__bg_quality)

		    --怪物头像
		    local icon_path_func = LogicGUI.IconLayer.IconPathFuncMap[Macros.Game.ObjType.HERO]
		    local icon_path = icon_path_func(config_monster.hero_id or config_monster.head_icon)
		    local sp_icon = Util:Sprite(icon_path)
		    self:GetWidget("Panel_RebelIcon"):AddChild(sp_icon)
		    PosTool.Center(sp_icon)

			-- local img_head = LogicGUI.IconLayer.New(false,false)
			-- self:GetWidget("Panel_RebelIcon"):AddChild(img_head:GetNode())
			-- PosTool.Center(img_head)
			-- img_head:SetData({id = config_monster.hero_id or config_monster.head_icon})

			local effectId, effect = self:__CreateEffect({res_name = "UI_panjuntouxiang"}, self:GetWidget("Panel_RebelIcon"),1)
			PosTool.Center(effect)

		end

	end
	self.__init_rebel = false
end


--初始化等级礼包通知
function Modules.DungeonGameLevelView:__InitLevelGiftInfo()
	local Panel_Levelgift = self:GetWidget("Panel_Levelgift")

	if CPPSdkTools.IsInExamine() then
		Panel_Levelgift:SetVisible(false)
		return
	end

	local Button_Levelgift = self:GetWidget("Button_Levelgift")
	Panel_Levelgift:SetVisible(false)
	Util:WidgetButton(Button_Levelgift, nil, function( ... )
		CallModuleFunc(ModuleType.ACTIVITY, "OpenView", BaseViewType.ACTIVITY, Macros.Game.SystemID.LEVEL_GIFTBAG )
	end)
	local num = CallModuleFunc(ModuleType.ACTIVITY, "GetLevelGiftRedDotNum")
	if num ~= 0 then
		ActionManager.GetInstance():AboutRotateAnimation( self:GetWidget("Button_Levelgift")  , 0.1 , 15)
		Panel_Levelgift:SetVisible(true)
	end

	--名字
	local xtName = Util:Label( CPPGameLib.GetString("activity_levelGiftBag"),Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	xtName:SetAnchorPoint(0.5,0.5)
	xtName:SetPosition( Panel_Levelgift:GetContentWidth()/2 , 10 )
	Panel_Levelgift:AddChild( xtName )
end

--初始化战队和排行榜按钮
function Modules.DungeonGameLevelView:__InitLineUpAndRankBtn()
	local panel_systementry = self:GetWidget("Panel_SystemEntry")
	--排行榜
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.RANK)
	if not CPPSdkTools.IsInExamine() then
		if is_open and not self.__btn_rank then
			local city_icon_id = CallModuleFunc(ModuleType.MAIN_CITY,"TranslateSystemIDToEntryID",Macros.Game.SystemID.RANK)
			local entry_info = CPPGameLib.GetConfig("CityIcon",city_icon_id)
			self.__btn_rank = Util:Button(Resource.PathTool.GetSystemEntryIconPath(entry_info.res_name), function()
				CallModuleFunc(ModuleType.RANK,"OpenView", BaseViewType.RANK,3)
			end)
			panel_systementry:AddChild(self.__btn_rank)
			self.__btn_rank:SetPosition(47,48)
			local label = Util:Label(CPPGameLib.GetString("rank_title"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
			self.__btn_rank:AddChild(label)
			PosTool.CenterBottom(label,0,12)
			CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_GAMELEVEL_RANK, self.__btn_rank)				
		end		
	end

	--阵容
	is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_LINEUP)
	if is_open and not self.__btn_lineup then
		local city_icon_id = CallModuleFunc(ModuleType.MAIN_CITY,"TranslateSystemIDToEntryID",Macros.Game.SystemID.HERO_LINEUP)
		local entry_info = CPPGameLib.GetConfig("CityIcon",city_icon_id)
		self.__btn_lineup = Util:Button(Resource.PathTool.GetSystemEntryIconPath(entry_info.res_name), function()
			CallModuleFunc(ModuleType.HERO,"OpenView", BaseViewType.HERO_LINEUP)
		end)
		panel_systementry:AddChild(self.__btn_lineup)
		self.__btn_lineup:SetPosition(150,48)
		local label = Util:Label(CPPGameLib.GetString("dungeon_lineup_btn"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
		self.__btn_lineup:AddChild(label)
		PosTool.CenterBottom(label,0,12)

		--红点
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.__entry_red_dot:SetAnchorPoint(1, 1)
		self.__btn_lineup:AddChild(self.__entry_red_dot,1)
		PosTool.RightTop(self.__entry_red_dot)
		self:__UpdateLineupBtnRedDot()	

		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_GAMELEVEL_HERO_LINEUP, self.__btn_lineup)			
	end	

	--聊天
	if not self.__btn_chat then
		self.__btn_chat = self:GetWidget("Button_chat")
		Util:WidgetButton(self.__btn_chat, "",function ()
	        CallModuleFunc(ModuleType.CHAT, "OpenView", BaseViewType.CHAT)
		end)
	end

	--功能预告
	is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SYSTEM_FORESHOW)
	local sort_list,is_no_foreshow = CallModuleFunc(ModuleType.RECOMMEND_LINEUP,"SortSystemForeshowList")
	local is_visible = is_open and not is_no_foreshow and true or false
	if not self.__img_foreshow then
		self.__img_foreshow = self:GetWidget("Image_foreshow")
		self.__img_foreshow:SetPosition(self.__img_foreshow:GetPositionX()-20,self.__img_foreshow:GetPositionY())
		self.__img_foreshow:SetScaleFactor(0.8)
	end
	self.__img_foreshow:SetVisible(is_visible)
	if is_visible then
		--排列		
		if self.__system_foreshow_label then
			self.__system_foreshow_label:RemoveSelf(true)
			self.__system_foreshow_label = nil
		end
		if self.__system_foreshow_level_label then
			self.__system_foreshow_level_label:RemoveSelf(true)
			self.__system_foreshow_level_label = nil
		end
		if self.__btn_recommend then
			self.__btn_recommend:RemoveSelf(true)
			self.__btn_recommend = nil
		end
		self.__system_foreshow_label = Util:RichText(string.format(Macros.Color.city_headline_rt, sort_list[1].system_name), Macros.TypefaceSize.largish)
		self.__system_foreshow_label:SetAnchorPoint(0.5,0.5)
		self.__img_foreshow:AddChild(self.__system_foreshow_label)
		PosTool.Center(self.__system_foreshow_label,0,18)	
		--图标
		self.__btn_recommend = Util:Button(Resource.PathTool.GetSystemEntryIconPath(sort_list[1].icon_id), function()
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.SYSTEM_FORESHOW, nil, false)
		end)
		self.__btn_recommend:SetScaleFactor(0.8)		
		self.__img_foreshow:AddChild(self.__btn_recommend)
		PosTool.Center(self.__btn_recommend,-100,10)
		local scale_factor = 90/self.__btn_recommend:GetContentWidth()
		self.__btn_recommend:SetScaleFactor(scale_factor)
		--开放等级
		self.__system_foreshow_level_label = Util:RichText(CPPGameLib.GetString("common_open_lev",tostring(sort_list[1].open_level)), Macros.TypefaceSize.largish,0,0,Macros.Color.golden_hex)
		self.__system_foreshow_level_label:SetAnchorPoint(0.5,0.5)
		self.__img_foreshow:AddChild(self.__system_foreshow_level_label)
		PosTool.Center(self.__system_foreshow_level_label,0,-12)
	end
end

--更新阵容按钮红点
function Modules.DungeonGameLevelView:__UpdateLineupBtnRedDot()
	if self.__entry_red_dot then
		local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.MAX)	
		self.__entry_red_dot:SetVisible(red_num > 0)
		--更新提示
		self:__UpdateLineupBtnTips()
	end
end

--更新阵容按钮红点
function Modules.DungeonGameLevelView:__UpdateLineupBtnTips()
	--提示
	local tips_txt	
	--主界面提示数据列表
	local maincity_tips_data_list = CallModuleFunc(ModuleType.HERO, "GetMainCityLineupHeroRedDotList")
	--遍历寻找提示
	for i,tips_num in ipairs(maincity_tips_data_list) do
		if tips_num > 0 then
			tips_txt = CPPGameLib.GetString("maincity_lineup_tips"..i)
			break
		end
	end
	--阵容
	if tips_txt and not self.__lineup_tips then
		--气泡
		self.__lineup_tips = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"bg_lineup_tips"))
    	self.__lineup_tips:SetAnchorPoint(0.5,0)
    	self.__btn_lineup:AddChild(self.__lineup_tips)
		PosTool.CenterTop(self.__lineup_tips,-10,-5)
		--文字
		self.__lineup_tips_txt = Util:Label("", Macros.TypefaceSize.largish , Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
		self.__lineup_tips:AddChild(self.__lineup_tips_txt)
		PosTool.Center(self.__lineup_tips_txt,0,5)
	end
	--文字
	if self.__lineup_tips_txt then
		self.__lineup_tips_txt:SetString(tips_txt or "")
		self.__lineup_tips:SetVisible(tips_txt and true or false)
	end
end

--更新关卡数据
function Modules.DungeonGameLevelView:__UpdateGameLevelData(cur_gamelevel_index)
	if self.__gamelevel_list then
		local templ_gamelevel = self.__gamelevel_list[cur_gamelevel_index]
		if templ_gamelevel then
			self.__chapter_info = self:CallModuleFunc("GetChapterInfo", self.__chapter_index,self.__dungeon_type)
			templ_gamelevel:RestData({
					chapter_index = self.__chapter_index, gamelevel_index = cur_gamelevel_index, gamelevel_id = self.__config_chapter.gamelevel_list[cur_gamelevel_index],
					gamelevel_info = self.__chapter_info.gamelevel_list[cur_gamelevel_index],dungeon_type = self.__dungeon_type})
		end
	end
end

--宝箱特效
function Modules.DungeonGameLevelView:__SetStarboxImg(index)
	local status = self.__chapter_info.star_award_list[index]
	local starbox_img_map = {"low", "middle", "high"}
	local starbox_effect_map = {"UI_wood_box", "UI_silverybox", "UI_red_box"}
	local imgdata
	local effect
	local widget = self:GetWidget("Image_StarAward"..index)
	if self["starbox_specialeffect_"..index] then
		widget:SetVisible(true)
		self:__DelEffect(self["starbox_specialeffect_"..index])
		self["starbox_specialeffect_"..index] = nil
		self:__DelEffect(self["starbox_receiveeffect_"..index])
		self["starbox_receiveeffect_"..index] = nil
	end	
	if status == Modules.DungeonModule.BoxStatus.NONE then
		imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, starbox_img_map[index].."_box_close")		
	else
		if status == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
			widget:SetVisible(false)
			local star_widget = self:GetWidget("Panel_StarAward"..index) 
			self["starbox_specialeffect_"..index], effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"}, star_widget,1)
			effect:SetPosition(56,47)
			self["starbox_receiveeffect_"..index], effect = self:__CreateEffect({res_name = starbox_effect_map[index], type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE2}, star_widget,2)
			effect:SetPosition(56,26 - math.min(1,index-1) * 5)
			-- PosTool.Center(effect)
		else
			if self.__jump_reward and index == #self.__chapter_info.star_award_list then
				CPPActionManager.StopAllActions(self.__jump_reward)
				self.__jump_reward:RemoveSelf(true)
				self.__jump_reward = nil
			end
		end
		imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, starbox_img_map[index].."_box_open")
	end
	widget:IgnoreContentAdaptWithSize(true)
	widget:SetImage(imgdata)	
	--查找配置
	local reward_config
	if index == #self.__chapter_info.star_award_list then
	 	reward_config = CPPGameLib.GetConfig("DungeonStarReward",self.__chapter_index,false)
	end
	--创建浮动奖励提示
	if not self.__jump_reward and status ~= Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE and reward_config and reward_config.is_starbox_reward == Macros.Global.TRUE and self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON then
		local bubble_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"bg_reward_small")
		local bubble_txt_sp = Resource.PathTool.GetItemPath(reward_config.equip_id)
		self.__jump_reward = Util:Sprite(bubble_bg_sp)
		self:GetNode():AddChild(self.__jump_reward)
		self.__jump_reward:SetPosition(370,90)
		local equip_icon = Util:Sprite(bubble_txt_sp)
		self.__jump_reward:AddChild(equip_icon)
		PosTool.Center(equip_icon,0,5)
		self.funId1 = GlobalCallbackMgr:AddCallBackFunc(function ()
			ActionManager.GetInstance():RunJumpByAction(self.__jump_reward,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
				self:RunAddAction(self.__jump_reward,1)
			end))	
		end)
		self:RunAddAction(self.__jump_reward,1)
	end
	return widget
end

function Modules.DungeonGameLevelView:RunAddAction(node,i)
	ActionManager.GetInstance():RunJumpByAction(node,0.5,0,20,0,0,self["funId"..i])
end

function Modules.DungeonGameLevelView:__InitHeroAdvent()
	local panel_heroadvent = self:GetWidget("Panel_HeroAdvent")
	--审核状态下屏蔽
	if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_ADVENT) then
		panel_heroadvent:SetVisible(false)
		return
	end
	panel_heroadvent:SetVisible(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))
	--英雄降临
	local bt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"hero_icon",true)
	self.__hero_advent_btn = Util:Button(bt_sp,function ()
		CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT)
	end)
	panel_heroadvent:AddChild(self.__hero_advent_btn)
	self.__hero_advent_btn:SetAnchorPoint(0.5,0.5)
	PosTool.Center(self.__hero_advent_btn,-45,0)
	--副本进度字体
	local label = Util:Label(CPPGameLib.GetString("maincity_dungeon_progress"), Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	self.__hero_advent_btn:AddChild(label)
	PosTool.Center(label,0,-42)
	--红点
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__hero_advent_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__hero_advent_red_dot:SetAnchorPoint(1, 1)
	self.__hero_advent_btn:AddChild(self.__hero_advent_red_dot,1)
	self.__hero_advent_red_dot:SetTag(RED_DOT_TAG)
	PosTool.RightTop(self.__hero_advent_red_dot,-30,0)	
	--进度条
	local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"progress_bar")
	local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,"bg_progress_bar")
	self.__dungeon_progressbar = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,101,13,103,15) 
	self.__dungeon_progressbar:SetAnchorPoint(0.5,0.5)
	self.__hero_advent_btn:AddChild(self.__dungeon_progressbar:GetNode())
	PosTool.CenterBottom(self.__dungeon_progressbar,0,10)	
	--进度条字体
	self.__dungeon_progress_txt = Util:Label("", Macros.TypefaceSize.minimum, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	self.__hero_advent_btn:AddChild(self.__dungeon_progress_txt)
	PosTool.CenterBottom(self.__dungeon_progress_txt,0,10)	
	--呼吸效果
	ActionManager:ZoomAnimation(self.__hero_advent_btn,1, 1.02 , 0.98 )
	--初始化
	self:__HandleDungeonReward()
end

--更新英雄奖励
function Modules.DungeonGameLevelView:__HandleDungeonReward()
	--审核状态下屏蔽
	if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_ADVENT) then
		panel_heroadvent:SetVisible(false)
		return
	end
	local dungeonbox_reward_level = self:CallModuleFunc("GetFBPassedBoxRewardLevel")	
	if self.__lighting_effect_id then
		self:__DelEffect(self.__lighting_effect_id)
		self.__lighting_effect_id = nil
	end
	if dungeonbox_reward_level < #CPPGameLib.GetConfig("MainFBChapterReward",nil,nil,true) then
		local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",dungeonbox_reward_level + 1)
		--当前通关章节数
		local passed_chapter_num = self:CallModuleFunc("GetPassedAllChapterNum")
		--按钮图
		local bt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, reward_info.is_equip == Macros.Global.TRUE and "equip_icon" or "hero_icon",true)
		self.__hero_advent_btn:SetNormalImage(bt_sp)
		--进度条
		self.__dungeon_progressbar:SetProgress(math.min(1,(passed_chapter_num/reward_info.need_chapter))*100)
		--进度条文字
		self.__dungeon_progress_txt:SetString(passed_chapter_num.."/"..reward_info.need_chapter)
		--红点
		self.__hero_advent_red_dot:SetVisible(passed_chapter_num >= reward_info.need_chapter)
		if passed_chapter_num >= reward_info.need_chapter then
			if not self.__lighting_effect_id then
				local effect
				self.__lighting_effect_id, effect = self:__CreateEffect({id = 100017,scale = 0.6}, self.__hero_advent_btn,-1)
				effect:SetAnchorPoint(0.5,0.5)
				PosTool.Center(effect)
			end
		end
	else
		self.__hero_advent_btn:SetVisible(false)
	end
end

function Modules.DungeonGameLevelView:FirstOpenGameLevelView()
	--闪烁效果
	CPPActionManager.DelayTo(self.__panel_blink, 0.25, GlobalCallbackMgr:AddCallBackFunc(function ()
		--通关效果
		local effectid,effect = self:__CreateEffect({id = 100030,loop = false,end_callback = function ()			
			--已通关的章节
			local chapter_index = self:CallModuleFunc("GetPassedAllChapterNum")	
			local is_post	
			if not CallModuleFunc(ModuleType.LEAD, "IsInLead") and self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON then								
				--弹窗分优先级								
				local sp_func = function ()
					--是否为升级
					if CallModuleFunc(ModuleType.DUNGEON,"GetIsFightDungeonFBMakeLevelUp") then					
						CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
							self:TheSameAction()						
							CallModuleFunc(ModuleType.ROLE,"OpenView",BaseViewType.ROLEUPGRADE, close_callback,function ()
								if CallModuleFunc(ModuleType.DUNGEON,"GetIsHeroAdventVaild") and not CPPSdkTools.IsInExamine() then
									--弹英雄降临
									CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT,true)						
								else
									self:FireNextFrame(Macros.Event.DungeonModule.AFTER_HERO_ADVENT_POPUP)
								end
							end)
						end, Macros.Game.SpDialogPriority.ROLE_UPGRADE, nil, true)
						CallModuleFunc(ModuleType.DUNGEON,"SetIsFightDungeonFBMakeLevelUp",false)
					else
						if CallModuleFunc(ModuleType.DUNGEON,"GetIsHeroAdventVaild") and not CPPSdkTools.IsInExamine() then
							--弹英雄降临
							CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT,true)
							self:TheSameAction()
						else
							self:FireNextFrame(Macros.Event.DungeonModule.AFTER_HERO_ADVENT_POPUP)
						end
					end					
				end
				--不推送
				if not is_post then
					sp_func()
				end	
			else
				--是否为升级
				if CallModuleFunc(ModuleType.DUNGEON,"GetIsFightDungeonFBMakeLevelUp") then
					CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
						CallModuleFunc(ModuleType.ROLE,"OpenView",BaseViewType.ROLEUPGRADE,close_callback)
					end, Macros.Game.SpDialogPriority.ROLE_UPGRADE, nil, true)
					CallModuleFunc(ModuleType.DUNGEON,"SetIsFightDungeonFBMakeLevelUp",false)
					self:TheSameAction()
				else
					if self.__open_status then
						self:JudgeOpenWhichView(true)
					else
						self:TheSameAction()
					end
				end								
				if self.__lead_player_attack_effectid then
					self:__DelEffect(self.__lead_player_attack_effectid)
					self.__lead_player_attack_effectid = nil
				end			
			end			
		end},self:GetNode(),100)
		PosTool.Center(effect)
	end))	
end

--判断打开哪个界面
function Modules.DungeonGameLevelView:JudgeOpenWhichView(is_new_chapter)
	if not self.__gamelevel_index then
		self:TheSameAction()		
		return
	end
	local cur_gamelevel_index = self.__gamelevel_index	
	if not is_new_chapter then
		cur_gamelevel_index = self.__open_status == Macros.Global.TRUE and (cur_gamelevel_index+1) or cur_gamelevel_index	
	end
	--挑战界面			
	local challenge_func = function ()									
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__config_chapter.gamelevel_list[cur_gamelevel_index])
		--除了普通关卡都请求留言
		local data = {
			chapter_index = self.__chapter_index, gamelevel_index = cur_gamelevel_index, gamelevel_id = self.__config_chapter.gamelevel_list[cur_gamelevel_index],
			gamelevel_info = self.__chapter_info.gamelevel_list[cur_gamelevel_index], dungeon_type = self.__dungeon_type}
		--判断是否是精英怪
		if self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON then
			--打开挑战界面		
			Modules.TemplDungeonGameLevelChallenge.New(data,function ()
				self:TheSameAction()
			end)
			-- if config_gamelevel.icon_type == 1 then
						
			-- else
			-- 	CallModuleFunc(ModuleType.DUNGEON,"MapCommentListReq",data,self.__config_chapter.gamelevel_list[cur_gamelevel_index],function ()
			-- 		self:TheSameAction()
			-- 	end)
			-- end
		else
			--打开挑战界面		
			Modules.TemplDungeonGameLevelChallenge.New(data,function ()
				self:TheSameAction()
			end)
		end
	end
	--助战界面
	local assist_func = function ()		
		--请求助战目标
		CallModuleFunc(ModuleType.BATTLE, "BattleAssistTargetReq", self.__chapter_index, cur_gamelevel_index,self.__dungeon_type,function ()
			self:TheSameAction()
		end)
	end
	--判断该副本能否上阵精灵
	local is_open_spirit
	local could_use_spirit_list = ConfigAdapter.Common.GetCouldUseSpiritDungeonTypeList()
	for i,fb_type in ipairs(could_use_spirit_list) do
		if fb_type == self.__dungeon_type then
			is_open_spirit = true
			break
		end
	end	
	if is_open_spirit then
		if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT) then			
			assist_func()
		else
			challenge_func()
		end
	else
		challenge_func()
	end
end

--
function Modules.DungeonGameLevelView:JudgeToAction()
	--首次打开
	if self.__is_first_open then
		self.__panel_shield:SetVisible(not CallModuleFunc(ModuleType.LEAD, "IsInLead") and true or false)
		self:FirstOpenGameLevelView()
	else
		--是否为升级
		if CallModuleFunc(ModuleType.DUNGEON,"GetIsFightDungeonFBMakeLevelUp") then
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
				CallModuleFunc(ModuleType.ROLE,"OpenView",BaseViewType.ROLEUPGRADE, close_callback)
			end, Macros.Game.SpDialogPriority.ROLE_UPGRADE, nil, true)
			CallModuleFunc(ModuleType.DUNGEON,"SetIsFightDungeonFBMakeLevelUp",false)
		end	
		--打开界面
		if self.__open_status then
			if self.__gamelevel_index and #self.__config_chapter.gamelevel_list == self.__gamelevel_index and self.__open_status == Macros.Global.TRUE then
				self.__weaklead_gamelevel_index = nil
				CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1,1,nil,nil,nil,Macros.Global.FALSE,self.__battle_end_func)
			else
				self:JudgeOpenWhichView(is_new_chapter)
			end			
		end
	end
end

function Modules.DungeonGameLevelView:TheSameAction()
	self.__is_first_passed_all = false
	self.__panel_shield:SetVisible(false)
	self.__is_first_open = false
	if self.__battle_end_func then
		self.__battle_end_func()
		self.__battle_end_func = nil
	end
end

function Modules.DungeonGameLevelView:GetBossGameLevelWorldStarEffectID()
	if not self.__gamelevel_list then
		return
	end
	local templ_gamelevel = self.__gamelevel_list[#self.__gamelevel_list]
	if not templ_gamelevel then
		return
	end

	return templ_gamelevel:GetWorldStarEffectID()
end

--创建弹幕
function Modules.DungeonGameLevelView:__CreateBulletScreen() 	
	self.__bullet_screen_label_list = {}
	self.__count_num = 0
	self:CreateBulletScreenLabel()
end

--删除弹幕
function Modules.DungeonGameLevelView:__DeleteBulletScreen() 
	if self.__bullet_screen_label_list then
		CPPActionManager.StopAllActions(self.__btn_message_board)
		for _,label in pairs(self.__bullet_screen_label_list) do
			CPPActionManager.StopAllActions(label)
			label:RemoveSelf(true)
		end
		self.__bullet_screen_label_list = nil
	end
end

--随机时间创建
function Modules.DungeonGameLevelView:CreateBulletScreenLabel() 
	if not self.__messageboard_data then
		return
	end
	local tips_info = CPPGameLib.GetConfig("GameLevelTips",self.__chapter_index)
	if self.__count_num < (#self.__messageboard_data + (tips_info and #tips_info.tips_list or 0)) then
		self:CreateBulletLabel()
		local randomNum = MathTool.GetRandom(1,3)
		CPPActionManager.DelayTo(self.__btn_message_board,randomNum,self.__message_board_funId)
	end	
end

--创建字幕
function Modules.DungeonGameLevelView:CreateBulletLabel() 
	self.__count_num = self.__count_num + 1
	local tips_info = CPPGameLib.GetConfig("GameLevelTips",self.__chapter_index)
	local randomNum = MathTool.GetRandom(1,#self.__messageboard_data + (tips_info and #tips_info.tips_list or 0))
	while(self.__bullet_screen_label_list[randomNum] ~= nil) do
		randomNum = MathTool.GetRandom(1,#self.__messageboard_data + (tips_info and #tips_info.tips_list or 0))	
	end	
	local txt,txt_color,txt_stroke_color
	if randomNum > #self.__messageboard_data then
		local tips_info = CPPGameLib.GetConfig("GameTipsList",tips_info.tips_list[randomNum - #self.__messageboard_data]) 
		txt = tips_info.text
		txt_color = Macros.Color.lilyWhite
		txt_stroke_color = Macros.Color.common_stroke
	else
		txt = self.__messageboard_data[randomNum].text
		txt_color = randomNum <= 3 and Macros.Color.red or Macros.Color.lilyWhite
		txt_stroke_color = Macros.Color.red_stroke
	end					 
	self.__bullet_screen_label_list[randomNum] = Util:Label(txt, Macros.TypefaceSize.largish,txt_color,0,0,txt_stroke_color)
	self.__bullet_screen_label_list[randomNum]:SetAnchorPoint(1,0.5)
	local random_pos = MathTool.GetRandom(Macros.Global.DesignSize.height/2,Macros.Global.DesignSize.height - 30)
	self.__bullet_screen_label_list[randomNum]:SetPosition(Macros.Global.DesignSize.width-20+self.__bullet_screen_label_list[randomNum]:GetContentWidth(),random_pos)
	self:GetNode():AddChild(self.__bullet_screen_label_list[randomNum],5)
	CPPActionManager.MoveTo(self.__bullet_screen_label_list[randomNum],10,-5,random_pos)
end

function Modules.DungeonGameLevelView:__InitMessageBoard()
	if CPPSdkTools.IsInExamine() then
		return
	end

	local panel_systementry = self:GetWidget("Panel_SystemEntry")
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	--留言
	if role_level >= ConfigAdapter.Common.GetMessageBoardOpenLevel() and not self.__btn_message_board and self.__dungeon_type == Macros.Game.FBType.MAIN_DUNGEON then
		--是否有关闭弹幕
		local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.MessageBoard)) ~= "0"
		if flag then
			--事件id
			self.__message_board_funId = GlobalCallbackMgr:AddCallBackFunc(function ()
				self:CreateBulletScreenLabel()
			end)
			--请求数据
			CallModuleFunc(ModuleType.DUNGEON,"MapCommentListReq",self.__chapter_index)
		end
		--按钮
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, "chat")
		self.__btn_message_board = Util:Button(sp, function()
			--删除弹幕
			self:__DeleteBulletScreen()
			--打开留言界面
			Modules.TemplMessageBoard.New(self.__chapter_index)
		end)
		panel_systementry:AddChild(self.__btn_message_board)
		self.__btn_message_board:SetPosition(-160,48)
		local label = Util:Label(CPPGameLib.GetString("dungeon_message_txt"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
		self.__btn_message_board:AddChild(label)
		PosTool.CenterBottom(label,0,-8)
	end
end

--处理IPhoneX刘海遮挡问题
function Modules.DungeonGameLevelView:__HandleIPhoneXAdaptation()
    if Macros.Global.IphoneSafeAreaRect then
        local safe_area_origin = Macros.Global.IphoneSafeAreaRect.origin
        --左箭头
        if self.__button_left then
	        self.__button_left:SetPosition(safe_area_origin.x, self.__button_left:GetPositionY())
	    end
	    --右箭头
        if self.__button_right then
	        self.__button_right:SetPosition(safe_area_origin.x+Macros.Global.IphoneSafeAreaRect.size.width, self.__button_right:GetPositionY())
	    end
    end
end