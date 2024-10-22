LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresRelieveStateTipView = LogicGUI.TemplPlunderedTreasuresRelieveStateTipView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresRelieveStateTipView:__init(type)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI(type)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_RELIEVESTATETIPVIEW)
end

function LogicGUI.TemplPlunderedTreasuresRelieveStateTipView:initUI(type)
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_tipsTitle"), Macros.TypefaceSize.popup, Macros.Color.btn)
	--内容	
	Util:WidgetLabel(self:GetWidget("Text_tip"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_13"), Macros.TypefaceSize.normal, Macros.Color.content,350,50)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--取消按钮
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	--确定按钮
	Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("common_confirm"),function ()
		self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_OPENWAR,type)
		self:DeleteMe()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red)
end

