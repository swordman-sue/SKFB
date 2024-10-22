
--[[
%% @module：精灵祝福技能
%% @author: ldx
%% @created: 2018-04-16
--]]

Modules = Modules or {}

Modules.TemplSpriteBlessSkill = Modules.TemplSpriteBlessSkill or BaseClass(GUI.Template)

function Modules.TemplSpriteBlessSkill:__init(spirit_id)
	self.__is_auto_load = true	
	self.__open_callback = function()
		self:__OpenCallback(spirit_id)
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS_SKILL)
end

function Modules.TemplSpriteBlessSkill:__delete()
	--模型
	if self.__spirit_bless_model then
		self.__spirit_bless_model:DeleteMe()
		self.__spirit_bless_model = nil
	end
end

function Modules.TemplSpriteBlessSkill:__OpenCallback(spirit_id)	
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",spirit_id)
	--精灵模型
	local panel_spiritmodule = self:GetWidget("Panel_SpiritModule")
	self.__spirit_bless_model = LogicGUI.Model.New({info_id = spirit_id,model_type = LogicGUI.ModelType.SPRITE,parent = panel_spiritmodule})
	self.__spirit_bless_model:SetScaleFactor(2.5)
	self.__spirit_bless_model:SetPosition(5,spirit_id == 50012 and -30 or 5)
	--点击屏幕提示
	Util:WidgetLabel(self:GetWidget("Text_TouchTips"), CPPGameLib.GetString("spirit_touch_tips"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--特性配置
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty",spirit_id)
	--当前星级
	local cur_star = spirit_info and spirit_info.star or 0
	--祝福技能
	local bless_skill_des = Util:RichText(config_speciality.star_list and config_speciality.star_list[cur_star].bless_skill_des or "", Macros.TypefaceSize.normal, 270, 0, Macros.Color.content_hex )
	bless_skill_des:SetAnchorPoint(0,1)
	self:GetWidget("Panel_BlessSkill"):AddChild(bless_skill_des)
	bless_skill_des:SetPosition(490,447)
	--点击关闭
	WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
		self:DeleteMe()
	end)
end