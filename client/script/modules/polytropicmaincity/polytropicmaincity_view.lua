--
-- @brief 多变的主城
-- @auther: ludexian
-- @date 2018-08-09
--

Modules = Modules or {}
Modules.PolytropicMaincityView = Modules.PolytropicMaincityView or BaseClass(BaseView)

local PolytropicTypeMap = 
{
	--活动界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.ACTIVITY_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.LIMIT_DUNGEON},
		panel_name = "Panel_ActivityViewType",item_class = Modules.TemplPolytropicTypeActivityItem,
        item_width = 300,item_height = 130,row = 4,col = 1,item_space_r = 7,
        selectable = true,view_height = 544,horizon = false,
	},
	--挑战界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.CHALLENGE_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.CHALLENGE},
		panel_name = "Panel_ChallengeViewType",item_class = Modules.TemplPolytropicTypeChallengeItem,
        item_width = 302,item_height = 415,row = 1,col = 3,item_space_c = 40,
		view_width = Macros.Global.DesignSize.width,panel_view_height = 415,horizon = true,touchable = true,
		panel_anchorpoint_y = 0.5,panel_pos_y = 300,
	},
	--竞技场界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.ARENA_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.ARENA},
		TemplateList = {GUI.TemplateRegistry.ARENA_HEROMESSAGE_CELL},
		panel_name = "Panel_ArenaViewType",item_class = Modules.TemplPolytropicTypeArenaItem,
        item_width = 257,item_height = Macros.Global.DesignSize.height,row = 1,col = 3,
		view_width = Macros.Global.DesignSize.width,view_height = Macros.Global.DesignSize.height,panel_view_height = Macros.Global.DesignSize.height,horizon = true,touchable = true,
		panel_anchorpoint_y = 0,panel_pos_y = 0,
	},
	--主线副本界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.DUNGEON_VIEW_TYPE] = 
	{
		panel_name = "Panel_DungeonViewType",item_class = Modules.TemplPolytropicTypeDungeonItem,
		item_space_c = 10,item_width = 214,item_height = 400,row = 1,col = 5,
		selectable = true,view_width = 1110,horizon = true,
	},
	--图鉴界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.HANDBOOK_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.HERO_HANDBOOK},
		TemplateList = {GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON},
		panel_name = "Panel_HandBookViewType",item_class = Modules.TemplPolytropicTypeHandBookItem,
		item_space_c = 8,item_space_r = 5,item_width = 120,item_height = 150,row = 4,col = 6,
		touchable = true,view_height = 500,
	},
	--商店界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.SHOP_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.SHOP_FAILARMY},
		panel_name = "Panel_ShopViewType",item_class = Modules.TemplPolytropicTypeShopItem,
		item_space_c = 20,item_space_r = 5,item_width = 285,item_height = 214,row = 2,col = 3,
		touchable = true,view_height = 440,
	},
	--Vip界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.VIP_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.VIP},
		TemplateList = {GUI.TemplateRegistry.TEMPL_VIP_ITEM},
		panel_name = "Panel_VipViewType",pageitem_class = GUI.TemplPolytropicTypeVipItem,
	},
	--世界之心界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.WORLDHEART_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.WORLD_HREAT},
		panel_name = "Panel_WorldHeartViewType",item_class = Modules.TemplPolytropicTypeWorldHeartItem,
        item_width = 120,item_height = 120,row = 4,col = 1,selectable = true,
        view_width = 120,view_height = 480,horizon = false,
	},
	--精灵界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.SPIRIT_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.SPIRIT},
		panel_name = "Panel_SpiritViewType",item_class = Modules.TemplPolytropicTypeSpiritItem,
        item_width = 90,item_height = 80,row = 4,col = 1,selectable = true,item_space_r = 15,
        view_width = 90,view_height = 485,horizon = false,
	},
	--推荐阵容界面类型
	[Modules.PolytropicMaincityModule.PolytropicType.RECOMMEND_VIEW_TYPE] = 
	{
		UITextureAtlasTypeList = {Resource.UITextureAtlasType.RECOMMENDLINEUP},
		panel_name = "Panel_RecommendViewType",item_class = Modules.TemplPolytropicTypeRecommendItem,
        item_width = 225,item_height = 88,row = 4,col = 1,selectable = true,item_space_r = 20,
        view_width = 225,view_height = 520,horizon = false,
	},
}

