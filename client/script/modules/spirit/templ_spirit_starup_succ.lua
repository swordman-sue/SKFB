
--[[
%% @module：英雄进阶成功模板
%% @author: swordman sue
%% @created: 2016-09-1
--]]

Modules = Modules or {}

Modules.TemplSpiritStarUpSucc = Modules.TemplSpiritStarUpSucc or BaseClass(GUI.Template)

function Modules.TemplSpiritStarUpSucc:__init(data)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)
		self:__OpenCallback()
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_STARUP_SUCC)
end

function Modules.TemplSpiritStarUpSucc:__delete()
	for i=1,3 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
		    self["__delay_timer"..i] = nil
		end
	end
end

function Modules.TemplSpiritStarUpSucc:__OpenCallback()	
	local Image_ribbon = self:GetWidget( "Image_ribbon" )
	Image_ribbon:IgnoreContentAdaptWithSize( true )

	local Image_starup = self:GetWidget( "Image_starup" )
	Image_starup:IgnoreContentAdaptWithSize( true )

	local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", self.__data.spirit_id)
	local config_spirit = CPPGameLib.GetConfig("Spirit", spirit_info.id)
	
	--英雄头像
	for i = 1, 2 do
		local panel_icon = self:GetWidget("Panel_HeroIcon"..i)
		local icon = LogicGUI.IconLayer.New(false, false)
		local data = {}
		data.starup_num = spirit_info.star + i - 2
		icon:SetData({id = self.__data.spirit_id,obj_info = data})		
		panel_icon:AddChild(icon:GetNode())
		icon:ShowSpiritAwakenStar()
		PosTool.Center(icon)
	end

	--资质
	Util:WidgetLabel(self:GetWidget("Text_CurPropTitle2"),CPPGameLib.GetString("spirit_qulaity_txt2"), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_CurProp2"),config_spirit.qualifications[spirit_info.star], Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextPropTitle2"),CPPGameLib.GetString("spirit_qulaity_txt2"), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextProp2"),config_spirit.qualifications[spirit_info.star+1], Macros.TypefaceSize.popup, Macros.Color.green)

	--属性加成
	Util:WidgetLabel(self:GetWidget("Text_CurPropTitle3"),CPPGameLib.GetString("spirit_prop_add"), Macros.TypefaceSize.popup, Macros.Color.btn)
	local config_qa = CPPGameLib.GetConfig("SpriteQualifications", config_spirit.qualifications[spirit_info.star])
	Util:WidgetLabel(self:GetWidget("Text_CurProp3"),CPPGameLib.GetString("spirit_cur_transform_probability_back",MathTool.TransProbCoeffPercentage(config_qa.add_rate)), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextPropTitle3"),CPPGameLib.GetString("spirit_prop_add"), Macros.TypefaceSize.popup, Macros.Color.btn)
	config_qa = CPPGameLib.GetConfig("SpriteQualifications", config_spirit.qualifications[spirit_info.star+1])
	Util:WidgetLabel(self:GetWidget("Text_NextProp3"),CPPGameLib.GetString("spirit_cur_transform_probability_back",MathTool.TransProbCoeffPercentage(config_qa.add_rate)), Macros.TypefaceSize.popup, Macros.Color.green)

	-- --天赋
	-- local config_sp = CPPGameLib.GetConfig("SpriteSpecialProperty", spirit_info.id)
	-- print("ldx","config_sp.star_list[spirit_info.star].speciality_des ",config_sp.star_list[spirit_info.star].speciality_des)
	-- local rb = Util:RichText(config_sp.star_list[spirit_info.star].speciality_des, Macros.TypefaceSize.popup, 0, 0, Macros.Color.btn_hex)
	-- self:GetWidget("Panel_SpecialityInfo"):AddChild(rb)
	-- PosTool.LeftCenter(rb,30,0)

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

function Modules.TemplSpiritStarUpSucc:__PlayEffect()
	--出场特效
	local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9,delay_callback = function()
		self.__panel_bg:SetVisible(true)

		--皇冠特效
		effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
		PosTool.Center(effect, 0, -15)	

		--闪光特效
		self.__delay_timer3 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			self.__delay_timer3 = nil
			--闪光特效
			effect_id, effect = self:__CreateEffect({id = 100045,loop = false}, self:GetWidget("Panel_effect"),2)
			effect:SetAnchorPoint(0,0.5)
			PosTool.RightCenter(effect,180,-20)			
		end), 0.9)

		--播放CSB动画
		self:PlayActionTimeline("Spirit_Starup", false, function()
			local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", self.__data.spirit_id)
			--天赋
			local config_sp = CPPGameLib.GetConfig("SpriteSpecialProperty", spirit_info.id)
			local rb = Util:RichText(config_sp.star_list[spirit_info.star].speciality_des, Macros.TypefaceSize.popup, 0, 0, Macros.Color.btn_hex)
			rb:SetAnchorPoint(0,0.5)
			self:GetWidget("Panel_SpecialityInfo"):AddChild(rb)
			PosTool.LeftCenter(rb,10,0)
			
			self.__is_anim_end = true
			self.__text_continue:SetVisible(true)     			
		end)

	end}, self:GetWidget("Panel_Shield"))
	PosTool.Center(effect, 1, 7)	
end

