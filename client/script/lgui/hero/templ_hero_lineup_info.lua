
--[[
%% @module：英雄阵位信息模板
%% @author: swordman sue
%% @created: 2016-08-23
注：需手动释放(DeleteMe)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroLineupInfo = LogicGUI.TemplHeroLineupInfo or BaseClass(GUI.Template)

function LogicGUI.TemplHeroLineupInfo:__init(parent_node)
	self:InitTemplate(GUI.TemplateRegistry.HERO_LINEUP_INFO, parent_node, true)

	self:__InitUI()

	--阵位更新
	self.__lineup_info_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_LINEUP_UPDATE, function( _ , data )
		local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo", self.__data.lineup_index)
		if lineup_info then
			self.__data.lineup_info = lineup_info
			self:SetData(self.__data)
		end
	end)

	--一键强化装备更新
	self.__equip_onekey_update_event = self:BindGlobalEvent(Macros.Event.EquipModule.ONE_KEY_ENHANCE_EQUIP, function( _ , data )		
		self:__OneKeyEnhanceUpdateEquip(data)	
	end)

	--圣器更新
	self.__treasure_update_event = self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_INTENSIFY, function( _ , data )		
		local treasure_info = CallModuleFunc(ModuleType.TREASURE,"GetTreasure", data.uid)
		self:__UpdateTreasure(treasure_info)
	end)

	--角色等级更新
	self.__role_level_update_event = self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function( _ , data )		
		self:__UpdateGridStatus()

		--原石
	    local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.GEM)
	    self.__btn_gem:SetVisible(is_open)
	end)

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self:__CheckSectionOperateSP()
	end)  	
end

function LogicGUI.TemplHeroLineupInfo:__delete()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end

	CPPGameLib.UnbindEvent(self.__lineup_info_event)
	self.__lineup_info_event = nil

	CPPGameLib.UnbindEvent(self.__equip_update_event)
	self.__equip_update_event = nil

	CPPGameLib.UnbindEvent(self.__equip_onekey_update_event)
	self.__equip_onekey_update_event = nil

	CPPGameLib.UnbindEvent(self.__treasure_update_event)
	self.__treasure_update_event = nil

	CPPGameLib.UnbindEvent(self.__role_level_update_event)
	self.__role_level_update_event = nil

	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
	    self.__delay_timer = nil
	end

	for equip_type=0,3 do
		if self["__equip_suit_effectid"..equip_type] then
			self:__DelEffect(self["__equip_suit_effectid"..equip_type])
			self["__equip_suit_effectid"..equip_type] = nil
		end
	end	

	if self.__spirit_bless_model then 
		self.__spirit_bless_model:DeleteMe()
		self.__spirit_bless_model = nil
	end

	--精灵上特效
	if self.__bless_spirit_effectid then
		self:__DelEffect(self.__bless_spirit_effectid)
		self.__bless_spirit_effectid = nil
	end
	--精灵下特效
	if self.__down_bless_spirit_effectid then
		self:__DelEffect(self.__down_bless_spirit_effectid)
		self.__down_bless_spirit_effectid = nil
	end
end