function Modules.PolytropicMaincityView:__init()
	self.__layout_name = "polytropicmaincity_view"
	self.__open_loading = false
	self.__title_bar_params = {
		show_bg = false,
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy	= true,
		res_beg_offset_x = -90,
		res_unit_bg_imageinfo = {Resource.UITextureAtlasType.CITY, "bg_resource"},
		res_unit_title_offset_x = -16,
	}
	--获取当前样式类型
	local channel_type = CPPSdkTools.GetCPChannelType()
	local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
	self.__cur_polytropic_maincity_viewtype = config_channel_setting and config_channel_setting.use_examine_maincity_type or 1
	local polytropic_type_info = PolytropicTypeMap[self.__cur_polytropic_maincity_viewtype]
	if not polytropic_type_info then
		return
	end
	--资源加载
	if polytropic_type_info.UITextureAtlasTypeList then
		for i,AtlasType in ipairs(polytropic_type_info.UITextureAtlasTypeList) do
			local is_language
			if polytropic_type_info.UITextureAtlasTypeList[i-1] and polytropic_type_info.UITextureAtlasTypeList[i-1] == polytropic_type_info.UITextureAtlasTypeList[i] then
				is_language = true
			end
			self:__AddPreloadTextureAtlas(AtlasType,is_language)
		end
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CITY)   
	--控件加载
	if polytropic_type_info.TemplateList then
		for _,TemplateType in ipairs(polytropic_type_info.TemplateList) do
			self:__AddPreloadTemplate(TemplateType)
		end
	end	
	self.__open_callback = function ()
		self:__OpenCallback()
	end
end

function Modules.PolytropicMaincityView:__Dispose()
	if self.__system_list then
		self.__system_list:DeleteMe()
		self.__system_list = nil
	end

	if self.__back_effectid then
		local effect = GlobalEffectMgr:GetEffect(self.__back_effectid)
		if effect then
			effect:DeleteMe()
			self.__back_effectid = nil
		end
	end
	
	if self.__front_effectid then
		local effect = GlobalEffectMgr:GetEffect(self.__front_effectid)
		if effect then
			effect:DeleteMe()
			self.__front_effectid = nil
		end
	end

	if self.__page_list then
		for _,page in ipairs(self.__page_list) do
			page:DeleteMe()
		end
		self.__page_list = nil
	end

	if self.__smog_effectid then
		local effect = GlobalEffectMgr:GetEffect(self.__smog_effectid)
		if effect then
			effect:DeleteMe()
			self.__smog_effectid = nil
		end
	end

	if self.__scene_effect_id then
		local effect = GlobalEffectMgr:GetEffect(self.__scene_effect_id)
		if effect then
			effect:DeleteMe()
			self.__scene_effect_id = nil
		end
	end
end

function Modules.PolytropicMaincityView:__OpenCallback()
	local callback = unpack(self.__open_data)
	if callback then
		callback()
	end
	--创建背景
	self:__SetPolytropicMaincityViewBg()
	--初始化各种类型界面
	self:__InitPolytropicMaincityView()
end

