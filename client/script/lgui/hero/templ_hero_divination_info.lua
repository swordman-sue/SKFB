
--[[
%% @module：阵位阵位信息模板
%% @author: ldx
%% @created: 2017-11-23
注：需手动释放(DeleteMe)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroDivinationInfo = LogicGUI.TemplHeroDivinationInfo or BaseClass(GUI.Template)

function LogicGUI.TemplHeroDivinationInfo:__init(parent_node)
	self:InitTemplate(GUI.TemplateRegistry.HERO_DIVINATION_LINEUP_INFO, parent_node, true)

	self:__InitUI()

	--阵位更新
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_LINEUP_UPDATE, function(_,data)
		local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo", self.__data.lineup_index)
		if lineup_info then
			self.__data.lineup_info = lineup_info
			self:SetData(self.__data)
		end
	end)

	--角色等级更新
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function()
		--原石
	    local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.GEM )
	    self.__btn_gem:SetVisible( openType )
	end)
	
	--更新卡牌通知
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_UPDATE_DIVINATION_NOTICE, function()
		self:__UpdateDivinationStatus()	
	end)
end

function LogicGUI.TemplHeroDivinationInfo:__delete()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
end

--[[
@ data：
	lineup_index
	lineup_info
--]]
function LogicGUI.TemplHeroDivinationInfo:SetData(data)
	if not data then
		LogTool.LogError("[LogicGUI.TemplHeroDivinationInfo:SetData] 参数data无效")
		return
	end
	self.__data = data
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
		self.__img_camp:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_"..config_hero.camp, true))
		self.__img_hero_pro:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..config_hero.profession, true))
		self.__text_herolevel:SetText(CPPGameLib.GetString("hero_level",hero_info.level))
		self.__text_herointelligence:SetText(CPPGameLib.GetString("hero_intelligence",config_hero.Qualifications))
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

		--英雄觉醒
		--判断是否开启觉醒
		local IsOpenAwaken = self:__JudgeIsOpenAwaken()
		--觉醒星级
		self:__RefreshHeroAwakenStar(IsOpenAwaken,hero_info)

	end

	--卡牌格子
	local obj_info
	for pos=1,8 do
		--卡牌信息				
		obj_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination",lineup_info.divination_list[pos].card_id)
		self:__UpdateDivination(obj_info, pos)	
	end
end

function LogicGUI.TemplHeroDivinationInfo:__RegistAllEvents()
	--红点更新事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id, sub_system_id)
		--更新卡牌穿戴红点
		if sub_system_id == Macros.Game.SystemID.DIVINATION_DRESS then
			for pos, _ in pairs(self.__divination_widget_info_map) do
				self:__UpdateRedDot(pos)
			end
		end
	end)
end