--[[
@ data：
	lineup_index
	lineup_info
--]]
function LogicGUI.TemplHeroLineupInfo:SetData(data,equip_type)
	if not data then
		LogTool.LogError("[LogicGUI.TemplHeroLineupInfo:SetData] 参数data无效")
		return
	end
	self.__data = data
	self.__equip_type = equip_type
	local lineup_info = self.__data.lineup_info

	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)
	if hero_info then
		local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
		if not config_hero then
			return
		end

		if hero_info.break_level > 0 then
			self.__txt_hero_name:SetText(CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
		else
			self.__txt_hero_name:SetText(config_hero.name)
		end
		self.__txt_hero_name:SetTextColor(Macros.QualityColor(hero_info.quality_level))
		self.__txt_hero_name:SetStrokeColor(Macros.QualityColorStroke(hero_info.quality_level))		
		self.__img_hero_pro:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..config_hero.profession, true))
		self.__text_herolevel:SetText(CPPGameLib.GetString("hero_level",hero_info.level))
		self.__img_camp:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_"..config_hero.camp, true))
		--资质
		local qualifications = ConfigAdapter.Hero.GetHeroQualifications(hero_info.quality_level,hero_info.info_id,hero_info.quality_stage)
		self.__text_herointelligence:SetText(CPPGameLib.GetString("hero_intelligence",qualifications))
		local isCouldUpgrade = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldUpgrade",hero_info.uid)  
		local isCouldBreak = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldBreak",hero_info.uid) 
		--英雄模型
		if not self.__ui_hero then
			self.__ui_hero = LogicGUI.Model.New({info_id = hero_info.info_id, parent = self.__panel_model})	
			self.__ui_hero:SetScaleFactor(1.2)
			self.__ui_hero:SetTouchRect(-100, 0, 200, 220)			
			PosTool.CenterBottom(self.__ui_hero:GetModel())

			--特效
			local effect_id,effect = self:__CreateEffect({res_name = "UI_zhenrongbeijing"}, self.__panel_model, -1)	
			PosTool.CenterBottom(effect)		
		else
			self.__ui_hero:SetInfoID(hero_info.info_id)
		end
		--缩放比例
		self.__ui_hero:SetScaleFactor(config_hero.ui_scale or 1)

		self.__ui_hero:SetTouchFun(nil, nil, function()
			local tab_index = isCouldBreak and BaseViewSubType.HeroPromote.BREAK or (isCouldUpgrade and BaseViewSubType.HeroPromote.UPGRADE or nil)
			CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, tab_index, self.__data.lineup_info.uid)
		end)
		
		if isCouldBreak or isCouldUpgrade then
			local str = isCouldBreak and CPPGameLib.GetString("hero_could_break") or CPPGameLib.GetString("hero_could_upgrade")
			self.__ui_hero:AddUpgradeTips(str,1.1,1.3)
		else
			self.__ui_hero:HideUpgradeTips()
		end

		--精灵祝福
	    local is_open,is_pre_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_BLESS)
	    self.__btn_bless:SetBright(true)
	    if is_open then
	    	self.__panel_spiritbless:SetVisible(lineup_info.bless_sprite_id ~= 0)
	    	self.__btn_bless:SetVisible(lineup_info.bless_sprite_id == 0)
	    	--显示精灵模型和祝福技能
	    	if lineup_info.bless_sprite_id ~= 0 then
	    		if not self.__spirit_bless_model then
	    			self.__spirit_bless_model = LogicGUI.Model.New({info_id = lineup_info.bless_sprite_id,model_type = LogicGUI.ModelType.SPRITE,parent = self.__panel_spiritbless})
	    			self.__spirit_bless_model:SetScaleFactor(0.7)    			
	    		else
	    			self.__spirit_bless_model:SetInfoID(lineup_info.bless_sprite_id)
	    		end
	    		self.__spirit_bless_model:SetPosition(30,lineup_info.bless_sprite_id == 50012 and -17 or 0)
	    		--祝福技能名字
	    		--TODO
	    		local sp_info = CPPGameLib.GetConfig("SpriteSpecialProperty",lineup_info.bless_sprite_id)
	    		local config_spirit = CPPGameLib.GetConfig("Spirit",lineup_info.bless_sprite_id)
	    		self.__txt_spiritblessname:SetText(sp_info.star_list[1] and sp_info.star_list[1].bless_skill_name or "")
	    		self.__txt_spiritblessname:SetColor(unpack(Macros.Quality[config_spirit.quality]))
	    		self.__txt_spiritblessname:SetStrokeColor(unpack(Macros.Quality_Stroke[config_spirit.quality]))
	    	end
	    	--精灵上特效
	    	if self.__bless_spirit_effectid then
	    		self:__DelEffect(self.__bless_spirit_effectid)
	    		self.__bless_spirit_effectid = nil
	    	end
	    	--精灵下特效
	    	if self.__down_bless_spirit_effectid then
	    		self:__DelEffect(self.__down_bless_spirit_effectid)
	    		self.__down_bless_spirit_effectid = nil
	    	end
	    	if lineup_info.bless_sprite_id ~= 0 then
	    		local spirit_config = CPPGameLib.GetConfig("Spirit",lineup_info.bless_sprite_id)
	    		--精灵下特效
	    		local effect
	    		if spirit_config and spirit_config.down_bless_effect_id then
					self.__down_bless_spirit_effectid,effect = self:__CreateEffect({id = spirit_config.down_bless_effect_id,scale = config_hero.ui_scale}, self.__panel_model, -1)	
					PosTool.CenterBottom(effect)	
				end
				--精灵上特效
	    		if spirit_config and spirit_config.bless_effect_id then
					self.__bless_spirit_effectid,effect = self:__CreateEffect({id = spirit_config.bless_effect_id,scale = config_hero.ui_scale}, self.__panel_model,2)	
					PosTool.CenterBottom(effect)	
				end
	    	end
	    else
	    	self.__panel_spiritbless:SetVisible(false)
	    	self.__btn_bless:SetVisible(false)
	    	if is_pre_open then
	    		self.__btn_bless:SetVisible(true)
	    		self.__btn_bless:SetBright(false)
	    	end	    	
	    end

	    --英雄进化入口	 
	    local is_open,is_pre_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_EVOLUTION)   
		self.__btn_evolution:SetBright(is_open)
		self.__btn_evolution:SetVisible(((is_open or is_pre_open) and not is_protagonist) and hero_info.quality_level >= Macros.Game.QualityType.ORANGE)
		--英雄觉醒
		--判断是否开启觉醒
		local IsOpenAwaken = self:JudgeIsOpenAwaken()
		--觉醒星级
		self:RefreshHeroAwakenStar(IsOpenAwaken,hero_info)

		--新手指引特殊操作检测
		self:__CheckSectionOperateSP()
	end

	--装备&&圣器格子
	local obj_info
	for key, _ in pairs(self.__grid_widget_info_map) do
		if self:__IsTreasureGrid(key) then
			obj_info = CallModuleFunc(ModuleType.TREASURE, "GetTreasure", lineup_info[key])
		else
			obj_info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", lineup_info[key])
		end
		self:__UpdateGrid(obj_info, key)
	end	
