
--[[
%% @module：英雄觉醒成功模板
%% @author: ldx
%% @created: 2017/5/9
--]]

Modules = Modules or {}

Modules.TemplHeroAwakenSucc = Modules.TemplHeroAwakenSucc or BaseClass(GUI.Template)

function Modules.TemplHeroAwakenSucc:__init(data)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)

		self:__OpenCallback()
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_AWAKEN, true)
	
	self:InitTemplate(GUI.TemplateRegistry.HERO_BREAK_SUCC)
end

function Modules.TemplHeroAwakenSucc:__delete()
	--战力变化 --进阶
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged", Macros.Game.COMBAT_CHANGE.HERO_AWAKEN, true)
	for i=1,3 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
		    self["__delay_timer"..i] = nil
		end
	end
end

function Modules.TemplHeroAwakenSucc:__OpenCallback()
	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__data)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	self:GetWidget("Image_upgrade"):IgnoreContentAdaptWithSize(true)

	--当前和下阶属性加成
	local next_plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id,hero_info.awaken_star,hero_info.awaken_level,nil,true)
	local before_star = hero_info.awaken_level == 0 and (hero_info.awaken_star - 1) or hero_info.awaken_star
	local before_level = hero_info.awaken_level == 0 and Macros.Game.HERO_AWAKE_LEVEL_MAX or (hero_info.awaken_level - 1)
	local plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id, before_star,before_level,nil,true)

	local prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	for i, prop_type in ipairs(prop_type_list) do
		Util:WidgetLabel(self:GetWidget("Text_CurPropTitle"..prop_type), CPPGameLib.GetString("common_property_"..prop_type), Macros.TypefaceSize.popup, Macros.Color.golden)
		Util:WidgetLabel(self:GetWidget("Text_NextPropTitle"..prop_type), CPPGameLib.GetString("common_property_"..prop_type), Macros.TypefaceSize.popup, Macros.Color.golden)

		Util:WidgetLabel(self:GetWidget("Text_CurProp"..prop_type), plus_prop_map[prop_type] or 0,Macros.TypefaceSize.popup)
		Util:WidgetLabel(self:GetWidget("Text_NextProp"..prop_type),next_plus_prop_map[prop_type] or 0, Macros.TypefaceSize.popup, Macros.Color.green)
	end

	local star_list = {before_star,hero_info.awaken_star}
	--英雄头像
	for i = 1,2 do
		local panel_icon = self:GetWidget("Panel_HeroIcon"..i)
		local icon = LogicGUI.IconLayer.New(false, false)
		icon:SetData({id = hero_info.info_id,obj_info = hero_info})		
		panel_icon:AddChild(icon:GetNode())
		icon:ShowAwakenStar(star_list[i])
		PosTool.Center(icon)
	end

	--设置箭头位置
	local pos_x = self:GetWidget("Text_NextProp"..Macros.Game.BattleProp.HP):GetPositionX() 
					+ self:GetWidget("Text_NextProp"..Macros.Game.BattleProp.HP):GetContentWidth() + 10
	for i=1,4 do
		self:GetWidget("Image_"..i):SetPosition(pos_x,self:GetWidget("Image_"..i):GetPositionY())
	end
	--天赋
	local unlock_talent_name = ""
	local unlock_talent_value = ""
	local special_id = config_hero.init_quality * 10 + config_hero.profession
	local awaken_config = CPPGameLib.GetConfig("HeroAwakening",special_id)
	local awaken_info = awaken_config[hero_info.awaken_star][hero_info.awaken_level]
	if awaken_info then
		unlock_talent_name = awaken_info.lightup_talent_name or ""
		unlock_talent_value = awaken_info.lightup_talent_des or ""
	end
	Util:WidgetLabel(self:GetWidget("Text_TalentInfo"),CPPGameLib.GetString("common_other2", unlock_talent_name, unlock_talent_value), Macros.TypefaceSize.popup, Macros.Color.green)

	--继续
	self.__text_continue = Util:WidgetLabel(self:GetWidget("Text_Continue"), CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.popup, Macros.Color.blinkcontent)
	self.__text_continue:SetVisible(false)
	self.__panel_lead_touch = self:GetWidget("Panel_LeadTouch")

	local function close_func()
		if not self.__is_anim_end then
			return
		end
		self:DeleteMe()
	end
	WidgetUnity.SetWidgetListener(self.__panel_lead_touch, nil, nil, function()
		close_func()
	end)
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		close_func()
	end)

	--播放特效
	self.__is_anim_end = false
	self.__panel_bg = self:GetWidget("Panel_BG")
	self.__panel_bg:SetVisible(false)
	self:__PlayEffect()
end

function Modules.TemplHeroAwakenSucc:__PlayEffect()
	self:GetWidget("Image_ribbon"):IgnoreContentAdaptWithSize(true)

	--将进阶成功换成升星成功
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_AWAKEN, "promote_star_succ",true) 
	self:GetWidget("Image_upgrade"):SetImage(sp)
	self:GetWidget("Image_upgrade"):IgnoreContentAdaptWithSize(true)
	
	--出场特效
	local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9,delay_callback = function()
		self.__panel_bg:SetVisible(true)

		--皇冠特效
		effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
		PosTool.Center(effect, 0, -15)	

		--激活天赋特效
		self.__delay_timer1 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			self.__delay_timer1 = nil
			--激活天赋特效
			self.__effect_id, effect = self:__CreateEffect({res_name = "UI_juexingtianfu_language_chs",loop = false}, self:GetWidget("Panel_effect"),2)
			PosTool.Center(effect,-30,0)			
		end), 0.4)

		self.__delay_timer2 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			self.__delay_timer2 = nil
			GlobalEffectMgr:PasueEffect(self.__effect_id)				
		end),2)

		--闪光特效
		self.__delay_timer3 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			self.__delay_timer3 = nil
			--闪光特效
			effect_id, effect = self:__CreateEffect({id = 100045,loop = false}, self:GetWidget("Panel_effect"),2)
			effect:SetAnchorPoint(0,0.5)
			PosTool.RightCenter(effect,160,-13)			
		end), 0.9)

		--播放CSB动画
		self:PlayActionTimeline("animation", false, function()
			self.__is_anim_end = true
			self.__text_continue:SetVisible(true)

			--播放箭头特效
			for i=1,4 do
				self:GetWidget("Image_"..i):SetVisible(true)
				ActionManager.GetInstance():CycleBlinkMoveUpEffect(self:GetWidget("Image_"..i))
			end

		    --新手指引特殊操作检测(英雄进阶继续)
		    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_BREAK_CONTINUE, self.__panel_lead_touch)         			
		end)

	end}, self:GetWidget("Panel_Shield"))
	PosTool.Center(effect, 1, 7)	
end