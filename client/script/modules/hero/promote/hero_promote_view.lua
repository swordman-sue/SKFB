
--[[
%% @module: 英雄提升界面
%% @author: swordman sue
%% @created: 2016-08-23
--]]

Modules = Modules or {}

Modules.HeroPromoteView = Modules.HeroPromoteView or BaseClass(BaseView)
--界面状态
local HeroPromoteViewState = 
{
	MAIN_VIEW = 1,	--主界面
	SELECTED_HERO = 2,	--选择英雄界面
}

function Modules.HeroPromoteView:__init()
	self.__layout_name = "hero_promote"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}

	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("hero_promote_title"),
		show_diamond = true,
		show_gold = true,
		show_power = true,
		show_level = true,
		close_func = function()
			if self.__mainview_state == HeroPromoteViewState.MAIN_VIEW then
				self:CloseManual()
			else 
				self.__panel_main:SetVisible(true)
				self.__panel_selected:SetVisible(false)
				self.__mainview_state = HeroPromoteViewState.MAIN_VIEW
			end
		end,
	}
	--界面层次
	self.__mainview_state = HeroPromoteViewState.MAIN_VIEW
	--功能开启等级	
	self:__CountLockLevel()
	local tab_sp1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
	local tab_sp2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
	self.__tab_params = {
		{tab_sp1, tab_sp2},
		{
			CPPGameLib.GetString("hero_promote_tab0"),
			CPPGameLib.GetString("hero_promote_tab1"),
			CPPGameLib.GetString("hero_promote_tab2"),
			CPPGameLib.GetString("hero_promote_tab3"),
			CPPGameLib.GetString("hero_promote_tab4"),
			CPPGameLib.GetString("hero_promote_tab5"),
        }, 
        10, 
        CPPGameLib.Handler(self, self.__TabChange), 
        false,
        false,
        self.__prepare_level_list,
        self.__lock_level_list
    }

	self.__tab_view_params = 
	{
		{Modules.HeroDetailTabView, "Panel_Detail"},
		{Modules.HeroUpgradeTabView, "Panel_Upgrade"},
		{Modules.HeroBreakTabView, "Panel_Break"},
		{Modules.HeroTrainTabView, "Panel_Train"},
		{Modules.HeroPotentialTabView, "Panel_Potential"},
		{Modules.HeroAwakenTabView, "Panel_Awaken"},
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end        
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_PROMOTE)
    --self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_AWAKEN, true)
     self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_PROMOTE, true)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_LINEUP_ITEM)  	
end