end

function LogicGUI.TemplHeroLineupInfo:__InitUI()
	self.__panel_model = self:GetWidget("Panel_HeroModel")

	--套装特效名
	self.__equip_suit_effect_list = {[1] = "UI_taozhuang_lvse",[2] = "UI_taozhuang_lanse",[3] = "UI_taozhuang_zise",[4] = "UI_taozhuang_chengse",[5] = "UI_taozhuang_hongse"}

	self.__txt_hero_name = self:GetWidget("Text_HeroName")
	self.__img_camp = self:GetWidget("Image_camp")
	self.__img_camp:IgnoreContentAdaptWithSize(true)
	self.__img_camp:SetScaleFactor(0.7)
	self.__img_hero_pro = self:GetWidget("Image_Profession")
	self.__text_herolevel = self:GetWidget("Text_HeroLevel")
	Util:WidgetLabel(self.__text_herolevel, "", Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.content_stroke)
	self.__text_herointelligence = self:GetWidget("Text_HeroIntelligence")
	Util:WidgetLabel(self.__text_herointelligence, "", Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.content_stroke)
	--原石入口
	self.__btn_gem = self:GetWidget("Button_gem")
	self.__btn_gem:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_gem"),TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_gem,"",function ()
        --打开原石界面
        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.GEM,{self.__data.lineup_info.uid})
    end)
    local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.GEM)
    self.__btn_gem:SetVisible(is_open)
    --原石入口文字
    Util:WidgetLabel(self:GetWidget("Text_gem"),CPPGameLib.GetString("hero_lineup_gem_txt"), Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
      
	--精灵祝福入口
	self.__btn_bless = self:GetWidget("Button_bless")
	self.__btn_bless:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_bless"),TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_bless,"",function ()
		--预告
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_BLESS)
	    if is_pre_open and not is_open then
	    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
	    	return
	    end
        --打开祝福界面
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS, false, 1, {lineup_index = self.__data.lineup_index,uid = self.__data.lineup_info.uid})
    end)
    --精灵祝福入口文字
    Util:WidgetLabel(self:GetWidget("Text_bless"),CPPGameLib.GetString("hero_lineup_bless_txt"), Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --精灵模型
    self.__panel_spiritbless = self:GetWidget("Panel_SpiritBless")
    self.__panel_spiritbless:SetPosition(self.__panel_spiritbless:GetPositionX(),self.__panel_spiritbless:GetPositionY()+8)
    WidgetUnity.SetWidgetListener(self.__panel_spiritbless, nil, nil,function ()
    	--打开精灵详情界面
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_DETAIL, false, self.__data)
    end)
    --祝福技能名字
    self.__txt_spiritblessname = self:GetWidget("Text_SpiritBlessName")
    self.__txt_spiritblessname:SetZOrder(20)
    Util:WidgetLabel(self.__txt_spiritblessname,"", Macros.TypefaceSize.slightly, Macros.Color.btn)

    --英雄进化入口
	self.__btn_evolution = self:GetWidget("Button_Evolution")
	self.__btn_evolution:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_bless"),TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_evolution,"",function ()
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_EVOLUTION)
		--预告
	    if is_pre_open and not is_open then
	    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
	    	return
	    end
        --打开进化界面
        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_EVOLUTION,{self.__data.lineup_info.uid})
    end)
    --英雄进化入口文字
    Util:WidgetLabel(self:GetWidget("Text_evolution"),CPPGameLib.GetString("hero_evolution_title2"), Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)

	self.__grid_type_map = 
	{
		["weapon_equipment_uid"] = Macros.Game.EquipType.WEAPON,
		["hat_equipment_uid"] = Macros.Game.EquipType.HELMET,
		["belt_equipment_uid"] = Macros.Game.EquipType.BELT,
		["clothes_equipment_uid"] = Macros.Game.EquipType.CLOTH,
		["attack_treasure_uid"] = Macros.Game.TreasureType.EMBLEM,
		["defence_treasure_uid"] = Macros.Game.TreasureType.GAD,
	}		

	local grid_widget_map =
	{
		["weapon_equipment_uid"] = "Weapon",
		["hat_equipment_uid"] = "Helmet",
		["belt_equipment_uid"] = "Belt",
		["clothes_equipment_uid"] = "Cloth",
		["attack_treasure_uid"] = "AttackingTreasure",
		["defence_treasure_uid"] = "DefenseTreasure",
	}
	self.__grid_widget_info_map = {}
	for key, grid_name in pairs(grid_widget_map) do
		local widget_info = {}
		widget_info.panel = self:GetWidget("Panel_"..grid_name)
		widget_info.txt_name = WidgetUnity.GetWidgetByName(widget_info.panel, "Text_"..grid_name)		

		if self:__IsTreasureGrid(key) then
			local image_slot = WidgetUnity.GetWidgetByName(widget_info.panel, "Image_Slot")
			image_slot:SetZOrder(8)
			Util:WidgetLabel(widget_info.txt_name, CPPGameLib.GetString("treasure_type"..self.__grid_type_map[key]), nil, nil, nil, nil, Macros.Color.common_stroke)
		else
			Util:WidgetLabel(widget_info.txt_name, CPPGameLib.GetString("equip_type"..self.__grid_type_map[key]), nil, nil, nil, nil, Macros.Color.common_stroke)
		end

		widget_info.btn_add = WidgetUnity.GetWidgetByName(widget_info.panel, "Button_"..grid_name)
		widget_info.btn_add:SetTouchEnabled(false)
		self.__grid_widget_info_map[key] = widget_info
	end
