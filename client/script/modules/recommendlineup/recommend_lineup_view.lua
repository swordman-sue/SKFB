--
-- @brief 阵容推荐界面
-- @auther: ldx
-- @date 2017年3月22日 18:21:36
--

Modules = Modules or {}

Modules.RecommendLineupView = Modules.RecommendLineupView or BaseClass(BaseView)

local RECOMMENDLINEUP_TYPE = {
	RECOMMEND_LINEUP = 3,
	SYSTEM_FORESHOW = 4,
}

function Modules.RecommendLineupView:__init()
	self.__layout_name = "recommend_lineup"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()			
		end,
		title = CPPGameLib.GetString("recommend_lineup_title"),
		show_level = true,
		show_power = true,
		show_diamond = true,				
	}

	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse_1")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no_1")
	self.__tab_params = {
		{sp_2, sp_1},
		{
			"",
			"",
			"",
        }, 
        15, 
        CPPGameLib.Handler(self, self.SelectedCampCallBack), 
        false,
    }

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RECOMMENDLINEUP)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RECOMMENDLINEUP,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_RECOMMENDLINEUP_PAGE)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SYSTEM_FORESHOW_CELL)
	self.__open_callback = function ()
		self:__OpenCallback()
	end
end

function Modules.RecommendLineupView:__OpenCallback()
    self.__is_already_have_effect = false
	local selected_index = unpack(self.__open_data)
	selected_index = selected_index or 1
	self.__first_page_listener = true

	--背景
	local image_castleBg = self:GetWidget("Image_castleBg")
	image_castleBg:IgnoreContentAdaptWithSize(true)
	image_castleBg:SetImage(Resource.PathTool.GetUIResPath("bigbg/bg_recommendlineup"),TextureResType.LOCAL)
	--界面位置
	self.__mark_lineup_index = {0,0,0}
	--左边列表
	local btn_res = {"btn_btn_activity_on","btn_btn_activity_no"}
	self.__btn_res = {}
	for _,btn_res_name in ipairs(btn_res) do
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,btn_res_name)
		table.insert(self.__btn_res,sp)
	end	
	--初始化
	local left_namelist = {}
	local left_iconlist = {}
	for i=1,3 do
		local camp_name = CPPGameLib.GetString("hero_handbook_buttonType_"..(i-1))
		table.insert(left_namelist,camp_name)
		local camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..(i-1),true)
		table.insert(left_iconlist,camp_sp)
	end	
	local systemid_list = {}
	for i,systemid in ipairs(systemid_list) do
		local config = CPPGameLib.GetConfig("SystemEntry",systemid)
		table.insert(left_namelist,config.name)
		local sp = Resource.PathTool.GetSystemEntryIconPath(config.team_icon)
		table.insert(left_iconlist,sp)
	end

	self.image_leftbigbg = self:GetWidget("Image_leftbigBg")
	
	local panel_leftlist = self:GetWidget("Panel_leftList")
	self.__btn_list = {}
	self.__page_list = {}
	local offset_y = 0
	local interval = 20
	for i,name_str in ipairs(left_namelist) do
		local btn = Util:Button(self.__btn_res[2],function ()
			self:SetSelectedCallBack(i)
		end)
		btn:SetAnchorPoint(0,1)
		btn:SetContentSize(206, 87)
		btn:IgnoreContentAdaptWithSize(false)
		panel_leftlist:AddChild(btn)
		PosTool.LeftTop(btn,0,offset_y)
		table.insert(self.__btn_list,btn)
		offset_y = offset_y - btn:GetContentHeight() - interval
		--图标
		local icon = Util:Sprite9(left_iconlist[i])
		icon:SetAnchorPoint(0,0.5)
		btn:AddChild(icon)
		PosTool.LeftCenter(icon,10,0)
		--文字
		local label = Util:Label(name_str, 28, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
		label:SetAnchorPoint(0,0.5)
		btn:AddChild(label)
		PosTool.LeftCenter(label,100,0)
	end
	--跳转图鉴
	Util:WidgetButton(self:GetWidget("Button_lineup"),"",function ()
		self:CloseManual(true)
		--跳转图鉴
		CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK)
	end)
	self:GetWidget("Button_lineup"):SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_handbook"),TextureResType.LOCAL)
	--翻页容器
	self.__pageview_lineup = self:GetWidget("PageView_lineup")
	WidgetUnity.SetPageViewListener(self.__pageview_lineup,function ()
		local page = self.__pageview_lineup:GetCurPageIndex()
		self:PageviewScrollListener()
	end)
	--后退
    WidgetUnity.SetWidgetListener(self:GetWidget("Button_left"), nil, nil,function ()
    	self.__is_shield_touch = true
   		local page = self.__pageview_lineup:GetCurPageIndex()
   		self.__pageview_lineup:ScrollToPageByTime(page -1,0.5)
    end)
    --前进
    WidgetUnity.SetWidgetListener(self:GetWidget("Button_right"), nil, nil,function ()
    	self.__is_shield_touch = true
   		local page = self.__pageview_lineup:GetCurPageIndex()
   		self.__pageview_lineup:ScrollToPageByTime(page+1,0.5)
    end)
    --图片适配
    self:GetWidget("Image_text"):IgnoreContentAdaptWithSize(true)
    --阵型类型   
    self:GetWidget("Image_28"):SetVisible(false)
    self.__text_lineuptype = self:GetWidget("Text_lineupType")
    self.__text_lineuptype:SetVisible(false)
    Util:WidgetLabel(self.__text_lineuptype,"", 28, Macros.Color.Special, 0, 0, Macros.Color.common_stroke)
    --阵型说明
    Util:WidgetLabel(self:GetWidget("Text_introduce"),CPPGameLib.GetString("recommend_lineup_introduce"), Macros.TypefaceSize.normal, Macros.Color.blinkcontent )
    --阵型详细说明
    self.__text_introducedetail = self:GetWidget("Text_introduceDetail")
 	Util:WidgetLabel(self.__text_introducedetail,"", Macros.TypefaceSize.normal, Macros.Color.blinkcontent ,686,65)	

	local panel_rightList = self:GetWidget("Panel_rightList")
    self.__tab:SetPosition(0,0)
    self.__tab:SetFontSize(24)
    panel_rightList:AddChild(self.__tab:GetNode())
	
 	--默认打开
 	self:SetSelectedCallBack(selected_index,true)
