
--[[
%% @module：英雄进化成功模板
%% @author: ldx
%% @created: 2018-07-20
--]]

Modules = Modules or {}

Modules.TemplHeroEvolutionSucc = Modules.TemplHeroEvolutionSucc or BaseClass(GUI.Template)

function Modules.TemplHeroEvolutionSucc:__init(data,func)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)
		self:__OpenCallback(func)
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_EVOLUTION, true)
	
	self:InitTemplate(GUI.TemplateRegistry.HERO_EVOLUTION_SUCC)
end

function Modules.TemplHeroEvolutionSucc:__delete()
	--战力变化 --进阶
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged", Macros.Game.COMBAT_CHANGE.HERO_BREAK, true)
	for i=1,3 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
		    self["__delay_timer"..i] = nil
		end
	end
end

function Modules.TemplHeroEvolutionSucc:__OpenCallback(func)	
	local Image_ribbon = self:GetWidget( "Image_ribbon" )
	Image_ribbon:IgnoreContentAdaptWithSize( true )

	local Image_upgrade = self:GetWidget( "Image_upgrade" )
	Image_upgrade:IgnoreContentAdaptWithSize( true )

	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__data.hero_uid)
	local before_hero_info
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	local hero_quality = hero_info.quality_level
	local hero_quality_stage = hero_info.quality_stage
	if self.__data.is_evolution then
		before_hero_info = CPPGameLib.CopyTblShallow(hero_info) 
		before_hero_info.quality_level = hero_info.quality_level - 1
		hero_quality = hero_info.quality_level - 1
		--当前品阶信息
		local config_heroevolution = CPPGameLib.GetConfig("HeroEvolution",hero_info.quality_level - 1)
		if not config_heroevolution then
			return
		end
		hero_quality_stage = table.maxn(config_heroevolution.stage_list)		
	else
		before_hero_info = CPPGameLib.CopyTblShallow(hero_info) 
		hero_quality_stage = hero_info.quality_stage - 1
	end
	print("skfb","__OpenCallback__OpenCallback__OpenCallback ",hero_quality,hero_quality_stage)
	--英雄头像
	for i = 1, 2 do
		--英雄头像
		local panel_icon = self:GetWidget("Panel_HeroIcon"..i)
		local icon = LogicGUI.IconLayer.New(false, false)
		icon:SetData({id = hero_info.info_id, obj_info = i == 2 and hero_info or before_hero_info})		
		panel_icon:AddChild(icon:GetNode())
		icon:adaptParentContentSize()
		PosTool.Center(icon)	
	end

	--当前和下阶属性加成
	--获取英雄进化属性
	local cur_prop_map = ConfigAdapter.Hero.GetHeroEvolutionPropMap(hero_quality,hero_info.info_id,hero_info.level,hero_quality_stage) 
	local next_prop_map = ConfigAdapter.Hero.GetHeroEvolutionPropMap(hero_info.quality_level,hero_info.info_id,hero_info.level,hero_info.quality_stage) 
	local prop_type_list = {0,Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	--文字
	local cur_prop_txt = CPPGameLib.GetString("hero_evolution_prop0",CPPGameLib.GetString("hero_evolution_stage_quality"..hero_quality),hero_quality_stage,cur_prop_map.qualifications)
	local next_prop_txt = CPPGameLib.GetString("hero_evolution_prop0",CPPGameLib.GetString("hero_evolution_stage_quality"..hero_info.quality_level),hero_info.quality_stage,next_prop_map.qualifications)
	for i, prop_type in ipairs(prop_type_list) do
		Util:WidgetLabel(self:GetWidget("Text_CurPropTitle"..prop_type), prop_type ~= 0 and CPPGameLib.GetString("common_property_"..prop_type) or CPPGameLib.GetString("hero_evolution"), Macros.TypefaceSize.popup, Macros.Color.golden)
		Util:WidgetLabel(self:GetWidget("Text_NextPropTitle"..prop_type), prop_type ~= 0 and CPPGameLib.GetString("common_property_"..prop_type) or CPPGameLib.GetString("hero_evolution"), Macros.TypefaceSize.popup, Macros.Color.golden)

		Util:WidgetLabel(self:GetWidget("Text_CurProp"..prop_type),prop_type ~= 0 and cur_prop_map[prop_type] or cur_prop_txt,Macros.TypefaceSize.popup)
		Util:WidgetLabel(self:GetWidget("Text_NextProp"..prop_type),prop_type ~= 0 and next_prop_map[prop_type] or next_prop_txt, Macros.TypefaceSize.popup, Macros.Color.green)
	end

	--设置箭头位置
	local pos_x = self:GetWidget("Text_NextProp"..Macros.Game.BattleProp.HP):GetPositionX() 
					+ self:GetWidget("Text_NextProp"..Macros.Game.BattleProp.HP):GetContentWidth() + 10
	for i=1,4 do
		self:GetWidget("Image_"..i):SetPosition(pos_x,self:GetWidget("Image_"..i):GetPositionY())
	end

	--品质提升文字
	self:GetWidget("Image_Promote"):SetVisible(self.__data.is_evolution)
	self:GetWidget("Panel_effect"):SetVisible(self.__data.is_evolution)
	if self.__data.is_evolution then
		local panel_2 = self:GetWidget("Panel_2")
		-- local rt_txt = string.format(Macros.Quality_Stroke_RT[hero_info.quality_level],CPPGameLib.GetString("hero_evolution_stage_quality"..hero_info.quality_level)..CPPGameLib.GetString("hero_evolution_quality"))
		-- print("skfb","??????????????!!!!!!!!!!!!!!!!!!!!!!!!!!!!zayixkan ",rt_txt,CPPGameLib.GetString("hero_evolution_succ_tips",rt_txt))
		-- local rich_txt = Util:RichText(CPPGameLib.GetString("hero_evolution_succ_tips",rt_txt), Macros.TypefaceSize.largish, 0, 0, Macros.Color.btn_hex,Macros.Color.green_stroke_hex,2)
		-- rich_txt:SetAnchorPoint(0,0.5)
		-- panel_2:AddChild(rich_txt)
		-- PosTool.LeftCenter(rich_txt)
		--拼字
		local label1 = Util:Label(CPPGameLib.GetString("hero_evolution_succ_tips2"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.green_stroke, 2)
		label1:SetAnchorPoint(0,0.5)
		panel_2:AddChild(label1)
		PosTool.LeftCenter(label1)
		--拼字2
		local label2 = Util:Label(CPPGameLib.GetString("hero_evolution_stage_quality"..hero_info.quality_level)..CPPGameLib.GetString("hero_evolution_quality"), Macros.TypefaceSize.largish, Macros.Color.red, 0, 0, Macros.Color.red_stroke, 2)
		label2:SetAnchorPoint(0,0.5)
		panel_2:AddChild(label2)
		PosTool.RightTo(label1,label2)
	end

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

function Modules.TemplHeroEvolutionSucc:__PlayEffect()
	--出场特效
	local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9,delay_callback = function()
		self.__panel_bg:SetVisible(true)

		--皇冠特效
		effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
		PosTool.Center(effect, 0, -15)	

		-- --激活天赋特效
		-- self.__delay_timer1 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		-- 	self.__delay_timer1 = nil
		-- 	--激活天赋特效
		-- 	self.__effect_id, effect = self:__CreateEffect({id = 100047,loop = false}, self:GetWidget("Panel_effect"),2)
		-- 	PosTool.Center(effect)			
		-- end), 0.4)

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
		self:PlayActionTimeline("animation_break", false, function()
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

