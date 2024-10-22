
--[[
%% @module: 精英副本界面
%% @author: swordman sue
%% @created: 2016-10-28
	
--]]

Modules = Modules or {}

Modules.EliteDungeonTabView = Modules.EliteDungeonTabView or BaseClass(BaseTabView)


function Modules.EliteDungeonTabView:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_CHAPTER)	    
end

--选项卡界面创建完成回调
function Modules.EliteDungeonTabView:__LoadedCallback()
	--创建章节列表
    local panel_chapters = self:GetWidget("Panel_Chapters")
    local item_w = 214
    local item_s_c = 10

    local class = Modules.TemplEliteDungeonChapter

    local params = 
    {
        item_class = class ,
        item_width = item_w ,
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
				self:CallModuleFunc("OpenView", BaseViewType.ELITE_DUNGEON_GAMELEVEL, item_data.index)
			end))				       	
        end,
    }
    
    --精英副本回调
	self:BindGlobalEvent(Macros.Event.DungeonModule.ELITE_DUNGEON_DATA_RESP, function()									
		if self:IsOpen() then		
			self.__chapters_scrollview = GUI.SpecialScrollView.New(params)
	    	panel_chapters:AddChild(self.__chapters_scrollview:GetNode())
	    	PosTool.Center(self.__chapters_scrollview)			

			self:__HandleDungeonData()
		end
	end)  

	--外域入侵
	self.__button_inbreak = self:GetWidget("Button_inbreak")
	self.__button_inbreak:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_monster"),TextureResType.LOCAL)
	WidgetUnity.SetWidgetListener(self.__button_inbreak, nil, nil,function ()
		local extra_data = self:CallModuleFunc("GetExtraInfo",Macros.Game.FBType.ELITE_DUNGEON)
		Modules.TemplDungeonExternalAreaView.New(extra_data)
	end)
	local text_inbreak = self:GetWidget("Text_inbreak")
	text_inbreak:SetPosition(text_inbreak:GetPositionX(),text_inbreak:GetPositionY()+ 10)
	Util:WidgetLabel(text_inbreak, CPPGameLib.GetString("dungeon_external_area"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
end

--选项卡界面打开回调
function Modules.EliteDungeonTabView:__OpenCallback()
	self.__passed_all_dungeontype,self.__is_open_breakview = unpack(self.__open_data)		
	if self.__is_data_inited then
		self:__LocateChapter()
	else
		--请求精英副本数据
		self:CallModuleFunc("FBEliteDataReq")
	end
end

--选项卡界面关闭回调
function Modules.EliteDungeonTabView:__CloseCallback()
	if self.__chapters_scrollview then
		self.__chapters_scrollview:UnSelectItem()
	end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.EliteDungeonTabView:__Dispose()
	self.__is_data_inited = false

	if self.__chapters_scrollview then
		self.__chapters_scrollview:DeleteMe()
		self.__chapters_scrollview = nil
	end	
end

--处理副本数据
function Modules.EliteDungeonTabView:__HandleDungeonData()
	local chapter_show_list = self:CallModuleFunc("GetChapterShowList",Macros.Game.FBType.ELITE_DUNGEON)
	if not chapter_show_list then
		return
	end
	self.__is_data_inited = true
    self.__chapters_scrollview:SetDataList(chapter_show_list)
    self:__LocateChapter()
end

--定位章节(居中最新章节)
function Modules.EliteDungeonTabView:__LocateChapter()
	local chapter_list = self:CallModuleFunc("GetChapterList",Macros.Game.FBType.ELITE_DUNGEON)

	--设置解锁副本的章数
	self.__chapters_scrollview:SetNecessaryData(#chapter_list)

	
	--默认选中上次章节
	local chapter_index = self:CallModuleFunc("GetOpenChapterIndex", Macros.Game.FBType.ELITE_DUNGEON)
	--将索引为Index的条目移动到屏幕中心
	self.__chapters_scrollview:MoveItemToScreenCenterByIndex(chapter_index)	
	self.__chapters_scrollview:SelectItem(chapter_index)	

	--设置条目选择过渡效果的完成回调
	self.__chapters_scrollview:SetSelectItemEffectEndCallback(function()
		--新手指引特殊操作检测
		local selected_item = self.__chapters_scrollview:GetSelectedItem()
		if selected_item then
			selected_item:CheckSectionOperateSP()
		end
	end)

	--外域入侵
	local extra_data = self:CallModuleFunc("GetExtraInfo",Macros.Game.FBType.ELITE_DUNGEON)
	self.__button_inbreak:SetVisible(#extra_data > 0)

	if self.__is_open_breakview and not self.__have_open_breakview then
		self.__have_open_breakview = true
		Modules.TemplDungeonExternalAreaView.New(extra_data)

		--刷新主界面
		CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.ELITE_DUNGEON_BREAK)
	end

	--通关效果
	if self.__passed_all_dungeontype and self.__passed_all_dungeontype == Macros.Game.FBType.ELITE_DUNGEON and chapter_index < #CPPGameLib.GetConfig("EliteFBChapter",nil,nil,true) and not self.__have_passed_all then
		self.__have_passed_all = true
		if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			--非指引过程中，添加屏蔽层
			self.__parent_view:GetWidget("Panel_shield"):SetVisible(true)
			self.__parent_view:SetPassedChapterShield(true)
		end
		self.__chapters_scrollview:ChapterPassedAllEffect(#chapter_list,function ()
			local effectid,effect = self:__CreateEffect({id = 100030,loop = false,end_callback = function ()
				self.__parent_view:GetWidget("Panel_shield"):SetVisible(false)
				self.__parent_view:SetPassedChapterShield(false)
			end},self.__root_widget,2)
			PosTool.Center(effect)
		end)
	end
end