function LogicGUI.TemplHeroDivinationInfo:__InitUI()
	self.__panel_model = self:GetWidget("Panel_HeroModel")

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
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local system_info = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.GEM )
    self.__btn_gem:SetVisible(system_infol)
    --原石入口文字
    Util:WidgetLabel(self:GetWidget("Text_gem"),CPPGameLib.GetString("hero_lineup_gem_txt"), Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    
	self.__divination_widget_info_map = {}
	for pos=1,8  do   
		local widget_info = {}
		widget_info.panel = self:GetWidget("Panel_DivinationPos"..pos)
		widget_info.txt_name = WidgetUnity.GetWidgetByName(widget_info.panel, "Text_Pos"..pos)		
		Util:WidgetLabel(widget_info.txt_name,"", nil, nil, nil, nil, Macros.Color.common_stroke)

		widget_info.btn_add = WidgetUnity.GetWidgetByName(widget_info.panel, "Button_add"..pos)
		widget_info.btn_add:SetTouchEnabled(false)
		table.insert(self.__divination_widget_info_map,widget_info)
	end
end

function LogicGUI.TemplHeroDivinationInfo:__UpdateDivination(obj_info, pos)
	local widget_info = self.__divination_widget_info_map[pos]
	widget_info.txt_name:SetVisible(false)
	widget_info.btn_add:SetVisible(false)
	CPPActionManager.StopAllActions(widget_info.btn_add)
	WidgetUnity.SetWidgetListener(widget_info.panel)
	if widget_info.icon_divination then
		widget_info.icon_divination:SetVisible(false)
	end

	local lineup_info = self.__data.lineup_info

	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)
	--可穿戴的卡牌队列
	local divination_list = CallModuleFunc(ModuleType.DIVINATION, "GetAllDivinationWithoutWear")
	
    if not obj_info then
    	--显示空格子
		widget_info.panel:SetBackGroundImageOpacity(255)

		widget_info.txt_name:SetVisible(true)

		--功能未开
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		local hole_info = CPPGameLib.GetConfig("DivinationHole",pos)
		if hole_info and role_level < hole_info.role_level then
			widget_info.txt_name:SetVisible(false)
			if not widget_info.lock_icon then
				--锁
				widget_info.lock_icon = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
				widget_info.panel:AddChild(widget_info.lock_icon)	
				-- lock_icon:SetTouchEnabled(true)
				-- WidgetUnity.SetWidgetListener(lock_icon, nil, nil,function ()
				-- 	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lock_text_tips_1",hole_info.level))
				-- end)
				PosTool.Center(widget_info.lock_icon)
				--开启等级
				widget_info.lab_open_lev = Util:Label(nil, nil,Macros.Color.btn,50,0,Macros.Color.button_yellow)
			    widget_info.lab_open_lev:SetAnchorPoint(0.5, 0.5)
			    widget_info.lab_open_lev:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
			    widget_info.panel:AddChild(widget_info.lab_open_lev)
			    PosTool.Center(widget_info.lab_open_lev)			
				widget_info.lab_open_lev:SetString(string.format(CPPGameLib.GetString("common_open_lev"), hole_info.role_level))
			end
		else
			widget_info.btn_add:SetVisible(true)
			ActionManager.GetInstance():CycleBlinkEffect(widget_info.btn_add, 0.5)
		end		
		--隐藏
		self:__HideStrengthen(pos)

		--穿戴按钮		
		WidgetUnity.SetWidgetListener(widget_info.panel, nil, nil, function()					
			--功能未开
			local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			local hole_info = CPPGameLib.GetConfig("DivinationHole",pos)
			if hole_info and role_level < hole_info.role_level then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lock_text_tips_1",hole_info.role_level))
				return
			end
			local lineup_info = self.__data.lineup_info
			local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)
			--可穿戴的卡牌队列
			local divination_list = CallModuleFunc(ModuleType.DIVINATION, "GetAllDivinationWithoutWear")		
			local is_have_another_type = CallModuleFunc(ModuleType.HERO, "JudgeIsHaveAnotherDivinationType",divination_list,lineup_info.divination_list)		
			if is_have_another_type then 
				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DIVINATION_DRESS, {{hero_id = hero_info.info_id ,  index = self.__data.lineup_index, item_type = pos}})
			else
				--卡牌通用获取途径	
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetCommonDivinationAchievingId())
			end						
		end)						
    else
		widget_info.panel:SetBackGroundImageOpacity(0)

		--显示装备、圣器
		if not widget_info.icon_divination then
			widget_info.icon_divination = LogicGUI.IconLayer.New(false)
			widget_info.icon_divination:SetTouchCallback(function()
				local lineup_info = self.__data.lineup_info
				local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)				
				--打开装备详情界面
				local data = 
				{
					lineup_index = self.__data.lineup_index,
					item_type = pos,
					-- equip_uid = lineup_info[key],
					btn = true,
					hero_id = hero_info.info_id
				}
				
				--比墙
				local t1 = CallModuleFunc(ModuleType.DIVINATION , "ToUidGetStrongCard" , lineup_info.divination_list[pos].card_id )
				--强化
				local t2 = CallModuleFunc(ModuleType.DIVINATION , "ToUidGetCardIsStrengthen" , lineup_info.divination_list[pos].card_id )
				--突破
				local t3 = CallModuleFunc(ModuleType.DIVINATION , "ToUidGetCardIsBreak" , lineup_info.divination_list[pos].card_id )

				if #t1 ~= 0 then
					CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_INTENSIFY, 1 , lineup_info.divination_list[pos].card_id )
				elseif t2 == "true" then
					CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_INTENSIFY, 2 , lineup_info.divination_list[pos].card_id )
				elseif t3 == "true" then
					CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_INTENSIFY, 3 , lineup_info.divination_list[pos].card_id )
				else
					CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_INTENSIFY, 1 , lineup_info.divination_list[pos].card_id )
				end
			end)
			widget_info.panel:AddChild(widget_info.icon_divination:GetNode())
			PosTool.Center(widget_info.icon_divination)								
		end		
		--强化效果
		self:__JudgeIsNeedStrengthen(pos)

		widget_info.icon_divination:SetVisible(true)
		widget_info.icon_divination:SetData({id = obj_info.info_id, obj_info = obj_info})
		--TODO
		widget_info.icon_divination:ShowDivinationName(true, 0)
		widget_info.icon_divination:ShowDivinationStrengthenLev(true)	
    end 

    --更新红点
    self:__UpdateRedDot(pos)