end

function LogicGUI.TemplHeroLineupInfo:__UpdateGrid(obj_info, key)
	local widget_info = self.__grid_widget_info_map[key]
	widget_info.txt_name:SetVisible(false)
	widget_info.btn_add:SetVisible(false)
	CPPActionManager.StopAllActions(widget_info.btn_add)
	WidgetUnity.SetWidgetListener(widget_info.panel)
	if widget_info.icon_equip then
		widget_info.icon_equip:SetVisible(false)
	end

	local lineup_info = self.__data.lineup_info

	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)

	local equipList
	--可穿戴的装备
	if self:__IsTreasureGrid(key) then
		equipList = CallModuleFunc(ModuleType.TREASURE, "GetTreasureListByType",self.__grid_type_map[key])			
	else
		equipList = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip",self.__grid_type_map[key])
	end
    if not obj_info then
    	--显示空格子
		widget_info.panel:SetBackGroundImageOpacity(255)

		widget_info.txt_name:SetVisible(true)

		--功能未开
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(self:__IsTreasureGrid(key) and Macros.Game.SystemID.TREASURE_DRESS or Macros.Game.SystemID.EQUIP_DRESS)
		if not is_open then
			widget_info.txt_name:SetVisible(false)
			if not widget_info.lock_icon then
				--锁
				widget_info.lock_icon = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
				widget_info.panel:AddChild(widget_info.lock_icon)	
				PosTool.Center(widget_info.lock_icon)
				--开启等级
				widget_info.lab_open_lev = Util:Label(nil, nil,Macros.Color.btn,50,0,Macros.Color.button_yellow)
			    widget_info.lab_open_lev:SetAnchorPoint(0.5, 0.5)
			    widget_info.lab_open_lev:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
			    widget_info.panel:AddChild(widget_info.lab_open_lev)
			    PosTool.Center(widget_info.lab_open_lev)			
				widget_info.lab_open_lev:SetString(string.format(CPPGameLib.GetString("common_open_lev"), open_level))
			end
		else
			widget_info.btn_add:SetVisible(true)
			ActionManager.GetInstance():CycleBlinkEffect(widget_info.btn_add, 0.5)
			if #equipList > 0 then 
				self:__AddRedDot(key)
			else
				self:__HideRedDot(key)
			end
		end		
		--隐藏
		self:__HideStrengthen(key)

		--穿戴按钮		
		WidgetUnity.SetWidgetListener(widget_info.panel, nil, nil, function()		
			local lineup_info = self.__data.lineup_info
			local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)		
			if self:__IsTreasureGrid(key) then
				 if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.TREASURE_DRESS,true) then
				 	return
				 end
				--打开圣器列表界面
				local treasureType = CallModuleFunc(ModuleType.TREASURE, "GetTreasureListByType",self.__grid_type_map[key])				
				if #treasureType > 0 then 
					GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.TREASURE_DRESS, {{hero_id = hero_info.info_id ,  index = self.__data.lineup_index, item_type = self.__grid_type_map[key]}})
				else
					local treasureId = CallModuleFunc(ModuleType.TREASURE, "ToTypeGetWeakestId",self.__grid_type_map[key])	
					GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, treasureId)
				end
			else
				if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.EQUIP_DRESS,true) then
				 	return
				 end
				--打开装备列表界面
				local equipType = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip",self.__grid_type_map[key])				
				if #equipType > 0 then 
					GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.EQUIP_DRESS, {{hero_id = hero_info.info_id , index = self.__data.lineup_index, item_type = self.__grid_type_map[key]}})
				else
					local equipId = CallModuleFunc(ModuleType.EQUIP, "ToTypeGetWeakestId",self.__grid_type_map[key])	
					GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, equipId)
				end
			end
		end)						
    else
		widget_info.panel:SetBackGroundImageOpacity(0)

		--显示装备、圣器
		if not widget_info.icon_equip then
			widget_info.icon_equip = LogicGUI.IconLayer.New(false)
			widget_info.icon_equip:SetTouchCallback(function()
				local lineup_info = self.__data.lineup_info
				local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)
				if self:__IsTreasureGrid(key) then
					--打开圣器详情界面
					local data = 
					{
						lineup_index = self.__data.lineup_index,
						item_type = self.__grid_type_map[key],
						treasure_uid = lineup_info[key],
						btn = true,
						hero_id = hero_info.info_id

					}

					local strengthenType = CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsStrengthen", lineup_info[key] )
					local refineType = CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsRefine", lineup_info[key] )
					local treasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetTreasure", lineup_info[key] )
					CallModuleFunc(ModuleType.TREASURE, "SetOnHeroInfo" , data )
					local betterLst = CallModuleFunc(ModuleType.TREASURE, "GetBetterTreasureList", treasureInfo )
				
					if refineType == "true" and #betterLst == 0 then
						CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_LIFT, 3, data)
					elseif strengthenType == "true" and #betterLst == 0 then
						CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_LIFT, 2, data)
					else
						CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_LIFT, 1, data)
					end
							
				else
					--打开装备详情界面
					local data = 
					{
						lineup_index = self.__data.lineup_index,
						item_type = self.__grid_type_map[key],
						equip_uid = lineup_info[key],
						btn = true,
						hero_id = hero_info.info_id
					}
					
					local strengthenType = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsStrengthen", lineup_info[key] )
					local refineType = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsRefine", lineup_info[key] )
					local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip", lineup_info[key] )
				    CallModuleFunc(ModuleType.EQUIP, "SetOnHeroInfo" , data )
					local betterLst = CallModuleFunc(ModuleType.EQUIP, "GetBetterEquip", equipInfo )
					
					if strengthenType == "true" and #betterLst == 0 then
						CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY, 2 , data)
					else
						CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY, 1 , data)
					end
				end
			end)
			widget_info.panel:AddChild(widget_info.icon_equip:GetNode())
			PosTool.Center(widget_info.icon_equip)								
		end

		--格子红点类型表
		self.__grid_reddot_type_map = self.__grid_reddot_type_map or {}

		--检测是否有更好的装备，并显示红点
		local is_have_reddot = CallModuleFunc(ModuleType.HERO, "ParseEquipListReinforcement", equipList, obj_info, hero_info.info_id, self.__data.lineup_index)
		if is_have_reddot then
			self:__AddRedDot(key)
			self.__grid_reddot_type_map[key] = 1
		else
			self:__HideRedDot(key)
			self.__grid_reddot_type_map[key] = nil
		end

		--检测能否强化或精炼，并显示相关特效与红点
		self:__JudgeIsNeedStrengthen(key, obj_info.uid)

		widget_info.icon_equip:SetVisible(true)
		widget_info.icon_equip:SetData({id = obj_info.info_id, obj_info = obj_info})
		widget_info.icon_equip:ShowEquipName(true, 0)
		widget_info.icon_equip:ShowEquipStrengthenLev(true)
		widget_info.icon_equip:ShowTreasureName(true, 0, nil, 0, -20)
		widget_info.icon_equip:ShowTreasureStrengthenLev(true)	
		widget_info.icon_equip:SetEquipStar( )
    end

	--新手指引特殊操作检测(阵容装备格子（装备类型）)
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LINEUP_EQUIP_GRID, widget_info.panel, self.__grid_type_map[key])                	

	--新手指引特殊操作检测(阵容圣器格子（圣器类型）)
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
	if operate_id == Macros.Game.LeadOperateSpecialID.LINEUP_TREASURE_GRID and obj_info and data_id == self.__grid_type_map[key] and self:__IsTreasureGrid(key) then
        --如果已经有圣器激活，则打断指引
        CallModuleFunc(ModuleType.LEAD, "Break")
    else
    	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LINEUP_TREASURE_GRID, widget_info.panel, self.__grid_type_map[key])  
    end	              			    