end

function Modules.RecommendLineupView:PageviewScrollListener()
	local page = self.__pageview_lineup:GetCurPageIndex()
   	self:GetWidget("Button_left"):SetVisible(not (page == 0))
   	self:GetWidget("Button_right"):SetVisible(not (page == (self.__pageview_lineup:GetPageNum() - 1)))

   	--设置文字
   	self:SetPageText(page)
   	--点击设置
   	self.__tab:On(page + 1)
end

--设置文字
function Modules.RecommendLineupView:SetPageText(index)
	local lineup_info = CPPGameLib.GetConfig("HeroRecommendLineup",self.__sp_selected_index - 1)
	self.__text_lineuptype:SetText(lineup_info.recommend_list[index+1].lineup_name)
	self.__text_introducedetail:SetText(lineup_info.recommend_list[index+1].lineup_des)
end

function Modules.RecommendLineupView:SetSelectedCallBack(index)
	if self.__selected_index and self.__selected_index == index then
		return
	end
	self.__selected_index = index
	self.__lineup_type = 1

	local is_recommend_lineup = index <= RECOMMENDLINEUP_TYPE.RECOMMEND_LINEUP

	--移除标签特效
	if self["__effect_id"..index] then
		self:__DelEffect(self["__effect_id"..index])
		self["__effect_id"..index] = nil
	end

	--设置标题
	self:SetTitle(is_recommend_lineup and CPPGameLib.GetString("recommend_lineup_title") or CPPGameLib.GetString("system_foreshow_title"))


	--设置左侧、右侧选项卡显隐
	self.image_leftbigbg:SetVisible(is_recommend_lineup)
	for i,btn in ipairs(self.__btn_list) do
		btn:SetNormalImage(index == i and self.__btn_res[1] or self.__btn_res[2])

		if i <= 2 then
			local widget = self:GetWidget("Panel_right"..i)
			if widget then
				widget:SetVisible((i == 1 and is_recommend_lineup) or (i == 2 and not is_recommend_lineup))
			end
		end
	end	

	--推荐阵容
	if is_recommend_lineup then
		self.__sp_selected_index = index
		self.__tab:SetVisible(true)
		local lineup_info = CPPGameLib.GetConfig("HeroRecommendLineup",self.__sp_selected_index - 1)
		for i=1,#self.__tab.btn_list do
			local btn = self.__tab:GetBtn(i)
			if btn then
				btn:SetText(lineup_info.recommend_list[i].lineup_name or "")
			end
		end
		self.__is_need_init_data = true
		self.__tab.__not_forbidden = true
		self.__tab:On(1)

	--系统功能预告
	else
		self.__tab:SetVisible(false)
		if not self.__system_foreshow_scrollview then
			--右边列表
			local panel_systemforeshowlist = self:GetWidget("Panel_systemForeShowList")
			local params = {
				item_class = Modules.TemplSystemForeShowScrollItem,
			    item_width = 681,
			    item_height	 = 122,
			    row	= 4,
			    col	= 1,
			    item_space_r = 5,
			    view_width = 681,
			    view_height = 503,
			}
			self.__system_foreshow_scrollview = GUI.ScrollView.New(params)
			panel_systemforeshowlist:AddChild(self.__system_foreshow_scrollview:GetNode())
			PosTool.Center(self.__system_foreshow_scrollview)
			local system_foreshow_list = self:CallModuleFunc("SortSystemForeshowList")
			self.__system_foreshow_scrollview:SetDataList(system_foreshow_list)
		end
	end
