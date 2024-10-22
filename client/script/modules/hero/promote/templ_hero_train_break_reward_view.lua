LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroTrainBreakRewardView = LogicGUI.TemplHeroTrainBreakRewardView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroTrainBreakRewardView:__init(pro_str,lv_str,func)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(pro_str,lv_str,func)
	end
	self:InitTemplate(GUI.TemplateRegistry.HERO_TRAIN_BREAK_REWARD_VIEW)
end

function LogicGUI.TemplHeroTrainBreakRewardView:InitUI(pro_str,lv_str,func)
	self:GetWidget("Image_18"):IgnoreContentAdaptWithSize(true)
	--奖励属性
	Util:WidgetLabel(self:GetWidget("Text_rewardProperty"),CPPGameLib.GetString("hero_train_reward_property"), Macros.TypefaceSize.headline, Macros.Color.Special)
	--增加属性
	Util:WidgetLabel(self:GetWidget("Text_addRewardProperty"),pro_str, Macros.TypefaceSize.headline, Macros.Color.green)
	--等级提升
	Util:WidgetLabel(self:GetWidget("Text_levelUp"),CPPGameLib.GetString("hero_train_level_up"), Macros.TypefaceSize.headline, Macros.Color.Special)
	--增加属性等级
	Util:WidgetLabel(self:GetWidget("Text_levelNum"),lv_str, Macros.TypefaceSize.headline, Macros.Color.green)
	--确定按钮
	Util:WidgetButton(self:GetWidget("Button_sure"), CPPGameLib.GetString("common_confirm"), function ()
		if func then
			func()
		end
		self:DeleteMe()
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--播放动画
	self:PlayActionTimeline("animation0", false,nil)
end