
--[[
%% @module: 英雄阵容界面
%% @author: swordman sue
%% @created: 2016-08-23
--]]

Modules = Modules or {}

Modules.HeroLineupView = Modules.HeroLineupView or BaseClass(BaseView)

function Modules.HeroLineupView:__init()
	self.__layout_name = "hero_lineup"
	self.__close_mode = BaseView.CloseMode.VISIBLE
	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("hero_lineup_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_power = true,
		close_func = function()
			if self.__sub_type == BaseViewSubType.HeroLineup.MAIN or 
				self.__sub_type == BaseViewSubType.HeroLineup.REINFORCEMENTS then
				self:CloseManual()
			else
				--清空已选阵位信息
				self:__SelectLineupPos()
				self:__ChangeSubView(self.__is_return_reinforcement and BaseViewSubType.HeroLineup.REINFORCEMENTS or BaseViewSubType.HeroLineup.MAIN)
			end
		end,
	}

	self.__loaded_callback = function()
		self:__LoadedCallback()
	end

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__close_callback = function()
    	self:__CloseCallback()
    end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_LINEUP_INFO)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_DIVINATION_LINEUP_INFO)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_LINEUP_ITEM)

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_COMMON)	
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_LINEUP)	
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_LINEUP, true)	
end

function Modules.HeroLineupView:__LoadedCallback()
	--初始化子页面
	local widget, widget_key
	local widget_name_list = {"Panel_Main", "Panel_Select", "Panel_Reinforcements", "Panel_Lineups"}
	for _, widget_name in ipairs(widget_name_list) do
		widget_key = "__"..string.lower(widget_name)
		self[widget_key] = self:GetWidget(widget_name)
		self[widget_key]:SetVisible(false)
	end

	--基础类型
	self.prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}

	--背景
	self:GetWidget("Image_bg"):IgnoreContentAdaptWithSize(true)
	self:GetWidget("Image_bg"):SetImage(Resource.PathTool.GetUIResPath("bigbg/bg_herolineup"),TextureResType.LOCAL)

	--装备容器
	self.__panel_equiplist = self:GetWidget("Panel_EquipList")
	--卡牌容器
	self.__panel_divination = self:GetWidget("Panel_Divination")
end

function Modules.HeroLineupView:__OpenCallback()
	--切换到初始子页面	
	self:__ChangeSubView(unpack(self.__open_data))	
end

function Modules.HeroLineupView:__CloseCallback()
	self:__DeleteTypeEffect()

	if self.__wind_words_layout then
		self.__wind_words_layout:RemoveSelf(true)
		self.__wind_words_layout = nil
		self._is_no_setting = false
		self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)
	end
	
	if self.__templ_hero_info then
		self.__templ_hero_info:DeleteMe()
		self.__templ_hero_info = nil
	end

	if self.__templ_lineup_info then
		self.__templ_lineup_info:DeleteMe()
		self.__templ_lineup_info = nil
	end

	if self.__templ_divination_info then
		self.__templ_divination_info:DeleteMe()
		self.__templ_divination_info = nil
	end

	if self.__templ_Reinforcements then
		self.__templ_Reinforcements:DeleteMe()
		self.__templ_Reinforcements = nil
	end
end

function Modules.HeroLineupView:__RegistAllEvents()
	--红点更新
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id, sub_system_id)
		if not self:IsOpen() then
			return
		end     

		if system_id ~= Macros.Game.SystemID.HERO_LINEUP then
			return
		end

		--援军阵位
		if sub_system_id == Macros.Game.SystemID.REINFORCEMENT then
			--更新援军红点
			self:__UpdateReinforcementsRedDot()
			return

		--卡牌穿戴、装备穿戴、圣器穿戴
		elseif sub_system_id == Macros.Game.SystemID.DIVINATION_DRESS or sub_system_id == Macros.Game.SystemID.EQUIP_DRESS then

			--更新卡牌/装备切换按钮红点
			self:__UpdateMoreFunctionBtnReddot()

			--更新英雄阵位红点
			self.__scrollview_lineup_list:ItemDataIter(function(item_index, item_data)
				local item = self.__scrollview_lineup_list:GetItem(item_index)
				if item then
					item:JudgeHaveRedDot()
				end
			end)		
		end
	end)

	--更新英雄信息
	local function update_hero_info()
		if not self:IsOpen() then
			return
		end     

		--处理飘字特效
		if self:CallModuleFunc("GetIsHaveChangePropertyNeedWindWords") then
			self:__WindWords()

		--刷新英雄数据
		else
			if self.__templ_hero_info then
				local hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", self.__selected_lineup_index)	
				self.__templ_hero_info:SetData(hero_info)
			end	
		end	

		--移除屏蔽层
		CPPActionManager.DelayTo(self.__root_widget, 1, GlobalCallbackMgr:AddCallBackFunc( function ( ... )
			if self.__shield_layout then
				self.__shield_layout:RemoveSelf(true)
				self.__shield_layout = nil
			end
		end))
	end

	--装备穿戴、卸下、更新
	self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_CROSS_EQUIP, update_hero_info)
	self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_DISCHARGE_EQUIP, update_hero_info)
	self:BindGlobalEvent(Macros.Event.EquipModule.ONE_KEY_ENHANCE_EQUIP, update_hero_info)
	
	--圣器穿戴、卸下、更新
	self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_CROSS, update_hero_info)
	self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_DISCHARGE, update_hero_info)

	--卡牌穿戴、卸下
	self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DRESS, update_hero_info)
	self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DISCHARGE, update_hero_info)

	--精灵祝福
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_BLESS_RESP, update_hero_info)

	--英雄/援军上阵
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_ON_OR_OFF_FIGHT, function(is_on_fight)
		if is_on_fight then
			update_hero_info()
		end
	end)

	--援军上阵事件
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REINFORCEMENTS_ONLINE, function(_, pos)  
		if not self:IsOpen() then
			return
		end     
        self.__reinforcements_lineup_index = pos
        self:__ChangeSubView(BaseViewSubType.HeroLineup.SELECT, nil, Macros.Game.HeroLineup.REINFORCEMENT)
    end)

    --飘字预处理
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_WIND_WORDS_PREPROCESS, function(_, is_reinforcement)  
		if not self:IsOpen() then
			return
		end

		self._is_no_setting = false

		--刷新英雄数据
		if not is_reinforcement then
			if self.__templ_hero_info then
				local hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", self.__selected_lineup_index)
				self.__templ_hero_info:SetData(hero_info)
			end
		end

		--移除飘字特效
		if self.__wind_words_layout then
			self.__wind_words_layout:RemoveSelf(true)
			self.__wind_words_layout = nil		
			self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)
		end
		self:__DeleteTypeEffect()
    end)    