end

function LogicGUI.TemplHeroDivinationInfo:__UpdateEquip(data)
	for key, _ in pairs(self.__grid_type_map) do
		if self.__data.lineup_info[key] == data.uid then
			self:__UpdateDivination(data, key)
		end
	end
end
					
--添加红点
function LogicGUI.TemplHeroDivinationInfo:__AddRedDot(pos)
	--红点
	if not self["__entry_red_dot_"..pos] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["__entry_red_dot_"..pos] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["__entry_red_dot_"..pos]:SetAnchorPoint(1, 1)
		self.__divination_widget_info_map[pos].panel:AddChild(self["__entry_red_dot_"..pos],2)
		PosTool.RightTop(self["__entry_red_dot_"..pos])
	end
	self["__entry_red_dot_"..pos]:SetVisible(true)
end

--隐藏红点
function LogicGUI.TemplHeroDivinationInfo:__HideRedDot(pos)
	if self["__entry_red_dot_"..pos] then
		self["__entry_red_dot_"..pos]:SetVisible(false)
	end	
end

--更新红点
function LogicGUI.TemplHeroDivinationInfo:__UpdateRedDot(pos)
	local divination_reddot_table = CallModuleFunc(ModuleType.HERO,"GetHeroDivinationRedDotNumByLineUpIndex",self.__data.lineup_index) 
	local red_dot_num = divination_reddot_table[pos] or 0
	if red_dot_num > 0 then
		self:__AddRedDot(pos)
	else
		self:__HideRedDot(pos)
	end            			    	
end

--强化效果
function LogicGUI.TemplHeroDivinationInfo:__JudgeIsNeedStrengthen(pos)
	local is_strengthen = CallModuleFunc(ModuleType.HERO,"GetDivinationIsStrengthenByLineupIndexAndCardPos",self.__data.lineup_index,pos)
	if is_strengthen then
		if not self["__strengthen_"..pos] then
			local icon_upgrade_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips")
			self["__strengthen_"..pos] = SpriteEx.CreateWithImageFile(icon_upgrade_image_data)
			self["__strengthen_"..pos]:SetAnchorPoint(0, 0)
			self.__divination_widget_info_map[pos].panel:AddChild(self["__strengthen_"..pos],2)			
		end
		CPPActionManager.StopAllActions(self["__strengthen_"..pos])
		self["__strengthen_"..pos]:SetVisible(true)
		PosTool.RightBottom(self["__strengthen_"..pos],-30,0)
		ActionManager.GetInstance():CycleBlinkMoveUpEffect(self["__strengthen_"..pos])
	else
		if self["__strengthen_"..pos] then
			CPPActionManager.StopAllActions(self["__strengthen_"..pos])
			self["__strengthen_"..pos]:SetVisible(false)
		end	
	end
end

function LogicGUI.TemplHeroDivinationInfo:__HideStrengthen(pos)
	if self["__strengthen_"..pos] then
		CPPActionManager.StopAllActions(self["__strengthen_"..pos])
		self["__strengthen_"..pos]:SetVisible(false)
	end	
end

function LogicGUI.TemplHeroDivinationInfo:__WindsWords()
	
end

--角色等级更新装备格子
function LogicGUI.TemplHeroDivinationInfo:__UpdateDivinationStatus()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)	
	for pos=1,8 do		
		--功能未开
		local widget_info = self.__divination_widget_info_map[pos]
		local hole_info = CPPGameLib.GetConfig("DivinationHole",pos)		
		if hole_info and role_level >= hole_info.role_level and widget_info.lock_icon then
			widget_info.lock_icon:RemoveSelf(true)
			widget_info.lock_icon = nil
			widget_info.lab_open_lev:RemoveSelf(true)
			widget_info.lab_open_lev = nil				
			widget_info.btn_add:SetVisible(true)
			ActionManager.GetInstance():CycleBlinkEffect(widget_info.btn_add, 0.5)

			--更新红点
			self:__UpdateRedDot(pos)
		end
	end
end

--判断英雄是否达到觉醒开放等级
function LogicGUI.TemplHeroDivinationInfo:__JudgeIsOpenAwaken()
	local config_sys = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.HERO_AWAKEN )
	return config_sys
end

--刷新英雄觉醒星级
function LogicGUI.TemplHeroDivinationInfo:__RefreshHeroAwakenStar(is_open,hero_info)
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


