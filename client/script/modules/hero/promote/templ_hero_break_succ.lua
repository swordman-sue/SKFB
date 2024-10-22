
--[[
%% @module：英雄进阶成功模板
%% @author: swordman sue
%% @created: 2016-09-1
--]]

Modules = Modules or {}

Modules.TemplHeroBreakSucc = Modules.TemplHeroBreakSucc or BaseClass(GUI.Template)

function Modules.TemplHeroBreakSucc:__init(data,func)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)

		self:__OpenCallback(func)
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_BREAK, true)
	
	self:InitTemplate(GUI.TemplateRegistry.HERO_BREAK_SUCC)
end

function Modules.TemplHeroBreakSucc:__delete()
	--战力变化 --进阶
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged", Macros.Game.COMBAT_CHANGE.HERO_BREAK, true)
	for i=1,3 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
		    self["__delay_timer"..i] = nil
		end
	end
end

function Modules.TemplHeroBreakSucc:__OpenCallback(func)	
	local Image_ribbon = self:GetWidget( "Image_ribbon" )
	Image_ribbon:IgnoreContentAdaptWithSize( true )

	local Image_upgrade = self:GetWidget( "Image_upgrade" )
	Image_upgrade:IgnoreContentAdaptWithSize( true )

	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__data)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	--英雄头像
	for i = 1, 2 do
		local panel_icon = self:GetWidget("Panel_HeroIcon"..i)
		local icon = LogicGUI.IconLayer.New(false, false)
		icon:SetData({id = hero_info.info_id,num = ("+"..(hero_info.break_level - 2 + i))})		
		panel_icon:AddChild(icon:GetNode())
		icon:SetNum(("+"..(hero_info.break_level - 2 + i)),Macros.Color.green,Macros.TypefaceSize.normal)
		PosTool.Center(icon)
	end

	--当前和下阶属性加成
	local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
	local plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusBasicPropMap(hero_info.info_id, hero_info.break_level - 1)
	local next_plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusBasicPropMap(hero_info.info_id, hero_info.break_level)	
	self.__prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	for i, prop_type in ipairs(self.__prop_type_list) do
		local txt_curproptitle = self:GetWidget("Text_CurPropTitle"..prop_type)
		Util:WidgetLabel(txt_curproptitle, CPPGameLib.GetString("common_property_"..prop_type), Macros.TypefaceSize.popup, Macros.Color.golden)
		local txt_nextproptitle = self:GetWidget("Text_NextPropTitle"..prop_type)
		Util:WidgetLabel(txt_nextproptitle, CPPGameLib.GetString("common_property_"..prop_type), Macros.TypefaceSize.popup, Macros.Color.golden)

		local txt_curprop = self:GetWidget("Text_CurProp"..prop_type)
		Util:WidgetLabel(txt_curprop, basic_prop_map[prop_type] + (plus_prop_map[prop_type] or 0),Macros.TypefaceSize.popup)
		local txt_nextprop = self:GetWidget("Text_NextProp"..prop_type)
		Util:WidgetLabel(txt_nextprop, basic_prop_map[prop_type] + (next_plus_prop_map[prop_type] or 0), Macros.TypefaceSize.popup, Macros.Color.green)
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
	local break_info = ConfigAdapter.Hero.GetBreakInfo(hero_info.break_level, hero_info.info_id)
	if break_info then
		unlock_talent_name = break_info.break_name or ""
		unlock_talent_value = break_info.break_desc or ""
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
		if func then
			func()
		end
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

function Modules.TemplHeroBreakSucc:__PlayEffect()
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
			self.__effect_id, effect = self:__CreateEffect({id = 100047,loop = false}, self:GetWidget("Panel_effect"),2)
			PosTool.Center(effect)			
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