end

function Modules.HeroLineupView:__Dispose()
	self:__CloseCallback()	

	if self.__scrollview_lineup_list then
		self.__scrollview_lineup_list:DeleteMe()
		self.__scrollview_lineup_list = nil
	end

	if self.__scrollview_select_list then
		self.__scrollview_select_list:DeleteMe()
		self.__scrollview_select_list = nil
	end
end

--英雄上阵反馈
function Modules.HeroLineupView:HeroGotoBattleResp()
	if self.__sub_type == BaseViewSubType.HeroLineup.SELECT then
		self:__ChangeSubView(BaseViewSubType.HeroLineup.MAIN)
	end
end

--援军上阵反馈
function Modules.HeroLineupView:HeroGotoReinforcementView()
	if self.__sub_type == BaseViewSubType.HeroLineup.SELECT then
		self:__ChangeSubView(BaseViewSubType.HeroLineup.REINFORCEMENTS)
	end
end

--获取打开参数
function Modules.HeroLineupView:GetOpenParams()
	if self.__sub_type then
		return {self.__sub_type, self.__selected_lineup_index}
	end
end

--切换子界面
function Modules.HeroLineupView:__ChangeSubView(type, lineup_index, lineup_type, equip_type,is_on_divination_view)
	self.__is_return_reinforcement = false
	--判断是否返回援军界面
	if self.__sub_type and self.__sub_type == BaseViewSubType.HeroLineup.REINFORCEMENTS and type and type == BaseViewSubType.HeroLineup.SELECT then
		self.__is_return_reinforcement = true
	end
	self.__sub_type = type or BaseViewSubType.HeroLineup.MAIN
	self.__equip_type = equip_type
	--是否在占卜界面			
	self.__is_on_divination_view = self.__is_on_divination_view or is_on_divination_view
	
	if self.__panel_now then
		self.__panel_now:SetVisible(false)
		self.__panel_now = nil
	end
	self.__panel_lineups:SetVisible(false)
	if self.__templ_Reinforcements then
		self.__templ_Reinforcements:SetVisible(false)
	end
	if self.__reinforcements_selectedimage then
		self.__reinforcements_selectedimage:SetVisible(false)
	end

	local title = ""
	CPPGameLib.Switch(self.__sub_type)
	{
		[BaseViewSubType.HeroLineup.MAIN] = function()
			title = CPPGameLib.GetString("hero_lineup_title")

			self:__HandleLineupList()
			self.__panel_now = self.__panel_main
			self:__HandleMain(lineup_index)
		end,
		[BaseViewSubType.HeroLineup.SELECT] = function()
			title = CPPGameLib.GetString("hero_lineup_select_title")

			self.__panel_now = self.__panel_select
			self.__lineup_type = lineup_type or Macros.Game.HeroLineup.HERO_LINEUP
			self:__HandleSelect()
		end,
		[BaseViewSubType.HeroLineup.REINFORCEMENTS] = function()
			title = CPPGameLib.GetString("hero_reinforcements_title")			

			self:__HandleLineupList()
			self.__panel_now = self.__panel_reinforcements
			self:__HandleReinforcements()
		end,
	}

	if self.__panel_now then
		self.__panel_now:SetVisible(true)
	end

	self:SetTitle(title)
end