end

--是否圣器格子
function LogicGUI.TemplHeroLineupInfo:__IsTreasureGrid(key)
	if key == "attack_treasure_uid" or
		key == "defence_treasure_uid" then
		return true
	end
end

--更新圣器格子
function LogicGUI.TemplHeroLineupInfo:__UpdateTreasure(data)
	for key, _ in pairs(self.__grid_type_map) do
		if self:__IsTreasureGrid(key) then
			if self.__data.lineup_info[key] == data.uid then
				self:__UpdateGrid(data, key)
				break
			end
		end
	end
end

--新手指引特殊操作检测(阵容选择英雄)
function LogicGUI.TemplHeroLineupInfo:__CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LINEUP_SELECT_HERO, self.__panel_model)	
end
						
--添加红点
function LogicGUI.TemplHeroLineupInfo:__AddRedDot(key)
	--红点
	if not self["entry_red_dot_"..key] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["entry_red_dot_"..key] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["entry_red_dot_"..key]:SetAnchorPoint(1, 1)
		self.__grid_widget_info_map[key].panel:AddChild(self["entry_red_dot_"..key],2)
		PosTool.RightTop(self["entry_red_dot_"..key])
	end
	self["entry_red_dot_"..key]:SetVisible(true)
end

--隐藏红点
function LogicGUI.TemplHeroLineupInfo:__HideRedDot(key)
	if self["entry_red_dot_"..key] then
		self["entry_red_dot_"..key]:SetVisible(false)
	end	