end

function Modules.RecommendLineupView:SelectedCampCallBack(index)
	self.__lineup_type = index
	self.__is_first_ingore = false

	--屏蔽下面的操作
	if self.__is_shield_touch then
		self.__is_shield_touch = false
		return
	end
	if self.__is_need_init_data then
		local page_num = self.__pageview_lineup:GetPageNum()
		-- local page_index = page_num > 0 and self.__pageview_lineup:GetCurPageIndex() or 0
		local lineup_info = CPPGameLib.GetConfig("HeroRecommendLineup",self.__sp_selected_index - 1)
		for i,recommend_info in ipairs(lineup_info.recommend_list) do
			local page = self.__page_list[i]
			if not page then
				page = GUI.TemplRecommendLineupPage.New(recommend_info.recommend_hero)
				self.__pageview_lineup:AddPage(page:GetNode())
				table.insert(self.__page_list,page)
			else			
				page:__ChangeData(recommend_info.recommend_hero)
			end
		end
		--多的界面删去
		page_num = self.__pageview_lineup:GetPageNum()
		if page_num > #lineup_info.recommend_list then
			for i=(#lineup_info.recommend_list + 1),page_num do
				local cur_page_num = self.__pageview_lineup:GetPageNum()
				self.__pageview_lineup:RemovePageAtIndex(cur_page_num - 1)
			end
		end
	end
	self.__is_need_init_data = false
	--local page_index = self.__mark_lineup_index[index]
	self.__pageview_lineup:ScrollToPage(index - 1)
	self:SetPageText(index - 1)
	if self.__first_page_listener then
		self.__first_page_listener = false
		self:PageviewScrollListener()
	end
end

function Modules.RecommendLineupView:__Dispose()	
	for _,page in ipairs(self.__page_list) do
		if page then
			page:DeleteMe()
			page = nil
		end
	end

	if self.__system_foreshow_scrollview then
		self.__system_foreshow_scrollview:DeleteMe()
		self.__system_foreshow_scrollview = nil
	end

	for i=1,3 do
		if self["__effect_id"..i] then
			self:__DelEffect(self["__effect_id"..i])
			self["__effect_id"..i] = nil			
		end
	end
end