--------------------------------------------------------------------------------------------------------------------------------
--阵容界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.HeroLineupView:__HandleMain(lineup_index)
	--选中指定阵位/首个上阵阵位
	self.__selected_lineup_index = lineup_index or self.__selected_lineup_index or self:CallModuleFunc("GetFirstLineupPos")
	
	if self.__selected_lineup_index then
		self.__scrollview_lineup_list:SelectItem(self.__selected_lineup_index)
		if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			self.__scrollview_lineup_list:LocateItem(self.__selected_lineup_index)
		end
		self:CallModuleFunc("__SetSelectedLineupIndex", self.__selected_lineup_index)
	end

	--是否需要飘字
	if self:CallModuleFunc("GetIsHaveChangePropertyNeedWindWords") then
		self:__WindWords()
	end

	if not self.__main_view_inited then
		--判断卡牌系统是否开启
		local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.DIVINATION)
		--卡牌容器
		local panel_13_2 = self:GetWidget("Panel_13_2")
		panel_13_2:SetVisible(is_open)
		--强化大师
		local panel_13 = self:GetWidget("Panel_13")
		panel_13:SetPosition(is_open and panel_13:GetPositionX() or panel_13_2:GetPositionX(),panel_13:GetPositionY())
		self.__btn_forging = self:GetWidget("Button_Forging")
		self.__txt_forging = self:GetWidget("Text_Forging")
		Util:WidgetLabel(self.__txt_forging, CPPGameLib.GetString("hero_lineup_btn1"), Macros.TypefaceSize.normal, nil, nil, nil, Macros.Color.common_stroke)
		Util:WidgetButton(self.__btn_forging, nil, function()
			local lineup_info = self:CallModuleFunc("GetLineupInfo", self.__selected_lineup_index)
			if not lineup_info then
				return
			end
			if self.__is_on_divination_view then
				--前往占卜
                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DIVINATION)
                return
			end
			--强化大师界面		
			if ( lineup_info.hat_equipment_uid and lineup_info.hat_equipment_uid ~= 0 and
			   lineup_info.clothes_equipment_uid and lineup_info.clothes_equipment_uid ~= 0 and
			   lineup_info.belt_equipment_uid and lineup_info.belt_equipment_uid ~= 0 and
			   lineup_info.weapon_equipment_uid and lineup_info.weapon_equipment_uid ~= 0) or 
			   (lineup_info.attack_treasure_uid and lineup_info.attack_treasure_uid ~= 0 and 
			   	lineup_info.defence_treasure_uid and lineup_info.defence_treasure_uid ~= 0 ) then
				   Modules.TemplHeroForging.New(self.__selected_lineup_index)
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("equip_forgeOpen"))
			end

		end, Macros.TypefaceSize.normal)

		--切换英雄
		self.__panel_switch = self:GetWidget("Panel_13_0")
		local btn_switch = self:GetWidget("Button_Switch")
		btn_switch:IgnoreContentAdaptWithSize(true)
		Util:WidgetLabel(self:GetWidget("Text_Switch"), CPPGameLib.GetString("hero_lineup_btn2"), Macros.TypefaceSize.normal, nil, nil, nil, Macros.Color.common_stroke)
		Util:WidgetButton(btn_switch, nil, function()
			--英雄上阵界面
			self:__ChangeSubView(BaseViewSubType.HeroLineup.SELECT, self.__selected_lineup_index)
		end, Macros.TypefaceSize.normal)

		--一键强化
		local panel_13_1 = self:GetWidget("Panel_13_1")
		panel_13_1:SetPosition(is_open and panel_13_1:GetPositionX() or 442,panel_13_1:GetPositionY())
		local btn_onekey = self:GetWidget("Button_oneKey")
		btn_onekey:IgnoreContentAdaptWithSize(true)
		Util:WidgetLabel(self:GetWidget("Text_oneKey"), CPPGameLib.GetString("hero_lineup_btn3"), Macros.TypefaceSize.normal, nil, nil, nil, Macros.Color.common_stroke)
		Util:WidgetButton(btn_onekey, nil, function()
			--判断是否正在卡牌界面
			if self.__is_on_divination_view then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_change_the_view_tips"))
				return
			end
			local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")
			local vipLevel_ = ConfigAdapter.Common.GetOneKeyIntensifyVipLevel()

			local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			local roleLevel_ = ConfigAdapter.Common.GetOneKeyIntensifyRoleLevel()

			if (vipLevel >= vipLevel_) or (roleLevel >= roleLevel_) then
				local _ , __ , notype = CallModuleFunc(ModuleType.EQUIP, "OneKeyIntensify" , self.__selected_lineup_index )

				if notype == "nogold" then
                    self.gold = CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD , function ( ... )
                        self.gold = nil
                    end)
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
				else
					LogicGUI.TemplEquipOneKey.New( self.__selected_lineup_index , function ( ... )
						if not self.__shield_layout then
							self.__shield_layout = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height ,function ( ... )
							end)
							self:GetWidget("Panel_Main"):AddChild( self.__shield_layout )
						end
					end)
				end
			elseif vipLevel < vipLevel_ and roleLevel < roleLevel_ then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_train_times_conditional" , vipLevel_ , roleLevel_ ) )
			end

		end, Macros.TypefaceSize.normal)	

		--切换卡牌或者装备
		self.__btn_morefunction = self:GetWidget("Button_MoreFunction")		

		--红点
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.__morefunction_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.__morefunction_red_dot:SetAnchorPoint(1, 1)
		self:GetWidget("Panel_13_2"):AddChild(self.__morefunction_red_dot,2)
		PosTool.RightTop(self.__morefunction_red_dot)
		self:__UpdateMoreFunctionBtnReddot()

		--按钮文字
		self.__txt_morefunction = self:GetWidget("Text_MoreFunction")
		Util:WidgetLabel(self.__txt_morefunction, CPPGameLib.GetString("hero_lineup_btn5"), Macros.TypefaceSize.normal, nil, nil, nil, Macros.Color.common_stroke)
		Util:WidgetButton(self.__btn_morefunction, nil, function()
			--是否在占卜界面			
			self.__is_on_divination_view = not self.__is_on_divination_view
			--切换按钮状态和文字
			self:__SwitchBtnStatusAndText()		
		end, Macros.TypefaceSize.normal,nil,nil,nil)
		self.__main_view_inited = true
	end
	--切换按钮状态和文字
	self:__SwitchBtnStatusAndText()	
end

--------------------------------------------------------------------------------------------------------------------------------
--选择英雄界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.HeroLineupView:__HandleSelect()	
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
	        	if self.__lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then
	        		local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", self.__selected_lineup_index)
	        		print("xy" , "=1-058329tiuowejnfvlsdkj")
	        		if hero_info then
	        			CallModuleFunc(ModuleType.EQUIP, "SetEquipHero", hero_info.info_id , item_data.hero_info.info_id )
	        			CallModuleFunc(ModuleType.TREASURE, "SetTreasureHero", hero_info.info_id , item_data.hero_info.info_id )
	        		end					
	        		--英雄上阵
	        		self:CallModuleFunc("HeroGotoBattleReq", item_data.hero_info.uid, self.__selected_lineup_index)   
	        	else
	        		--援军上阵
	        		self:CallModuleFunc("RecinforcementToBattleReq", item_data.hero_info.uid, self.__reinforcements_lineup_index,true) 
	        	end     
	        end,
	    }
	    self.__scrollview_select_list = GUI.ScrollView.New(params)
	    panel_select_list:AddChild(self.__scrollview_select_list:GetNode())
	    PosTool.Center(self.__scrollview_select_list)	
	end

	local hero_info_list = nil
	if self.__lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then
		local lineup_info = self:CallModuleFunc("GetLineupInfo", self.__selected_lineup_index)
		local hero_info = self:CallModuleFunc("GetHeroInfo", lineup_info.uid)
		hero_info_list = self:CallModuleFunc("GetLineupSelectList",hero_info and hero_info.info_id or nil, self.__lineup_type, self.__selected_lineup_index)  
	else
		local lineup_info = self:CallModuleFunc("GetReinforcementLineupInfo", self.__reinforcements_lineup_index)
		local hero_info = self:CallModuleFunc("GetHeroInfo", lineup_info.uid)
		hero_info_list = self:CallModuleFunc("GetLineupSelectList", hero_info and hero_info.info_id or nil, self.__lineup_type, self.__reinforcements_lineup_index)  
	end     
    self.__scrollview_select_list:SetDataList(hero_info_list)