end

--检测能否强化或精炼，并显示相关特效与红点
function LogicGUI.TemplHeroLineupInfo:__JudgeIsNeedStrengthen(key, uid)
	local strengthen, refine
	if self:__IsTreasureGrid(key) then
		strengthen = CallModuleFunc(ModuleType.TREASURE,"ToUidGetTreasureIsStrengthen",uid)	
		refine = CallModuleFunc(ModuleType.TREASURE,"ToUidGetTreasureIsRefine",uid)		
	else
		strengthen = CallModuleFunc(ModuleType.EQUIP,"ToUidGetEquipIsStrengthen",uid)
		refine = CallModuleFunc(ModuleType.EQUIP,"ToUidGetEquipIsRefine",uid)
	end

	--可强化或精炼	
	if strengthen == "true" or refine == "true" then
		if not self["Strengthen_"..key] then
			local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips")
			self["Strengthen_"..key] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
			self["Strengthen_"..key]:SetAnchorPoint(0, 0)
			self.__grid_widget_info_map[key].panel:AddChild(self["Strengthen_"..key],2)			
		end
		CPPActionManager.StopAllActions(self["Strengthen_"..key])
		self["Strengthen_"..key]:SetVisible(true)
		PosTool.RightBottom(self["Strengthen_"..key],-30,0)
		ActionManager.GetInstance():CycleBlinkMoveUpEffect(self["Strengthen_"..key])

		--红点
		if self.__grid_reddot_type_map[key] ~= 1 then
			self:__AddRedDot(key)
			self.__grid_reddot_type_map[key] = 2
		end
	else
		if self["Strengthen_"..key] then
			CPPActionManager.StopAllActions(self["Strengthen_"..key])
			self["Strengthen_"..key]:SetVisible(false)
		end	

		--红点
		if self.__grid_reddot_type_map[key] ~= 1 then
			self:__HideRedDot(key)
			self.__grid_reddot_type_map[key] = nil
		end
	end