--创建背景
function Modules.PolytropicMaincityView:__SetPolytropicMaincityViewBg()
	local res_name = self:CallModuleFunc("GetPolytropicMaincityBgName")
	if not res_name then
		--审核中，使用随机背景
		local bg_list = 
		{
			"bg_sp_daliydungeon", "bg_sp_activity", "bg_sp_arena", "bg_sp_arena_close_layer", "bg_sp_assist", 
			"bg_sp_cham_rank", "bg_sp_christmas", "bg_sp_direct_buy", "bg_sp_hero_evolution", "bg_sp_knighthood", 
			"bg_sp_ladder", "bg_sp_onecallrecruit", "bg_sp_panjun", "bg_sp_pvp", "bg_sp_quick_plunder", "bg_sp_recovery", 
			"bg_sp_scene", "bg_sp_spirit", "bg_sp_springfestival", "bg_sp_worldboss", "bg_sp_worldheart", "bp_sp_challenge", 
			"bp_sp_crystal", "bp_sp_dungeon", "bp_sp_ladder", "bp_sp_manor", "bp_sp_newtower", "bp_sp_prompt",
			"bp_sp_sevendays", "bp_sp_union", "bp_sp_union_dungeon", "bp_sp_union_transcript"
		}
		local channel_type = CPPSdkTools.GetCPChannelType()
		local config_channel_setting = CPPGameLib.GetConfig("ChannelSetting", channel_type, false)
		local examine_bg_index_max = #bg_list
		local examine_bg_index = config_channel_setting and config_channel_setting.examine_bg_index or nil
		examine_bg_index = (examine_bg_index or MathTool.GetRandom(examine_bg_index_max)) % examine_bg_index_max
		examine_bg_index = examine_bg_index ~= 0 and examine_bg_index or examine_bg_index_max
		res_name = bg_list[examine_bg_index]
		self:CallModuleFunc("SetPolytropicMaincityBgName",res_name)
	end
    self.__bg_params = 
    {
    	type = BaseView.BGType.IMAGE, 
    	image_path = Resource.PathTool.GetUIResPath("bigbg/"..res_name),    	
    }
	self.__bg = CommonBG.New(self.__bg_params, self)										
	self:AddChild(self.__bg, Macros.Global.z_order_bottom)
	PosTool.Center(self.__bg)

	--若背景图可点击，则设置根节点可穿透
	self.__root_widget:SetTouchSwallow(false)					
end