end

--------------------------------------------------------------------------------------------------------------------------------
--援军界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.HeroLineupView:__HandleReinforcements()
	if not self.__reinforcements_selectedimage then
		local selectedImage = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
		self.__reinforcements_selectedimage = Util:Sprite(selectedImage) 
		self:GetWidget("Image_2"):AddChild(self.__reinforcements_selectedimage)  	
		PosTool.Center(self.__reinforcements_selectedimage)				
	end
	self.__reinforcements_selectedimage:SetVisible(true)

	--英雄援军
	if not self.__templ_Reinforcements then
		self.__templ_Reinforcements = LogicGUI.TemplHeroReinforcements.New(self:GetWidget("Panel_Reinforcements"))		
	else
		self.__templ_Reinforcements:SetVisible(true)
		self.__templ_Reinforcements:OpenCallbackEx()
	end
	--是否需要飘字
	if self:CallModuleFunc("GetIsHaveChangePropertyNeedWindWords") then
		self:__WindWords()
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--阵位列表
--------------------------------------------------------------------------------------------------------------------------------
function Modules.HeroLineupView:__HandleLineupList()
	self.__panel_lineups:SetVisible(true)

	--阵位列表
	if not self.__lineup_list_inited then
		--阵位列表
		local panel_lineup_list = self:GetWidget("Panel_LineupList")
		local size = panel_lineup_list:GetContentSize()
	    local params = 
	    {
	        item_class = Modules.HeroLineupItem,
	        item_width = 88,
	        item_height = 76,
	        row = 5,
	        col = 1,
	        item_space_r = 16,
	        view_height = size.height,
	        selectable = true,
	        select_filter_func = function(item_index, item_data)
				local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(item_index)
				local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
				if pos_info and pos_info.need_level > role_info.level then
					--未开启
					return false
				end
				return true
	        end,
	        select_callback = function(item_index, item_data)
				--阵位选中回调
				if self.__sub_type == BaseViewSubType.HeroLineup.MAIN then
					--切换英雄
					self:__SelectLineupPos(item_index, not item_data.icon_data)
				else
					--返回主页面
					self:__ChangeSubView(BaseViewSubType.HeroLineup.MAIN)
				end  
	        end,
	    }
	    self.__scrollview_lineup_list = GUI.ScrollView.New(params)
	    panel_lineup_list:AddChild(self.__scrollview_lineup_list:GetNode())
	    PosTool.Center(self.__scrollview_lineup_list)

		--援军入口
		self.__reinforcements_entry = self:GetWidget("Image_2")
		local Text_Reinforcements = Util:RichText(string.format(Macros.Color.system_headline_stroke_rt, CPPGameLib.GetString("hero_reinforcements_title")), Macros.TypefaceSize.normal)
		Text_Reinforcements:SetAnchorPoint(0.5,0.5)
		self.__reinforcements_entry:AddChild(Text_Reinforcements)
		PosTool.CenterBottom(Text_Reinforcements,0,7)	
		if not self.__reinforcements_selectedimage then
			local selectedImage = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
			self.__reinforcements_selectedimage = Util:Sprite(selectedImage) 
			self.__reinforcements_entry:AddChild(self.__reinforcements_selectedimage)  	
			PosTool.Center(self.__reinforcements_selectedimage)
			self.__reinforcements_selectedimage:SetVisible(false)
		end
		--援军开启等级提示
		self:__CheckReinforcementsOpen()

		self.__button_reinforcements = Util:WidgetButton(self:GetWidget("Button_Reinforcements"), nil, function()
			local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.REINFORCEMENT)
			if not is_open then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
				return 
			end
			self:__ChangeSubView(BaseViewSubType.HeroLineup.REINFORCEMENTS)
		end)

		self.__lineup_list_inited = true
	end
	--新手指引特殊操作检测（阵容-援军）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LEINUP_REINFORCEMENTS_ENTRY, self.__button_reinforcements)						

	local lineup_list = self:__GetLineupList()
	self.__scrollview_lineup_list:SetDataList(lineup_list)

	--援军开启
	self:__CheckReinforcementsOpen()

	--新手指引特殊操作检测
	self:__CheckSectionOperateSP()
end

--获取阵位列表
function Modules.HeroLineupView:__GetLineupList()
	local list = {}
	local lineup_list = self:CallModuleFunc("GetLineupList")
	for i, lineup_info in ipairs(lineup_list) do
		local data = {}
		data.index = i
		table.insert(list, data)
	end
	return list
end

--选择阵位
function Modules.HeroLineupView:__SelectLineupPos(lineup_index, to_lineup)
	--阵位反选效果
	if self.__hero_icon_list and self.__selected_lineup_index then
		self.__hero_icon_list[self.__selected_lineup_index]:SetSelected(false)
	end
	if not self.is_need_show then
		if self.__wind_words_layout then
			self.__wind_words_layout:RemoveSelf(true)
			self.__wind_words_layout = nil		
			self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)
		end
		self:__DeleteTypeEffect()
		self._is_no_setting = false
	else
		self.is_need_show = nil
	end

	self.__selected_lineup_index = lineup_index
	self:CallModuleFunc("__SetSelectedLineupIndex", self.__selected_lineup_index)

	local hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", self.__selected_lineup_index)
	if not hero_info then
		if to_lineup and not self.__select_lineup_for_main then
			--英雄上阵界面
			self:__ChangeSubView(BaseViewSubType.HeroLineup.SELECT, self.__selected_lineup_index)
		end
		return
	end

	--阵位选中效果
	if self.__hero_icon_list then
		self.__hero_icon_list[self.__selected_lineup_index]:SetSelected(true)
	end
	
	if not self._is_no_setting then
		--英雄信息
		if not self.__templ_hero_info then
			self.__templ_hero_info = LogicGUI.TemplHeroInfo.New(self:GetWidget("Panel_Info"))
		end
		self.__templ_hero_info:SetData(hero_info)
	end

	--切换装备与卡牌
	self:__SwitchEquipAndDivination()
