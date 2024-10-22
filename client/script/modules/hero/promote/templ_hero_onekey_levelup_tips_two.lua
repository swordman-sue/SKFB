--[[
%% @module：一键升级2模板
%% @author: ldx
%% @created: 2017/8/4
--]]

Modules = Modules or {}

Modules.TemplHeroOnekeyLevelUpTipsTwo = Modules.TemplHeroOnekeyLevelUpTipsTwo or BaseClass(GUI.Template)

function Modules.TemplHeroOnekeyLevelUpTipsTwo:__init(add_exp,confirm_func,close_func,is_full)
	self.__is_auto_load = true	
	self.__open_callback = function()
		self:__OpenCallback(add_exp,confirm_func,close_func,is_full)
	end
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self:InitTemplate(GUI.TemplateRegistry.HERO_ONEKEY_LEVELUP_TIPS_TWO)
end

function Modules.TemplHeroOnekeyLevelUpTipsTwo:__OpenCallback(add_exp,confirm_func,close_func,is_full)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--提示1
	Util:WidgetLabel(self:GetWidget("Text_tip1"),is_full and CPPGameLib.GetString("hero_onekey_levelup_tips6") or CPPGameLib.GetString("hero_onekey_levelup_tips1"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--提示2
	Util:WidgetLabel(self:GetWidget("Text_tip2"),CPPGameLib.GetString("hero_onekey_levelup_tips2",add_exp), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--消耗
	Util:WidgetLabel(self:GetWidget("Text_cost"),CPPGameLib.GetString("hero_recruitCostText"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--消耗金币
	Util:WidgetLabel(self:GetWidget("Text_costMoney"),add_exp, Macros.TypefaceSize.largish, Macros.Color.content)
	--确定
	Util:WidgetButton(self:GetWidget("Button_sure"), CPPGameLib.GetString("common_confirm"),function ()
		local gold_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
		if gold_num < add_exp then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_onekey_levelup_tips4"))		
			return
		end
		confirm_func()
		self:DeleteMe()
	end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_red)
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"), CPPGameLib.GetString("common_cancel"),function ()
		close_func()
		self:DeleteMe()
	end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_yellow)
end