--初始化各种类型界面
function Modules.PolytropicMaincityView:__InitPolytropicMaincityView()
	local polytropic_type_info = PolytropicTypeMap[self.__cur_polytropic_maincity_viewtype]
	if not polytropic_type_info then
		return
	end
	--当前界面类型容器
	local cur_visible_panel = self:GetWidget(polytropic_type_info.panel_name)
	if not cur_visible_panel then
		return
	end
	cur_visible_panel:SetVisible(true)
	--入口
	self.__ret_entry_list = self:CallModuleFunc("GetModuleEntrysList")
	--创建scrollview
	if polytropic_type_info.item_class then
		--创建队列
		if not self.__system_list then
			local panel_system_list = WidgetUnity.GetWidgetByName(cur_visible_panel,"Panel_SystemList",false)
			--全屏容器
			if not panel_system_list then
				panel_system_list = Util:Layout(Macros.Global.DesignSize.width,polytropic_type_info.panel_view_height or 0)
			    panel_system_list:SetClippingEnable(false)
			    panel_system_list:SetAnchorPoint(0.5,polytropic_type_info.panel_anchorpoint_y)
			    cur_visible_panel:AddChild(panel_system_list)
			    PosTool.CenterBottom(panel_system_list,0,polytropic_type_info.panel_pos_y)
			end
			--选择列表
		    local params =  
		    {
		        item_class = polytropic_type_info.item_class,
		        item_width = polytropic_type_info.item_width,
		        item_height = polytropic_type_info.item_height,
		        row = polytropic_type_info.row,
		        col = polytropic_type_info.col,
		        horizon = polytropic_type_info.horizon or false,
		        item_space_r = polytropic_type_info.item_space_r or 0,
		        item_space_c = polytropic_type_info.item_space_c or 0,
		        selectable = polytropic_type_info.selectable, 
		        select_callback = function(index,itemData)
		        	--副本界面类型
		        	if self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.DUNGEON_VIEW_TYPE then	            
			            local item = self.__system_list:GetItem(index)
			        	if not item then
			        		return
			        	end
			        	--设置当前打开章节       	
						CPPActionManager.ScaleTo(item:GetNode(), 0.25,1.2,GlobalCallbackMgr:AddCallBackFunc(function()
							self:__SelectedSystemIconCallBack(itemData)
						end))	
					else
						self:__SelectedSystemIconCallBack(itemData)
					end
		        end,
		        touchable = polytropic_type_info.touchable, 
		        touch_callback = function(item_index, item_data)	
		        	self:__SelectedSystemIconCallBack(item_data)
		        end,
		        view_height = polytropic_type_info.view_height,
		        view_width = polytropic_type_info.view_width,
		    }
		    -- 创建参数
		    if self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.ARENA_VIEW_TYPE then
		    	self.__system_list = GUI.ArenaScrollView.New(params)
		    elseif self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.DUNGEON_VIEW_TYPE then
		    	self.__system_list = GUI.SpecialScrollView.New(params)
		    else
			    self.__system_list = GUI.ScrollView.New(params)
			end
		    panel_system_list:AddChild(self.__system_list:GetNode())
		    PosTool.Center(self.__system_list:GetNode())
		end		
		self.__system_list:SetDataList(self.__ret_entry_list)
		--副本界面类型
		if self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.DUNGEON_VIEW_TYPE then
			--设置解锁副本的章数
			self.__system_list:SetNecessaryData(#self.__ret_entry_list)	
			--将索引为Index的条目移动到屏幕中心
			self.__system_list:MoveItemToScreenCenterByIndex(1)	
		elseif self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.RECOMMEND_VIEW_TYPE then
			self.__system_list:SetScissorSize(300,520)
		end	
		--默认
		if polytropic_type_info.selectable then
			self.__system_list:SelectItem(1)
		end
	elseif polytropic_type_info.pageitem_class then
		self.__page_list = self.__page_list or {}
		--翻页容器
		if not self.__pageview then
			--后退
		    WidgetUnity.SetWidgetListener(self:GetWidget("Image_back"), nil, nil,function ()
		        self:OnButtonLeft()
		    end)

		    local effect
		    self.__back_effectid ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou"},self:GetWidget("Image_back"),999)
		    effect:SetAnchorPoint(0.5,0.5)
		    effect:SetPosition(self:GetWidget("Image_back"):GetContentWidth()/2,self:GetWidget("Image_back"):GetContentHeight()/2)
		    effect:SetScaleXFactor(-1)

		    --前进
		    WidgetUnity.SetWidgetListener(self:GetWidget("Image_front"), nil, nil,function ()
		        self:OnButtonRight()
		    end)

		    self.__front_effectid,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou"},self:GetWidget("Image_front"),999)
		    effect:SetAnchorPoint(0.5,0.5)
		    effect:SetPosition(self:GetWidget("Image_front"):GetContentWidth()/2,self:GetWidget("Image_front"):GetContentHeight()/2)
			--翻页容器
			self.__pageview = WidgetUnity.GetWidgetByName(cur_visible_panel,"PageView",false)
			--创建页面
			for _,entry_info in ipairs(self.__ret_entry_list) do
				local page = polytropic_type_info.pageitem_class.New(entry_info)
				self.__pageview:AddPage(page:GetNode())
				PosTool.CenterBottom(page)
				table.insert(self.__page_list,page)
			end			
			--默认跳到第一个信息界面
			self.__pageview:ForceDoLayout()
			self.__pageview:ScrollToPage(0)
		end
	end
end

--选择图标回调
function Modules.PolytropicMaincityView:__SelectedSystemIconCallBack(itemData)
	if not itemData then
		return
	end
	self.__cur_item_data = itemData
	--根据选择界面类型来进行处理
	if self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.ACTIVITY_VIEW_TYPE then
		--活动界面类型
		if not self.__btn_go then
			--前往按钮
			self.__btn_go = self:GetWidget("Button_go")
			Util:WidgetButton(self.__btn_go,CPPGameLib.GetString("polytropic_maincity_go_for"),function ()
				--前往系统
				self:GoForSystem(self.__cur_item_data)
			end,Macros.TypefaceSize.tab,Macros.Color.btn, Macros.Color.button_yellow, 2)
		end
		--框
		if not self.__img_frame then
		    self.__img_frame = self:GetWidget("Image_frame") 
		    self.__img_frame:SetImage(Resource.PathTool.GetPromptPath("sp_activity_frame") , TextureResType.LOCAL )
		end
		--更改背景图
		local sp_res_list = {"47","72","84","102"}
		local random = MathTool.GetRandom(1, #sp_res_list)
		local scene_sp = Resource.PathTool.GetPromptPath("sp_prompt_"..sp_res_list[random])
    	local promptwh = Util:Sprite(scene_sp)
		if not self.__scene_sp then
			--前往按钮
			self.__panel_scene = self:GetWidget("Panel_scene")
			self.__scene_sp = Util:Sprite9(scene_sp,{promptwh:GetContentWidth(),promptwh:GetContentHeight()})
			self.__scene_sp:SetContentSize(self.__panel_scene:GetContentWidth(),self.__panel_scene:GetContentHeight())  
			self.__panel_scene:AddChild(self.__scene_sp)
			PosTool.Center(self.__scene_sp)
		else
			self.__scene_sp:SetImage(scene_sp)
			self.__scene_sp:SetContentSize(self.__panel_scene:GetContentWidth(),self.__panel_scene:GetContentHeight()) 
		end	
	elseif self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.WORLDHEART_VIEW_TYPE then
		--世界之心界面类型
		--系统名字
		if not self.__Text_spirit_crystalname then
			self.__txt_spirit_crystalname = self:GetWidget("Text_spirit_crystalname")			
			Util:WidgetLabel(self.__txt_spirit_crystalname,"", Macros.TypefaceSize.headlineII, Macros.Color.btn_1,10,300)
			self.__txt_spirit_crystalname:SetTextHorizontalAlign(UITextEx.ALIGIN_CENTER)
		end
		self.__txt_spirit_crystalname:SetText(self.__cur_item_data.module_txt or "")
		--前往按钮
		if not self.__btn_lighton then
			--添加烟雾特效
			local effect
			self.__smog_effectid,effect = self:__CreateEffect({res_name = "UI_dizuo"},self:GetWidget("Image_fazhen"))
			PosTool.Center(effect)
			self.__btn_lighton = self:GetWidget("Button_lightOn")
			Util:WidgetButton(self.__btn_lighton,CPPGameLib.GetString("polytropic_maincity_go_for"),function ()
				--前往系统
				self:GoForSystem(self.__cur_item_data)
			end,Macros.TypefaceSize.button,Macros.Color.btn, Macros.Color.button_yellow)
		end
		--系统图标
		local sp = Resource.PathTool.GetSystemEntryIconPath(self.__cur_item_data.res_name)
		if not self.__img_center_crystal then
			self.__img_center_crystal = self:GetWidget("Image_center_crystal")
			self.__img_center_crystal:IgnoreContentAdaptWithSize(true)
		end
		self.__img_center_crystal:SetImage(sp,TextureResType.LOCAL)
		self.__img_center_crystal:SetScaleFactor(2)
	elseif self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.SPIRIT_VIEW_TYPE then
		--精灵界面类型
		--系统名字
		if not self.__txt_spiritname then
			self.__txt_spiritname = self:GetWidget("Text_spiritName")			
			Util:WidgetLabel(self.__txt_spiritname,"", Macros.TypefaceSize.headlineII,Macros.Color.red,0,0,Macros.Color.red_stroke)
		end
		self.__txt_spiritname:SetText(self.__cur_item_data.module_txt or "")
		--前往按钮
		if not self.__btn_onfight then
			--场景特效容器
			local panel_sceneeffect = self:GetWidget("Panel_sceneEffect")
			self.__scene_effect_id,effect = self:__CreateEffect({res_name = "UI_jinglingchangjin",loop = true},panel_sceneeffect,-1)
			effect:SetAnchorPoint(0.5,0.5)
			effect:SetPosition(200,Macros.Global.DesignSize.height/2)
			--前往按钮
			self.__btn_onfight = self:GetWidget("Button_onFight")
			Util:WidgetButton(self.__btn_onfight,CPPGameLib.GetString("polytropic_maincity_go_for"),function ()
				--前往系统
				self:GoForSystem(self.__cur_item_data)
			end,Macros.TypefaceSize.button,Macros.Color.btn, Macros.Color.button_red)
		end
		--系统图标
		local sp = Resource.PathTool.GetSystemEntryIconPath(self.__cur_item_data.res_name)
		if not self.__img_system_icon then
			local polytropic_type_info = PolytropicTypeMap[self.__cur_polytropic_maincity_viewtype]
			--当前界面类型容器
			local cur_visible_panel = self:GetWidget(polytropic_type_info.panel_name)
			--图标
			self.__img_system_icon = Util:Sprite(sp)
			self.__img_system_icon:SetScaleFactor(2)
			cur_visible_panel:AddChild(self.__img_system_icon)
			PosTool.Center(self.__img_system_icon)
		else
			self.__img_system_icon:SetImage(sp)
		end
	elseif self.__cur_polytropic_maincity_viewtype == Modules.PolytropicMaincityModule.PolytropicType.RECOMMEND_VIEW_TYPE then
		--推荐阵容界面类型
		--系统名字
		if not self.__txt_lineuptype then
			self.__txt_lineuptype= self:GetWidget("Text_lineupType")			
			Util:WidgetLabel(self.__txt_lineuptype,"", Macros.TypefaceSize.headlineII,Macros.Color.red,0,0,Macros.Color.red_stroke)
		end
		self.__txt_lineuptype:SetText(self.__cur_item_data.module_txt or "")
		--背景
		if not self.__img_castlebg then
			self.__img_castlebg = self:GetWidget("Image_castleBg")
			self.__img_castlebg:IgnoreContentAdaptWithSize(true)
			self.__img_castlebg:SetImage(Resource.PathTool.GetUIResPath("bigbg/bg_recommendlineup"),TextureResType.LOCAL)
		end
		local cur_panel = self:GetWidget("Panel_right1")
		--系统图标
		local sp = Resource.PathTool.GetSystemEntryIconPath(self.__cur_item_data.res_name)
		if not self.__img_system_icon then			
			--图标
			self.__img_system_icon = Util:Sprite(sp)
			self.__img_system_icon:SetScaleFactor(2)
			cur_panel:AddChild(self.__img_system_icon)
			PosTool.Center(self.__img_system_icon)
		else
			self.__img_system_icon:SetImage(sp)
		end
		--前往按钮
		if not self.__btn_gofor then
			local sp_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,"btn_btn_biggreen2")
			self.__btn_gofor = Util:Button(sp_btn, function ()
				--前往系统
				self:GoForSystem(self.__cur_item_data)
			end, CPPGameLib.GetString("polytropic_maincity_go_for"), Macros.TypefaceSize.button,Macros.Color.btn, Macros.Color.button_yellow)
			cur_panel:AddChild(self.__btn_gofor)
			PosTool.CenterBottom(self.__btn_gofor,0,50)
		end
	else
		--前往系统
		self:GoForSystem(self.__cur_item_data)
	end
end

--前往系统
function Modules.PolytropicMaincityView:GoForSystem(entry_info)
	if not entry_info then
		return
	end
	
	if entry_info.system_id == Macros.Game.SystemID.UNION then
		local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
		if role_info.guild_id == 0 then
			CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
    	else
			--系统模块跳转
			GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false)
    	end
    elseif entry_info.system_id == Macros.Game.SystemID.LIMIT_TEAM then	
    	CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)	
    elseif entry_info.system_id == Macros.Game.SystemID.FIRST_RECHARGE then	
    	CallModuleFunc(ModuleType.CHARGE , "SetFirstRechargeRed" , 0 )
		--系统模块跳转
		GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false) 
	-- unlock定制FB功能  	
	elseif entry_info.system_id == Macros.Game.SystemID.UNLOCK_FABEBOOK then
		self:__OnUnlockFacebook()	
	-- unlock定制看视频功能  	
	elseif entry_info.system_id == Macros.Game.SystemID.UNLOCK_AD then
		self:__OnUnlockAdvertising()	
	else
		--系统模块跳转
		GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false)
	end
end

--左
function Modules.PolytropicMaincityView:OnButtonLeft()
    if self.__pageview:GetCurPageIndex() ~= 0 then
        self.__pageview:ScrollToPage(self.__pageview:GetCurPageIndex() - 1)
    end
end

--右
function Modules.PolytropicMaincityView:OnButtonRight()
    if self.__pageview:GetCurPageIndex() < (#self.__ret_entry_list - 1) then
        self.__pageview:ScrollToPage(self.__pageview:GetCurPageIndex() + 1)
    end
end
