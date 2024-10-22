Modules = Modules or {}

Modules.HeroHandBookMainView = Modules.HeroHandBookMainView or BaseClass(BaseView)
local HeroHandBookType = {
	SuperPower = 0, --超能
	Legend = 1,		--传说
	Fantasy = 2,	--幻想
}	

function Modules.HeroHandBookMainView:__init()
	self.__layout_name = "hero_handbook"
	self.__bg_params = {type = BaseView.BGType.IMAGE,image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()
		end,
		title = CPPGameLib.GetString("hero_handbook_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy	= true,
	}

	local tab_sp1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigwhite")
	local tab_sp2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigyellow")
	self.__tab_params = {
		{tab_sp1, tab_sp2},
		{
			CPPGameLib.GetString("hero_handbook_buttonType_0"),
			CPPGameLib.GetString("hero_handbook_buttonType_1"),
			CPPGameLib.GetString("hero_handbook_buttonType_2"),
        }, 
        10, 
        CPPGameLib.Handler(self, self.heroTypeChoose),
        true
    }

	self.__open_callback = function ()
		self:__OpenCallback()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_HANDBOOK)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_HANDBOOK,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_MAINVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON)
end

function Modules.HeroHandBookMainView:__OpenCallback()
	local tab_index = unpack(self.__open_data) or HeroHandBookType.Legend
	--关闭界面事件
	self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	        self:CloseManual()
	    end)
	--进度条文字
	self.hero_handbookProgressText = self:GetWidget("Text_loadingBar") 	
	Util:WidgetLabel(self.hero_handbookProgressText,"",Macros.TypefaceSize.normal,Macros.Color.headline )
	--进度条
	local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview_bg")
	local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview")
	self.hero_handbookProgressBar = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,470,21,470,25) 
	self.hero_handbookProgressBar:SetAnchorPoint(0.5,0.5)
	self.hero_handbookProgressBar:SetPosition(555,530)
	self:GetWidget("Panel_mainView"):AddChild(self.hero_handbookProgressBar:GetNode())
	
	--进度条进度文字
	self.Text_progress = self:GetWidget("Text_progress")
	self.Text_progress:SetZOrder(1)
	Util:WidgetLabel(self.Text_progress,"",Macros.TypefaceSize.normal,Macros.Color.btn)
	--初始化列表容器
	for HeroHandBookType=1,3 do
		self["ListView_scrollview_"..HeroHandBookType] = self:GetWidget("ListView_scrollview_"..HeroHandBookType) 
	end
	--获取收集列表
	self.CollectionList = self:CallModuleFunc("GetCollectionList")
	--初始化选项卡、子页面
	local widget = self:GetWidget("Panel_tab")
	widget:AddChild(self.__tab:GetNode())
	self.__tab:SetDirection(true,0)
    PosTool.LeftTop(self.__tab,0,0)
    --跳转阵容推荐界面
    WidgetUnity.SetWidgetListener(self:GetWidget("Button_recommend"), nil, nil,function ()
    	self:CloseManual()
    	CallModuleFunc(ModuleType.RECOMMEND_LINEUP,"OpenView",BaseViewType.RECOMMEND_LINEUP,self.__cur_selected_index)
    end)
    self:GetWidget("Button_recommend"):SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_lineup"),TextureResType.LOCAL)
    Util:WidgetLabel(self:GetWidget("Text_recommend"),CPPGameLib.GetString("hero_handbookRecommendText"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0,Macros.Color.button_yellow) 
    --阵容说明
    self.__text_introduce = self:GetWidget("Text_introduce")
    Util:WidgetLabel(self.__text_introduce, "", Macros.TypefaceSize.normal, Macros.Color.content, 485, 50)   
	--跳转初始页面
	self.__tab:On(tab_index)		
end

function Modules.HeroHandBookMainView:heroTypeChoose(tab_index)
	self.__cur_selected_index = tab_index
	--推荐阵容说明
	local lineup_info = CPPGameLib.GetConfig("HeroHandBook",tab_index - 1)	
	local max_quality = table.maxn(lineup_info.hero_list)
	self.__text_introduce:SetText(lineup_info.hero_list[max_quality].camp_des)
	--阵营英雄进度
	self.hero_handbookProgressText:SetText(CPPGameLib.GetString("hero_handbookProgressText",CPPGameLib.GetString("hero_camp_"..(tab_index - 1))))
	
	--设置进度条
	local totalNum = 0
	local totalHaveNum = 0
	for i=1,max_quality do
		totalNum = totalNum + #lineup_info.hero_list[i].hero_list
		for key,hero_id in ipairs(lineup_info.hero_list[i].hero_list) do
			if self.CollectionList[hero_id] then
				totalHaveNum = totalHaveNum + 1
			end
		end
	end
	self.Text_progress:SetText(totalHaveNum.."/"..totalNum)
	self.hero_handbookProgressBar:SetProgress((totalHaveNum/totalNum)*100)
	--隐藏其他
	local stroke_width = 2
	for HeroHandBookType=1,3 do
		self["ListView_scrollview_"..HeroHandBookType]:SetVisible(false)
		-- self.__tab.btn_list[HeroHandBookType]:DisableEffect()		
		--self.__tab.btn_list[HeroHandBookType]:SetText(CPPGameLib.GetString("hero_handbook_buttonType_"..(HeroHandBookType -1)))
		-- self.__tab.btn_list[HeroHandBookType]:SetTextColor(unpack(HeroHandBookType == tab_index and Macros.Color.btn or Macros.Color.content))
		-- local r, g, b, a = unpack(Macros.Color.button_yellow)
		-- self.__tab.btn_list[HeroHandBookType]:SetOutline(r, g, b, HeroHandBookType == tab_index and a or 0, HeroHandBookType == tab_index and stroke_width or 1)
	end
	if not self["is_istView_scrollview_init"..tab_index] then
		self["is_istView_scrollview_init"..tab_index] = true 
		self["templList_"..tab_index] = {}
		for i=max_quality,1,-1 do 
			local panel = GUI.TemplHandBookMainView.New(lineup_info.hero_list[i].hero_list,i)
			table.insert(self["templList_"..tab_index],panel)
			self["ListView_scrollview_"..tab_index]:PushBackCustomItem(panel:GetNode())
		end	
		--添加滚动回调
		-- WidgetUnity.SetScrollListener(self["ListView_scrollview_"..tab_index], function ()
		-- 	print("ldx","1111111111111111111111111111111111111111 ")
		-- end)
	end
	self["ListView_scrollview_"..tab_index]:SetVisible(true)
end

function Modules.HeroHandBookMainView:__Dispose()	
	for tab_index=1,3 do
		if self["templList_"..tab_index] then
			for _,panel in ipairs(self["templList_"..tab_index]) do
				panel:DeleteMe()			
			end
			self["templList_"..tab_index] = nil
		end
	end
end