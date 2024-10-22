
--[[
%% @module: 英雄副本关卡界面
%% @author: swordman sue
%% @created: 2016-10-28
--]]

Modules = Modules or {}

Modules.HeroDungeonGameLevelView = Modules.HeroDungeonGameLevelView or BaseClass(BaseView)

local dst_bg_width = 1386
local src_bg_width = 937

function Modules.HeroDungeonGameLevelView:__init()
	self.__layout_name = "dungeon_gamelevel"	
	self.__is_ignore_iphone_safe_area = true
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("dungeon_hero_gamelevel"),
		show_stamina = true,
		show_diamond = true,
		show_power = true,
		show_level = true,
		close_func = function()
			if self.__is_first_passed_all then
				return
			end
			local top_stack_info = GlobalViewMgr:GetTopStackInfo()
			if not top_stack_info then
				self:CallModuleFunc("OpenView", BaseViewType.DUNGEON, BaseViewSubType.DUNGEON.HERO, function()
					GlobalViewMgr:ClearStackInfo()
					self:CloseManual(true)
				end)
			else
				self:CloseManual()
			end
		end,
	}

	self.__loaded_callback = function ()
		--跳章按钮
		--后退
		self.__button_left = self:GetWidget("Button_left")	
		self.__button_left:SetAnchorPoint(0,0.5)
		Util:WidgetButton(self.__button_left,"",function ()
			CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index - 1)
		end)
		--前进
		self.__button_right = self:GetWidget("Button_right")
		self.__button_right:SetAnchorPoint(1,0.5)		
		Util:WidgetButton(self.__button_right,"",function ()
			CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1)
		end)

		--处理iPhoneX的适配
		self:__HandleIPhoneXAdaptation()

		--叛军
		local widget = self:GetWidget("Panel_Rebel")
		widget:SetVisible(false)

		--星级宝箱
		widget = self:GetWidget("Panel_StarAward")
		widget:SetVisible(false)

		--屏蔽层
		self.__panel_shield = self:GetWidget("Panel_shield")
		self.__panel_shield:SetVisible(false)

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
    self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_DUNGEON_GAMELEVEL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL)   
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON) 
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL,true) 
end

function Modules.HeroDungeonGameLevelView:__OpenCallback()
	self.__chapter_index,self.__is_first_passed_all,is_first_win,self.__gamelevel_index,self.__open_status,self.__battle_end_func = unpack(self.__open_data)
	local chapter_list = self:CallModuleFunc("GetChapterList", Macros.Game.FBType.HERO_DUNGEON)
	if not self.__chapter_index then		
		self.__chapter_index = #chapter_list
	end	
	self.__chapter_info = self:CallModuleFunc("GetChapterInfo", self.__chapter_index, Macros.Game.FBType.HERO_DUNGEON)

	--今日可挑战次数
	local extra_data = self:CallModuleFunc("GetExtraInfo", Macros.Game.FBType.HERO_DUNGEON)
	local max_times = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_TRANSCRIPT)
	local remain_times = math.max(0, max_times - extra_data.battle_times)
	widget = self:GetWidget("Text_HeroDungeon_CanChallengeTimes")
	widget:SetText(CPPGameLib.GetString("dungeon_can_challenge_times", remain_times, max_times))
	
	--闪烁效果
	self.__panel_blink:SetVisible(true)
	CPPActionManager.FadeOut(self.__panel_blink, 0.25, GlobalCallbackMgr:AddCallBackFunc(function ()
		self.__panel_blink:SetVisible(false)
		self.__panel_blink:SetOpacity(255)
	end))

	--设置当前打开章节
    self:CallModuleFunc("SetOpenChapterIndex",Macros.Game.FBType.HERO_DUNGEON,self.__chapter_index)
	self.__config_chapter = CPPGameLib.GetConfig("HeroFBChapter", self.__chapter_index)
	self:__InitTitleAndBG()
	self:__InitGameLevels()

	--红点数量
	local before_reddot_num,after_reddot_num = self:CallModuleFunc("GetDungeonRedDotByChapterIndexAndSystemId",Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL,self.__chapter_index)
	--跳章按钮
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

	--章节通关效果
	if is_first_win then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.DUNGEON_FIRSTWIN, false, self.__config_chapter.gamelevel_list[self.__gamelevel_index or 1],function ()
				if self.__is_first_passed_all then
					self.__panel_shield:SetVisible(true)
					self:__ChapterPassedAllEffect(close_callback)
				else
					if close_callback then
						close_callback()
					end
					--打开界面
					self:JudgeOpenWhichView()
				end
			end)
		end, Macros.Game.SpDialogPriority.CHAPTER_PASSED_ALL)
	else
		--打开界面
		self:JudgeOpenWhichView()
	end
