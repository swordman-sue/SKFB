Modules = Modules or {}

--怪物攻城鼓舞
Modules.TemplMonsterStormCastleEncourage = Modules.TemplMonsterStormCastleEncourage or BaseClass(GUI.Template)

function Modules.TemplMonsterStormCastleEncourage:__init(encourage_times)
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__is_auto_load = true
	self.__open_callback = function()

		local encourage_config = CPPGameLib.GetConfig("AttackCityEncourage",encourage_times + 1)
		--属性值
		local name,worth = Util:Nature(encourage_config.pro_type, encourage_config.pro_value,true)
		Util:WidgetLabel(self:GetWidget("Text_nextValueType"),worth, Macros.TypefaceSize.largish, Macros.Color.front_color)
		--积分
		local add_intergral_rate = MathTool.TransProbCoeffPercentage(encourage_config.score_add_rate)
		add_intergral_rate = string.format(" +%s%%",tostring(add_intergral_rate))
		Util:WidgetLabel(self:GetWidget("Text_nextIntergral"),add_intergral_rate, Macros.TypefaceSize.largish, Macros.Color.front_color)
		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"),"", function (  )
			self:DeleteMe()
		end)

		--标题
		Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("monster_stormcastle_encourage_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
		
		local panel_encourage = self:GetWidget("Panel_encourage")
		--花费
		local text = 
			{
				{ zi = CPPGameLib.GetString("monster_stormcastle_encourage_cost"), ys = Macros.Color.content , dx = Macros.TypefaceSize.largish,jg = -50},
				{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND),dx = 0.8,jg = -140},
				{ zi = encourage_config.cost_value, ys = Macros.Color.headline , dx = Macros.TypefaceSize.largish,jg = -60},
			}
		local Affirm = Util:LabCenter(text)
		Affirm:SetPosition(panel_encourage:GetContentWidth()/2 - 30,255)
		panel_encourage:AddChild(Affirm)

		--鼓舞次数
		Util:WidgetLabel(self:GetWidget("Text_curEncourageTimes"),CPPGameLib.GetString("monster_stormcastle_curencourage_times",encourage_times,#CPPGameLib.GetConfig("AttackCityEncourage", nil, nil , true )), Macros.TypefaceSize.largish, Macros.Color.content)
		--属性
		local type_name = Macros.Game.BattlePropName[encourage_config.pro_type] or ""
		local intergral_txt = CPPGameLib.GetString("monster_stormcastle_intergralpercenttxt")
		local type_value,intergral_value
		if encourage_times > 0 then
			local encourage_config = CPPGameLib.GetConfig("AttackCityEncourage",encourage_times)
			local name,worth = Util:Nature(encourage_config.pro_type, encourage_config.pro_value,true)
			type_value = " "..worth
			--积分
			local add_intergral_rate = MathTool.TransProbCoeffPercentage(encourage_config.score_add_rate)
			intergral_value = string.format(" +%s%%",tostring(add_intergral_rate))
		else
			type_value = " +0"
			intergral_value = " +0%"
		end
		Util:WidgetLabel(self:GetWidget("Text_curValueType"),type_name..type_value, Macros.TypefaceSize.largish, Macros.Color.content)
		Util:WidgetLabel(self:GetWidget("Text_curIntergral"),intergral_txt..intergral_value, Macros.TypefaceSize.largish, Macros.Color.content)
		
		--取消
		Util:WidgetButton(self:GetWidget("Button_canel"), CPPGameLib.GetString("common_cancel"),function ()
			self:DeleteMe()
		end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)

		--确定
		Util:WidgetButton(self:GetWidget("Button_sure"), CPPGameLib.GetString("common_confirm"),function ()
			local diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
			if diamond < encourage_config.cost_value then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_encourage_diamond_no_enough"))
				return
			end
			CallModuleFunc(ModuleType.MONSTER_STORMCASTLE,"AttackCityEncourageReq")
			self:DeleteMe()
		end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_red)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_ENCOURAGE)	
end