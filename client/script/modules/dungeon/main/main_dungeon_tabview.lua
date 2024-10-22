
--[[
%% @module: 主线副本界面
%% @author: swordman sue
%% @created: 2016-10-27
--]]

Modules = Modules or {}

Modules.MainDungeonTabView = Modules.MainDungeonTabView or BaseClass(BaseTabView)

function Modules.MainDungeonTabView:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_CHAPTER)	    
end

--选项卡界面创建完成回调
function Modules.MainDungeonTabView:__LoadedCallback()
	--创建章节列表
    local panel_chapters = self:GetWidget("Panel_Chapters")
    local item_w = 214
    local item_s_c = 10


    local class = Modules.TemplDungeonChapter

    local params = 
    {
        item_class = class ,
        item_width = item_w,
        item_height = 400 ,
        row = 1,
        col = 5,
        item_space_c = item_s_c,
        view_width = 1110, 
        horizon = true,
        selectable = true,
        select_callback = function(item_index, item_data)
        	local item = self.__chapters_scrollview:GetItem(item_index)
        	if not item then
        		return
        	end
        	--设置当前打开章节       	
			CPPActionManager.ScaleTo(item:GetNode(), 0.25,1.2,GlobalCallbackMgr:AddCallBackFunc(function()
				self:CallModuleFunc("OpenView", BaseViewType.DUNGEON_GAMELEVEL, item_data.index)
			end))				       	
        end,
    }
    self.__chapters_scrollview = GUI.SpecialScrollView.New(params)
    panel_chapters:AddChild(self.__chapters_scrollview:GetNode())
    PosTool.Center(self.__chapters_scrollview)
    self.__chapters_scrollview:SetScissorSize(nil, Macros.Global.DesignSize.height)
    
	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self.__chapters_scrollview:ItemIter(function(item_index, item)
			item:CheckSectionOperateSP()
		end)
	end)
	
	--英雄降临容器
	local panel_heroadvent = self:GetWidget("Panel_HeroAdvent")
	--审核状态下屏蔽
	if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_ADVENT) then
		panel_heroadvent:SetVisible(false)
		return
	end
	--英雄降临
	local bt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON,"hero_icon",true)
	self.__hero_advent_btn = Util:Button(bt_sp,function ()
		CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT)
	end)
	panel_heroadvent:AddChild(self.__hero_advent_btn)
	self.__hero_advent_btn:SetAnchorPoint(0.5,0.5)
	PosTool.Center(self.__hero_advent_btn,-45,20)
	--副本进度字体
	local label = Util:Label(CPPGameLib.GetString("maincity_dungeon_progress"), Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	self.__hero_advent_btn:AddChild(label)
	PosTool.Center(label,0,-42)
	--红点
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__entry_red_dot:SetAnchorPoint(1, 1)
	self.__hero_advent_btn:AddChild(self.__entry_red_dot,1)
	self.__entry_red_dot:SetTag(RED_DOT_TAG)
	PosTool.RightTop(self.__entry_red_dot,-30,0)	
	--进度条
	local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON,"progress_bar")
	local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON,"bg_progress_bar")
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

	--领取主线副本章节奖励
	self:BindGlobalEvent(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD, function(_,data)
		self:__HandleDungeonReward()
	end)
end

--选项卡界面打开回调
function Modules.MainDungeonTabView:__OpenCallback()
	self.__passed_all_dungeontype = unpack(self.__open_data)	
	if not self.__is_data_inited then
		self:__HandleDungeonData()
	else
		self:__LocateChapter()
	end
end

--选项卡界面关闭回调
function Modules.MainDungeonTabView:__CloseCallback()
	self.__chapters_scrollview:UnSelectItem()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.MainDungeonTabView:__Dispose()
	self.__is_data_inited = false

	if self.__chapters_scrollview then
		self.__chapters_scrollview:DeleteMe()
		self.__chapters_scrollview = nil
	end	
end

--处理副本数据
function Modules.MainDungeonTabView:__HandleDungeonData()
	local chapter_show_list = self:CallModuleFunc("GetChapterShowList")
	if not chapter_show_list then
		return
	end
	self.__is_data_inited = true
    self.__chapters_scrollview:SetDataList(chapter_show_list)
    self:__HandleDungeonReward()
    self:__LocateChapter()