function Modules.HeroPromoteView:__OpenCallback()

	CPPGameLib.PrintTable("xy" , self.__open_data , "Modules.HeroPromoteView") 


	local tab_index, hero_uid, isHideList,reinforcements_lineup_index = unpack(self.__open_data)
	tab_index = tab_index or BaseViewSubType.HeroPromote.DETAIL
	self.__is_hide_list = isHideList or false
	self.__cur_hero_uid = hero_uid or self:CallModuleFunc("GetFirstLineupHeroUID")
	self.__reinforcements_lineup_index = reinforcements_lineup_index
	--初始化选项卡、子页面
	local widget = self:GetWidget("Panel_Tab")
	widget:AddChild(self.__tab:GetNode())
	--跳转初始页面
	self.__tab:On(tab_index)

	--隐藏升级材料列表
	widget = self:GetWidget("Panel_UpgradeMaterial")	
	widget:SetVisible(false)

	--主界面
	self.__panel_main = self:GetWidget("Panel_Main")	

	--隐藏选择英雄列表
	self.__panel_selected = self:GetWidget("Panel_Select")	
	self.__panel_selected:SetVisible(false)

	--替换
	Util:WidgetButton(self:GetWidget("Button_replace"), CPPGameLib.GetString("hero_lineup_btn2"),function ()
		self:__HandleSelect()
	end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_red)

	--卸下
	Util:WidgetButton(self:GetWidget("Button_leavedown"), CPPGameLib.GetString("equip_btn_discharge"),function ()
		self:CallModuleFunc("RecinforcementToBattleReq", 0,reinforcements_lineup_index)			
	end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

	--居中
	if self.__is_hide_list then
		local Panel_main = self:GetWidget("Panel_10")	
		PosTool.Center(Panel_main)	
		self:GetWidget("Panel_HeroList"):SetVisible(false)
		self:GetWidget("Panel_Tab"):SetPosition(self:GetWidget("Panel_Tab"):GetPositionX() + 60,self:GetWidget("Panel_Tab"):GetPositionY())
		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
		self:__SelectedHero(nil,hero_info)
		if reinforcements_lineup_index then
			self:GetWidget("Button_leavedown"):SetVisible(true)
			self:GetWidget("Button_replace"):SetVisible(true)
		end
	else
		--初始化英雄列表
		self:__InitHeroList()
	end	
end

function Modules.HeroPromoteView:__Dispose()
	if self.scv_hero_list then
		self.scv_hero_list:DeleteMe()
		self.scv_hero_list = nil
	end
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
end

function Modules.HeroPromoteView:__GetIsHideHeroList()
	return self.__is_hide_list
end

function Modules.HeroPromoteView:__CountLockLevel()
	self.__lock_level_list = {1}
	self.__prepare_level_list = {1}
	local module_type_list = {Macros.Game.SystemID.HERO_UPGRADE,Macros.Game.SystemID.HERO_BREAK,
	Macros.Game.SystemID.HERO_TRAIN,Macros.Game.SystemID.HERO_POTENTIAL,Macros.Game.SystemID.HERO_AWAKEN}
	for i,module_id in ipairs(module_type_list) do
		local module_info = CPPGameLib.GetConfig("SystemEntry",module_id)
		table.insert(self.__lock_level_list,module_info.level)
		table.insert(self.__prepare_level_list,module_info.prepare_level)
	end
end

function Modules.HeroPromoteView:__TabChange(tab_index)
	BaseView.__TabChange(self, tab_index)
	self.__tab_index = tab_index
	--觉醒页面不显示阵营
	if not self.__img_camp then
		self.__img_camp = self:GetWidget("Image_camp")
		self.__img_camp:IgnoreContentAdaptWithSize(true)
		self.__img_camp:SetScaleFactor(0.7)
	end
	self.__img_camp:SetVisible(tab_index ~= BaseViewSubType.HeroPromote.AWAKEN)	
end

function Modules.HeroPromoteView:__RegistAllEvents()
	--觉醒响应，刷新对应英雄的觉醒星级
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_AWAKEN_RESP, function(_, data)
		if self.__cur_hero_uid == data.hero_uid then
			self:RefreshHeroAwakenStar(true)
		end 
		if self.scv_hero_list then
			self.scv_hero_list:ItemIter(function(item_index, item)
				local item_data = item:GetData()
				if item_data.uid == data.hero_uid then
					item:SetAwakenStar(data.awaken_star)
					return true
				end
			end)
		end
	end)

	--进阶响应，刷新对应英雄的进阶等级
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_BREAK_SUCC, function(_, data)
		if self.__cur_hero_uid == data then
			self.__is_overlook_refrash = true
			self:RefreshHeroName()
			--刷新红点
			if self.scv_hero_list then
				local hero_info_list = self:__GetHeroList()
				self.scv_hero_list:SetDataList(hero_info_list)

				--选中打开的英雄并滚动到该位置
				self:RefreshHeroList(hero_info_list)
			else
				local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
				self:__SelectedHero(nil,hero_info)
			end
		end
	end)	

	--升级响应
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_UPGRADE_SUCC, function(_, data)
		--刷新红点
		self.__is_no_need_refrash = true
		if self.scv_hero_list then
			local hero_info_list = self:__GetHeroList()
			self.scv_hero_list:SetDataList(hero_info_list)
			
			--选中打开的英雄并滚动到该位置
			self:RefreshHeroList(hero_info_list)
		else
			local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
			self:__SelectedHero(nil,hero_info)
		end
	end)

	--培养响应
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_TRAIN_RESP, function()
		self:JudgeIsHeroTrainRedDot()
	end)

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self.scv_hero_list:ItemIter(function(item_index, item)
			item:CheckSectionOperateSP()
		end)
	end)	
end

--获取选中的英雄
function Modules.HeroPromoteView:GetCurHeroUID()
	return self.__cur_hero_uid
end