end

function LogicGUI.TemplHeroLineupInfo:__HideStrengthen(key)
	if self["Strengthen_"..key] then
		CPPActionManager.StopAllActions(self["Strengthen_"..key])
		self["Strengthen_"..key]:SetVisible(false)
	end	
end


--判断可穿戴的装备列表里面是否有品质大于现在穿着的
function LogicGUI.TemplHeroLineupInfo:__ParseEquipList(equipList,quality)
	for _,equip_info in ipairs(equipList) do
		if equip_info.quality > quality then
			return true
		end
	end
	return false
end

--一键强化装备后的更新处理
function LogicGUI.TemplHeroLineupInfo:__OneKeyEnhanceUpdateEquip(data)
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
	end
	self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer = nil
		LogicGUI.TemplEquipOnekeyNature.New( data )			
	end),0.9)

	--更新装备格子
	for key, _ in pairs(self.__grid_type_map) do
		if not self:__IsTreasureGrid(key) then
			local obj_info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", self.__data.lineup_info[key])
			if obj_info then
				--移除强化/精炼特效
				if self["Strengthen_"..key] then
					CPPActionManager.StopAllActions(self["Strengthen_"..key])
					self["Strengthen_"..key]:SetVisible(false)
				end	

				--一键强化特效
				local widget_info = self.__grid_widget_info_map[key]				
				widget_info.icon_equip:SetEquipStrengthenLev(data.enhance_level_list[self.__grid_type_map[key]+1].new_level)
				if data.enhance_level_list[self.__grid_type_map[key]+1].new_level > data.enhance_level_list[self.__grid_type_map[key]+1].old_level then
					local effect_id,effect = self:__CreateEffect({id = 100044,loop = false},widget_info.icon_equip:GetNode(),5)
					PosTool.Center(effect)
				end

				--更新红点
				self:__JudgeIsNeedStrengthen(key, obj_info.uid)
			end
		end
	end

	--全局特效
	local effect_id,effect = self:__CreateEffect({res_name = "UI_qianghuabeiguang",loop = false},self:GetNode(),3)
	PosTool.Center(effect)											 