end

--更新英雄奖励
function Modules.MainDungeonTabView:__HandleDungeonReward()
	--英雄降临容器
	local panel_heroadvent = self:GetWidget("Panel_HeroAdvent")
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
		local bt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, reward_info.is_equip == Macros.Global.TRUE and "equip_icon" or "hero_icon",true)
		self.__hero_advent_btn:SetNormalImage(bt_sp)
		--进度条
		self.__dungeon_progressbar:SetProgress(math.min(1,(passed_chapter_num/reward_info.need_chapter))*100)
		--进度条文字
		self.__dungeon_progress_txt:SetString(passed_chapter_num.."/"..reward_info.need_chapter)
		--红点
		self.__entry_red_dot:SetVisible(passed_chapter_num >= reward_info.need_chapter)
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

--定位章节(居中最新章节)
function Modules.MainDungeonTabView:__LocateChapter()
	local chapter_list = self:CallModuleFunc("GetChapterList")

	--设置解锁副本的章数
	self.__chapters_scrollview:SetNecessaryData(#chapter_list)

	--将索引为Index的条目移动到屏幕中心
	local chapter_index = self:CallModuleFunc("GetOpenChapterIndex", Macros.Game.FBType.MAIN_DUNGEON)
	self.__chapters_scrollview:MoveItemToScreenCenterByIndex(chapter_index)	
	--默认选中上次章节
	self.__chapters_scrollview:SelectItem(chapter_index)	

	--设置条目选择过渡效果的完成回调
	self.__chapters_scrollview:SetSelectItemEffectEndCallback(function()
		--新手指引特殊操作检测
		local selected_item = self.__chapters_scrollview:GetSelectedItem()
		if selected_item then
			selected_item:CheckSectionOperateSP()
		end
	end)

	--弹出英雄降临界面
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local flag = Cookies.Get(Cookies.LevelType.ROLE,Macros.Setting.PopHeroAdventView) ~= "1"
	if role_level <= ConfigAdapter.Common.GetFirstOpenDungeonPopHeroAdventLevel() and not CallModuleFunc(ModuleType.LEAD, "IsInLead") and flag and not CPPSdkTools.IsInExamine() then
		CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.HERO_ADVENT)
		Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.PopHeroAdventView, "1")
	end
	-- --通关效果
	-- if self.__passed_all_dungeontype and self.__passed_all_dungeontype == Macros.Game.FBType.MAIN_DUNGEON and chapter_index < #GameConfig.MainFuben and not self.__have_passed_all then
	-- 	self.__have_passed_all = true
	-- 	if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
	-- 		--非指引过程中，添加屏蔽层
	-- 		self.__parent_view:GetWidget("Panel_shield"):SetVisible(true)
	-- 		self.__parent_view:SetPassedChapterShield(true)
	-- 	end
	-- 	self.__chapters_scrollview:ChapterPassedAllEffect(#chapter_list,function ()
	-- 		local effectid,effect = self:__CreateEffect({id = 100030,loop = false,end_callback = function ()
	-- 			self.__parent_view:GetWidget("Panel_shield"):SetVisible(false)
	-- 			self.__parent_view:SetPassedChapterShield(false)
	-- 			if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
	-- 				--非指引过程中，添加屏蔽层
	-- 				local chapter_index = self:CallModuleFunc("GetPassedAllChapterNum")
	-- 				self:FireNextFrame(Macros.Event.DungeonModule.HERO_ADVENT_POPUP,chapter_index)
	-- 			end
	-- 		end},self.__root_widget,2)
	-- 		PosTool.Center(effect)
	-- 	end)
	-- else
	-- 	if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
	-- 		--非指引过程中，添加屏蔽层
	-- 		local chapter_index = self:CallModuleFunc("GetPassedAllChapterNum")
	-- 		self:FireNextFrame(Macros.Event.DungeonModule.HERO_ADVENT_POPUP,chapter_index)
	-- 	end
	-- end
end