--------------------------------------------------------------------------------------------------------------------------------
--选择英雄界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.HeroPromoteView:__HandleSelect()	
	self.__mainview_state = HeroPromoteViewState.SELECTED_HERO
	self.__panel_main:SetVisible(false)
	self.__panel_selected:SetVisible(true)
	--隐藏已上阵英雄
	if not self.__cbox_hide_lineuped then
		self.__is_filter_lineuped = true
		local function cbox_selected_func(selected)				
			self.__is_filter_lineuped = selected
			self.__scrollview_select_list:RefreshLayout()
		end  
		self.__cbox_hide_lineuped = self:GetWidget("CheckBox_HideLineuped")
		self.__cbox_hide_lineuped:SetSelectedState(true)
		WidgetUnity.SetCheckboxListener(self.__cbox_hide_lineuped, 
			function()
				cbox_selected_func(true)
			end, 
			function()
				cbox_selected_func(false)
			end)

		local cbox_title = Util:WidgetLabel(				
												self:GetWidget("Text_HideLineuped"), 
												CPPGameLib.GetString("hero_lineup_hide_linuped_hero"),
												nil,nil,nil,nil,Macros.Color.white_stroke
											)
		cbox_title:SetPosition( cbox_title:GetPositionX() + 20 , cbox_title:GetPositionY() )
	end

	--英雄列表
	if not self.__scrollview_select_list then
		local panel_select_list = self:GetWidget("Panel_SelectList")
		local size = panel_select_list:GetContentSize()
	    local params = 
	    {
	        item_class = Modules.TemplHeroLineupItem,
	        item_width = 455,
	        item_height = 122,
	        row = 4,
	        col = 2,
	        item_space_r = 5,
	        item_space_c = 7,
	        view_height = size.height,
	        filter_func = function(item_data)
	        	if not self.__is_filter_lineuped then
		        	return true
	        	end
	        	return item_data.hero_info.is_to_battle == Macros.Global.FALSE
	        end,
	        touchable = true,
	        touch_callback = function(item_index, item_data)
	        	--援军上阵
	        	self:CallModuleFunc("RecinforcementToBattleReq", item_data.hero_info.uid, self.__reinforcements_lineup_index)     
	        end,
	    }
	    self.__scrollview_select_list = GUI.ScrollView.New(params)
	    panel_select_list:AddChild(self.__scrollview_select_list:GetNode())
	    PosTool.Center(self.__scrollview_select_list)	
	end

	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__cur_hero_uid)
	local hero_info_list = self:CallModuleFunc("GetLineupSelectList", hero_info and hero_info.info_id or nil, Macros.Game.HeroLineup.REINFORCEMENT, self.__reinforcements_lineup_index)    
    self.__scrollview_select_list:SetDataList(hero_info_list)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--初始化英雄列表
function Modules.HeroPromoteView:__InitHeroList()
	local widget = self:GetWidget("Panel_HeroList")
	local params = {
		item_class	= Modules.HeroPromoteViewItem,
	    item_width	= 115,
	    item_height	= 110,
	    row	= 4,
	    col	= 1,
	    item_space_r = 10,
	    -- view_width = 115,
	    view_height = 448,
	    selectable = true,
	    select_callback	= function (index, data)
	    	self:__SelectedHero(index, data)
	    end,
	    select_filter_func = function(index, data)
	    	--若该英雄不能觉醒，提示
			local _, tab_index = self:GetActivatedTabView()
			if tab_index == BaseViewSubType.HeroPromote.AWAKEN then
				local config_hero = CPPGameLib.GetConfig("Hero", data.info_id)
				if config_hero.init_quality < Macros.Game.QualityType.ORANGE then
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_awaken_CouldNotAwakenTip"))
					return false
				end
			elseif tab_index == BaseViewSubType.HeroPromote.TRAIN then
				local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",data.uid) 
				if hero_info.level < ConfigAdapter.Common.GetHeroTrainmMinimumNeedLevel() then
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_CouldNotTrainTip"))
					return false
				end
			end
			return true
	    end,
	}
	self.scv_hero_list = GUI.ScrollView.New(params)
	widget:AddChild(self.scv_hero_list:GetNode())
	PosTool.Center(self.scv_hero_list)
	local hero_info_list = self:__GetHeroList()
	self.scv_hero_list:SetDataList(hero_info_list)

    --选中打开的英雄并滚动到该位置
	self:RefreshHeroList(hero_info_list)

end

--获取阵位列表
function Modules.HeroPromoteView:__GetHeroList()
	local list = {}
	local lineup_list = self:CallModuleFunc("GetLineupList")
	for i, lineup_info in ipairs(lineup_list) do
		local hero_info = self:CallModuleFunc("GetHeroInfo", lineup_info.uid)
		table.insert(list, hero_info)
	end
	return list
end