end

--切换装备与卡牌
function Modules.HeroLineupView:__SwitchEquipAndDivination()
	self.__panel_equiplist:SetVisible(not self.__is_on_divination_view)
	self.__panel_divination:SetVisible(self.__is_on_divination_view)
	--是否在卡牌界面
	if self.__is_on_divination_view then
		--英雄占卜卡牌信息
		if not self.__templ_divination_info then
			self.__templ_divination_info = LogicGUI.TemplHeroDivinationInfo.New(self.__panel_divination)
		end			

		local lineup_info = self:CallModuleFunc("GetLineupInfo", self.__selected_lineup_index)
		self.__templ_divination_info:SetData({
			lineup_index = self.__selected_lineup_index,
			lineup_info = lineup_info,		
		})
	else
		--英雄阵位信息
		if not self.__templ_lineup_info then
			self.__templ_lineup_info = LogicGUI.TemplHeroLineupInfo.New(self.__panel_equiplist)
		end			

		local lineup_info = self:CallModuleFunc("GetLineupInfo", self.__selected_lineup_index)
		self.__templ_lineup_info:SetData({
			lineup_index = self.__selected_lineup_index,
			lineup_info = lineup_info,		
		},self.__equip_type)
	end
end

--切换按钮状态和文字
function Modules.HeroLineupView:__SwitchBtnStatusAndText()
	--切换图标
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_LINEUP, self.__is_on_divination_view and "switch_equip" or "switch_card")
	self.__btn_morefunction:SetNormalImage(sp)
	--强化图标
	sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_LINEUP, self.__is_on_divination_view and "achieve_divination" or "master_icon")
	self.__btn_forging:SetNormalImage(sp)
	--文字切换
	self.__txt_morefunction:SetText(self.__is_on_divination_view and CPPGameLib.GetString("hero_lineup_btn4") or CPPGameLib.GetString("hero_lineup_btn5"))
	--强化文字
	self.__txt_forging:SetText(self.__is_on_divination_view and CPPGameLib.GetString("hero_lineup_btn6") or CPPGameLib.GetString("hero_lineup_btn1"))

	--更新卡牌/装备切换按钮红点
	self:__UpdateMoreFunctionBtnReddot()

	--切换界面
	self:__SwitchEquipAndDivination()
end

--更新卡牌/装备切换按钮红点
function Modules.HeroLineupView:__UpdateMoreFunctionBtnReddot()
	local reddot_num = 0
	if self.__is_on_divination_view then
		reddot_num = CallModuleFunc(ModuleType.HERO,"GetHeroEquipRedDotNumByLineUpIndex",self.__selected_lineup_index)
	else
		local reddot_table = CallModuleFunc(ModuleType.HERO,"GetHeroDivinationRedDotNumByLineUpIndex",self.__selected_lineup_index) 
		for i,cur_reddot in ipairs(reddot_table) do
			reddot_num = reddot_num + cur_reddot
		end
	end			
	self.__morefunction_red_dot:SetVisible(reddot_num > 0)
end

--新手指引特殊操作检测(英雄上阵按钮)
function Modules.HeroLineupView:__CheckSectionOperateSP()
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
	if operate_id == Macros.Game.LeadOperateSpecialID.HERO_LINEUP_BTN then
		local locate_index
		local lineup_list = self:CallModuleFunc("GetLineupList")
		for i, lineup_info in ipairs(lineup_list) do
			if lineup_info.uid <= 0 then
				locate_index = i
				break
			end
		end
		if locate_index then
			self.__scrollview_lineup_list:LocateItem(locate_index, true)
			local item = self.__scrollview_lineup_list:GetItem(locate_index)
			if item then
				item:CheckSectionOperateSP()
			end
		end
	end

end

--删除滚字特效
function Modules.HeroLineupView:__DeleteTypeEffect()
	for prop_type=1,4 do
		if self["__typeeffect_"..prop_type] then
			self["__typeeffect_"..prop_type]:DeleteMe()
			self["__typeeffect_"..prop_type] = nil
		end
	end
end

--援军开放检测
function Modules.HeroLineupView:__CheckReinforcementsOpen()
	local module_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.REINFORCEMENT)
	local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.REINFORCEMENT )
	if IsOpen then
		if self.__sp_lock and self.__lab_open_lev then
			self.__sp_lock:RemoveSelf(true)
			self.__sp_lock = nil
			self.__lab_open_lev:RemoveSelf(true)
			self.__lab_open_lev = nil			
		end
		--红点
		self:__UpdateReinforcementsRedDot()
	else
		if not self.__sp_lock then
			--锁
			self.__sp_lock = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))		
			self.__reinforcements_entry:AddChild(self.__sp_lock)
			PosTool.Center(self.__sp_lock)
			--开启等级提示
		    self.__lab_open_lev = Util:Label(nil, nil, Macros.Color.btn, 50,0,Macros.Color.button_yellow)
		    self.__lab_open_lev:SetAnchorPoint(0.5, 0.5)
		    self.__lab_open_lev:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
		    self.__reinforcements_entry:AddChild(self.__lab_open_lev)
		    PosTool.Center(self.__lab_open_lev)			
			self.__lab_open_lev:SetString(string.format(CPPGameLib.GetString("common_open_lev"), module_info.level))
		end
	end