end

function Modules.HeroDungeonGameLevelView:GetOpenParams()
	return {self.__chapter_index}
end

function Modules.HeroDungeonGameLevelView:__Dispose()
	CPPGameLib.ClearObjList(self.__gamelevel_list)
	self.__gamelevel_list = nil		
	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end
end

function Modules.HeroDungeonGameLevelView:__RegistAllEvents()
    self:__InitLevelGiftInfo()  
end

function Modules.HeroDungeonGameLevelView:__InitTitleAndBG()
	--章节名
	if not self.__rt_chapter_name then
		local img_chapter_name_bg = self:GetWidget("Image_ChapterNameBG")
		self.__rt_chapter_name = Util:RichText(string.format(Macros.Color.dungeon_chapter_name_rt_gradient, self.__config_chapter.name), Macros.TypefaceSize.popup)
		self.__rt_chapter_name:SetAnchorPoint(0, 0.5)
		img_chapter_name_bg:AddChild(self.__rt_chapter_name)
		PosTool.LeftCenter(self.__rt_chapter_name, 10)
	else
		local lab = GUI.RichText.GetColorWord(string.format(Macros.Color.dungeon_chapter_name_rt_gradient, self.__config_chapter.name))
		self.__rt_chapter_name:SetText(lab)
	end

	--背景
	if not self.__scroll_node then
		self.__scroll_node = self:GetWidget("ScrollView_Node")
		self.__scroll_node:SetScrollDirection(UIScrollViewEx.ScrollDirHorizontal)
		self.__scroll_node:SetInnerContainerSize(dst_bg_width, 0)
		self.__scroll_node:SetTouchEnabled(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))
		GUI.MultiResolutionAdapter.AdaptForBG(self.__scroll_node, self:GetNode(), self.__is_force_stretch, self.__is_ignore_iphone_safe_area)
	end

	local chapter_bg_id = ConfigAdapter.Dungeon.GetChapterBGID(self.__chapter_index, Macros.Game.FBType.HERO_DUNGEON)
	local sp = Resource.PathTool.GetUIResPath("dungeon_gamelevel_bg/"..chapter_bg_id, nil, "jpg")
	if not self.__img_bg then		
		self.__img_bg = Util:Sprite(sp)
		self.__img_bg:SetAnchorPoint(0, 0)
		self.__img_bg:SetScaleFactor(dst_bg_width / src_bg_width)
		self.__scroll_node:AddChild(self.__img_bg)
	else
		self.__img_bg:SetImage(sp)
	end
end

function Modules.HeroDungeonGameLevelView:__InitGameLevels()
	CPPGameLib.ClearObjList(self.__gamelevel_list)
	self.__gamelevel_list = {}

	--寻找当前已开启到的关卡
	local open_gamelevel_num = 0

	--额外关卡
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

	--关卡
	for index, id in ipairs(self.__config_chapter.gamelevel_list) do
		--OGL坐标	
		local pos_info = self.__config_chapter.gamelevel_pos_list[index]
		if not pos_info then
			LogTool.LogError("[Modules.HeroDungeonGameLevelView:__InitGameLevels] pos_info无效")
		else
			local templ_gamelevel = Modules.TemplHeroDungeonGameLevel.New(self.__scroll_node)
			CPPGameLib.PrintTable(self.__chapter_info.gamelevel_list[index],"self.__chapter_info.gamelevel_list[index]: ")
			templ_gamelevel:SetPosition(pos_info[1], pos_info[2])
			templ_gamelevel:SetData({
				chapter_index = self.__chapter_index, 
				gamelevel_index = index, gamelevel_id = id, gamelevel_info = self.__chapter_info.gamelevel_list[index]
			})
			--配置
			local config_gamelevel = CPPGameLib.GetConfig("GameLevel",id)
			if not config_gamelevel then
				return
			end
			--关卡点连接
			local posInfo_2 = self.__config_chapter.gamelevel_pos_list[index + 1]
			if open_gamelevel_num == 1 then
				templ_gamelevel:__ShowEffect()
			elseif open_gamelevel_num == 0 then
				if index ~= #self.__config_chapter.gamelevel_list then				
					Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
				end
			else
				if index < open_gamelevel_num - 1 then
					Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
				elseif index == open_gamelevel_num - 1 then		
					local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.HeroDungeonGameLevelEffect, self.__config_chapter.gamelevel_list[index+1])) ~= "1"			
					if flag then
						Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list,true,function ()
							--self.__gamelevel_list[index+1]:GetNode():SetEnabled(true)
							self.__gamelevel_list[index+1]:__ShowEffect()
						end)
					else
						Util:GameLevelConnect(templ_gamelevel:GetNode(),{pos_info[1], pos_info[2]},{posInfo_2[1], posInfo_2[2]},(index + 1) == #self.__config_chapter.gamelevel_list)
					end
				elseif index == open_gamelevel_num then	
					local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.HeroDungeonGameLevelEffect, self.__config_chapter.gamelevel_list[index])) ~= "1"
					--templ_gamelevel:GetNode():SetEnabled(not flag)
					if not flag then
						templ_gamelevel:__ShowEffect()
					end
				end
			end
			-- if self.__gamelevel_index and self.__gamelevel_index == index and not self.__open_status and not self.__is_first_passed_all then
			-- 	local effect
			-- 	self.__lead_player_attack_effectid,effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"},templ_gamelevel:GetNode(),2)
			-- 	PosTool.Center(effect)
			-- end
			table.insert(self.__gamelevel_list, templ_gamelevel)
		end
	end