--选择英雄回调
function Modules.HeroPromoteView:__SelectedHero(index, data)
	if not self.__is_no_need_refrash then
		--若该英雄不能觉醒，隐藏觉醒选项卡
		local config_hero = CPPGameLib.GetConfig("Hero", data.info_id)
		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",data.uid) 
		local hide_table = {}
	    if config_hero.init_quality < Macros.Game.QualityType.ORANGE then
	    	table.insert(hide_table,BaseViewSubType.HeroPromote.AWAKEN)    	
	    end
	    self.__tab:HideIndex(hide_table)
		--刷新英雄信息
		self:RefreshHero(data.uid)

		--刷新子页面
		local tab_view, _ = self:GetActivatedTabView()
		if tab_view then
			if self.__is_overlook_refrash then
				self.__is_overlook_refrash = false
			else
				tab_view:RefreshBySelectedHero()
			end			
		end
	else
		self.__is_no_need_refrash = false
	end

	--刷新选项卡红点
	local isCouldUpgrade = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldUpgrade",data.uid)  
	local isCouldBreak = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldBreak",data.uid) 
	if isCouldUpgrade then
		self.__tab:AddRedDot(BaseViewSubType.HeroPromote.UPGRADE) 
	else
		self.__tab:HideRedDot(BaseViewSubType.HeroPromote.UPGRADE) 
	end
	if isCouldBreak then
		 self.__tab:AddRedDot(BaseViewSubType.HeroPromote.BREAK) 
	else
		self.__tab:HideRedDot(BaseViewSubType.HeroPromote.BREAK) 
	end
	--培养选项卡
	self:JudgeIsHeroTrainRedDot()
end

--刷新英雄
function Modules.HeroPromoteView:RefreshHero(uid)
	if uid then
		self.__cur_hero_uid = uid
	end
	-- print("self.__cur_hero_uid: ",self.__cur_hero_uid)
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__cur_hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	--名字、进阶等级
	self:RefreshHeroName()

	--判断是否开启觉醒
	local IsOpenAwaken = self:JudgeIsOpenAwaken()
	--觉醒星级
	self:RefreshHeroAwakenStar(IsOpenAwaken)
	--职业
	if not self.__image_herocompro then
		self.__image_herocompro = self:GetWidget("Image_HeroComPro")
	end
	self.__image_herocompro:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..config_hero.profession, true))

	--阵营
	self.__img_camp:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_"..config_hero.camp, true))

	--模型
	if not self.__ui_hero then
		local panel_model = self:GetWidget("Panel_HeroComModel")
		self.__ui_hero = LogicGUI.Model.New({info_id = hero_info.info_id, parent = panel_model})	
		self.__ui_hero:SetScaleFactor(1.2)
		self.__ui_hero:SetPosition(15,0)
	else
		self.__ui_hero:SetInfoID(hero_info.info_id)
	end
	--缩放比例
	self.__ui_hero:SetScaleFactor(config_hero.ui_scale or 1)
end

--刷新英雄名字、进阶等级
function Modules.HeroPromoteView:RefreshHeroName()
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__cur_hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	if not self.__text_herocomname then
		self.__text_herocomname = self:GetWidget("Text_HeroComName")
	end
	if hero_info.break_level > 0 then
		self.__text_herocomname:SetText(CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
	else
		self.__text_herocomname:SetText(config_hero.name)
	end
	self.__text_herocomname:SetTextColor(Macros.QualityColor(hero_info.quality_level))
	self.__text_herocomname:SetStrokeColor(Macros.QualityColorStroke(hero_info.quality_level))

	--资质
	if not self.__text_herointelligence then
		self.__text_herointelligence = self:GetWidget("Text_HeroIntelligence")			
		Util:WidgetLabel(self.__text_herointelligence,"", Macros.TypefaceSize.largish, Macros.Color.keypoint, 30,90)
	end 
	--资质
	local qualifications = ConfigAdapter.Hero.GetHeroQualifications(hero_info.quality_level,hero_info.info_id,hero_info.quality_stage)
	self.__text_herointelligence:SetText(CPPGameLib.GetString("hero_intelligence",qualifications or 0))
end

--判断英雄是否达到觉醒开放等级
function Modules.HeroPromoteView:JudgeIsOpenAwaken()
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_AWAKEN)	
	return is_open
end