end

--添加红点
function Modules.HeroLineupView:__AddReinforcementsRedDot()
	--红点
	if not self.entry_red_dot then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.entry_red_dot:SetAnchorPoint(1, 1)
		self.__reinforcements_entry:AddChild(self.entry_red_dot,2)
		PosTool.RightTop(self.entry_red_dot)
	end
	self.entry_red_dot:SetVisible(true)
end

--隐藏红点
function Modules.HeroLineupView:__HideReinforcementsRedDot()
	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

--更新援军红点
function Modules.HeroLineupView:__UpdateReinforcementsRedDot()
	local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.REINFORCEMENT)
	if red_num > 0 then
		self:__AddReinforcementsRedDot()
	else
		self:__HideReinforcementsRedDot()
	end
end

--当前选中英雄的属性变化飘字
function Modules.HeroLineupView:__WindWords()
	if self.__wind_words_layout then
		self.__wind_words_layout:RemoveSelf(true)
		self.__wind_words_layout = nil
	end

	--改变之前英雄属性 缘分数量 数据类型
	local before_change_hero_info, reinforcement_name_list, message_type, lineup_type, is_need_show, before_strengthen_list, equipment_uid, cur_reinforcement_cheerlevel = self:CallModuleFunc("GetHeroPropertyBeforeChange")
	if not before_change_hero_info then
		self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)
		return
	end

	self.is_need_show = is_need_show 
	self:__DeleteTypeEffect()
	--英雄信息
	if not self.__templ_hero_info then
		self.__templ_hero_info = LogicGUI.TemplHeroInfo.New(self:GetWidget("Panel_Info"))
	end
	self.__selected_lineup_index = self.__selected_lineup_index or self:CallModuleFunc("__GetSelectedLineupIndex")
	self.__selected_lineup_index = lineup_type and lineup_type == Macros.Game.WIND_WORDS_LINEUP_TYPE.REINFORCEMENT_MESSAGE and 1 or self.__selected_lineup_index
	local now_hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", self.__selected_lineup_index)
	local is_need_property = true
	local is_reinforcement = false
	if lineup_type then
		if lineup_type == Macros.Game.WIND_WORDS_LINEUP_TYPE.LINEUP_MESSGAE then
			before_change_hero_info.level = now_hero_info.level
			before_change_hero_info.break_level = now_hero_info.break_level
			before_change_hero_info.prop_constructed = true
			self.__templ_hero_info:SetData(before_change_hero_info)
		else
			self.__templ_hero_info:SetData(now_hero_info)
			is_need_property = false
			is_reinforcement = true
		end
	else
		self.__templ_hero_info:SetData(before_change_hero_info)
	end	
	self._is_no_setting = true	
	local lablist = {}
	local config = CPPGameLib.GetConfig("Hero",now_hero_info.info_id)	
	if not is_reinforcement then
		self.__wind_words_layout = Util:Layout(100,100)
		self.__wind_words_layout:SetAnchorPoint(0.5,0.5)
		self:GetWidget("Panel_Main"):AddChild(self.__wind_words_layout,2)
		self.__wind_words_layout:SetPosition(388,344)
	else
		if not self.__templ_Reinforcements then
			self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)			
			return
		end
		self.__wind_words_layout = Util:Layout(100,100)
		self.__wind_words_layout:SetAnchorPoint(0.5,0.5)
		self.__templ_Reinforcements:GetNode():AddChild(self.__wind_words_layout,2)
		PosTool.Center(self.__wind_words_layout)
	end
	local interval = 14
	--统计数量计算容器大小
	local total = 0	
	--运动完做特殊动作
	local is_run_reinforcement,is_run_property,is_run_refine,is_run_reinforcement_special,is_run_reinforcement_cheer
	--缘分条数
	if reinforcement_name_list then
		total = total + #reinforcement_name_list
		if not is_reinforcement then
			is_run_reinforcement = true
		else
			is_run_reinforcement_special = true
		end		
	end
	--锻造大师条数
	local strengthen_level_min,refine_level_min
	--激活套装数量
	local light_up_num,pos_list,suit_name,equipInfo_quality
	--装备或者圣器
	if message_type then
		local _, lineup_equip_map, lineup_treasure_map = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",self.__selected_lineup_index)
		--套装信息
		if message_type == Macros.Game.ObjType.EQUIP then
			light_up_num,pos_list,suit_name,equipInfo_quality = CallModuleFunc(ModuleType.EQUIP,"GetLightUpSuitNum",equipment_uid,lineup_equip_map)
			if light_up_num >= 2 then
				total = total + 1
				is_run_refine = true
			end
		end
		local plus_prop_map
		if message_type == Macros.Game.ObjType.EQUIP then		   
		    plus_prop_map,strengthen_level_min,refine_level_min = ConfigAdapter.Forge.GetEquipPlusPropMap(lineup_equip_map)    
		else
			plus_prop_map,strengthen_level_min,refine_level_min = ConfigAdapter.Forge.GetTreasurePlusPropMap(lineup_treasure_map)
		end		
		strengthen_level_min = strengthen_level_min or 0
		refine_level_min = refine_level_min or 0
		if before_strengthen_list ~= nil and (strengthen_level_min ~= before_strengthen_list[1]) and (strengthen_level_min > 0) then
			is_run_refine = true
			total = total + 1
		end
		if before_strengthen_list ~= nil and (refine_level_min ~= before_strengthen_list[2]) and (refine_level_min > 0) then
			is_run_refine = true
			total = total + 1
		end		
	end
	--变化属性条数
	if is_need_property then
		for i, prop_type in ipairs(self.prop_type_list) do
			prop_key = Macros.Game.BattlePropKey[prop_type]
			if before_change_hero_info[prop_key] - now_hero_info[prop_key] ~= 0 then
				is_run_property = true
				total = total + 1
			end
		end
	end
	--援军助威
	if cur_reinforcement_cheerlevel then
		total = total + 5
		is_run_reinforcement_cheer = true
	end
	local height = 26 * total + interval * (total - 1)
	self.__wind_words_layout:SetContentSize(500,height)	
	total = 0	
	--激活套装数量
	if light_up_num and light_up_num >= 2 then
		self["__rich_text_"..total] = Util:RichText(string.format(Macros.Quality_Stroke_RT[equipInfo_quality],"["..suit_name.."]")..CPPGameLib.GetString("hero_lineup_equip_collect_tips",light_up_num),26,0,0,Macros.Color.lilyWhite_hex,Macros.Color.common_stroke_hex)
		self["__rich_text_"..total]:SetAnchorPoint(0.5,1)			
		self.__wind_words_layout:AddChild(self["__rich_text_"..total])
		self["__rich_text_"..total]:SetPosition(self.__wind_words_layout:GetContentWidth()/2,height - (total - 1)*(self["__rich_text_"..total]:GetContentHeight() + interval))
		local info = {}
		info.node = self["__rich_text_"..total]
		info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
		local tgt_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__panel_main, self.__wind_words_layout, COCOPoint(355,107))
		info.tgt_pos = {x = tgt_pos.x,y = tgt_pos.y}
		table.insert(lablist,info)
	end
	--缘分
	local reinforcement_node_list = {}
	if reinforcement_name_list then
		for i,reinforcement_name_info in ipairs(reinforcement_name_list) do
			total = total + 1
			local hero_name = reinforcement_name_info.hero_name
			self["__rich_text_"..total] = Util:RichText(CPPGameLib.GetString("hero_linup_reinforcement_wind_words","["..hero_name.."]","["..reinforcement_name_info.reinforcementname.."]"),26,0,0,Macros.Color.lilyWhite_hex,Macros.Color.common_stroke_hex)
			self["__rich_text_"..total]:SetAnchorPoint(0.5,1)			
			self.__wind_words_layout:AddChild(self["__rich_text_"..total])
			self["__rich_text_"..total]:SetPosition(self.__wind_words_layout:GetContentWidth()/2 + (is_reinforcement and 100 or 0),height - (total - 1)*(self["__rich_text_"..total]:GetContentHeight() + interval))
			--文字控件
			local lab_reinforcement,lab_index
			--信息模板滚动容器
			if self.__templ_hero_info.__reinforcement_scrollview then
				--寻找对应的条目
				self.__templ_hero_info.__reinforcement_scrollview:ItemDataIter(function (index, data,item)
					if data.pos == reinforcement_name_info.pos then
						lab_index = index
						return true
					end
				end)
				--滚到对应的条目
				if lab_index then
					self.__templ_hero_info.__reinforcement_scrollview:LocateItem(lab_index)
					local cur_item = self.__templ_hero_info.__reinforcement_scrollview:GetItem(lab_index)
					lab_reinforcement = cur_item and cur_item:GetLabReinforcementInfo() or nil
				end
			end		
			local info = {}
			info.node = self["__rich_text_"..total]	
			if lab_reinforcement then				
				if not is_reinforcement then
					info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
					local data = {}
					data.tgt_node = lab_reinforcement
					data.item_index = lab_index
					table.insert(reinforcement_node_list,data)
					local tgt_pos = NodeUnity.ConvertPosBetweenTwoContainer(WidgetUnity.GetWidgetByName(self.__templ_hero_info:GetNode(), "Panel_BasicProp3"), self.__wind_words_layout, COCOPoint(lab_reinforcement:GetPositionX(),lab_reinforcement:GetPositionY()))
					info.tgt_pos = {x = tgt_pos.x,y = tgt_pos.y}
				else
					info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE				
					info.tgt_pos = {x = self["__rich_text_"..total]:GetPositionX(),y = self["__rich_text_"..total]:GetPositionY() + 80}
				end			
				table.insert(lablist,info)
			else
				info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE				
				info.tgt_pos = {x = self["__rich_text_"..total]:GetPositionX(),y = self["__rich_text_"..total]:GetPositionY() + 80}
				table.insert(lablist,info)
			end
		end
	end
	--锻造大师		
	if is_run_refine then		
		for i=1,2 do
			local is_all_zero = true
			local tmp_level = (i == 1 and strengthen_level_min or refine_level_min)
			if (tmp_level ~= before_strengthen_list[i]) and (tmp_level > 0) then
				total = total + 1
				is_all_zero = false
				local str_type = (message_type == Macros.Game.ObjType.EQUIP) and "equip" or "treasure"
				local str_lv_type = i == 1 and "_intensify" or "_refining"
				self["__rich_text_"..total] = Util:RichText(CPPGameLib.GetString("hero_linup_"..str_type..str_lv_type.."_wind_words",tmp_level),26,0,0,Macros.Color.lilyWhite_hex,Macros.Color.common_stroke_hex)
			end
			if not is_all_zero then
				self["__rich_text_"..total]:SetAnchorPoint(0.5,1)
				self.__wind_words_layout:AddChild(self["__rich_text_"..total])
				self["__rich_text_"..total]:SetPosition(self.__wind_words_layout:GetContentWidth()/2,height - (total - 1)*(self["__rich_text_"..total]:GetContentHeight() + interval))
				local info = {}
				info.node = self["__rich_text_"..total]
				info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
				local tgt_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__panel_main, self.__wind_words_layout, COCOPoint(355,107))
				info.tgt_pos = {x = tgt_pos.x,y = tgt_pos.y}
				table.insert(lablist,info)
			end
		end				
	end	
	--基础属性	
	if is_need_property then
		local lab
		local labcolor
		local labStrokeColor
		local prop_key
		for i, prop_type in ipairs(self.prop_type_list) do
			prop_key = Macros.Game.BattlePropKey[prop_type]
			if before_change_hero_info[prop_key] - now_hero_info[prop_key] ~= 0 then
				lab = before_change_hero_info[prop_key] - now_hero_info[prop_key] < 0 and " + " or " - "
				labcolor = before_change_hero_info[prop_key] - now_hero_info[prop_key] < 0 and Macros.Color.green_hex or Macros.Color.red_hex 
				labStrokeColor = before_change_hero_info[prop_key] - now_hero_info[prop_key] < 0 and Macros.Color.green_stroke_hex or Macros.Color.red_stroke_hex 
				total = total + 1
				self["__rich_text_"..total] = Util:RichText(CPPGameLib.GetString("nature"..prop_type)..lab..math.abs(before_change_hero_info[prop_key] - now_hero_info[prop_key]),26,0,0,labcolor,labStrokeColor)
				self["__rich_text_"..total]:SetAnchorPoint(0.5,1)
				self.__wind_words_layout:AddChild(self["__rich_text_"..total])
				self["__rich_text_"..total]:SetPosition(self.__wind_words_layout:GetContentWidth()/2,height - (total - 1)*(self["__rich_text_"..total]:GetContentHeight() + interval))
				local info = {}
				info.node = self["__rich_text_"..total]
				info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
				local tgt_node = self.__templ_hero_info["lab_prop_"..prop_type]
				local tgt_pos = NodeUnity.ConvertPosBetweenTwoContainer(WidgetUnity.GetWidgetByName(self.__templ_hero_info:GetNode(), "Panel_BasicProp1"), self.__wind_words_layout, COCOPoint(tgt_node:GetPositionX(),tgt_node:GetPositionY()))
				info.tgt_pos = {x = tgt_pos.x,y = tgt_pos.y}
				table.insert(lablist,info)
			end
		end
	end
	--援军助威
	if cur_reinforcement_cheerlevel then
		local config_cheer = CPPGameLib.GetConfig("ReinforcementEncourage",nil,nil,true)
		local encoruage_info
		for _,v in ipairs(config_cheer) do
			if v.encourage_level == cur_reinforcement_cheerlevel then
				encoruage_info = v
				break
			end
		end
		if encoruage_info then
			for i=1,5 do
				total = total + 1
				if i <= 1 then
					self["__rich_text_"..total] = Util:RichText(CPPGameLib.GetString("reinforcements_cheerlevel_windwords_rt",tostring(cur_reinforcement_cheerlevel)),26,0,0,Macros.Color.lilyWhite_hex,Macros.Color.common_stroke_hex)
				else
					self["__rich_text_"..total] = Util:RichText(CPPGameLib.GetString("nature"..encoruage_info.pro_type_list[i-1]).." + "..encoruage_info.pro_value_list[i-1],26,0,0,Macros.Color.green_hex,Macros.Color.green_stroke_hex)
				end			
				self["__rich_text_"..total]:SetAnchorPoint(0.5,1)			
				self.__wind_words_layout:AddChild(self["__rich_text_"..total])
				self["__rich_text_"..total]:SetPosition(self.__wind_words_layout:GetContentWidth()/2+100,height - (total - 1)*(self["__rich_text_"..total]:GetContentHeight() + interval))
				local info = {}
				info.node = self["__rich_text_"..total]
				info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
				local tgt_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__panel_reinforcements, self.__wind_words_layout, COCOPoint(555,44))
				info.tgt_pos = {x = tgt_pos.x,y = tgt_pos.y}
				table.insert(lablist,info)
			end
		end				
	end
	--设置位置
	if total > 0 then			
		self.windwords_effects = WindWordsEffects.New()
		self.windwords_effects:Wind(lablist,function ()
			--基本属性
			if is_run_property then
				local is_roll = true
				for i, prop_type in ipairs(self.prop_type_list) do
					prop_key = Macros.Game.BattlePropKey[prop_type]
					if before_change_hero_info[prop_key] - now_hero_info[prop_key] ~= 0 then
						self["__typeeffect_"..prop_type] = TypefaceEffects.New()
						self["__typeeffect_"..prop_type]:Roll(self.__templ_hero_info["lab_prop_"..prop_type],now_hero_info[prop_key],nil,function ()
							self._is_no_setting = false	
						end)
					end
				end
			end
			--锻造大师
			if is_run_refine then
				local panel_intensify = WidgetUnity.GetWidgetByName(self.__panel_main, "Panel_13")
				local funid = GlobalCallbackMgr:AddCallBackFunc(function ()
					CPPActionManager.ScaleTo(panel_intensify,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
						self._is_no_setting = false
					end))
				end)
				CPPActionManager.ScaleTo(panel_intensify,0.3,1.3,funid)
			end
			--阵位缘分
			if is_run_reinforcement then
				for i,cur_info in ipairs(reinforcement_node_list) do
					local funid = GlobalCallbackMgr:AddCallBackFunc(function ()
						CPPActionManager.ScaleTo(cur_info.tgt_node,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
							if i == #reinforcement_node_list then
								self._is_no_setting = false
								self.__templ_hero_info:SetData(now_hero_info)
							end
						end))
					end)
					if self.__templ_hero_info.__reinforcement_scrollview then
						self.__templ_hero_info.__reinforcement_scrollview:LocateItem(cur_info.item_index or 1)
					end
					CPPActionManager.ScaleTo(cur_info.tgt_node,0.3,1.3,funid)
				end
			end
			--延时
			CPPActionManager.DelayTo(self.__templ_hero_info:GetNode(),1,GlobalCallbackMgr:AddCallBackFunc(function ()
				self.__templ_hero_info:SetData(now_hero_info)
			end))
			--援军缘分
			if is_run_reinforcement_special then
				self._is_no_setting = false
			end
			--援军助威
			if is_run_reinforcement_cheer then
				self._is_no_setting = false
			end
		end)
	else
		if self.__wind_words_layout then
			self.__wind_words_layout:RemoveSelf(true)
			self.__wind_words_layout = nil
			self._is_no_setting = false
			self.__templ_hero_info:SetData(now_hero_info)
			self:CallModuleFunc("SetIsHaveChangePropertyNeedWindWords",false)
		end
	end
end
