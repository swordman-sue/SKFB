
--[[
%% @module：英雄进阶成功模板
%% @author: swordman sue
%% @created: 2016-09-1
--]]

Modules = Modules or {}

Modules.TemplSpiritSublimationSucc = Modules.TemplSpiritSublimationSucc or BaseClass(GUI.Template)

function Modules.TemplSpiritSublimationSucc:__init(data)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)
		self:__OpenCallback()
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_SUBLIMATION_SUCC)
end

function Modules.TemplSpiritSublimationSucc:__delete()
	--模型
	if self.__spirit_new_model then
		self.__spirit_new_model:DeleteMe()
		self.__spirit_new_model = nil
	end
end

function Modules.TemplSpiritSublimationSucc:__OpenCallback()	
	local Image_ribbon = self:GetWidget( "Image_ribbon" )
	Image_ribbon:IgnoreContentAdaptWithSize( true )

	local Image_sublimation = self:GetWidget( "Image_sublimation" )
	Image_sublimation:IgnoreContentAdaptWithSize( true )

	local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", self.__data.spirit_id)
	local config_spirit = CPPGameLib.GetConfig("Spirit", spirit_info.id)
	local config_train = CPPGameLib.GetConfig("SpriteTrain", spirit_info.id)	
	--隐藏新形象
	self:GetWidget("Panel_1_2"):SetVisible(false)

	--升华阶段
	Util:WidgetLabel(self:GetWidget("Text_CurSublimationPropTitle"),CPPGameLib.GetString("spirit_sublimate_txt"), Macros.TypefaceSize.popup, Macros.Color.Special)
	Util:WidgetLabel(self:GetWidget("Text_CurSublimationProp"),CPPGameLib.GetString("spirit_sublimate_level2",spirit_info.train_stage - 1), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextSublimationProp"),CPPGameLib.GetString("spirit_sublimate_level2",spirit_info.train_stage), Macros.TypefaceSize.popup, Macros.Color.green)

	--属性转换
	Util:WidgetLabel(self:GetWidget("Text_CurTranslatePropTitle"),CPPGameLib.GetString("spirit_property_transform_txt"), Macros.TypefaceSize.popup, Macros.Color.Special)
	Util:WidgetLabel(self:GetWidget("Text_CurTranslateProp"),CPPGameLib.GetString("spirit_cur_transform_probability_back",MathTool.TransProbCoeffPercentage(config_train.stage_list[spirit_info.train_stage - 1].pro_conversion_rate)), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextTranslateProp"),CPPGameLib.GetString("spirit_cur_transform_probability_back",MathTool.TransProbCoeffPercentage(config_train.stage_list[spirit_info.train_stage].pro_conversion_rate)), Macros.TypefaceSize.popup, Macros.Color.green)

	--天赋技能
	Util:WidgetLabel(self:GetWidget("Text_CurTalentPropTitle"),CPPGameLib.GetString("spirit_sublimation_talent_txt"), Macros.TypefaceSize.popup, Macros.Color.Special)
	Util:WidgetLabel(self:GetWidget("Text_CurTalentProp"),CPPGameLib.GetString("common_lev6",config_train.stage_list[spirit_info.train_stage - 1].talent_level), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextTalentProp"),CPPGameLib.GetString("common_lev6",config_train.stage_list[spirit_info.train_stage].talent_level), Macros.TypefaceSize.popup, Macros.Color.green)

	--被动技能
	Util:WidgetLabel(self:GetWidget("Text_CurAutoSkillPropTitle"),CPPGameLib.GetString("spirit_sublimation_auto_txt"), Macros.TypefaceSize.popup, Macros.Color.Special)
	Util:WidgetLabel(self:GetWidget("Text_CurAutoSkillProp"),CPPGameLib.GetString("common_lev6",config_train.stage_list[spirit_info.train_stage - 1].skill_level), Macros.TypefaceSize.popup, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_NextAutoSkillProp"),CPPGameLib.GetString("common_lev6",config_train.stage_list[spirit_info.train_stage].skill_level), Macros.TypefaceSize.popup, Macros.Color.green)

	--精灵模型
	local panel_spiritmodule = self:GetWidget("Panel_SpiritModule")
	self.__spirit_new_model = LogicGUI.Model.New({info_id = self.__data.spirit_id,model_type = LogicGUI.ModelType.SPRITE,parent = panel_spiritmodule})
	self.__spirit_new_model:SetScaleFactor(2)
	self.__spirit_new_model:SetPosition(10,self.__data.spirit_id == 50012 and 20 or 50)
	--新形象
	self:GetWidget("Image_NewFigure"):SetVisible(false)
	
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

function Modules.TemplSpiritSublimationSucc:__PlayEffect()
	--出场特效
	local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9,delay_callback = function()
		self.__panel_bg:SetVisible(true)

		--皇冠特效
		effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
		PosTool.Center(effect, 0, -15)	

		--播放CSB动画
		self:PlayActionTimeline("Spirit_Sublimation", false, function()
			self.__is_anim_end = true
			self.__text_continue:SetVisible(true)         			
		end)

	end}, self:GetWidget("Panel_Shield"))
	PosTool.Center(effect, 1, 7)	
end