--刷新英雄觉醒星级
function Modules.HeroPromoteView:RefreshHeroAwakenStar(is_open)
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__cur_hero_uid)

	if not self.__awaken_star_list then
		self.__awaken_star_list = {}
		for i = 1, Macros.Game.HERO_AWAKE_STAR_MAX do
			self.__awaken_star_list[i] = self:GetWidget("Image_AwakenStar"..i)
		end
	end
	for i, image_star in ipairs(self.__awaken_star_list) do
		self:GetWidget("Image_AwakenStarBg"..i):SetVisible(is_open and hero_info.quality_level >= Macros.Game.QualityType.ORANGE
			and true or false)
		image_star:SetVisible(hero_info.awaken_star >= i)
	end
end

--判断英雄培养页签红点
function Modules.HeroPromoteView:JudgeIsHeroTrainRedDot()
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_TRAIN)
	--培养选项卡
	local train_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetHeroTrainPropId())
	if train_num >= ConfigAdapter.Common.GetHeroTrainRedDotNum() and is_open then
		self.__tab:AddRedDot(BaseViewSubType.HeroPromote.TRAIN) 
	else
		self.__tab:HideRedDot(BaseViewSubType.HeroPromote.TRAIN) 
	end
end

--刷新英雄所在队伍位置
function Modules.HeroPromoteView:RefreshHeroList(hero_info_list)	
	for i, v in ipairs(hero_info_list) do
		if v.uid == self.__cur_hero_uid then
			self.scv_hero_list:LocateItem(i)
			self.scv_hero_list:SelectItem(i)
			break
		end
	end	
end

--获取打开参数
function Modules.HeroPromoteView:GetOpenParams()
	return {self.__tab_index,self.__cur_hero_uid}
end

--------------------------------------------------------------------------------------------
--英雄列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.HeroPromoteViewItem = Modules.HeroPromoteViewItem or BaseClass(GUI.ScrollItem)

function Modules.HeroPromoteViewItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.HeroPromoteViewItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(115,110)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.HeroPromoteViewItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	--头像
	if not self.icon then
	    self.icon = LogicGUI.IconLayer.New(false, false)
	    self.icon:SetAnchorPoint(0.5,1)  
	    self.layout:AddChild(self.icon:GetNode())
	    PosTool.CenterTop(self.icon)
	end
    self.icon:SetData({id = data.info_id,obj_info = self.__data})
    self.icon:ShowAwakenStar()
    --名字
    local break_level = ""
    if data.break_level ~= 0 then
      break_level = "+"..data.break_level
    end
    local heroInfo = CPPGameLib.GetConfig("Hero", data.info_id)
    self.name = Util:Name(heroInfo.name..break_level, Macros.TypefaceSize.slightly,data.quality_level)
    self.name:SetAnchorPoint(0.5,0)
    self.layout:AddChild(self.name)
    PosTool.CenterBottom(self.name)

    local isCouldUpgrade = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldUpgrade",data.uid)  	
	if isCouldUpgrade then
		self:AddRedDot()
	else
		local isCouldBreak = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldBreak",data.uid) 
		if isCouldBreak then
			self:AddRedDot()
		end
	end    

	self:CheckSectionOperateSP()
end

--觉醒等级
function Modules.HeroPromoteViewItem:SetAwakenStar(awaken_star)
    if self.icon then
    	self.icon:ShowAwakenStar(awaken_star)
    end
end

function Modules.HeroPromoteViewItem:SetSelected(var)
	if var then
		if not self.selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
		    self.selected_image = Util:Sprite(sp)
		    self.icon:AddChild(self.selected_image, Macros.Global.z_order_top - 1)
		    PosTool.Center(self.selected_image)
		end
	else
		if self.selected_image then
			self.selected_image:RemoveSelf(true)
			self.selected_image = nil
		end
	end	
end

--添加红点
function Modules.HeroPromoteViewItem:AddRedDot()
	--红点
	if not self.entry_red_dot then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.entry_red_dot:SetAnchorPoint(1, 1)
		self:AddChild(self.entry_red_dot,2)
		PosTool.RightTop(self.entry_red_dot,-20,0)
	end
	self.entry_red_dot:SetVisible(true)
end

--隐藏红点
function Modules.HeroPromoteViewItem:HideRedDot()
	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

--新手指引特殊操作检测(英雄提升-选择英雄)
function Modules.HeroPromoteViewItem:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_PROMOTE_SELECT_HERO, self:GetNode(), self.__data.info_id)
end