end

--章节通关
function Modules.HeroDungeonGameLevelView:__ChapterPassedAllEffect(close_callback)
	local effectid,effect = self:__CreateEffect({id = 100029,loop = false,end_callback = function ()
		if close_callback then
			close_callback()
		end
		--打开界面
		self:JudgeOpenWhichView()		
	end},self:GetNode(),3)	
	PosTool.Center(effect)
end


--初始化等级礼包通知
function Modules.HeroDungeonGameLevelView:__InitLevelGiftInfo()
	local Panel_Levelgift = self:GetWidget("Panel_Levelgift")
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

--判断打开哪个界面
function Modules.HeroDungeonGameLevelView:JudgeOpenWhichView(is_new_chapter)
	if not self.__open_status or not self.__gamelevel_index then			
		self:TheSameAction()
		return
	end
	--删除特效
	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end
	local cur_gamelevel_index = self.__gamelevel_index	
	if not is_new_chapter then
		cur_gamelevel_index = self.__open_status == Macros.Global.TRUE and (cur_gamelevel_index+1) or cur_gamelevel_index	
	end
	--检测条件
	local check_conditional = function ()
		local gamelevel_info = self.__chapter_info.gamelevel_list[cur_gamelevel_index]
		if not gamelevel_info then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips2"))
			return
		end

		if gamelevel_info.battle_times > 0 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_has_challenged"))
			return
		end
		--今日可挑战次数
		local extra_data = self:CallModuleFunc("GetExtraInfo", Macros.Game.FBType.HERO_DUNGEON)
		local max_times = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_TRANSCRIPT)
		local remain_times = math.max(0, max_times - extra_data.battle_times)
		if remain_times <= 0 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips3"))
			return
		end
		return true
	end
	--挑战界面			
	local challenge_func = function ()	
		--检测条件
		if not check_conditional() then
			self:TheSameAction()
			return
		end								
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__config_chapter.gamelevel_list[cur_gamelevel_index])
		--除了普通关卡都请求留言
		local data = {
			chapter_index = self.__chapter_index, gamelevel_index = cur_gamelevel_index, gamelevel_id = self.__config_chapter.gamelevel_list[cur_gamelevel_index],
			gamelevel_info = self.__chapter_info.gamelevel_list[cur_gamelevel_index]}
		--打开挑战界面		
		Modules.TemplHeroDungeonGameLevelChallenge.New(data,function ()
			self:TheSameAction()
		end)		
	end
	--助战界面
	local assist_func = function ()	
		--检测条件	
		if not check_conditional() then
			self:TheSameAction()
			return
		end
		--请求助战目标
		CallModuleFunc(ModuleType.BATTLE, "BattleAssistTargetReq", self.__chapter_index, cur_gamelevel_index,Macros.Game.FBType.HERO_DUNGEON,function ()
			self:TheSameAction()
		end)
	end	
	--判断该副本能否上阵精灵
	local is_open_spirit
	local could_use_spirit_list = ConfigAdapter.Common.GetCouldUseSpiritDungeonTypeList()
	for i,fb_type in ipairs(could_use_spirit_list) do
		if fb_type == Macros.Game.FBType.HERO_DUNGEON then
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

function Modules.HeroDungeonGameLevelView:TheSameAction()
	self.__is_first_passed_all = false
	self.__panel_shield:SetVisible(false)
	self.__is_first_open = false
	if self.__battle_end_func then
		self.__battle_end_func()
	end
end

--处理IPhoneX刘海遮挡问题
function Modules.HeroDungeonGameLevelView:__HandleIPhoneXAdaptation()
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