end

--角色等级更新装备格子
function LogicGUI.TemplHeroLineupInfo:__UpdateGridStatus()
	for key,widget_info in pairs(self.__grid_widget_info_map) do
		--功能未开		
		local is_open = GlobalModulesMgr:IsOpen(self:__IsTreasureGrid(key) and Macros.Game.SystemID.TREASURE_DRESS or Macros.Game.SystemID.EQUIP_DRESS)

		if is_open then
			if widget_info.lock_icon then
				widget_info.lock_icon:RemoveSelf(true)
				widget_info.lock_icon = nil
				widget_info.lab_open_lev:RemoveSelf(true)
				widget_info.lab_open_lev = nil				
				widget_info.btn_add:SetVisible(true)
				ActionManager.GetInstance():CycleBlinkEffect(widget_info.btn_add, 0.5)
				local equipList
				--可穿戴的装备
				if self:__IsTreasureGrid(key) then
					equipList = CallModuleFunc(ModuleType.TREASURE, "GetTreasureListByType",self.__grid_type_map[key])			
				else
					equipList = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip",self.__grid_type_map[key])
				end
				if #equipList > 0 then 
					self:__AddRedDot(key)
				else
					self:__HideRedDot(key)
				end
			end
		end
	end
end

--更新装备套装特效
function LogicGUI.TemplHeroLineupInfo:__UpdateEquipSuitEffect()	
	local _, lineup_equip_map,tbl_tr,lineup_special_equip_map = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",self.__data.lineup_index)
	local pos_list = CallModuleFunc(ModuleType.EQUIP,"GetLightUpSuitNumList",lineup_equip_map,lineup_special_equip_map)
	--当前装备状态
	local effect
	for uid_key,equip_type in pairs(self.__grid_type_map) do
		if not self:__IsTreasureGrid(uid_key) then
			--如果激活套装效果
			if self["__equip_suit_effectid"..equip_type] then
				self:__DelEffect(self["__equip_suit_effectid"..equip_type])
				self["__equip_suit_effectid"..equip_type] = nil
			end
			if pos_list[equip_type+1] >= 2 then	
				 --获取到装备信息
	            local equip_info = CPPGameLib.GetConfig("Equipment", lineup_special_equip_map[equip_type].info_id)			
				local widget_info = self.__grid_widget_info_map[uid_key]
				self["__equip_suit_effectid"..equip_type],effect = self:__CreateEffect({res_name = self.__equip_suit_effect_list[equip_info and equip_info.quality or 1]},widget_info.panel,7)
				PosTool.Center(effect)				
			end
		end
	end
end

--判断英雄是否达到觉醒开放等级
function LogicGUI.TemplHeroLineupInfo:JudgeIsOpenAwaken()
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_AWAKEN)	
	return is_open
end

--刷新英雄觉醒星级
function LogicGUI.TemplHeroLineupInfo:RefreshHeroAwakenStar(is_open,hero_info)
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


